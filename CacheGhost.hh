/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/CacheGhost.hh,v 1.1 2001/07/18 20:36:11 tmwong Exp $
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

  bool normalizeFlag;

public:
  CacheGhost(uint32_t inCacheSize, bool inNormalizeFlag) :
    demoteGhost(inCacheSize),
    demoteGhostReadHits(1),
    readGhost(inCacheSize),
    readGhostReadHits(1),
    demoteProb(0),
    readProb(0),
    normalizeFlag(inNormalizeFlag) { ; };

  void blockPut(IORequestOp_t op,
		Block block);

  void probUpdate(Block block);

  double probGet(IORequestOp_t op);

  void statisticsReset();

  void statisticsShow() const;
};

inline double
CacheGhost::probGet(IORequestOp_t op)
{
  double retval;

  if (normalizeFlag) {
    retval =
      (op == Demote ? demoteProb : readProb) / max(demoteProb, readProb);
  }
  else {
    retval = (op == Demote ? demoteProb : readProb);
  }    
  return (retval);
}


#endif /* _CACHEGHOST_HH_ */
