// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTabWidget>
#include <QVBoxLayout>

#include "ballotbranchwindow.h"
#include "ballotballotwindow.h"
#include "ballotoutcomewindow.h"
#include "ballotoutcometablemodel.h"
#include "ballotsealedvotewindow.h"
#include "ballotvotewindow.h"
#include "ballotvotetablemodel.h"
#include "ballotview.h"
#include "chain.h"
#include "guiutil.h"
#include "main.h"
#include "marketbranchtablemodel.h"
#include "primitives/market.h"
#include "txdb.h"
#include "walletmodel.h"


extern CMarketTreeDB *pmarkettree;
extern CChain chainActive;

const int col0 = 0;
const int col1 = 1;

static QString formatUint256(const uint256 &hash)
{
    return QString::fromStdString(hash.ToString());
}

#if 0
QWidget *scrollWidget = NULL;
QScrollArea *scrollArea = NULL;
#endif

#if 0
bool branchptrcmp(const marketBranch *aptr, const marketBranch *bptr)
{
    return (aptr->name.compare(bptr->name) <= 0);
}
#endif

BallotView::BallotView(QWidget *parent)
    : QWidget(parent), model(0), branch(0)
{
    /* vlayout                        */
    /*    Grid Layout                 */
    /*    Horizontal Layout           */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    QGridLayout *glayout = new QGridLayout();
    vlayout->addLayout(glayout);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addLayout(hlayout);


    /* Grid Layout */

    /* create widgets */
    branchLabels[0] = new QLabel(tr("Branch: "));
    branchLabels[0]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    branchLabels[0]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    branchLabels[1] = new QLabel("");
    branchLabels[1]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    branchLabels[1]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ballotLabels[0] = new QLabel(tr("Block Number: "));
    ballotLabels[0]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    ballotLabels[0]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ballotLabels[1] = new QLabel("");
    ballotLabels[1]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    ballotLabels[1]->setTextInteractionFlags(Qt::TextSelectableByMouse);

    /* add to grid layout */
    /* Branch    label0   label1   */
    /* Ballot    label0   label1   */
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 1);
    glayout->setColumnStretch(2, 9);
    glayout->addWidget(branchLabels[0], /* row */0, col0);
    glayout->addWidget(branchLabels[1], /* row */0, col1);
    glayout->addWidget(ballotLabels[0], /* row */1, col0);
    glayout->addWidget(ballotLabels[1], /* row */1, col1);

    /* Horizontal Layout */
    /* Select     Create */
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);
    QGroupBox *groupbox1 = new QGroupBox(tr("Select"));
    QVBoxLayout *gb1layout = new QVBoxLayout(groupbox1);
    hlayout->addWidget(groupbox1);
    QGroupBox *groupbox2 = new QGroupBox(tr("Create"));
    QVBoxLayout *gb2layout = new QVBoxLayout(groupbox2);
    hlayout->addWidget(groupbox2);

    /* Select Tabs */
    QTabWidget *tabs1 = new QTabWidget();
    QWidget *page1 = new QWidget();
    tabs1->addTab(page1, tr("Branch"));
    initSelectBranchTab(page1);
    QWidget *page2 = new QWidget();
    tabs1->addTab(page2, tr("Ballot"));
    initSelectBallotTab(page2);
    QWidget *page3 = new QWidget();
    tabs1->addTab(page3, tr("Sealed Vote"));
    initSelectSealedVoteTab(page3);
    QWidget *page4 = new QWidget();
    tabs1->addTab(page4, tr("Vote"));
    initSelectVoteTab(page4);
    QWidget *page5 = new QWidget();
    tabs1->addTab(page5, tr("Outcomes"));
    initSelectOutcomeTab(page5);
    gb1layout->addWidget(tabs1);

    /* Create Tabs */
    QTabWidget *tabs2 = new QTabWidget();
    QWidget *page6 = new QWidget();
    tabs2->addTab(page6, tr("Sealed Vote"));
    initCreateSealedVoteTab(page6);
    QWidget *page7 = new QWidget();
    tabs2->addTab(page7, tr("Vote"));
    initCreateVoteTab(page7);
    gb2layout->addWidget(tabs2);


