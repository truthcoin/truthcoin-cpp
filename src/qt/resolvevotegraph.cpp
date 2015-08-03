// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdlib.h>
#include <stdio.h>
extern "C" {
#include "linalg/src/tc_mat.h"
}
#include "resolvevotegraph.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QRectF>
#include <QString>


static void draw_graph(
    QPainter &painter,
    int w, /* width */
    int h, /* height */
    int margin, 
    const struct tc_vote **vote_ptr)
{
    if ((w <= 3*margin) || (h <= 2*margin))
       return;

    /* Qt coordinate system */
    int x0 = 2*margin;
    int x1 = w - margin;
    int y0 = margin;
    int y1 = h - margin;

    if (x0 >= x1) return;
    if (y0 >= y1) return;


    /* draw labels */
    QString Title("Plot of Judgement Space");
    QRectF rect0(x1/2-4*margin, 0, 8*margin, margin);
    painter.drawText(rect0, Qt::AlignCenter, Title);

    QString Xaxis("Outcome");
    QRectF rect1(x1/2-2*margin, y1, 4*margin, margin);
    painter.drawText(rect1, Qt::AlignCenter, Xaxis);

    QString Yaxis("Unscaled Votes");
    QRectF rect2(-y1/2-2*margin, 0, 4*margin, margin);
    painter.rotate(-90);
    painter.drawText(rect2, Qt::AlignCenter, Yaxis);
    painter.rotate(90);

    if (!vote_ptr || !*vote_ptr) {
        /* draw boundary rectangle */
        QRectF rect(x0, y0, x1-x0, y1-y0);
        painter.drawRect(rect);

        return;
    }

    const struct tc_vote *vote = *vote_ptr;
    uint32_t nDecisions = vote->nc;
    uint32_t nVoters = vote->nr;
    if (nDecisions) {
        uint32_t *colors = new uint32_t [nVoters];
        colors[0] = 0xFF0000;
        colors[1] = 0x00FF00;
        colors[2] = 0x0000FF;
        colors[3] = 0xFFFF00;
        colors[4] = 0x00FFFF;
        colors[5] = 0xFF00FF;
        uint32_t N = (nVoters + 6) / 6;
        for(uint32_t i=6; i < nVoters; i++) {
            uint32_t I = i / 6;
            colors[i] = (colors[i%6] * (N - I)) / N;
        }

        uint32_t rh = (y1 - y0)/(nDecisions);
        int Rectwidth = x1 - x0;
        int rectwidth = Rectwidth / 10;
        int Rectheight = rh - 2;
        int rectheight = Rectheight / nVoters;
        for(uint32_t i=0; i < nDecisions; i++) {
            /* draw box */
            QRectF rect1(margin, margin+rh*i, Rectwidth, Rectheight);
            painter.setBrush(Qt::NoBrush);
            painter.setPen(Qt::black);
            painter.drawRect(rect1);

            /* draw label */
            char tmp[16];
            snprintf(tmp, sizeof(tmp), "Dec %d", i);
            QString label(tmp);

            QRectF rect2(margin+rh*i, -(x1-x0)-2*margin, 2*margin, margin);
            painter.rotate(90);
            painter.drawText(rect2, Qt::AlignCenter, label);
            painter.rotate(-90);

            for(uint32_t j=0; j < nVoters; j++) {
                double dvalue = vote->M->a[j][i];
                if (dvalue == vote->NA)
                    continue;
                uint32_t r = (colors[j] & 0xFF0000) >> 16;
                uint32_t g = (colors[j] & 0x00FF00) >>  8;
                uint32_t b = (colors[j] & 0x0000FF) >>  0;
                painter.setBrush(QColor(r,g,b));
                painter.setPen(Qt::black);
                int x0 = margin + dvalue*(Rectwidth - rectwidth);
                int y0 = margin + rh*i + (nVoters - 1 - j) * rectheight;
                QRectF rect(x0, y0, rectwidth, rectheight);
                painter.drawRect(rect);
            }
        }
        delete [] colors;
    }
}

ResolveVoteGraph::ResolveVoteGraph(QWidget *parent)
    : QWidget(parent),
    vote_ptr(0)
{

}

void ResolveVoteGraph::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    uint32_t margin = 20;
    draw_graph(painter, width(), height(), margin, vote_ptr);
}

