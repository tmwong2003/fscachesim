/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.hh,v 1.6 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _CACHE_HH_
#define _CACHE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>

#include "BlockStore.hh"

class Cache {
protected:
  typedef map<Block, list<Block>::iterator, BlockLessThan> CacheIndex;
  typedef CacheIndex::iterator CacheIndexIter;

  list<Block> cache;
  CacheIndex cacheIndex;

  uint32_t blockCount;
  uint32_t blockCountMax;

public:
  Cache(uint32_t inCacheSize) :
    cache(),
    cacheIndex(),
    blockCount(0),
    blockCountMax(inCacheSize) { ; };

  ~Cache() { ; };

  void blockEject(Block &outBlock);
  void blockGet(Block inBlock);
  void blockPutAtHead(Block inBlock);
  void blockPutAtTail(Block inBlock);

  bool isCached(Block inBlock);
  bool isFull();
};

inline void
Cache::blockEject(Block &outBlock)
{
  if (!cache.empty()) {
    outBlock = *cache.begin();
    cacheIndex.erase(cache.front());
    cache.pop_front();
    blockCount--;
  }
};

inline void
Cache::blockGet(Block inBlock)
{
  CacheIndexIter blockIter = cacheIndex.find(inBlock);
  if (blockIter != cacheIndex.end()) {
    cacheIndex.erase(blockIter);
    cache.erase(blockIter->second);
    blockCount--;
  }
};

inline void
Cache::blockPutAtHead(Block inBlock)
{
  blockCount++;
  cache.push_front(inBlock);
  cacheIndex[inBlock] = cache.begin();
};

inline void
Cache::blockPutAtTail(Block inBlock)
{
  blockCount++;
  cache.push_back(inBlock);
  cacheIndex[inBlock] = --cache.end();
};

inline bool
Cache::isCached(Block inBlock)
{
  return (cacheIndex.find(inBlock) != cacheIndex.end());
};

inline bool
Cache::isFull()
{
  return (blockCount == blockCountMax);
};

#endif /* _CACHE_HH_ */
