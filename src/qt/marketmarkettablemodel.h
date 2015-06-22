// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETMARKETTABLEMODEL_H
#define TRUTHCOIN_QT_MARKETMARKETTABLEMODEL_H

#include "truthcoinunits.h"

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketDecision;
class marketMarket;
class MarketMarketTablePriv;
class WalletModel;
class CWallet;

class MarketMarketTableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MarketMarketTableModel(CWallet *, WalletModel * = 0);
    ~MarketMarketTableModel();

    enum ColumnIndex {
        Address = 0,
        B = 1,
        TradingFee = 2,
        MaxCommission = 3,
        Title = 4,
        Description = 5,
        Tags = 6,
        Maturation = 7,
        DecisionIDs = 8,
        DecisionFunctionIDs = 9,
        AccountValue = 10,
        TxPoW = 11,
        Hash = 12,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
        TitleRole,
        DescriptionRole,
        TagsRole,
        DecisionIDsRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketMarket *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex & parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onDecisionChange(const marketBranch *, const marketDecision *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    MarketMarketTablePriv *priv;

public slots:
    friend class MarketMarketTablePriv;
};

QString formatAddress(const marketMarket *);
QString formatB(const marketMarket *);
QString formatTradingFee(const marketMarket *);
QString formatMaxCommission(const marketMarket *);
QString formatTitle(const marketMarket *);
QString formatDescription(const marketMarket *);
QString formatTags(const marketMarket *);
QString formatMaturation(const marketMarket *);
QString formatDecisionIDs(const marketMarket *);
QString formatDecisionFunctionIDs(const marketMarket *);
QString formatAccountValue(const marketMarket *);
QString formatTxPoW(const marketMarket *);
QString formatHash(const marketMarket *);

#endif // TRUTHCOIN_QT_MARKETMARKETTABLEMODEL_H
