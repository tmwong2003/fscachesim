/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/StoreCache.hh,v 1.1 2002/02/12 21:50:56 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _STORECACHE_HH_
#define _STORECACHE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>

#include "Block.hh"
#include "Cache.hh"
#include "Char.hh"
#include "IORequest.hh"
#include "Store.hh"

/**
 * Interface for finite block caches. Classes that inherit from
 * StoreCache must implement all abstract methods.
 */
class StoreCache : public Store {
private:
  bool logRequestFlag;

protected:
  /**
   * Per-originator counts of demotes that hit in the cache.
   */
  Char::Counter demoteHitsPerOrig;

  /**
   * Per-originator counts of demotes that miss in the cache.
   */
  Char::Counter demoteMissesPerOrig;

  /**
   * Per-originator counts of reads that hit in the cache.
   */
  Char::Counter readHitsPerOrig;

  /**
   * Per-originator counts of reads that miss in the cache.
   */
  Char::Counter readMissesPerOrig;

private:
  // Copy constructors - declared private and never defined

  StoreCache(const StoreCache&);
  StoreCache& operator=(const StoreCache&);

protected:
  /**
   * Cache management function. All the work of inserting and ejecting
   * blocks is done in this function.
   *
   * @param inIOReq The I/O request containing the blocks.
   * @param inBlock A block from the I/O request.
   * @param outIOReqs A list of new I/O requests to send to the lower-level
   * storage device, if any.
   */
  virtual void BlockCache(const IORequest& inIOReq,
			  const Block::block_t& inBlock,
			  list<IORequest>& outIOReqs) = 0;

public:
  // Constructors and destructors

  /**
   * Create a block cache.
   *
   * @param inName A string name for the cache.
   * @param inNextStore A lower-level storage device (can be NULL).
   * @param inBlockSize The size of each block, in bytes.
   */
  StoreCache(const char *inName,
	     Store *inNextStore,
	     uint64_t inBlockSize) :
    Store(inName, inNextStore, inBlockSize),
    logRequestFlag(false) { ; };

  /**
   * Destroy a block cache.
   */
  ~StoreCache() { ; };

  // I/O request handlers

  virtual bool IORequestDown(const IORequest& inIOReq);

  /**
   * Log incoming I/O requests. Initially set to not log.
   */
  void logRequestToggle() {
    logRequestFlag = (logRequestFlag ? false : true);
  };

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _STORECACHE_HH_ */
