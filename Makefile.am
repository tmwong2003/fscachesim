# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Makefile.am,v 1.2 2000/09/22 16:15:39 tmwong Exp $
# Description: fscachesim Makefile.am
# Created by:  T.M. Wong <tmwong+@cs.cmu.edu>

EXTRA_DIST = reconf

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# Rules for building fscachesim

bin_PROGRAMS = fscachesim

fscachesim_SOURCES = \
	BlockStoreCache.cc \
	BlockStoreInfinite.cc \
	IORequestGenerator.cc \
	Node.cc \
	main.cc \
	top-down-size-splay.c
