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

#include "marketbranchfilterproxymodel.h"
#include "marketbranchtablemodel.h"
#include "ballotbranchwindow.h"
#include "ballotview.h"
#include "walletmodel.h"


BallotBranchWindow::BallotBranchWindow(QWidget *parent)
    : ballotView((BallotView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Branches"));
    setMinimumSize(800,200);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);
    vlayout->addLayout(glayout);

    QLabel *filterByDescriptionLabel = new QLabel(tr("Filter By Description: "));
    filterByDescriptionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByDescriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByDescriptionLabel, 0, 0);

    filterDescription = new QLineEdit();
    glayout->addWidget(filterDescription, 0, 1);
    connect(filterDescription, SIGNAL(textChanged(QString)), this, SLOT(filterDescriptionChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void BallotBranchWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketBranchTableModel();

    if (!tableModel)
        return;

    proxyModel = new MarketBranchFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

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

void BallotBranchWindow::setTableViewFocus(void)
{
    if (tableView)
        tableView->setFocus();
}

bool BallotBranchWindow::eventFilter(QObject *obj, QEvent *event)
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

void BallotBranchWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !ballotView || !proxyModel || !curr.isValid())
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketBranch *branch = tableModel->index(row);
    ballotView->onBranchChange(branch);
}

void BallotBranchWindow::filterDescriptionChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterDescription(str);
}

