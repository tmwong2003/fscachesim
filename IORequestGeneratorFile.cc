/*
  RCS:          $Header: /afs/cs.cmu.edu/project/pdl-62/Cvs/fscachesim/IORequestGeneratorFile.cc,v 1.1 2000/10/30 01:12:44 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequestGeneratorFile.hh"

// tmwong 27 SEP 2000: I will replace abort() calls with exceptions "soon".

IORequestGeneratorFile::IORequestGeneratorFile(Node *inNode,
					       const char *inFilename) :
  IORequestGenerator(inNode)
{
  filename = strdup(inFilename);

  // tmwong 19 JUL 2001: Special case "-" means read records from stdin.

  if (strcmp(filename, "-") == 0) {
    file = stdin;
  }
  else if ((file = fopen(filename, "r")) == NULL) {
    perror(filename);
    abort();
  }
}

IORequestGeneratorFile::~IORequestGeneratorFile()
{
  if (file != NULL && fclose(file) != 0) {
    perror(NULL);
    abort();
  }
  delete filename;
}

bool
IORequestGeneratorFile::IORequestDown()
{
  bool retval = false;

  if (nextRequest) {
    retval = node->IORequestDown(*nextRequest);
    IORequestQueue();
  }

  return (retval);
}
