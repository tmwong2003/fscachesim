/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/fscachesim.cc,v 1.3 2002/02/13 20:21:08 tmwong Exp $
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <functional>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "IORequest.hh"
#include "IORequestGeneratorBatch.hh"
#include "IORequestGeneratorFile.hh"
#include "IORequestGeneratorFileGeneric.hh"
#include "IORequestGeneratorFileMambo.hh"
#include "Store.hh"
#include "StoreCache.hh"
#include "StoreCacheSLRU.hh"
#include "StoreCacheSeg.hh"
#include "StoreCacheSimple.hh"

// Command usage.

const char *globalProgArgs = "b:DdGgmns:uw:";

const char *globalProgUsage = \
"[-D] " \
"[-d] " \
"[-g] " \
"[-m] " \
"[-b block_size] " \
"[-w warmup_time] " \
"client_cache_size array_cache_size trace_files...";

const int globalMBToB = 1048576;

const int globalStoreCacheSegSegCount = 10;

const int globalStoreCacheSegSegMultiplier = 2;

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

  uint64_t arrayProbSize = 0;
  uint64_t arrayProbSizeMB = 0;
  StoreCacheSimple::EjectPolicy_t arrayEjectPolicy =
    StoreCacheSimple::LRU;

  // Default client parameters.

  StoreCacheSimple::DemotePolicy_t clientDemotePolicy = 
    StoreCacheSimple::None;

  // Default warmups.

  uint64_t warmupCount = 0;
  double warmupTime = 0;

  // Default flags.

  bool useMamboFlag = false;

  bool useArraySegFlag = false;
  bool useArraySLRUFlag = false;

  bool useArraySegUniformFlag = false;
  bool useArraySegNormalizeGhostFlag = false;

  // Process command-line args.

  bool errFlag = false;
  char opt;
  while (!errFlag && (opt = getopt(argc, argv, globalProgArgs)) != EOF) {
    switch (opt) {
    case 'b':
      blockSize = atol(optarg);
      break;
    case 'D':
      clientDemotePolicy = StoreCacheSimple::Demand;
      break;
    case 'd':
      clientDemotePolicy = StoreCacheSimple::Demand;
      arrayEjectPolicy = StoreCacheSimple::MRU;
      break;
    case 'g':
      useArraySegFlag = true;
      break;
    case 'm':
      useMamboFlag = true;
      break;
    case 'n':
      useArraySegNormalizeGhostFlag = true;
      break;
    case 's':
      useArraySLRUFlag = true;
      arrayProbSizeMB = atol(optarg);
      arrayProbSize = arrayProbSizeMB * (globalMBToB / blockSize);
      break;
    case 'u':
      useArraySegUniformFlag = true;
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

  uint64_t clientSizeMB = atol(argv[optind]);
  uint64_t clientSize = clientSizeMB * (globalMBToB / blockSize);
  uint64_t arraySizeMB = atol(argv[optind + 1]);
  uint64_t arraySize = arraySizeMB * (globalMBToB / blockSize);

  // Create a single array cache for all client-missed I/Os to feed into.

  Store *array;
  if (useArraySLRUFlag) {
    array = new StoreCacheSLRU("array",
			       blockSize,
			       arraySize,
			       arrayProbSize);
  }
  else if (useArraySegFlag) {
    if (useArraySegUniformFlag) {
      array = new StoreCacheSeg("array",
				blockSize,
				arraySize,
				globalStoreCacheSegSegCount,
				useArraySegNormalizeGhostFlag);
    }
    else {
      array = new StoreCacheSeg("array",
				blockSize,
				arraySize,
				globalStoreCacheSegSegCount,
				globalStoreCacheSegSegMultiplier,
				useArraySegNormalizeGhostFlag);
    }
  }
  else {
    array = new StoreCacheSimple("array",
				 blockSize,
				 arraySize,
				 arrayEjectPolicy,
				 StoreCacheSimple::None);
  }
  generators->StatisticsAdd(array);

  // Create a client cache for each I/O request stream.

  for (int i = (optind + 2); i < argc; i++) {
    char buffer[40];

    sprintf(buffer, "%s", basename(argv[i]));
    StoreCacheSimple *client = new StoreCacheSimple(buffer,
						    array,
						    blockSize,
						    clientSize,
						    StoreCacheSimple::LRU,
						    clientDemotePolicy);
    generators->StatisticsAdd(client);

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

  printf("Client cache size %llu\n", clientSizeMB);
  printf("Array cache size %llu\n", arraySizeMB);
  printf("Array cache policy %s-",
	 (arrayEjectPolicy == StoreCacheSimple::LRU ? "LRU" : "MRU"));
  printf("%s\n",
	 (clientDemotePolicy == StoreCacheSimple::Demand ? "LRU" : "NONE"));
  printf("Block size %llu\n", blockSize);
  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
