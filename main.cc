/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/main.cc,v 1.5 2000/10/24 19:54:42 tmwong Exp $
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
#include "Node.hh"

const int globalBlockSize = 4096;

const int globalHostCacheSize = 16384;
const CacheDemotePolicy_t globalHostDemotePolicy = None;

const int globalArrayCacheSize = 262144;
const CacheReplPolicy_t globalArrayReplPolicy = LRU;

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
  BlockStoreCache arrayCache(globalBlockSize,
			     globalArrayCacheSize,
			     globalArrayReplPolicy,
			     None);
  Node array(&arrayCache, NULL);
  int records = 0;

  for (int i = 1; i < argc; i++) {
    BlockStoreCache *cache = new BlockStoreCache(globalBlockSize,
						 globalHostCacheSize,
						 LRU,
						 globalHostDemotePolicy);
    Node *host = new Node(cache, &array);
    IORequestGeneratorGeneric *generator =
      new IORequestGeneratorGeneric(host, argv[i]);

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
