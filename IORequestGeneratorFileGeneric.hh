/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorGeneric.hh,v 1.3 2000/10/30 01:12:45 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORFILEGENERIC_HH_
#define _IOREQUESTGENERATORFILEGENERIC_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "IORequestGeneratorFile.hh"

class IORequestGeneratorFileGeneric : public IORequestGeneratorFile {
private:
  // Copy constructors - declared private and never defined

  IORequestGeneratorFileGeneric(const IORequestGeneratorFileGeneric&);
  IORequestGeneratorFileGeneric& operator=(const IORequestGeneratorFileGeneric&);

protected:
  virtual void IORequestQueue();

public:
  IORequestGeneratorFileGeneric(Node *inNode,
				const char *inFilename) :
    IORequestGeneratorFile(inNode, inFilename) {
    IORequestQueue();
  };

  virtual ~IORequestGeneratorFileGeneric() { ; };
};

#endif /* _IOREQUESTGENERATORFILEGENERIC_HH_ */
