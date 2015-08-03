// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketview.h"

#include "csvmodelwriter.h"
#include "guiutil.h"
#include "json/json_spirit_writer_template.h"
#include "marketbranchwindow.h"
#include "marketbranchtablemodel.h"
#include "marketdecisionwindow.h"
#include "marketdecisiontablemodel.h"
#include "marketmarketwindow.h"
#include "marketmarkettablemodel.h"
#include "markettradewindow.h"
#include "markettradetablemodel.h"
#include "marketviewgraph.h"
#include "optionsmodel.h"
#include "primitives/market.h"
#include "scicon.h"
#include "truthcoinunits.h"
#include "ui_interface.h"
#include "walletmodel.h"

#include <QComboBox>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSignalMapper>
#include <QTabWidget>
#include <QVBoxLayout>

using namespace json_spirit;

const int col0 = 0;
const int col1 = 1;

static QString formatUint256(const uint256 &hash)
{
    return QString::fromStdString(hash.ToString());
}

MarketView::MarketView(QWidget *parent)
    : QWidget(parent), model(0), branch(0), decision(0), market(0),
      graphWidget(0)
{
    /* vlayout:                       */
    /*    Grid layout                 */
    /*    Horizontal layout           */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    QGridLayout *glayout = new QGridLayout();
    vlayout->addLayout(glayout);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addLayout(hlayout);

    decisionBranchLabel = new QLabel("");
    decisionBranchLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    marketBranchLabel = new QLabel("");
    marketBranchLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    marketDecisionLabel = new QLabel("");
    marketDecisionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    marketDecisionFunction = new QComboBox();
    for(int i=1; ; i++) {
        std::string str = decisionFunctionIDToString(i);
        if (!str.size())
            break;
        marketDecisionFunction->addItem(QString::fromStdString(str), QVariant(i));
    }
    marketDecisionFunction->setCurrentIndex(0);
    connect(marketDecisionFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(onMarketDecisionFunctionIndexChanged(int)));


    tradeBranchLabel = new QLabel("");
    tradeBranchLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    tradeDecisionLabel = new QLabel("");
    tradeDecisionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    tradeMarketLabel = new QLabel("");
    tradeMarketLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    /* Grid Layout */ 

    /* Branch labels and button and popup window */
    branchLabels[0] = new QLabel(tr("Branch: "));
    branchLabels[0]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    branchLabels[0]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    branchLabels[1] = new QLabel("");
    branchLabels[1]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    branchLabels[1]->setTextInteractionFlags(Qt::TextSelectableByMouse);

    /* Decision labels and button and popup window */
    decisionLabels[0] = new QLabel(tr("Decision: "));
    decisionLabels[0]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    decisionLabels[0]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    decisionLabels[1] = new QLabel("");
    decisionLabels[1]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    decisionLabels[1]->setTextInteractionFlags(Qt::TextSelectableByMouse);

    /* Market labels and button and popup window */
    marketLabels[0] = new QLabel(tr("Market: "));
    marketLabels[0]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    marketLabels[0]->setTextInteractionFlags(Qt::TextSelectableByMouse);
    marketLabels[1] = new QLabel("");
    marketLabels[1]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    marketLabels[1]->setTextInteractionFlags(Qt::TextSelectableByMouse);

    /* add to grid layout */ 
    /* Branch    label0   label1   */
    /* Decision  label0   label1   */
    /* Market    label0   label1   */
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);
    glayout->addWidget(branchLabels[0], /* row */0, col0);
    glayout->addWidget(branchLabels[1], /* row */0, col1);
    glayout->addWidget(decisionLabels[0], /* row */1, col0);
    glayout->addWidget(decisionLabels[1], /* row */1, col1);
    glayout->addWidget(marketLabels[0], /* row */2, col0);
    glayout->addWidget(marketLabels[1], /* row */2, col1);

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
    QWidget *page5 = new QWidget();
    tabs1->addTab(page5, tr("Branch"));
    initSelectBranchTab(page5);
    QWidget *page6 = new QWidget();
    tabs1->addTab(page6, tr("Decision"));
    initSelectDecisionTab(page6);
    QWidget *page7 = new QWidget();
    tabs1->addTab(page7, tr("Market"));
    initSelectMarketTab(page7);
    QWidget *page8 = new QWidget();
    tabs1->addTab(page8, tr("Trade"));
    initSelectTradeTab(page8);
    graphWidget = new MarketViewGraph();
    QWidget *page9 = (QWidget *) graphWidget;
    tabs1->addTab(page9, tr("Graph"));
    gb1layout->addWidget(tabs1);

    /* Create Tabs */
    QTabWidget *tabs2 = new QTabWidget();
    QWidget *page0 = new QWidget();
    tabs2->addTab(page0, tr("Branch"));
    initCreateBranchTab(page0);
    QWidget *page1 = new QWidget();
    tabs2->addTab(page1, tr("Decision"));
    initCreateDecisionTab(page1);
    QWidget *page2 = new QWidget();
    tabs2->addTab(page2, tr("Market"));
    initCreateMarketTab(page2);
    QWidget *page3 = new QWidget();
    tabs2->addTab(page3, tr("Trade"));
    initCreateTradeTab(page3);
    gb2layout->addWidget(tabs2);
}

