/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/main.cc,v 1.9 2001/06/30 21:56:02 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <functional>
#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <string.h>
#include <unistd.h>

#include "BlockStoreCache.hh"
#include "BlockStoreCacheSegmented.hh"
#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

// tmwong XXX: this is a big hack. Include the experimental parameters.

#include "experimentParams.hh"

// Command usage.

const char *globalProgArgs = "b:ms:w:";

const char *globalProgUsage = \
"[-m] " \
"[-b block_size] " \
"[-s prob_cache_size] " \
"[-w warmup_time] " \
"trace_files...";

// Default command-line argument values.

const int globalBlockSize = 4096;

void
usage(char *inProgName,
      int inExitStatus)
{
  printf("Usage: %s %s\n", basename(inProgName), globalProgUsage);

  exit(inExitStatus);
}

int
main(int argc,
     char *argv[])
{
  uint32_t blockSize = globalBlockSize;
  uint32_t probArrayCacheSize = 0;
  uint32_t warmupCount = 0;
  double warmupTime = 0;

  bool useMamboFlag = false;
  bool useSLRUcacheFlag = false;

  // Process command-line args.

  bool errFlag = false;
  char opt;
  while (!errFlag && (opt = getopt(argc, argv, globalProgArgs)) != EOF) {
    switch (opt) {
    case 'b':
      blockSize = atol(optarg);
      break;
    case 'm':
      useMamboFlag = true;
      break;
    case 's':
      useSLRUcacheFlag = true;
      probArrayCacheSize = atol(optarg);
      break;
    case 'w':
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

  // Create a single array cache for all client-missed I/Os to feed into.

  BlockStore *arrayCache;
  if (useSLRUcacheFlag) {
    arrayCache = new BlockStoreCacheSegmented("array",
					      blockSize,
					      globalArrayCacheSize,
					      probArrayCacheSize);
  }
  else {
    arrayCache = new BlockStoreCache("array",
				     blockSize,
				     globalArrayCacheSize,
				     globalArrayReplPolicy,
				     None);
  }
  generators->StatisticsAdd(arrayCache);
  Node array(arrayCache, NULL);

  for (int i = optind; i < argc; i++) {
    char buffer[40];

    sprintf(buffer, "%s", basename(argv[i]));
    BlockStoreCache *cache = new BlockStoreCache(buffer,
						 blockSize,
						 globalHostCacheSize,
						 LRU,
						 globalHostDemotePolicy);
    generators->StatisticsAdd(cache);
    Node *host = new Node(cache, &array);

    // Create I/O generator based on the input trace type.

    IORequestGeneratorFile *generator;
    if (useMamboFlag) {
      generator = new IORequestGeneratorMambo(host, argv[i]);
    }
    else {
      generator = new IORequestGeneratorGeneric(host, argv[i]);
    }
    generators->IORequestGeneratorAdd(generator);
  }

  // Run until we have no more I/Os to process.

  while (generators->IORequestDown());
  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
