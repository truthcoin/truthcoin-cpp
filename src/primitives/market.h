// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRUTHCOIN_PIMITIVES_MARKET_H
#define TRUTHCOIN_PIMITIVES_MARKET_H

#include <limits.h>
#include <stdint.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "pubkey.h"
#include "script/script.h"
#include "serialize.h"
#include "primitives/transaction.h"
#include "uint256.h"

using namespace std;

struct marketObj {
    char marketop;
    uint32_t nHeight;
    uint256 txid;

    marketObj(void): nHeight(INT_MAX) { } 
    virtual ~marketObj(void) { } 

    uint256 GetHash(void) const;
    CScript GetScript(void) const;
    virtual string ToString(void) const; 
};
marketObj *marketObjCtr(const CScript &);

struct marketObjBlockIndexLess {
    inline bool operator()(const marketObj *a, const marketObj *b)
    {
        return (a->nHeight < b->nHeight)? true: false;
    }
};

struct marketDecision : public marketObj {
    CKeyID keyID;
    uint256 branchid;
    string prompt;
    uint32_t eventOverBy;
    uint8_t isScaled;
    int64_t min;
    int64_t max;
    uint8_t answerOptionality; /* false=not optional, true=optional */

    marketDecision(void) : marketObj() { marketop = 'D'; } 
    virtual ~marketDecision(void) { } 

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(marketop);
        READWRITE(keyID);
        READWRITE(branchid);
        READWRITE(prompt);
        READWRITE(eventOverBy);
        READWRITE(isScaled);
        READWRITE(min);
        READWRITE(max);
        READWRITE(answerOptionality);
    }

    string ToString(void) const;
};

struct marketTrade : public marketObj {
    CKeyID keyID;
    uint256 marketid;
    bool isBuy;
    uint64_t nShares;
    uint64_t price;
    uint32_t decisionState;
    uint32_t nonce;
    uint32_t blockNum;

    marketTrade(void) : marketObj() { marketop = 'T'; } 
    virtual ~marketTrade(void) { } 

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(marketop);
        READWRITE(keyID);
        READWRITE(marketid);
        READWRITE(isBuy);
        READWRITE(nShares);
        READWRITE(price);
        READWRITE(decisionState);
        READWRITE(nonce);
    }

    string ToString(void) const;
};

enum decisionfunctionid {
	DFID_X1 = 1,
	DFID_X2 = 2,
	DFID_X3 = 3,
	DFID_LNX1 = 4,
};

inline int decisionFunctionToInt(const string &s) {
    if (s == "X1") return DFID_X1;
    if (s == "X2") return DFID_X2;
    if (s == "X3") return DFID_X3;
    if (s == "LNX1") return DFID_LNX1;
    return -1;
}

inline string decisionFunctionIDToString(int i) {
    if (i == DFID_X1) return "X1";
    if (i == DFID_X2) return "X2";
    if (i == DFID_X3) return "X3";
    if (i == DFID_LNX1) return "LNX1";
    return "";
}

struct marketMarket : public marketObj {
    CKeyID keyID;
    uint64_t B;
    uint64_t tradingFee;
    uint64_t maxCommission;
    string title;
    string description;
    string tags;
    uint32_t maturation;
    uint256 branchid;
    vector<uint256> decisionIDs;
    vector<uint8_t> decisionFunctionIDs;
    uint64_t account;
    uint32_t txPoW;
    map<uint256, marketTrade *> trades; /* owner of memory */
    multiset<marketTrade *, marketObjBlockIndexLess> orderedTrades;

    marketMarket(void) : marketObj() { marketop = 'M'; } 
    virtual ~marketMarket(void) { 
        std::map<uint256, marketTrade *>::iterator it;
        for(it=trades.begin(); it != trades.end(); it++)
            delete it->second;
    } 

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(marketop);
        READWRITE(keyID);
        READWRITE(B);
        READWRITE(tradingFee);
        READWRITE(maxCommission);
        READWRITE(title);
        READWRITE(description);
        READWRITE(tags);
        READWRITE(maturation);
        READWRITE(branchid);
        READWRITE(decisionIDs);
        READWRITE(decisionFunctionIDs);
        READWRITE(txPoW);
    }

    string ToString(void) const;
    void getNShares(double &, double &) const;
    double getAccount(double, double) const;
};

double marketMarket_capitalrequired(
    double B /* liquidity parameter */,
    uint32_t nstates);

struct marketVote : public marketObj {
    uint256 branchid;
    uint32_t height; /* a multiple of tau */
    vector<uint256> decisionIDs;
    vector<uint64_t> decisionVotes;
    uint64_t NA;
    CKeyID keyID;

    marketVote(void) : marketObj() { marketop = 'V'; } 
    virtual ~marketVote(void) { } 

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(marketop);
        READWRITE(branchid);
        READWRITE(height);
        READWRITE(decisionIDs);
        READWRITE(decisionVotes);
        READWRITE(NA);
        READWRITE(keyID);
    }

    string ToString(void) const;
};

