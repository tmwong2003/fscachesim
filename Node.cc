/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include "IORequest.hh"

#include "Node.hh"

bool
Node::IORequestStart(IORequest &inIOReq)
{
  return (blockStore->IORequestHandle(inIOReq));
}

void
Node::StatisticsShow()
{
  blockStore->StatisticsShow();
}
