/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Ghost.cc,v 1.3 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "Block.hh"
#include "IORequest.hh"

#include "Ghost.hh"

using Block::block_t;

void
Ghost::blockPut(IORequestOp_t inOp,
		Block::block_t block)
{
  switch(inOp) {
  case Demote:
    if (demote.isCached(block)) {

      demote.blockGet(block);
    }
    else if (demote.isFull()) {
      block_t ejectBlock;

      demote.blockGetAtHead(ejectBlock);
    }
    demote.blockPutAtTail(block);
    break;
  case Read:
    if (read.isCached(block)) {

      read.blockGet(block);
    }
    else if (read.isFull()) {
      block_t ejectBlock;

      read.blockGetAtHead(ejectBlock);
    }
    read.blockPutAtTail(block);
    break;
  default:
    abort();
  }
}

void
Ghost::probUpdate(Block::block_t block)
{
  if (demote.isCached(block)) {
    // If all we cached was demotes, we would have won.

    demote.blockGet(block);
    demote.blockPutAtTail(block);
    demoteReadHits++;
  }
  if (read.isCached(block)) {
    // If all we cached was reads, we would have won.

    read.blockGet(block);
    read.blockPutAtTail(block);
    readReadHits++;
  }

  // Update probabilities.

  demoteProb =
    (double)demoteReadHits / (demoteReadHits + readReadHits);
  readProb =
    (double)readReadHits / (demoteReadHits + readReadHits);
}

void
Ghost::statisticsReset()
{
  demoteReadHits = 1;
  readReadHits = 1;

  demoteProb = 0;
  readProb = 0;
}

void
Ghost::statisticsShow() const
{
  printf("Read hits (demote ghost) for %s %llu\n", nameGet(), demoteReadHits);
  printf("Read hits (read ghost) for %s %llu\n", nameGet(), readReadHits);
}
