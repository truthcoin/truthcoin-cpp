// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketview.h"

#include "csvmodelwriter.h"
#include "guiutil.h"
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

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSignalMapper>
#include <QTabWidget>
#include <QVBoxLayout>

MarketView::MarketView(QWidget *parent)
    : QWidget(parent), model(0), branch(0), decision(0), market(0),
      graphWidget(0)
{
    /* window is a vlayout containing */
    /*    Grid layout                 */
    /*    Horizontal layout           */
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    QGridLayout *glayout = new QGridLayout();
    vlayout->addLayout(glayout);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addLayout(hlayout);


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
    /* aArket    label0   label1   */
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);
    glayout->addWidget(branchLabels[0], /* row */0, /* col */0);
    glayout->addWidget(branchLabels[1], /* row */0, /* col */1);
    glayout->addWidget(decisionLabels[0], /* row */1, /* col */0);
    glayout->addWidget(decisionLabels[1], /* row */1, /* col */1);
    glayout->addWidget(marketLabels[0], /* row */2, /* col */0);
    glayout->addWidget(marketLabels[1], /* row */2, /* col */1);


    /* Horizontal Layout */ 

    QGroupBox *newTrade = new QGroupBox(tr("New Trade")); 

    QGridLayout *tglayout = new QGridLayout();
    buyRadioButton  = new QRadioButton(tr("Buy"));
    buyRadioButton->setChecked(true);
    sellRadioButton = new QRadioButton(tr("Sell"));
    shares = new QLineEdit();
    shares->setText("1.00000000");
    shares->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    shares->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    price = new QLineEdit();
    price->setText("0.00000000");
    price->setValidator( new QDoubleValidator(0.0, 1000000.0, 8, this) );
    price->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QPushButton *doTradeButton = new QPushButton(tr("Trade"));
    connect(doTradeButton, SIGNAL(clicked()), this, SLOT(onDoTrade()));
    tglayout->addWidget(buyRadioButton, /* row */0, /* col */0);
    tglayout->addWidget(sellRadioButton, /* row */0, /* col */1);
    tglayout->addWidget(new QLabel(tr("Shares: ")), /* row */2, /* col */0);
    tglayout->addWidget(shares, /* row */2, /* col */1);
    tglayout->addWidget(new QLabel(tr("Price: ")), /* row */3, /* col */0);
    tglayout->addWidget(price, /* row */3, /* col */1);
    tglayout->addWidget(doTradeButton, /* row */4, /* col */1);
    
    QVBoxLayout *tvlayout = new QVBoxLayout(newTrade);
    tvlayout->addLayout(tglayout, 0);
    tvlayout->addWidget(new QWidget(), 10); /* receives all the strectch */

    /* Trade Tab */
    QTabWidget *tabs = new QTabWidget();
    QWidget *page0 = new QWidget();
    QWidget *page1 = new QWidget();
    QWidget *page2 = new QWidget();
    QWidget *page3 = new QWidget();
    graphWidget = new MarketViewGraph();
    QWidget *page4 = (QWidget *) graphWidget;

    initBranchTab(page0);
    initDecisionTab(page1);
    initMarketTab(page2);
    initTradeTab(page3);

    tabs->addTab(page0, tr("Branch"));
    tabs->addTab(page1, tr("Decision"));
    tabs->addTab(page2, tr("Market"));
    tabs->addTab(page3, tr("Last Trade"));
    tabs->addTab(page4, tr("Graph"));

    /* horizonal layout */
    /* last Trade   graph/trade tab */
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);
    hlayout->addWidget(newTrade);
    hlayout->addWidget(tabs);
}

void MarketView::initBranchTab(QWidget *page)
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
    };

    for(uint32_t i=0; i < MARKETBRANCH_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       glayout->addWidget(key, /* row */i, /* col */0);

       QLabel *value = &branchTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, /* col */1);
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

void MarketView::initDecisionTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETDECISION_NLABLES] = {
        "Address: ",
        "Prompt: ",
        "Event Over By: ",
        "Is Scaled?: ",
        "Minimum: ",
        "Maximum: ",
        "Answer Is Optional: ",
        "Branch: ",
        "Hash: ",
    };

    for(uint32_t i=0; i < MARKETDECISION_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, /* col */0);

       QLabel *value = &decisionTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, /* col */1);
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

