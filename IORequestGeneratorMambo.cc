/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGenerator.cc,v 1.3 2000/10/24 19:54:41 tmwong Exp $
  Description:  Generate I/O requests from a Mambo trace file.
  Author:       A. Acharya <acha@cs.umd.edu>, T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "IORequestGeneratorMambo.hh"

#define MAMBO_MAX_LINE_LEN 1024

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

void
IORequestGeneratorMambo::headerProcess()
{
  char line[MAMBO_MAX_LINE_LEN];
  int len;

  /* parse the header */

  fscanf(file, "%d", &(traceHeader.numOfProcesses));
  assert(traceHeader.numOfProcesses > 0);

  fscanf(file, "%d", &(traceHeader.numOfFiles));
  assert(traceHeader.numOfFiles > 0);

  fscanf(file, "%d\n", &(traceHeader.numOfRecords));
  assert(traceHeader.numOfRecords > 0);

  len = sizeof(string_t)*traceHeader.numOfFiles;
  traceHeader.fileNames = new string_t[len];
  assert(traceHeader.fileNames);

  for (int i = 0; i < traceHeader.numOfFiles; i++) {
    int linelen;

    fscanf(file, "%d %s\n", &linelen, line);
    linelen++;
    assert(linelen > 1);

    /* round up linelen to the nearest  alignment */
    linelen = ((linelen + (SALIGN - 1))/SALIGN) * SALIGN;
    
    traceHeader.fileNames[i].length = linelen;
    traceHeader.fileNames[i].name   = new char[linelen];
    assert(traceHeader.fileNames[i].name);
    memcpy(traceHeader.fileNames[i].name,line,linelen);
    len += linelen;
  }

  traceHeader.offsetToTraceRecords = len;
}

void
IORequestGeneratorMambo::IORequestQueue()
{
  if (nextRequest) {
    delete nextRequest;
    nextRequest = NULL;
  }

  if (file != NULL) {
    bool gotEOF = false;

    // Keep scanning records until we get a read record.

    do {
      int rc;
      tracerec_t record;

      rc = fscanf(file,
		  "%d %d %d %d %lf %lf %ld %ld",
		  &record.op,
		  &record.numRecords,
		  &record.pid,
		  &record.fileId,
		  &record.wallClock,
		  &record.processClock,
		  &record.offset,
		  &record.length);

      if (rc == EOF) {
	// nextRequest might be non-NULL.

	nextRequest = NULL;
	if (ferror(file)) {
	  abort();
	}

	if (fclose(file) != 0) {
	  abort();
	}
	file = NULL;
	gotEOF = true;
      }
      else if (rc < 8) {
	// Got an incomplete record.

	abort();
      }
      else if (record.op == READ &&
	       record.length != 0 &&
	       record.fileId != (traceHeader.numOfFiles - 1)) {
	// Got a read record of non-zero length, that wasn't an access to a
	// Mambo miscellaneous "others" file.

	// Validate the record.

	if (record.op == LISTIO_HEADER) {
	  assert(record.numRecords >= 1);
	}
	else {
	  assert(record.numRecords == 1);
	}
	assert(record.pid >= 0);
	assert(record.fileId >= 0);
	assert(record.wallClock >= 0.0L);
	assert(record.processClock >= 0.0L);
	assert(record.offset >= 0L);
	assert(record.length >= 0);

	if (strcmp(traceHeader.fileNames[record.fileId].name, "others") == 0) {
	  fprintf(stderr, "O");
	}

	nextRequest = new IORequest(basename(filename),
				    Read,
				    record.wallClock,
				    record.fileId,
				    record.offset,
				    record.length);
      }
    } while (!gotEOF && !nextRequest);
  }
}
