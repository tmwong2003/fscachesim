/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequest.hh,v 1.7 2001/11/16 23:32:46 tmwong Exp $
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
protected:
  const char *originator;
  const uint64_t requestID;

  IORequestOp_t op;

  double timeIssued;

  uint64_t devID;
  uint64_t objectID;
  uint64_t offset;
  uint64_t length;

private:
  // Copy constructors - declared private and never defined

  IORequest& operator=(const IORequest&);

public:
  // Constructors and destructors

  IORequest(const char *inOriginator,
	    IORequestOp_t inOp,
	    double inTimeIssued,
	    uint64_t inObjectID,
	    uint64_t inOffset,
	    uint64_t inLength) :
    originator(inOriginator),
    requestID(0),
    op(inOp),
    timeIssued(inTimeIssued),
    objectID(inObjectID),
    offset(inOffset),
    length(inLength) { ; };

  IORequest(const IORequest& inIOReq) :
    originator(inIOReq.originator),
    requestID(inIOReq.requestID),
    op(inIOReq.op),
    timeIssued(inIOReq.timeIssued),
    devID(inIOReq.devID),
    objectID(inIOReq.objectID),
    offset(inIOReq.offset),
    length(inIOReq.length) { ; };

  ~IORequest() { ; };

  // Accessors

  const char *originatorGet() const {
    return (originator);
  };

  uint64_t requestIDGet() const {
    return (requestID);
  };

  IORequestOp_t opGet() const {
    return (op);
  };

  double timeIssuedGet() const {
    return (timeIssued);
  };

  uint64_t objectIDGet() const {
    return (objectID);
  };

  uint64_t offsetGet() const {
    return (offset);
  };

  uint64_t lengthGet() const {
    return (length);
  };

  uint64_t blockOffsetGet(const uint64_t inBlockSize) const {
    return (offset / inBlockSize);
  };

  uint64_t blockLengthGet(const uint64_t inBlockSize) const {
    //  The following expression is length + starting fill + ending fill.

    return ((length + (offset % inBlockSize) +
	     ((length + offset) % inBlockSize ?
	      inBlockSize - ((length + offset) % inBlockSize) : 0)) / inBlockSize);
  };
};

#endif /* _IOREQUEST_HH_ */
