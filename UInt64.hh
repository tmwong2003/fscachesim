/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/UInt64.hh,v 1.1 2002/02/11 20:11:28 tmwong Exp $
  Description:  uint64_t utility containers
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _UINT64_HH_
#define _UINT64_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <map>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

namespace UInt64 {
  using std::map;

  /**
   * "Less than" ordering function class for 64-bit integers
   */
  class LessThan {
  public:
    /**
     * "Less than" operator
     */
    bool operator()(const uint64_t i1, const uint64_t i2) const
    {
      return (i1 < i2);
    }
  };

  typedef map<uint64_t, uint64_t, LessThan> Counter;
};

#endif /* _UINT64_HH_ */
