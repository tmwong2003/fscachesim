/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.cc,v 1.4 2000/10/25 03:32:30 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequestGenerator.hh"

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

IORequestGenerator::IORequestGenerator(Node *inNode,
				       const char *inFilename) :
  node(inNode),
  nextRequest(NULL)
{
  filename = strdup(inFilename);
  if ((file = fopen(filename, "r")) == NULL) {
    perror(filename);
    abort();
  }
}

IORequestGenerator::~IORequestGenerator()
{
  if (file != NULL && fclose(file) != 0) {
    perror(NULL);
    abort();
  }
  delete filename;
  if (nextRequest) {
    delete nextRequest;
  }
}

bool
IORequestGenerator::IORequestDown()
{
  bool retval = false;

  if (nextRequest) {
    retval = node->IORequestDown(*nextRequest);
    IORequestQueue();
  }

  return (retval);
}

bool
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
}
