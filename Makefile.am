if ENABLE_SYSTEMD_COMPATIBILITY
SUBDIRS = systemd-compat
endif

bin_PROGRAMS =
include_HEADERS =
lib_LTLIBRARIES =
moduledir = $(libdir)/security
module_LTLIBRARIES =

all-local :

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS =

CLEANFILES =


include_HEADERS += libuseless-logind.h


lib_LTLIBRARIES += libuseless-logind.la

libuseless_logind_la_LDFLAGS = -export-dynamic
libuseless_logind_la_CPPFLAGS = $(AM_CPPFLAGS)
libuseless_logind_la_CFLAGS = $(AM_CFLAGS)
libuseless_logind_la_LIBADD =

libuseless_logind_la_SOURCES =			\
	libuseless-logind.c


bin_PROGRAMS += useless-logind

useless_logind_LDFLAGS =
useless_logind_CPPFLAGS = $(AM_CPPFLAGS)
useless_logind_CFLAGS = $(AM_CFLAGS)
useless_logind_LDADD = libuseless-logind.la

useless_logind_SOURCES =				\
	useless-logind.c				\
	useless-logind_messages.c			\
	useless-logind_session.c


module_LTLIBRARIES += pam_useless_logind.la

pam_useless_logind_la_LDFLAGS = -module -avoid-version
pam_useless_logind_la_CPPFLAGS = $(AM_CPPFLAGS)
pam_useless_logind_la_CFLAGS = $(AM_CFLAGS)
pam_useless_logind_la_LIBADD = $(PAM_LIBS)

pam_useless_logind_la_SOURCES =				\
	pam_useless_logind.c


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libuseless-logind.pc

SCRIPT_IN_FILES = data/useless-logind.service.in