#if 0
    /*    Vertical layout v1 branch/blocknum */
    /*    Vertical layout v2 decisions       */
    /*    Vertical layout v3 submit button   */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QVBoxLayout *v1layout = new QVBoxLayout();
    vlayout->addLayout(v1layout, 0);
    v2layout = new QVBoxLayout();
    vlayout->addLayout(v2layout, 10); /* receives all the spacing */
    QVBoxLayout *v3layout = new QVBoxLayout();
    vlayout->addLayout(v3layout, 0);

    /* add grid layout to v1       */
    /* Branch    label   combobox  */
    /* Block Num label   line edit */
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    glayout->setRowStretch(0, 0);
    branchLabel = new QLabel(tr("Branch: "));
    glayout->addWidget(branchLabel, /* row */0, /* col */0);
    branchWidget = new QComboBox();
    glayout->addWidget(branchWidget, /* row */0, /* col */1);

    glayout->setRowStretch(1, 0);
    blockNumLabel = new QLabel(tr("Block Number: "));
    glayout->addWidget(blockNumLabel, /* row */1, /* col */0);
    blockNumWidget = new QLineEdit();
    glayout->addWidget(blockNumWidget, /* row */1, /* col */1);

    glayout->setRowStretch(2, 0);
    minBlockNumLabel = new QLabel(tr("Min Block Number: "));
    glayout->addWidget(minBlockNumLabel, /* row */2, /* col */0);
    minBlockNum = new QLabel(tr(""));
    glayout->addWidget(minBlockNum, /* row */2, /* col */1);

    glayout->setRowStretch(3, 0);
    maxBlockNumLabel = new QLabel(tr("Max Block Number: "));
    glayout->addWidget(maxBlockNumLabel, /* row */3, /* col */0);
    maxBlockNum = new QLabel(tr(""));
    glayout->addWidget(maxBlockNum, /* row */3, /* col */1);

    glayout->setRowStretch(4, 0);
    currHeightLabel = new QLabel(tr("Current Height: "));
    glayout->addWidget(currHeightLabel, /* row */4, /* col */0);
    currHeight = new QLabel(tr(""));
    glayout->addWidget(currHeight, /* row */4, /* col */1);

    glayout->setRowStretch(5, 10); /* receives the stretch */
    glayout->addWidget(new QLabel(tr("")), /* row */5, /* col */0);
    v1layout->addLayout(glayout);

    /* add scroll area to v2 */
    scrollWidget = new QWidget();
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(false);
    scrollArea->setWidget(scrollWidget);
    v2layout->addWidget(scrollArea);

    /* add button to v3 */
    submitButton = new QPushButton(tr("Submit Ballot"));
    submitButton->setFixedWidth(121);
    v3layout->addWidget(submitButton);

    branches = pmarkettree->GetBranches();
    std::sort(branches.begin(), branches.end(), branchptrcmp);
    for(uint32_t i=0; i < branches.size(); i++)
        branchWidget->addItem(branches[i]->name.c_str(), i);

    connect(branchWidget, SIGNAL(activated(int)), this, SLOT(changedBranch(int)));
    connect(blockNumWidget, SIGNAL(textChanged(QString)), this, SLOT(changedBlock(QString)));

    blocknum = chainActive.Height();
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", blocknum);
    blockNumWidget->setText( QString(tmp) );

    if (branches.size())
        changedBranch(0);
#endif
}


void BallotView::setModel(WalletModel *model)
{
    this->model = model;

    /* branch is last. */
    voteWindow->setModel(model);
    sealedVoteWindow->setModel(model);
    ballotWindow->setModel(model);
    outcomeWindow->setModel(model);
    branchWindow->setModel(model);
}

