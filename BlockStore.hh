/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStore.hh,v 1.6 2001/11/16 23:32:46 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORE_HH_
#define _BLOCKSTORE_HH_

using namespace ::std;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

#include "IORequest.hh"
#include "Statistics.hh"

class Block {
public:
  uint64_t devID;
  uint64_t objectID;
  uint64_t blockID;
};

struct BlockLessThan
{
  bool operator()(const Block block1, const Block block2) const {
    return (block1.objectID < block2.objectID ||
	    (block1.objectID == block2.objectID &&
	     block1.blockID < block2.blockID));
  }
};

class BlockStore : public Statistics {
protected:
  uint64_t blockSize;

  uint64_t blockDemoteHits;
  uint64_t blockDemoteMisses;

  uint64_t blockReadHits;
  uint64_t blockReadMisses;

private:
  // Copy constructors - declared private and never defined

  BlockStore(const BlockStore&);
  BlockStore& operator=(const BlockStore&);

public:
  // Constructors

  BlockStore(const char *inName,
	     uint64_t inBlockSize);

  virtual ~BlockStore() { ; };

  // Process incoming I/O requests

  virtual bool IORequestDown(const IORequest &inIOReq,
			     list<IORequest> &outIOReqList) = 0;

  // Statistics management

  virtual void statisticsReset() {
    blockDemoteHits = 0;
    blockDemoteMisses = 0;
    blockReadHits = 0;
    blockReadMisses = 0;
  };
};

inline
BlockStore::BlockStore(const char *inName,
		       uint64_t inBlockSize) :
  Statistics(inName),
  blockSize(inBlockSize)
{
  statisticsReset();
};

#endif /* _BLOCKSTORE_HH_ */
