/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStoreCache.hh,v 1.1 2000/09/22 16:15:38 tmwong Exp $
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

enum CacheDemotePolicy_t {None, DemoteDemand};
enum CacheReplPolicy_t {LRU, MRU};

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
  CacheReplPolicy_t cacheReplPolicy;
  CacheDemotePolicy_t cacheDemotePolicy;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCache(const BlockStoreCache&);
  BlockStoreCache& operator=(const BlockStoreCache&);

public:
  BlockStoreCache(uint32_t inBlockSize,
		  uint32_t inCacheSize,
		  CacheReplPolicy_t inCacheReplPolicy,
		  CacheDemotePolicy_t inCacheDemotePolicy) :
    BlockStore(inBlockSize),
    cache(),
    cacheSize(inCacheSize),
    cacheBlocks(0),
    cacheIndex(),
    cacheReplPolicy(inCacheReplPolicy),
    cacheDemotePolicy(inCacheDemotePolicy) { ; };

  ~BlockStoreCache() { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReq);

  // Output statistics

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHE_HH_ */
