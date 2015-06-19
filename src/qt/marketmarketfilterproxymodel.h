// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETMARKETFILTERPROXYMODEL_H
#define TRUTHCOIN_QT_MARKETMARKETFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class MarketMarketFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MarketMarketFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterAddress(const QString &);
    void setFilterTitle(const QString &);
    void setFilterDescription(const QString &);
    void setFilterTag(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterAddress;
    QString filterTitle;
    QString filterDescription;
    QString filterTag;
};


#endif // TRUTHCOIN_QT_MARKETMARKETFILTERPROXYMODEL_H
