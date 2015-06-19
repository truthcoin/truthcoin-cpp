// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketbranchfilterproxymodel.h"
#include "marketbranchtablemodel.h"


bool MarketBranchFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString description = index.data(MarketBranchTableModel::DescriptionRole).toString();
    if (!description.contains(filterDescription, Qt::CaseInsensitive))
        return false;

    return true;
}

void MarketBranchFilterProxyModel::setFilterDescription(const QString &str)
{
    filterDescription = (str.size() >= 2)? str: "";
    invalidateFilter();
}

