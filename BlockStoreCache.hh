/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHE_HH_
#define _BLOCKSTORECACHE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>
#include <stdio.h>

#include "BlockStore.hh"

class BlockStoreCache : public BlockStore {
private:
  typedef list<Block> Cache;
  typedef Cache::iterator CacheIter;
  typedef map<Block, CacheIter, BlockLessThan> CacheIndex;
  typedef CacheIndex::iterator CacheIndexIter;

  Cache cache;
  uint32_t cacheSize;
  uint32_t cacheBlocks;
  CacheIndex cacheIndex;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCache(const BlockStoreCache&);
  BlockStoreCache& operator=(const BlockStoreCache&);

public:
  BlockStoreCache(uint32_t inBlockSize,
		  uint32_t inCacheSize) :
    BlockStore(inBlockSize),
    cache(),
    cacheSize(inCacheSize),
    cacheBlocks(0),
    cacheIndex()
    { ; };

  ~BlockStoreCache()
    { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReq);

  // Output statistics

  virtual void StatisticsShow();
};

#endif /* _BLOCKSTORECACHE_HH_ */
