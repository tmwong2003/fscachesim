/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreInfinite.hh,v 1.9 2002/02/11 20:08:22 tmwong Exp $
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTOREINFINITE_HH_
#define _BLOCKSTOREINFINITE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>

extern "C" {
#include "top-down-size-splay.h"
}

#include "Block.hh"
#include "BlockStore.hh"
#include "Cache.hh"
#include "UInt64.hh"

/**
 * Infinite LRU block cache.
 */
class BlockStoreInfinite : public BlockStore {
private:

  // These data structures together form the 'cache'.

  Block::Counter blockTimestampMap;
  uint64_t blockTimestampClock;
  Tree *LRUTree;

  // For finite caches.

  Cache cache;

  // These keep stats on the cache.

  UInt64::Counter LRUMap; // LRU stack depth
  Block::Counter freqMap; // Block access frequency

  // Keep the frequency count or not?

  bool freqFlag;

private:
  // Copy constructors - declared private and never defined

  BlockStoreInfinite(const BlockStoreInfinite&);
  BlockStoreInfinite& operator=(const BlockStoreInfinite&);

public:
  /**
   * Create an infinite block cache.
   *
   * @param inName A string name for the cache.
   * @param inBlockSize The size of each block, in bytes.
   * @param inSize The size of the cache, in blocks.
   */
  BlockStoreInfinite(const char *inName,
		     uint64_t inBlockSize,
		     uint64_t inSize,
		     bool inFreqFlag) :
    BlockStore(inName, inBlockSize),
    blockTimestampMap(),
    blockTimestampClock(0),
    LRUTree(NULL),
    cache(inSize),
    LRUMap(),
    freqMap(),
    freqFlag(inFreqFlag) { ; };

  /**
   * Destroy the infinite cache.
   */
  ~BlockStoreInfinite() { ; };

  // I/O request handlers

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;

  void statisticsFreqShow() const;
  void statisticsLRUShow() const;
  void statisticsLRUCumulShow() const;
  void statisticsSummaryShow() const;
};

#endif /* _BLOCKSTOREINFINITE_HH_ */
