dnl ###########################################################################
dnl HIntLib autoconf macro definitions
dnl ###########################################################################

# HL_CHECK_C_OR_CPP_HEADER(HEADER)
# ------------------------
# Checks if either header cHEADER or HEADER.h exists
AC_DEFUN([HL_CHECK_C_OR_CPP_HEADER],
[hl_found=no
 AC_CHECK_HEADERS([c$1 $1.h],hl_found=yes;break,,[ ])
 if test $hl_found = no; then
   AC_MSG_WARN([Header file missing: We need either c$1 or $1.h!!!])
 fi
])

# HL_UNREACHABLE_CALLS_REMOVED
# ----------------------------
# Check if unreachable calls are removed
AC_DEFUN([HL_UNREACHABLE_CALLS_REMOVED],
[AC_CACHE_CHECK([whether unreachable calls are removed],
hl_cv_unreachable_calls_removed,
[AC_LINK_IFELSE(AC_LANG_PROGRAM([[
#ifdef HAVE_LIMITS
#include <limits>
#endif

void missing();

template<typename T> class Y {};

template<> class Y<int>
{
public:
   static bool f ()  { return true; }
   static const int anInt = 5;
};

template<typename T,unsigned a> class X
{
   public: void test();
};

template<typename T, unsigned a>
void X<T,a>::test()
{
   if (1==2) missing();
   if (a==3) missing();
   if (! Y<T>::f())  missing();
   if (~T() >> Y<T>::anInt == 0) missing();
   if (a % 4 == 1)  missing();
#ifdef HAVE_LIMITS
   if (! std::numeric_limits<T>::is_signed)  missing();
   if (~T() >> (std::numeric_limits<T>::digits - 5) == 0) missing();
#endif
}
]],[[
X<int, 8> x;
x.test();
exit (0);
]]), hl_cv_unreachable_calls_removed=yes, hl_cv_unreachable_calls_removed=no)])

if test x"$hl_cv_unreachable_calls_removed" = xyes; then
   AC_DEFINE([UNREACHABLE_CALLS_REMOVED],1,
      [define if unreachable calls are removed by the optimizer])
fi
])


# HL_WHICH_ABS(FNAMES,TYPE)
# -------------------------
# Tries to use each entry in FNAMES to calculate the absolute value for TYPE.
AC_DEFUN([HL_WHICH_ABS],
[
AC_CACHE_CHECK([which abs() should be used for $2],
translit([hl_cv_abs_for_$2],[ ],[_]),
[
 translit([hl_cv_abs_for_$2_num],[ ],[_])=0
 translit([hl_cv_abs_for_$2],[ ],[_])="???"
 hl_counter=0
 for hl_fname in $1
 do
  hl_counter=`expr ${hl_counter} + 1`
AC_RUN_IFELSE(AC_LANG_PROGRAM([[
#ifdef HAVE_CSTDLIB
  #include <cstdlib>
  #define STD std::
#else
  #include <stdlib.h>
  #define STD
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

int xfloat = 10; int xdouble = 11; int xlong_double = 12;
int xint = 20; int xlong_int = 21; int xlong_long_int = 22;

int detect (float)  { return xfloat; }
int detect (double) { return xdouble; }
int detect (long double) { return xlong_double; }

int detect (int) { return xint; }
int detect (long int) { return xlong_int; }
#ifdef HAVE_LONG_LONG_INT
int detect (long long int) { return xlong_long_int; }
#endif

]],[[
   if (detect (${hl_fname} (static_cast<$2> (-1))) != translit([x$2],[ ],[_]))
      STD exit (1);
   if (${hl_fname} (static_cast<$2>(-17001) / static_cast<$2>(1000)) !=
                static_cast<$2>( 17001) / static_cast<$2>(1000))
      STD exit (1);
   STD exit (0);
]]),
[
translit([hl_cv_abs_for_$2],[ ],[_])=${hl_fname}
translit([hl_cv_abs_for_$2_num],[ ],[_])=${hl_counter}
break
],,
AC_MSG_ERROR([HL_ WHICH_ABS cannot be used when crosscompiling!]))
done
])
if test translit([$hl_cv_abs_for_$2_num],[ ],[_]) -eq 0 ; then
AC_MSG_ERROR([No appropriate function found!])
fi
AC_DEFINE_UNQUOTED(translit([ABS_FOR_$2],[ a-z],[_A-Z]),translit([$hl_cv_abs_for_$2],[ ],[_]),
   [function to use for abs($2)])
])dnl AC_DEFUN


