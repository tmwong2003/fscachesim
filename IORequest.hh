/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequest.hh,v 1.8 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUEST_HH_
#define _IOREQUEST_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>

enum IORequestOp_t {Demote, Read};

class IORequest {
private:
  const char *orig;
  const uint64_t reqID;

  IORequestOp_t op;

  double timeIssued;

  uint64_t objID;
  uint64_t off;
  uint64_t len;

private:
  // Copy constructors - declared private and never defined

  IORequest& operator=(const IORequest&);

public:
  // Constructors and destructors

  IORequest(const char *inOrig,
	    IORequestOp_t inOp,
	    double inTimeIssued,
	    uint64_t inObjectID,
	    uint64_t inOff,
	    uint64_t inLen) :
    orig(inOrig),
    reqID(0),
    op(inOp),
    timeIssued(inTimeIssued),
    objID(inObjectID),
    off(inOff),
    len(inLen) { ; };

  IORequest(const IORequest& inIOReq) :
    orig(inIOReq.orig),
    reqID(inIOReq.reqID),
    op(inIOReq.op),
    timeIssued(inIOReq.timeIssued),
    objID(inIOReq.objID),
    off(inIOReq.off),
    len(inIOReq.len) { ; };

  ~IORequest() { ; };

  // Accessors

  const char *origGet() const {
    return (orig);
  };

  uint64_t reqIDGet() const {
    return (reqID);
  };

  IORequestOp_t opGet() const {
    return (op);
  };

  double timeIssuedGet() const {
    return (timeIssued);
  };

  uint64_t objIDGet() const {
    return (objID);
  };

  uint64_t offGet() const {
    return (off);
  };

  uint64_t lenGet() const {
    return (len);
  };

  uint64_t blockOffGet(const uint64_t inBlockSize) const {
    return (off / inBlockSize);
  };

  uint64_t blockLenGet(const uint64_t inBlockSize) const {
    //  The following expression is len + starting fill + ending fill.

    return ((len + (off % inBlockSize) +
	     ((len + off) % inBlockSize ?
	      inBlockSize - ((len + off) % inBlockSize) : 0)) / inBlockSize);
  };
};

#endif /* _IOREQUEST_HH_ */
