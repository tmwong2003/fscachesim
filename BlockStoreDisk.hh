/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreDisk.hh,v 1.3 2000/10/24 19:54:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTOREDISK_HH_
#define _BLOCKSTOREDISK_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#include <stdio.h>

#include "BlockStore.hh"

class BlockStoreDisk : public BlockStore {
private:
  // Copy constructors - declared private and never defined

  BlockStoreDisk(const BlockStoreDisk&);
  BlockStoreDisk& operator=(const BlockStoreDisk&);

public:
  BlockStoreDisk(uint64_t inBlockSize) :
    BlockStore(inBlockSize) { ; };
  ~BlockStoreDisk() { ; };

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReqList) {
      blockReadHits++;

      return (true);
    };

  // Output statistics

  virtual void statisticsShow() const {
      printf("Block hits %u\n", blockReadHits);
    };
};

#endif /* _BLOCKSTOREDISK_HH_ */
