/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/tracestats.cc,v 1.1 2000/10/02 18:17:42 tmwong Exp $
  Description:  Generate LRU and frequency trace stats using fscachesim
                objects.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <functional>
#include <stdio.h>
#include <stdlib.h>

#include "BlockStoreInfinite.hh"
#include "IORequest.hh"
#include "IORequestGenerator.hh"
#include "Node.hh"

const int globalBlockSize = 256;

const int globalRecordsPerDot = 1000;

class IORequestGeneratorLess:
  public binary_function<IORequestGenerator *, IORequestGenerator *, bool> {
public:
  bool operator()(const IORequestGenerator *inGenL,
		  const IORequestGenerator *inGenR) {
    return (*inGenL < *inGenR);
  };
};

int
main(int argc, char *argv[])
{
  list<IORequestGenerator *> generators;
  BlockStoreInfinite cache(0, globalBlockSize);
  Node host(&cache, NULL);
  int records = 0;

  for (int i = 1; i < argc; i++) {
    IORequestGenerator *generator = new IORequestGenerator(&host, argv[i]);

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

  host.statisticsShow();
}
