# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Makefile.am,v 1.15 2002/02/12 00:38:54 tmwong Exp $
# Description: fscachesim Makefile.am
# Created by:  T.M. Wong <tmwong+@cs.cmu.edu>

EXTRA_DIST = reconf

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# Rules for building fscachesim

bin_PROGRAMS = fscachesim sharestats tracestats

fscachesim_SOURCES = \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	StoreCache.cc \
	StoreCacheSLRU.cc \
	StoreCacheSimple.cc \
	fscachesim.cc \
	top-down-size-splay.c

sharestats_SOURCES = \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	StoreSharestats.cc \
	top-down-size-splay.c \
	sharestats.cc

tracestats_SOURCES = \
	StoreInfinite.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	top-down-size-splay.c \
	tracestats.cc
