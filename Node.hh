/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _NODE_HH_
#define _NODE_HH_

#include "BlockCache.hh"
#include "IORequest.hh"

class Node
{
private:
  BlockCache *blockStore;

private:
  // Copy constructors - declared private and never defined

  Node(const Node&);
  Node& operator=(const Node&);

public:
  // Constructors and destructors

  Node(BlockCache *inBlockStore) :
    blockStore(inBlockStore)
    { ; };
  ~Node()
    { ; };

  // Process incoming I/O requests

  bool IORequestStart(IORequest &inIOReq);
  int IORequestFinish(IORequest &inIOReq);

  // Output statistics

  void StatisticsShow();
};

#endif /* _NODE_HH_ */
