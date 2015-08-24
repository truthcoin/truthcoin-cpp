// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015 The Truthcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdint.h>
#include <boost/thread.hpp>
#include "pow.h"
#include "txdb.h"
#include "uint256.h"

using namespace std;

void static BatchWriteCoins(CLevelDBBatch &batch, const uint256 &hash, const CCoins &coins) {
    if (coins.IsPruned())
        batch.Erase(make_pair('c', hash));
    else
        batch.Write(make_pair('c', hash), coins);
}

void static BatchWriteHashBestChain(CLevelDBBatch &batch, const uint256 &hash) {
    batch.Write('B', hash);
}

CCoinsViewDB::CCoinsViewDB(size_t nCacheSize, bool fMemory, bool fWipe) : db(GetDataDir() / "chainstate", nCacheSize, fMemory, fWipe) {
}

bool CCoinsViewDB::GetCoins(const uint256 &txid, CCoins &coins) const {
    return db.Read(make_pair('c', txid), coins);
}

bool CCoinsViewDB::HaveCoins(const uint256 &txid) const {
    return db.Exists(make_pair('c', txid));
}

uint256 CCoinsViewDB::GetBestBlock() const {
    uint256 hashBestChain;
    if (!db.Read('B', hashBestChain))
        return uint256();
    return hashBestChain;
}

bool CCoinsViewDB::BatchWrite(CCoinsMap &mapCoins, const uint256 &hashBlock) {
    CLevelDBBatch batch;
    size_t count = 0;
    size_t changed = 0;
    for (CCoinsMap::iterator it = mapCoins.begin(); it != mapCoins.end();) {
        if (it->second.flags & CCoinsCacheEntry::DIRTY) {
            BatchWriteCoins(batch, it->first, it->second.coins);
            changed++;
        }
        count++;
        CCoinsMap::iterator itOld = it++;
        mapCoins.erase(itOld);
    }
    if (!hashBlock.IsNull())
        BatchWriteHashBestChain(batch, hashBlock);

    LogPrint("coindb", "Committing %u changed transactions (out of %u) to coin database...\n", (unsigned int)changed, (unsigned int)count);
    return db.WriteBatch(batch);
}

CBlockTreeDB::CBlockTreeDB(size_t nCacheSize, bool fMemory, bool fWipe) : CLevelDBWrapper(GetDataDir() / "blocks" / "index", nCacheSize, fMemory, fWipe) {
}

bool CBlockTreeDB::ReadBlockFileInfo(int nFile, CBlockFileInfo &info) {
    return Read(make_pair('f', nFile), info);
}

bool CBlockTreeDB::WriteReindexing(bool fReindexing) {
    if (fReindexing)
        return Write('R', '1');
    else
        return Erase('R');
}

bool CBlockTreeDB::ReadReindexing(bool &fReindexing) {
    fReindexing = Exists('R');
    return true;
}

bool CBlockTreeDB::ReadLastBlockFile(int &nFile) {
    return Read('l', nFile);
}

bool CCoinsViewDB::GetStats(CCoinsStats &stats) const {
    /* It seems that there are no "const iterators" for LevelDB.  Since we
       only need read operations on it, use a const-cast to get around
       that restriction.  */
    boost::scoped_ptr<leveldb::Iterator> pcursor(const_cast<CLevelDBWrapper*>(&db)->NewIterator());
    pcursor->SeekToFirst();

    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    stats.hashBlock = GetBestBlock();
    ss << stats.hashBlock;
    CAmount nTotalAmount = 0;
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            char chType;
            ssKey >> chType;
            if (chType == 'c') {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);
                CCoins coins;
                ssValue >> coins;
                uint256 txhash;
                ssKey >> txhash;
                ss << txhash;
                ss << VARINT(coins.nVersion);
                ss << (coins.fCoinBase ? 'c' : 'n');
                ss << VARINT(coins.nHeight);
                stats.nTransactions++;
                for (unsigned int i=0; i<coins.vout.size(); i++) {
                    const CTxOut &out = coins.vout[i];
                    if (!out.IsNull()) {
                        stats.nTransactionOutputs++;
                        ss << VARINT(i+1);
                        ss << out;
                        nTotalAmount += out.nValue;
                    }
                }
                stats.nSerializedSize += 32 + slValue.size();
                ss << VARINT(0);
            }
            pcursor->Next();
        } catch (const std::exception& e) {
            return error("%s : Deserialize or I/O error - %s", __func__, e.what());
        }
    }
    stats.nHeight = mapBlockIndex.find(GetBestBlock())->second->nHeight;
    stats.hashSerialized = ss.GetHash();
    stats.nTotalAmount = nTotalAmount;
    return true;
}

