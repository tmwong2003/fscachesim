/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Node.cc,v 1.2 2000/09/22 16:15:39 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include "IORequest.hh"

#include "Node.hh"

bool
Node::IORequestDown(IORequest& inIOReq)
{
  list<IORequest> newIOReqList;

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
