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
class QLineEdit;
class QModelIndex;
class QPushButton;
class QRadioButton;
QT_END_NAMESPACE

#define MARKETBRANCH_NLABLES     12
#define MARKETDECISION_NLABLES   9
#define MARKETMARKET_NLABLES     9
#define MARKETTRADE_NLABLES      7


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
    void initBranchTab(QWidget *);
    void initDecisionTab(QWidget *);
    void initMarketTab(QWidget *);
    void initTradeTab(QWidget *);

private:
    WalletModel *model;

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
    QLabel tradeTabLabels[MARKETTRADE_NLABLES];

    QRadioButton *buyRadioButton;
    QRadioButton *sellRadioButton;
    QLineEdit *shares;
    QLineEdit *price;

    MarketViewGraph *graphWidget;

    virtual void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:

public slots:
    void onDoTrade(void);

    void showBranchWindow(void);
    void showDecisionWindow(void);
    void showMarketWindow(void);
    void showTradeWindow(void);
};

#endif // TRUTHCOIN_QT_MARKETVIEW_H
