// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETDECISIONFILTERPROXYMODEL_H
#define TRUTHCOIN_QT_MARKETDECISIONFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class MarketDecisionFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MarketDecisionFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterAddress(const QString &);
    void setFilterPrompt(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterAddress;
    QString filterPrompt;
};


#endif // TRUTHCOIN_QT_MARKETDECISIONFILTERPROXYMODEL_H
