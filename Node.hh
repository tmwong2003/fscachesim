/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Node.hh,v 1.4 2000/10/24 19:54:42 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _NODE_HH_
#define _NODE_HH_

#include "BlockStore.hh"
#include "IORequest.hh"

class Node
{
private:
  BlockStore *blockStore;
  Node *nextNode;

  double warmupTime;
  bool warmupDone;

private:
  // Copy constructors - declared private and never defined

  Node(const Node&);
  Node& operator=(const Node&);

public:
  // Constructors and destructors

  Node(BlockStore *inBlockStore,
       Node *inNextNode,
       double inWarmupTime) :
    blockStore(inBlockStore),
    nextNode(inNextNode),
    warmupTime(inWarmupTime),
    warmupDone(false) { ; };
  ~Node() { ; };

  // Process incoming I/O requests

  bool IORequestDown(IORequest& inIOReq);
  int IORequestUp(IORequest& inIOReq);

  // Statistics management

  void statisticsReset() {
    blockStore->statisticsReset();
  };

  void statisticsShow() const {
    blockStore->statisticsShow();
  };
};

#endif /* _NODE_HH_ */
