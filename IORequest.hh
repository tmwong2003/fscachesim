/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _IOREQUEST_HH_
#define _IOREQUEST_HH_

typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

enum IORequestOp_t {Read};

class IORequest {
protected:
  IORequestOp_t op;

  uint32_t timeIssued;

  uint32_t devID;
  uint32_t objectID;
  uint32_t offset;
  uint32_t length;

private:
  // Copy constructors - declared private and never defined

  IORequest(const IORequest&);
  IORequest& operator=(const IORequest&);

public:
  // Constructors and destructors

  IORequest(IORequestOp_t inOp,
	    uint32_t inTimeIssued,
	    uint32_t inObjectID,
	    uint32_t inOffset,
	    uint32_t inLength) :
    op(inOp),
    timeIssued(inTimeIssued),
    objectID(inObjectID),
    offset(inOffset),
    length(inLength)
    { ; };
  ~IORequest()
    { ; };

  // Accessors

  uint32_t objectIDGet()
    { return (objectID); };
  uint32_t offsetGet()
    { return (offset); };
  uint32_t lengthGet()
    { return (length); };
};

#endif /* _IOREQUEST_HH_ */
