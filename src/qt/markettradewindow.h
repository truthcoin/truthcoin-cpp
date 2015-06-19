// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETTRADEWINDOW_H
#define TRUTHCOIN_QT_MARKETTRADEWINDOW_H

#include "guiutil.h"

#include <QDialog>
#include <QLabel>

class marketBranch;
class marketDecision;
class marketMarket;
class MarketTradeFilterProxyModel;
class MarketTradeTableModel;
class MarketView;
class WalletModel;


QT_BEGIN_NAMESPACE
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class MarketTradeWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        ADDR_COLUMN_WIDTH = 180,
        BUYSELL_COLUMN_WIDTH = 100,
        NSHARES_COLUMN_WIDTH = 100,
        PRICE_COLUMN_WIDTH = 100,
        DECISIONSTATE_COLUMN_WIDTH = 60,
        NONCE_COLUMN_WIDTH = 100,
    };

    explicit MarketTradeWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onMarketChange(const marketBranch *, const marketDecision *, const marketMarket *);

private:
    QLineEdit *filterAddress;

    MarketView *marketView;
    MarketTradeTableModel *tableModel;
    QTableView *tableView;
    MarketTradeFilterProxyModel *marketTradeProxyModel;

public slots:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterAddressChanged(const QString &);
};

#endif // TRUTHCOIN_QT_MARKETTRADEWINDOW_H
