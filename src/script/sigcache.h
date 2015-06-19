// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_SCRIPT_SIGCACHE_H
#define TRUTHCOIN_SCRIPT_SIGCACHE_H

#include "script/interpreter.h"

#include <vector>

class CPubKey;

class CachingSignatureChecker : public SignatureChecker
{
private:
    bool store;

public:
    CachingSignatureChecker(const CTransaction& txToIn, unsigned int nInIn, bool storeIn=true) : SignatureChecker(txToIn, nInIn), store(storeIn) {}

    bool VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& vchPubKey, const uint256& sighash) const;
};

#endif // TRUTHCOIN_SCRIPT_SIGCACHE_H
