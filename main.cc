/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/main.cc,v 1.1.1.1 2000/09/21 16:25:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "BlockStoreCache.hh"
#include "BlockStoreDisk.hh"
#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "Node.hh"

const int globalBlockSize = 4096;

const int globalRecordsPerDot = 1000;

int
main(int argc, char *argv[])
{
  FILE *trace_file = stdin;
  //  BlockStoreInfinite cache(0, globalBlockSize);
  BlockStoreCache disk(globalBlockSize, 10000);
  BlockStoreCache cache(globalBlockSize, 10000);
  Node array(&disk, NULL);
  Node host(&cache, &array);
  int records = 0;

  // Get the file name.

  if (argc > 1) {
    if ((trace_file = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "ERROR: could not open %s\n", argv[1]);

      exit(EXIT_FAILURE);
    }
  }

  do {
    uint32_t objectID, offset, length;
    int rc;

    // Read I/O request.

    rc = fscanf(trace_file, "%u %u %u", &objectID, &offset, &length);
    if (rc == EOF) {
      break;
    }

    IORequest req(Read, 0, objectID, offset, length);
    host.IORequestDown(req);

    // Increment the record count.

    records++;
    if (records % globalRecordsPerDot == 0) {
      fprintf(stderr, ".");
      fflush(stderr);
    }
  } while (!feof(trace_file));
  fprintf(stderr, "\n");

  // Close the file.

  if (fclose(trace_file)) {
    fprintf(stderr, "WARNING: could not close %s\n", argv[1]);

    exit(EXIT_FAILURE);
  }

  host.StatisticsShow();
  array.StatisticsShow();
}
