// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_RESOLVEVOTEGRAPH_H
#define TRUTHCOIN_QT_RESOLVEVOTEGRAPH_H

#include <stdint.h>
#include <QWidget>

struct tc_vote;

class ResolveVoteGraph
   : public QWidget
{
    Q_OBJECT

public:
    ResolveVoteGraph(QWidget *parent=0);

    void paintEvent(QPaintEvent *);
    void setVotePtr(const struct tc_vote **ptr) { vote_ptr = ptr; }

private:
    const struct tc_vote **vote_ptr;
};

#endif // TRUTHCOIN_QT_RESOLVEVOTEGRAPH_H
