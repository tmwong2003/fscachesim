/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/fscachesim.cc,v 1.6 2002/02/18 00:23:45 tmwong Exp $
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <fcntl.h>
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
#include "StoreCacheSeg.hh"
#include "StoreCacheSimple.hh"

// Command usage.

const char *globalProgArgs = "b:dmo:w:";

const char *globalProgUsage = \
"[-b block_size] " \
"[-d] " \
"[-m] " \
"[-o output_file_prefix] " \
"[-w warmup_time] " \
"array_cache_type client_cache_size array_cache_size trace_files...";

const int globalMBToB = 1048576;

const int globalStoreCacheSegSegCount = 10;

const int globalStoreCacheSegSegMultiplier = 2;

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

  // Default output file prefix.

  const char *outFilePrefix = NULL;

  // Default warmups.

  uint64_t warmupCount = 0;
  double warmupTime = 0;

  // Default flags.

  bool useDemoteFlag = false;

  bool useMamboFlag = false;

  bool useArraySLRUFlag = false;

  // Process command-line args.

  bool errFlag = false;
  char opt;
  while (!errFlag && (opt = getopt(argc, argv, globalProgArgs)) != EOF) {
    switch (opt) {
    case 'b':
      blockSize = atol(optarg);
      break;
    case 'd':
      useDemoteFlag = true;
      break;
    case 'o':
      outFilePrefix = optarg;
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

  if (optind + 1 >= argc || atol(argv[optind + 1]) == 0) {
    usage(argv[0], EXIT_FAILURE);
  }

  uint64_t clientSizeMB = atol(argv[optind + 1]);
  uint64_t clientSize = clientSizeMB * (globalMBToB / blockSize);

  if (optind + 2 >= argc || atol(argv[optind + 2]) == 0) {
    usage(argv[0], EXIT_FAILURE);
  }

  uint64_t arraySizeMB = atol(argv[optind + 2]);
  uint64_t arraySize = arraySizeMB * (globalMBToB / blockSize);

  // Create an array cache to receive all I/O requests that miss in the
  // clients.

  const char *arrayType = argv[optind];
  Store *array;
  if (!strcmp("LRU", arrayType)) {
    array = new StoreCacheSimple("array",
				 blockSize,
				 arraySize,
				 StoreCacheSimple::LRU,
				 StoreCacheSimple::None);
    fprintf(stderr,
	    "LRU: "
	    "Simple array, size %llu MB eject policy LRU\n",
	    arraySizeMB);
  }
  else if (!strcmp("MRULRU", arrayType)) {
    array = new StoreCacheSimple("array",
				 blockSize,
				 arraySize,
				 StoreCacheSimple::MRU,
				 StoreCacheSimple::None);
    fprintf(stderr,
	    "MRULRU: "
	    "Simple array, size %llu MB eject policy MRULRU\n",
	    arraySizeMB);
  }
  else if (!strcmp("NSEGEXP", arrayType)) {
    array = new StoreCacheSeg("array",
			      blockSize,
			      arraySize,
			      globalStoreCacheSegSegCount,
			      globalStoreCacheSegSegMultiplier,
			      true);
    fprintf(stderr,
	    "NSEGEXP: "
	    "Segmented normalized adaptive array, size %llu MB exp segs %d\n",
	    arraySizeMB,
	    globalStoreCacheSegSegCount);
  }
  else if (!strcmp("NSEGUNI", arrayType)) {
    array = new StoreCacheSeg("array",
			      blockSize,
			      arraySize,
			      globalStoreCacheSegSegCount,
			      true);
    fprintf(stderr,
	    "NSEGUNI: "
	    "Segmented normalized adaptive array, size %llu MB uni segs %d \n",
	    arraySizeMB,
	    globalStoreCacheSegSegCount);
  }
  else if (!strcmp("RSEGEXP", arrayType)) {
    array = new StoreCacheSeg("array",
			      blockSize,
			      arraySize,
			      globalStoreCacheSegSegCount,
			      globalStoreCacheSegSegMultiplier,
			      false);
    fprintf(stderr,
	    "RSEGEXP: "
	    "Segmented raw adaptive array, size %llu MB exp segs %d\n",
	    arraySizeMB,
	    globalStoreCacheSegSegCount);
  }
  else if (!strcmp("RSEGUNI", arrayType)) {
    array = new StoreCacheSeg("array",
			      blockSize,
			      arraySize,
			      globalStoreCacheSegSegCount,
			      false);
    fprintf(stderr,
	    "RSEGUNI: "
	    "Segmented raw adaptive array, size %llu MB uni segs %d \n",
	    arraySizeMB,
	    globalStoreCacheSegSegCount);
  }
  else {
    fprintf(stderr,
	    "%s: Unrecognized array cache type\n",
	    basename (argv[0]));
    usage(argv[0], EXIT_FAILURE);
  }
  generators->StatisticsAdd(array);

  // Create a client cache for each I/O request stream.

  for (int i = (optind + 3); i < argc; i++) {
    char buffer[40];

    sprintf(buffer, "%s", basename(argv[i]));
    StoreCacheSimple *client =
      new StoreCacheSimple(buffer,
			   array,
			   blockSize,
			   clientSize,
			   StoreCacheSimple::LRU,
			   (useDemoteFlag ?
			    StoreCacheSimple::Demand :
			    StoreCacheSimple::None));
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

  // Prepare output files.

  {
    char filename[1024];

    sprintf(filename,
	    "%s-%s-%s-%llu-%llu",
	    outFilePrefix,
	    (useDemoteFlag ? "DEMOTE" : "NONE"),
	    arrayType,
	    clientSizeMB,
	    arraySizeMB);
    if (!stdoutRedirect(filename)) {
      exit(EXIT_FAILURE);
    }
  }

  // Run until we have no more I/Os to process.

  while (generators->IORequestDown());

  // Dump out the statistics.

  generators->statisticsShow();

  // Clean up after ourselves.

  delete generators;

  return (EXIT_SUCCESS);
}