# HL_WHICH_MATH_FUN(FUNCTION,FNAMES,TYPE)
# ---------------------------------------
# Tries to use each entry in FNAMES to calculate FUNCTION for TYPE.
AC_DEFUN([HL_WHICH_MATH_FUN],
[
AC_CACHE_CHECK([which function should be used for $1($3)],
translit([hl_cv_$1_for_$3],[ ],[_]),
[
 translit([hl_cv_$1_for_$3_num],[ ],[_])=0
 translit([hl_cv_$1_for_$3],[ ],[_])="???"
 hl_counter=0
 for hl_fname in $2
 do
  hl_counter=`expr ${hl_counter} + 1`
AC_RUN_IFELSE(AC_LANG_PROGRAM([[
#ifdef HAVE_CSTDLIB
  #include <cstdlib>
  #define STD std::
#else
  #include <stdlib.h>
  #define STD
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

int xfloat = 10; int xdouble = 11; int xlong_double = 12;
int xint = 20; int xlong_int = 21; int xlong_long_int = 22;

int detect (float)  { return xfloat; }
int detect (double) { return xdouble; }
int detect (long double) { return xlong_double; }

int detect (int) { return xint; }
int detect (long int) { return xlong_int; }
#ifdef HAVE_LONG_LONG_INT
int detect (long long int) { return xlong_long_int; }
#endif

]],[[
   if (detect (${hl_fname} (static_cast<$3> (-1))) != translit([x$3],[ ],[_]))
      STD exit (1);
   STD exit (0);
]]),
[
translit([hl_cv_$1_for_$3],[ ],[_])=${hl_fname}
translit([hl_cv_$1_for_$3_num],[ ],[_])=${hl_counter}
break
],,
AC_MSG_ERROR([HL_ WHICH_ABS cannot be used when crosscompiling!]))
done
])
if test translit([$hl_cv_$1_for_$3_num],[ ],[_]) -eq 0 ; then
AC_MSG_ERROR([No appropriate function found!])
fi
AC_DEFINE_UNQUOTED(translit([$1_FOR_$3],[ a-z],[_A-Z]),translit([$hl_cv_$1_for_$3],[ ],[_]),
   [function to use for $1($3)])
AC_DEFINE_UNQUOTED(translit([$1_FOR_$3_NUM],[ a-z],[_A-Z]),translit([$hl_cv_$1_for_$3_num],[ ],[_]),
   [function number to use for $1($3)])
])dnl AC_DEFUN


# HL_FLT_ROUNDS_CONST
# -------------------
# Check if FLT_ROUNDS is constant
AC_DEFUN([HL_FLT_ROUNDS_CONST],
[AC_CACHE_CHECK([whether FLT_ROUNDS is constant], hl_cv_flt_rounds_const,
[AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[
#include <float.h>
]],[[
#if FLT_ROUNDS < 7
  ;
#endif
  ;
]]),
hl_cv_flt_rounds_const=yes, hl_cv_flt_rounds_const=no)])

if test x"$hl_cv_flt_rounds_const" = xyes; then
   AC_DEFINE([FLT_ROUNDS_CONST],1,[define if FLT_ROUNDS is constant])
fi
])


# HL_EQUAL_BUG
# ------------
# Check whether the Standard Template Library has the equal()-bug
AC_DEFUN([HL_EQUAL_BUG],
[AC_CACHE_CHECK([whether the STL has the equal()-bug], hl_cv_equal_bug,
[AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[
#include <vector>
struct X { int a; X() : a(7) {} };
inline bool operator==(const X& x1, const X& x2)  { return x1.a == x2.a; }
]],[[
std::vector<X> v1, v2;
v1 == v2;
]]),
hl_cv_equal_bug=no, hl_cv_equal_bug=yes)])

if test x"$hl_cv_equal_bug" = xyes; then
   AC_DEFINE([EQUAL_BUG],1,[define if STL has the equal()-bug])
fi
])


# HL_COMPLEX_POW_BUG
# ------------------
# Check whether std::pow(std::complex<long double>,int) is broken
AC_DEFUN([HL_COMPLEX_POW_BUG],
[AC_CACHE_CHECK([whether std::pow(std::complex<long double>,int) is broken],
hl_cv_complex_pow_bug,
[AC_RUN_IFELSE(AC_LANG_PROGRAM([[
#ifdef HAVE_CSTDLIB
  #include <cstdlib>
  #define STD std::
#else
  #include <stdlib.h>
  #define STD
#endif
#include <complex>
]],[[
std::complex<long double> x (0, 1);
if (std::abs(std::pow (x, 1)) < .5)  STD exit (1);
]]),
hl_cv_complex_pow_bug=no, hl_cv_complex_pow_bug=yes)])

if test x"$hl_cv_complex_pow_bug" = xyes; then
   AC_DEFINE([COMPLEX_POW_BUG],1,[define whether std::pow(std::complex<long double>,int) is broken])
fi
])