void BallotView::onBallotChange(unsigned int blockNum)
{
   this->ballotNum = blockNum;
   if (blockNum == (unsigned int)(-1)) {
        ballotLabels[1]->setText("");

        for(uint32_t i=0; i < BALLOTBALLOT_NLABLES; i++)
           ballotTabLabels[i].setText("");
   } else {
        char tmp[32];
        /* Decisions: */
        sprintf(tmp, "%u - %u", blockNum - branch->tau + 1, blockNum);
        ballotLabels[0]->setText( QString(tmp) );

        /* Sealed Ballots: */
        sprintf(tmp, "<= %u", blockNum + branch->ballotTime);
        ballotLabels[0]->setText( QString(tmp) );

        /* Unsealed Ballots: */
        sprintf(tmp, "<= %u", blockNum + branch->ballotTime + branch->unsealTime);
        ballotLabels[0]->setText( QString(tmp) );

        /* Outcome: */
        sprintf(tmp, "%u", blockNum + branch->ballotTime + branch->unsealTime + 1);
        ballotLabels[0]->setText( QString(tmp) );
   }
   
    /* propagate */
    // outcomeWindow->onBallotChange(branch, blockNum);
    // voteWindow->onBallotChange(branch, blockNum);
    // sealedVoteWindow->onBallotChange(branch, blockNum);
}

void BallotView::onBranchChange(const marketBranch *branch)
{
    this->branch = branch;

    if (!branch) {
        branchLabels[1]->setText("");
//        decisionBranchLabel->setText("");
//        marketBranchLabel->setText("");
//        tradeBranchLabel->setText("");

        for(uint32_t i=0; i < BALLOTBRANCH_NLABLES; i++)
           branchTabLabels[i].setText("");
    } else {
        branchLabels[1]->setText( QString::fromStdString(branch->name) );
//        decisionBranchLabel->setText( QString::fromStdString(branch->name) );
//        marketBranchLabel->setText( QString::fromStdString(branch->name) );
//        tradeBranchLabel->setText( QString::fromStdString(branch->name) );

        branchTabLabels[0].setText( formatName(branch) );
        branchTabLabels[1].setText( formatDescription(branch) );
        branchTabLabels[2].setText( formatBaseListingFee(branch) );
        branchTabLabels[3].setText( formatFreeDecisions(branch) );
        branchTabLabels[4].setText( formatTargetDecisions(branch) );
        branchTabLabels[5].setText( formatMaxDecisions(branch) );
        branchTabLabels[6].setText( formatMinTradingFee(branch) );
        branchTabLabels[7].setText( formatTau(branch) );
        branchTabLabels[8].setText( formatBallotTime(branch) );
        branchTabLabels[9].setText( formatUnsealTime(branch) );
        branchTabLabels[10].setText( formatConsensusThreshold(branch) );
        branchTabLabels[11].setText( formatHash(branch) );
        branchTabLabels[12].setText( formatUint256(branch->txid) );
    }

    /* propagate */
/*
    ballotWindow->onBranchChange(branch);
*/
    outcomeWindow->onBranchChange(branch);

    /* update CLIs */
//    updateCreateDecisionCLI();
//    updateCreateMarketCLI();
//    updateCreateTradeCLI();
}

