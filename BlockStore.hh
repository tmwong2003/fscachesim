/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStore.hh,v 1.8 2002/02/11 20:08:22 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORE_HH_
#define _BLOCKSTORE_HH_

using namespace std;

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

/**
 * Interface for objects that model block storage devices. Classes that
 * inherit from BlockStore must implement all abstract methods.
 */
class BlockStore : public Statistics {
protected:
  /**
   * The size of blocks in the storage device.
   */
  uint64_t blockSize;

  /**
   * The number of received demotions that hit in the storage device.
   */
  uint64_t demoteHits;

  /**
   * The number of received demotions that missed in the storage device.
   */
  uint64_t demoteMisses;

  /**
   * The number of received reads that hit in the storage device.
   */
  uint64_t readHits;
  /**
   * The number of received reads that missed in the storage device.
   */
  uint64_t readMisses;

private:
  // Copy constructors - declared private and never defined

  BlockStore(const BlockStore&);
  BlockStore& operator=(const BlockStore&);

public:
  // Constructors and destructors

  /**
   * Create a block store.
   *
   * @param inName A string name for the store.
   * @param inBlockSize The size of each block, in bytes.
   */
  BlockStore(const char *inName,
	     uint64_t inBlockSize);

  /**
   *Destroy a block store.
   */
  virtual ~BlockStore() { ; };

  // I/O request handlers

  /**
   * Receive an incoming I/O request sent down from a higher-level block
   * store or request generator.
   *
   * @param inIOReq The I/O request from the higher-level block store or
   * request generator.
   * @param outIOReq An output list of I/O requests to the next lower-level
   * block store.
   *
   * @return true if the request was handled successfully, false otherwise.
   */
  virtual bool IORequestDown(const IORequest &inIOReq,
			     list<IORequest> &outIOReqList) = 0;

  // Statistics management

  virtual void statisticsReset() {
    demoteHits = 0;
    demoteMisses = 0;
    readHits = 0;
    readMisses = 0;
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
