/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/main.cc,v 1.16 2001/07/19 20:24:54 tmwong Exp $
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
#include "BlockStoreCacheSegVariable.hh"
#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

// Command usage.

const char *globalProgArgs = "b:Ddgms:w:";

const char *globalProgUsage = \
"[-D] " \
"[-d] " \
"[-g] " \
"[-m] " \
"[-b block_size] " \
"[-s prob_cache_size] " \
"[-w warmup_time] " \
"client_cache_size array_cache_size trace_files...";

const int globalMBToB = 1048576;

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
  uint32_t arrayProbCacheSize = 0;
  CacheReplPolicy_t arrayReplPolicy = LRU;

  uint32_t blockSize = 4096;

  CacheDemotePolicy_t hostDemotePolicy = None;

  uint32_t warmupCount = 0;
  double warmupTime = 0;

  bool useGhostFlag = false;
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
    case 'D':
      hostDemotePolicy = DemoteDemand;
      break;
    case 'd':
      hostDemotePolicy = DemoteDemand;
      arrayReplPolicy = MRU;
      break;
    case 'g':
      useGhostFlag = true;
      break;
    case 'm':
      useMamboFlag = true;
      break;
    case 's':
      useSLRUcacheFlag = true;
      arrayProbCacheSize = atol(optarg) * globalMBToB / blockSize;
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

  // Get the cache sizes.

  uint32_t hostCacheSize = atol(argv[optind]) * globalMBToB / blockSize;
  uint32_t arrayCacheSize = atol(argv[optind + 1]) * globalMBToB / blockSize;

  // Create a single array cache for all client-missed I/Os to feed into.

  BlockStore *arrayCache;
  if (useSLRUcacheFlag) {
    arrayCache = new BlockStoreCacheSegmented("array",
					      blockSize,
					      arrayCacheSize,
					      arrayProbCacheSize);
  }
  else if (useGhostFlag) {
    arrayCache = new BlockStoreCacheSegVariable("array",
						blockSize,
						arrayCacheSize,
						10);
  }
  else {
    arrayCache = new BlockStoreCache("array",
				     blockSize,
				     arrayCacheSize,
				     arrayReplPolicy,
				     None);
  }
  generators->StatisticsAdd(arrayCache);
  Node array(arrayCache, NULL);

  for (int i = (optind + 2); i < argc; i++) {
    char buffer[40];

    sprintf(buffer, "%s", basename(argv[i]));
    BlockStoreCache *cache = new BlockStoreCache(buffer,
						 blockSize,
						 hostCacheSize,
						 LRU,
						 hostDemotePolicy);
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

  // Show stats.

  printf("Client cache size %ld\n", hostCacheSize * blockSize / globalMBToB);
  printf("Array cache size %ld\n", arrayCacheSize * blockSize / globalMBToB);
  if (useSLRUcacheFlag) {
    printf("Array prob cache size %ld\n",
	   arrayProbCacheSize * blockSize / globalMBToB);
    printf("Array cache policy SLRU-%s\n",
	   (hostDemotePolicy == DemoteDemand ? "SLRU" : "NONE"));
  }
  else if (useGhostFlag) {
    printf("Array cache policy SEGVAR-%s\n",
	   (hostDemotePolicy == DemoteDemand ? "SEGVAR" : "NONE"));
  }
  else {
    printf("Array cache policy %s-",
	   (arrayReplPolicy == LRU ? "LRU" : "MRU"));
    printf("%s\n",
	   (hostDemotePolicy == DemoteDemand ? "LRU" : "NONE"));
  }
  printf("Block size %ld\n", blockSize);
  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
