/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/CacheGhost.cc,v 1.1 2001/07/18 20:36:11 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "IORequest.hh"

#include "CacheGhost.hh"

void
CacheGhost::blockPut(IORequestOp_t inOp,
		     Block block)
{
  switch(inOp) {
  case Demote:
    if (demoteGhost.isCached(block)) {

      demoteGhost.blockGet(block);
    }
    else if (demoteGhost.isFull()) {
      Block ejectBlock;
      demoteGhost.blockGetAtHead(ejectBlock);
    }
    demoteGhost.blockPutAtTail(block);
    break;
  case Read:
    if (readGhost.isCached(block)) {

      readGhost.blockGet(block);
    }
    else if (readGhost.isFull()) {
      Block ejectBlock;
      readGhost.blockGetAtHead(ejectBlock);
    }
    readGhost.blockPutAtTail(block);
    break;
  default:
    abort();
  }
}

void
CacheGhost::probUpdate(Block block)
{
  if (demoteGhost.isCached(block)) {
    // If all we cached was demotes, we would have won.

    demoteGhost.blockGet(block);
    demoteGhost.blockPutAtTail(block);
    demoteGhostReadHits++;
  }
  if (readGhost.isCached(block)) {
    // If all we cached was reads, we would have won.

    readGhost.blockGet(block);
    readGhost.blockPutAtTail(block);
    readGhostReadHits++;
  }

  // Update probabilities.

  demoteProb =
    (double)demoteGhostReadHits / (demoteGhostReadHits + readGhostReadHits);
  readProb =
    (double)readGhostReadHits / (demoteGhostReadHits + readGhostReadHits);
}

void
CacheGhost::statisticsReset()
{
  demoteGhostReadHits = 1;
  readGhostReadHits = 1;

  demoteProb = 0;
  readProb = 0;
}

void
CacheGhost::statisticsShow() const
{
  printf("Read hits (demote ghost) %u\n", demoteGhostReadHits);
  printf("Read hits (read ghost) %u\n", readGhostReadHits);
}
