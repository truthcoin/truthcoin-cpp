// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTBALLOTWINDOW_H
#define TRUTHCOIN_QT_BALLOTBALLOTWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class BallotBallotFilterProxyModel;
class BallotBallotTableModel;
class BallotView;
class WalletModel;


class BallotBallotWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        HEIGHT_COLUMN_WIDTH = 100,
        NDECISIONS_COLUMN_WIDTH = 150,
    };

    explicit BallotBallotWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onBranchChange(const marketBranch *);
    bool eventFilter(QObject *, QEvent *);

private:
    BallotView *ballotView;
    BallotBallotTableModel *tableModel;
    QTableView *tableView;
    BallotBallotFilterProxyModel *proxyModel;

public slots:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
};

#endif // TRUTHCOIN_QT_BALLOTBALLOTWINDOW_H