void BallotView::onOutcomeChange(const marketOutcome *outcome)
{
    if (!outcome) {
        for(uint32_t i=0; i < BALLOTOUTCOME_NLABLES; i++)
           outcomeTabLabels[i].setText("");
    } else {
        outcomeTabLabels[0].setText( formatHeight(outcome) );
        outcomeTabLabels[1].setText( formatHash(outcome) );
    }

    /* delete widgets from outcomeLayout */
    for(uint32_t i=0; i < BALLOT_NOUTCOMEGRIDLAYOUTS; i++) {

        QLayoutItem *child;
        while ((child=outcomeLayouts[i]->takeAt(0)) != 0) {
            delete child->widget();
            delete child;
        }
    }

    if (!outcome)
        return;

    QGridLayout *glayout;
    QLabel *label;
    char tmp[32];
    uint32_t row;

    /* outcomeLayouts[0]: main params */
    glayout = outcomeLayouts[0];
    row = 0;

    snprintf(tmp, sizeof(tmp), "%s", "nVoters:");
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    glayout->addWidget(label, row, col0);
    snprintf(tmp, sizeof(tmp), "%u", outcome->nVoters);
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignRight|Qt::AlignTop);
    glayout->addWidget(label, row, col1);
    row++;

    snprintf(tmp, sizeof(tmp), "%s", "nDecisions:");
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    glayout->addWidget(label, row, col0);
    snprintf(tmp, sizeof(tmp), "%u", outcome->nDecisions);
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignRight|Qt::AlignTop);
    glayout->addWidget(label, row, col1);

    row++;
    snprintf(tmp, sizeof(tmp), "%s", "NA");
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    glayout->addWidget(label, row, col0);
    snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*outcome->NA);
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignRight|Qt::AlignTop);
    glayout->addWidget(label, row, col1);
    row++;

    snprintf(tmp, sizeof(tmp), "%s", "alpha");
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    glayout->addWidget(label, row, col0);
    snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*outcome->alpha);
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignRight|Qt::AlignTop);
    glayout->addWidget(label, row, col1);
    row++;

    snprintf(tmp, sizeof(tmp), "%s", "tol");
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    glayout->addWidget(label, row, col0);
    snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*outcome->tol);
    label = new QLabel(QString(tmp));
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setAlignment(Qt::AlignRight|Qt::AlignTop);
    glayout->addWidget(label, row, col1);
    row++;

    /* outcomeLayouts[1]: voterIDs */
    glayout = outcomeLayouts[1];
    row = 0;
    for(uint32_t i=0; i < outcome->voterIDs.size(); i++) {
        snprintf(tmp, sizeof(tmp), "Voter %d:", i);
        label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        glayout->addWidget(label, row, col0);
        snprintf(tmp, sizeof(tmp), "%s", outcome->voterIDs[i].ToString().c_str());
        label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col1);
        row++;
    }

    /* outcomeLayouts[2]: decisionIDs */
    glayout = outcomeLayouts[2];
    row = 0;
    for(uint32_t i=0; i < outcome->decisionIDs.size(); i++) {
        snprintf(tmp, sizeof(tmp), "Decision %d:", i);
        label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        glayout->addWidget(label, row, col0);
        snprintf(tmp, sizeof(tmp), "%s", outcome->decisionIDs[i].ToString().c_str());
        label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col1);
        row++;
    }

    /* outcomeLayouts[3]: vote matrix */
    glayout = outcomeLayouts[3];
    row = 0;
    for(uint32_t j=0; j < outcome->nDecisions; j++) { 
        snprintf(tmp, sizeof(tmp), "Decision %u", j);
        QLabel *label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col0 + 1 + j);
    }
    row++;
    for(uint32_t i=0; i < outcome->nVoters; i++) { 
        snprintf(tmp, sizeof(tmp), "Voter %u", i);
        QLabel *label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col0);
        for(uint32_t j=0; j < outcome->nDecisions; j++) { 
            uint64_t value = outcome->voteMatrix[i*outcome->nDecisions + j];
            if (value == outcome->NA)
               snprintf(tmp, sizeof(tmp), "NA");
            else
               snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*value);
            QLabel *label = new QLabel(QString(tmp));
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            label->setAlignment(Qt::AlignRight|Qt::AlignTop);
            glayout->addWidget(label, row, col0 + 1 + j);
        }
        row++;
    }

    /* outcomeLayouts[4]: row vectors */
    glayout = outcomeLayouts[4];
    row = 0;
    const char *rowLabels[9] = {"Is Scaled", "First Loading", "Decision Raw", "Consensus",
	"Certainty", "NA Col", "Partic Col", "Author Bonus", "Decision Final"};
    const vector<uint64_t> *rvecs[9];
    rvecs[0] = &outcome->isScaled;
    rvecs[1] = &outcome->firstLoading;
    rvecs[2] = &outcome->decisionsRaw;
    rvecs[3] = &outcome->consensusReward;
    rvecs[4] = &outcome->certainty;
    rvecs[5] = &outcome->NACol;
    rvecs[6] = &outcome->particCol;
    rvecs[7] = &outcome->authorBonus;
    rvecs[8] = &outcome->decisionsFinal;
    for(uint32_t j=0; j < outcome->nDecisions; j++) { 
        snprintf(tmp, sizeof(tmp), "Decision %u", j);
        QLabel *label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col0 + 1 + j);
    }
    row++;
    for(uint32_t i=0; i < 9; i++) { 
        QLabel *label = new QLabel(tr(rowLabels[i]));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col0);
        for(uint32_t j=0; j < rvecs[i]->size(); j++) { 
            snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*rvecs[i]->at(j));
            QLabel *label = new QLabel(QString(tmp));
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            label->setAlignment(Qt::AlignRight|Qt::AlignTop);
            glayout->addWidget(label, row, col0 + 1 + j);
        }
        row++;
    }

    /* outcomeLayouts[5]: column vectors */
    glayout = outcomeLayouts[5];
    row = 0;
    const char *colLabels[7] = {"Old Rep", "This Rep", "Smooth Rep", "NA",
	"Partic Row", "Partic Rel", "Row Bonus"};
    const vector<uint64_t> *cvecs[7];
    cvecs[0] = &outcome->oldRep;
    cvecs[1] = &outcome->thisRep;
    cvecs[2] = &outcome->smoothedRep;
    cvecs[3] = &outcome->NARow;
    cvecs[4] = &outcome->particRow;
    cvecs[5] = &outcome->particRel;
    cvecs[6] = &outcome->rowBonus;
    for(uint32_t j=0; j < 7; j++) { 
        QLabel *label = new QLabel(tr(colLabels[j]));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        glayout->addWidget(label, row, col0 + 1 + j);
    }
    row++;
    for(uint32_t i=0; i < outcome->nVoters; i++) { 
        snprintf(tmp, sizeof(tmp), "Voter %u", i);
        QLabel *label = new QLabel(QString(tmp));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setAlignment(Qt::AlignRight|Qt::AlignTop);
        glayout->addWidget(label, row, col0);
        for(uint32_t j=0; j < 7; j++) { 
            if (i < cvecs[j]->size())
               snprintf(tmp, sizeof(tmp), "%.8f", 1e-8*cvecs[j]->at(i));
            else
               snprintf(tmp, sizeof(tmp), "%s", "--------");
            QLabel *label = new QLabel(QString(tmp));
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            label->setAlignment(Qt::AlignRight|Qt::AlignTop);
            glayout->addWidget(label, row, col0 + 1 + j);
        }
        row++;
    }
}

