/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.hh,v 1.1 2000/09/28 02:54:50 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATOR_HH_
#define _IOREQUESTGENERATOR_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>

#include "IORequest.hh"
#include "Node.hh"

class IORequestGenerator {
private:
  const char *filename;
  FILE *file;

  Node *node;

  IORequest *nextRequest;

private:
  IORequestGenerator(const IORequestGenerator&);
  IORequestGenerator& operator=(const IORequestGenerator&);

protected:

  void IORequestQueue();

public:
  IORequestGenerator(Node *inNode,
		     const char *inFilename);

  ~IORequestGenerator();

  bool IORequestDown();

  const char *filenameGet() const {
    return (filename);
  };

  const Node *nodeGet() const {
    return (node);
  };

  // Comparison operator '<' sorts by the issue time of the next request.
  // a < b is:
  //
  // true if b has a NULL nextRequest
  // false if a has a NULL nextRequest
  // the result of comparing the times with double-precision '<' otherwise

  bool operator<(const IORequestGenerator& inGenerator) const;
};

#endif /* _IOREQUESTGENERATOR_HH_ */
