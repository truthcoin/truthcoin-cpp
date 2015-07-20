// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_RESOLVEVOTEROWTABLEMODEL_H
#define TRUTHCOIN_QT_RESOLVEVOTEROWTABLEMODEL_H

#include <QAbstractTableModel>

struct tc_vote;

class ResolveVoteRowTableModel
    : public QAbstractTableModel
{
    Q_OBJECT

public:
    ResolveVoteRowTableModel();
    ~ResolveVoteRowTableModel();
    void setVotePtr(tc_vote **ptr) { voteptr = ptr; }
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onVoteChange(unsigned int, unsigned int);
    void callBeginInsertColumns(const QModelIndex &parent, int first, int last) {
        beginInsertColumns(parent, first, last); }
    void callEndInsertColumns(void) { endInsertColumns(); }
    void callBeginRemoveColumns(const QModelIndex &parent, int first, int last) {
        beginRemoveColumns(parent, first, last); }
    void callEndRemoveColumns(void) { endRemoveColumns(); }
    void callBeginInsertRows(const QModelIndex &parent, int first, int last) {
        beginInsertRows(parent, first, last); }
    void callEndInsertRows(void) { endInsertRows(); }
    void callBeginRemoveRows(const QModelIndex &parent, int first, int last) {
        beginRemoveRows(parent, first, last); }
    void callEndRemoveRows(void) { endRemoveRows(); }

private:
    tc_vote **voteptr;

public slots:
};

#endif // TRUTHCOIN_QT_RESOLVEVOTEROWTABLEMODEL_H
