/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreInfinite.hh,v 1.7 2001/11/20 02:20:13 tmwong Exp $
  Description:  
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

#include "BlockStore.hh"
#include "Cache.hh"

struct uint64LessThan
{
  bool operator()(const uint64_t i1, const uint64_t i2) const
    {
      return (i1 < i2);
    }
};

typedef map<uint64_t, uint64_t, uint64LessThan> uint64Map;
typedef map<uint64_t, uint64_t, uint64LessThan>::iterator uint64MapIter;
typedef map<uint64_t, uint64_t, uint64LessThan>::const_iterator uint64MapConstIter;

class BlockStoreInfinite : public BlockStore {
private:
  typedef map<Block, uint64_t, BlockLessThan> BlockMap;
  typedef BlockMap::iterator BlockMapIter;
  typedef BlockMap::const_iterator BlockMapConstIter;

  // These data structures together form the 'cache'.

  BlockMap blockTimestampMap;
  uint64_t blockTimestampClock;
  Tree *LRUTree;

  // For finite caches.

  Cache cache;

  // These keep stats on the cache.

  uint64Map LRUMap; // LRU stack depth
  BlockMap freqMap; // Block access frequency

  // Keep the frequency count or not?

  bool freqFlag;

private:
  // Copy constructors - declared private and never defined

  BlockStoreInfinite(const BlockStoreInfinite&);
  BlockStoreInfinite& operator=(const BlockStoreInfinite&);

public:
  BlockStoreInfinite(const char *inName,
		     unsigned long inCacheSize,
		     unsigned int inBlockSize,
		     bool inFreqFlag) :
    BlockStore(inName, inBlockSize),
    blockTimestampMap(),
    blockTimestampClock(0),
    LRUTree(NULL),
    cache(inCacheSize),
    LRUMap(),
    freqMap(),
    freqFlag(inFreqFlag) { ; };
  ~BlockStoreInfinite() { ; };

  // Process incoming I/O requests

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
