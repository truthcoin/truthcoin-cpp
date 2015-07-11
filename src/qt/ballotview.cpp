// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ballotview.h"

#include "chain.h"
#include "guiutil.h"
#include "main.h"
#include "primitives/market.h"
#include "txdb.h"
#include "walletmodel.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

extern CMarketTreeDB *pmarkettree;
extern CChain chainActive;

QWidget *scrollWidget = NULL;
QScrollArea *scrollArea = NULL;

bool branchptrcmp(const marketBranch *aptr, const marketBranch *bptr)
{
    return (aptr->name.compare(bptr->name) <= 0);
}

BallotView::BallotView(QWidget *parent)
    : QWidget(parent), model(0), branch(0)
{
    /* window is a vlayout containing        */
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
    scrollArea = new QScrollArea();
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(false);

    scrollWidget = new QWidget();
    // QGridLayout *scrollLayout = new QGridLayout(scrollWidget);
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
}


void BallotView::setModel(WalletModel *model)
{
    this->model = model;
}

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
    branch = ((i < 0) || (((unsigned int) i) >= branches.size()))? 0: branches[i];
    refresh();
}

void BallotView::changedBlock(const QString &str)
{
    blocknum = str.toInt();
    refresh();
}

