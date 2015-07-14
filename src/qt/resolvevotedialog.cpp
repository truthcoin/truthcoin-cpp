// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

extern "C" {
#include "linalg/src/tc_mat.h"
}
#include "resolvevotedialog.h"
#include "resolvevotecoltablemodel.h"
#include "resolvevoterowtablemodel.h"
#include "resolvevoteinputtablemodel.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>


ResolveVoteDialog::ResolveVoteDialog(QWidget *parent)
    : QDialog(parent),
    vote(0),
    model(0)
{
    setWindowTitle(tr("Resolve Vote Scenarios"));
    setMinimumSize(800, 200);

    /* vlayout:                */
    /*     [Input  groupbox]   */
    /*     [Buttons] x3        */
    /*     [Output groupbox]   */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);

    QGroupBox *groupbox1 = new QGroupBox(tr("Input"));
    QVBoxLayout *vlayout1 = new QVBoxLayout();
    groupbox1->setLayout(vlayout1);
    vlayout->addWidget(groupbox1);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setSpacing(20);
    vlayout->addLayout(hlayout);

    QGroupBox *groupbox2 = new QGroupBox(tr("Output"));
    QVBoxLayout *vlayout2 = new QVBoxLayout();
    groupbox2->setLayout(vlayout2);
    vlayout->addWidget(groupbox2);

    /* vlayout1 (input)        */
    inputTableView = new QTableView();
    inputTableView->installEventFilter(this);
    inputTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    inputTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    vlayout1->addWidget(inputTableView);

    /* hlayout (button row)    */
    QPushButton *addVoterButton = new QPushButton(tr("Add Voter"));
    connect(addVoterButton, SIGNAL(clicked()), this, SLOT(onAddVoter()));
    hlayout->addWidget(addVoterButton);
    QPushButton *addDecisionButton = new QPushButton(tr("Add Decision"));
    connect(addDecisionButton, SIGNAL(clicked()), this, SLOT(onAddDecision()));
    hlayout->addWidget(addDecisionButton);
    QPushButton *calculateButton = new QPushButton(tr("Calculate"));
    connect(calculateButton, SIGNAL(clicked()), this, SLOT(onCalculate()));
    hlayout->addWidget(calculateButton);

    /* vlayout2 (output)       */

    /* col table */
    colTableView = new QTableView();
    colTableView->installEventFilter(this);
    colTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    colTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    vlayout2->addWidget(colTableView);

    /* row table */
    rowTableView = new QTableView();
    rowTableView->installEventFilter(this);
    rowTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    rowTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    vlayout2->addWidget(rowTableView);


    /* starting data */

    NA = 0.138042e-30;
    alpha = 0.10;
    tol = 0.10;
    nVoters = 6;
    nDecisions = 6;
    vote = tc_vote_ctr(nVoters, nDecisions);
    vote->NA = NA;
    vote->alpha = alpha;
    vote->tol = tol;
    double **wgt = vote->rvecs[TC_VOTE_OLD_REP]->a;
    wgt[0][0] = 0.05;
    wgt[1][0] = 0.05;
    wgt[2][0] = 0.05;
    wgt[3][0] = 0.05;
    wgt[4][0] = 0.10;
    wgt[5][0] = 0.70;
    double **isbin = vote->cvecs[TC_VOTE_IS_BINARY]->a;
    for(uint32_t j=0; j < nDecisions; j++)
        isbin[0][j] = (j < 4)? 1.0: 0;
    double **m = vote->M->a;
    m[0][0] = 1.0; m[0][1] = 1.0; m[0][2] = 0.0; m[0][3] = 0.0; m[0][4] = 233.0/435; m[0][5] = (16027.59-8000)/(20000-8000);
    m[1][0] = 1.0; m[1][1] = 0.0; m[1][2] = 0.0; m[1][3] = 0.0; m[1][4] = 199.0/435; m[1][5] = NA;
    m[2][0] = 1.0; m[2][1] = 1.0; m[2][2] = 0.0; m[2][3] = 0.0; m[2][4] = 233.0/435; m[2][5] = (16027.59-8000)/(20000-8000);
    m[3][0] = 1.0; m[3][1] = 1.0; m[3][2] = 1.0; m[3][3] = 0.0; m[3][4] = 250.0/435; m[3][5] = NA;
    m[4][0] = 0.0; m[4][1] = 0.0; m[4][2] = 1.0; m[4][3] = 1.0; m[4][4] = 435.0/435; m[4][5] = ( 8001.00-8000)/(20000-8000);
    m[5][0] = 0.0; m[5][1] = 0.0; m[5][2] = 1.0; m[5][3] = 1.0; m[5][4] = 435.0/435; m[5][5] = (19999.00-8000)/(20000-8000);
    tc_vote_proc(vote);
}

