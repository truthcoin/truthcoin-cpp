// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_RESOLVEVOTEINPUTTABLEMODEL_H
#define TRUTHCOIN_QT_RESOLVEVOTEINPUTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>

struct tc_vote;
class CWallet;
class WalletModel;

class ResolveVoteInputTableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ResolveVoteInputTableModel(CWallet *, WalletModel * = 0);
    ~ResolveVoteInputTableModel();
    void setVotePtr(tc_vote **ptr) { voteptr = ptr; }
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    void onVoteChange(void);

private:
    tc_vote **voteptr;
    CWallet *wallet;
    WalletModel *walletModel;

public slots:
};

#endif // TRUTHCOIN_QT_RESOLVEVOTEINPUTTABLEMODEL_H
