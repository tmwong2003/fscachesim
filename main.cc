/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/main.cc,v 1.6 2000/10/26 16:14:24 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <functional>
#include <stdio.h>
#include <stdlib.h>

#include "BlockStoreCache.hh"
#include "BlockStoreDisk.hh"
#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

const char *globalProgArgs = "b:mw:";

const char *globalProgUsage = \
"[-m] " \
"[-b block_size] " \
"[-w warmup_time] " \
"trace_files...";

// Default command-line argument values.

const int globalBlockSize = 4096;
const double globalWarmupTime = 0;

const int globalHostCacheSize = 16384;
const CacheDemotePolicy_t globalHostDemotePolicy = DemoteDemand;

const int globalArrayCacheSize = 16384;
const CacheReplPolicy_t globalArrayReplPolicy = MRU;

const int globalRecordsPerDot = 1000;

class IORequestGeneratorLess:
  public binary_function<IORequestGenerator *, IORequestGenerator *, bool> {
public:
  bool operator()(const IORequestGenerator *inGenL,
		  const IORequestGenerator *inGenR) {
    return (*inGenL < *inGenR);
  };
};

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
  list<IORequestGenerator *> generators;

  uint32_t blockSize = globalBlockSize;
  double warmupTime = globalWarmupTime;

  bool useMamboFlag = false;

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

  BlockStoreCache arrayCache(blockSize,
			     globalArrayCacheSize,
			     globalArrayReplPolicy,
			     None);
  Node array(&arrayCache, NULL, warmupTime);
  int records = 0;

  for (int i = optind; i < argc; i++) {
    BlockStoreCache *cache = new BlockStoreCache(blockSize,
						 globalHostCacheSize,
						 LRU,
						 globalHostDemotePolicy);
    Node *host = new Node(cache, &array, warmupTime);

    // Create I/O generator based on the input trace type.

    IORequestGenerator *generator;
    if (useMamboFlag) {
      generator = new IORequestGeneratorMambo(host, argv[i]);
    }
    else {
      generator = new IORequestGeneratorGeneric(host, argv[i]);
    }
    generators.push_back(generator);
  }

  bool requestProcessed;
  do {
    generators.sort(IORequestGeneratorLess());
    requestProcessed = generators.front()->IORequestDown();

    records++;
    if (records % globalRecordsPerDot == 0) {
      fprintf(stderr, ".");
      fflush(stderr);
    }
  } while (requestProcessed);
  fprintf(stderr, "\n");

  for (list<IORequestGenerator *>::iterator i; i != generators.end(); i++) {
    IORequestGenerator *generator = *i;

    printf("Stats for IORequestGenerator %s\n", generator->filenameGet());
    generator->nodeGet()->statisticsShow();
  }
  printf("Stats for array\n");
  array.statisticsShow();
}
