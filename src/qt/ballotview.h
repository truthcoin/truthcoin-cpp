// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTVIEW_H
#define TRUTHCOIN_QT_BALLOTVIEW_H


#include <QLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

#include "guiutil.h"

class marketBranch;
class marketOutcome;
class marketSealedVote;
class marketVote;
class BallotBallotWindow;
class BallotBranchWindow;
class BallotOutcomeWindow;
class BallotSealedVoteWindow;
class BallotVoteWindow;
class WalletModel;


#define BALLOTBRANCH_NLABLES       13
#define BALLOTBALLOT_NLABLES        4
#define BALLOTOUTCOME_NLABLES       2
#define BALLOTSEALEDVOTE_NLABLES   13
#define BALLOTVOTE_NLABLES         13
#define BALLOT_NOUTCOMEGRIDLAYOUTS  6


class BallotView
    : public QWidget
{
    Q_OBJECT

public:
    explicit BallotView(QWidget *parent=0);

    void setModel(WalletModel *model);
    void onBallotChange(unsigned int);
    void onBranchChange(const marketBranch *);
    void onOutcomeChange(const marketOutcome *);
    void onSealedVoteChange(const marketSealedVote *);
    void onVoteChange(const marketVote *);

private:
    void initSelectBranchTab(QWidget *);
    void initSelectBallotTab(QWidget *);
    void initSelectSealedVoteTab(QWidget *);
    void initSelectVoteTab(QWidget *);
    void initSelectOutcomeTab(QWidget *);
    void initCreateSealedVoteTab(QWidget *);
    void initCreateVoteTab(QWidget *);

private:
    WalletModel *model;

    /* select tab variables */
    QLabel *branchLabels[2];
    QPushButton *branchButton;
    BallotBranchWindow *branchWindow;
    const marketBranch *branch;
    QLabel branchTabLabels[BALLOTBRANCH_NLABLES];

    QLabel *ballotLabels[2];
    QPushButton *ballotButton;
    BallotBallotWindow *ballotWindow;
    uint32_t ballotNum;
    QLabel ballotTabLabels[BALLOTBALLOT_NLABLES];

    QPushButton *outcomeButton;
    BallotOutcomeWindow *outcomeWindow;
    const marketOutcome *outcome;
    QLabel outcomeTabLabels[BALLOTOUTCOME_NLABLES];
    QGridLayout *outcomeLayouts[BALLOT_NOUTCOMEGRIDLAYOUTS];

    QPushButton *sealedVoteButton;
    BallotSealedVoteWindow *sealedVoteWindow;
    const marketSealedVote *sealedVote;
    QLabel sealedVoteTabLabels[BALLOTSEALEDVOTE_NLABLES];

    QPushButton *voteButton;
    BallotVoteWindow *voteWindow;
    const marketVote *vote;
    QLabel voteTabLabels[BALLOTVOTE_NLABLES];

public slots:
    void showBallotWindow(void);
    void showBranchWindow(void);
    void showOutcomeWindow(void);
    void showSealedVoteWindow(void);
    void showVoteWindow(void);
};

#endif // TRUTHCOIN_QT_BALLOTVIEW_H
