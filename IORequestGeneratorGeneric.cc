/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorGeneric.cc,v 1.2 2000/10/28 22:20:59 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <string.h>

#include "IORequest.hh"

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
