/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.hh,v 1.4 2000/10/26 16:14:24 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATOR_HH_
#define _IOREQUESTGENERATOR_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "IORequest.hh"
#include "Node.hh"

class IORequestGenerator {
protected:
  Node *node;

  IORequest *nextRequest;

private:
  IORequestGenerator(const IORequestGenerator&);
  IORequestGenerator& operator=(const IORequestGenerator&);

public:
  IORequestGenerator() :
    node(NULL),
    nextRequest(NULL) { ; };

  IORequestGenerator(Node *inNode) :
    node(inNode),
    nextRequest(NULL) { ; };

  virtual ~IORequestGenerator() {
    if (nextRequest) {
      delete nextRequest;
    }
  };

  const Node *nodeGet() const {
    return (node);
  };

  const IORequest *IORequestGet() const {
    return (nextRequest);
  };

  // Comparison operator '<' sorts by the issue time of the next request.
  // a < b is:
  //
  // true if b has a NULL nextRequest
  // false if a has a NULL nextRequest
  // the result of comparing the times with double-precision '<' otherwise

  bool operator<(const IORequestGenerator& inGenerator) const;

  // Send a queued request on to the caching node.

  virtual bool IORequestDown() = 0;
};

inline bool
IORequestGenerator::operator<(const IORequestGenerator& inGenerator) const
{
  bool retval;

  if (!inGenerator.nextRequest) {
    retval = true;
  }
  else if (!nextRequest) {
    retval = false;
  }
  else {
    retval = (nextRequest->timeIssuedGet() <
	      inGenerator.nextRequest->timeIssuedGet());
  }

  return (retval);
};

#endif /* _IOREQUESTGENERATOR_HH_ */
