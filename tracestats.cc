/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/tracestats.cc,v 1.2 2000/10/24 19:54:42 tmwong Exp $
  Description:  Generate LRU and frequency trace stats using fscachesim
                objects.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <fcntl.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>

#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "IORequestGeneratorMambo.hh"
#include "Node.hh"

const int globalBlockSize = 4096;

const char *globalFileFreq = "freq";
const char *globalFileLRU = "lru-cumul";

const int globalRecordsPerDot = 1000;

class IORequestGeneratorLess:
  public binary_function<IORequestGenerator *, IORequestGenerator *, bool> {
public:
  bool operator()(const IORequestGenerator *inGenL,
		  const IORequestGenerator *inGenR) {
    return (*inGenL < *inGenR);
  };
};

bool
stdoutRedirect(const char *inFile)
{
  int newStdoutFD;

  if ((newStdoutFD = open(inFile, O_CREAT|O_TRUNC|O_WRONLY, 0666)) < 0) {
    perror(inFile);

    return (false);
  }
  if (dup2(newStdoutFD, fileno(stdout)) < 0) {
    perror(inFile);

    return (false);
  }
  if (close(newStdoutFD) < 0) {
    perror(inFile);

    return (false);
  }

  return (true);
}

int
main(int argc, char *argv[])
{
  list<IORequestGenerator *> generators;
  BlockStoreInfinite cache(0, globalBlockSize);
  Node host(&cache, NULL);
  int records = 0;

  for (int i = 1; i < argc; i++) {
    IORequestGeneratorMambo *generator =
      new IORequestGeneratorMambo(&host, argv[i]);

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

  if (!stdoutRedirect(globalFileFreq)) {
    exit(EXIT_FAILURE);
  }
  cache.statisticsFreqShow();

  if (!stdoutRedirect(globalFileLRU)) {
    exit(EXIT_FAILURE);
  }
  cache.statisticsLRUCumulShow();

  return (EXIT_SUCCESS);
}
