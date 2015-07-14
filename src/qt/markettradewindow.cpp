// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "markettradefilterproxymodel.h"
#include "markettradetablemodel.h"
#include "markettradewindow.h"
#include "marketview.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QTableView>
#include <QVBoxLayout>


MarketTradeWindow::MarketTradeWindow(QWidget *parent)
   : marketView((MarketView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Trades"));
    setMinimumSize(800,200);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);
    vlayout->addLayout(glayout);

    QLabel *filterByAddressLabel = new QLabel(tr("Filter By Address: "));
    filterByAddressLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByAddressLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByAddressLabel, 0, 0);

    filterAddress = new QLineEdit();
    glayout->addWidget(filterAddress, 0, 1);
    connect(filterAddress, SIGNAL(textChanged(QString)), this, SLOT(filterAddressChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void MarketTradeWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketTradeTableModel();

    if (!tableModel)
        return;

    proxyModel = new MarketTradeFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(MarketTradeTableModel::BlockNumber, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(MarketTradeTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::BuySell, BUYSELL_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::DecisionState, DECISIONSTATE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::NShares, NSHARES_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::Price, PRICE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketTradeTableModel::Nonce, NONCE_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void MarketTradeWindow::onMarketChange(const marketBranch *branch, const marketDecision *decision, const marketMarket *market)
{
    if (!tableModel || !proxyModel)
        return;

    tableModel->onMarketChange(branch, decision, market);
    if (proxyModel->rowCount()) {
        QModelIndex topLeft = proxyModel->index(0, 0, QModelIndex());
        int columnCount = proxyModel->columnCount();
        if (columnCount > 0) {
            QModelIndex topRight = proxyModel->index(0, columnCount-1, QModelIndex());
            QItemSelection selection(topLeft, topRight);
            tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
        }
        tableView->setFocus();
        currentRowChanged(topLeft, topLeft);
    } else {
        marketView->onTradeChange(0);
    }
}

void MarketTradeWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !marketView || !proxyModel)
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketTrade *trade = tableModel->index(row);
    marketView->onTradeChange(trade);
}

void MarketTradeWindow::filterAddressChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterAddress(str);
}

bool MarketTradeWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == tableView)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            if ((ke->key() == Qt::Key_C)
                && (ke->modifiers().testFlag(Qt::ControlModifier)))
            {
                /* Ctrl-C: copy the selected cells in TableModel */
                QString selected_text;
                QItemSelectionModel *selection = tableView->selectionModel();
                QModelIndexList indexes = selection->selectedIndexes();
                int prev_row = -1;
                for(int i=0; i < indexes.size(); i++) {
                    QModelIndex index = indexes.at(i);
                    if (i) {
                        char c = (index.row() != prev_row)? '\n': '\t';
                        selected_text.append(c);
                    }
                    QVariant data = tableView->model()->data(index);
                    selected_text.append( data.toString() );
                    prev_row = index.row();
                }
                QApplication::clipboard()->setText(selected_text);
                return true;
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

