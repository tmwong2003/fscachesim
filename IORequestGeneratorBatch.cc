/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorBatch.cc,v 1.1 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>

#include "IORequestGeneratorBatch.hh"

IORequestGeneratorBatch::~IORequestGeneratorBatch()
{
  if (recordsPerDot > 0) {
    fprintf(stderr, "\nProcessed %llu requests.\n", requestsIssued);
  }
}

bool
IORequestGeneratorBatch::IORequestDown()
{
  // Sort the IORequestGenerators by next-I/O issue time, and issue the
  // earliest request.

  generators.sort(IORequestGeneratorLess());
  const IORequest *req = generators.front()->IORequestGet();

  // If the next request passes the threshold for warmups, reset any
  // tracked stats.

  if (!warmupDoneFlag && req != NULL) {
    switch (warmupType) {
    case WarmupCount:
      if (requestsIssued >= warmupCount) {
	statisticsReset();
	warmupDoneFlag = true;
	fprintf(stderr,
		"\nWarmups done after %llu request(s)\n",
		requestsIssued);
      }
      break;
    case WarmupTime:
      if (req->timeIssuedGet() >= warmupTime) {
	statisticsReset();
	warmupDoneFlag = true;
	fprintf(stderr,
		"\nWarmups done at %f second(s) after %llu request(s)\n",
		warmupTime,
		requestsIssued);
      }
      break;
    default:
      // Warmups are always "done" if we never asked for any!

      abort();
    }
  }

  requestsIssued++;
  if (recordsPerDot > 0 && (requestsIssued % recordsPerDot == 0)) {
    fprintf(stderr, ".");
    fflush(stderr);
  }

  return (generators.front()->IORequestDown());
}

void
IORequestGeneratorBatch::statisticsReset()
{
  for (list<Statistics *>::iterator i =  statistics.begin();
       i != statistics.end();
       i++) {
    (*i)->statisticsReset();
  }
}

void
IORequestGeneratorBatch::statisticsShow() const
{
  for (list<Statistics *>::const_iterator i =  statistics.begin();
       i != statistics.end();
       i++) {
    //    printf("Stats for IORequestGenerator %s\n", generator->filenameGet());
    (*i)->statisticsShow();
  }
}
