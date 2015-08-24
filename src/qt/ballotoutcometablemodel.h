// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_BALLOTOUTCOMETABLEMODEL_H
#define TRUTHCOIN_QT_BALLOTOUTCOMETABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketOutcome;
class BallotOutcomeTablePriv;
class WalletModel;
class CWallet;


class BallotOutcomeTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BallotOutcomeTableModel(CWallet *, WalletModel * = 0);
    ~BallotOutcomeTableModel();

    enum ColumnIndex {
        Height = 0,
        Hash = 1,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketOutcome *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onBranchChange(const marketBranch *);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    BallotOutcomeTablePriv *priv;

public slots:
    friend class BallotOutcomeTablePriv;
};

QString formatHeight(const marketOutcome *);
QString formatHash(const marketOutcome *);

#endif // TRUTHCOIN_QT_BALLOTOUTCOMETABLEMODEL_H
