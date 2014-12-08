bin_PROGRAMS =
include_HEADERS =
lib_LTLIBRARIES =

all-local :

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS =

CLEANFILES =


include_HEADERS += sd-login.h


lib_LTLIBRARIES += libsystemd-login.la

libsystemd_login_la_LDFLAGS = -export-dynamic
libsystemd_login_la_CPPFLAGS = $(AM_CPPFLAGS)
libsystemd_login_la_CFLAGS = $(AM_CFLAGS)
libsystemd_login_la_LIBADD = libuseless-logind.la

libsystemd_login_la_SOURCES =			\
	sd-login.c


bin_PROGRAMS += systemd-loginctl

systemd_loginctl_LDFLAGS =
systemd_loginctl_CPPFLAGS = $(AM_CPPFLAGS)
systemd_loginctl_CFLAGS = $(AM_CFLAGS)
systemd_loginctl_LDADD = libsystemd-login.la

systemd_loginctl_SOURCES =			\
	loginctl.c


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsystemd-login.pc
