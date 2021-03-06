// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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

#include "marketmarketfilterproxymodel.h"
#include "marketmarkettablemodel.h"
#include "marketmarketwindow.h"
#include "marketview.h"
#include "walletmodel.h"


MarketMarketWindow::MarketMarketWindow(QWidget *parent)
    : marketView((MarketView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Markets"));
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

    QLabel *filterByTitleLabel = new QLabel(tr("Filter By Title: "));
    filterByTitleLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByTitleLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByTitleLabel, 1, 0);

    filterTitle = new QLineEdit();
    glayout->addWidget(filterTitle, 1, 1);
    connect(filterTitle, SIGNAL(textChanged(QString)), this, SLOT(filterTitleChanged(QString)));

    QLabel *filterByDescriptionLabel = new QLabel(tr("Filter By Description: "));
    filterByDescriptionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByDescriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByDescriptionLabel, 2, 0);

    filterDescription = new QLineEdit();
    glayout->addWidget(filterDescription, 2, 1);
    connect(filterDescription, SIGNAL(textChanged(QString)), this, SLOT(filterDescriptionChanged(QString)));

    QLabel *filterByTagLabel = new QLabel(tr("Filter By Tag: "));
    filterByTagLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByTagLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByTagLabel, 3, 0);

    filterTag = new QLineEdit();
    glayout->addWidget(filterTag, 3, 1);
    connect(filterTag, SIGNAL(textChanged(QString)), this, SLOT(filterTagChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void MarketMarketWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketMarketTableModel();
    if (!tableModel)
        return;

    proxyModel = new MarketMarketFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(MarketMarketTableModel::Title, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(MarketMarketTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::B, B_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::TradingFee, TRADINGFEE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::Title, TITLE_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::Description, DESCRIPTION_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::Tags, TAGS_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::Maturation, MATURATION_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketMarketTableModel::DecisionIDs, DECISIONIDS_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void MarketMarketWindow::onDecisionChange(const marketBranch *branch, const marketDecision *decision)
{
    if (!tableModel || !proxyModel)
        return;

    tableModel->onDecisionChange(branch, decision);
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
       marketView->onMarketChange(0);
    }
}

bool MarketMarketWindow::eventFilter(QObject *obj, QEvent *event)
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

void MarketMarketWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !marketView || !proxyModel || !curr.isValid())
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketMarket *market = tableModel->index(row);
    marketView->onMarketChange(market);
}

void MarketMarketWindow::filterAddressChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterAddress(str);
}

void MarketMarketWindow::filterTitleChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterTitle(str);
}

void MarketMarketWindow::filterDescriptionChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterDescription(str);
}

void MarketMarketWindow::filterTagChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterTag(str);
}

