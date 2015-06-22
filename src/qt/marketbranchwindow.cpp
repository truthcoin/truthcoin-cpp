// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketbranchfilterproxymodel.h"
#include "marketbranchtablemodel.h"
#include "marketbranchwindow.h"
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


MarketBranchWindow::MarketBranchWindow(QWidget *parent)
    : marketView((MarketView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Branches"));
    setMinimumSize(800,200);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);

    QLabel *filterByDescriptionLabel = new QLabel(tr("Filter By Description: "));
    filterByDescriptionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByDescriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByDescriptionLabel, 0, 0);

    filterDescription = new QLineEdit();
    glayout->addWidget(filterDescription, 0, 1);
    connect(filterDescription, SIGNAL(textChanged(QString)), this, SLOT(filterDescriptionChanged(QString)));

    QTableView *view = new QTableView(this);
    vlayout->addLayout(glayout);
    vlayout->addWidget(view);
    vlayout->setSpacing(0);
    int width = view->verticalScrollBar()->sizeHint().width();

    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->installEventFilter(this);

    tableView = view;
}

void MarketBranchWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketBranchTableModel();

    if (!tableModel)
        return;

    proxyModel = new MarketBranchFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(MarketBranchTableModel::Name, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(MarketBranchTableModel::Name, NAME_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::Description, DESCRIPTION_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::BaseListingFee, BASELISTINGFEE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::TargetDecisions, TARGETDECISIONS_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::MaxDecisions, MAXDECISIONS_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::MinTradingFee, MINTRADINGFEE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::Tau, TAU_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::BallotTime, BALLOTTIME_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::UnsealTime, UNSEALTIME_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketBranchTableModel::ConsensusThreshold, CONSENSUSTHRESHOLD_COLUMN_WIDTH);

    tableModel->setTable();

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));

    QModelIndex topLeft = proxyModel->index(0, 0, QModelIndex());
    int columnCount = proxyModel->columnCount();
    if (columnCount > 0) {
        QModelIndex topRight = proxyModel->index(0, columnCount-1, QModelIndex());
        QItemSelection selection(topLeft, topRight);
        tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
    }
    tableView->setFocus();
    currentRowChanged(topLeft, topLeft);
}

void MarketBranchWindow::setTableViewFocus(void)
{
    if (tableView)
        tableView->setFocus();
}

void MarketBranchWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !marketView || !proxyModel)
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketBranch *branch = tableModel->index(row);
    marketView->onBranchChange(branch);
}

void MarketBranchWindow::filterDescriptionChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterDescription(str);
}