ResolveVoteDialog::~ResolveVoteDialog()
{

}

void ResolveVoteDialog::setModel(WalletModel *model)
{
    if (!model)
        return;

    this->model = model;

    inputTableModel = model->getResolveVoteInputTableModel();
    if (inputTableModel) {
        inputTableModel->setVotePtr(&vote);
        inputTableView->setModel(inputTableModel);
        inputTableView->setAlternatingRowColors(true);
        inputTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
#if QT_VERSION >= 0x050000
        QHeaderView *hHeader = inputTableView->horizontalHeader();
        hHeader->sectionResizeMode(QHeaderView::Fixed);
        hHeader->setDefaultSectionSize(100);
    
        QHeaderView *vHeader = inputTableView->verticalHeader();
        vHeader->sectionResizeMode(QHeaderView::Fixed);
        vHeader->setDefaultSectionSize(20);
#endif

        inputTableModel->onVoteChange();
    }

    colTableModel = model->getResolveVoteColTableModel();
    if (colTableModel) {
        colTableModel->setVotePtr(&vote);
        colTableView->setModel(colTableModel);
        colTableView->setAlternatingRowColors(true);
        colTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
#if QT_VERSION >= 0x050000
        QHeaderView *hHeader = inputTableView->horizontalHeader();
        hHeader->sectionResizeMode(QHeaderView::Fixed);
        hHeader->setDefaultSectionSize(100);
    
        QHeaderView *vHeader = colTableView->verticalHeader();
        vHeader->sectionResizeMode(QHeaderView::Fixed);
        vHeader->setDefaultSectionSize(20);
#endif
    }

    rowTableModel = model->getResolveVoteRowTableModel();
    if (rowTableModel) {
        rowTableModel->setVotePtr(&vote);
        rowTableView->setModel(rowTableModel);
        rowTableView->setAlternatingRowColors(true);
        rowTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
#if QT_VERSION >= 0x050000
        QHeaderView *hHeader = inputTableView->horizontalHeader();
        hHeader->sectionResizeMode(QHeaderView::Fixed);
        hHeader->setDefaultSectionSize(100);
    
        QHeaderView *vHeader = rowTableView->verticalHeader();
        vHeader->sectionResizeMode(QHeaderView::Fixed);
        vHeader->setDefaultSectionSize(20);
#endif
    }
}

bool ResolveVoteDialog::eventFilter(QObject *obj, QEvent *event)
{
    /* table with focus */
    QTableView *tableView = 0;
    if (obj == inputTableView)
        tableView = inputTableView;
    else
    if (obj == rowTableView)
        tableView = rowTableView;
    else
    if (obj == colTableView)
        tableView = colTableView;

    if (tableView)
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

void ResolveVoteDialog::onAddVoter(void)
{

}

void ResolveVoteDialog::onAddDecision(void)
{

}

void ResolveVoteDialog::onCalculate(void)
{
    tc_vote_proc(vote);
    colTableModel->onVoteChange(TC_VOTE_NCOLS,nDecisions);
    rowTableModel->onVoteChange(nVoters,TC_VOTE_NROWS);
}

