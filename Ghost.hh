/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Ghost.hh,v 1.3 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _GHOST_HH_
#define _GHOST_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "Block.hh"
#include "Cache.hh"
#include "IORequest.hh"
#include "Statistics.hh"

class Ghost : public Statistics {
private:
  Cache demote;
  uint64_t demoteReadHits;

  Cache read;
  uint64_t readReadHits;

  double demoteProb;
  double readProb;

  bool normalizeFlag;

private:
  // Copy constructors - declared private and never defined

  Ghost(const Ghost&);
  Ghost& operator=(const Ghost&);

public:
  Ghost(const char *inName,
	uint64_t inCacheSize,
	bool inNormalizeFlag) :
    Statistics(inName),
    demote(inCacheSize),
    demoteReadHits(1),
    read(inCacheSize),
    readReadHits(1),
    demoteProb(0),
    readProb(0),
    normalizeFlag(inNormalizeFlag) { ; };

  void blockPut(IORequestOp_t op,
		Block::block_t block);

  void probUpdate(Block::block_t block);

  double probGet(IORequestOp_t op);

  void statisticsReset();

  void statisticsShow() const;
};

inline double
Ghost::probGet(IORequestOp_t op)
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


#endif /* _GHOST_HH_ */
