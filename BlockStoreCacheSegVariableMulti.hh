/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegVariableMulti.hh,v 1.1 2001/11/18 07:48:46 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHESEGVARIABLEMULTI_HH_
#define _BLOCKSTORECACHESEGVARIABLEMULTI_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "BlockStore.hh"
#include "CacheGhost.hh"

class BlockStoreCacheSegVariableMulti : public BlockStore {
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

  typedef map<const char *, CacheGhost *> GhostMap;
  typedef GhostMap::iterator GhostMapIter;
  typedef GhostMap::const_iterator GhostMapConstIter;

  GhostMap ghostMap;
  uint64_t ghostCacheSize;

  bool normalizeFlag;

  StatMap demoteHitsMap;
  StatMap demoteMissesMap;

  StatMap readHitsMap;
  StatMap readMissesMap;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCacheSegVariableMulti(const BlockStoreCacheSegVariableMulti&);
  BlockStoreCacheSegVariableMulti& operator=(const BlockStoreCacheSegVariableMulti&);

protected:
  int blockGetCascade(Block inBlock);
  void blockPutAtSegCascade(Block inBlock,
			    int inSeg);

public:
  BlockStoreCacheSegVariableMulti(const char *inName,
				  uint64_t inBlockSize,
				  uint64_t inCacheSize,
				  int inSegCount,
				  bool inNormalizeFlag);

  BlockStoreCacheSegVariableMulti(const char *inName,
				  uint64_t inBlockSize,
				  uint64_t inCacheSize,
				  int inSegCount,
				  int inSegMultiplier,
				  bool inNormalizeFlag);

  ~BlockStoreCacheSegVariableMulti();

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHESEGVARIABLEMULTI_HH_ */
