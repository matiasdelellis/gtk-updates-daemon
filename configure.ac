AC_PREREQ(2.60)
AC_INIT([gtk-updates-daemon], [0.1.92], [mati86dl@gmail.com])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])

AC_PROG_CC
# Compiling sources with per-target flags requires AM_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= 3.0])
PKG_CHECK_MODULES(PACKAGEKIT, [packagekit-glib2 >= 1.0])
PKG_CHECK_MODULES(NOTIFY, [libnotify >= 0.7.6])

# Translation

AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=gtk-updates-daemon
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	po/Makefile.in
])
AC_OUTPUT
