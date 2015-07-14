// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H
#define TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H

#include <QDialog>

struct tc_vote;
class ResolveVoteColTableModel;
class ResolveVoteRowTableModel;
class ResolveVoteInputTableModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QEvent;
class QTableView;
QT_END_NAMESPACE

class ResolveVoteDialog
    : public QDialog
{
    Q_OBJECT

public:
    explicit ResolveVoteDialog(QWidget *parent);
    ~ResolveVoteDialog();
    void setModel(WalletModel *model);
    bool eventFilter(QObject *, QEvent *);

private:
    unsigned int nVoters, nDecisions;
    double NA;
    double alpha;
    double tol;
    struct tc_vote *vote;

    WalletModel *model;
    QTableView *inputTableView;
    ResolveVoteInputTableModel *inputTableModel;
    QTableView *rowTableView;
    ResolveVoteRowTableModel *rowTableModel;
    QTableView *colTableView;
    ResolveVoteColTableModel *colTableModel;

private slots:
    void onAddVoter(void);
    void onAddDecision(void);
    void onCalculate(void);
};

#endif // TRUTHCOIN_QT_RESOLVEVOTEDIALOG_H
