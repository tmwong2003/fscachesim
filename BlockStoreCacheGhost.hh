/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheGhost.hh,v 1.3 2001/11/16 23:32:46 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHEGHOST_HH_
#define _BLOCKSTORECACHEGHOST_HH_

#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

#include "BlockStore.hh"
#include "Cache.hh"
#include "CacheGhost.hh"
#include "IORequest.hh"

class BlockStoreCacheGhost : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, uint64_t, CharStarLessThan> StatMap;
  typedef StatMap::iterator StatMapIter;
  typedef StatMap::const_iterator StatMapConstIter;

  // The real cache, and the probabilities that we cache a demoted or a
  // read block

  Cache cache;
  uint64_t demotesCached;
  uint64_t demotesUncached;
  uint64_t readsCached;
  uint64_t readsUncached;

  // The ghost caches, for demote and read operations

  CacheGhost ghost;

  StatMap demoteHitsMap;
  StatMap demoteMissesMap;

  StatMap readHitsMap;
  StatMap readMissesMap;

private:
  void cacheInGhost(IORequestOp_t inOp, Block block);
  void isReadHitInGhost(Block block);

public:
  BlockStoreCacheGhost(const char *inName,
		       uint64_t inBlockSize,
		       uint64_t inCacheSize) :
    BlockStore(inName, inBlockSize),
    cache(inCacheSize),
    demotesCached(0),
    demotesUncached(0),
    readsCached(0),
    readsUncached(0),
    ghost(inCacheSize) {
      // I know, I know...

      srand(0);
  };

  ~BlockStoreCacheGhost() { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHEGHOST_HH_ */
