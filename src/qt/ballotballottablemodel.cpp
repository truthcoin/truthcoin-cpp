// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include <vector>
#include "guiutil.h"
#include "ballotballottablemodel.h"
#include "main.h"
#include "primitives/market.h"
#include "sync.h"
#include "txdb.h"
#include "uint256.h"
#include "util.h"
#include "wallet.h"
#include "walletmodel.h"


extern CMarketTreeDB *pmarkettree;

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignRight|Qt::AlignVCenter, /* Height */
        Qt::AlignRight|Qt::AlignVCenter, /* nDecisions */
    };

// Private implementation
class BallotBallotTablePriv
{
public:
    BallotBallotTablePriv(CWallet *wallet, BallotBallotTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    BallotBallotTableModel *parent;

    /* Local cache of Pairs */
    QList<const marketPair *> cached;

    int size()
    {
        return cached.size();
    }

    const marketPair *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketPair *pair, int unit)
    {
        return QString();
    }
};

BallotBallotTableModel::BallotBallotTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new BallotBallotTablePriv(wallet, this))
{
    columns
        << tr("Height")
        << tr("Number Decisions")
        ;
}

BallotBallotTableModel::~BallotBallotTableModel()
{
    delete priv;
}

int BallotBallotTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int BallotBallotTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant BallotBallotTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketPair *pair = (const marketPair *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Height:
            return formatHeight(pair);
        case nDecisions:
            return formatNDecisions(pair);
        default:
            ;
        }
        break;
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant BallotBallotTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
            return columns[section];
        else
        if (role == Qt::TextAlignmentRole)
            return column_alignments[section];
        else
        if (role == Qt::ToolTipRole)
        {
            switch(section)
            {
            case Height:
                return tr("Height");
            case nDecisions:
                return tr("Number of Decisions");
            }
        }
    }
    return QVariant();
}

const marketPair *BallotBallotTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex BallotBallotTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketPair *pair = priv->index(row);
    if (pair)
        return createIndex(row, column, (void *)pair);
    return QModelIndex();
}

Qt::ItemFlags BallotBallotTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BallotBallotTableModel::onBranchChange(
    const marketBranch *branch)
{
    if (!priv)
        return;

    /* erase cache */
    if (priv->cached.size()) {
        beginRemoveRows(QModelIndex(), 0, priv->cached.size()-1);
        for(ssize_t i=0; i < priv->cached.size(); i++)
            delete priv->cached[i];
        priv->cached.clear();
        endRemoveRows();
    }

    if (!branch)
	return;

    vector<marketDecision *> vec = pmarkettree->GetDecisions(branch->GetHash());

    /* insert into cache */
    if (vec.size()) {
        /* calc max */
        int maxEventOverBy = -1;
        for(size_t i=0; i < vec.size(); i++)
            if ((int)vec[i]->eventOverBy > maxEventOverBy)
                maxEventOverBy = vec[i]->eventOverBy;
        if (maxEventOverBy > 100000) /* temporary safeguard */
            maxEventOverBy = 100000;

        if (maxEventOverBy != -1) {
            /* init pairs[] */
            uint32_t nrows = maxEventOverBy / branch->tau;
            marketPair *pairs = new marketPair[nrows]; 
            for(uint32_t i=0; i < nrows; i++) {
                pairs[i].Height = branch->tau * (i+1);
                pairs[i].nDecisions = 0;
            }
    
            /* populate pairs[] */
            for(size_t i=0; i < vec.size(); i++) {
                if (vec[i]->eventOverBy < branch->tau)
                   continue;
                uint32_t bucket_num = vec[i]->eventOverBy / branch->tau - 1;
                if (bucket_num < nrows)
                    pairs[bucket_num].nDecisions++;
            }
    
            /* insert */
            beginInsertRows(QModelIndex(), 0, nrows-1);
            for(size_t i=0; i < nrows; i++)
                priv->cached.append(&pairs[i]);
            endInsertRows();
        } 

        /* clean up */
        for(uint32_t i=0; i < vec.size(); i++)
            delete vec[i];
    }
}

QString formatHeight(const marketPair *pair)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", pair->Height);
    return QString(tmp);
}

QString formatNDecisions(const marketPair *pair)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", pair->nDecisions);
    return QString(tmp);
}