bool CBlockTreeDB::WriteBatchSync(const std::vector<std::pair<int, const CBlockFileInfo*> >& fileInfo, int nLastFile, const std::vector<const CBlockIndex*>& blockinfo) {
    CLevelDBBatch batch;
    for (std::vector<std::pair<int, const CBlockFileInfo*> >::const_iterator it=fileInfo.begin(); it != fileInfo.end(); it++) {
        batch.Write(make_pair('f', it->first), *it->second);
    }
    batch.Write('l', nLastFile);
    for (std::vector<const CBlockIndex*>::const_iterator it=blockinfo.begin(); it != blockinfo.end(); it++) {
        batch.Write(make_pair('b', (*it)->GetBlockHash()), CDiskBlockIndex(*it));
    }
    return WriteBatch(batch, true);
}

bool CBlockTreeDB::ReadTxIndex(const uint256 &txid, CDiskTxPos &pos) {
    return Read(make_pair('t', txid), pos);
}

bool CBlockTreeDB::WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> >&vect) {
    CLevelDBBatch batch;
    for (std::vector<std::pair<uint256,CDiskTxPos> >::const_iterator it=vect.begin(); it!=vect.end(); it++)
        batch.Write(make_pair('t', it->first), it->second);
    return WriteBatch(batch);
}

bool CBlockTreeDB::WriteFlag(const std::string &name, bool fValue) {
    return Write(std::make_pair('F', name), fValue ? '1' : '0');
}

bool CBlockTreeDB::ReadFlag(const std::string &name, bool &fValue) {
    char ch;
    if (!Read(std::make_pair('F', name), ch))
        return false;
    fValue = ch == '1';
    return true;
}

bool CBlockTreeDB::LoadBlockIndexGuts()
{
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());

    CDataStream ssKeySet(SER_DISK, CLIENT_VERSION);
    ssKeySet << make_pair('b', uint256());
    pcursor->Seek(ssKeySet.str());

    // Load mapBlockIndex
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            char chType;
            ssKey >> chType;
            if (chType == 'b') {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);
                CDiskBlockIndex diskindex;
                ssValue >> diskindex;

                // Construct block index object
                CBlockIndex* pindexNew = InsertBlockIndex(diskindex.GetBlockHash());
                pindexNew->pprev          = InsertBlockIndex(diskindex.hashPrev);
                pindexNew->nHeight        = diskindex.nHeight;
                pindexNew->nFile          = diskindex.nFile;
                pindexNew->nDataPos       = diskindex.nDataPos;
                pindexNew->nUndoPos       = diskindex.nUndoPos;
                pindexNew->nVersion       = diskindex.nVersion;
                pindexNew->hashMerkleRoot = diskindex.hashMerkleRoot;
                pindexNew->nTime          = diskindex.nTime;
                pindexNew->nBits          = diskindex.nBits;
                pindexNew->nNonce         = diskindex.nNonce;
                pindexNew->nStatus        = diskindex.nStatus;
                pindexNew->nTx            = diskindex.nTx;

                if (!CheckProofOfWork(pindexNew->GetBlockHash(), pindexNew->nBits))
                    return error("LoadBlockIndex() : CheckProofOfWork failed: %s", pindexNew->ToString());

                pcursor->Next();
            } else {
                break; // if shutdown requested or finished loading block index
            }
        } catch (const std::exception& e) {
            return error("%s : Deserialize or I/O error - %s", __func__, e.what());
        }
    }

    return true;
}

CMarketTreeDB::CMarketTreeDB(size_t nCacheSize, bool fMemory, bool fWipe)
  : CLevelDBWrapper(GetDataDir() / "blocks" / "market", nCacheSize, fMemory, fWipe) {
}

bool CMarketTreeDB::ReadBlockFileInfo(int nFile, CBlockFileInfo &info) {
    return Read(make_pair('f', nFile), info);
}

bool CMarketTreeDB::WriteReindexing(bool fReindexing) {
    if (fReindexing)
        return Write('R', '1');
    else
        return Erase('R');
}

