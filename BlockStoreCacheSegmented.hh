/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Blockstorecachesegmented.hh,v 1.5 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHESEGMENTED_HH_
#define _BLOCKSTORECACHESEGMENTED_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "BlockStoreCache.hh"

class BlockStoreCacheSegmented : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, uint32_t, CharStarLessThan> StatMap;
  typedef StatMap::iterator StatMapIter;
  typedef StatMap::const_iterator StatMapConstIter;

  Cache probCache;
  Cache protCache;

  StatMap probDemoteHitsMap;
  StatMap probReadHitsMap;

  StatMap protDemoteHitsMap;
  StatMap protReadHitsMap;

  StatMap blockDemoteMissesMap;
  StatMap blockReadMissesMap;

  StatMap probToProtXfersMap;
  StatMap protToProbXfersMap;

public:
  BlockStoreCacheSegmented(const char *inName,
			   uint32_t inBlockSize,
			   uint32_t inCacheSize,
			   uint32_t inProbCacheSize) :
    BlockStore(inName, inBlockSize),
    probCache(inProbCacheSize),
    protCache(inCacheSize - inProbCacheSize) { ; };

  ~BlockStoreCacheSegmented() { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHESEGMENTED_HH_ */
