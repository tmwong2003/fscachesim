# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Makefile.am,v 1.12 2001/07/19 20:24:54 tmwong Exp $
# Description: fscachesim Makefile.am
# Created by:  T.M. Wong <tmwong+@cs.cmu.edu>

EXTRA_DIST = reconf

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# Rules for building fscachesim

bin_PROGRAMS = fscachesim sharestats tracestats

fscachesim_SOURCES = \
	CacheGhost.cc \
	BlockStoreCache.cc \
	BlockStoreCacheSegVariable.cc \
	BlockStoreCacheSegmented.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorGeneric.cc \
	IORequestGeneratorMambo.cc \
	Node.cc \
	main.cc \
	top-down-size-splay.c

sharestats_SOURCES = \
	BlockStoreShareStats.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorGeneric.cc \
	IORequestGeneratorMambo.cc \
	Node.cc \
	top-down-size-splay.c \
	sharestats.cc

tracestats_SOURCES = \
	BlockStoreInfinite.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorGeneric.cc \
	IORequestGeneratorMambo.cc \
	Node.cc \
	top-down-size-splay.c \
	tracestats.cc