bool CMarketTreeDB::ReadReindexing(bool &fReindexing) {
    fReindexing = Exists('R');
    return true;
}

bool CMarketTreeDB::ReadLastBlockFile(int &nFile) {
    return Read('l', nFile);
}

bool CMarketTreeDB::WriteBatchSync(const std::vector<std::pair<int, const CBlockFileInfo*> >& fileInfo, int nLastFile, const std::vector<const CBlockIndex*>& blockinfo) {
    CLevelDBBatch batch;
    for (std::vector<std::pair<int, const CBlockFileInfo*> >::const_iterator it=fileInfo.begin(); it != fileInfo.end(); it++) {
        batch.Write(make_pair('f', it->first), *it->second);
    }
    batch.Write('l', nLastFile);
    return WriteBatch(batch, true);
}

bool CMarketTreeDB::WriteMarketIndex(const std::vector<std::pair<uint256, const marketObj *> >&vect)
{
    CLevelDBBatch batch;

    std::vector<std::pair<uint256,const marketObj *> >::const_iterator it;
    for (it=vect.begin(); it != vect.end(); it++) {
        const uint256 &objid = it->first;
        const marketObj *obj = it->second;
        pair<char,uint256> key = make_pair(obj->marketop, objid);

        if (obj->marketop == 'B') {
           const marketBranch *ptr = (const marketBranch *) obj;
           pair<marketBranch,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
        }
        else 
        if (obj->marketop == 'D') {
           const marketDecision *ptr = (const marketDecision *) obj;
           pair<marketDecision,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           batch.Write(make_pair(make_pair('d',ptr->branchid),objid), value);
        }
        else
        if (obj->marketop == 'M') {
           const marketMarket *ptr = (const marketMarket *) obj;
           pair<marketMarket,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           for(size_t i=0; i < ptr->decisionIDs.size(); i++)
               batch.Write(make_pair(make_pair('m',ptr->decisionIDs[i]),objid), value);
        }
        else
        if (obj->marketop == 'O') {
           const marketOutcome *ptr = (const marketOutcome *) obj;
           pair<marketOutcome,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           batch.Write(make_pair(make_pair('o',ptr->branchid),objid), value);
        }
        else
        if (obj->marketop == 'S') {
           const marketSealedVote *ptr = (const marketSealedVote *) obj;
           pair<marketSealedVote,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           batch.Write(make_pair(make_pair(make_pair('s',ptr->branchid),ptr->height),objid), value);
        }
        else
        if (obj->marketop == 'T') {
           const marketTrade *ptr = (const marketTrade *) obj;
           pair<marketTrade,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           batch.Write(make_pair(make_pair('t',ptr->marketid),objid), value);
        }
        else
        if (obj->marketop == 'V') {
           const marketVote *ptr = (const marketVote *) obj;
           pair<marketVote,uint256> value = make_pair(*ptr, obj->txid);
           batch.Write(key, value);
           batch.Write(make_pair(make_pair(make_pair('v',ptr->branchid),ptr->height),objid), value);
        }
    }
    return WriteBatch(batch);
}

bool CMarketTreeDB::WriteFlag(const std::string &name, bool fValue) {
    return Write(std::make_pair('F', name), fValue ? '1' : '0');
}

bool CMarketTreeDB::ReadFlag(const std::string &name, bool &fValue) {
    char ch;
    if (!Read(std::make_pair('F', name), ch))
       return false;
    fValue = ch == '1';
    return true;
}

