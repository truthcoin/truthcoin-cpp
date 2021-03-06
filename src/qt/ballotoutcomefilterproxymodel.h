// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTOUTCOMEFILTERPROXYMODEL_H
#define TRUTHCOIN_QT_BALLOTOUTCOMEFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class BallotOutcomeFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BallotOutcomeFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

protected:

private:
};


#endif // TRUTHCOIN_QT_BALLOTOUTCOMEFILTERPROXYMODEL_H
