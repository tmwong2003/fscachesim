/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGeneratorGeneric.hh,v 1.2 2000/10/24 19:54:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORGENERIC_HH_
#define _IOREQUESTGENERATORGENERIC_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "IORequestGenerator.hh"

class IORequestGeneratorGeneric : public IORequestGenerator {
private:
  IORequestGeneratorGeneric(const IORequestGeneratorGeneric&);
  IORequestGeneratorGeneric& operator=(const IORequestGeneratorGeneric&);

protected:

  virtual void IORequestQueue();

public:
  IORequestGeneratorGeneric(Node *inNode,
			    const char *inFilename) :
    IORequestGenerator(inNode, inFilename) {
    IORequestQueue();
  };

  ~IORequestGeneratorGeneric() { ; };
};

#endif /* _IOREQUESTGENERATORGENERIC_HH_ */
