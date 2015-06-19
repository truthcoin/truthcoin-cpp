// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_MARKETBRANCHTABLEMODEL_H
#define TRUTHCOIN_QT_MARKETBRANCHTABLEMODEL_H

#include "truthcoinunits.h"

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class MarketBranchTablePriv;
class WalletModel;
class CWallet;

class MarketBranchTableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MarketBranchTableModel(CWallet *, WalletModel * = 0);
    ~MarketBranchTableModel();

    enum ColumnIndex {
        Name = 0,
        Description = 1,
        BaseListingFee = 2,
        FreeDecisions = 3,
        TargetDecisions = 4,
        MaxDecisions = 5,
        MinTradingFee = 6,
        Tau = 7,
        BallotTime = 8,
        UnsealTime = 9,
        ConsensusThreshold = 10,
        Hash = 11,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        DescriptionRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketBranch *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex & parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void setTable(void);

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    MarketBranchTablePriv *priv;

public slots:
    friend class MarketBranchTablePriv;
};

QString formatName(const marketBranch *);
QString formatDescription(const marketBranch *);
QString formatBaseListingFee(const marketBranch *);
QString formatFreeDecisions(const marketBranch *);
QString formatTargetDecisions(const marketBranch *);
QString formatMaxDecisions(const marketBranch *);
QString formatMinTradingFee(const marketBranch *);
QString formatTau(const marketBranch *);
QString formatBallotTime(const marketBranch *);
QString formatUnsealTime(const marketBranch *);
QString formatConsensusThreshold(const marketBranch *);
QString formatHash(const marketBranch *);

#endif // TRUTHCOIN_QT_MARKETBRANCHTABLEMODEL_H
