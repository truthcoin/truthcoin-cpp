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

#include "marketdecisionfilterproxymodel.h"
#include "marketdecisiontablemodel.h"
#include "marketdecisionwindow.h"
#include "marketview.h"
#include "walletmodel.h"


MarketDecisionWindow::MarketDecisionWindow(QWidget *parent)
    : marketView((MarketView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Decisions"));
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

    QLabel *filterByPromptLabel = new QLabel(tr("Filter By Prompt: "));
    filterByPromptLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByPromptLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByPromptLabel, 1, 0);

    filterPrompt = new QLineEdit();
    glayout->addWidget(filterPrompt, 1, 1);
    connect(filterPrompt, SIGNAL(textChanged(QString)), this, SLOT(filterPromptChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void MarketDecisionWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getMarketDecisionTableModel();
    if (!tableModel)
        return;

    proxyModel = new MarketDecisionFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(MarketDecisionTableModel::Prompt, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(MarketDecisionTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::Prompt, PROMPT_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::EventOverBy, EVENTOVERBY_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::IsScaled, ISSCALED_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::Minimum, MINIMUM_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::Maximum, MAXIMUM_COLUMN_WIDTH);
    tableView->setColumnWidth(MarketDecisionTableModel::AnswerOptional, ANSWEROPTIONAL_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void MarketDecisionWindow::onBranchChange(const marketBranch *branch)
{
    if (!tableModel || !proxyModel)
        return;

    tableModel->onBranchChange(branch);
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
    }
    else {
       marketView->onDecisionChange(0);
    }
}

bool MarketDecisionWindow::eventFilter(QObject *obj, QEvent *event)
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

void MarketDecisionWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !marketView || !proxyModel || !curr.isValid())
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketDecision *decision = tableModel->index(row);
    marketView->onDecisionChange(decision);
}

void MarketDecisionWindow::filterAddressChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterAddress(str);
}

void MarketDecisionWindow::filterPromptChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterPrompt(str);
}

