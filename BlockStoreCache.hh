/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.hh,v 1.10 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHE_HH_
#define _BLOCKSTORECACHE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>

#include "BlockStore.hh"
#include "Char.hh"

/**
 * Interface for finite block caches. Classes that inherit from
 * BlockStoreCache must implement all abstract methods.
 */
class BlockStoreCache : public BlockStore {
protected:
  Char::Counter demoteHitsPerOrig;
  Char::Counter demoteMissesPerOrig;

  Char::Counter readHitsPerOrig;
  Char::Counter readMissesPerOrig;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCache(const BlockStoreCache&);
  BlockStoreCache& operator=(const BlockStoreCache&);

public:
  // Constructors and destructors

  /**
   * Create a block cache.
   *
   * @param inName A string name for the cache.
   * @param inBlockSize The size of each block, in bytes.
   * @param inSize The size of the cache, in blocks.
   */
  BlockStoreCache(const char *inName,
		  uint64_t inBlockSize) :
    BlockStore(inName, inBlockSize) { ; };

  /**
   * Destroy a block cache.
   */
  ~BlockStoreCache() { ; };

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _BLOCKSTORECACHE_HH_ */
