/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegVariable.hh,v 1.3 2001/11/18 07:48:46 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHESEGVARIABLE_HH_
#define _BLOCKSTORECACHESEGVARIABLE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "BlockStore.hh"
#include "CacheGhost.hh"

class BlockStoreCacheSegVariable : public BlockStore {
protected:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, uint64_t, CharStarLessThan> StatMap;
  typedef StatMap::iterator StatMapIter;
  typedef StatMap::const_iterator StatMapConstIter;

  Cache **cacheSegs;
  int cacheSegCount;

  uint64_t *segHits;

  // The ghost caches, for demote and read operations

  CacheGhost ghost;

  StatMap demoteHitsMap;
  StatMap demoteMissesMap;

  StatMap readHitsMap;
  StatMap readMissesMap;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCacheSegVariable(const BlockStoreCacheSegVariable&);
  BlockStoreCacheSegVariable& operator=(const BlockStoreCacheSegVariable&);

protected:
  int blockGetCascade(Block inBlock);
  void blockPutAtSegCascade(Block inBlock,
			    int inSeg);

public:
  BlockStoreCacheSegVariable(const char *inName,
			     uint64_t inBlockSize,
			     uint64_t inCacheSize,
			     int inSegCount,
			     bool inNormalizeFlag);

  BlockStoreCacheSegVariable(const char *inName,
			     uint64_t inBlockSize,
			     uint64_t inCacheSize,
			     int inSegCount,
			     int inSegMultiplier,
			     bool inNormalizeFlag);

  ~BlockStoreCacheSegVariable();

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHESEGVARIABLE_HH_ */
