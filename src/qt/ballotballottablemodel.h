// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTBALLOTTABLEMODEL_H
#define TRUTHCOIN_QT_BALLOTBALLOTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class BallotBallotTablePriv;
class WalletModel;
class CWallet;


struct marketPair {
    unsigned int Height;
    unsigned int nDecisions;
};

class BallotBallotTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BallotBallotTableModel(CWallet *, WalletModel * = 0);
    ~BallotBallotTableModel();

    enum ColumnIndex {
        Height = 0,
        nDecisions = 1,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketPair *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onBranchChange(const marketBranch *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    BallotBallotTablePriv *priv;

public slots:
    friend class BallotBallotTablePriv;
};

QString formatHeight(const marketPair *);
QString formatNDecisions(const marketPair *);

#endif // TRUTHCOIN_QT_BALLOTBALLOTTABLEMODEL_H
