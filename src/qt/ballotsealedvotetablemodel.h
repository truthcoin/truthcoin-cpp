// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTSEALEDVOTETABLEMODEL_H
#define TRUTHCOIN_QT_BALLOTSEALEDVOTETABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketSealedVote;
class BallotSealedVoteTablePriv;
class WalletModel;
class CWallet;


class BallotSealedVoteTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BallotSealedVoteTableModel(CWallet *, WalletModel * = 0);
    ~BallotSealedVoteTableModel();

    enum ColumnIndex {
        Height = 0,
        VoteID = 1,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketSealedVote *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onBranchChange(const marketBranch *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    BallotSealedVoteTablePriv *priv;

public slots:
    friend class BallotSealedVoteTablePriv;
};

QString formatHeight(const marketSealedVote *);
QString formatVoteID(const marketSealedVote *);
QString formatHash(const marketSealedVote *);

#endif // TRUTHCOIN_QT_BALLOTSEALEDVOTETABLEMODEL_H
