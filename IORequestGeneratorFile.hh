/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/IORequestGeneratorFile.hh,v 1.4 2000/10/26 16:14:24 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#ifndef _IOREQUESTGENERATORFILE_HH_
#define _IOREQUESTGENERATORFILE_HH_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#include "IORequest.hh"
#include "IORequestGenerator.hh"
#include "Node.hh"

class IORequestGeneratorFile : public IORequestGenerator {
protected:
  const char *filename;
  FILE *file;

private:
  IORequestGeneratorFile(const IORequestGeneratorFile&);
  IORequestGeneratorFile& operator=(const IORequestGeneratorFile&);

protected:

  virtual void IORequestQueue() = 0;

public:
  IORequestGeneratorFile(Node *inNode,
			 const char *inFilename);

  virtual ~IORequestGeneratorFile();

  const char *filenameGet() const {
    return (filename);
  };

  virtual bool IORequestDown();
};

#endif /* _IOREQUESTGENERATORFILE_HH_ */
