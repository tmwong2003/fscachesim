/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequest.hh,v 1.5 2000/10/24 19:54:41 tmwong Exp $
  Description:  Statistics interface for functions that keep and print stats.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _STATISTICS_HH_
#define _STATISTICS_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "string.h"

// class Statistics is an interface class - classes that subclass it must
// implement its methods.

class Statistics {
private:
  const char *name;

private:
  // Copy constructors - declared private and never defined

  Statistics(const Statistics&);
  Statistics& operator=(const Statistics&);

public:
  Statistics(const char *inName) {
    name = strdup(inName);
  };

  virtual ~Statistics() {
    delete name;
  };

  const char *nameGet() const {
    return (name);
  };

  virtual void statisticsReset() = 0;
  virtual void statisticsShow() const = 0;
};

#endif /* _STATISTICS_HH_ */