void MarketView::initSelectBranchTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETBRANCH_NLABLES] = {
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

    for(uint32_t i=0; i < MARKETBRANCH_NLABLES; i++) {
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
    branchWindow = new MarketBranchWindow(this);
    connect(branchButton, SIGNAL(clicked()), this, SLOT(showBranchWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(branchButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the strectch */
}

void MarketView::initSelectDecisionTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETDECISION_NLABLES] = {
        "Prompt: ",
        "Address: ",
        "Event Over By: ",
        "Is Scaled?: ",
        "Minimum: ",
        "Maximum: ",
        "Answer Is Optional: ",
        "Branch: ",
        "Hash: ",
        "In Tx: ",
    };

    for(uint32_t i=0; i < MARKETDECISION_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &decisionTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    decisionButton = new QPushButton(tr("Select"));
    decisionWindow = new MarketDecisionWindow(this);
    connect(decisionButton, SIGNAL(clicked()), this, SLOT(showDecisionWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(decisionButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the strectch */
}

void MarketView::initSelectMarketTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETMARKET_NLABLES] = {
        "Title: ",
        "Description: ",
        "Address: ",
        "Liquidity Parameter: ",
        "Trading Fee: ",
        "Tags: ",
        "Maturation: ",
        "Decision IDs: ",
        "Hash: ",
        "In Tx: ",
    };

    for(uint32_t i=0; i < MARKETMARKET_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &marketTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, col1);
    }

    marketButton = new QPushButton(tr("Select"));
    marketWindow = new MarketMarketWindow(this);
    connect(marketButton, SIGNAL(clicked()), this, SLOT(showMarketWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(marketButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the strectch */
}

void MarketView::initSelectTradeTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETTRADE_NLABLES] = {
        "Address: ",
        "Buy/Sell:",
        "Number of Shares:",
        "Price per Share:",
        "Decision State:",
        "Nonce:",
        "Hash:",
        "In Tx:",
    };

    for(uint32_t i=0; i < MARKETTRADE_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, col0);

       QLabel *value = &tradeTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       glayout->addWidget(value, /* row */i, col1);
    }

    tradeButton = new QPushButton(tr("Show All Trades"));
    tradeWindow = new MarketTradeWindow(this);
    connect(tradeButton, SIGNAL(clicked()), this, SLOT(showTradeWindow()));

    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(2);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(tradeButton, 0);
    vlayout->addWidget(new QWidget(), 10); /* receives all the strectch */
}

void MarketView::showBranchWindow(void)
{
    if (!branchWindow)
        return;
    branchWindow->show();
    branchWindow->raise();
    branchWindow->setFocus();
    branchWindow->setTableViewFocus();
}

void MarketView::initCreateBranchTab(QWidget *page)
{
    /* widgets */
    branchName = new QLineEdit();
    branchName->setText("Branch Name");
    branchName->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(branchName, SIGNAL(textChanged(QString)), this, SLOT(onBranchNameTextChanged(QString)));
    branchDescription = new QLineEdit();
    branchDescription->setText("Branch Description");
    branchDescription->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(branchDescription, SIGNAL(textChanged(QString)), this, SLOT(onBranchDescriptionTextChanged(QString)));
    branchBaseListingFee = new QLineEdit();
    branchBaseListingFee->setText("0.01");
    branchBaseListingFee->setValidator( new QDoubleValidator(0.0, 1.0, 8, this) );
    branchBaseListingFee->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchBaseListingFee, SIGNAL(textChanged(QString)), this, SLOT(onBranchBaseListingFeeTextChanged(QString)));
    branchFreeDecisions = new QLineEdit();
    branchFreeDecisions->setText("10");
    branchFreeDecisions->setValidator( new QIntValidator(0, 1000, this) );
    branchFreeDecisions->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchFreeDecisions, SIGNAL(textChanged(QString)), this, SLOT(onBranchFreeDecisionsTextChanged(QString)));
    branchTargetDecisions = new QLineEdit();
    branchTargetDecisions->setText("20");
    branchTargetDecisions->setValidator( new QIntValidator(0, 1000, this) );
    branchTargetDecisions->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchTargetDecisions, SIGNAL(textChanged(QString)), this, SLOT(onBranchTargetDecisionsTextChanged(QString)));
    branchMaxDecisions = new QLineEdit();
    branchMaxDecisions->setText("30");
    branchMaxDecisions->setValidator( new QIntValidator(0, 1000, this) );
    branchMaxDecisions->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchMaxDecisions, SIGNAL(textChanged(QString)), this, SLOT(onBranchMaxDecisionsTextChanged(QString)));
    branchMinTradingFee = new QLineEdit();
    branchMinTradingFee->setText("0.01");
    branchMinTradingFee->setValidator( new QDoubleValidator(0.0, 1.0, 8, this) );
    branchMinTradingFee->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchMinTradingFee, SIGNAL(textChanged(QString)), this, SLOT(onBranchMinTradingFeeTextChanged(QString)));
    branchTau = new QLineEdit();
    branchTau->setText("1000");
    branchTau->setValidator( new QIntValidator(0, 1000, this) );
    branchTau->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchTau, SIGNAL(textChanged(QString)), this, SLOT(onBranchTauTextChanged(QString)));
    branchBallotTime = new QLineEdit();
    branchBallotTime->setText("100");
    branchBallotTime->setValidator( new QIntValidator(0, 1000, this) );
    branchBallotTime->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchBallotTime, SIGNAL(textChanged(QString)), this, SLOT(onBranchBallotTimeTextChanged(QString)));
    branchUnsealTime = new QLineEdit();
    branchUnsealTime->setText("100");
    branchUnsealTime->setValidator( new QIntValidator(0, 1000, this) );
    branchUnsealTime->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchUnsealTime, SIGNAL(textChanged(QString)), this, SLOT(onBranchUnsealTimeTextChanged(QString)));
    branchConsensusThreshold = new QLineEdit();
    branchConsensusThreshold->setText("0.10");
    branchConsensusThreshold->setValidator( new QDoubleValidator(0.0, 1.0, 8, this) );
    branchConsensusThreshold->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(branchConsensusThreshold, SIGNAL(textChanged(QString)), this, SLOT(onBranchConsensusThresholdTextChanged(QString)));
    QPushButton *createBranchButton = new QPushButton(tr("Create Branch"));
    connect(createBranchButton, SIGNAL(clicked()), this, SLOT(onCreateBranchClicked()));
    createBranchCLI = new QLabel("");
    createBranchCLI->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createBranchCLI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createBranchCLI->setWordWrap(true);
    createBranchCLIResponse = new QLabel("");
    createBranchCLIResponse->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createBranchCLIResponse->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createBranchCLIResponse->setWordWrap(true);

    /* disable all widgets */
    branchName->setEnabled(false);
    branchDescription->setEnabled(false);
    branchBaseListingFee->setEnabled(false);
    branchFreeDecisions->setEnabled(false);
    branchTargetDecisions->setEnabled(false);
    branchMaxDecisions->setEnabled(false);
    branchMinTradingFee->setEnabled(false);
    branchTau->setEnabled(false);
    branchBallotTime->setEnabled(false);
    branchUnsealTime->setEnabled(false);
    branchConsensusThreshold->setEnabled(false);

    /* Grid Layout           */
    /*     label0   widget0  */
    /*     label1   widget1  */
    /*     ...      ...      */
    /*                       */
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(15);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 5);
    int row = 0;
    glayout->addWidget(new QLabel(tr("Name: ")), row, col0);
    glayout->addWidget(branchName, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Description: ")), row, col0);
    glayout->addWidget(branchDescription, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Base Listing Fee: ")), row, col0);
    glayout->addWidget(branchBaseListingFee, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Free Decisions: ")), row, col0);
    glayout->addWidget(branchFreeDecisions, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Target Decisions: ")), row, col0);
    glayout->addWidget(branchTargetDecisions, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Max Decisions: ")), row, col0);
    glayout->addWidget(branchMaxDecisions, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Min Trading Fee: ")), row, col0);
    glayout->addWidget(branchMinTradingFee, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Tau: ")), row, col0);
    glayout->addWidget(branchTau, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Ballot Time: ")), row, col0);
    glayout->addWidget(branchBallotTime, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Unseal Time: ")), row, col0);
    glayout->addWidget(branchUnsealTime, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Consensus Threshold: ")), row, col0);
    glayout->addWidget(branchConsensusThreshold, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("CLI: ")), row, col0);
    glayout->addWidget(createBranchCLI, row, col1);
    row++;
    glayout->addWidget(createBranchButton, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Status: ")), row, col0);
    glayout->addWidget(createBranchCLIResponse, row, col1);

    /* VBoxLayout Layout */
    /*     glayout       */
    /*     widget        */
    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(5);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(new QWidget(), 8); /* receives all the strectch */

    /* update CLI */
    updateCreateBranchCLI();
}

void MarketView::initCreateDecisionTab(QWidget *page)
{
    /* widgets */
    decisionAddress = new QLineEdit();
    decisionAddress->setText("");
    decisionAddress->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(decisionAddress, SIGNAL(textChanged(QString)), this, SLOT(onDecisionAddressTextChanged(QString)));
    decisionPrompt = new QLineEdit();
    decisionPrompt->setText("Decision Prompt");
    decisionPrompt->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(decisionPrompt, SIGNAL(textChanged(QString)), this, SLOT(onDecisionPromptTextChanged(QString)));
    decisionEventOverBy = new QLineEdit();
    decisionEventOverBy->setText("1");
    decisionEventOverBy->setValidator( new QIntValidator(0, 1000000, this) );
    decisionEventOverBy->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(decisionEventOverBy, SIGNAL(textChanged(QString)), this, SLOT(onDecisionEventOverByTextChanged(QString)));
    decisionAnswerIsOptionalRadioButton = new QRadioButton(tr("Optional"));
    decisionAnswerIsOptionalRadioButton->setChecked(true);
    connect(decisionAnswerIsOptionalRadioButton, SIGNAL(toggled(bool)), this, SLOT(onDecisionAnswerIsOptionalRadioButtonToggled(bool)));
    decisionIsBinaryRadioButton = new QRadioButton(tr("Binary"));
    decisionIsBinaryRadioButton->setChecked(true);
    connect(decisionIsBinaryRadioButton, SIGNAL(toggled(bool)), this, SLOT(onDecisionIsBinaryRadioButtonToggled(bool)));
    decisionMinimum = new QLineEdit();
    decisionMinimum->setText("0.00000000");
    decisionMinimum->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    decisionMinimum->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(decisionMinimum, SIGNAL(textChanged(QString)), this, SLOT(onDecisionMinimumTextChanged(QString)));
    decisionMaximum = new QLineEdit();
    decisionMaximum->setText("1.00000000");
    decisionMaximum->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    decisionMaximum->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(decisionMaximum, SIGNAL(textChanged(QString)), this, SLOT(onDecisionMaximumTextChanged(QString)));
    QPushButton *createDecisionButton = new QPushButton(tr("Create Decision"));
    connect(createDecisionButton, SIGNAL(clicked()), this, SLOT(onCreateDecisionClicked()));
    createDecisionCLI = new QLabel("");
    createDecisionCLI->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createDecisionCLI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createDecisionCLI->setWordWrap(true);
    createDecisionCLIResponse = new QLabel("");
    createDecisionCLIResponse->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createDecisionCLIResponse->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createDecisionCLIResponse->setWordWrap(true);

    QWidget *radiobuttons1 = new QWidget();
    QHBoxLayout *rb1layout = new QHBoxLayout(radiobuttons1);
    rb1layout->addWidget(decisionAnswerIsOptionalRadioButton);
    rb1layout->addWidget(new QRadioButton(tr("Mandatory")));
    rb1layout->addWidget(new QWidget()); /* for spacing */
    rb1layout->addWidget(new QWidget()); /* for spacing */

    QWidget *radiobuttons2 = new QWidget();
    QHBoxLayout *rb2layout = new QHBoxLayout(radiobuttons2);
    rb2layout->addWidget(decisionIsBinaryRadioButton);
    rb2layout->addWidget(new QRadioButton(tr("Scaled")));
    rb2layout->addWidget(new QWidget()); /* for spacing */
    rb2layout->addWidget(new QWidget()); /* for spacing */

    /* Grid Layout           */
    /*     label0   widget0  */
    /*     label1   widget1  */
    /*     ...      ...      */
    /*                       */
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(15);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 5);
    int row = 0;
    glayout->addWidget(new QLabel(tr("Branch: ")), row, col0);
    glayout->addWidget(decisionBranchLabel, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Address: ")), row, col0);
    glayout->addWidget(decisionAddress, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Prompt: ")), row, col0);
    glayout->addWidget(decisionPrompt, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("EventOverBy: ")), row, col0);
    glayout->addWidget(decisionEventOverBy, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Voting: ")), row, col0);
    glayout->addWidget(radiobuttons1, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Scaling: ")), row, col0);
    glayout->addWidget(radiobuttons2, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Minimum: ")), row, col0);
    glayout->addWidget(decisionMinimum, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Maximum: ")), row, col0);
    glayout->addWidget(decisionMaximum, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("CLI: ")), row, col0);
    glayout->addWidget(createDecisionCLI, row, col1);
    row++;
    glayout->addWidget(createDecisionButton, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Status: ")), row, col0);
    glayout->addWidget(createDecisionCLIResponse, row, col1);

    /* VBoxLayout Layout */
    /*     glayout       */
    /*     widget        */
    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(5);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(new QWidget(), 8); /* receives all the strectch */

    /* update CLI */
    onDecisionIsBinaryRadioButtonToggled(true);
}

void MarketView::initCreateMarketTab(QWidget *page)
{
    /* widgets */
    marketAddress = new QLineEdit();
    marketAddress->setText("");
    marketAddress->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(marketAddress, SIGNAL(textChanged(QString)), this, SLOT(onMarketAddressTextChanged(QString)));
    marketTitle = new QLineEdit();
    marketTitle->setText("Market Title");
    marketTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(marketTitle, SIGNAL(textChanged(QString)), this, SLOT(onMarketTitleTextChanged(QString)));
    marketDescription = new QLineEdit();
    marketDescription->setText("Market Description");
    marketDescription->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(marketDescription, SIGNAL(textChanged(QString)), this, SLOT(onMarketDescriptionTextChanged(QString)));
    marketB = new QLineEdit();
    marketB->setText("0.10000000");
    marketB->setValidator( new QDoubleValidator(0.0, 10.0, 8, this) );
    marketB->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(marketB, SIGNAL(textChanged(QString)), this, SLOT(onMarketBTextChanged(QString)));
    marketTradingFee = new QLineEdit();
    marketTradingFee->setText("0.10000000");
    marketTradingFee->setValidator( new QDoubleValidator(0.0, 10.0, 8, this) );
    marketTradingFee->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(marketTradingFee, SIGNAL(textChanged(QString)), this, SLOT(onMarketTradingFeeTextChanged(QString)));
    marketMaxCommission = new QLineEdit();
    marketMaxCommission->setText("0.10000000");
    marketMaxCommission->setValidator( new QDoubleValidator(0.0, 10.0, 8, this) );
    marketMaxCommission->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(marketMaxCommission, SIGNAL(textChanged(QString)), this, SLOT(onMarketMaxCommissionTextChanged(QString)));
    marketTags = new QLineEdit();
    marketTags->setText("tag1,tag2,tag3");
    marketTags->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(marketTags, SIGNAL(textChanged(QString)), this, SLOT(onMarketTagsTextChanged(QString)));
    marketMaturation = new QLineEdit();
    marketMaturation->setText("1");
    marketMaturation->setValidator( new QIntValidator(0, 1000000, this) );
    marketMaturation->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(marketMaturation, SIGNAL(textChanged(QString)), this, SLOT(onMarketMaturationTextChanged(QString)));
    marketTxPoW = new QLineEdit();
    marketTxPoW->setText("0.10000000");
    marketTxPoW->setValidator( new QDoubleValidator(0.0, 10.0, 8, this) );
    marketTxPoW->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(marketTxPoW, SIGNAL(textChanged(QString)), this, SLOT(onMarketTxPoWTextChanged(QString)));
    QPushButton *createMarketButton = new QPushButton(tr("Create Market"));
    connect(createMarketButton, SIGNAL(clicked()), this, SLOT(onCreateMarketClicked()));
    createMarketCLI = new QLabel("");
    createMarketCLI->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createMarketCLI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createMarketCLI->setWordWrap(true);
    createMarketCLIResponse = new QLabel("");
    createMarketCLIResponse->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createMarketCLIResponse->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createMarketCLIResponse->setWordWrap(true);

    QWidget *decisionWidget  = new QWidget();
    QHBoxLayout *hlayout = new QHBoxLayout(decisionWidget);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->addWidget(marketDecisionLabel);
    hlayout->addWidget(marketDecisionFunction);

    /* Grid Layout           */
    /*     label0   widget0  */
    /*     label1   widget1  */
    /*     ...      ...      */
    /*                       */
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(15);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 5);
    int row = 0;
    glayout->addWidget(new QLabel(tr("Branch: ")), row, col0);
    glayout->addWidget(marketBranchLabel, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Decision: ")), row, col0);
    glayout->addWidget(decisionWidget, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Address: ")), row, col0);
    glayout->addWidget(marketAddress, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Title: ")), row, col0);
    glayout->addWidget(marketTitle, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Description: ")), row, col0);
    glayout->addWidget(marketDescription, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Liquidity Factor: ")), row, col0);
    glayout->addWidget(marketB, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Trading Fee: ")), row, col0);
    glayout->addWidget(marketTradingFee, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Max Commission: ")), row, col0);
    glayout->addWidget(marketMaxCommission, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Tags: ")), row, col0);
    glayout->addWidget(marketTags, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Maturation: ")), row, col0);
    glayout->addWidget(marketMaturation, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("TxPow: ")), row, col0);
    glayout->addWidget(marketTxPoW, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("CLI: ")), row, col0);
    glayout->addWidget(createMarketCLI, row, col1);
    row++;
    glayout->addWidget(createMarketButton, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Status: ")), row, col0);
    glayout->addWidget(createMarketCLIResponse, row, col1);

    /* VBoxLayout Layout */
    /*     glayout       */
    /*     widget        */
    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(5);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(new QWidget(), 8); /* receives all the strectch */

    /* update CLI */
    updateCreateMarketCLI();
}

void MarketView::initCreateTradeTab(QWidget *page)
{
    /* widgets */
    tradeAddress = new QLineEdit();
    tradeAddress->setText("");
    tradeAddress->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(tradeAddress, SIGNAL(textChanged(QString)), this, SLOT(onTradeAddressTextChanged(QString)));
    tradeBuyRadioButton = new QRadioButton(tr("Buy"));
    tradeBuyRadioButton->setChecked(true);
    connect(tradeBuyRadioButton, SIGNAL(toggled(bool)), this, SLOT(onTradeBuyRadioButtonToggled(bool)));
    tradeShares = new QLineEdit();
    tradeShares->setText("1.00000000");
    tradeShares->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    tradeShares->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(tradeShares, SIGNAL(textChanged(QString)), this, SLOT(onTradeSharesTextChanged(QString)));
    tradePrice = new QLineEdit();
    tradePrice->setText("0.00000000");
    tradePrice->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    tradePrice->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(tradePrice, SIGNAL(textChanged(QString)), this, SLOT(onTradePriceTextChanged(QString)));
    tradeDecState = new QLineEdit();
    tradeDecState->setText("0");
    tradeDecState->setValidator( new QIntValidator(0, 10000, this) );
    tradeDecState->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(tradeDecState, SIGNAL(textChanged(QString)), this, SLOT(onTradeDecStateTextChanged(QString)));
    tradeNonce = new QLineEdit();
    tradeNonce->setText("0");
    tradeNonce->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    tradeNonce->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    connect(tradeNonce, SIGNAL(textChanged(QString)), this, SLOT(onTradeNonceTextChanged(QString)));
    QPushButton *createTradeButton = new QPushButton(tr("Create Trade"));
    connect(createTradeButton, SIGNAL(clicked()), this, SLOT(onCreateTradeClicked()));
    createTradeCLI = new QLabel("");
    createTradeCLI->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createTradeCLI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createTradeCLI->setWordWrap(true);
    createTradeCLIResponse = new QLabel("");
    createTradeCLIResponse->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    createTradeCLIResponse->setTextInteractionFlags(Qt::TextSelectableByMouse);
    createTradeCLIResponse->setWordWrap(true);

    QWidget *radiobuttons = new QWidget();
    QHBoxLayout *rblayout = new QHBoxLayout(radiobuttons);
    rblayout->addWidget(tradeBuyRadioButton);
    rblayout->addWidget(new QRadioButton(tr("Sell")));
    rblayout->addWidget(new QWidget()); /* for spacing */
    rblayout->addWidget(new QWidget()); /* for spacing */

    /* Grid Layout           */
    /*     label0   widget0  */
    /*     label1   widget1  */
    /*     ...      ...      */
    /*                       */
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(15);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 5);
    int row = 0;
    glayout->addWidget(new QLabel(tr("Branch: ")), row, col0);
    glayout->addWidget(tradeBranchLabel, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Decision: ")), row, col0);
    glayout->addWidget(tradeDecisionLabel, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Market: ")), row, col0);
    glayout->addWidget(tradeMarketLabel, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Address: ")), row, col0);
    glayout->addWidget(tradeAddress, row, col1);
    row++;
    glayout->addWidget(radiobuttons, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Shares: ")), row, col0);
    glayout->addWidget(tradeShares, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Price: ")), row, col0);
    glayout->addWidget(tradePrice, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Decision State: ")), row, col0);
    glayout->addWidget(tradeDecState, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Nonce: ")), row, col0);
    glayout->addWidget(tradeNonce, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("CLI: ")), row, col0);
    glayout->addWidget(createTradeCLI, row, col1);
    row++;
    glayout->addWidget(createTradeButton, row, col1);
    row++;
    glayout->addWidget(new QLabel(tr("Status: ")), row, col0);
    glayout->addWidget(createTradeCLIResponse, row, col1);

    /* VBoxLayout Layout */
    /*     glayout       */
    /*     widget        */
    QVBoxLayout *vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(10,10,10,10);
    vlayout->setSpacing(5);
    vlayout->addLayout(glayout, 0);
    vlayout->addWidget(new QWidget(), 8); /* receives all the strectch */

    /* update CLI */
    updateCreateTradeCLI();
}

void MarketView::updateCreateBranchCLI(void)
{
    if (!createBranchCLI)
        return;

    QString cli("truthcoin-cli createbranch");
    cli += QString(" '") + ((branchName->text().size())? branchName->text(): QString("&lt;name&gt;"));
    cli += QString("' '") + ((branchDescription->text().size())? branchDescription->text(): QString("&lt;description&gt;"));
    cli += QString("' ") + ((branchBaseListingFee->text().size())? branchBaseListingFee->text(): QString("&lt;baseListingFee&gt;"));
    cli += QString(" ") + ((branchFreeDecisions->text().size())? branchFreeDecisions->text(): QString("&lt;freeDecisions&gt;"));
    cli += QString(" ") + ((branchTargetDecisions->text().size())? branchTargetDecisions->text(): QString("&lt;targetDecisions&gt;"));
    cli += QString(" ") + ((branchMaxDecisions->text().size())? branchMaxDecisions->text(): QString("&lt;maxDecisions&gt;"));
    cli += QString(" ") + ((branchMinTradingFee->text().size())? branchMinTradingFee->text(): QString("&lt;minTradingFee&gt;"));
    cli += QString(" ") + ((branchTau->text().size())? branchTau->text(): QString("&lt;tau&gt;"));
    cli += QString(" ") + ((branchBallotTime->text().size())? branchBallotTime->text(): QString("&lt;ballotTime&gt;"));
    cli += QString(" ") + ((branchUnsealTime->text().size())? branchUnsealTime->text(): QString("&lt;unsealTime&gt;"));
    cli += QString(" ") + ((branchConsensusThreshold->text().size())? branchConsensusThreshold->text(): QString("&lt;consensusThreshold&gt;"));
    createBranchCLI->setText(cli);
}

void MarketView::updateCreateDecisionCLI(void)
{
    if (!createDecisionCLI)
        return;

    QString branchid = branchTabLabels[11].text();

    QString cli("truthcoin-cli createdecision");
    cli += QString(" ") + ((decisionAddress->text().size())? decisionAddress->text(): QString("&lt;address&gt;"));
    cli += QString(" ") + ((branchid.size())? branchid: QString("&lt;branchid&gt;"));
    cli += QString(" '") + ((decisionPrompt->text().size())? decisionPrompt->text(): QString("&lt;prompt&gt;"));
    cli += QString("' ") + ((decisionEventOverBy->text().size())? decisionEventOverBy->text(): QString("&lt;eventOverBy&gt;"));
    cli += QString(" ") + ((decisionAnswerIsOptionalRadioButton->isChecked())? QString("true"): QString("false"));
    cli += QString(" ") + ((decisionIsBinaryRadioButton->isChecked())? QString("false"): QString("true"));
    if (!decisionIsBinaryRadioButton->isChecked()) {
        cli += QString(" ") + ((decisionMinimum->text().size())? decisionMinimum->text(): QString("&lt;minimum&gt;"));
        cli += QString(" ") + ((decisionMaximum->text().size())? decisionMaximum->text(): QString("&lt;maximum&gt;"));
    }

    createDecisionCLI->setText(cli);
    createDecisionCLIResponse->setText(QString(""));
}

void MarketView::updateCreateMarketCLI(void)
{
    if (!createMarketCLI)
        return;

    QString decisionid = decisionTabLabels[8].text();
    QString decitionfunc = marketDecisionFunction->currentText();

    QString cli("truthcoin-cli createmarket");
    cli += QString(" ") + ((marketAddress->text().size())? marketAddress->text(): QString("&lt;address&gt;"));
    cli += QString(" ") + ((decisionid.size())? decisionid: QString("&lt;decisionid&gt;"));
    cli += QString(":") + decitionfunc;
    cli += QString(" ") + ((marketB->text().size())? marketB->text(): QString("&lt;liquidityFactor&gt;"));
    cli += QString(" ") + ((marketTradingFee->text().size())? marketTradingFee->text(): QString("&lt;tradingFee&gt;"));
    cli += QString(" ") + ((marketMaxCommission->text().size())? marketMaxCommission->text(): QString("&lt;maxCommission&gt;"));
    cli += QString(" '") + ((marketTitle->text().size())? marketTitle->text(): QString("&lt;title&gt;"));
    cli += QString("' '") + ((marketDescription->text().size())? marketDescription->text(): QString("&lt;description&gt;"));
    cli += QString("' '") + ((marketTags->text().size())? marketTags->text(): QString("&lt;tags&gt;"));
    cli += QString("' ") + ((marketMaturation->text().size())? marketMaturation->text(): QString("&lt;maturation&gt;"));
    cli += QString(" ") + ((marketTxPoW->text().size())? marketTxPoW->text(): QString("&lt;txPoW&gt;"));

    createMarketCLI->setText(cli);
    createMarketCLIResponse->setText(QString(""));
}

void MarketView::updateCreateTradeCLI(void)
{
    if (!createTradeCLI || !tradeShares || !tradePrice)
        return;

    QString marketid = marketTabLabels[8].text();

    QString cli("truthcoin-cli createtrade");
    cli += QString(" ") + ((tradeAddress->text().size())? tradeAddress->text(): QString("&lt;address&gt;"));
    cli += QString(" ") + ((marketid.size())? marketid: QString("&lt;marketid&gt;"));
    cli += QString(" ") + ((tradeBuyRadioButton->isChecked())? QString("buy"): QString("sell"));
    cli += QString(" ") + ((tradeShares->text().size())? tradeShares->text(): QString("&lt;shares&gt;"));
    cli += QString(" ") + ((tradePrice->text().size())? tradePrice->text(): QString("&lt;price&gt;"));
    cli += QString(" ") + ((tradeDecState->text().size())? tradeDecState->text(): QString("&lt;decisionState&gt;"));
    cli += QString(" ") + ((tradeNonce->text().size())? tradeNonce->text(): QString("&lt;nonce&gt;"));

    createTradeCLI->setText(cli);
    createTradeCLIResponse->setText(QString(""));
}

void MarketView::showDecisionWindow(void)
{
    if (!decisionWindow)
        return;
    decisionWindow->show();
    decisionWindow->raise();
    decisionWindow->setFocus();
}

void MarketView::showMarketWindow(void)
{
    if (!marketWindow)
        return;
    marketWindow->show();
    marketWindow->raise();
    marketWindow->setFocus();
}

void MarketView::showTradeWindow(void)
{
    if (!tradeWindow)
        return;
    tradeWindow->show();
    tradeWindow->raise();
    tradeWindow->setFocus();
}

void MarketView::setModel(WalletModel *model)
{
    this->model = model;
    /* branch is last. it populate decisions and market */
    tradeWindow->setModel(model);
    marketWindow->setModel(model);
    decisionWindow->setModel(model);
    branchWindow->setModel(model);
}

void MarketView::onBranchChange(const marketBranch *branch)
{
    this->branch = branch;

    if (!branch) {
        branchLabels[1]->setText("");
        decisionBranchLabel->setText("");
        marketBranchLabel->setText("");
        tradeBranchLabel->setText("");

        for(uint32_t i=0; i < MARKETBRANCH_NLABLES; i++)
           branchTabLabels[i].setText("");
    } else {
        branchLabels[1]->setText( QString::fromStdString(branch->name) );
        decisionBranchLabel->setText( QString::fromStdString(branch->name) );
        marketBranchLabel->setText( QString::fromStdString(branch->name) );
        tradeBranchLabel->setText( QString::fromStdString(branch->name) );

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
    decisionWindow->onBranchChange(branch);

    /* update CLIs */
    updateCreateDecisionCLI();
    updateCreateMarketCLI();
    updateCreateTradeCLI();
}

void MarketView::onDecisionChange(const marketDecision *decision)
{
    this->decision = decision;

    if (!decision) {
        decisionLabels[1]->setText("");
        marketDecisionLabel->setText("");
        tradeDecisionLabel->setText("");

        for(uint32_t i=0; i < MARKETDECISION_NLABLES; i++)
           decisionTabLabels[i].setText("");
    } else {
        decisionLabels[1]->setText( QString::fromStdString(decision->prompt) );
        marketDecisionLabel->setText( QString::fromStdString(decision->prompt) );
        tradeDecisionLabel->setText( QString::fromStdString(decision->prompt) );

        decisionTabLabels[0].setText( formatPrompt(decision) );
        decisionTabLabels[1].setText( formatAddress(decision) );
        decisionTabLabels[2].setText( formatEventOverBy(decision) );
        decisionTabLabels[3].setText( formatIsScaled(decision) );
        decisionTabLabels[4].setText( formatMinimum(decision) );
        decisionTabLabels[5].setText( formatMaximum(decision) );
        decisionTabLabels[6].setText( formatAnswerOptional(decision) );
        decisionTabLabels[7].setText( formatBranchID(decision) );
        decisionTabLabels[8].setText( formatHash(decision) );
        decisionTabLabels[9].setText( formatUint256(decision->txid) );
    }

    /* propagate */
    marketWindow->onDecisionChange(branch, decision);

    /* update CLIs */
    updateCreateMarketCLI();
    updateCreateTradeCLI();
}

void MarketView::onMarketChange(const marketMarket *market)
{
    this->market = market;

    if (!market) {
        marketLabels[1]->setText("");
        tradeMarketLabel->setText("");

        for(uint32_t i=0; i < MARKETMARKET_NLABLES; i++)
           marketTabLabels[i].setText("");
    } else {
        marketLabels[1]->setText( QString::fromStdString(market->title) );
        tradeMarketLabel->setText( QString::fromStdString(market->title) );

        marketTabLabels[0].setText( formatTitle(market) );
        marketTabLabels[1].setText( formatDescription(market) );
        marketTabLabels[2].setText( formatAddress(market) );
        marketTabLabels[3].setText( formatB(market) );
        marketTabLabels[4].setText( formatTradingFee(market) );
        marketTabLabels[5].setText( formatTags(market) );
        marketTabLabels[6].setText( formatMaturation(market) );
        marketTabLabels[7].setText( formatDecisionIDs(market) );
        marketTabLabels[8].setText( formatHash(market) );
        marketTabLabels[9].setText( formatUint256(market->txid) );
    }

    /* propagate */
    tradeWindow->onMarketChange(branch, decision, market);

    /* graph */
    const MarketTradeTableModel *tableModel = tradeWindow->getTradeModel();
    if (tableModel) {
        double *X = (double *)0;
        double *Y = (double *)0;
        unsigned int N = 0;
        tableModel->getData(&X, &Y, &N);
        graphWidget->setData(X, Y, N);
    }

    /* update CLIs */
    updateCreateTradeCLI();
}

void MarketView::onTradeChange(const marketTrade *trade)
{
    this->trade = trade;

    if (!trade) {
        for(uint32_t i=0; i < MARKETTRADE_NLABLES; i++)
           tradeTabLabels[i].setText("");
    } else {
        tradeTabLabels[0].setText( formatAddress(trade) );
        tradeTabLabels[1].setText( formatBuySell(trade) );
        tradeTabLabels[2].setText( formatNShares(trade) );
        tradeTabLabels[3].setText( formatPrice(trade) );
        tradeTabLabels[4].setText( formatDecisionState(trade) );
        tradeTabLabels[5].setText( formatNonce(trade) );
        tradeTabLabels[6].setText( formatHash(trade) );
        tradeTabLabels[7].setText( formatUint256(trade->txid) );
    }
}

void MarketView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // columnResizingFixer->stretchColumnWidth(MarketTableModel::Hash);
}

// Need to override default Ctrl+C action for amount as default behaviour is just to copy DisplayRole text
bool MarketView::eventFilter(QObject *obj, QEvent *event)
{
    return QWidget::eventFilter(obj, event);
}

void MarketView::onBranchNameTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchDescriptionTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchBaseListingFeeTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchFreeDecisionsTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchTargetDecisionsTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchMaxDecisionsTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchMinTradingFeeTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchTauTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchBallotTimeTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onBranchUnsealTimeTextChanged(const QString &)
{
    updateCreateBranchCLI();
}

void MarketView::onDecisionAddressTextChanged(const QString &)
{
    updateCreateDecisionCLI();
}

void MarketView::onDecisionPromptTextChanged(const QString &)
{
    updateCreateDecisionCLI();
}

void MarketView::onDecisionEventOverByTextChanged(const QString &)
{
    updateCreateDecisionCLI();
}

void MarketView::onDecisionAnswerIsOptionalRadioButtonToggled(bool)
{
    updateCreateDecisionCLI();
}

void MarketView::onDecisionIsBinaryRadioButtonToggled(bool)
{
    bool enableMinMax = !decisionIsBinaryRadioButton->isChecked(); 
    decisionMinimum->setEnabled(enableMinMax);
    decisionMaximum->setEnabled(enableMinMax);
    updateCreateDecisionCLI();
}

void MarketView::onDecisionMinimumTextChanged(const QString &)
{
    updateCreateDecisionCLI();
}

void MarketView::onDecisionMaximumTextChanged(const QString &)
{
    updateCreateDecisionCLI();
}

void MarketView::onMarketAddressTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketDecisionFunctionIndexChanged(int)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketTitleTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketDescriptionTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketBTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketTradingFeeTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketMaxCommissionTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketTagsTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketMaturationTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onMarketTxPoWTextChanged(const QString &)
{
    updateCreateMarketCLI();
}

void MarketView::onTradeBuyRadioButtonToggled(bool)
{
    updateCreateTradeCLI();
}

void MarketView::onTradeAddressTextChanged(const QString &)
{
    updateCreateTradeCLI();
}

void MarketView::onTradePriceTextChanged(const QString &)
{
    updateCreateTradeCLI();
}

void MarketView::onTradeSharesTextChanged(const QString &)
{
    updateCreateTradeCLI();
}

void MarketView::onTradeDecStateTextChanged(const QString &)
{
    updateCreateTradeCLI();
}

void MarketView::onTradeNonceTextChanged(const QString &)
{
    updateCreateTradeCLI();
}

/* onCreateBranchClicked:
 * createbranch params:
 *      "createbranch name description baselistingfee"
 *      " freedecisions targetdecisions maxdecisions"
 *      " mintradingfee"
 *      " tau ballottime unsealtime"
 *      " consensusthreshold"
 *      "\nCreates a new branch."
 *      "\n1. name                (string) the name of the branch"
 *      "\n2. description         (string) a short description of the branch"
 *      "\n3. baselistingfee      (numeric)"
 *      "\n4. freedecisions       (numeric < 65536)"
 *      "\n5. targetdecisions     (numeric < 65536)"
 *      "\n6. maxdecisions        (numeric < 65536)"
 *      "\n7. mintradingfee       (numeric)"
 *      "\n8. tau                 (block number < 65536)"
 *      "\n9. ballottime          (block number < 65536)"
 *      "\n10. unsealtime         (block number < 65536)"
 *      "\n11. consensusthreshold (numeric)";
 */
void MarketView::onCreateBranchClicked(void)
{
    extern Value createbranch(const Array &params, bool fHelp);

    std::string name = (branchName->text().size())? branchName->text().toStdString(): "<name>";
    std::string description = (branchDescription->text().size())? branchDescription->text().toStdString(): "<description>";
    double baselistingfee = atof( branchBaseListingFee->text().toStdString().c_str() );
    int freedecisions = atoi( branchFreeDecisions->text().toStdString().c_str() );
    int targetdecisions = atoi( branchTargetDecisions->text().toStdString().c_str() );
    int maxdecisions = atoi( branchMaxDecisions->text().toStdString().c_str() );
    double mintradingfee = atof( branchMinTradingFee->text().toStdString().c_str() );
    int tau = atoi( branchTau->text().toStdString().c_str() );
    int ballottime = atoi( branchBallotTime->text().toStdString().c_str() );
    int unsealtime = atoi( branchUnsealTime->text().toStdString().c_str() );
    double consensusthreshold = atof( branchConsensusThreshold->text().toStdString().c_str() );

    Array params;
    params.push_back( Value(name) );
    params.push_back( Value(description) );
    params.push_back( Value(baselistingfee) );
    params.push_back( Value(freedecisions) );
    params.push_back( Value(targetdecisions) );
    params.push_back( Value(maxdecisions) );
    params.push_back( Value(mintradingfee) );
    params.push_back( Value(tau) );
    params.push_back( Value(ballottime) );
    params.push_back( Value(unsealtime) );
    params.push_back( Value(consensusthreshold) );

    Value resp;
    try {
        resp = createbranch(params, false);
    } catch (const std::runtime_error &e) {
        createBranchCLIResponse->setText(QString(e.what()));
        return; 
    } catch (const std::exception &e) {
        createBranchCLIResponse->setText(QString(e.what()));
        return; 
    }  catch (const Object &e) {
        resp = e;
    } catch (...) {
        createBranchCLIResponse->setText(tr("createbranch: unknown exception"));
        return; 
    }

    try {
        std::string text = write_string(resp, true);
        createBranchCLIResponse->setText(QString::fromStdString(text));
    } catch (...) {
        createBranchCLIResponse->setText(tr("write_string: unknown exception"));
    }
}

/* onCreateDecisionClicked:
 * createdecision params:
 *      "createdecision branchid prompt address eventoverby [scaled min max]"
 *      "\nCreates a new decision within the branch."
 *      "\n1. address             (base58 address)"
 *      "\n2. branchid            (uint256 string)"
 *      "\n3. prompt              (string)"
 *      "\n4. eventoverby         (block number)"
 *      "\n5. answer optionality  (false=mandatory to answer, true=optional to answer)"
 *      "\n6. is_scaled           (boolean)"
 *      "\n7. scaled min          (if scaled, numeric)"
 *      "\n8. scaled max          (if scaled, numeric)";
 */
void MarketView::onCreateDecisionClicked(void)
{
    extern Value createdecision(const Array &params, bool fHelp);

    std::string address = (decisionAddress->text().size())? decisionAddress->text().toStdString(): "<address>";
    std::string branchid = (branchTabLabels[11].text().size())? branchTabLabels[11].text().toStdString(): "<branchid>";
    std::string prompt = (decisionPrompt->text().size())? decisionPrompt->text().toStdString(): "<prompt>";
    int eventoverby = atoi( decisionEventOverBy->text().toStdString().c_str() );
    bool answer_optionality = (decisionAnswerIsOptionalRadioButton->isChecked())? true: false;
    bool is_scaled = (decisionIsBinaryRadioButton->isChecked())? false: true;
    double scaled_min = atof( decisionMinimum->text().toStdString().c_str() );
    double scaled_max = atof( decisionMaximum->text().toStdString().c_str() );

    Array params;
    params.push_back( Value(address) );
    params.push_back( Value(branchid) );
    params.push_back( Value(prompt) );
    params.push_back( Value(eventoverby) );
    params.push_back( Value(answer_optionality) );
    params.push_back( Value(is_scaled) );
    if (!decisionIsBinaryRadioButton->isChecked()) {
        params.push_back( Value(scaled_min) );
        params.push_back( Value(scaled_max) );
    }

    Value resp;
    try {
        resp = createdecision(params, false);
    } catch (const std::runtime_error &e) {
        createDecisionCLIResponse->setText(QString(e.what()));
        return; 
    } catch (const std::exception &e) {
        createDecisionCLIResponse->setText(QString(e.what()));
        return; 
    }  catch (const Object &e) {
        resp = e;
    } catch (...) {
        createDecisionCLIResponse->setText(tr("createdecision: unknown exception"));
        return; 
    }

    try {
        std::string text = write_string(resp, true);
        createDecisionCLIResponse->setText(QString::fromStdString(text));
    } catch (...) {
        createDecisionCLIResponse->setText(tr("write_string: unknown exception"));
    }
}

/* onCreateMarketClicked:
 * createmarket params:
 *      "createmarket address decisionid[,...] B tradingfee address title"
 *      " description tags[,...] maturation"
 *      "\nCreates a new market on the decisions."
 *      "\n1. address             (base58 address)"
 *      "\n2. decisionid[,...]    (comma-separated list of decisions)"
 *      "\n3. B                   (numeric) liquidity parameter"
 *      "\n4. trading_fee         (numeric)"
 *      "\n5. max_commission      (numeric)"
 *      "\n6. title               (string)"
 *      "\n7. description         (string)"
 *      "\n8. tags[,...]          (comma-separated list of strings)"
 *      "\n9. maturation          (block number)"
 *      "\n10. tx PoW             (numeric)"
 * 
 * Note: multiple decisions are not implemented here.
 */
void MarketView::onCreateMarketClicked(void)
{
    extern Value createmarket(const Array &params, bool fHelp);

    std::string address = (marketAddress->text().size())? marketAddress->text().toStdString(): "<address>";
    std::string decisionid = (decisionTabLabels[8].text().size())? decisionTabLabels[8].text().toStdString(): "<decisionid>";
    decisionid += ":";
    decisionid += marketDecisionFunction->currentText().toStdString();
    double B = atof( marketB->text().toStdString().c_str() );
    double trading_fee = atof( marketTradingFee->text().toStdString().c_str() );
    double max_commission = atof( marketMaxCommission->text().toStdString().c_str() );
    std::string title = (marketTitle->text().size())? marketTitle->text().toStdString(): "<title>";
    std::string description = (marketDescription->text().size())? marketDescription->text().toStdString(): "<description>";
    std::string tags = (marketTags->text().size())? marketTags->text().toStdString(): "<tags>";
    int maturation = atof( marketMaturation->text().toStdString().c_str() );
    int txPoW = atoi( marketTxPoW->text().toStdString().c_str() );

    Array params;
    params.push_back( Value(address) );
    params.push_back( Value(decisionid) );
    params.push_back( Value(B) );
    params.push_back( Value(trading_fee) );
    params.push_back( Value(max_commission) );
    params.push_back( Value(title) );
    params.push_back( Value(description) );
    params.push_back( Value(tags) );
    params.push_back( Value(maturation) );
    params.push_back( Value(txPoW) );

    Value resp;
    try {
        resp = createmarket(params, false);
    } catch (const std::runtime_error &e) {
        createMarketCLIResponse->setText(QString(e.what()));
        return; 
    } catch (const std::exception &e) {
        createMarketCLIResponse->setText(QString(e.what()));
        return; 
    }  catch (const Object &e) {
        resp = e;
    } catch (...) {
        createMarketCLIResponse->setText(tr("createmarket: unknown exception"));
        return; 
    }

    try {
        std::string text = write_string(resp, true);
        createMarketCLIResponse->setText(QString::fromStdString(text));
    } catch (...) {
        createMarketCLIResponse->setText(tr("write_string: unknown exception"));
    }
}

/* onCreateTradeClicked:
 * createtrade params:
 *      "createtrade address marketid buy_or_sell number_shares"
 *      " price decision_state [nonce]"
 *      "\nCreates a new trade order in the market."
 *      "\n1. address             (base58 address)"
 *      "\n2. marketid            (u256 string)"
 *      "\n3. buy_or_sell         (string)"
 *      "\n4. number_shares       (numeric)"
 *      "\n5. price               (numeric)"
 *      "\n6. decision_state      (string)"
 *      "\n7. nonce               (optional numeric)"
 */
void MarketView::onCreateTradeClicked(void)
{
    extern Value createtrade(const Array &params, bool fHelp);

    std::string address = (tradeAddress->text().size())? tradeAddress->text().toStdString(): "<address>";
    std::string marketid = (marketTabLabels[8].text().size())? marketTabLabels[8].text().toStdString(): "<marketid>";
    std::string buy_or_sell = (tradeBuyRadioButton->isChecked())? "buy": "sell";
    double number_shares = atof( tradeShares->text().toStdString().c_str() );
    double price = atof( tradePrice->text().toStdString().c_str() );
    int decision_state = atoi( tradeDecState->text().toStdString().c_str() );
    int nonce = atoi( tradeNonce->text().toStdString().c_str() );

    Array params;
    params.push_back( Value(address) );
    params.push_back( Value(marketid) );
    params.push_back( Value(buy_or_sell) );
    params.push_back( Value(number_shares) );
    params.push_back( Value(price) );
    params.push_back( Value(decision_state) );
    params.push_back( Value(nonce) );

    Value resp;
    try {
        resp = createtrade(params, false);
    } catch (const std::runtime_error &e) {
        createTradeCLIResponse->setText(QString(e.what()));
        return; 
    } catch (const std::exception &e) {
        createTradeCLIResponse->setText(QString(e.what()));
        return; 
    }  catch (const Object &e) {
        resp = e;
    } catch (...) {
        createTradeCLIResponse->setText(tr("createtrade: unknown exception"));
        return; 
    }

    try {
        std::string text = write_string(resp, true);
        createTradeCLIResponse->setText(QString::fromStdString(text));
    } catch (...) {
        createTradeCLIResponse->setText(tr("write_string: unknown exception"));
    }
}

