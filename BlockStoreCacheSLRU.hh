/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSLRU.hh,v 1.2 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHESLRU_HH_
#define _BLOCKSTORECACHESLRU_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "BlockStoreCache.hh"
#include "Cache.hh"

/**
 * SLRU block cache with support incoming demotions sent down from a
 * higher-level block store or request generator.
 *
 * @warning Will not demotions to send to a lower-level store.
 */
class BlockStoreCacheSLRU : public BlockStoreCache {
protected:
  Cache probCache;
  Cache protCache;

  Char::Counter probDemoteHitsPerOrig;
  Char::Counter probReadHitsPerOrig;

  Char::Counter protDemoteHitsPerOrig;
  Char::Counter protReadHitsPerOrig;

  Char::Counter probToProtXfersPerOrig;
  Char::Counter protToProbXfersPerOrig;

public:
  BlockStoreCacheSLRU(const char *inName,
		      uint64_t inBlockSize,
		      uint64_t inSize,
		      uint64_t inProbSize) :
    BlockStoreCache(inName, inBlockSize),
    probCache(inProbSize),
    protCache(inSize - inProbSize) { ; };

  ~BlockStoreCacheSLRU() { ; };

  // Documented at the definition.
  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHESLRU_HH_ */
