/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/IORequestGeneratorMambo.hh,v 1.4 2000/10/30 01:12:45 tmwong Exp $
  Description:  Generate I/O requests from a Mambo trace file.
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORMAMBO_HH_
#define _IOREQUESTGENERATORMAMBO_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

extern "C" {
#include "mambolib/iotrace.h"
}

#include "IORequestGeneratorFile.hh"

struct lt_string
{
  bool operator()(const char *str1, const char *str2) const {
    return (strcmp(str1, str2) < 0);
  }
};

typedef map<const char *, int, lt_string> dir_t;

class IORequestGeneratorMambo : public IORequestGeneratorFile {
private:
  static dir_t staticDir;
  static int staticDirID;

  traceHeader_t traceHeader;
private:
  IORequestGeneratorMambo(const IORequestGeneratorMambo&);
  IORequestGeneratorMambo& operator=(const IORequestGeneratorMambo&);

  void headerProcess();

protected:

  virtual void IORequestQueue();

public:
  IORequestGeneratorMambo(Node *inNode,
			  const char *inFilename) :
    IORequestGeneratorFile(inNode, inFilename) {
      headerProcess();
      IORequestQueue();
  };

  virtual ~IORequestGeneratorMambo();
};

#endif /* _IOREQUESTGENERATORMAMBO_HH_ */
