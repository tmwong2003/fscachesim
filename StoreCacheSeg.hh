/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/StoreCacheSeg.hh,v 1.4 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _STORECACHESEG_HH_
#define _STORECACHESEG_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>

#include "Block.hh"
#include "Ghost.hh"
#include "StoreCache.hh"

/**
 * Segmented LRU cache with an adaptive cache-insertion policy
 * [Wong2002]. Incoming {read, demote} blocks are "cached" in a {read,
 * demote} ghost cache before being insterted in the actual cache. The
 * relative hit rates in the ghosts determine into which segment the
 * incoming block will be inserted.
 *
 * @warning Will not demotions to send to a lower-level store.
 */
class StoreCacheSeg : public StoreCache {
private:
  /**
   * The cache segments.
   */
  Cache **cacheSegs;
  
  /**
   * The number of cache segments.
   */
  int cacheSegCount;

  /**
   * The ghost cache.
   */
  Ghost ghost;

  /**
   * The number of hits in each segment.
   */
  uint64_t *segHits;

private:
  // Copy constructors - declared private and never defined

  StoreCacheSeg(const StoreCacheSeg&);
  StoreCacheSeg& operator=(const StoreCacheSeg&);

  int blockGetCascade(const Block::block_t& inBlock);
  void blockPutAtSegCascade(const Block::block_t& inBlock,
			    int inSeg);

protected:
  virtual void BlockCache(const IORequest& inIOReq,
			  const Block::block_t& inBlock,
			  list<IORequest>& outIOReqList);

public:
  StoreCacheSeg(const char *inName,
		Store *inNextStore,
		uint64_t inBlockSize,
		uint64_t inSize,
		int inSegCount,
		bool inNormalizeFlag);

  StoreCacheSeg(const char *inName,
		Store *inNextStore,
		uint64_t inBlockSize,
		uint64_t inSize,
		int inSegCount,
		int inSegMultiplier,
		bool inNormalizeFlag);

  ~StoreCacheSeg();

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _STORECACHESEG_HH_ */
