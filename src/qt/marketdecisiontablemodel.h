// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETDECISIONTABLEMODEL_H
#define TRUTHCOIN_QT_MARKETDECISIONTABLEMODEL_H

#include "truthcoinunits.h"

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketDecision;
class MarketDecisionTablePriv;
class WalletModel;
class CWallet;


class MarketDecisionTableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MarketDecisionTableModel(CWallet *, WalletModel * = 0);
    ~MarketDecisionTableModel();

    enum ColumnIndex {
        Address = 0,
        Prompt = 1,
        EventOverBy = 2,
        IsScaled = 3,
        Minimum = 4,
        Maximum = 5,
        AnswerOptional = 6,
        Hash = 7,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
        PromptRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketDecision *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex & parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onBranchChange(const marketBranch *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    MarketDecisionTablePriv *priv;

public slots:
    friend class MarketDecisionTablePriv;
};

QString formatAddress(const marketDecision *);
QString formatPrompt(const marketDecision *);
QString formatEventOverBy(const marketDecision *);
QString formatIsScaled(const marketDecision *);
QString formatMinimum(const marketDecision *);
QString formatMaximum(const marketDecision *);
QString formatAnswerOptional(const marketDecision *);
QString formatHash(const marketDecision *);

#endif // TRUTHCOIN_QT_MARKETDECISIONTABLEMODEL_H
