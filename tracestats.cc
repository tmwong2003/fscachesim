/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/tracestats.cc,v 1.7 2001/06/30 21:56:02 tmwong Exp $
  Description:  Generate LRU and frequency trace stats using fscachesim
                objects.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <fcntl.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <string.h>
#include <unistd.h>

#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorFile.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

// Command usage.

const char *globalProgArgs = "b:f:c:mt:";

const char *globalProgUsage = "[-m] " \
"[-b block_size] " \
"[-f file_prefix] " \
"[-c warmup_count] "\
"[-t warmup_time] " \
"trace_file";

// Default command-line argument values.

const int globalBlockSize = 4096;

const char *globalFreqFileSuffix = "freq";
const char *globalLRUFileSuffix = "lru-cumul";
const char *globalSummaryFileSuffix = "summary";

void
usage(char *inProgName,
      int inExitStatus)
{
  printf("Usage: %s %s\n", basename(inProgName), globalProgUsage);

  exit(inExitStatus);
}

bool
stdoutRedirect(const char *inFilename)
{
  int newStdoutFD;

  if ((newStdoutFD = open(inFilename, O_CREAT|O_TRUNC|O_WRONLY, 0666)) < 0) {
    perror(inFilename);
 
    return (false);
  }
  if (dup2(newStdoutFD, fileno(stdout)) < 0) {
    perror(inFilename);
 
    return (false);
  }
  if (close(newStdoutFD) < 0) {
    perror(inFilename);
 
    return (false);
  }
 
  return (true);
}

int
main(int argc, char *argv[])
{
  char *filePrefix = "results";
  uint32_t blockSize = globalBlockSize;
  uint32_t warmupCount = 0;
  double warmupTime = 0;

  bool useMamboFlag = false;

  // Process command-line args.

  bool errFlag = false;
  char opt;
  while (!errFlag && (opt = getopt(argc, argv, globalProgArgs)) != EOF) {
    switch (opt) {
    case 'b':
      blockSize = atol(optarg);
      break;
    case 'c':
      warmupCount = strtoul(optarg, NULL, 0);
      break;
    case 'f':
      filePrefix = optarg;
      break;
    case 'm':
      useMamboFlag = true;
      break;
    case 't':
      warmupTime = strtod(optarg, NULL);
      break;
    default:
      errFlag = true;
    }
  }

  if (errFlag || optind >= argc) {
    usage(argv[0], EXIT_FAILURE);
  }

  // Set the warmup style for this experiment.

  IORequestGeneratorBatch *generators;
  if (warmupCount > 0) {
    generators = new IORequestGeneratorBatch(warmupCount);
  }
  else if (warmupTime > 0) {
    generators = new IORequestGeneratorBatch(warmupTime);
  }
  else {
    generators = new IORequestGeneratorBatch();
  }

  // Create a single infinite cache for all I/Os to feed into.

  BlockStoreInfinite cache("cache-infinite", 0, blockSize);
  generators->StatisticsAdd(&cache);
  Node host(&cache, NULL);

  for (int i = optind; i < argc; i++) {
    // Create I/O generator based on the input trace type.

    IORequestGeneratorFile *generator;
    if (useMamboFlag) {
      generator = new IORequestGeneratorMambo(&host, argv[i]);
    }
    else {
      generator = new IORequestGeneratorGeneric(&host, argv[i]);
    }
    generators->IORequestGeneratorAdd(generator);
  }

  // Run until we have no more I/Os to process.

  while (generators->IORequestDown());

  // Output the stats to separate files.

  {
    char buffer[1024];

    sprintf(buffer, "%s.%s", filePrefix, globalFreqFileSuffix);
    if (!stdoutRedirect(buffer)) {
      exit(EXIT_FAILURE);
    }
    cache.statisticsFreqShow();

    sprintf(buffer, "%s.%s", filePrefix, globalLRUFileSuffix);
    if (!stdoutRedirect(buffer)) {
      exit(EXIT_FAILURE);
    }
    cache.statisticsLRUCumulShow();

    sprintf(buffer, "%s.%s", filePrefix, globalSummaryFileSuffix);
    if (!stdoutRedirect(buffer)) {
      exit(EXIT_FAILURE);
    }
    cache.statisticsSummaryShow();
  }

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
