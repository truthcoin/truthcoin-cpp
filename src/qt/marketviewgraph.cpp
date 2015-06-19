// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketviewgraph.h"
#include "marketview.h"

#include <QPainter>


MarketViewGraph::MarketViewGraph(QWidget *parent)
    : QWidget(parent),
    marketView((MarketView *)parent)
{

}

void MarketViewGraph::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    unsigned int h = height();
    unsigned int w = width();
    if((h > 20) && (w > 20))
    {
        QRectF rectangle(10.0, 10.0, w-20, h-20);
/*
        int startAngle = 30 * 16;
        int spanAngle = 120 * 16;
        painter.drawArc(rectangle, startAngle, spanAngle);
*/
        painter.drawRect(rectangle);
    }
}

