/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/main.cc,v 1.19 2001/11/20 02:20:14 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "BlockStore.hh"
#include "BlockStoreCache.hh"
#include "BlockStoreCacheSimple.hh"
#include "BlockStoreCacheSLRU.hh"
#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorFile.hh"
#include "IORequestGeneratorFileGeneric.hh"
#include "IORequestGeneratorFileMambo.hh"
#include "Node.hh"

// Command usage.

const char *globalProgArgs = "b:DdGgmns:uw:";

const char *globalProgUsage = \
"[-D] " \
"[-d] " \
"[-m] " \
"[-b block_size] " \
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
  // Default block size.

  uint64_t blockSize = 4096;

  // Default array parameters.

  uint64_t arrayProbCacheSize = 0;
  uint64_t arrayProbCacheSizeMB = 0;
  BlockStoreCacheSimple::EjectPolicy_t arrayEjectPolicy =
    BlockStoreCacheSimple::LRU;

  // Default client parameters.

  BlockStoreCacheSimple::DemotePolicy_t clientDemotePolicy = 
    BlockStoreCacheSimple::None;

  // Default warmups.

  uint64_t warmupCount = 0;
  double warmupTime = 0;

  // Default flags.

  bool useMamboFlag = false;

  // Process command-line args.

  bool errFlag = false;
  char opt;
  while (!errFlag && (opt = getopt(argc, argv, globalProgArgs)) != EOF) {
    switch (opt) {
    case 'b':
      blockSize = atol(optarg);
      break;
    case 'D':
      clientDemotePolicy = BlockStoreCacheSimple::DemoteDemand;
      break;
    case 'd':
      clientDemotePolicy = BlockStoreCacheSimple::DemoteDemand;
      arrayEjectPolicy = BlockStoreCacheSimple::MRU;
      break;
    case 'm':
      useMamboFlag = true;
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

  uint64_t clientCacheSizeMB = atol(argv[optind]);
  uint64_t clientCacheSize = clientCacheSizeMB * (globalMBToB / blockSize);
  uint64_t arrayCacheSizeMB = atol(argv[optind + 1]);
  uint64_t arrayCacheSize = arrayCacheSizeMB * (globalMBToB / blockSize);

  // Create a single array cache for all client-missed I/Os to feed into.

  BlockStore *arrayCache;
  {
    arrayCache = new BlockStoreCacheSimple("array",
					   blockSize,
					   arrayCacheSize,
					   arrayEjectPolicy,
					   BlockStoreCacheSimple::None);
  }
  generators->StatisticsAdd(arrayCache);
  Node array(arrayCache, NULL);

  for (int i = (optind + 2); i < argc; i++) {
    char buffer[40];

    sprintf(buffer, "%s", basename(argv[i]));
    BlockStoreCache *cache = new BlockStoreCacheSimple(buffer,
						       blockSize,
						       clientCacheSize,
						       BlockStoreCacheSimple::LRU,
						       clientDemotePolicy);
    generators->StatisticsAdd(cache);
    Node *client = new Node(cache, &array);

    // Create I/O generator based on the input trace type.

    IORequestGeneratorFile *generator;
    if (useMamboFlag) {
      generator = new IORequestGeneratorFileMambo(client, argv[i]);
    }
    else {
      generator = new IORequestGeneratorFileGeneric(client, argv[i]);
    }
    generators->IORequestGeneratorAdd(generator);
  }

  // Run until we have no more I/Os to process.

  while (generators->IORequestDown());

  // Show stats.

  printf("Client cache size %llu\n", clientCacheSizeMB);
  printf("Array cache size %llu\n", arrayCacheSizeMB);
  printf("Array cache policy %s-",
	 (arrayEjectPolicy == BlockStoreCacheSimple::LRU ? "LRU" : "MRU"));
  printf("%s\n",
	 (clientDemotePolicy == BlockStoreCacheSimple::DemoteDemand ? "LRU" : "NONE"));
  printf("Block size %llu\n", blockSize);
  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
