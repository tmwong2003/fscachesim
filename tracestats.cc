/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/tracestats.cc,v 1.4 2000/10/26 02:00:50 tmwong Exp $
  Description:  Generate LRU and frequency trace stats using fscachesim
                objects.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <fcntl.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "IORequestGeneratorGeneric.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

const char *globalProgArgs = "b:mw:";
const char *globalProgUsage = "[-m] [-b block_size] [-w warmup_time] trace_file";

// Default command-line argument values.

const int globalBlockSize = 4096;

const char *globalFreqFilename = "freq";
const char *globalLRUFilename = "lru-cumul";
const char *globalSummaryFilename = "summary";

const double globalWarmupTime = 0;

// Other default values.

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

  BlockStoreInfinite cache(0, blockSize);
  Node host(&cache, NULL, warmupTime);
  int records = 0;

  for (int i = optind; i < argc; i++) {
    IORequestGenerator *generator;

    if (useMamboFlag) {
      generator = new IORequestGeneratorMambo(&host, argv[i]);
    }
    else {
      generator = new IORequestGeneratorGeneric(&host, argv[i]);
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

  // Output the stats to separate files.

  if (!stdoutRedirect(globalFreqFilename)) {
    exit(EXIT_FAILURE);
  }
  cache.statisticsFreqShow();

  if (!stdoutRedirect(globalLRUFilename)) {
    exit(EXIT_FAILURE);
  }
  cache.statisticsLRUCumulShow();

  if (!stdoutRedirect(globalSummaryFilename)) {
    exit(EXIT_FAILURE);
  }
  cache.statisticsSummaryShow();

  return (EXIT_SUCCESS);
}
