// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "ballotvotetablemodel.h"
#include "guiutil.h"
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
        Qt::AlignRight|Qt::AlignVCenter, /* Address */
    };


// Private implementation
class BallotVoteTablePriv
{
public:
    BallotVoteTablePriv(CWallet *wallet, BallotVoteTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    BallotVoteTableModel *parent;

    /* Local cache of Votes */
    QList<const marketVote *> cached;

    int size()
    {
        return cached.size();
    }

    const marketVote *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketVote *vote, int unit)
    {
        return QString();
    }
};

BallotVoteTableModel::BallotVoteTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new BallotVoteTablePriv(wallet, this))
{
    columns
        << tr("Block Number")
        << tr("Hash")
        ;
}

BallotVoteTableModel::~BallotVoteTableModel()
{
    delete priv;
}

int BallotVoteTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int BallotVoteTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant BallotVoteTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketVote *vote = (const marketVote *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Height:
            return formatHeight(vote);
        case Address:
            return formatAddress(vote);
        case Hash:
            return formatHash(vote);
        default:
            ;
        }
        break;
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant BallotVoteTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return tr("Block Number");
            case Address:
                return tr("Address");
            case Hash:
                return tr("Hash");
            }
        }
    }
    return QVariant();
}

const marketVote *BallotVoteTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex BallotVoteTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketVote *vote = priv->index(row);
    if (vote)
        return createIndex(row, column, (void *)vote);
    return QModelIndex();
}

Qt::ItemFlags BallotVoteTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BallotVoteTableModel::onBranchChange(const marketBranch *branch)
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

#if 0
    /* insert into cache */
    vector<marketVote *> vec = pmarkettree->GetVotes(branch->GetHash(), blocknum);
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++) {
            InsertMarketObjectHeight(vec[i]);
            priv->cached.append(vec[i]);
        }
        endInsertRows();
    }
#endif
}

QString formatHeight(const marketVote *vote)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", vote->nHeight);
    return QString(tmp);
}

QString formatAddress(const marketVote *vote)
{
    CTruthcoinAddress addr;
    if (addr.Set(vote->keyID))
        return QString::fromStdString(addr.ToString());
    return QString("Address");
}

QString formatHash(const marketVote *vote)
{
    return QString::fromStdString(vote->GetHash().ToString());
}

