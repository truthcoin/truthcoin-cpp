// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
#include <stdlib.h>
extern "C" {
#include "linalg/src/tc_mat.h"
}
#include "resolvevoteinputtablemodel.h"
#include "wallet.h"
#include "walletmodel.h"

ResolveVoteInputTableModel::ResolveVoteInputTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    voteptr(0),
    wallet(wallet),
    walletModel(parent)
{

}

ResolveVoteInputTableModel::~ResolveVoteInputTableModel()
{

}

int ResolveVoteInputTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3 + ((voteptr && *voteptr)? (*voteptr)->nr: 0);
}

int ResolveVoteInputTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (voteptr && *voteptr)? (*voteptr)->nc: 0;
}

QVariant ResolveVoteInputTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (!voteptr || !*voteptr)
            return QVariant();

        uint32_t row = index.row();
        uint32_t col = index.column();
        if ((row < 3+(*voteptr)->nr)
            && (col < (*voteptr)->nc))
        {
            double value = 0.0;
            if (row == 0) /* Binary/Scalar */
                value = (*voteptr)->cvecs[TC_VOTE_IS_BINARY]->a[0][col];
            else
            if (row == 1) /* Minimum */
                value = 0.0;
            else
            if (row == 2) /* Maximum */
                value = 1.0;
            else
                value = (*voteptr)->M->a[row-3][col];

            if (value != (*voteptr)->NA) {
                char tmp[32];
                snprintf(tmp, sizeof(tmp), "%.8f", value);
                return QVariant(QString(tmp));
            }
            return QVariant(QString("NA"));
        }
    }
    else
    if (role == Qt::TextAlignmentRole)
        return (int)(Qt::AlignRight|Qt::AlignVCenter);

    return QVariant();
}

QVariant ResolveVoteInputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Decision %u", section+1);
            return QVariant(QString(tmp));
        }
        else
        if (role == Qt::TextAlignmentRole)
            return (int)(Qt::AlignLeft|Qt::AlignVCenter);
        else
        if (role == Qt::ToolTipRole)
        {
        }
    }

    if(orientation == Qt::Vertical)
    {
        if (role == Qt::DisplayRole) {
            if (section == 0) return QVariant(tr("Binary/Scalar"));
            if (section == 1) return QVariant(tr("Minumum"));
            if (section == 2) return QVariant(tr("Maximum"));
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Voter %u", section - 2);
            return QVariant(QString(tmp));
        }
        else
        if (role == Qt::TextAlignmentRole)
            return (int)(Qt::AlignLeft|Qt::AlignVCenter);
        else
        if (role == Qt::ToolTipRole)
        {
        }
    }

    return QVariant();
}

Qt::ItemFlags ResolveVoteInputTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool ResolveVoteInputTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (!index.isValid())
        return false;

    const char *str = value.toString().toStdString().c_str();
    if (!str)
        return false;

    double dvalue = atof(str);

    int row = index.row();
    int col = index.column();
    if ((row < 3+(int)(*voteptr)->nr)
        && (col < (int)(*voteptr)->nc))
    {
        if (row == 0) {
           (*voteptr)->cvecs[TC_VOTE_IS_BINARY]->a[0][col] = (dvalue > 0.5)? 1.0: 0.0;
        }
        else
        if (row == 1) {
        }
        else
        if (row == 2) {
        }
        else {
           (*voteptr)->M->a[row-3][col] = dvalue;
        }

        emit dataChanged(index, index);
    }
    return false;
}

void ResolveVoteInputTableModel::onVoteChange(void)
{

}

