/*
  RCS:          $Header:  $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequest.hh"
#include "Node.hh"

const int glbl_bytes_per_diskaddr = 4096;

const int glbl_recordReads_dot = 1000;

int
main(int argc, char *argv[])
{
  BlockCache cache(0, glbl_bytes_per_diskaddr);
  Node host(&cache);
  int recordReads = 0;
  FILE *trace_file = stdin;

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

    rc = fscanf(trace_file, "%lu %lu %lu", &objectID, &offset, &length);
    if (rc == EOF) {
      break;
    }

    IORequest req(Read, 0, objectID, offset, length);
    host.IORequestStart(req);

    // Increment the record count.

    recordReads++;
    if (recordReads % glbl_recordReads_dot == 0) {
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
}