struct marketOutcome : public marketObj {
    uint256 branchid;
    /* size() == nVoters */
    uint32_t nVoters;
    vector<CKeyID> voterIDs;
    vector<uint64_t> oldRep;
    vector<uint64_t> thisRep; /* output */
    vector<uint64_t> smoothedRep; /* output */
    vector<uint64_t> NARow; /* output */
    vector<uint64_t> particRow; /* output */
    vector<uint64_t> particRel; /* output */
    vector<uint64_t> rowBonus; /* output */
    /* size() == nDecisions */
    uint32_t nDecisions;
    vector<uint256> decisionIDs;
    vector<uint64_t> isScaled;
    vector<uint64_t> firstLoading; /* output */
    vector<uint64_t> decisionsRaw; /* output */
    vector<uint64_t> consensusReward; /* output */
    vector<uint64_t> certainty; /* output */
    vector<uint64_t> NACol; /* output */
    vector<uint64_t> particCol; /* output */
    vector<uint64_t> authorBonus; /* output */
    vector<uint64_t> decisionsFinal; /* output */
    vector<uint64_t> voteMatrix; /* [nVoters][nDecisions] */
    /* params */
    uint64_t NA;
    uint64_t alpha;
    uint64_t tol;
    CTransaction tx; /* transaction */

    marketOutcome(void) : marketObj() { marketop = 'O'; } 
    virtual ~marketOutcome(void) { };

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(branchid);
        READWRITE(voterIDs);
        READWRITE(nVoters);
        READWRITE(oldRep);
        READWRITE(thisRep);
        READWRITE(smoothedRep);
        READWRITE(NARow);
        READWRITE(particRow);
        READWRITE(particRel);
        READWRITE(rowBonus);
        READWRITE(nDecisions);
        READWRITE(decisionIDs);
        READWRITE(isScaled);
        READWRITE(firstLoading);
        READWRITE(decisionsRaw);
        READWRITE(consensusReward);
        READWRITE(certainty);
        READWRITE(NACol);
        READWRITE(particCol);
        READWRITE(authorBonus);
        READWRITE(decisionsFinal);
        READWRITE(voteMatrix);
        READWRITE(NA);
        READWRITE(alpha);
        READWRITE(tol);
    }
    string ToString(void) const;
    int calc(void);
};

struct marketBallot : public marketObj {
    uint32_t height; /* a multiple of tau */
    map<uint256, marketVote *> votes; /* owner of memory */

    marketBallot(void) : marketObj() { marketop = 'b'; } 
    virtual ~marketBallot(void) {
        std::map<uint256, marketVote *>::iterator vit;
        for(vit=votes.begin(); vit != votes.end(); vit++)
            delete vit->second;
    }
};

/* market Branch 
 * decisions partitioned via ending times in blocks ((n-1)*tau, n*tau]
 * ballots available at block n*tau
 * submit sealed ballots during (n*tau, n*tau+ballotTime]
 * submit unsealed ballots during (n*tau, n*tau+ballotTime+unsealTime]
 * outcomes decided by the miner for block n*tau+ballotTime+unsealTime+1
 */
struct marketBranch : public marketObj {
    string name;
    string description;
    uint64_t baseListingFee;
    uint16_t freeDecisions;
    uint16_t targetDecisions;
    uint16_t maxDecisions;
    uint64_t minTradingFee;
    uint16_t tau;
    uint16_t ballotTime;
    uint16_t unsealTime;
    uint64_t consensusThreshold;

    map<uint256, marketDecision *> decisions; /* owner of memory */
    map<uint256, marketMarket *> markets; /* owner of memory */
    map<uint256, marketOutcome *> outcomes; /* owner of memory */
    map<uint32_t, marketBallot *> ballots; /* owner of memory */
    CTransaction tx; /* last vote transaction. TODO: adjust according to forks  */

    marketBranch(void) : marketObj() { marketop = 'B'; } 
    virtual ~marketBranch(void) {
        std::map<uint256, marketDecision *>::iterator dit;
        for(dit=decisions.begin(); dit != decisions.end(); dit++)
            delete dit->second;
        std::map<uint256, marketMarket *>::iterator mit;
        for(mit=markets.begin(); mit != markets.end(); mit++)
            delete mit->second;
        std::map<uint256, marketOutcome *>::iterator oit;
        for(oit=outcomes.begin(); oit != outcomes.end(); oit++)
            delete oit->second;
    } 

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(marketop);
        READWRITE(name);
        READWRITE(description);
        READWRITE(baseListingFee);
        READWRITE(freeDecisions);
        READWRITE(targetDecisions);
        READWRITE(maxDecisions);
        READWRITE(minTradingFee);
        READWRITE(tau);
        READWRITE(ballotTime);
        READWRITE(unsealTime);
        READWRITE(consensusThreshold);
    }
    string ToString(void) const;
};


#endif // TRUTHCOIN_PRIMITIVES_MARKET_H
