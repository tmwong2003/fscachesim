/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreInfinite.hh,v 1.6 2000/10/30 01:12:44 tmwong Exp $
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

struct uint32LessThan
{
  bool operator()(const uint64_t i1, const uint32_t i2) const
    {
      return (i1 < i2);
    }
};

typedef map<uint64_t, uint32_t, uint32LessThan> uint32Map;
typedef map<uint64_t, uint32_t, uint32LessThan>::iterator uint32MapIter;
typedef map<uint64_t, uint32_t, uint32LessThan>::const_iterator uint32MapConstIter;

class BlockStoreInfinite : public BlockStore {
private:
  typedef map<Block, uint64_t, BlockLessThan> BlockMap;
  typedef BlockMap::iterator BlockMapIter;
  typedef BlockMap::const_iterator BlockMapConstIter;

  // These data structures together form the 'cache'.

  BlockMap blockTimestampMap;
  uint64_t blockTimestampClock;
  Tree *LRUTree;

  // These keep stats on the cache.

  uint32Map LRUMap; // LRU stack depth
  BlockMap freqMap; // Block access frequency

private:
  // Copy constructors - declared private and never defined

  BlockStoreInfinite(const BlockStoreInfinite&);
  BlockStoreInfinite& operator=(const BlockStoreInfinite&);

public:
  BlockStoreInfinite(const char *inName,
		     unsigned long inCacheSize,
		     unsigned int inBlockSize) :
    BlockStore(inName, inBlockSize),
    blockTimestampMap(),
    blockTimestampClock(0),
    LRUTree(NULL),
    LRUMap(),
    freqMap() { ; };
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
