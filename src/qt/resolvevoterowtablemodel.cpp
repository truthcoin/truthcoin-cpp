// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
extern "C" {
#include "linalg/src/tc_mat.h"
}
#include "resolvevoterowtablemodel.h"


ResolveVoteRowTableModel::ResolveVoteRowTableModel()
    : QAbstractTableModel(0),
    voteptr(0)
{
}

ResolveVoteRowTableModel::~ResolveVoteRowTableModel()
{

}

int ResolveVoteRowTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (voteptr && *voteptr)? (*voteptr)->nr: 0;
}

int ResolveVoteRowTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TC_VOTE_NROWS;
}

QVariant ResolveVoteRowTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (!voteptr || !*voteptr)
            return QVariant();

        uint32_t row = index.row();
        uint32_t col = index.column();
        if ((row < (*voteptr)->nr)
            && (col < TC_VOTE_NROWS))
        {
            double value = (*voteptr)->rvecs[col]->a[row][0];
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

QVariant ResolveVoteRowTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
    {
        if(role == Qt::DisplayRole)
        {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Voter %u", (section+1));
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
    else
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            switch(section)
            {
            case 0: return tr("Old Rep");
            case 1: return tr("This Rep");
            case 2: return tr("Smoothed Rep");
            case 3: return tr("NA");
            case 4: return tr("Partic Row");
            case 5: return tr("Partic Rel");
            case 6: return tr("Row Bonus");
            default: return tr("");
            }
        }
        else
        if (role == Qt::TextAlignmentRole)
            return (int)(Qt::AlignLeft|Qt::AlignVCenter);
        else
        if (role == Qt::ToolTipRole)
        {
            switch(section)
            {
            case 0: return tr("Old Reputation");
            case 1: return tr("This Reputation");
            case 2: return tr("Smoothed Reputation");
            case 3: return tr("NA");
            case 4: return tr("Participation Row");
            case 5: return tr("Participation Rel");
            case 6: return tr("Row Bonus");
            default: return tr("");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags ResolveVoteRowTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ResolveVoteRowTableModel::onVoteChange(unsigned int rmax, unsigned int cmax)
{
    emit dataChanged( index(0,0,QModelIndex()), index(rmax,cmax,QModelIndex()) );
}


