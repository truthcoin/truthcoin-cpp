// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_INIT_H
#define TRUTHCOIN_INIT_H

#include <string>

class CWallet;

namespace boost
{
class thread_group;
} // namespace boost

extern CWallet* pwalletMain;

void StartShutdown();
bool ShutdownRequested();
void Shutdown();
bool AppInit2(boost::thread_group& threadGroup);

/** The help message mode determines what help message to show */
enum HelpMessageMode {
    HMM_TRUTHCOIND,
    HMM_TRUTHCOIN_QT
};

/** Help for options shared between UI and daemon (for -help) */
std::string HelpMessage(HelpMessageMode mode);
/** Returns licensing information (for -version) */
std::string LicenseInfo();

#endif // TRUTHCOIN_INIT_H
