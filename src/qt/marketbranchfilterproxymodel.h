// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETBRANCHFILTERPROXYMODEL_H
#define TRUTHCOIN_QT_MARKETBRANCHFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class MarketBranchFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MarketBranchFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterDescription(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterDescription;
};


#endif // TRUTHCOIN_QT_MARKETBRANCHFILTERPROXYMODEL_H
