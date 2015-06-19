// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "markettradefilterproxymodel.h"
#include "markettradetablemodel.h"
#include "markettradewindow.h"
#include "marketview.h"
#include "walletmodel.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTableView>
#include <QVBoxLayout>

MarketTradeWindow::MarketTradeWindow(QWidget *parent)
   : marketView((MarketView *)parent),
    tableModel(0),
    tableView(0),
    marketTradeProxyModel(0)
{
    setWindowTitle(tr("Trades"));
    setMinimumSize(800,200);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);

    QLabel *filterByAddressLabel = new QLabel(tr("Filter By Address: "));
    filterByAddressLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByAddressLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByAddressLabel, 0, 0);

    filterAddress = new QLineEdit();
    glayout->addWidget(filterAddress, 0, 1);
    connect(filterAddress, SIGNAL(textChanged(QString)), this, SLOT(filterAddressChanged(QString)));

    QTableView *view = new QTableView(this);
    vlayout->addLayout(glayout);
    vlayout->addWidget(view);
    vlayout->setSpacing(0);

    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->installEventFilter(this);

    tableView = view;
}

void MarketTradeWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketTradeTableModel();

    if (!tableModel)
        return;

    marketTradeProxyModel = new MarketTradeFilterProxyModel(this);
    marketTradeProxyModel->setSourceModel(tableModel);

    // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(marketTradeProxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(MarketTradeTableModel::BlockNumber, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(MarketTradeTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::BuySell, BUYSELL_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::NShares, NSHARES_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::Price, PRICE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::DecisionState, DECISIONSTATE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::Nonce, NONCE_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void MarketTradeWindow::onMarketChange(const marketBranch *branch, const marketDecision *decision, const marketMarket *market)
{
    if (!branch || !decision || !market || !tableModel)
        return;

    tableModel->onMarketChange(branch, decision, market);

    QModelIndex topLeft = tableModel->index(0, 0, QModelIndex());
    tableView->setCurrentIndex(topLeft);
    currentRowChanged(topLeft, topLeft);
}

void MarketTradeWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableView || !tableModel)
        return;

    uint32_t row = curr.row();
    const marketTrade *trade = tableModel->index(row);
    marketView->onTradeChange(trade);
}

void MarketTradeWindow::filterAddressChanged(const QString &str)
{
    if (marketTradeProxyModel)
        marketTradeProxyModel->setFilterAddress(str);
}

