## -*- M4 -*-
#
# Check for threads support and make appropriate settings


# _AC_CHECK_THREADS_POSIX_SIGNALS([ACTION-IF-TRUE], [ACTION-IF-FALSE])
# Check for available signals to use for thread cancel and suspend-resume
AC_DEFUN([_AC_CHECK_THREADS_POSIX_SIGNALS],
[
  AC_MSG_CHECKING([for available signals to restart and suspend threads])
  ac_save_LDFLAGS="$LDFLAGS"
  LDFLAGS="$LDFLAGS $THREAD_LIBS"
  AC_TRY_RUN(
    [
      /*
       * Find 2 available signal numbers for Lisle in the 32..NSIG range.
       * The 2 signals will be used for:
       *   1) thread restart
       *   2) thread suspend
       */

      #include <csignal>
      #include <cstdio>

      void handler (int signal)
      {
      }

      int in_rt_signals (unsigned signals[2])
      {
        unsigned         sig;
        bool             available[NSIG];
        struct sigaction new_action, old_action;
        unsigned         n_available_signals, used;

        for (sig=0; sig<NSIG; sig++)
          available[sig] = false;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;
        new_action.sa_handler = handler;
        for (sig=32; sig<NSIG; sig++)
          if (sigaction(sig, &new_action, &old_action) == 0)
            {
              available[sig] = true;
              sigaction(sig, &old_action, NULL);
            }
          else
            available[sig] = false;
        n_available_signals = 0;
        for (sig=0; sig<NSIG; sig++)
          if (available[sig])
            n_available_signals++;
        if (n_available_signals >= 2)
          {
            used = 0;
            for (sig=0; sig<NSIG && used<2; sig++)
              if (available[sig])
                {
                  signals[used] = sig;
                  used++;
                }
          }
        else
          return 0;
        return 1;
      }

      int main ()
      {
        unsigned signals[2];
        FILE*    out;

	if (!in_rt_signals(signals))
          {
            signals[0] = SIGUSR1;
            signals[1] = SIGUSR2;
          }
        out = fopen("signal.conf", "w");
        printf("%u ", signals[0]);
        fprintf(out, "%u\n", signals[0]);
        printf("%u ", signals[1]);
        fprintf(out, "%u\n", signals[1]);
        fclose(out);
        return 0;
      }
    ],
    [
      AC_MSG_RESULT([ok])
      $1
      rm -f signal.conf
    ],
    [
      AC_MSG_RESULT([none]);
      $2
    ])
  LDFLAGS="$ac_save_LDFLAGS"
])# _AC_CHECK_THREADS_POSIX_SIGNALS


# _AC_CHECK_THREADS_POSIX_TYPED_MUTEXES([ACTION-IF-TRUE], [ACTION-IF-FALSE])
# Check for typed mutex (Darwin, aka Mac OS X, lacks them)
AC_DEFUN([_AC_CHECK_THREADS_POSIX_TYPED_MUTEXES],
[
  AC_MSG_CHECKING([for typed mutexes])
  ac_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $THREAD_CFLAGS"
  AC_TRY_COMPILE(
    [
      #include <pthread.h>
    ],
    [
      pthread_mutexattr_t attr;
      pthread_mutexattr_settype(&attr, 0);
    ],
    [
      AC_MSG_RESULT([yes])
      $1
    ],
    [
      AC_MSG_RESULT([no])
      $2
    ]
  )
  CFLAGS="$ac_save_CFLAGS"
])# _AC_CHECK_THREADS_POSIX_TYPED_MUTEXES


# _AC_CHECK_THREADS_POSIX_KILL([ACTION-IF-TRUE], [ACTION-IF-FALSE])
# Check for the pthread_kill function
AC_DEFUN([_AC_CHECK_THREADS_POSIX_KILL],
[
  AC_MSG_CHECKING([for pthread_kill function in pthread library])
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $THREAD_CFLAGS"
  LIBS="$LIBS $THREAD_LIBS"
  AC_TRY_LINK(
    [
      #include <signal.h>
      #include <pthread.h>
    ],
    [
      pthread_kill(pthread_self(), SIGKILL);
    ],
    [
      AC_MSG_RESULT([yes])
      $1
    ],
    [
      AC_MSG_RESULT([no])
      $2
    ]
  )
  LIBS="$ac_save_LIBS"
  CFLAGS="$ac_save_CFLAGS"
])# _AC_CHECK_THREADS_POSIX_KILL


# _AC_CHECK_THREADS_POSIX([ACTION-IF-FALSE])
# Check for POSIX threads
AC_DEFUN([_AC_CHECK_THREADS_POSIX],
[
  AC_MSG_NOTICE([checking for POSIX threads])
  AC_CHECK_HEADER(pthread.h,
    [
      AC_MSG_NOTICE([found POSIX threads])
      AC_MSG_NOTICE([setting up lisle to use POSIX threads])
      THREAD_CFLAGS=""
      THREAD_LIBS=""
      TS_NAME="posix"
      TS_TMUX="posix"
      TS_NTID="posix"
      case $host_os in
        linux* )
          THREAD_CFLAGS="-D_REENTRANT -D_XOPEN_SOURCE=600"
          THREAD_LIBS="-lpthread"
          ;;
        solaris* )
          THREAD_CFLAGS="-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS"
          THREAD_LIBS="-lrt -lpthread"
          ;;
        freebsd* | \
        openbsd* )
          THREAD_CFLAGS="-D_REENTRANT"
          THREAD_LIBS="-pthread"
          ;;
        darwin* )
          THREAD_CFLAGS="-D_REENTRANT"
          THREAD_LIBS="-lpthread"
          TS_NTID="darwin"
          _AC_CHECK_THREADS_POSIX_KILL([],
            [
              AC_MSG_NOTICE([upgrade to Mac OS X 10.2, aka Jaguar, to get pthread_kill().])
              $1
            ]
          )
          _AC_CHECK_THREADS_POSIX_TYPED_MUTEXES([TS_TMUX="posix"], [TS_TMUX="darwin"])
          ;;
        * )
          THREAD_CFLAGS=""
          THREAD_LIBS=""
          ;;
      esac
      _AC_CHECK_THREADS_POSIX_SIGNALS(
        [
          AC_DEFINE_UNQUOTED(LISLE_SIG_RESTART, `tail -2l signal.conf | head -n 1`,
                             "Signal used to restart a thread")
          AC_DEFINE_UNQUOTED(LISLE_SIG_SUSPEND, `tail -1l signal.conf | head -n 1`,
                             "Signal used to suspend a thread")
        ],
        [$1])
      AC_SUBST([TS_NAME], [$TS_NAME])
      AC_SUBST([TS_TMUX], [$TS_TMUX])
      AC_SUBST([TS_NTID], [$TS_NTID])
      AC_SUBST([THREAD_CFLAGS], [$THREAD_CFLAGS])
      AC_SUBST([THREAD_LIBS], [$THREAD_LIBS])
    ],
    [$1])
])


# AC_CHECK_THREADS()
# Check for threads:
#   1. Check if system supports threads
#   2. If 1. = yes, check what kind of threads (Posix)
AC_DEFUN([AC_CHECK_THREADS],
[
  AC_MSG_NOTICE([checking for threads])
  _AC_CHECK_THREADS_POSIX(
    [
      AC_MSG_ERROR([no or uncomplete threads system found.
                  Lisle can not be built on systems without threads.
                  If you are constucting lyric, you can disable the building
                  of lisle by adding '--disable-lisle' to your configuration
                  options.])
    ]
  )
])
