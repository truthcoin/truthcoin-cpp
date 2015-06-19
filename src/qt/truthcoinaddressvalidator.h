// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_QT_TRUTHCOINADDRESSVALIDATOR_H
#define TRUTHCOIN_QT_TRUTHCOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class TruthcoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit TruthcoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Truthcoin address widget validator, checks for a valid truthcoin address.
 */
class TruthcoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit TruthcoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // TRUTHCOIN_QT_TRUTHCOINADDRESSVALIDATOR_H
