/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStoreInfinite.hh,v 1.3 2000/10/02 18:18:17 tmwong Exp $
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
  bool operator()(const uint32_t i1, const uint32_t i2) const
    {
      return (i1 < i2);
    }
};

typedef map<uint32_t, uint32_t, uint32LessThan> uint32Map;
typedef map<uint32_t, uint32_t, uint32LessThan>::iterator uint32MapIter;
typedef map<uint32_t, uint32_t, uint32LessThan>::const_iterator uint32MapConstIter;

class BlockStoreInfinite : public BlockStore {
private:
  typedef map<Block, uint32_t, BlockLessThan> BlockMap;
  typedef BlockMap::iterator BlockMapIter;
  typedef BlockMap::const_iterator BlockMapConstIter;

  BlockMap blockTimestampMap;
  uint32_t blockTimestampClock;

  uint32Map LRUMap;
  Tree *LRUTree;

  BlockMap freqMap;

private:
  // Copy constructors - declared private and never defined

  BlockStoreInfinite(const BlockStoreInfinite&);
  BlockStoreInfinite& operator=(const BlockStoreInfinite&);

public:
  BlockStoreInfinite(unsigned long inCacheSize,
		     unsigned int inBlockSize) :
    BlockStore(inBlockSize),
    blockTimestampMap(),
    blockTimestampClock(0),
    LRUMap(),
    LRUTree(NULL),
    freqMap() { ; };
  ~BlockStoreInfinite() { ; };

  // Process incoming I/O requests

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Output statistics

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTOREINFINITE_HH_ */
