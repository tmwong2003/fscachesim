/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreShareStats.cc,v 1.1 2001/07/04 17:49:30 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
#include "IORequest.hh"

#include "BlockStoreShareStats.hh"

BlockStoreShareStats::~BlockStoreShareStats()
{
  for (OrigMap::iterator i = origToAccessMap.begin();
       i != origToAccessMap.end();
       i++) {
    delete i->second;
  };
}

bool
BlockStoreShareStats::IORequestDown(const IORequest& inIOReq,
				    list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  const char *orig = inIOReq.originatorGet();
  uint64_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint64_t i = 0; i < reqBlockLength; i++) {
    BlockMap *theMap = NULL;
    OrigMap::iterator origIter = origToAccessMap.find(orig);

    // Check to see if we've seen this originator before, and if so,
    // if this originator has seen this block.

    if (origIter != origToAccessMap.end()) {
      if (origIter->second->find(block) == origIter->second->end()) {
	// New access - update the number of originators accessing
	// this block.

	theMap = origToAccessMap[orig];
      }
    }
    else {
      origToAccessMap[orig] = new BlockMap;
      theMap = origToAccessMap[orig];
    }

    if (theMap != NULL) {
      (*theMap)[block] = 0;
      if (accessMap.find(block) == accessMap.end()) {
	accessMap[block] = 1;
      }
      else {
	accessMap[block]++;
      }
    }

    block.blockID++;
  }

  return (true);
}

void
BlockStoreShareStats::statisticsReset()
{
  accessMap.clear();
  origToAccessMap.clear();
}

void
BlockStoreShareStats::statisticsShow() const
{
  // Scan the counts of originators accessing each block, and return a
  // table of number of blocks against number of times that block was
  // accessed.

  int *blocksPerNumOrig = new int[(origToAccessMap.size() + 1)];
  for (int i = 1; i <= origToAccessMap.size(); i++) {
    blocksPerNumOrig[i] = 0;
  }

  for (BlockMap::const_iterator i = accessMap.begin();
       i != accessMap.end();
       i++) {
    blocksPerNumOrig[i->second]++;
  }

  for (int i = 1; i <= origToAccessMap.size(); i++) {
    printf("%d %d\n", i, blocksPerNumOrig[i]);
  }
}
