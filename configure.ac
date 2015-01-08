AC_PREREQ([2.64])
AC_INIT([DuelJam], [1.0], [keigen.shu@gmail.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])

LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile
                 src/libawe/Makefile src/kiss_fft130/Makefile
                 src/Makefile
                 ])

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CC_C_O

AX_CXX_COMPILE_STDCXX_11(noext,mandatory)

####    LIBARIES
have_clanlib=no
have_portaudio=no
have_sndfile=no
have_soxr=no

PKG_PROG_PKG_CONFIG()
PKG_CHECK_MODULES(ClanLib, [clanApp-4.0,clanCore-4.0,clanDisplay-4.0,clanGL-4.0,clanNetwork-4.0,clanUI-4.0], [have_clanlib=yes])

AC_SEARCH_LIBS([Pa_Initialize], [portaudio], [have_portaudio=yes])
AC_SEARCH_LIBS([sf_open], [sndfile], [have_sndfile=yes])
AC_SEARCH_LIBS([soxr_create], [soxr], [have_soxr=yes])

if test "x${have_clanlib}" = xno; then
    AC_MSG_ERROR([Unable to find ClanLib 4.0 in the system.])
fi

if test "x${have_portaudio}" = xno; then
    AC_MSG_ERROR([Unable to find PortAudio in the system.])
fi

if test "x${have_sndfile}" = xno; then
    AC_MSG_ERROR([Unable to find sndfile in the system.])
fi

if test "x${have_soxr}" = xno; then
    AC_MSG_ERROR([
        Unable to find soxr in the system.
        Check out the README in src/soxr for instructions to build and install
        it on your system.
    ])
fi

####    OUTPUT

echo "
================================================
  :: $PACKAGE_NAME :: version $PACKAGE_VERSION
================================================
  Compiler Command..: $CXX $CXXFLAGS $CPPFLAGS
  Linker Command....: $LD $LDFLAGS $LIBS
  Install Prefix....: $prefix
================================================
"

AC_OUTPUT
