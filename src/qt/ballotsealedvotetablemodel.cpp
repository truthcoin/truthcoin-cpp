// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "guiutil.h"
#include "ballotsealedvotetablemodel.h"
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
        Qt::AlignRight|Qt::AlignVCenter, /* Hash */
    };

// Private implementation
class BallotSealedVoteTablePriv
{
public:
    BallotSealedVoteTablePriv(CWallet *wallet, BallotSealedVoteTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    BallotSealedVoteTableModel *parent;

    /* Local cache of SealedVotes */
    QList<const marketSealedVote *> cached;

    int size()
    {
        return cached.size();
    }

    const marketSealedVote *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketSealedVote *vote, int unit)
    {
        return QString();
    }
};

BallotSealedVoteTableModel::BallotSealedVoteTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new BallotSealedVoteTablePriv(wallet, this))
{
    columns
        << tr("Block Number")
        << tr("VoteID")
        ;
}

BallotSealedVoteTableModel::~BallotSealedVoteTableModel()
{
    delete priv;
}

int BallotSealedVoteTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int BallotSealedVoteTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant BallotSealedVoteTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketSealedVote *vote = (const marketSealedVote *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Height:
            return formatHeight(vote);
        case VoteID:
            return formatVoteID(vote);
        default:
            ;
        }
        break;
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant BallotSealedVoteTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case VoteID:
                return tr("VoteID");
            }
        }
    }
    return QVariant();
}

const marketSealedVote *BallotSealedVoteTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex BallotSealedVoteTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketSealedVote *vote = priv->index(row);
    if (vote)
        return createIndex(row, column, (void *)vote);
    return QModelIndex();
}

Qt::ItemFlags BallotSealedVoteTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BallotSealedVoteTableModel::onBranchChange(const marketBranch *branch)
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

    /* insert into cache */
#if 0
    vector<marketSealedVote *> vec = pmarkettree->GetSealedVotes(branch->GetHash());
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

QString formatHeight(const marketSealedVote *vote)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", vote->nHeight);
    return QString(tmp);
}

QString formatVoteID(const marketSealedVote *vote)
{
    return QString::fromStdString(vote->voteid.ToString());
}

QString formatHash(const marketSealedVote *vote)
{
    return QString::fromStdString(vote->GetHash().ToString());
}

