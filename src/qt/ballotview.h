// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTVIEW_H
#define TRUTHCOIN_QT_BALLOTVIEW_H

#include <vector>
#include "guiutil.h"

#include <QLabel>
#include <QWidget>

class WalletModel;
class marketBranch;

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
QT_END_NAMESPACE

class BallotView
    : public QWidget
{
    Q_OBJECT

public:
    explicit BallotView(QWidget *parent=0);

    void setModel(WalletModel *model);

private:
    void refresh(void);

    WalletModel *model;
    std::vector<marketBranch *> branches;
    marketBranch *branch;
    QLabel *branchLabel;
    QComboBox *branchWidget;
    QLabel *blockNumLabel;
    QLineEdit *blockNumWidget;
    QLabel *minBlockNumLabel;
    QLabel *minBlockNum;
    QLabel *maxBlockNumLabel;
    QLabel *maxBlockNum;
    QLabel *currHeightLabel;
    QLabel *currHeight;
    QVBoxLayout *v2layout;
    QPushButton *submitButton;
    uint32_t blocknum;
    uint32_t minblock;
    uint32_t maxblock;

public slots:
    void changedBranch(int);
    void changedBlock(const QString &);
};

#endif // TRUTHCOIN_QT_BALLOTVIEW_H
