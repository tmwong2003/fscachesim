/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Node.hh,v 1.5 2000/10/26 16:14:24 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _NODE_HH_
#define _NODE_HH_

#include "BlockStore.hh"
#include "IORequest.hh"
#include "Statistics.hh"

class Node {
private:
  BlockStore *blockStore;
  Node *nextNode;

private:
  // Copy constructors - declared private and never defined

  Node(const Node&);
  Node& operator=(const Node&);

public:
  // Constructors and destructors

  Node(BlockStore *inBlockStore,
       Node *inNextNode) :
    blockStore(inBlockStore),
    nextNode(inNextNode) { ; };
  ~Node() { ; };

  // Process incoming I/O requests

  bool IORequestDown(IORequest& inIOReq);
  int IORequestUp(IORequest& inIOReq);
};

#endif /* _NODE_HH_ */
