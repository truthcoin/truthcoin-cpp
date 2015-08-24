// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "guiutil.h"
#include "ballotoutcometablemodel.h"
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
class BallotOutcomeTablePriv
{
public:
    BallotOutcomeTablePriv(CWallet *wallet, BallotOutcomeTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    BallotOutcomeTableModel *parent;

    /* Local cache of Outcomes */
    QList<const marketOutcome *> cached;

    int size()
    {
        return cached.size();
    }

    const marketOutcome *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketOutcome *outcome, int unit)
    {
        return QString();
    }
};

BallotOutcomeTableModel::BallotOutcomeTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new BallotOutcomeTablePriv(wallet, this))
{
    columns
        << tr("Height")
        << tr("Hash")
        ;
}

BallotOutcomeTableModel::~BallotOutcomeTableModel()
{
    delete priv;
}

int BallotOutcomeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int BallotOutcomeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant BallotOutcomeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketOutcome *outcome = (const marketOutcome *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Height:
            return QVariant((int)outcome->nHeight);
        case Hash:
            return formatHash(outcome);
        default:
            ;
        }
        break;
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant BallotOutcomeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Hash:
                return tr("Hash");
            }
        }
    }
    return QVariant();
}

const marketOutcome *BallotOutcomeTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex BallotOutcomeTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketOutcome *outcome = priv->index(row);
    if (outcome)
        return createIndex(row, column, (void *)outcome);
    return QModelIndex();
}

Qt::ItemFlags BallotOutcomeTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BallotOutcomeTableModel::onBranchChange(const marketBranch *branch)
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
    vector<marketOutcome *> vec = pmarkettree->GetOutcomes(branch->GetHash());
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++) {
            InsertMarketObjectHeight(vec[i]);
            priv->cached.append(vec[i]);
        }
        endInsertRows();
    }
}

QString formatHeight(const marketOutcome *outcome)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", outcome->nHeight);
    return QString(tmp);
}

QString formatHash(const marketOutcome *outcome)
{
    return QString::fromStdString(outcome->GetHash().ToString());
}

