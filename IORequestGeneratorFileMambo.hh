/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorMambo.hh,v 1.5 2001/07/05 14:00:54 tmwong Exp $
  Description:  Generate I/O requests from a Mambo trace file.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORFILEMAMBO_HH_
#define _IOREQUESTGENERATORFILEMAMBO_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

extern "C" {
#include "mambolib/iotrace.h"
}

#include "IORequestGeneratorFile.hh"

class IORequestGeneratorFileMambo : public IORequestGeneratorFile {
private:
  static Char::Counter staticDir;
  static uint64_t staticDirID;

  traceHeader_t traceHeader;
private:
  IORequestGeneratorFileMambo(const IORequestGeneratorFileMambo&);
  IORequestGeneratorFileMambo& operator=(const IORequestGeneratorFileMambo&);

  void headerProcess();

protected:

  virtual void IORequestQueue();

public:
  IORequestGeneratorFileMambo(Node *inNode,
			      const char *inFilename) :
    IORequestGeneratorFile(inNode, inFilename) {
      headerProcess();
      IORequestQueue();
  };

  virtual ~IORequestGeneratorFileMambo();
};

#endif /* _IOREQUESTGENERATORFILEMAMBO_HH_ */
