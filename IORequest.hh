/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequest.hh,v 1.2 2000/09/22 16:15:39 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _IOREQUEST_HH_
#define _IOREQUEST_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

enum IORequestOp_t {Demote, Read};

class IORequest {
protected:
  IORequestOp_t op;

  double timeIssued;

  uint32_t devID;
  uint32_t objectID;
  uint32_t offset;
  uint32_t length;

private:
  // Copy constructors - declared private and never defined

  IORequest& operator=(const IORequest&);

public:
  // Constructors and destructors

  IORequest(IORequestOp_t inOp,
	    double inTimeIssued,
	    uint32_t inObjectID,
	    uint32_t inOffset,
	    uint32_t inLength) :
    op(inOp),
    timeIssued(inTimeIssued),
    objectID(inObjectID),
    offset(inOffset),
    length(inLength) { ; };

  IORequest(const IORequest& inIOReq) {
    op = inIOReq.op;
    timeIssued = inIOReq.timeIssued;
    devID = inIOReq.devID;
    objectID = inIOReq.objectID;
    offset = inIOReq.offset;
    length = inIOReq.length;
  };

  ~IORequest() { ; };

  // Accessors

  IORequestOp_t opGet() const {
    return (op);
  };

  double timeIssuedGet() const {
    return (timeIssued);
  };

  uint32_t objectIDGet() const {
    return (objectID);
  };

  uint32_t offsetGet() const {
    return (offset);
  };

  uint32_t lengthGet() const {
    return (length);
  };

  uint32_t blockOffsetGet(const uint32_t inBlockSize) const {
    return (offset / inBlockSize);
  };

  uint32_t blockLengthGet(const uint32_t inBlockSize) const {
    //  The following expression is length + starting fill + ending fill.

    return ((length + (offset % inBlockSize) +
	     ((length + offset) % inBlockSize ?
	      inBlockSize - ((length + offset) % inBlockSize) : 0)) / inBlockSize);
  };
};

#endif /* _IOREQUEST_HH_ */
