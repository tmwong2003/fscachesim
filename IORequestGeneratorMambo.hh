/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGeneratorMambo.hh,v 1.2 2000/10/24 19:54:41 tmwong Exp $
  Description:  
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

#include "IORequestGenerator.hh"

class IORequestGeneratorMambo : public IORequestGenerator {
private:
  struct lt_string
  {
    bool operator()(const char *str1, const char *str2) const {
      return (strcmp(str1, str2) < 0);
    }
  };

  typedef map<const char *, int, lt_string> dir_t;
  typedef dir_t::iterator dir_iter_t;

  static dir_t static_dir_map;

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
    IORequestGenerator(inNode, inFilename) {
    headerProcess();
    IORequestQueue();
  };

  ~IORequestGeneratorMambo() { ; };
};

#endif /* _IOREQUESTGENERATORMAMBO_HH_ */
