#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT(ROSV_Control, 1.0, thomasburns88@gmail.com)
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile
        src/Makefile])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([strchr])

AC_OUTPUT

