/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreInfinite.hh,v 1.6 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORESHARESTATS_HH_
#define _BLOCKSTORESHARESTATS_HH_

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

class BlockStoreShareStats : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<Block, uint32_t, BlockLessThan> BlockMap;

  typedef map<const char *, BlockMap *, CharStarLessThan> OrigMap;

  BlockMap accessMap;

  OrigMap origToAccessMap;

private:
  // Copy constructors - declared private and never defined

  BlockStoreShareStats(const BlockStoreShareStats&);
  BlockStoreShareStats& operator=(const BlockStoreShareStats&);

public:
  BlockStoreShareStats(const char *inName,
		       unsigned int inBlockSize) :
    BlockStore(inName, inBlockSize),
    accessMap(),
    origToAccessMap() { ; };
  ~BlockStoreShareStats();

  // Process incoming I/O requests

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList);

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORESHARESTATS_HH_ */
