/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorBatch.hh,v 1.2 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORBATCH_HH_
#define _IOREQUESTGENERATORBATCH_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <list>

#include "IORequestGenerator.hh"
#include "Statistics.hh"

class IORequestGeneratorBatch :
  public IORequestGenerator,
  public Statistics {
protected:
  class IORequestGeneratorLess:
    public binary_function<IORequestGenerator *, IORequestGenerator *, bool> {
  public:
    bool operator()(const IORequestGenerator *inGenL,
		    const IORequestGenerator *inGenR) {
      return (*inGenL < *inGenR);
    };
  };

  list<IORequestGenerator *> generators;

  list<Statistics *> statistics;
  uint64_t requestsIssued;

  enum {WarmupNone, WarmupCount, WarmupTime} warmupType;
  uint64_t warmupCount;
  double warmupTime;
  bool warmupDoneFlag;

  uint64_t recordsPerDot;

private:
  IORequestGeneratorBatch(const IORequestGeneratorBatch&);
  IORequestGeneratorBatch& operator=(const IORequestGeneratorBatch&);

public:
  IORequestGeneratorBatch() :
    IORequestGenerator(),
    Statistics(""),
    generators(),
    requestsIssued(0),
    warmupType(WarmupNone),
    warmupCount(0),
    warmupTime(0),
    warmupDoneFlag(true),
    recordsPerDot(1000) { ; };

  IORequestGeneratorBatch(uint64_t inWarmupCount) :
    IORequestGenerator(),
    Statistics(""),
    generators(),
    requestsIssued(0),
    warmupType(WarmupCount),
    warmupCount(inWarmupCount),
    warmupTime(0),
    warmupDoneFlag(false),
    recordsPerDot(1000) { ; };

  IORequestGeneratorBatch(double inWarmupTime) :
    IORequestGenerator(),
    Statistics(""),
    generators(),
    requestsIssued(0),
    warmupType(WarmupTime),
    warmupCount(0),
    warmupTime(inWarmupTime),
    warmupDoneFlag(false),
    recordsPerDot(1000) { ; };

  virtual ~IORequestGeneratorBatch();

  void IORequestGeneratorAdd(IORequestGenerator *inGenerator) {
    generators.push_back(inGenerator);
  };

  void StatisticsAdd(Statistics *inStatistics) {
    statistics.push_back(inStatistics);
  };

  virtual bool IORequestDown();

  // Statistics management

  virtual void statisticsReset();

  virtual void statisticsShow() const;
};

#endif /* _IOREQUESTGENERATORBATCH_HH_ */
