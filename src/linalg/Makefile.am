ACLOCAL_AMFLAGS = -I build-aux/m4

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = liblinalg.la
include_HEADERS = src/tc_mat.h
noinst_HEADERS =

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = liblinalg.pc

liblinalg_la_SOURCES = src/tc_mat.c
liblinalg_la_CPPFLAGS = -I$(top_srcdir)/include $(LINALG_INCLUDES)
liblinalg_la_LIBADD = $(LINALG_LIBS)


noinst_PROGRAMS =

EXTRA_DIST = autogen.sh