# HL_IEEE_MAGIC_WORKS
# -------------------
# Check if we can use IEEE Magic
AC_DEFUN([HL_IEEE_MAGIC_WORKS],
[AC_CACHE_CHECK([whether IEEE magic works], hl_cv_ieee_magic_works,
[AC_RUN_IFELSE(AC_LANG_PROGRAM([[
#include <stdlib.h>
#if SIZEOF_UNSIGNED_INT >= 4
   #undef UNSIGNED_NOT_EQUAL_U32
   typedef unsigned u32;
#else
#if SIZEOF_UNSIGNED_LONG_INT >= 4
   #define UNSIGNED_NOT_EQUAL_U32 1
   typedef unsigned long u32;
#else
   error "unsigned long int does not have 32 bits!"
#endif
#endif

#if SIZEOF_UNSIGNED_INT >= 8
   #undef U32_NOT_EQUAL_U64
   typedef unsigned u64;
#else
#if SIZEOF_UNSIGNED_LONG_INT >= 8
   #ifdef UNSIGNED_NOT_EQUAL_U32
      #undef U32_NOT_EQUAL_U64
   #else
      #define U32_NOT_EQUAL_U64
   #endif
   typedef unsigned long u64;
#else
#ifdef HAVE_UNSIGNED_LONG_LONG_INT
   #if SIZEOF_UNSIGNED_LONG_LONG_INT >= 8
      #define U32_NOT_EQUAL_U64 1
      typedef unsigned long long u64;
   #else
      #error "Cannot determine an unsigned integer type with at least 64 bits!"
   #endif
#else
   #error "Cannot determine an unsigned integer type with at least 64 bits!"
#endif
#endif
#endif
]],[[
   union { double d; u64 i; } x64;

   x64.d = 1.0; if (x64.d != 1.0)  exit (1);
   x64.i |= (1ull << 51); if (x64.d != 1.5)   exit (1);
   x64.i |= (1ull << 50); if (x64.d != 1.75)  exit (1);
   x64.i ^= (1ull << 51); if (x64.d != 1.25)  exit (1);
   
   union { float d; u32 i; } x32;

   x32.d = 1.0f; if (x32.d != 1.0f)  exit (1);
   x32.i |= (1ul << 22); if (x32.d != 1.5f)   exit (1);
   x32.i |= (1ul << 21); if (x32.d != 1.75f)  exit (1);
   x32.i ^= (1ul << 22); if (x32.d != 1.25f)  exit (1);
   
   exit (0);
]]),
hl_cv_ieee_magic_works=yes, hl_cv_ieee_magic_works=no,
hl_cv_ieee_magic_works=no)])

if test x"$hl_cv_ieee_magic_works" = xyes; then
   AC_DEFINE([IEEE_MAGIC_WORKS],1,[define if IEEE magic works])
fi
])


# HL_STREAMS_SUPPORT_LOCAL
# ------------------------
# Check if streams support local
AC_DEFUN([HL_STREAMS_SUPPORT_LOCAL],
[
AC_CACHE_CHECK([whether streams support local], hl_cv_streams_support_local,
[AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[
#include <iostream>
]],[[
   std::cin.imbue (std::cout.getloc());
]]),
hl_cv_streams_support_local=yes, hl_cv_streams_support_local=no)])

if test x"$hl_cv_streams_support_local" = xyes; then
   AC_DEFINE(STREAMS_SUPPORT_LOCAL,1,[define if streams support local])
fi
])

# HL_NAMEPSPACE_REL_OPS
# ---------------------
# Check if the namespace std::rel_ops exists
AC_DEFUN([HL_NAMESPACE_REL_OPS],
[
AC_CACHE_CHECK([whether the namespace std::rel_ops exists],
hl_cv_namespace_rel_ops,
[AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[
#include <utility>
using namespace std::rel_ops;      
struct A { A (int aa) : a(aa) {} int a;};
bool operator==(const A& x1, const A& x2)  { return x1.a == x2.a; }
]],[[
   A s1 (3);
   A s2 (4);
   s1 != s2;
]]),
hl_cv_namespace_rel_ops=yes, hl_cv_namespace_rel_ops=no)])

if test x"$hl_cv_namespace_rel_ops" = xyes; then
   AC_DEFINE(NAMESPACE_REL_OPS,1,[define if namespace std::rel_ops exists])
fi
])

# HL_PROG_F77_REALLY_WORKS
# ------------------------
#
# Deterimes if the Fortran 77 compile found by AC_PROG_F77 can comile a
# program without errors.
AC_DEFUN([HL_PROG_F77_REALLY_WORKS],
[
  AC_REQUIRE([AC_PROG_F77])dnl
  AC_CACHE_CHECK(
    [whether ${F77} really works], hl_cv_prog_f77_really_works,
    [
      AC_LANG_PUSH([Fortran 77])
      AC_COMPILE_IFELSE(AC_LANG_PROGRAM(,
        [
          INTEGER I
          I = 1
        ]),
        hl_cv_prog_f77_really_works=yes,
        hl_cv_prog_f77_really_works=no)
      AC_LANG_POP(Fortran 77)
    ])
])

dnl End of acinclude.m4

