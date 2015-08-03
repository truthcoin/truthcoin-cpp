// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdlib.h>
#include <stdio.h>
#include "marketviewgraph.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QRectF>
#include <QString>


static int qpoint_cmp(const void *aptr, const void *bptr)
{
    const QPoint *a = *(const QPoint **) aptr;
    const QPoint *b = *(const QPoint **) bptr;
    if (a->x() != b->x())
        return (a->x() < b->x())? -1: 1;
    return 0;
}

static void draw_graph(
    QPainter &painter,
    int w, /* width */
    int h, /* height */
    int margin, 
    double *X, /* x-values */
    double *Y, /* y-values */
    unsigned int N /* number of X[], Y[] */
    )
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

    /* draw boundary rectangle */
    QRectF rectangle(x0, y0, x1-x0, y1-y0);
    painter.drawRect(rectangle);

    if (!X || !Y || !N)
       return;

    double Ymin = 0.0;
    double Ymax = 1.0;
    double Xmin = X[0];
    double Xmax = X[0];
    for(uint32_t i=1; i < N; i++) {
       double x = X[i];
       if (x < Xmin) Xmin = x;
       if (x > Xmax) Xmax = x;
    }

    if ((Xmax > Xmin) && (Ymax > Ymin)) {
        /* allocate */
        QPoint **points = (QPoint **) malloc(N*sizeof(QPoint *));

        /* convert to qt coordinate system */
        for(uint32_t i=0; i < N; i++) {
            int x = (int) (x0 + (x1 - x0) * (X[i] - Xmin) / (Xmax - Xmin));
            int y = (int) (y1 - (y1 - y0) * (Y[i] - Ymin) / (Ymax - Ymin));
            points[i] = new QPoint(x, y);
        }

        /* order along horizontal axis */
        qsort((void *)points, N, sizeof(QPoint *), qpoint_cmp);

        /* draw line */
        for(uint32_t i=0; i+1 < N; i++)
            painter.drawLine(*points[i], *points[i+1]);

        int labelwidth = 3*margin/2;
        int labelheight = margin;
        const char *labelfmtX = "%.0f";
        const char *labelfmtY = "%.2f";

        char label[32];
        uint32_t labelsz = sizeof(label) - 1;

        /* draw horizontal labels */
        snprintf(label, labelsz, labelfmtX, Xmin);
        QString XminStr = label;
        QRectF rect0(x0-labelwidth/2, y1, labelwidth, labelheight);
        painter.drawText(rect0, Qt::AlignCenter, XminStr);

        snprintf(label, labelsz, labelfmtX, Xmax);
        QString XmaxStr = label;
        QRectF rect1(x1-labelwidth/2, y1, labelwidth, labelheight);
        painter.drawText(rect1, Qt::AlignCenter, XmaxStr);

        /* draw vertical labels */
        snprintf(label, labelsz, labelfmtY, Ymin);
        QString YminStr = label;
        QRectF rect2(x0-labelwidth, y1-labelheight/2, labelwidth, labelheight);
        painter.drawText(rect2, Qt::AlignCenter, YminStr);

        snprintf(label, labelsz, labelfmtY, Ymax);
        QString YmaxStr = label;
        QRectF rect3(x0-labelwidth, y0-labelheight/2, labelwidth, labelheight);
        painter.drawText(rect3, Qt::AlignCenter, YmaxStr);

        /* clean up */
        for(uint32_t i=0; i < N; i++)
            delete points[i];
        free(points);
    }
    else
    if ((Xmin == Xmax) && (Ymin == Ymax)) {
        /* draw point */
    }
    else
    if (Xmin == Xmax) {
        /* draw vertical line */
    }
    else
    if (Ymin == Ymax) {
        /* draw horizonal line */
    }
}

MarketViewGraph::MarketViewGraph(QWidget *parent)
    : QWidget(parent),
    X(0), Y(0), N(0),
    dataIsChanging(false)
{

}

void MarketViewGraph::paintEvent(QPaintEvent *)
{
    if (dataIsChanging)
       return;

    QPainter painter(this);
    unsigned int margin = 20;
    draw_graph(painter, width(), height(), margin, X, Y, N);
}

void MarketViewGraph::setData(const double *X_, const double *Y_, unsigned int N_)
{
    dataIsChanging = true;

    /* clear data */
    if (X) delete X;
    X = (double *)0;
    if (Y) delete Y;
    Y = (double *)0;
    N = 0;

    if (X_ && Y_ && N_) {
        N = N_;

        X = new double [N];
        for(uint32_t i=0; i < N; i++)
            X[i] = X_[i];

        Y = new double [N];
        for(uint32_t i=0; i < N; i++)
            Y[i] = Y_[i];
    }

    dataIsChanging = false;
}

