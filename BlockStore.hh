/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStore.hh,v 1.1 2000/09/22 16:15:38 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _BLOCKSTORE_HH_
#define _BLOCKSTORE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <map>

#include "IORequest.hh"

class Block {
public:
  uint32_t devID;
  uint32_t objectID;
  uint32_t blockID;
};

struct BlockLessThan
{
  bool operator()(const Block block1, const Block block2) const {
    return (block1.objectID < block2.objectID ||
	    (block1.objectID == block2.objectID &&
	     block1.blockID < block2.blockID));
  }
};

class BlockStore {
protected:
  uint32_t blockSize;

  uint32_t blockDemoteHits;
  uint32_t blockDemoteMisses;

  uint32_t blockReadHits;
  uint32_t blockReadMisses;

private:
  // Copy constructors - declared private and never defined

  BlockStore(const BlockStore&);
  BlockStore& operator=(const BlockStore&);

public:
  // Constructors

  BlockStore(uint32_t inBlockSize) :
    blockSize(inBlockSize),
    blockDemoteHits(0),
    blockDemoteMisses(0),
    blockReadHits(0),
    blockReadMisses(0) { ; };
  virtual ~BlockStore() { ; };

  // Process incoming I/O requests

  virtual bool IORequestDown(const IORequest &inIOReq,
			     list<IORequest> &outIOReqList) = 0;

  // Output statistics

  virtual void statisticsShow() const = 0;
};

#endif /* _BLOCKSTORE_HH_ */
