/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorGeneric.cc,v 1.4 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <string.h>

#include "IORequest.hh"

#include "IORequestGeneratorFileGeneric.hh"

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

void
IORequestGeneratorFileGeneric::IORequestQueue()
{
  if (nextRequest) {
    delete nextRequest;
    nextRequest = NULL;
  }

  if (file != NULL) {
    double timeIssued;
    int rc;
    uint64_t objID, off, len;

    rc = fscanf(file,
		"%lf %llu %llu %llu",
		&timeIssued,
		&objID,
		&off,
		&len);
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
				  objID,
				  off,
				  len);
    }
  }
}
