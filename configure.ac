# Header
AC_PREREQ([2.64])
AC_INIT([useless-logind], [0.1], [])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 parallel-tests foreign no-dist-gzip dist-xz color-tests subdir-objects])
AM_SILENT_RULES([yes])

# Check for programs
AC_PROG_CC

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

# Check for host OS
AC_CANONICAL_HOST
AC_MSG_CHECKING([for host OS])
case $host in
	*-*-linux*)
		AC_MSG_RESULT([Linux])
		AC_DEFINE([LINUX], [1], [])
		;;
	*-*-freebsd*)
		AC_MSG_RESULT([FreeBSD])
		AC_DEFINE([FREEBSD], [1], [])
		;;
esac

# Check dependencies
AC_CHECK_FUNCS([socket strdup getline])

AC_MSG_CHECKING([for PAM])
have_pam=no
AC_CHECK_HEADERS([security/pam_modules.h security/pam_ext.h],
                 [have_pam=yes],
                 [AC_MSG_ERROR([PAM headers not found !])])
AC_CHECK_LIB([pam], [pam_get_user],
             [have_pam=yes],
             [AC_MSG_ERROR([libpam not found !])])
if test x$have_pam = xyes; then
	PAM_LIBS="-lpam"
else
	AC_ERROR([useless-logind requires PAM to build ! Exiting...])
AC_SUBST(PAM_LIBS)
fi

# Process configure-time options
AC_ARG_ENABLE(systemd-compatibility,
              AS_HELP_STRING([--enable-systemd-compatibility],
                             [build with systemd-logind wrappers]),
                             [],[enable_systemd_compatibility=no])
AM_CONDITIONAL(ENABLE_SYSTEMD_COMPATIBILITY, test x$enable_systemd_compatibility = xyes)
if test x$enable_systemd_compatibility = xyes; then
	AC_DEFINE([ENABLE_SYSTEMD_COMPATIBILITY], [1], [Build with systemd-login wrappers])
fi

# Define generated files
AC_CONFIG_FILES([Makefile libuseless-logind.pc systemd-compat/Makefile systemd-compat/libsystemd-login.pc data/useless-logind.service])

# Finish
AC_OUTPUT

AC_MSG_RESULT([
 Build options:
	systemd compatibility		${enable_systemd_compatibility}
])
