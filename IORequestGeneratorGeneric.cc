/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.cc,v 1.3 2000/10/24 19:54:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequestGeneratorGeneric.hh"

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

void
IORequestGeneratorGeneric::IORequestQueue()
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
