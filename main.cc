/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/main.cc,v 1.18 2001/11/18 07:48:47 tmwong Exp $
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
#include "BlockStoreCacheSegVariableMulti.hh"
#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

// Command usage.

const char *globalProgArgs = "b:DdGgmns:uw:";

const char *globalProgUsage = \
"[-D] " \
"[-d] " \
"[-G] " \
"[-g] " \
"[-m] " \
"[-n] " \
"[-b block_size] " \
"[-s prob_cache_size] " \
"[-w warmup_time] " \
"client_cache_size array_cache_size trace_files...";

const int globalMBToB = 1048576;

const int globalCacheSegVariableSegCount = 10;

const int globalCacheSegVariableSegMultiplier = 2;

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
  uint64_t arrayProbCacheSize = 0;
  uint64_t arrayProbCacheSizeMB = 0;
  CacheReplPolicy_t arrayReplPolicy = LRU;

  uint64_t blockSize = 4096;

  CacheDemotePolicy_t hostDemotePolicy = None;

  uint64_t warmupCount = 0;
  double warmupTime = 0;

  bool useGhostFlag = false;
  bool useGhostMultiFlag = false;
  bool useMamboFlag = false;
  bool useNormalizeFlag = false;
  bool useSLRUcacheFlag = false;
  bool useUniformFlag = false;

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
    case 'G':
      useGhostMultiFlag = true;
      break;
    case 'g':
      useGhostFlag = true;
      break;
    case 'm':
      useMamboFlag = true;
      break;
    case 'n':
      useNormalizeFlag = true;
      break;
    case 's':
      useSLRUcacheFlag = true;
      arrayProbCacheSizeMB = atol(optarg);
      arrayProbCacheSize = arrayProbCacheSizeMB * (globalMBToB / blockSize);
      break;
    case 'u':
      useUniformFlag = true;
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

  uint64_t hostCacheSizeMB = atol(argv[optind]);
  uint64_t hostCacheSize = hostCacheSizeMB * (globalMBToB / blockSize);
  uint64_t arrayCacheSizeMB = atol(argv[optind + 1]);
  uint64_t arrayCacheSize = arrayCacheSizeMB * (globalMBToB / blockSize);

  // Create a single array cache for all client-missed I/Os to feed into.

  BlockStore *arrayCache;
  if (useSLRUcacheFlag) {
    arrayCache = new BlockStoreCacheSegmented("array",
					      blockSize,
					      arrayCacheSize,
					      arrayProbCacheSize);
  }
  else if (useGhostFlag) {
    if (useUniformFlag) {
      arrayCache = 
	new BlockStoreCacheSegVariable("array",
				       blockSize,
				       arrayCacheSize,
				       globalCacheSegVariableSegCount,
				       useNormalizeFlag);
    }
    else {
      arrayCache = 
	new BlockStoreCacheSegVariable("array",
				       blockSize,
				       arrayCacheSize,
				       globalCacheSegVariableSegCount,
				       globalCacheSegVariableSegMultiplier,
				       useNormalizeFlag);
    }
  }
  else if (useGhostMultiFlag) {
    if (useUniformFlag) {
      arrayCache = 
	new BlockStoreCacheSegVariableMulti("array",
					    blockSize,
					    arrayCacheSize,
					    globalCacheSegVariableSegCount,
					    useNormalizeFlag);
    }
    else {
      arrayCache = 
	new BlockStoreCacheSegVariableMulti("array",
					    blockSize,
					    arrayCacheSize,
					    globalCacheSegVariableSegCount,
					    globalCacheSegVariableSegMultiplier,
					    useNormalizeFlag);
    }
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

  printf("Client cache size %llu\n", hostCacheSizeMB);
  printf("Array cache size %llu\n", arrayCacheSizeMB);
  if (useSLRUcacheFlag) {
    printf("Array prob cache size %llu\n", arrayProbCacheSizeMB);
    printf("Array cache policy SLRU-%s\n",
	   (hostDemotePolicy == DemoteDemand ? "SLRU" : "NONE"));
  }
  else if (useGhostFlag) {
    if (useUniformFlag) {
      printf("Array cache policy %sSEG-%s%s\n",
	     (useNormalizeFlag ? "N" : "U"),
	     (useNormalizeFlag ? "N" : "U"),
	     (hostDemotePolicy == DemoteDemand ? "SEG" : "NONE"));
    }
    else {
      printf("Array cache policy %sSEGEXP-%s%s\n",
	     (useNormalizeFlag ? "N" : "U"),
	     (useNormalizeFlag ? "N" : "U"),
	     (hostDemotePolicy == DemoteDemand ? "SEGEXP" : "NONE"));
    }
  }
  else if (useGhostMultiFlag) {
    if (useUniformFlag) {
      printf("Array cache policy %sSEGMULTI-%s%s\n",
	     (useNormalizeFlag ? "N" : "U"),
	     (useNormalizeFlag ? "N" : "U"),
	     (hostDemotePolicy == DemoteDemand ? "SEGMULTI" : "NONE"));
    }
    else {
      printf("Array cache policy %sSEGEXPMULTI-%s%s\n",
	     (useNormalizeFlag ? "N" : "U"),
	     (useNormalizeFlag ? "N" : "U"),
	     (hostDemotePolicy == DemoteDemand ? "SEGEXPMULTI" : "NONE"));
    }
  }
  else {
    printf("Array cache policy %s-",
	   (arrayReplPolicy == LRU ? "LRU" : "MRU"));
    printf("%s\n",
	   (hostDemotePolicy == DemoteDemand ? "LRU" : "NONE"));
  }
  printf("Block size %llu\n", blockSize);
  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
