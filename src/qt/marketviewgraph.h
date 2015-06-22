// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETVIEWGRAPH_H
#define TRUTHCOIN_QT_MARKETVIEWGRAPH_H

#include <QWidget>

class MarketView;

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE


class MarketViewGraph
   : public QWidget
{
    Q_OBJECT

public:
    explicit MarketViewGraph(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
    void setData(const double *X, const double *Y, unsigned int N);

private:
    MarketView *marketView;

    /* data */
    double *X;
    double *Y;
    unsigned int N;
    bool dataIsChanging;

signals:

public slots:
};

#endif // TRUTHCOIN_QT_MARKETVIEWGRAPH_H
