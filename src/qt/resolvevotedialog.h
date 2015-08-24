// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H
#define TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H

#include <QDialog>
 
struct tc_vote;
class ResolveVoteColTableModel;
class ResolveVoteGraph;
class ResolveVoteInputTableModel;
class ResolveVoteRowTableModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QEvent;
class QLabel;
class QLineEdit;
class QScrollArea;
class QTableView;
class QVBoxLayout;
QT_END_NAMESPACE

class ResolveVoteDialog
    : public QDialog
{
    Q_OBJECT

public:
    explicit ResolveVoteDialog(QWidget *parent=0);
    ~ResolveVoteDialog();
    void setModel(WalletModel *model);
    bool eventFilter(QObject *, QEvent *);

private:
    struct tc_vote *vote;
    unsigned int vote_proc_rc;

    WalletModel *model;
    QLabel *nVotersLabel; /* nVotes: nVoters */
    QLineEdit *nVotersLineEdit;
    QLabel *nDecisionsLabel; /* nDecisions: nDecisions */
    QLineEdit *nDecisionsLineEdit;
    QLabel *alphaLabel; /* alpha: alpha */
    QLineEdit *alphaLineEdit;
    QLabel *tolLabel; /* tol: tol */
    QLineEdit *tolLineEdit;
    QLabel *NALabel; /* NA: NA */
    QLineEdit *NALineEdit;
    QTableView *inputTableView;
    ResolveVoteInputTableModel *inputTableModel;
    QLabel *voteProcRCLabel[2]; /* rc: rc */
    QTableView *rowTableView;
    ResolveVoteRowTableModel *rowTableModel;
    QTableView *colTableView;
    ResolveVoteColTableModel *colTableModel;
    ResolveVoteGraph *resolveVoteGraph;
    QVBoxLayout *graphLayout;
    QScrollArea *graphScrollArea;


private slots:
    void onNVotersChange();
    void onNDecisionsChange();
    void onAlphaChange();
    void onTolChange();
    void onNAChange();

public slots:
    void onInputChange(void);
};

#endif // TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H
