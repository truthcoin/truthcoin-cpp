// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTVOTETABLEMODEL_H
#define TRUTHCOIN_QT_BALLOTVOTETABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketVote;
class BallotVoteTablePriv;
class WalletModel;
class CWallet;


class BallotVoteTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BallotVoteTableModel(CWallet *, WalletModel * = 0);
    ~BallotVoteTableModel();

    enum ColumnIndex {
        Height = 0,
        Address = 1,
        Hash = 2,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketVote *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onBranchChange(const marketBranch *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    BallotVoteTablePriv *priv;

public slots:
    friend class BallotVoteTablePriv;
};

QString formatHeight(const marketVote *);
QString formatAddress(const marketVote *);
QString formatHash(const marketVote *);

#endif // TRUTHCOIN_QT_BALLOTVOTETABLEMODEL_H