marketBranch *
CMarketTreeDB::GetBranch(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('B', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketBranch *obj = new marketBranch;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketDecision *
CMarketTreeDB::GetDecision(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('D', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketDecision *obj = new marketDecision;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
           error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketMarket *
CMarketTreeDB::GetMarket(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('M', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketMarket *obj = new marketMarket;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            } 
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketOutcome *
CMarketTreeDB::GetOutcome(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('O', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketOutcome *obj = new marketOutcome;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketSealedVote *
CMarketTreeDB::GetSealedVote(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('S', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketSealedVote *obj = new marketSealedVote;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketTrade *
CMarketTreeDB::GetTrade(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('T', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketTrade *obj = new marketTrade;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

marketVote *
CMarketTreeDB::GetVote(const uint256 &objid)
{
    pair<char,uint256> idx = make_pair('V', objid);
    ostringstream ss;
    ::Serialize(ss, idx, SER_DISK, CLIENT_VERSION);
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    pcursor->Seek(ss.str());
    if (pcursor->Valid()) {
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);

            pair<char,uint256> key;
            ssKey >> key;

            if (key == idx) {
                leveldb::Slice slValue = pcursor->value();
                CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

                marketVote *obj = new marketVote;
                ssValue >> *obj;
                ssValue >> obj->txid;
                return obj;
            }
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
        }
    }
    return NULL;
}

vector<marketBranch *>
CMarketTreeDB::GetBranches(void)
{
    const char marketop = 'B';
    ostringstream ss;
    ss << marketop;

    vector<marketBranch *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            char key;
            ssKey >> key;
            if (key != marketop)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketBranch *obj = new marketBranch;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

vector<marketDecision *>
CMarketTreeDB::GetDecisions(const uint256 & /* branchid */ id)
{
    const char marketop = 'd';
    ostringstream ss;
    ::Serialize(ss, make_pair(make_pair(marketop, id), uint256()), SER_DISK, CLIENT_VERSION);

    vector<marketDecision *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            pair<char,uint256> key;
            ssKey >> key;
            if (key.first != marketop)
                break;
            if (key.second != id)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketDecision *obj = new marketDecision;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

vector<marketMarket *>
CMarketTreeDB::GetMarkets(const uint256 & /* decisionid */ id)
{
    const char marketop = 'm';
    ostringstream ss;
    ::Serialize(ss, make_pair(make_pair(marketop, id), uint256()), SER_DISK, CLIENT_VERSION);

    vector<marketMarket *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            pair<char,uint256> key;
            ssKey >> key;
            if (key.first != marketop)
                break;
            if (key.second != id)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketMarket *obj = new marketMarket;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

vector<marketOutcome *>
CMarketTreeDB::GetOutcomes(const uint256 & /* branchid */ id)
{
    const char marketop = 'o';
    ostringstream ss;
    ::Serialize(ss, make_pair(make_pair(marketop, id), uint256()), SER_DISK, CLIENT_VERSION);

    vector<marketOutcome *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            pair<char,uint256> key;
            ssKey >> key;
            if (key.first != marketop)
                break;
            if (key.second != id)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketOutcome *obj = new marketOutcome;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

vector<marketTrade *>
CMarketTreeDB::GetTrades(const uint256 & /* marketid */ id)
{
    const char marketop = 't';
    ostringstream ss;
    ::Serialize(ss, make_pair(make_pair(marketop, id), uint256()), SER_DISK, CLIENT_VERSION);

    vector<marketTrade *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            pair<char,uint256> key;
            ssKey >> key;
            if (key.first != marketop)
                break;
            if (key.second != id)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketTrade *obj = new marketTrade;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

vector<marketVote *>
CMarketTreeDB::GetVotes(const uint256 & /* branchid */ id, uint32_t height)
{
    const char marketop = 'v';
    ostringstream ss;
    ::Serialize(ss, make_pair(make_pair(make_pair(marketop, id), height), uint256()), SER_DISK, CLIENT_VERSION);

    vector<marketVote *> vec;
    boost::scoped_ptr<leveldb::Iterator> pcursor(NewIterator());
    for(pcursor->Seek(ss.str()); pcursor->Valid(); pcursor->Next()) {
        boost::this_thread::interruption_point();
        try {
            leveldb::Slice slKey = pcursor->key();
            CDataStream ssKey(slKey.data(), slKey.data()+slKey.size(), SER_DISK, CLIENT_VERSION);
            pair<pair<char,uint256>,uint32_t> key;
            ssKey >> key;
            if (key.first.first != marketop)
                break;
            if (key.first.second != id)
                break;
            if (key.second != height)
                break;

            leveldb::Slice slValue = pcursor->value();
            CDataStream ssValue(slValue.data(), slValue.data()+slValue.size(), SER_DISK, CLIENT_VERSION);

            marketVote *obj = new marketVote;
            ssValue >> *obj;
            ssValue >> obj->txid;
            vec.push_back(obj);
        } catch (const std::exception& e) {
            error("%s: %s", __func__, e.what());
            break;
        }
    }
    return vec;
}

