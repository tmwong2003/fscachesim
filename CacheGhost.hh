/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Cache.hh,v 1.2 2001/07/04 17:49:30 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _CACHEGHOST_HH_
#define _CACHEGHOST_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "Cache.hh"
#include "IORequest.hh"

class CacheGhost {
private:
  Cache demoteGhost;
  uint32_t demoteGhostReadHits;

  Cache readGhost;
  uint32_t readGhostReadHits;

  double demoteProb;
  double readProb;

public:
  CacheGhost(uint32_t inCacheSize) :
    demoteGhost(inCacheSize),
    demoteGhostReadHits(1),
    readGhost(inCacheSize),
    readGhostReadHits(1),
    demoteProb(0),
    readProb(0) { ; };

  void blockPut(IORequestOp_t op,
		Block block);

  void probUpdate(Block block);

  double probGet(IORequestOp_t op) {
    return (op == Demote ? demoteProb : readProb); };

  void statisticsReset();

  void statisticsShow() const;
};


#endif /* _CACHEGHOST_HH_ */
