/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheGhost.hh,v 1.1 2001/07/04 17:49:30 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHEGHOST_HH_
#define _BLOCKSTORECACHEGHOST_HH_

#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "BlockStore.hh"
#include "Cache.hh"
#include "IORequest.hh"

class CacheGhost {
private:
  Cache demoteGhost;
  uint32_t demoteGhostReadHits;

  Cache readGhost;
  uint32_t readGhostReadHits;

  double demoteProb;
  double readProb;

public:
  CacheGhost(uint32_t inCacheSize) :
    demoteGhost(inCacheSize),
    demoteGhostReadHits(1),
    readGhost(inCacheSize),
    readGhostReadHits(1),
    demoteProb(0),
    readProb(0) { ; };

  void blockPut(IORequestOp_t op,
		Block block);

  void probUpdate(Block block);

  double probGet(IORequestOp_t op) {
    return (op == Demote ? demoteProb : readProb); };

  void statisticsShow() const;
};

class BlockStoreCacheGhost : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, uint32_t, CharStarLessThan> StatMap;
  typedef StatMap::iterator StatMapIter;
  typedef StatMap::const_iterator StatMapConstIter;

  // The real cache, and the probabilities that we cache a demoted or a
  // read block

  Cache cache;
  uint32_t requestsUncached;

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
		       uint32_t inBlockSize,
		       uint32_t inCacheSize) :
    BlockStore(inName, inBlockSize),
    cache(inCacheSize),
    requestsUncached(0),
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
