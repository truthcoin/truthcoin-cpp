// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "marketdecisionfilterproxymodel.h"
#include "marketdecisiontablemodel.h"


bool MarketDecisionFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString address = index.data(MarketDecisionTableModel::AddressRole).toString();
    if (!address.contains(filterAddress, Qt::CaseSensitive))
        return false;

    QString prompt = index.data(MarketDecisionTableModel::PromptRole).toString();
    if (!prompt.contains(filterPrompt, Qt::CaseInsensitive))
        return false;

    return true;
}

void MarketDecisionFilterProxyModel::setFilterAddress(const QString &str)
{
    filterAddress = (str.size() >= 2)? str: "";
    invalidateFilter();
}

void MarketDecisionFilterProxyModel::setFilterPrompt(const QString &str)
{
    filterPrompt = (str.size() >= 2)? str: "";
    invalidateFilter();
}

