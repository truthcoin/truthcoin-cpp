// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "addresstablemodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "marketbranchtablemodel.h"
#include "optionsmodel.h"
#include "primitives/market.h"
#include "scicon.h"
#include "walletmodel.h"

#include "main.h"
#include "sync.h"
#include "txdb.h"
#include "uint256.h"
#include "util.h"
#include "wallet.h"

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QList>

extern CMarketTreeDB *pmarkettree;


// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* name */
        Qt::AlignLeft|Qt::AlignVCenter, /* description */
        Qt::AlignLeft|Qt::AlignVCenter, /* baseListingFee */
        Qt::AlignRight|Qt::AlignVCenter, /* freeDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* targetDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* maxDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* minTradingFee */
        Qt::AlignRight|Qt::AlignVCenter, /* tau */
        Qt::AlignRight|Qt::AlignVCenter, /* ballotTime */
        Qt::AlignLeft|Qt::AlignVCenter, /* unsealTime */
        Qt::AlignLeft|Qt::AlignVCenter /* consensusThreshold */
    };


// Private implementation
class MarketBranchTablePriv
{
public:
    MarketBranchTablePriv(CWallet *wallet, MarketBranchTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    MarketBranchTableModel *parent;

    /* Local cache of Branches */
    QList<const marketBranch *> cached;

    int size()
    {
        return cached.size();
    }

    const marketBranch *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketBranch *branch, int unit)
    {
        return QString();
    }
};

MarketBranchTableModel::MarketBranchTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new MarketBranchTablePriv(wallet, this))
{
    columns
        << tr("Name")
        << tr("Description")
        << tr("Base Listing Fee")
        << tr("Free Decisions")
        << tr("Target Decisions")
        << tr("Max Decisions")
        << tr("Min Trading Fee")
        << tr("Tau")
        << tr("Ballot Time")
        << tr("Unseal Time")
        << tr("ConsensusThreshold")
        ;
}

MarketBranchTableModel::~MarketBranchTableModel()
{
    delete priv;
}

int MarketBranchTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int MarketBranchTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant MarketBranchTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketBranch *branch = (const marketBranch *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Name:
            return formatName(branch);
        case Description:
            return formatDescription(branch);
        case BaseListingFee:
            return formatBaseListingFee(branch);
        case FreeDecisions:
            return formatFreeDecisions(branch);
        case TargetDecisions:
            return formatTargetDecisions(branch);
        case MaxDecisions:
            return formatMaxDecisions(branch);
        case MinTradingFee:
            return formatMinTradingFee(branch);
        case Tau:
            return formatTau(branch);
        case BallotTime:
            return formatBallotTime(branch);
        case UnsealTime:
            return formatUnsealTime(branch);
        case ConsensusThreshold:
            return formatConsensusThreshold(branch);
        default:
            ;
        }
        break;
    case DescriptionRole:
        return formatDescription(branch);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant MarketBranchTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Name:
                return tr("Name.");
            case Description:
                return tr("Description.");
            case BaseListingFee:
                return tr("BaseListingFee.");
            case FreeDecisions:
                return tr("FreeDecisions.");
            case TargetDecisions:
                return tr("TargetDecisions.");
            case MaxDecisions:
                return tr("MaxDecisions.");
            case MinTradingFee:
                return tr("MinTradingFee.");
            case Tau:
                return tr("Tau.");
            case BallotTime:
                return tr("BallotTime.");
            case UnsealTime:
                return tr("UnsealTime.");
            case ConsensusThreshold:
                return tr("ConsensusThreshold.");
            }
        }
    }
    return QVariant();
}

const marketBranch *MarketBranchTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex MarketBranchTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketBranch *branch = priv->index(row);
    if (branch)
        return createIndex(row, column, (void *)branch);
    return QModelIndex();
}

Qt::ItemFlags MarketBranchTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
MarketBranchTableModel::setTable(void)
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

    /* insert into cache */
    vector<marketBranch *> vec = pmarkettree->GetBranches();
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(size_t i=0; i < vec.size(); i++)
            priv->cached.append(vec[i]);
        endInsertRows();
    }
}

QString formatName(const marketBranch *branch)
{
    return QString::fromStdString(branch->name);
}

QString formatDescription(const marketBranch *branch)
{
    return QString::fromStdString(branch->description);
}

QString formatBaseListingFee(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", branch->baseListingFee*1e-8);
    return QString(tmp);
}

QString formatFreeDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->freeDecisions);
    return QString(tmp);
}

QString formatTargetDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->targetDecisions);
    return QString(tmp);
}

QString formatMaxDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->maxDecisions);
    return QString(tmp);
}

QString formatMinTradingFee(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", branch->minTradingFee*1e-8);
    return QString(tmp);
}

QString formatTau(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->tau);
    return QString(tmp);
}

QString formatBallotTime(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->ballotTime);
    return QString(tmp);
}

QString formatUnsealTime(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->unsealTime);
    return QString(tmp);
}

QString formatConsensusThreshold(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.3f", branch->consensusThreshold*1e-8);
    return QString(tmp);
}

QString formatHash(const marketBranch *branch)
{
    return QString::fromStdString(branch->GetHash().ToString());
}

