/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGenerator.hh,v 1.5 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATOR_HH_
#define _IOREQUESTGENERATOR_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "IORequest.hh"
#include "Store.hh"

class IORequestGenerator {
protected:
  Store *store;

  IORequest *nextRequest;

private:
  // Copy constructors - declared private and never defined

  IORequestGenerator(const IORequestGenerator&);
  IORequestGenerator& operator=(const IORequestGenerator&);

public:
  /**
   * Create a request stream generator.
   */
  IORequestGenerator() :
    store(NULL),
    nextRequest(NULL) { ; };

  /**
   * Create a request stream generator.
   */
  IORequestGenerator(Store *inStore) :
    store(inStore),
    nextRequest(NULL) { ; };

  /**
   * Destroy the generator.
   */
  virtual ~IORequestGenerator() {
    if (nextRequest) {
      delete nextRequest;
    }
  };

  const IORequest *IORequestGet() const {
    return (nextRequest);
  };

  /**
   * Comparison operator '<' sorts by the issue time of the next request.
   *
   * a < b is:
   *
   * true if b has a NULL nextRequest, false if a has a NULL nextRequest,
   * or the result of comparing the times with double-precision '<'
   * otherwise.
   */
  bool operator<(const IORequestGenerator& inGenerator) const;

  // Send a queued request on to the lower-level storage device.

  virtual bool IORequestDown() = 0;
};

inline bool
IORequestGenerator::operator<(const IORequestGenerator& inGenerator) const
{
  bool retval;

  if (!inGenerator.nextRequest) {
    retval = true;
  }
  else if (!nextRequest) {
    retval = false;
  }
  else {
    retval = (nextRequest->timeIssuedGet() <
	      inGenerator.nextRequest->timeIssuedGet());
  }

  return (retval);
};

#endif /* _IOREQUESTGENERATOR_HH_ */
