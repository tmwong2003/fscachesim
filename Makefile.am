# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/Makefile.am,v 1.5 2000/10/25 03:32:30 tmwong Exp $
# Description: fscachesim Makefile.am
# Created by:  T.M. Wong <tmwong+@cs.cmu.edu>

EXTRA_DIST = reconf

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# Rules for building fscachesim

bin_PROGRAMS = fscachesim tracestats

fscachesim_SOURCES = \
	BlockStoreCache.cc \
	BlockStoreInfinite.cc \
	IORequestGenerator.cc \
	IORequestGeneratorGeneric.cc \
	IORequestGeneratorMambo.cc \
	Node.cc \
	main.cc \
	top-down-size-splay.c

tracestats_SOURCES = \
	BlockStoreInfinite.cc \
	IORequestGenerator.cc \
	IORequestGeneratorGeneric.cc \
	IORequestGeneratorMambo.cc \
	Node.cc \
	top-down-size-splay.c \
	tracestats.cc
