/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreShareStats.hh,v 1.2 2001/11/20 02:20:13 tmwong Exp $
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

#include "Block.hh"
#include "BlockStore.hh"

class BlockStoreShareStats : public BlockStore {
private:
  struct CharStarLessThan {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    };
  };

  typedef map<const char *, Block::Counter *, CharStarLessThan> OrigMap;

  Block::Counter accessMap;

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
