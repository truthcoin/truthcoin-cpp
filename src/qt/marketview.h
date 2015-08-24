// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETVIEW_H
#define TRUTHCOIN_QT_MARKETVIEW_H

#include "guiutil.h"

#include <QLabel>
#include <QWidget>

class marketBranch;
class marketDecision;
class marketMarket;
class marketTrade;
class MarketBranchWindow;
class MarketDecisionWindow;
class MarketMarketWindow;
class MarketTradeWindow;
class MarketViewGraph;
class WalletModel;

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
QT_END_NAMESPACE

#define MARKETBRANCH_NLABLES     13
#define MARKETDECISION_NLABLES   10
#define MARKETMARKET_NLABLES     10
#define MARKETTRADE_NLABLES      8


class MarketView
   : public QWidget
{
    Q_OBJECT

public:
    explicit MarketView(QWidget *parent = 0);

    void setModel(WalletModel *model);

    void onBranchChange(const marketBranch *);
    void onDecisionChange(const marketDecision *);
    void onMarketChange(const marketMarket *);
    void onTradeChange(const marketTrade *);

private:
    void initSelectBranchTab(QWidget *);
    void initSelectDecisionTab(QWidget *);
    void initSelectMarketTab(QWidget *);
    void initSelectTradeTab(QWidget *);
    void initCreateBranchTab(QWidget *);
    void initCreateDecisionTab(QWidget *);
    void initCreateMarketTab(QWidget *);
    void initCreateTradeTab(QWidget *);
    void updateCreateBranchCLI();
    void updateCreateDecisionCLI();
    void updateCreateMarketCLI();
    void updateCreateTradeCLI();

private:
    WalletModel *model;

    /* select tab variables */
    QLabel *branchLabels[2];
    QPushButton *branchButton;
    MarketBranchWindow *branchWindow;
    const marketBranch *branch; 
    QLabel branchTabLabels[MARKETBRANCH_NLABLES];

    QLabel *decisionLabels[2];
    QPushButton *decisionButton;
    MarketDecisionWindow *decisionWindow;
    const marketDecision *decision; 
    QLabel decisionTabLabels[MARKETDECISION_NLABLES];

    QLabel *marketLabels[2];
    QPushButton *marketButton;
    MarketMarketWindow *marketWindow;
    const marketMarket *market; 
    QLabel marketTabLabels[MARKETMARKET_NLABLES];

    QPushButton *tradeButton;
    MarketTradeWindow *tradeWindow;
    const marketTrade *trade; 
    QLabel tradeTabLabels[MARKETTRADE_NLABLES];

    /* create tab variables */
    QLineEdit *branchName;
    QLineEdit *branchDescription;
    QLineEdit *branchBaseListingFee;
    QLineEdit *branchFreeDecisions;
    QLineEdit *branchTargetDecisions;
    QLineEdit *branchMaxDecisions;
    QLineEdit *branchMinTradingFee;
    QLineEdit *branchTau;
    QLineEdit *branchBallotTime;
    QLineEdit *branchUnsealTime;
    QLineEdit *branchConsensusThreshold;
    QLabel *createBranchCLI;
    QLabel *createBranchCLIResponse;

    QLabel *decisionBranchLabel;
    QLineEdit *decisionAddress;
    QLineEdit *decisionPrompt;
    QLineEdit *decisionEventOverBy;
    QRadioButton *decisionAnswerIsOptionalRadioButton;
    QRadioButton *decisionIsBinaryRadioButton;
    QLineEdit *decisionMinimum;
    QLineEdit *decisionMaximum;
    QLabel *createDecisionCLI;
    QLabel *createDecisionCLIResponse;

    QLabel *marketBranchLabel;
    QLabel *marketDecisionLabel;
    QComboBox *marketDecisionFunction;
    QLineEdit *marketAddress;
    QLineEdit *marketTitle;
    QLineEdit *marketDescription;
    QLineEdit *marketB;
    QLineEdit *marketTradingFee;
    QLineEdit *marketMaxCommission;
    QLineEdit *marketTags;
    QLineEdit *marketMaturation;
    QLineEdit *marketTxPoW;
    QLabel *createMarketCLI;
    QLabel *createMarketCLIResponse;

    QLabel *tradeBranchLabel;
    QLabel *tradeDecisionLabel;
    QLabel *tradeMarketLabel;
    QLineEdit *tradeAddress;
    QRadioButton *tradeBuyRadioButton;
    QLineEdit *tradeShares;
    QLineEdit *tradeDecState;
    QLineEdit *tradePrice;
    QLineEdit *tradeNonce;
    QLabel *createTradeCLI;
    QLabel *createTradeCLIResponse;

    /* graph widget */
    MarketViewGraph *graphWidget;

    virtual void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void onBranchNameTextChanged(const QString &);
    void onBranchDescriptionTextChanged(const QString &);
    void onBranchBaseListingFeeTextChanged(const QString &);
    void onBranchFreeDecisionsTextChanged(const QString &);
    void onBranchTargetDecisionsTextChanged(const QString &);
    void onBranchMaxDecisionsTextChanged(const QString &);
    void onBranchMinTradingFeeTextChanged(const QString &);
    void onBranchTauTextChanged(const QString &);
    void onBranchBallotTimeTextChanged(const QString &);
    void onBranchUnsealTimeTextChanged(const QString &);
    void onDecisionAddressTextChanged(const QString &);
    void onDecisionPromptTextChanged(const QString &);
    void onDecisionEventOverByTextChanged(const QString &);
    void onDecisionAnswerIsOptionalRadioButtonToggled(bool);
    void onDecisionIsBinaryRadioButtonToggled(bool);
    void onDecisionMinimumTextChanged(const QString &);
    void onDecisionMaximumTextChanged(const QString &);
    void onMarketDecisionFunctionIndexChanged(int);
    void onMarketAddressTextChanged(const QString &);
    void onMarketTitleTextChanged(const QString &);
    void onMarketDescriptionTextChanged(const QString &);
    void onMarketBTextChanged(const QString &);
    void onMarketTradingFeeTextChanged(const QString &);
    void onMarketMaxCommissionTextChanged(const QString &);
    void onMarketTagsTextChanged(const QString &);
    void onMarketMaturationTextChanged(const QString &);
    void onMarketTxPoWTextChanged(const QString &);
    void onTradeAddressTextChanged(const QString &);
    void onTradeBuyRadioButtonToggled(bool);
    void onTradePriceTextChanged(const QString &);
    void onTradeSharesTextChanged(const QString &);
    void onTradeNonceTextChanged(const QString &);
    void onTradeDecStateTextChanged(const QString &);
    void onCreateBranchClicked(void);
    void onCreateDecisionClicked(void);
    void onCreateMarketClicked(void);
    void onCreateTradeClicked(void);
    void showBranchWindow(void);
    void showDecisionWindow(void);
    void showMarketWindow(void);
    void showTradeWindow(void);
};

#endif // TRUTHCOIN_QT_MARKETVIEW_H
