# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/Makefile.am,v 1.14 2001/11/20 02:20:14 tmwong Exp $
# Description: fscachesim Makefile.am
# Created by:  T.M. Wong <tmwong+@cs.cmu.edu>

EXTRA_DIST = reconf

# We do not have _all_ GNU files so add this to avoid warnings
AUTOMAKE_OPTIONS = foreign

# Rules for building fscachesim

bin_PROGRAMS = fscachesim sharestats tracestats

fscachesim_SOURCES = \
	BlockStoreCache.cc \
	BlockStoreCacheSimple.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	Node.cc \
	fscachesim.cc \
	top-down-size-splay.c

sharestats_SOURCES = \
	BlockStoreShareStats.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	Node.cc \
	top-down-size-splay.c \
	sharestats.cc

tracestats_SOURCES = \
	BlockStoreInfinite.cc \
	IORequestGeneratorBatch.cc \
	IORequestGeneratorFile.cc \
	IORequestGeneratorFileGeneric.cc \
	IORequestGeneratorFileMambo.cc \
	Node.cc \
	top-down-size-splay.c \
	tracestats.cc