void BallotView::onSealedVoteChange(const marketSealedVote *sealedVote)
{

}

void BallotView::onVoteChange(const marketVote *vote)
{

}

void BallotView::initSelectBranchTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[BALLOTBRANCH_NLABLES] = {
        "Name: ",
        "Description:",
        "Base Listing Fee:",
        "Free Decisions:",
        "Target Decisions:",
        "Max Decisions:",
        "Min Trading Fee:",
        "Tau:",
        "Ballot Time:",
        "Unseal Time:",
        "Consensus Threshold:",
        "Hash: ",
        "In Tx: ",
    };

    for(uint32_t i=0; i < BALLOTBRANCH_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &branchTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    branchButton = new QPushButton(tr("Select"));
    branchWindow = new BallotBranchWindow(this);
    connect(branchButton, SIGNAL(clicked()), this, SLOT(showBranchWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(branchButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the stretch */
}

void BallotView::initSelectBallotTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[BALLOTBALLOT_NLABLES] = {
        "Decisions: ",
        "Sealed Ballots:",
        "Unsealed Ballots:",
        "Outcome:",
    };

    for(uint32_t i=0; i < BALLOTBALLOT_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &ballotTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    ballotButton = new QPushButton(tr("Select"));
    ballotWindow = new BallotBallotWindow(this);
    connect(ballotButton, SIGNAL(clicked()), this, SLOT(showBallotWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(ballotButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the stretch */
}

void BallotView::initSelectSealedVoteTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[BALLOTSEALEDVOTE_NLABLES] = {
        "Name: ",
        "Description:",
        "Base Listing Fee:",
        "Free Decisions:",
        "Target Decisions:",
        "Max Decisions:",
        "Min Trading Fee:",
        "Tau:",
        "Ballot Time:",
        "Unseal Time:",
        "Consensus Threshold:",
        "Hash: ",
        "In Tx: ",
    };

    for(uint32_t i=0; i < BALLOTSEALEDVOTE_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &sealedVoteTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    sealedVoteButton = new QPushButton(tr("Select"));
    sealedVoteWindow = new BallotSealedVoteWindow(this);
    connect(sealedVoteButton, SIGNAL(clicked()), this, SLOT(showSealedVoteWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(sealedVoteButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the stretch */
}

void BallotView::initSelectVoteTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[BALLOTVOTE_NLABLES] = {
        "Name: ",
        "Description:",
        "Base Listing Fee:",
        "Free Decisions:",
        "Target Decisions:",
        "Max Decisions:",
        "Min Trading Fee:",
        "Tau:",
        "Ballot Time:",
        "Unseal Time:",
        "Consensus Threshold:",
        "Hash: ",
        "In Tx: ",
    };

    for(uint32_t i=0; i < BALLOTVOTE_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &voteTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    voteButton = new QPushButton(tr("Select"));
    voteWindow = new BallotVoteWindow(this);
    connect(voteButton, SIGNAL(clicked()), this, SLOT(showVoteWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(voteButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the stretch */
}

void BallotView::initSelectOutcomeTab(QWidget *page)
{
    for(uint32_t i=0; i < BALLOT_NOUTCOMEGRIDLAYOUTS; i++)
        outcomeLayouts[i] = 0;

    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[BALLOTOUTCOME_NLABLES] = {
        "Height: ",
        "Hash:",
    };

    for(uint32_t i=0; i < BALLOTOUTCOME_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &outcomeTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    outcomeButton = new QPushButton(tr("Select"));
    outcomeWindow = new BallotOutcomeWindow(this);
    connect(outcomeButton, SIGNAL(clicked()), this, SLOT(showOutcomeWindow()));

    QWidget *outcomeWidget = new QWidget();
    QVBoxLayout *outcomeLayout = new QVBoxLayout(outcomeWidget);
    outcomeLayout->setContentsMargins(0,0,0,0);
    outcomeLayout->setSpacing(10);
    for(uint32_t i=0; i < BALLOT_NOUTCOMEGRIDLAYOUTS; i++) {
        QGridLayout *glayout = new QGridLayout();
        if (i < 3)
           glayout->setColumnStretch(3, 10); /* receives the stretch */
        glayout->setContentsMargins(0,0,0,0);
        glayout->setHorizontalSpacing(4);
        glayout->setVerticalSpacing(2);
        outcomeLayout->addLayout(glayout, 0);
        /* record */
        outcomeLayouts[i] = glayout;
    }
    outcomeLayout->addLayout(new QGridLayout(), 10); /* add one more to receive the stretch */

    QScrollArea *outcomeScrollArea = new QScrollArea();
    outcomeScrollArea->setWidgetResizable(true);
    outcomeScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    outcomeScrollArea->setWidget(outcomeWidget);
    QVBoxLayout *outcomeScrollAreaLayout = new QVBoxLayout();
    outcomeScrollAreaLayout->addWidget(outcomeScrollArea, 1);

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(outcomeButton, 0);
    vlayout->addLayout(outcomeScrollAreaLayout, 10); /* receives all the stretch */

    /* fill in outcomeLayouts[] */
    onOutcomeChange(0);
}

void BallotView::initCreateSealedVoteTab(QWidget *page)
{

}

void BallotView::initCreateVoteTab(QWidget *page)
{

}

#if 0
void BallotView::refresh(void)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%8u", chainActive.Height());
    currHeight->setText( QString(tmp) );

    if (scrollWidget) {
        v2layout->removeWidget(scrollWidget);
        delete scrollWidget;
        scrollWidget = 0;
    }

    if (!branch || !blocknum) {
        minblock = 0;
        minBlockNum->setText(QString(""));

        maxblock = 0;
        maxBlockNum->setText(QString(""));
    } else {
        minblock = branch->tau * ((blocknum - 1) / branch->tau) + 1;
        snprintf(tmp, sizeof(tmp), "%8u", minblock);
        minBlockNum->setText( QString(tmp) );

        maxblock = minblock + branch->tau - 1;
        snprintf(tmp, sizeof(tmp), "%8u", maxblock);
        maxBlockNum->setText( QString(tmp) );

        QWidget *scrollWidget = new QWidget();
        QGridLayout *scrollLayout = new QGridLayout(scrollWidget);

        /* rebuild scrollLayout */
        uint32_t row = 0;
        vector<marketDecision *> vec = pmarkettree->GetDecisions(branch->GetHash());
        for(size_t i=0; i < vec.size(); i++) {
            const marketDecision *obj = vec[i];
            if ((obj->eventOverBy < minblock)
                || (obj->eventOverBy > maxblock))
                continue;

            QLabel *nameLabel = new QLabel( QString::fromStdString(obj->prompt) );
            scrollLayout->addWidget(nameLabel, row, 0);

            QString isOptional = (obj->answerOptionality)? tr("not optional"): tr("is optional");
            QLabel *answerOptionality = new QLabel(isOptional);
            scrollLayout->addWidget(answerOptionality, row, 1);

            QString isScaled = (obj->isScaled)? tr("is scaled"): tr("is binary");
            QLabel *scaled = new QLabel(isScaled);
            scrollLayout->addWidget(scaled, row, 2);

            if (!obj->isScaled) {
                QRadioButton *zero = new QRadioButton(tr("No"));
                QRadioButton *one = new QRadioButton(tr("Yes"));
                QRadioButton *NA = new QRadioButton(tr("N/A"));
                NA->setChecked(true);
                QButtonGroup *buttonGroup = new QButtonGroup();
                buttonGroup->addButton(zero);
                buttonGroup->addButton(one);
                buttonGroup->addButton(NA);

                scrollLayout->addWidget(zero, row, 3);
                scrollLayout->addWidget(one, row, 4);
                scrollLayout->addWidget(NA, row, 5);
            }

            row++;
        }
        for(size_t i=0; i < vec.size(); i++)
            delete vec[i];

        scrollArea->setWidget(scrollWidget);
        v2layout->addWidget(scrollArea);
    }
}

void BallotView::changedBranch(int i)
{
    branch = ((i < 0) || ((unsigned int)i >= branches.size()))? 0: branches[i];
    refresh();
}

void BallotView::changedBlock(const QString &str)
{
    blocknum = str.toInt();
    refresh();
}
#endif

void BallotView::showBranchWindow(void)
{
    if (!branchWindow)
        return;
    branchWindow->show();
    branchWindow->raise();
    branchWindow->setFocus();
    branchWindow->setTableViewFocus();
}

void BallotView::showBallotWindow(void)
{
    if (!ballotWindow)
        return;
    ballotWindow->show();
    ballotWindow->raise();
    ballotWindow->setFocus();
}

void BallotView::showOutcomeWindow(void)
{
    if (!outcomeWindow)
        return;
    outcomeWindow->show();
    outcomeWindow->raise();
    outcomeWindow->setFocus();
}

void BallotView::showSealedVoteWindow(void)
{
    if (!sealedVoteWindow)
        return;
    sealedVoteWindow->show();
    sealedVoteWindow->raise();
    sealedVoteWindow->setFocus();
}

void BallotView::showVoteWindow(void)
{
    if (!voteWindow)
        return;
    voteWindow->show();
    voteWindow->raise();
    voteWindow->setFocus();
}

