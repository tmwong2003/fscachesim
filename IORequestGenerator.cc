/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.cc,v 1.1 2000/09/28 02:54:50 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequestGenerator.hh"

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

void
IORequestGenerator::IORequestQueue()
{
  if (nextRequest) {
    delete nextRequest;
    nextRequest = NULL;
  }

  if (file != NULL) {
    double timeIssued;
    int rc;
    uint32_t objectID, offset, length;

    rc = fscanf(file,
		"%lf %u %u %u",
		&timeIssued,
		&objectID,
		&offset,
		&length);
    if (rc == EOF) {
      nextRequest = NULL;
      if (ferror(file)) {
	abort();
      }

      if (fclose(file) != 0) {
	abort();
      }
      file = NULL;
      nextRequest = NULL;
    }
    else {
      nextRequest = new IORequest(basename(filename),
				  Read,
				  timeIssued,
				  objectID,
				  offset,
				  length);
    }
  }
}

IORequestGenerator::IORequestGenerator(Node *inNode,
				       const char *inFilename) :
  node(inNode),
  nextRequest(NULL)
{
  filename = strdup(inFilename);
  if ((file = fopen(filename, "r")) == NULL) {
    abort();
  }
  IORequestQueue();
}

IORequestGenerator::~IORequestGenerator()
{
  if (file != NULL && fclose(file) != 0) {
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
