/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Node.cc,v 1.4 2000/10/24 19:54:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include "IORequest.hh"

#include "Node.hh"

bool
Node::IORequestDown(IORequest& inIOReq)
{
  list<IORequest> newIOReqList;

  // If this is the first I/O after the warmup time has passed, reset all
  // component parts.

  if (!warmupDone && inIOReq.timeIssuedGet() >= warmupTime) {
    blockStore->statisticsReset();
    warmupDone = true;
  }

  blockStore->IORequestDown(inIOReq, newIOReqList);

  if (nextNode) {
    for (list<IORequest>::iterator i = newIOReqList.begin();
	 i != newIOReqList.end();
	 i++) {
      // Forward the IO request to the next device.

      nextNode->IORequestDown(*i);
    }
  }

  return (true);
}
