/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Block.hh,v 1.1 2002/02/11 20:08:22 tmwong Exp $
  Description:  Disk block type abstraction
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _BLOCK_HH_
#define _BLOCK_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <map>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

namespace Block {
  using std::map;

  typedef struct {
    uint64_t objID;
    uint64_t blockID;
  } block_t;

  class LessThan {
  public:
    bool operator()(const block_t &block1, const block_t &block2) const {
      return (block1.objID < block2.objID ||
	      (block1.objID == block2.objID &&
	       block1.blockID < block2.blockID));
    }
  };

  typedef map<block_t, uint64_t, LessThan> Counter;
};

#endif /* _BLOCK_HH_ */
