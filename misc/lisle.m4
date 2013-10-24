## -*-M4-*-
#
# Configure paths to build a project using the Lisle C++ library
#
# Call this script from a configure.in script with:
# AC_PATH_LISLE([MINIMUM-VERSION, [ACTION_IF_FOUND, [ACTION-IF-NOT-FOUND]]])
#
# Warning: You must set the language to compile/link configure test programs
#          to C++. Add the following line in your configure.in file before
#          the AC_PATH_LISLE call:
#          AC_LANG_CPLUSPLUS
#
# AC_SUBST are done on:
#   LISLE_CFLAGS
#   LISLE_LIBS
#   LISLE_LIBS_G   <-- Same as LISLE_LIBS, but for debug version of library
# You can use @LISLE_CFLAGS@ and @LISLE_LIBS@ in your Makefile.am and get
# them replaced with the values detected by the macros stored in this file,
# and thus geting an accurate Makefile to build and link your project with
# Lisle.
#
# Adapted from "Configure path for GLIB", Owen Taylor.
#

AC_DEFUN([AC_PATH_LISLE],[
  AC_ARG_WITH(lisle-prefix,
              AC_HELP_STRING([--with-lisle-prefix=PREFIX],
                             [Prefix where Lisle is installed (optional)]),
              lisle_config_prefix="$withval",
              lisle_config_prefix="")
  AC_ARG_WITH(lisle-libdir,
              AC_HELP_STRING([--with-lisle-libdir=DIR],
                             [Directory where the Lisle library is installed (optional)]),
              lisle_config_libdir="$withval",
              lisle_config_libdir="")
  AC_ARG_WITH(lisle-includedir,
              AC_HELP_STRING([--with-lisle-includedir=DIR],
                             [Directory where the Lisle header files are installed, without /lisle/ (optional)]),
              lisle_config_includedir="$withval",
              lisle_config_includedir="")
  AC_ARG_WITH(lisle-bindir,
              AC_HELP_STRING([--with-lisle-exec-prefix=PREFIX],
                             [Prefix where the Lisle executables are installed (optional)]),
              lisle_config_bindir="$withval",
              lisle_config_bindir="")
  AC_ARG_ENABLE(lisletest,
                AC_HELP_STRING([--disable-lisletest],
                               [Do not try to compile and run a test Lisle library program]),
                enable_lisletest="$enableval",
                enable_lisletest="yes")

  # Figure out where the lisle-config script is
  if test x$lisle_config_exec_prefix != x ; then
     lisle_config_args="$lisle_config_args --exec-prefix=$lisle_config_exec_prefix"
     if test x${LISLE_CONFIG+set} != xset ; then
        LISLE_CONFIG=$lisle_config_exec_prefix/bin/lisle-config
     fi
  fi
  if test x$lisle_config_prefix != x ; then
     lisle_config_args="$lisle_config_args --prefix=$lisle_config_prefix"
     if test x${LISLE_CONFIG+set} != xset ; then
        LISLE_CONFIG=$lisle_config_prefix/bin/lisle-config
     fi
  fi
  AC_PATH_PROG(LISLE_CONFIG, lisle-config, no)

  # Make sure we have at least the MINIMUM-VERSION
  min_lisle_version=ifelse([$1], ,1.0.0,[$1])
  AC_MSG_CHECKING(for Lisle - version >= $min_lisle_version)
  no_lisle=""
  if test "$LISLE_CONFIG" = "no" ; then
    no_lisle=yes
  else
    LISLE_CFLAGS=`$LISLE_CONFIG $lisle_config_args --cflags`
    LISLE_LIBS=`$LISLE_CONFIG $lisle_config_args --libs`
    LISLE_LIBS_G=`$LISLE_CONFIG $lisle_config_args --libs-g`
    lisle_config_version=`$LISLE_CONFIG $lisle_config_args --version`
    if test "x$enable_lisletest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $LISLE_CFLAGS"
      CXXFLAGS="$CFLAGS"
      LIBS="$LISLE_LIBS $LIBS"
dnl
dnl Now check if the installed Lisle is sufficiently new.
dnl (Also sanity checks the results of lisle-config to some extent)
dnl
      rm -f conf.lisletest
      AC_TRY_RUN([
#include <lisle/lisle>
#include <lyric/Version>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  Version min_version;
  Version config_version;
  Version library_version;
  String  buf;

  min_version << "$min_lisle_version";
  config_version << "$lisle_config_version";
  library_version = lisle::version();

  system("touch conf.lisletest");

  /* Compare lisle-config output to the version stored in the library */
  if (config_version != library_version)
    {
      buf = "";
      buf << library_version;
      printf("\n*** Lisle library (version %ls) does not match\n", (wchar_t*)buf);
      buf = "";
      buf << config_version;
      printf("*** lisle-config (version %ls)\n", (wchar_t*)buf);
      return 1;
    } 

  /* Test that the library is greater than our minimum version */
  if (library_version >= min_version)
    {
      return 0;
    }
  else
    {
      buf = "";
      buf << library_version;
      printf("\n*** An old version of Lisle (%ls) was found.\n", (wchar_t*)buf);
      buf = "";
      buf << min_version;
      printf("*** You need a version of Lisle newer than %ls. The latest version of\n", (wchar_t*)buf);
      buf = lisle::url();
      printf("*** Lisle is always available from %ls\n", (wchar_t*)buf);
      printf("***\n");
      printf("*** If you have already installed a sufficiently new version, this error\n");
      printf("*** probably means that the wrong copy of the lisle-config shell script is\n");
      printf("*** being found. The easiest way to fix this is to remove the old version\n");
      printf("*** of Lisle, but you can also set the LISLE_CONFIG environment to point to the\n");
      printf("*** correct copy of lisle-config. (In this case, you will probably have to\n");
      printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
      printf("*** so that the correct libraries are found at run-time))\n");
    }
  return 1;
}
      ],, no_lisle=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])   # end of AC_TRY_RUN
      CFLAGS="$ac_save_CFLAGS"
      CXXFLAGS="$ac_save_CXXFLAGS"
      LIBS="$ac_save_LIBS"
    fi
  fi

  if test "x$no_lisle" = x ; then
    AC_MSG_RESULT(yes (version $lisle_config_version, CFLAGS "$LISLE_CFLAGS", LIBS "$LISLE_LIBS"))
    ifelse([$2], , :, [$2])     
  else
    AC_MSG_RESULT(no)
    if test "$LISLE_CONFIG" = "no" ; then
      echo "*** The lisle-config script installed by Lisle could not be found"
      echo "*** If Lisle was installed in PREFIX, make sure PREFIX/bin is in"
      echo "*** your path, or set the LISLE_CONFIG environment variable to the"
      echo "*** full path to lisle-config."
    else
      if test -f conf.lisletest ; then
        :
      else
        echo "*** Could not run Lisle test program, checking why..."
        CFLAGS="$CFLAGS $LISLE_CFLAGS"
        CXXFLAGS="$CFLAGS"
        LIBS="$LIBS $LISLE_LIBS"
        AC_TRY_LINK([
#include <lisle/lisle>
#include <lyric/Version>
#include <stdio.h>
        ],
        [ lisle::version(); return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding Lisle or finding the wrong"
          echo "*** version of Lisle. If it is not finding Lisle, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
          echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means Lisle was incorrectly installed"
          echo "*** or that you have moved Lisle since it was installed. In the latter case, you"
          echo "*** may want to edit the lisle-config script: $LISLE_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi

     LISLE_CFLAGS=""
     LISLE_LIBS=""
     LISLE_LIBS_G=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LISLE_CFLAGS)
  AC_SUBST(LISLE_LIBS)
  AC_SUBST(LISLE_LIBS_G)
  rm -f conf.lisletest
])