void MarketView::initMarketTab(QWidget *page)
{
    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(5);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 9);

    const char *labelnames[MARKETMARKET_NLABLES] = {
        "Address: ",
        "Liquidity Parameter: ",
        "Trading Fee: ",
        "Title: ",
        "Description: ",
        "Tags: ",
        "Maturation: ",
        "Decision IDs: ",
        "Hash: ",
    };

    for(uint32_t i=0; i < MARKETMARKET_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, /* col */0);

       QLabel *value = &marketTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       value->setSizePolicy(QSizePolicy::Ignored, value->sizePolicy().verticalPolicy());
       glayout->addWidget(value, /* row */i, /* col */1);
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

void MarketView::initTradeTab(QWidget *page)
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
    };

    for(uint32_t i=0; i < MARKETTRADE_NLABLES; i++) {
       QLabel *key = new QLabel(tr(labelnames[i]));
       key->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       key->setTextInteractionFlags(Qt::TextSelectableByMouse);
       glayout->addWidget(key, /* row */i, /* col */0);

       QLabel *value = &tradeTabLabels[i];
       value->setAlignment(Qt::AlignLeft|Qt::AlignTop);
       value->setTextInteractionFlags(Qt::TextSelectableByMouse);
       value->setWordWrap(true);
       glayout->addWidget(value, /* row */i, /* col */1);
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

        for(uint32_t i=0; i < MARKETBRANCH_NLABLES; i++)
           branchTabLabels[i].setText("");
    } else {
        branchLabels[1]->setText( QString::fromStdString(branch->name) );

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
    }

    decisionWindow->onBranchChange(branch);
}

void MarketView::onDecisionChange(const marketDecision *decision)
{
    this->decision = decision;

    if (!decision) {
        decisionLabels[1]->setText("");

        for(uint32_t i=0; i < MARKETDECISION_NLABLES; i++)
           decisionTabLabels[i].setText("");
    } else {
        decisionLabels[1]->setText( QString::fromStdString(decision->prompt) );

        decisionTabLabels[0].setText( formatAddress(decision) );
        decisionTabLabels[1].setText( formatPrompt(decision) );
        decisionTabLabels[2].setText( formatEventOverBy(decision) );
        decisionTabLabels[3].setText( formatIsScaled(decision) );
        decisionTabLabels[4].setText( formatMinimum(decision) );
        decisionTabLabels[5].setText( formatMaximum(decision) );
        decisionTabLabels[6].setText( formatAnswerOptional(decision) );
        decisionTabLabels[7].setText( formatBranchID(decision) );
        decisionTabLabels[8].setText( formatHash(decision) );
    }

    marketWindow->onDecisionChange(branch, decision);
}

void MarketView::onMarketChange(const marketMarket *market)
{
    this->market = market;

    if (!market) {
        marketLabels[1]->setText("");

        for(uint32_t i=0; i < MARKETMARKET_NLABLES; i++)
           marketTabLabels[i].setText("");
    } else {
        marketLabels[1]->setText( QString::fromStdString(market->title) );

        marketTabLabels[0].setText( formatAddress(market) );
        marketTabLabels[1].setText( formatB(market) );
        marketTabLabels[2].setText( formatTradingFee(market) );
        marketTabLabels[3].setText( formatTitle(market) );
        marketTabLabels[4].setText( formatDescription(market) );
        marketTabLabels[5].setText( formatTags(market) );
        marketTabLabels[6].setText( formatMaturation(market) );
        marketTabLabels[7].setText( formatDecisionIDs(market) );
        marketTabLabels[8].setText( formatHash(market) );
    }

    tradeWindow->onMarketChange(branch, decision, market);

    const MarketTradeTableModel *tableModel = tradeWindow->getTradeModel();
    if (tableModel) {
        double *X = (double *)0;
        double *Y = (double *)0;
        unsigned int N = 0;
        tableModel->getData(&X, &Y, &N);
        graphWidget->setData(X, Y, N);
    }
}

void MarketView::onTradeChange(const marketTrade *trade)
{
    /* TODO ?? */
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

void MarketView::onDoTrade(void)
{
	/*
    TODO
    const marketMarket *market = this->market;
    double shares = this->shares->text().toDouble();
    double price = this->price->text().toDouble();
    uint8_t isBuy = buyRadioButton->isChecked();
	*/
}

