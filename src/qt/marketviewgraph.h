// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETVIEWGRAPH_H
#define TRUTHCOIN_QT_MARKETVIEWGRAPH_H

#include <stdint.h>
#include <QWidget>


class MarketViewGraph
   : public QWidget
{
    Q_OBJECT

public:
    explicit MarketViewGraph(QWidget *parent=0);
    void paintEvent(QPaintEvent *);
    void setData(const double *X, const double *Y, uint32_t N);

private:
    double *X;  /* X[] */
    double *Y;  /* Y[] */
    uint32_t N;
    bool dataIsChanging;
};

#endif // TRUTHCOIN_QT_MARKETVIEWGRAPH_H
