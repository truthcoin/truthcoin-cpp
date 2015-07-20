// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
extern "C"{
#include "linalg/src/tc_mat.h"
}
#include "resolvevotecoltablemodel.h"


ResolveVoteColTableModel::ResolveVoteColTableModel()
    : QAbstractTableModel(0),
    voteptr(0)
{
}

ResolveVoteColTableModel::~ResolveVoteColTableModel()
{

}

int ResolveVoteColTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TC_VOTE_NCOLS;
}

int ResolveVoteColTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (voteptr && *voteptr)? (*voteptr)->nc: 0;
}

QVariant ResolveVoteColTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (!voteptr || !*voteptr)
            return QVariant();

        uint32_t row = index.row();
        uint32_t col = index.column();
        if ((row < TC_VOTE_NCOLS)
            && (col < (*voteptr)->nc))
        {
            double value = (*voteptr)->cvecs[row]->a[0][col];
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

QVariant ResolveVoteColTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Decision %u", (section+1));
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
    if(orientation == Qt::Vertical)
    {
        if(role == Qt::DisplayRole)
        {
            switch(section)
            {
            case 0: return tr("Is Binary");
            case 1: return tr("First Loading");
            case 2: return tr("Decision Raw");
            case 3: return tr("Consensus");
            case 4: return tr("Certainty");
            case 5: return tr("NA Col");
            case 6: return tr("Partic Col");
            case 7: return tr("Author Bonus");
            case 8: return tr("Decision Final");
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
            case 0: return tr("Is Binary");
            case 1: return tr("First Loading");
            case 2: return tr("Decision Raw");
            case 3: return tr("Consensus");
            case 4: return tr("Certainty");
            case 5: return tr("NA Column");
            case 6: return tr("Participation Column");
            case 7: return tr("Author Bonus");
            case 8: return tr("Decision Final");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags ResolveVoteColTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ResolveVoteColTableModel::onVoteChange(unsigned int rmax, unsigned int cmax)
{
    emit dataChanged( index(0,0,QModelIndex()), index(rmax,cmax,QModelIndex()) );
}


