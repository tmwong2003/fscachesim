/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Cache.hh,v 1.3 2001/07/19 00:11:00 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _CACHE_HH_
#define _CACHE_HH_

using namespace ::std;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#define NDEBUG

#include <assert.h>
#include <list>
#include <map>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

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

  void blockGet(Block inBlock);

  void blockGetAtHead(Block &outBlock);
  void blockPutAtHead(Block inBlock);

  void blockPutAtTail(Block inBlock);

  bool isCached(Block inBlock);
  bool isFull();
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
  assert(blockCount >= 0);
};

inline void
Cache::blockGetAtHead(Block &outBlock)
{
  if (!cache.empty()) {
    outBlock = *cache.begin();
    cacheIndex.erase(cache.front());
    cache.pop_front();
    blockCount--;
  }
  assert(blockCount >= 0);
};

inline void
Cache::blockPutAtHead(Block inBlock)
{
  blockCount++;
  cache.push_front(inBlock);
  cacheIndex[inBlock] = cache.begin();
  assert(blockCount <= blockCountMax);
};

inline void
Cache::blockPutAtTail(Block inBlock)
{
  blockCount++;
  cache.push_back(inBlock);
  cacheIndex[inBlock] = --cache.end();
  assert(blockCount <= blockCountMax);
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
