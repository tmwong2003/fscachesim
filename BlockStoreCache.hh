/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.hh,v 1.9 2001/11/16 23:32:46 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHE_HH_
#define _BLOCKSTORECACHE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>

#include "BlockStore.hh"
#include "Cache.hh"

enum CacheDemotePolicy_t {None, DemoteDemand};
enum CacheReplPolicy_t {LRU, MRU};

class BlockStoreCache : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, uint64_t, CharStarLessThan> StatMap;
  typedef StatMap::iterator StatMapIter;
  typedef StatMap::const_iterator StatMapConstIter;

  Cache cache;
  CacheReplPolicy_t cacheReplPolicy;
  CacheDemotePolicy_t cacheDemotePolicy;

  bool logRequestFlag;

  StatMap blockDemoteHitsMap;
  StatMap blockDemoteMissesMap;

  StatMap blockReadHitsMap;
  StatMap blockReadMissesMap;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCache(const BlockStoreCache&);
  BlockStoreCache& operator=(const BlockStoreCache&);

public:
  BlockStoreCache(const char *inName,
		  uint64_t inBlockSize,
		  uint64_t inCacheSize,
		  CacheReplPolicy_t inCacheReplPolicy,
		  CacheDemotePolicy_t inCacheDemotePolicy) :
    BlockStore(inName, inBlockSize),
    cache(inCacheSize),
    cacheReplPolicy(inCacheReplPolicy),
    cacheDemotePolicy(inCacheDemotePolicy),
    logRequestFlag(false) { ; };

  ~BlockStoreCache() { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReq);

  void logRequestToggle() {
    logRequestFlag = (logRequestFlag ? false : true);
  };

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHE_HH_ */
