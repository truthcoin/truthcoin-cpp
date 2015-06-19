// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETTRADETABLEMODEL_H
#define TRUTHCOIN_QT_MARKETTRADETABLEMODEL_H

#include "truthcoinunits.h"

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketDecision;
class marketMarket;
class marketTrade;
class MarketTradeTablePriv;
class WalletModel;
class CWallet;

class MarketTradeTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MarketTradeTableModel(CWallet *, WalletModel * = 0);
    ~MarketTradeTableModel();

    enum ColumnIndex {
        Address = 0,
        BuySell = 1,
        NShares = 2,
        Price = 3,
        DecisionState = 4,
        Nonce = 5,
        BlockNumber = 6,
        Hash = 7,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketTrade *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onMarketChange(const marketBranch *, const marketDecision *, const marketMarket *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    MarketTradeTablePriv *priv;

public slots:
    friend class MarketTradeTablePriv;
};

QString formatAddress(const marketTrade *);
QString formatBuySell(const marketTrade *);
QString formatNShares(const marketTrade *);
QString formatPrice(const marketTrade *);
QString formatDecisionState(const marketTrade *);
QString formatNonce(const marketTrade *);
QString formatBlockNumber(const marketTrade *);
QString formatHash(const marketTrade *);

#endif // TRUTHCOIN_QT_MARKETTRADETABLEMODEL_H
