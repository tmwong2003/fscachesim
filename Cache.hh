/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Cache.hh,v 1.6 2002/02/08 16:54:10 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _CACHE_HH_
#define _CACHE_HH_

using namespace std;

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

#include "Block.hh"
#include "BlockStore.hh"

class Cache {
protected:
  typedef map<Block::block_t, list<Block::block_t>::iterator, Block::LessThan> CacheIndex;
  typedef CacheIndex::iterator CacheIndexIter;

  list<Block::block_t> cache;
  CacheIndex cacheIndex;

  uint64_t blockCount;
  uint64_t blockCountMax;

public:
  Cache(uint64_t inCacheSize) :
    cache(),
    cacheIndex(),
    blockCount(0),
    blockCountMax(inCacheSize) { ; };

  ~Cache() { ; };

  void blockGet(Block::block_t inBlock);

  void blockGetAtHead(Block::block_t &outBlock);
  void blockPutAtHead(Block::block_t inBlock);

  void blockPutAtTail(Block::block_t inBlock);

  uint64_t sizeGet() { return (blockCountMax);};

  bool isCached(Block::block_t inBlock);
  bool isFull();
};

inline void
Cache::blockGet(Block::block_t inBlock)
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
Cache::blockGetAtHead(Block::block_t &outBlock)
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
Cache::blockPutAtHead(Block::block_t inBlock)
{
  blockCount++;
  cache.push_front(inBlock);
  cacheIndex[inBlock] = cache.begin();
  assert(blockCount <= blockCountMax);
};

inline void
Cache::blockPutAtTail(Block::block_t inBlock)
{
  blockCount++;
  cache.push_back(inBlock);
  cacheIndex[inBlock] = --cache.end();
  assert(blockCount <= blockCountMax);
};

inline bool
Cache::isCached(Block::block_t inBlock)
{
  return (cacheIndex.find(inBlock) != cacheIndex.end());
};

inline bool
Cache::isFull()
{
  return (blockCount == blockCountMax);
};

#endif /* _CACHE_HH_ */
