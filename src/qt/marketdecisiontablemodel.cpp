// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "addresstablemodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "marketdecisiontablemodel.h"
#include "optionsmodel.h"
#include "primitives/market.h"
#include "scicon.h"
#include "walletmodel.h"

#include "main.h"
#include "sync.h"
#include "uint256.h"
#include "util.h"
#include "wallet.h"

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QList>


// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* Address */
        Qt::AlignLeft|Qt::AlignVCenter, /* Prompt */
        Qt::AlignRight|Qt::AlignVCenter, /* EventOverBy */
        Qt::AlignRight|Qt::AlignVCenter, /* IsScaled */
        Qt::AlignRight|Qt::AlignVCenter, /* Minimum */
        Qt::AlignRight|Qt::AlignVCenter, /* Maximum */
        Qt::AlignRight|Qt::AlignVCenter, /* AnswerOptional */
    };


// Private implementation
class MarketDecisionTablePriv
{
public:
    MarketDecisionTablePriv(CWallet *wallet, MarketDecisionTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    MarketDecisionTableModel *parent;

    /* Local cache of Decisiones */
    QList<const marketDecision *> cached;

    int size()
    {
        return cached.size();
    }

    const marketDecision *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketDecision *decision, int unit)
    {
        return QString();
    }
};

MarketDecisionTableModel::MarketDecisionTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new MarketDecisionTablePriv(wallet, this))
{
    columns
        << tr("Address")
        << tr("Prompt")
        << tr("Event Over By")
        << tr("IsScaled")
        << tr("Minimum")
        << tr("Maximum")
        << tr("AnswerOptional")
        ;
}

MarketDecisionTableModel::~MarketDecisionTableModel()
{
    delete priv;
}

int MarketDecisionTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int MarketDecisionTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant MarketDecisionTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketDecision *decision = (const marketDecision *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Address:
            return formatAddress(decision);
        case Prompt:
            return formatPrompt(decision);
        case EventOverBy:
            return formatEventOverBy(decision);
        case IsScaled:
            return formatIsScaled(decision);
        case Minimum:
            return formatMinimum(decision);
        case Maximum:
            return formatMaximum(decision);
        default:
            ;
        }
        break;
    case AddressRole:
        return formatAddress(decision);
    case PromptRole:
        return formatPrompt(decision);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant MarketDecisionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Address:
                return tr("Address");
            case Prompt:
                return tr("Prompt");
            case EventOverBy:
                return tr("EventOverBy");
            case IsScaled:
                return tr("IsScaled");
            case Minimum:
                return tr("Minimum");
            case Maximum:
                return tr("Maximum");
            }
        }
    }
    return QVariant();
}

const marketDecision *MarketDecisionTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex MarketDecisionTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketDecision *decision = priv->index(row);
    if (decision)
        return createIndex(row, column, (void *)decision);
    return QModelIndex();
}

Qt::ItemFlags MarketDecisionTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
MarketDecisionTableModel::onBranchChange(const marketBranch *branch)
{
    if (!priv)
        return;

    /* erase cache */
    if (priv->cached.size()) {
        beginRemoveRows(QModelIndex(), 0, priv->cached.size()-1);
        priv->cached.clear();
        endRemoveRows();
    }

    if (!branch)
        return;

    /* new vector of decisions for cache */
    std::vector<const marketDecision *> vec;
    std::map<uint256, marketDecision *>::const_iterator it;
    for(it=branch->decisions.begin(); it != branch->decisions.end(); it++)
        vec.push_back(it->second);

    /* insert into cache */
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++)
            priv->cached.append(vec[i]);
        endInsertRows();
    }
}

QString formatAddress(const marketDecision *decision)
{
    CTruthcoinAddress addr;
    if (addr.Set(decision->keyID))
        return QString::fromStdString(addr.ToString());
    return QString("Address");
}

QString formatPrompt(const marketDecision *decision)
{
    return QString::fromStdString(decision->prompt);
}

QString formatEventOverBy(const marketDecision *decision)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", decision->eventOverBy);
    return QString(tmp);
}

QString formatIsScaled(const marketDecision *decision)
{
    return QString(decision->isScaled? "Yes": "No");
}

QString formatMaximum(const marketDecision *decision)
{
    if (decision->isScaled) {
       char tmp[32];
       snprintf(tmp, sizeof(tmp), "%.8f", decision->max*1e-8);
       return QString(tmp);
    }
    return QString("");
}

QString formatMinimum(const marketDecision *decision)
{
    if (decision->isScaled) {
       char tmp[32];
       snprintf(tmp, sizeof(tmp), "%.8f", decision->min*1e-8);
       return QString(tmp);
    }
    return QString("");
}

QString formatAnswerOptional(const marketDecision *decision)
{
    return QString(decision->answerOptionality? "Optional": "Not Optional");
}

QString formatBranchID(const marketDecision *decision)
{
    return QString::fromStdString(decision->branchid.ToString());
}

QString formatHash(const marketDecision *decision)
{
    return QString::fromStdString(decision->GetHash().ToString());
}

