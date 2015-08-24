// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETBRANCHWINDOW_H
#define TRUTHCOIN_QT_MARKETBRANCHWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class MarketBranchFilterProxyModel;
class MarketBranchTableModel;
class MarketView;
class WalletModel;


class MarketBranchWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        NAME_COLUMN_WIDTH = 100,
        DESCRIPTION_COLUMN_WIDTH = 150,
        BASELISTINGFEE_COLUMN_WIDTH = 80,
        FREEDECISIONS_COLUMN_WIDTH = 60,
        TARGETDECISIONS_COLUMN_WIDTH = 60,
        MAXDECISIONS_COLUMN_WIDTH = 60,
        MINTRADINGFEE_COLUMN_WIDTH = 80,
        TAU_COLUMN_WIDTH = 40,
        BALLOTTIME_COLUMN_WIDTH = 80,
        UNSEALTIME_COLUMN_WIDTH = 80,
        CONSENSUSTHRESHOLD_COLUMN_WIDTH = 100,
    };

    explicit MarketBranchWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void setTableViewFocus(void);
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *filterDescription;

    MarketView *marketView;
    MarketBranchTableModel *tableModel;
    QTableView *tableView;
    MarketBranchFilterProxyModel *proxyModel;

public slots:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterDescriptionChanged(const QString &);
};

#endif // TRUTHCOIN_QT_MARKETBRANCHWINDOW_H
