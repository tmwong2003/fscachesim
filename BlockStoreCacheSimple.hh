/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.hh,v 1.10 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCKSTORECACHESIMPLE_HH_
#define _BLOCKSTORECACHESIMPLE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>

#include "BlockStoreCache.hh"
#include "Cache.hh"

/**
 * Simple LRU/MRU block cache with support for demote operations.
 */
class BlockStoreCacheSimple : public BlockStoreCache {
public:
  // Policies

  /**
   * Cache demotion policy.
   */
  enum DemotePolicy_t {
    None, /**< No demotes. */
    DemoteDemand /**< Demote a block when the cache ejects the block. */
  };

  /**
   * Cache ejection policy.
   */
  enum EjectPolicy_t {
    LRU, /**< LRU: Eject blocks from the head of the cache. */
    MRU /**< MRU: Eject blocks from the tail of the cache. */
  };

private:
  Cache cache;
  EjectPolicy_t ejectPolicy;
  DemotePolicy_t demotePolicy;

  bool logRequestFlag;

private:
  // Copy constructors - declared private and never defined

  BlockStoreCacheSimple(const BlockStoreCacheSimple&);
  BlockStoreCacheSimple& operator=(const BlockStoreCacheSimple&);

public:
  // Constructors and destructors

  /**
   * Create a simple block cache.
   *
   * @param inName A string name for the cache.
   * @param inBlockSize The size of each block, in bytes.
   * @param inSize The size of the cache, in blocks.
   * @param inEjectPolicy The cache ejection policy.
   * @param inDemotePolicy_t The cache demotion policy.
   */
  BlockStoreCacheSimple(const char *inName,
			uint64_t inBlockSize,
			uint64_t inSize,
			EjectPolicy_t inEjectPolicy,
			DemotePolicy_t inDemotePolicy) :
    BlockStoreCache(inName, inBlockSize),
    cache(inSize),
    ejectPolicy(inEjectPolicy),
    demotePolicy(inDemotePolicy),
    logRequestFlag(false) { ; };

  /**
   * Destroy the cache.
   */
  ~BlockStoreCacheSimple() { ; };

  // I/O request handlers

  virtual bool IORequestDown(const IORequest& inIOReq,
			     list<IORequest>& outIOReq);

  /**
   * Log incoming I/O requests. Initially set to not log.
   */
  void logRequestToggle() {
    logRequestFlag = (logRequestFlag ? false : true);
  };
};

#endif /* _BLOCKSTORECACHESIMPLE_HH_ */
