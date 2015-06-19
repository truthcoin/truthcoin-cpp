AC_PREREQ([2.60])
AC_INIT([liblinalg],[0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AC_CANONICAL_HOST
AH_TOP([#ifndef LIBLINALG_CONFIG_H])
AH_TOP([#define LIBLINALG_CONFIG_H])
AH_BOTTOM([#endif //LIBLINALG_CONFIG_H])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

dnl make the compilation flags quiet unless V=1 is used
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

PKG_PROG_PKG_CONFIG

AC_PATH_TOOL(AR, ar)
AC_PATH_TOOL(RANLIB, ranlib)
AC_PATH_TOOL(STRIP, strip)

if test "x$CFLAGS" = "x"; then
  CFLAGS="-O3 -g"
fi

AC_PROG_CC_C99
if test x"$ac_cv_prog_cc_c99" == x"no"; then
  AC_MSG_ERROR([c99 compiler support required])
fi

case $host in
  *mingw*)
     use_pkgconfig=no
     ;;
   *)
     use_pkgconfig=yes
     ;;
esac

CFLAGS="$CFLAGS -W"

warn_CFLAGS="-Wall -Wextra -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes -Wno-unused-function"
saved_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $warn_CFLAGS"
AC_MSG_CHECKING([if ${CC} supports ${warn_CFLAGS}])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[char foo;]])],
    [ AC_MSG_RESULT([yes]) ],
    [ AC_MSG_RESULT([no])
      CFLAGS="$saved_CFLAGS"
    ])


AC_ARG_ENABLE(benchmark,
    AS_HELP_STRING([--enable-benchmark],[compile benchmark (default is yes)]),
    [use_benchmark=$enableval],
    [use_benchmark=yes])

AC_ARG_ENABLE(tests,
    AS_HELP_STRING([--enable-tests],[compile tests (default is yes)]),
    [use_tests=$enableval],
    [use_tests=yes])

AC_ARG_ENABLE(endomorphism,
    AS_HELP_STRING([--enable-endomorphism],[enable endomorphism (default is no)]),
    [use_endomorphism=$enableval],
    [use_endomorphism=no])

AC_CHECK_TYPES([__int128])

# AC_MSG_CHECKING([for __builtin_expect])
# AC_COMPILE_IFELSE([AC_LANG_SOURCE([[void myfunc() {__builtin_expect(0,0);}]])],
#     [ AC_MSG_RESULT([yes]);AC_DEFINE(HAVE_BUILTIN_EXPECT,1,[Define this symbol if __builtin_expect is available]) ],
#     [ AC_MSG_RESULT([no])
#     ])

# AC_CONFIG_HEADERS([src/liblinalg-config.h])
AC_CONFIG_FILES([Makefile liblinalg.pc])
AC_SUBST(LINALG_INCLUDES)
AC_SUBST(LINALG_LIBS)
AC_SUBST(LINALG_TEST_LIBS)
AC_SUBST(LINALG_TEST_INCLUDES)

# dnl make sure nothing new is exported so that we don't break the cache
PKGCONFIG_PATH_TEMP="$PKG_CONFIG_PATH"
unset PKG_CONFIG_PATH
PKG_CONFIG_PATH="$PKGCONFIG_PATH_TEMP"

AC_OUTPUT
