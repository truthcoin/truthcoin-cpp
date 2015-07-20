// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
extern "C" {
#include "linalg/src/tc_mat.h"
}
#include "resolvevotecoltablemodel.h"
#include "resolvevotedialog.h"
#include "resolvevotegraph.h"
#include "resolvevoteinputtablemodel.h"
#include "resolvevoterowtablemodel.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QString>
#include <QTableView>
#include <QTabWidget>
#include <QVBoxLayout>

static QVBoxLayout *v4layout = NULL;
static QScrollArea *scrollArea = NULL;

ResolveVoteDialog::ResolveVoteDialog(QWidget *parent)
    : QDialog(parent),
    vote(0),
    model(0)
{
    char tmp[32];

    /* starting data */

    vote = tc_vote_ctr(6, 6);
    vote->NA = 0.138042e-30;
    vote->alpha = 0.10;
    vote->tol = 0.10;
    double **rep = vote->rvecs[TC_VOTE_OLD_REP]->a;
    rep[0][0] = 0.05;
    rep[1][0] = 0.05;
    rep[2][0] = 0.05;
    rep[3][0] = 0.05;
    rep[4][0] = 0.10;
    rep[5][0] = 0.70;
    double **isbin = vote->cvecs[TC_VOTE_IS_BINARY]->a;
    for(uint32_t j=0; j < vote->nc; j++)
        isbin[0][j] = (j < 4)? 1.0: 0;
    double **m = vote->M->a;
    m[0][0] = 1.0; m[0][1] = 1.0; m[0][2] = 0.0; m[0][3] = 0.0; m[0][4] = 233.0/435; m[0][5] = (16027.59-8000)/(20000-8000);
    m[1][0] = 1.0; m[1][1] = 0.0; m[1][2] = 0.0; m[1][3] = 0.0; m[1][4] = 199.0/435; m[1][5] = vote->NA;
    m[2][0] = 1.0; m[2][1] = 1.0; m[2][2] = 0.0; m[2][3] = 0.0; m[2][4] = 233.0/435; m[2][5] = (16027.59-8000)/(20000-8000);
    m[3][0] = 1.0; m[3][1] = 1.0; m[3][2] = 1.0; m[3][3] = 0.0; m[3][4] = 250.0/435; m[3][5] = vote->NA;
    m[4][0] = 0.0; m[4][1] = 0.0; m[4][2] = 1.0; m[4][3] = 1.0; m[4][4] = 435.0/435; m[4][5] = ( 8001.00-8000)/(20000-8000);
    m[5][0] = 0.0; m[5][1] = 0.0; m[5][2] = 1.0; m[5][3] = 1.0; m[5][4] = 435.0/435; m[5][5] = (19999.00-8000)/(20000-8000);
    int rc = tc_vote_proc(vote);

    /* gui */
    setWindowTitle(tr("Resolve Vote Scenarios"));
    setMinimumSize(800, 200);

    /* vlayout:                */
    /*     [Input  groupbox]   */
    /*     [Output groupbox]   */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);

    QGroupBox *groupbox1 = new QGroupBox(tr("Input"));
    QVBoxLayout *v1layout = new QVBoxLayout();
    groupbox1->setLayout(v1layout);
    vlayout->addWidget(groupbox1);

    QGroupBox *groupbox2 = new QGroupBox(tr("Output"));
    QVBoxLayout *v2layout = new QVBoxLayout();
    groupbox2->setLayout(v2layout);
    vlayout->addWidget(groupbox2);

    /* v1layout (input)        */

    /* input params */
    /*   # Voters    [   ]    # Decisions [   ]                  */
    /*   alpha       [   ]    tol         [   ]     NA    [   ]  */
    QGridLayout *g1layout = new QGridLayout();
    g1layout->setHorizontalSpacing(0);
    g1layout->setColumnStretch(0, 1);
    g1layout->setColumnStretch(1, 2);
    g1layout->setColumnStretch(2, 1); /* space */
    g1layout->setColumnStretch(3, 1);
    g1layout->setColumnStretch(4, 2);
    g1layout->setColumnStretch(5, 1); /* space */
    g1layout->setColumnStretch(6, 1);
    g1layout->setColumnStretch(7, 2);
    v1layout->addLayout(g1layout);

    snprintf(tmp, sizeof(tmp), "%u", vote->nr);
    nVotersLabel = new QLabel(tr("# Voters: "));
    g1layout->addWidget(nVotersLabel, /* row */0, /* col */0);
    nVotersLineEdit = new QLineEdit();
    nVotersLineEdit->setText(tmp);
    nVotersLineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    g1layout->addWidget(nVotersLineEdit, /* row */0, /* col */1);
    connect(nVotersLineEdit, SIGNAL(editingFinished()), this, SLOT(onNVotersChange()));

    snprintf(tmp, sizeof(tmp), "%u", vote->nc);
    nDecisionsLabel = new QLabel(tr("# Decisions: "));
    g1layout->addWidget(nDecisionsLabel, /* row */0, /* col */3);
    nDecisionsLineEdit = new QLineEdit();
    nDecisionsLineEdit->setText(tmp);
    nDecisionsLineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    g1layout->addWidget(nDecisionsLineEdit, /* row */0, /* col */4);
    connect(nDecisionsLineEdit, SIGNAL(editingFinished()), this, SLOT(onNDecisionsChange()));

    snprintf(tmp, sizeof(tmp), "%.8f", vote->alpha);
    alphaLabel = new QLabel(tr("alpha: "));
    g1layout->addWidget(alphaLabel, /* row */1, /* col */0);
    alphaLineEdit = new QLineEdit();
    alphaLineEdit->setText(tmp);
    alphaLineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    g1layout->addWidget(alphaLineEdit, /* row */1, /* col */1);
    connect(alphaLineEdit, SIGNAL(editingFinished()), this, SLOT(onAlphaChange()));

    snprintf(tmp, sizeof(tmp), "%.8f", vote->tol);
    tolLabel = new QLabel(tr("tol: "));
    g1layout->addWidget(tolLabel, /* row */1, /* col */3);
    tolLineEdit = new QLineEdit();
    tolLineEdit->setText(tmp);
    tolLineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    g1layout->addWidget(tolLineEdit, /* row */1, /* col */4);
    connect(tolLineEdit, SIGNAL(editingFinished()), this, SLOT(onTolChange()));

    snprintf(tmp, sizeof(tmp), "%.8e", vote->NA);
    NALabel = new QLabel(tr("NA: "));
    g1layout->addWidget(NALabel, /* row */1, /* col */6);
    NALineEdit = new QLineEdit();
    NALineEdit->setText(tmp);
    NALineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    g1layout->addWidget(NALineEdit, /* row */1, /* col */7);
    connect(NALineEdit, SIGNAL(textEdited()), this, SLOT(onNAChange()));

    /* input table */
    inputTableView = new QTableView();
    inputTableView->installEventFilter(this);
    inputTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    inputTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    v1layout->addWidget(inputTableView);

    /* v2layout (output)       */

    QTabWidget *tabs = new QTabWidget();
    QWidget *dataTab = new QWidget();
    tabs->addTab(dataTab, tr("Data"));
    QWidget *graphTab = new QWidget();
    tabs->addTab(graphTab, tr("Plot"));
    v2layout->addWidget(tabs);

    /* graph tab */
    v4layout = new QVBoxLayout(graphTab);
    resolveVoteGraph = new ResolveVoteGraph();
    resolveVoteGraph->setVotePtr((const struct tc_vote **)&vote);
    scrollArea = new QScrollArea();
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(resolveVoteGraph);
    v4layout->addWidget(scrollArea);
    QVBoxLayout *v5layout = new QVBoxLayout(scrollArea);
    v5layout->addWidget(resolveVoteGraph);

    /* data tab */
    QVBoxLayout *v3layout = new QVBoxLayout(dataTab);

    /* output params */
    QGridLayout *g2layout = new QGridLayout();
    g2layout->setHorizontalSpacing(5);
    g2layout->setColumnStretch(0, 1);
    g2layout->setColumnStretch(1, 1);
    g2layout->setColumnStretch(2, 9);
    v3layout->addLayout(g2layout);
    voteProcRCLabel[0] = new QLabel(tr("rc: "));
    g2layout->addWidget(voteProcRCLabel[0], /* row */0, /* col */0);
    snprintf(tmp, sizeof(tmp), "%d", rc);
    voteProcRCLabel[1] = new QLabel(tmp);
    g2layout->addWidget(voteProcRCLabel[1], /* row */0, /* col */1);

    /* col table */
    colTableView = new QTableView();
    colTableView->installEventFilter(this);
    colTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    colTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    v3layout->addWidget(colTableView);

    /* row table */
    rowTableView = new QTableView();
    rowTableView->installEventFilter(this);
    rowTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    rowTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    v3layout->addWidget(rowTableView);
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
        inputTableModel->setResolveVoteDialog(this);
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
                /* Ctrl-C: copy from the selected cells */
                QString text;
                QItemSelectionModel *selection = tableView->selectionModel();
                QModelIndexList indexes = selection->selectedIndexes();
                int prev_row = -1;
                for(int i=0; i < indexes.size(); i++) {
                    QModelIndex index = indexes.at(i);
                    if (i) {
                        char c = (index.row() != prev_row)? '\n': '\t';
                        text.append(c);
                    }
                    QVariant data = tableView->model()->data(index);
                    text.append( data.toString() );
                    prev_row = index.row();
                }
                QApplication::clipboard()->setText(text);
                return true;
            }
            else
            if ((ke->key() == Qt::Key_V)
                && (ke->modifiers().testFlag(Qt::ControlModifier))
                && (tableView == inputTableView) /* inputTableView is modifiable */
                && (vote))
            {
                /* Ctrl-V: paste into the selected cells */
                QString text = QApplication::clipboard()->text();
                QStringList lines = text.split("\n");
                QItemSelectionModel *selection = tableView->selectionModel();
                QModelIndexList indexes = selection->selectedIndexes();
                if (indexes.size() && indexes.at(0).isValid()) { 
                    int row = indexes.at(0).row();
                    int col = indexes.at(0).column();
                    for(int i=0; i < lines.size(); i++) {
                        QStringList fields = lines[i].split("\t");
                        for(int j=0; j < fields.size(); j++) {
                            if (col + j >= (int)vote->nc)
                                break;

                            std::string field = fields[j].toStdString();
                            bool isNA = (strstr(field.c_str(), "NA"))? true: false;
                            double dvalue = atof(field.c_str());
                            if (row + i == 0) /* Binary/Scalar */
                                vote->cvecs[TC_VOTE_IS_BINARY]->a[0][col+j]
                                    = (dvalue < 0.5)? 0.0: 1.0;
                            else
                            if (row + i == 1) /* Minimum */
                               ; 
                            else
                            if (row + i == 2) /* Maximum */
                               ; 
                            else
                            if (row + i < 3 + (int)vote->nr)
                               vote->M->a[row+i-3][col+j] = (isNA)? vote->NA: dvalue;
                        }
                    }
                    inputTableModel->onDataChange();
                    onInputChange();
                }
                return true;
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

void ResolveVoteDialog::onNVotersChange()
{
    char tmp[32];
    int nVoters = atoi(nVotersLineEdit->text().toStdString().c_str());

    if ((nVoters <= 0) || (nVoters == (int)vote->nr)) {
        /* bad input or no change. reset. */
        snprintf(tmp, sizeof(tmp), "%u", vote->nr);
        nVotersLineEdit->setText(QString(tmp));
    } else {
        snprintf(tmp, sizeof(tmp), "%u", nVoters);
        nVotersLineEdit->setText(QString(tmp));

        /* create new tc_vote */
        struct tc_vote *vote = tc_vote_ctr(nVoters, this->vote->nc);
        /* copy this->vote to vote */
        vote->NA = this->vote->NA;
        vote->alpha = this->vote->alpha;
        vote->tol = this->vote->tol;
        for(uint32_t i=0; i < vote->nr; i++)
            vote->rvecs[TC_VOTE_OLD_REP]->a[i][0]
                = (i < this->vote->nr)? this->vote->rvecs[TC_VOTE_OLD_REP]->a[i][0]: 0.0;
        for(uint32_t j=0; j < vote->nc; j++)
            vote->cvecs[TC_VOTE_IS_BINARY]->a[0][j]
                = (j < this->vote->nc)? this->vote->cvecs[TC_VOTE_IS_BINARY]->a[0][j]: 1.0;
        for(uint32_t i=0; i < vote->nr; i++)
            for(uint32_t j=0; j < vote->nc; j++)
                vote->M->a[i][j]
                    = ((i < this->vote->nr) && (j < this->vote->nc))? this->vote->M->a[i][j]: vote->NA;

        /* replace this->vote with vote */
        struct tc_vote *oldvote = this->vote;
        if (nVoters > (int)oldvote->nr) {
            inputTableModel->callBeginInsertRows(QModelIndex(), 3+oldvote->nr, 3+nVoters-1);
            rowTableModel->callBeginInsertRows(QModelIndex(), oldvote->nr, nVoters-1);
        } else {
            inputTableModel->callBeginRemoveRows(QModelIndex(), 3+nVoters, 3+oldvote->nr-1);
            rowTableModel->callBeginRemoveRows(QModelIndex(), nVoters, oldvote->nr-1);
        }
        this->vote = vote;
        if (nVoters > (int)oldvote->nr) {
            inputTableModel->callEndInsertRows();
            rowTableModel->callEndInsertRows();
        } else {
            inputTableModel->callEndRemoveRows();
            rowTableModel->callEndRemoveRows();
        }
        tc_vote_dtr(oldvote);

        /* recalc */
        onInputChange();
    }
}

void ResolveVoteDialog::onNDecisionsChange()
{
    char tmp[32];
    uint32_t nDecisions = atoi(nDecisionsLineEdit->text().toStdString().c_str());

    if ((nDecisions <= 0) || (nDecisions == vote->nc)) {
        /* bad input or no change. reset. */
        snprintf(tmp, sizeof(tmp), "%u", vote->nc);
        nDecisionsLineEdit->setText(QString(tmp));
    } else {
        snprintf(tmp, sizeof(tmp), "%u", nDecisions);
        nDecisionsLineEdit->setText(QString(tmp));

        /* create new tc_vote */
        struct tc_vote *vote = tc_vote_ctr(this->vote->nr, nDecisions);
        /* copy this->vote to vote */
        vote->NA = this->vote->NA;
        vote->alpha = this->vote->alpha;
        vote->tol = this->vote->tol;
        for(uint32_t i=0; i < vote->nr; i++)
            vote->rvecs[TC_VOTE_OLD_REP]->a[i][0]
                = (i < this->vote->nr)? this->vote->rvecs[TC_VOTE_OLD_REP]->a[i][0]: 0.0;
        for(uint32_t j=0; j < vote->nc; j++)
            vote->cvecs[TC_VOTE_IS_BINARY]->a[0][j]
                = (j < this->vote->nc)? this->vote->cvecs[TC_VOTE_IS_BINARY]->a[0][j]: 1.0;
        for(uint32_t i=0; i < vote->nr; i++)
            for(uint32_t j=0; j < vote->nc; j++)
                vote->M->a[i][j]
                    = ((i < this->vote->nr) && (j < this->vote->nc))? this->vote->M->a[i][j]: vote->NA;

        /* replace this->vote with vote */
        struct tc_vote *oldvote = this->vote;
        if (nDecisions > oldvote->nc) {
            inputTableModel->callBeginInsertColumns(QModelIndex(), oldvote->nc, nDecisions-1);
            colTableModel->callBeginInsertColumns(QModelIndex(), oldvote->nc, nDecisions-1);
        } else {
            inputTableModel->callBeginRemoveColumns(QModelIndex(), nDecisions, oldvote->nc-1);
            colTableModel->callBeginRemoveColumns(QModelIndex(), nDecisions, oldvote->nc-1);
        }
        this->vote = vote;
        if (nDecisions > oldvote->nc) {
            inputTableModel->callEndInsertColumns();
            colTableModel->callEndInsertColumns();
        } else {
            inputTableModel->callEndRemoveColumns();
            colTableModel->callEndRemoveColumns();
        }
        tc_vote_dtr(oldvote);

        /* recalc */
        onInputChange();
    }
}

void ResolveVoteDialog::onAlphaChange()
{
    char tmp[32];
    double alpha = atof(alphaLineEdit->text().toStdString().c_str());
    if ((alpha <= 0.0) || (alpha == vote->alpha)) {
        /* bad input or no change. reset. */
        snprintf(tmp, sizeof(tmp), "%.8f", vote->alpha);
        alphaLineEdit->setText(QString(tmp));
    } else {
        vote->alpha = alpha;
        snprintf(tmp, sizeof(tmp), "%.8f", vote->alpha);
        alphaLineEdit->setText(QString(tmp));

        /* recalc */
        onInputChange();
    }
}

void ResolveVoteDialog::onTolChange()
{
    char tmp[32];
    double tol = atof(tolLineEdit->text().toStdString().c_str());
    if ((tol <= 0.0) || (tol == vote->tol)) {
        /* bad input or no change. reset. */
        snprintf(tmp, sizeof(tmp), "%.8f", vote->tol);
        tolLineEdit->setText(QString(tmp));
    } else {
        vote->tol = tol;
        snprintf(tmp, sizeof(tmp), "%.8f", vote->tol);
        tolLineEdit->setText(QString(tmp));

        /* recalc */
        onInputChange();
    }
}

void ResolveVoteDialog::onNAChange()
{
    char tmp[32];
    double NA = atof(NALineEdit->text().toStdString().c_str());
    if ((NA <= 0.0) || (NA == vote->NA)) {
        /* bad input or no change. reset. */
        snprintf(tmp, sizeof(tmp), "%.8e", vote->NA);
        NALineEdit->setText(QString(tmp));
    } else {
        vote->NA = NA;
        snprintf(tmp, sizeof(tmp), "%.8e", vote->NA);
        NALineEdit->setText(QString(tmp));

        /* recalc */
        onInputChange();
    }
}

void ResolveVoteDialog::onInputChange(void)
{
    vote_proc_rc = tc_vote_proc(vote);
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", vote_proc_rc);
    voteProcRCLabel[1]->setText(tmp);
    colTableModel->onVoteChange(TC_VOTE_NCOLS, vote->nc);
    rowTableModel->onVoteChange(vote->nr, TC_VOTE_NROWS);

    v4layout->removeWidget(scrollArea);
    delete scrollArea;

    resolveVoteGraph = new ResolveVoteGraph();
    resolveVoteGraph->setVotePtr((const struct tc_vote **)&vote);
    resolveVoteGraph->setMinimumHeight(2*20 + 50*vote->nc);
    scrollArea = new QScrollArea();
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(resolveVoteGraph);
    v4layout->addWidget(scrollArea);
    QVBoxLayout *v5layout = new QVBoxLayout(scrollArea);
    v5layout->addWidget(resolveVoteGraph);
}

