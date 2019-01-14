// BMDX library 1.1 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2019-01-13(2)
//
// Copyright 2004-2018 Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine)
// Contacts: bmdx-dev [at] mail [dot] ru, z7d9 [at] yahoo [dot] com
// Project website: hashx.dp.ua
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the Software or the use or other dealings in the Software.
// https://opensource.org/licenses/MIT

// bmdx_cpiomt.h: cross-platform tools for i/o and multithreading (standalone header).

// Features:
//
//    Automatic definition for 32, 64-bit signed integers.    (bmdx_meta::  s_long, u_long, s_ll, u_ll)
//    Read/write data words between integer variable and memory.    (bmdx_str::words::  le2 le4 le8 .. be8, set_le2 .. set_be8)
//    Conversions between strings and numbers. Fixed-length string.    (bmdx_str::conv::  str_from_*, ntocs, ntows, str2i, str2f; bmdx_str::  flstr_t)
//
//    Timer with near-microseconds resolution.    (bmdx::  clock_ms)
//    Sleep with near-microseconds resolution.    (bmdx::  sleep_mcs)
//    Recursive mutex.    (bmdx::  struct critsec_t)
//    Unified thread control. Priorities, high-level data passing to thread etc.    (bmdx::  struct threadctl)
//    Parallel task processing helper.     (bmdx::  struct multithread)
//    Simple process launcher. Execute, wait, check running state.    (bmdx::  struct processctl)
//    Const. reference object with threadsafe setting and copying, + unsafe non-const access.    (bmdx::  struct cref_t)
//
//    Basic unbuffered console i/o.    (bmdx::  struct console_io)
//    Bytewise file i/o + save/load whole file as string.    (bmdx::  struct file_io)
//
//    Experimental features: shared memory object, global critical section, non-blocking queue of bytes,
//      IPC queue of string messages.     (bmdx_shm::  shmobj2s_t, critsec_gn, rfifo_nbl11, shmfifo_s)

#ifndef bmdx_cpiomt_H
#define bmdx_cpiomt_H

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
  #pragma GCC diagnostic ignored "-Wdeprecated"
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
#endif
#ifdef _MSC_VER
  #pragma warning(disable:4290)
  #pragma warning(disable:4099)
#endif

  // Rough platform selector
#ifdef _WIN32
  #define _bmdxpl_Wnds
#else
  #define _bmdxpl_Psx
#endif

#undef _yk_reg
#if __cplusplus > 199711L
  #define _yk_reg
#else
  #define _yk_reg register
#endif

#if __APPLE__ && __MACH__
  #define __bmdx_noarg , bmdx_meta::t_noarg = bmdx_meta::t_noarg()
  #define __bmdx_noarg1 bmdx_meta::t_noarg = bmdx_meta::t_noarg()
  #define __bmdx_noargt , bmdx_meta::t_noarg
  #define __bmdx_noargt1 bmdx_meta::t_noarg
  #define __bmdx_noargv , bmdx_meta::t_noarg()
  #define __bmdx_noargv1 bmdx_meta::t_noarg()
#else
  #define __bmdx_noarg
  #define __bmdx_noarg1
  #define __bmdx_noargt
  #define __bmdx_noargt1
  #define __bmdx_noargv
  #define __bmdx_noargv1
#endif




namespace yk_c { namespace { struct __vecm_tu_selector; } }
namespace bmdx_meta
{
  struct nothing {};

  template<class T, class _ = yk_c::__vecm_tu_selector> struct noarg_tu_t {};
  typedef noarg_tu_t<nothing> t_noarg;

  template<bool cond, class T1, class T2> struct if_t { typedef T1 result; }; template<class T1, class T2> struct if_t<false, T1, T2> { typedef T2 result; };
  template <int n, class t1=nothing, class t2=nothing, class t3=nothing, class t4=nothing, class t5=nothing> struct find_size_n { typedef typename if_t<sizeof(t1) == n, t1, typename find_size_n<n, t2, t3, t4, nothing>::result>::result result; };
  template <int n> struct find_size_n<n> { typedef nothing result; };

    // 32-bit, 64-bit integers.
  typedef bmdx_meta::find_size_n<4, signed long int, signed int, signed short>::result s_long;
  typedef bmdx_meta::find_size_n<4, unsigned long int, unsigned int, unsigned short>::result u_long;
  typedef bmdx_meta::find_size_n<8, signed long long int, signed long int, signed int>::result s_ll;
  typedef bmdx_meta::find_size_n<8, unsigned long long int, unsigned long int, unsigned int>::result u_ll;

}

#undef _s_long
#undef _s_ll
#undef _u_ll
#ifdef yk_c_vecm_hashx_H
  #define _s_long ::yk_c::s_long
  #define _s_ll ::yk_c::meta::s_ll
#else
  #define _s_long ::bmdx_meta::s_long
  #define _s_ll ::bmdx_meta::s_ll
#endif
#define _u_ll ::bmdx_meta::u_ll



#include <new>
#include <typeinfo>
#include <string>
#include <ostream>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>

#if (defined(_MSC_VER) && _MSC_VER < 1800)
  #include <float.h>
  #define __bmdx_isfinite _finite
#elif defined(__BORLANDC__)
  namespace bmdx_str { namespace conv { static inline bool __bmdx_isfinite_ff(double x) { return (x == x) && x != std::numeric_limits<double>::infinity() && x != -std::numeric_limits<double>::infinity(); } }  }
  #define __bmdx_isfinite bmdx_str::conv::__bmdx_isfinite_ff
#elif defined(__SUNPRO_CC)
  #include <ieeefp.h>
  #define __bmdx_isfinite finite
#else
  #define __bmdx_isfinite std::isfinite
#endif

namespace bmdx_str
{
  namespace words
  {
      // NOTE pos is the offset in bytes from p.
    static inline _s_ll le8(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_ll x = _s_ll(*pc++); x |= _s_ll(*pc++) << 8; x |= _s_ll(*pc++) << 16; x |= _s_ll(*pc++) << 24; x |= _s_ll(*pc++) << 32; x |= _s_ll(*pc++) << 40; x |= _s_ll(*pc++) << 48; x |= _s_ll(*pc) << 56; return x; }
    static inline _s_long le4(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc++) << 8; x |= _s_long(*pc++) << 16; x |= _s_long(*pc) << 24; return x; }
    static inline _s_long le2(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc) << 8; return x; }
    static inline void set_le8(void* p, _s_long pos, _s_ll x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_le4(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_le2(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc = char(x);  }

      // NOTE pos is the offset in bytes from p.
    static inline _s_ll be8(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 7; _s_ll x = _s_ll(*pc--); x |= _s_ll(*pc--) << 8; x |= _s_ll(*pc--) << 16; x |= _s_ll(*pc--) << 24; x |= _s_ll(*pc--) << 32; x |= _s_ll(*pc--) << 40; x |= _s_ll(*pc--) << 48; x |= _s_ll(*pc) << 56; return x; }
    static inline _s_long be4(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 3; _s_long x = _s_long(*pc--); x |= _s_long(*pc--) << 8; x |= _s_long(*pc--) << 16; x |= _s_long(*pc) << 24; return x; }
    static inline _s_long be2(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 1; _s_long x = _s_long(*pc--); x |= _s_long(*pc) << 8; return x; }
    static inline void set_be8(void* p, _s_long pos, _s_ll x) throw() { _yk_reg char* pc = ((char*)p) + pos + 7; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_be4(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos + 3; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_be2(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos + 1; *pc-- = char(x); x >>= 8; *pc = char(x);  }

    template<class T> static inline void swap_bytes(T& a, T& b) { enum { n = sizeof(T)  }; if (!n || &a == &b) { return; } _s_ll c[(n + 7) / 8]; std::memcpy(&c[0], &a, n); std::memcpy(&a, &b, n); std::memcpy(&b, &c[0], n); }
  }

  namespace conv
  {
    struct exc_str2i : std::exception { const char* what() const throw() { return "bmdx_str::str2i"; } };
    struct exc_str2f : std::exception { const char* what() const throw() { return "bmdx_str::str2f"; } };

    template<class _ = yk_c::__vecm_tu_selector>
    struct _bmdx_str_impl
    {
      static _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) throw();
      static _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax, bool b_nans) throw();
      static _s_ll str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans);
      static _s_ll str2i(const char* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const char* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans);
    };

    static inline bool is_finite(double x)
      { return __bmdx_isfinite(x); }

    template<class _>
    _s_long _bmdx_str_impl<_>::str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) throw()
    {
      if (!(buf && nchars >= 0)) { return -1; }
      if (nchars == 0) { return 0; }
      bool b_neg = b_signed && x < 0;
      _s_long n = 0;
      if (b_signed) { _s_ll x2 = x; do { n += 1; x2 /= 10; } while (x2); if (b_neg) { n += 1; } }
        else { _u_ll x2 = x; do { n += 1; x2 /= 10; } while (x2);  }
      if (n > nchars) { if (x > 0) { buf[0] = '+'; } else if (x < 0) { buf[0] = b_signed ? '-' : '+'; } else { buf[0] = '0'; } return 1; }
      if (b_neg) { buf[0] = '-'; }
      _s_long pos = n;
      if (b_signed) { _s_ll x2 = x; do { _s_long d = x2 % 10; if (d < 0) { d = -d; } d += '0'; buf[--pos] = char(d); x2 /= 10; } while (x2); }
        else { _u_ll x2 = x; do { _s_long d = x2 % 10; d += '0'; buf[--pos] = char(d); x2 /= 10; } while (x2); }
      return n;
    }

    template<class _>
    _s_long _bmdx_str_impl<_>::str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax, bool b_nans) throw()
    {
      if (!(buf && nchars >= 0)) { return -1; }
      if (nchars == 0) { return 0; }
      const bool b_fin = is_finite(x);
      if (!b_fin)
      {
        if (!b_nans) { return -1; }
        if (!(x == x)) { buf[0] = 'n'; if (nchars < 3) { return 1; } buf[1] = 'a'; buf[2] = 'n'; return 3; }
      }
      if (nchars == 1) { buf[0] = x > 0. ? '+' : (x < 0. ? '-' : '0'); return 1; }
      if (!b_fin)
      {
        buf[0] = x == std::numeric_limits<double>::infinity() ? '+' : (-x == std::numeric_limits<double>::infinity() ? '-' : '0');
        buf[1] = 'i';  if (nchars < 4) { return 2; } buf[2] = 'n'; buf[3] = 'f'; return 4;
      }
      const int ndm_mm = 14;
      const int nfrac_mm = 13;
      static const _s_ll _pow10[ndm_mm + 1] =
        { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000,
          100000000000, 1000000000000, 10000000000000, 100000000000000 };
      if (ndmmax < 1) { ndmmax = 1; } else if (ndmmax > ndm_mm) { ndmmax = ndm_mm; }
      if (nfracmax < 1) { nfracmax = 1; } else if (nfracmax > nfrac_mm) { nfracmax = nfrac_mm; }
      #if defined(__ICC) || defined(__INTEL_COMPILER)
        _s_long n_neg = _s_long(std::signbit(x));
      #else
        _s_long n_neg = _s_long(x < 0.);
      #endif
      double x2 = n_neg ? -x : x;
      _s_long order = 0;
      static const double cmp1[310] = { 1e0,1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16,1e17,1e18,1e19,1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27,1e28,1e29,1e30,1e31,1e32,1e33,1e34,1e35,1e36,1e37,1e38,1e39,1e40,1e41,1e42,1e43,1e44,1e45,1e46,1e47,1e48,1e49,1e50,1e51,1e52,1e53,1e54,1e55,1e56,1e57,1e58,1e59,1e60,1e61,1e62,1e63,1e64,1e65,1e66,1e67,1e68,1e69,1e70,1e71,1e72,1e73,1e74,1e75,1e76,1e77,1e78,1e79,1e80,1e81,1e82,1e83,1e84,1e85,1e86,1e87,1e88,1e89,1e90,1e91,1e92,1e93,1e94,1e95,1e96,1e97,1e98,1e99,1e100,1e101,1e102,1e103,1e104,1e105,1e106,1e107,1e108,1e109,1e110,1e111,1e112,1e113,1e114,1e115,1e116,1e117,1e118,1e119,1e120,1e121,1e122,1e123,1e124,1e125,1e126,1e127,1e128,1e129,1e130,1e131,1e132,1e133,1e134,1e135,1e136,1e137,1e138,1e139,1e140,1e141,1e142,1e143,1e144,1e145,1e146,1e147,1e148,1e149,1e150,1e151,1e152,1e153,1e154,1e155,1e156,1e157,1e158,1e159,1e160,1e161,1e162,1e163,1e164,1e165,1e166,1e167,1e168,1e169,1e170,1e171,1e172,1e173,1e174,1e175,1e176,1e177,1e178,1e179,1e180,1e181,1e182,1e183,1e184,1e185,1e186,1e187,1e188,1e189,1e190,1e191,1e192,1e193,1e194,1e195,1e196,1e197,1e198,1e199,1e200,1e201,1e202,1e203,1e204,1e205,1e206,1e207,1e208,1e209,1e210,1e211,1e212,1e213,1e214,1e215,1e216,1e217,1e218,1e219,1e220,1e221,1e222,1e223,1e224,1e225,1e226,1e227,1e228,1e229,1e230,1e231,1e232,1e233,1e234,1e235,1e236,1e237,1e238,1e239,1e240,1e241,1e242,1e243,1e244,1e245,1e246,1e247,1e248,1e249,1e250,1e251,1e252,1e253,1e254,1e255,1e256,1e257,1e258,1e259,1e260,1e261,1e262,1e263,1e264,1e265,1e266,1e267,1e268,1e269,1e270,1e271,1e272,1e273,1e274,1e275,1e276,1e277,1e278,1e279,1e280,1e281,1e282,1e283,1e284,1e285,1e286,1e287,1e288,1e289,1e290,1e291,1e292,1e293,1e294,1e295,1e296,1e297,1e298,1e299,1e300,1e301,1e302,1e303,1e304,1e305,1e306,1e307,1e308,1.7976e308 };
      static const double cmp2[309] = { 2.225e-308,1e-307,1e-306,1e-305,1e-304,1e-303,1e-302,1e-301,1e-300,1e-299,1e-298,1e-297,1e-296,1e-295,1e-294,1e-293,1e-292,1e-291,1e-290,1e-289,1e-288,1e-287,1e-286,1e-285,1e-284,1e-283,1e-282,1e-281,1e-280,1e-279,1e-278,1e-277,1e-276,1e-275,1e-274,1e-273,1e-272,1e-271,1e-270,1e-269,1e-268,1e-267,1e-266,1e-265,1e-264,1e-263,1e-262,1e-261,1e-260,1e-259,1e-258,1e-257,1e-256,1e-255,1e-254,1e-253,1e-252,1e-251,1e-250,1e-249,1e-248,1e-247,1e-246,1e-245,1e-244,1e-243,1e-242,1e-241,1e-240,1e-239,1e-238,1e-237,1e-236,1e-235,1e-234,1e-233,1e-232,1e-231,1e-230,1e-229,1e-228,1e-227,1e-226,1e-225,1e-224,1e-223,1e-222,1e-221,1e-220,1e-219,1e-218,1e-217,1e-216,1e-215,1e-214,1e-213,1e-212,1e-211,1e-210,1e-209,1e-208,1e-207,1e-206,1e-205,1e-204,1e-203,1e-202,1e-201,1e-200,1e-199,1e-198,1e-197,1e-196,1e-195,1e-194,1e-193,1e-192,1e-191,1e-190,1e-189,1e-188,1e-187,1e-186,1e-185,1e-184,1e-183,1e-182,1e-181,1e-180,1e-179,1e-178,1e-177,1e-176,1e-175,1e-174,1e-173,1e-172,1e-171,1e-170,1e-169,1e-168,1e-167,1e-166,1e-165,1e-164,1e-163,1e-162,1e-161,1e-160,1e-159,1e-158,1e-157,1e-156,1e-155,1e-154,1e-153,1e-152,1e-151,1e-150,1e-149,1e-148,1e-147,1e-146,1e-145,1e-144,1e-143,1e-142,1e-141,1e-140,1e-139,1e-138,1e-137,1e-136,1e-135,1e-134,1e-133,1e-132,1e-131,1e-130,1e-129,1e-128,1e-127,1e-126,1e-125,1e-124,1e-123,1e-122,1e-121,1e-120,1e-119,1e-118,1e-117,1e-116,1e-115,1e-114,1e-113,1e-112,1e-111,1e-110,1e-109,1e-108,1e-107,1e-106,1e-105,1e-104,1e-103,1e-102,1e-101,1e-100,1e-99,1e-98,1e-97,1e-96,1e-95,1e-94,1e-93,1e-92,1e-91,1e-90,1e-89,1e-88,1e-87,1e-86,1e-85,1e-84,1e-83,1e-82,1e-81,1e-80,1e-79,1e-78,1e-77,1e-76,1e-75,1e-74,1e-73,1e-72,1e-71,1e-70,1e-69,1e-68,1e-67,1e-66,1e-65,1e-64,1e-63,1e-62,1e-61,1e-60,1e-59,1e-58,1e-57,1e-56,1e-55,1e-54,1e-53,1e-52,1e-51,1e-50,1e-49,1e-48,1e-47,1e-46,1e-45,1e-44,1e-43,1e-42,1e-41,1e-40,1e-39,1e-38,1e-37,1e-36,1e-35,1e-34,1e-33,1e-32,1e-31,1e-30,1e-29,1e-28,1e-27,1e-26,1e-25,1e-24,1e-23,1e-22,1e-21,1e-20,1e-19,1e-18,1e-17,1e-16,1e-15,1e-14,1e-13,1e-12,1e-11,1e-10,1e-9,1e-8,1e-7,1e-6,1e-5,1e-4,1e-3,1e-2,1e-1,1 };
      if (x2 >= 1.e6)
      {
        if (x2 >= 1.79e308) { x2 = 1.79e308; }
        _s_long n0 = 0; _s_long n2 = 310; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp1[n]) { n2 = n; } else if (x2 >= cmp1[n]) { n0 = n; } }
        order = n0; x2 /= cmp1[n0];
      }
      else if (x2 < 0.001)
      {
        if (x2 < 2.23e-308) { x2 = 0.; }
        else
        {
          _s_long n0 = 0; _s_long n2 = 309; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp2[n]) { n2 = n; } else if (x2 >= cmp2[n]) { n0 = n; } }
          if (n0 == 0) { n0 = 1; } order = n0 - 308; x2 /= cmp2[n0];
        }
      }
      _s_long i = _s_long(x2);
      _s_long n_int = 0; if (true) { _s_long i2 = i; do { ++n_int; i2 /= 10; } while (i2); }
      if (n_int > ndmmax)
      {
        _s_long n0 = 0; _s_long n2 = 7; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp1[n]) { n2 = n; } else if (x2 >= cmp1[n]) { n0 = n; } }
        order += n0; n_int -= n0; x2 /= cmp1[n0];
        i = _s_long(x2);
      }
      _s_long n_order = 0; if (order) { if (order <= -100) { n_order = 5; } else if (order <= -10 || order >= 100) { n_order = 4; } else if (order <= -1 || order >= 10) { n_order = 3; } else { n_order = 2; } }
      _s_long n = n_neg + n_int + 1 + n_order;

      if (n > nchars) { buf[0] = x > 0. ? '+' : (x < 0. ? '-' : '0'); return 1; }

      _s_long pos = 0; if (n_neg) { buf[pos++] = '-'; }
      _s_long q = str_from_s_ll(i, buf + pos, n_int, false); pos += q;
      n_int = q;
      n = n_neg + n_int + 1 + n_order;
      buf[pos++] = '.';
      q = ndmmax - n_int; _s_long n_frac = nchars - n; if (q < n_frac) { n_frac = q; } if (nfracmax < n_frac) { n_frac = nfracmax; }
      q = 0;
      if (n_frac > 0)
      {
        if (i) { x2 -= double(i); } if (x2 < 0.) { x2 = 0.; } else if (x2 >= 0.9999999999999) { x2 = 0.9999999999999; }
        _s_ll fr = _s_ll(x2 * 1.e13 + 0.5), mult = 1000000000000ll;
        _s_ll fr_min = _pow10[nfrac_mm - n_frac];
        while (n_frac && fr >= fr_min) { buf[pos++] = char((fr / mult) + '0'); fr %= mult; mult /= 10; --n_frac; ++q; }
      }
      if (n_order) { buf[pos++] = 'e'; str_from_s_ll(order, buf + pos, n_order - 1, true); }
      return n + q;
    }

    template<class _>
    _s_ll _bmdx_str_impl<_>::str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt, bool no_exc)
    {
        if (!x) { if (no_exc) { return dflt; } throw exc_str2i(); }
        if (xlen == -1) { xlen = _s_ll(wcslen(x)); }
        if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2i(); }
        const _u_ll mn = _u_ll(1) << (8 * sizeof(_u_ll) - 1);
        const _u_ll mp = ~mn;
        _u_ll q(0); _s_long nq(0);
        bool bf = false;
        _s_ll pos = 0; wchar_t c = L' ';
        bool b_neg(false);
        do // once
        {
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
          c = x[pos]; if (c == L'+') { ++pos; } else if (c == L'-') { b_neg = true; ++pos; }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q = q * 10 + int(c - L'0'); ++nq; ++pos; if (nq > 19) { bf = true; break; } } else { break; } }
              if (bf) { break; } if (pos >= xlen) { bf = nq == 0; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos < xlen) { bf = true; break; }
        } while (false);
        _s_ll z(0);
        do // once
        {
          if (bf) { break; }
          if (b_neg) { if (q < mn) { z = -_s_ll(q); } else if (q == mn) { z = -_s_ll(mp) - 1; } else { bf = true; break; } }
            else { if (q <= mp) { z = _s_ll(q); } else { bf = true; break; } }
        } while (false);
        if (!bf) { return z; }
        if (no_exc) { return dflt; }
        throw exc_str2i();
    }

    template<class _>
    double _bmdx_str_impl<_>::str2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans)
    {
        if (!x) { if (no_exc) { return dflt; } throw exc_str2f(); }
        if (xlen == -1) { xlen = _s_ll(wcslen(x)); }
        if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2f(); }
        bool bf = false;
        _s_ll pos = 0; wchar_t c = L' ';
        _s_ll q1(0), q2(0), q3(0); _s_long nq1(0), nq2(0), nq3(0); bool b_sign1(false), b_neg1(false), b_neg3(false); _s_ll nsd(0);
        do // once
        {
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
            if (b_nans && xlen - pos >= 3 && 0 == wcsncmp(L"nan", x + pos, 3)) { pos += 3; while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } return std::numeric_limits<double>::quiet_NaN(); }
          c = x[pos]; if (c == L'+') { b_sign1 = true; ++pos; } else if (c == L'-') { b_sign1 = true; b_neg1 = true; ++pos; }
              if (pos >= xlen) { bf = true; break; }
            if (b_nans && b_sign1 && xlen - pos >= 3 && 0 == wcsncmp(L"inf", x + pos, 3)) { pos += 3; while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } return b_neg1 ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity(); }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q1 = q1 * 10 + int(c - L'0'); ++nq1; if (q1) { ++nsd; } ++pos; if (nq1 > 19) { bf = true; break; } } else { break; } }
              if (bf) { break; } if (pos >= xlen) { bf = nq1 == 0; break; }
          c = x[pos];
            if (c == L'.')
            {
              ++pos;
              while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q2 = q2 * 10 + int(c - L'0'); ++nq2; if (q1 || (!q1 && q2)) { ++nsd; } ++pos; if (nq2 == 18) { break; } } else { break; } }
            }
            if (pos >= xlen) { bf = nq1 == 0 && nq2 == 0; break; }
          while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
            if (pos >= xlen) { break; }
          c = x[pos];
            if (c == L'e' || c == L'E')
            {
              ++pos;
              c = x[pos]; if (c == L'+') { ++pos; } else if (c == L'-') { b_neg3 = true; ++pos; }
                  if (pos >= xlen) { bf = true; break; }
              while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q3 = q3 * 10 + int(c - L'0'); ++nq3; ++pos; if (nq3 > 3) { bf = true; break; } } else { break; } }
                if (bf) { break; }
                if (nq3 == 0) { bf = true; break; }
            }
            else { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; }
        } while (false);
        double z(0.);
        do // once
        {
          if (bf) { break; }
          if (nq2)
          {
            const _s_ll nprec = 15;
            _s_ll m(1); for (_s_ll i = 0; i < nq2; ++i) { m *= 10; }
            if (nsd >= nprec)
            {
              z = (q1 * m + q2 + 0.0) / m;
            }
            else
            {
              _s_ll m2 = 1; for (_s_ll i = nsd; i < nprec; ++i) { m2 *= 10; }
              z = ((q1 * m + q2) * m2 + 0.1) / (m * m2);
            }
          }
          else { z = double(q1); }
          if (b_neg1) { z = -z; }
          if (q3)
          {
            if (b_neg3) { q3 = -q3; }
            if (q3 >= 280) { z *= 1.e280; q3 -= 280; if (q3 > 50) { bf = true; break; } }
              else if (q3 >= -280) { }
              else { z *= 1.e-280; q3 += 280; if (q3 < -50) { z = 0.; break; } }
            z *= std::pow(10., int(q3));
          }
        } while (false);
        if (!bf) { return z; }
        if (no_exc) { return dflt; }
        throw exc_str2f();
    }

      // Trivial analog of str2i(wchar_t*).
    template<class _>
    _s_ll _bmdx_str_impl<_>::str2i(const char* x, _s_ll xlen, _s_ll dflt, bool no_exc)
    {
if (!x) { if (no_exc) { return dflt; } throw exc_str2i(); } if (xlen == -1) { xlen = _s_ll(std::strlen(x)); } if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2i(); } const _u_ll mn = _u_ll(1) << (8 * sizeof(_u_ll) - 1); const _u_ll mp = ~mn; _u_ll q(0); _s_long nq(0); bool bf = false; _s_ll pos = 0; char c = ' '; bool b_neg(false);
do { while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } c = x[pos]; if (c == '+') { ++pos; } else if (c == '-') { b_neg = true; ++pos; } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q = q * 10 + int(c - '0'); ++nq; ++pos; if (nq > 19) { bf = true; break; } } else { break; } } if (bf) { break; } if (pos >= xlen) { bf = nq == 0; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } } while (false);
_s_ll z(0); do { if (bf) { break; } if (b_neg) { if (q < mn) { z = -_s_ll(q); } else if (q == mn) { z = -_s_ll(mp) - 1; } else { bf = true; break; } } else { if (q <= mp) { z = _s_ll(q); } else { bf = true; break; } } } while (false);
if (!bf) { return z; } if (no_exc) { return dflt; } throw exc_str2i();
    }

      // Trivial analog of str2f(wchar_t*).
    template<class _>
    double _bmdx_str_impl<_>::str2f(const char* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans)
    {
if (!x) { if (no_exc) { return dflt; } throw exc_str2f(); } if (xlen == -1) { xlen = _s_ll(std::strlen(x)); } if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2f(); } bool bf = false; _s_ll pos = 0; char c = ' '; _s_ll q1(0), q2(0), q3(0); _s_long nq1(0), nq2(0), nq3(0); bool b_sign1(false), b_neg1(false), b_neg3(false);
do { while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } if (b_nans && xlen - pos >= 3 && 0 == std::strncmp("nan", x + pos, 3)) { pos += 3; while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } return std::numeric_limits<double>::quiet_NaN(); } c = x[pos]; if (c == '+') { b_sign1 = true; ++pos; } else if (c == L'-') { b_sign1 = true; b_neg1 = true; ++pos; } if (pos >= xlen) { bf = true; break; } if (b_nans && b_sign1 && xlen - pos >= 3 && 0 == std::strncmp("inf", x + pos, 3)) { pos += 3; while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } return b_neg1 ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity(); } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q1 = q1 * 10 + int(c - '0'); ++nq1; ++pos; if (nq1 > 19) { bf = true; break; } } else { break; } } if (bf) { break; } if (pos >= xlen) { bf = nq1 == 0; break; }
c = x[pos]; if (c == '.') { ++pos; while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q2 = q2 * 10 + int(c - '0'); ++nq2; ++pos; if (nq2 == 18) { break; } } else { break; } } } if (pos >= xlen) { bf = nq1 == 0 && nq2 == 0; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { break; } c = x[pos]; if (c == 'e' || c == 'E') { ++pos; c = x[pos]; if (c == '+') { ++pos; } else if (c == '-') { b_neg3 = true; ++pos; } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q3 = q3 * 10 + int(c - '0'); ++nq3; ++pos; if (nq3 > 3) { bf = true; break; } } else { break; } } if (bf) { break; } if (nq3 == 0) { bf = true; break; } } else { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } } while (false);
double z(0.); do { if (bf) { break; } if (nq1 > 0) { if (nq2 > 0) { const _s_ll nprec = 15; _s_ll m(1); for (_s_ll i = 0; i < nq2; ++i) { m *= 10; } _s_ll nd = nq1 + nq2; if (nd >= nprec) { z = (q1 * m + q2 + 0.0) / m; } else { _s_ll m2 = 1; for (_s_ll i = nd; i < nprec; ++i) { m2 *= 10; } z = ((q1 * m + q2) * m2 + 0.1) / (m * m2); } } else { z = double(q1); } } if (b_neg1) { z = -z; } if (q3) { if (b_neg3) { q3 = -q3; } if (q3 >= 280) { z *= 1.e280; q3 -= 280; if (q3 > 50) { bf = true; break; } } else if (q3 >= -280) { } else { z *= 1.e-280; q3 += 280; if (q3 < -50) { z = 0.; break; } } z *= std::pow(10., int(q3)); } } while (false);
if (!bf) { return z; } if (no_exc) { return dflt; } throw exc_str2f();
    }
  }



    // Fixed-length string of char.
    //    Automatic conversion from std string, wstring, integer types, floating-point types.
    //    Min., max. length: 4, 2^20 - 1.
  template<_s_long nmax_>
  struct flstr_t
  {
  private: enum { _m_size = 0xfffff, _sh_res = 20 };
  public:
    typedef flstr_t t_string; enum { nmin_c = 4, nmax_c = nmax_ >= nmin_c ? (nmax_ <= _m_size ? nmax_ : _s_long(_m_size)) : nmin_c, res_max = _m_size, res_min = -res_max };

    flstr_t() { _nr = 1 << _sh_res; _x[0] = 0; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already.
    flstr_t(const char* ps, _s_ll n = -1) throw() { _nr = 0; _x[0] = 0; _set_res_u(_append_s(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::string& s) throw() { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }
    flstr_t(const wchar_t* ps, _s_ll n = -1) throw() { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::wstring& s) throw() { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(s.c_str(), s.length())); }

    template<_s_long nmax2> flstr_t(const flstr_t<nmax2>& s) throw() { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }

    flstr_t(double x, _s_long ndmmax = 6, _s_long nfracmax = 12, bool b_nans = true) throw() { _s_long n = conv::_bmdx_str_impl<>::str_from_double(x, _x, nmax(), ndmmax, nfracmax, b_nans); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 ? 1 : (n >= 0 ? 0 : -1)); }
    flstr_t(_s_ll x, bool b_signed) throw() { _s_long n = conv::_bmdx_str_impl<>::str_from_s_ll(x, _x, nmax(), b_signed); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 || (n == 1 && _x[0] != '-' && _x[0] != '+') ? 1 : (n >= 0 ? 0 : -1)); }

    flstr_t(signed short x) throw() { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed int x) throw() { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long x) throw() { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long long x) throw() { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(unsigned short x) throw() { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned int x) throw() { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long x) throw() { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long long x) throw() { new (this) flstr_t(_s_ll(x), false); }

    flstr_t(const void* x) throw() { new (this) flstr_t((_s_ll)x, false); }

    _s_long nmax() const  throw() { return nmax_c; }
    _s_long length() const throw() { return _nr & _m_size; }
    _s_long size() const throw() { return _nr & _m_size; }
    _s_long n() const throw() { return _nr & _m_size; }
    _s_long is_full() const throw() { return length() == nmax(); }
    _s_long is_empty() const throw() { return length() == 0; }
    _s_long res() const throw() { return _nr >> _sh_res; }
    const char* c_str() const throw() { return _x; }
    _s_long type() const throw() { return 0; }

      // If n >= 0, chars [pos0..min(pos0+n, length())) are returned.
      //  If n < 0, chars [pos0..length()) are returned.
    t_string substr(_s_long pos0, _s_long n) const throw() { _s_long n0 = length(); if (n <= 0) { if (n == 0) { return t_string(); } n = n0; } if (n > n0) { n = n0; } if (pos0 < 0) { pos0 = 0; } else if (pos0 > n0) { pos0 = n0; } if (pos0 + n > n0) { n = n0 - pos0; } return t_string(_x + pos0, n); }
      // Starts searching from pos0.
      //  (find_any) n_pcc >= 0 specifies num of chars. in pcc.
      // Returns [0..length()), or -1 if not found.
    _s_long find(char c, _s_long pos0 = 0) const throw() { _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } while (pos0 < pos2) { if (_x[pos0] == c) { return pos0; } ++pos0; } return -1; }
    _s_long find_any(const char* pcc, _s_long pos0 = 0, _s_long n_pcc = -1) const throw() { if (!pcc) { return -1; } _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } if (pos0 >= pos2) { return -1; } if (n_pcc < 0) { n_pcc = 0; const char* p = pcc; while (*p++) { ++n_pcc; } } while (pos0 < pos2) { char c = _x[pos0]; for (_s_long i = 0; i < n_pcc; ++i) { if (c == pcc[i]) { return pos0; } } ++pos0; } return -1; }

    #ifdef _MSC_VER
      inline operator std::string() const { return _x; }
      inline operator std::wstring() const { return wstr(); }
    #else
      inline operator std::string() const throw (std::exception __bmdx_noargt) { return _x; }
      inline operator std::wstring() const throw (std::exception __bmdx_noargt) { return wstr(); }
    #endif
    inline std::string str(__bmdx_noarg1) const { return _x; }
    std::wstring wstr(__bmdx_noarg1) const // UTF-8 --> UTF-16
    {
      std::wstring s; _s_ll i = 0, n = length();
      while (i < n)
      {
        unsigned char c = _x[i];
        if ((c & 0x80) == 0) { s += wchar_t(c); ++i; }
        else
        {
          _s_long nbchr = 0;
          while (((c << nbchr) & 0xc0) == 0xc0) { ++nbchr; }
          if (nbchr >= 1 && nbchr <= 5 && i + nbchr < n)
          {
            _s_long q(c & (0x3f >> nbchr)); ++i;
            do { c = _x[i]; if ((c & 0xc0) != 0x80) { q = L'?'; break; } q = (q << 6) | (c & 0x3f); ++i; } while (--nbchr);
            if (q > 0x10ffff) { s += L'?'; }
              else if (q >= 0x10000) { q -= 0x10000; s += wchar_t(0xd800 | (q >> 10)); s += wchar_t(0xdc00 | (q & 0x3ff)); }
              else { s += wchar_t(q); }
          }
          else { s += L'?'; ++i; }
        }
      }
      return s;
    }

      // Modify the last result (res()) value. Before setting, x is limited by res_min..res_max.
    void set_res(_s_long x) throw() { if (x < res_min) { x = res_min; } else if (x > res_max) { x = res_max; } _set_res_u(x); }

      // res(): 1: success; 0: n > nmax(), so nmax() is set; -1: n is negative, nothing changed.
    void resize(_s_long n, char c = ' ') throw() { if (n < 0) { _set_res_u(-1); return; } bool b = n > nmax(); if (b) { n = nmax(); } _s_long delta = n - length(); if (delta > 0) { _set_u(c, _x + length(), delta); } if (delta != 0) { _set_end_u(n); } _set_res_u(b ? 0 : 1); }

      // res(): 1.
    void clear() throw() { resize(0); }


    char& operator[] (_s_long i) { return _x[i]; }
    const char& operator[] (_s_long i) const { return _x[i]; }

    bool operator == (const t_string& s) const        throw() { const _s_long n = length(); if (n != s.length()) { return false; } for (_s_long i = 0; i < n; ++i) { if (_x[i] != s._x[i]) { return false; } } return true; }
    bool operator < (const t_string& s) const        throw() { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() < s.n()); }
    bool operator <= (const t_string& s) const        throw() { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() <= s.n()); }
    bool operator != (const t_string& s) const        throw() { return !operator==(s); }
    bool operator > (const t_string& s) const        throw() { return !operator<(s); }
    bool operator >= (const t_string& s) const        throw() { return !operator<=(s); }

    template<_s_long n2> bool operator == (const flstr_t<n2>& s) const        throw() { const _s_long n = length(); if (n != s.length()) { return false; } for (_s_long i = 0; i < n; ++i) { if (_x[i] != s._x[i]) { return false; } } return true; }
    template<_s_long n2> bool operator < (const flstr_t<n2>& s) const        throw() { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() < s.n()); }
    template<_s_long n2> bool operator <= (const flstr_t<n2>& s) const        throw() { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() <= s.n()); }
    template<_s_long n2> bool operator != (const flstr_t<n2>& s) const        throw() { return !operator==(s); }
    template<_s_long n2> bool operator > (const flstr_t<n2>& s) const        throw() { return !operator<(s); }
    template<_s_long n2> bool operator >= (const flstr_t<n2>& s) const        throw() { return !operator<=(s); }

    template<class T> t_string& operator << (const T& x) { *this += x; return *this; }


      // res(): 1: success; -1: length() == nmax() already.
    t_string& operator += (char c) throw() { _s_long n = length(); if (n >= nmax()) { _set_res_u(-1); return *this; } _x[n] = c; ++n; _x[n] = 0; _nr = (1 << _sh_res) | n; return *this; }

    t_string& operator += (double x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (wchar_t x) throw() { append(&x, 1); return *this; }

    t_string& operator += (signed short x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (signed int x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (signed long x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (signed long long x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (unsigned short x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (unsigned int x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (unsigned long x) throw() { *this += t_string(x); return *this; }
    t_string& operator += (unsigned long long x) throw() { *this += t_string(x); return *this; }

    t_string operator + (char c) throw() { t_string s2(*this); s2 += c; return s2; }

    t_string operator + (double x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (wchar_t x) throw() { t_string s2(*this); s2 += x; return s2; }

    t_string operator + (signed short x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed int x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long long x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned short x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned int x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long x) throw() { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long long x) throw() { t_string s2(*this); s2 += x; return s2; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already; -2: ps == 0.
    t_string& append (const char* ps, _s_ll n) throw() { _set_res_u(_append_s(ps, n >= 0 ? n : -1)); return *this; }
    t_string& operator += (const char* ps) throw() { _set_res_u(_append_s(ps, -1)); return *this; }
    t_string& operator += (const std::string& s) throw() { _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    t_string& operator = (const char* ps) throw() { resize(0); _set_res_u(_append_s(ps, -1)); return *this; }
    t_string& operator = (const std::string& s) throw() { resize(0); _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    t_string operator + (const char* ps) throw() { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::string& s) throw() { t_string s2(*this); s2 += s; return s2; }

    t_string& append (const wchar_t* ps, _s_ll n) throw() { _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); return *this; }
    t_string& operator += (const wchar_t* ps) throw() {  _set_res_u(_append_wcs(ps, -1)); return *this; }
    t_string& operator += (const std::wstring& s) throw() { _set_res_u(_append_wcs(s.c_str(), s.length())); return *this; }
    t_string& operator = (const wchar_t* ps) throw() { resize(0); _set_res_u(_append_wcs(ps, -1)); return *this; }
    t_string& operator = (const std::wstring& s) throw() { resize(0); _set_res_u(_append_wcs(s.c_str(), s.length())); return *this; }
    t_string operator + (const wchar_t* ps) throw() { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::wstring& s) throw() { t_string s2(*this); s2 += s; return s2; }

    template<_s_long nmax2> t_string& operator += (const flstr_t<nmax2>& s) throw() { _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    template<_s_long nmax2> t_string operator + (const flstr_t<nmax2>& s) throw() { t_string s2(*this); s2 += s; return s2; }
    template<_s_long nmax2> t_string& operator = (const flstr_t<nmax2>& s) throw() { resize(0); _set_res_u(_append_s(s.c_str(), s.length())); return *this; }

    private:
      _s_long _nr; char _x[nmax_c + 1];
      inline void _copy_u(const char* buf, char* dest, _s_long n) { while (n > 0) { *dest++ = *buf++; --n; } }
        // Copies <= nc chars., stops on null character (it's not copied).
        //  Returns num. of characters copied. Num. < nmax means "null character is reached".
      inline _s_long _copy_le_u(const char* buf, char* dest, _s_long nc) { _s_long n = 0; while (nc > 0) { char c = *buf++; if (!c) { break; } *dest++ = c; ++n; --nc; } return n; }
      inline void _set_u(char c, char* dest, _s_long n) { while (n > 0) { *dest++ = c; --n; } }
      inline void _set_end_u(_s_long n) { _x[n] = 0; _nr &= ~_s_long(_m_size); _nr |= n; }
      inline void _set_res_u(_s_long x) throw() { _nr &= _m_size; _nr |= (x << _sh_res); }
        // 1: success, 0: partially copied, -1: string is full, cannot append, -2: invalid ps, nsrc.
      _s_long _append_s(const char* ps, _s_ll nsrc = -1) throw()
      {
        if (!(ps && nsrc >= -1)) { return -2; }
        _s_long ncmax = nmax() - length(); if (nsrc > 0 && ncmax <= 0) { return -1; }
        if (nsrc >= 0) { bool b_part = nsrc > ncmax; if (b_part) { nsrc = ncmax; } _copy_u(ps, &_x[length()], _s_long(nsrc)); _set_end_u(length() + _s_long(nsrc)); return b_part ? 0 : 1; }
          else { _s_long n = _copy_le_u(ps, &_x[length()], ncmax + 1); if (n == ncmax + 1) { _set_end_u(nmax()); return 0; } else { _set_end_u(length() + n); return 1; } }
      }
        // 1: success, 0: partially copied, -1: string is full, cannot append, -2: invalid ps, nsrc.
      inline _s_long _append_wcs(const wchar_t* ps, _s_ll nsrc = -1)
      {
        _s_long res; _s_ll pos = 0;
        do { res = _append_wc(ps, pos, nsrc); } while (res == 1);
        if (res == 0) { return 1; }
        if (res == -1 && pos > 0) { return 0; }
        return res;
      }
        // nsrc: >=0 - num. of characters available in the input, starting from ps. -1 - input is 0-terminated.
        // 1: success, 0: end of input, -1: string is full, cannot append, -2: invalid ps, pos, nsrc.
      inline _s_long _append_wc(const wchar_t* ps, _s_ll& pos, _s_ll nsrc)
      {
        if (!(ps && nsrc >= -1 && pos >= 0)) { return -2; } const wchar_t* p2 = ps + pos;
        if (nsrc >= 0 && pos >= nsrc) { return 0; }
        _s_long q = p2[0] & 0xffff; if (nsrc == -1 && q == 0) { return 0; }
        _s_ll dpos = 1;
        if (q >= 0xd800 && q < 0xe000)
        {
          if (q >= 0xdc00 || (nsrc == -1 && p2[1] == 0) || (nsrc >= 0 && pos + 1 >= nsrc)) { q = L'?'; }
            else { _s_long q2 = _s_long(p2[1]); if (q2 >= 0xdc00 && q2 < 0xe000) { q = (((q - 0xd800) << 10) | (q2 - 0xdc00)) + 0x10000; dpos += 1; } else { q = L'?'; } }
        }
        _s_long n = length();
        _s_long nmdest = nmax() - n;
        if (q <= 0x7f) { if (nmdest < 1) { return -1; } }
          else if (q <= 0x7ff) { if (nmdest < 2) { return -1; } }
          else if (q <= 0xffff) { if (nmdest < 3) { return -1; } }
          else { if (nmdest < 4) { return -1; } }
        if (q <= 0x7f) { _x[n] = char(q); _set_end_u(n + 1); }
          else if (q <= 0x7ff) { _x[n] = char((q >> 6) | 0xc0); _x[n + 1] = char((q & 0x3f) | 0x80); _set_end_u(n + 2); }
          else if (q <= 0xffff) { _x[n] = char((q >> 12) | 0xe0); _x[n + 1] = char(((q >> 6) & 0x3f) | 0x80); _x[n + 2] = char((q & 0x3f) | 0x80); _set_end_u(n + 3); }
          else { _x[n] = char((q >> 18) | 0xf0); _x[n + 1] = char(((q >> 12) & 0x3f) | 0x80); _x[n + 2] = char(((q >> 6) & 0x3f) | 0x80); _x[n + 3] = char((q & 0x3f) | 0x80); _set_end_u(n + 4); }
        pos += dpos;
        return 1;
      }
  };

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const char* ps, const flstr_t<nmax_>& s2) throw() { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::string& s, const flstr_t<nmax_>& s2) throw() { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const wchar_t* ps, const flstr_t<nmax_>& s2) throw() { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::wstring& s, const flstr_t<nmax_>& s2) throw() { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline std::ostream& operator << (std::ostream& stm, const flstr_t<nmax_>& s2) throw() { stm << s2.str(); return stm; }

  namespace conv { namespace
  {
      // Converting numbers and pointer values to strings, using default format.
    template<class T> static inline std::string ntocs(const T& x) { typedef flstr_t<25> S; return S(x).str(); }
    template<class T> static inline std::wstring ntows(const T& x) { typedef flstr_t<25> S; return S(x).wstr(); }
    static inline std::string ftocs(double x, _s_long ndmmax = 6, _s_long nfracmax = 12) { typedef flstr_t<25> S; return S(x, ndmmax, nfracmax).str(); }
    static inline std::wstring ftows(double x, _s_long ndmmax = 6, _s_long nfracmax = 12) { typedef flstr_t<25> S; return S(x, ndmmax, nfracmax).wstr(); }
    static inline std::string itocs(_s_ll x, bool b_signed) { typedef flstr_t<25> S; return S(x, b_signed).str(); }
    static inline std::wstring itows(_s_ll x, bool b_signed) { typedef flstr_t<25> S; return S(x, b_signed).wstr(); }

      // Converts x to decimal string representation.
      //    nchars >= 0.
      // NOTE Max. number of characters in string repr. of x is 20.
      // Returns:
      //    >=1 - success (if nchars is enough).
      //    1 if nchars < the number of characters in string repr. of x. buf[0] will be:
      //        a) x > 0: '+'.
      //        b) x == 0: '0'.
      //        c) x < 0: '-'.
      //    0 - nchars == 0.
      //    -1 - invlaid input parameter (buf == 0, nchars < 0).
    static inline _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed = true) throw() { return _bmdx_str_impl<>::str_from_s_ll(x, buf, nchars, b_signed); }

      // ndmmax (max. num. of digits in mantissa) is limited to 1..14.
      //    Max number of output characters == limited ndmmax + 7 (0..1 sign, 1 point, 0..1 "e", 0..4 order).
      // nfracmax (max. num. of digits in fractional part) is limited to 0..ndmmax-1.
      // abs(x) < 1.e-3 or  >= 1.e6 is shown in exponential format.
      //    if x cannot fit in ndmmax, it is also shown in exponential format.
      // b_nans: true: allow nan, +inf, -inf; false (dflt.): regard as error and return -1 for any of them.
      // Returns:
      //    >=2 - success.
      //    1 - string representation is longer than nchars, so only sign is extracted: buf[0] == '+', '0', or '-'.
      //    0 - nchars == 0.
      //    -1 - invlaid input parameter (buf == 0, nchars < 0, x is not finite with b_nans == false).
    static inline _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax = 6, _s_long nfracmax = 12, bool b_nans = true) throw() { return _bmdx_str_impl<>::str_from_double(x, buf, nchars, ndmmax, nfracmax, b_nans); }

      // Converts string to number. On failure, returns dflt, or (if no_exc == false) generates exc_str2i.
      //  For char* and wchar_t* versions, xlen == -1 means that x is 0-terminated.
    static inline _s_ll str2i(const std::wstring& x, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    static inline _s_ll strn2i(const wchar_t* x, _s_ll xlen, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x, xlen, dflt, no_exc); }
    static inline _s_ll str2i(const std::string& x, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    static inline _s_ll strn2i(const char* x, _s_ll xlen, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x, xlen, dflt, no_exc); }

      // Converts string to number. On failure, returns dflt, or (if no_exc == false) generates exc_str2f.
      //  For char* and wchar_t* versions, xlen == -1 means that x is 0-terminated.
      //  b_nans true accepts nan, +inf, -inf; by default they are not recognized.
    static inline double str2f(const std::wstring& x, double dflt = 0., bool no_exc = true, bool b_nans = true) { return _bmdx_str_impl<>::str2f(x.c_str(), _s_ll(x.length()), dflt, no_exc, b_nans); }
    static inline double strn2f(const wchar_t* x, _s_ll xlen, double dflt = 0., bool no_exc = true, bool b_nans = true) { return _bmdx_str_impl<>::str2f(x, xlen, dflt, no_exc, b_nans); }
    static inline double str2f(const std::string& x, double dflt = 0., bool no_exc = true, bool b_nans = true) { return _bmdx_str_impl<>::str2f(x.c_str(), _s_ll(x.length()), dflt, no_exc, b_nans); }
    static inline double strn2f(const char* x, _s_ll xlen, double dflt = 0., bool no_exc = true, bool b_nans = true) { return _bmdx_str_impl<>::str2f(x, xlen, dflt, no_exc, b_nans); }
  } }
}

#ifdef _bmdxpl_Wnds
  #include <windows.h>
  #include <io.h>
  #include <direct.h>
  #include <conio.h>
#endif



namespace bmdx
{
  using yk_c::__vecm_tu_selector;

    // operator= and copy constructor exception for array types with enabled exceptions. May be caught on the client.
  struct exc_carr_alloc_asg : std::exception { const char* what() const throw() { return "c[pp]array_t/operator=|cc/alloc"; } };

    // Internal exceptions, do not reach the client.
  struct __exc_carr_asg : std::exception { const char* what() const throw() { return "_carr_asgx_t::try_asg"; } };
  struct __exc_carr_cc : std::exception { const char* what() const throw() { return "_carr_asgx_t:try_cc"; } };
  template<class T, bool no_exc_asg> struct carray_t;
  template<class T, bool no_exc_asg> struct cpparray_t;
  template<class T, bool no_exc_asg> struct carray_r_t;
  template<class T> struct arrayref_t;
  template<class A, class Ax, class _ = __vecm_tu_selector> struct _carr_aux1_t { static inline void try_asg(A& dest, const A& src) { (Ax&)dest = (const Ax&)src; if (dest.n() != src.n()) { throw __exc_carr_asg(); } } static inline void try_cc(void* pdest, const A& src) { new (pdest) Ax((const Ax&)src); if (((A*)pdest)->n() != src.n()) { throw __exc_carr_cc(); } } };
    // _carr_asgx_t provides assignment and placement copy construction (try_asg, try_cc)
    //    with forced exceptions on error for all carray_t and cpparray_t variants.
    //    (Easier error processing for deeply nested structures.)
  template<class A, class Aux = bmdx_meta::nothing, class _ = __vecm_tu_selector> struct _carr_asgx_t { enum { is_carr_any = 0, is_cpparray = 0 }; typedef A t_ax; static inline void try_asg(A& dest, const A& src) { dest = src; } static inline void try_cc(void* pdest, const A& src) { new (pdest) A(src); } };
  template<class T, class _> struct _carr_asgx_t<carray_t<T, true>, bmdx_meta::nothing, _> : _carr_aux1_t<carray_t<T, true>, carray_t<T, false>, _> { enum { is_carr_any = 1, is_cpparray = 0 }; typedef carray_t<T, false> t_ax; static inline void check_exc_alloc() { } };
  template<class T, class _> struct _carr_asgx_t<cpparray_t<T, true>, bmdx_meta::nothing, _> : _carr_aux1_t<cpparray_t<T, true>, cpparray_t<T, false>, _> { enum { is_carr_any = 1, is_cpparray = 1 }; typedef cpparray_t<T, false> t_ax; static inline void check_exc_alloc() { } };
  template<class T, class _> struct _carr_asgx_t<carray_t<T, false>, bmdx_meta::nothing, _> : _carr_asgx_t<carray_t<T, false>, int, _> { enum { is_carr_any = 1, is_cpparray = 0 }; static inline void check_exc_alloc() { throw exc_carr_alloc_asg(); } };
  template<class T, class _> struct _carr_asgx_t<cpparray_t<T, false>, bmdx_meta::nothing, _> : _carr_asgx_t<cpparray_t<T, false>, int, _> { enum { is_carr_any = 1, is_cpparray = 1 }; static inline void check_exc_alloc() { throw exc_carr_alloc_asg(); } };

  template<class T, class _ = __vecm_tu_selector>
  struct _carray_tu_alloc_t
  {
      #ifdef calloc
        static inline void* _s_psf1() throw() { return 0; }
      #else
        static inline void* _s_psf1() throw() { return (void*)&std::calloc; }
      #endif
      static void* _sf_alloc(_s_ll nb) throw() { if (nb < 0) { return 0; } size_t n2 = size_t(nb); if (_s_ll(n2) != nb) { return 0; } try { return std::calloc(n2, 1); } catch (...) { return 0; } }
      static void _sf_free(void* p) throw() { try { std::free(p); } catch (...) {} }
      static void _sf_destroy1(T* p) throw() { if (p) { try { p->~T(); } catch (...) {} } }
    static void* _spff[];
  };
  template<class T, class _> void* _carray_tu_alloc_t<T, _>::_spff[4] = { (void*)(3 + (const char*)0), (void*)&_sf_alloc, (void*)&_sf_free, (void*)&_sf_destroy1 };

  template<class T>
  struct _carray_base_t
  {
    typedef T t_value;
    typedef size_t _t_size; // _t_size: private; client uses s_ll as index as size type

    _carray_base_t() throw() : _n(0), _data(0), _psf1(_carray_tu_alloc_t<T>::_s_psf1()) { __pad1 = 0; _pff = _carray_tu_alloc_t<T>::_spff; }

    inline _s_ll n() const throw()  { return _n; }

      // NOTE Normally, pd() == 0 if n() == 0. See also _set_n_u.
    inline const t_value* pd() const throw() { return _data; }
    inline t_value* pd() throw() { return _data; }

    inline const t_value& operator[] (_s_ll i) const throw() { return _data[_t_size(i)]; }
    inline t_value& operator[] (_s_ll i) throw() { return _data[_t_size(i)]; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) throw()
    {
      _t_size i0 = 0; _t_size nf(0); while (i0 < _t_size(_n)) { try { for (; i0 < _t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == _t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) throw()
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      _t_size _i0 = _t_size(i0); const _t_size _i2 = _t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const _t_size nset = _i2 - _i0;
      _t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

      // Reallocate storage. Copy, destroy, initialize elements as specified.
      //  n >= 0: the new number of elements of type T.
      //  dmode: 1 - use local destructor, 0 - do not call destructor.
      //  imode: 4 - copy existing elems. with std::memcpy, 1 - copy with local T(const T&), 0 - no copying (new allocation only + initialize all bytes to 0).
      //  px:
      //    != 0 - if elements are added, initialize them as copy *px (method depends on imode).
      //    == 0 with imode == 0, 4 - initialize all bytes to 0.
      //    == 0 with imode == 1 - initialize as T().
      // Returns: true - success, false - failure, no changes.
      // NOTE realloc() proceeds with realloc/copy/init even if n == n().
    bool realloc(_s_ll n, int dmode, int imode, const t_value* px) throw()
    {
      if (!(n >= 0 && dmode == (dmode & 1) && (imode == 4 || imode == (imode & 1)))) { return false; }
      T* pd2 = 0;
      if (n > 0)
      {
        pd2 = (T*)_alloc(n * sizeof(T)); if (!pd2) { return false; }
        _t_size n0 = _t_size(n < _n ? n : _n); _t_size i0 = 0;
        if (imode == 1)
        {
          try { for (; i0 < n0; ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, _data[i0]); } } catch (...) {}
            if (i0 < n0) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          if (n > _n)
          {
            if (px) { try { for (; i0 < _t_size(n); ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, *px); } } catch (...) {} }
              else { try { for (; i0 < _t_size(n); ++i0) { new (pd2 + i0) T(); } } catch (...) {} }
            if (i0 < _t_size(n)) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          }
        }
        else if (imode == 4)
          { if (n0) { std::memcpy(pd2, _data, n0 * sizeof(T)); } if (px && n > _n) { for (i0 = n0; i0 < _t_size(n); ++i0) { std::memcpy(pd2 + i0, px, sizeof(T)); } } }
      }
      if (_n > 0) { if (dmode == 1) { _destroy(_data, 0, _t_size(_n)); } _free(_data); }
      _data = pd2; _n = n;
      return true;
    }

      // Element-by-element equality check.
    bool is_eq(const _carray_base_t<t_value>& x) const { if (this == &x) { return true; } if (this->_n != x._n) { return false; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }

      // Sets _n field (number of elements), without memory reallocation or elements creation/destruction.
      //  May be used carefully to reserve/unreserve place.
    void _set_n_u(_s_ll n) { _n = n >= 0 ? n : 0; }

  protected:
    _s_ll _n; T* _data; void* _psf1; union { void** _pff; _s_ll __pad1; };
    ~_carray_base_t() throw() { if (_data) { _free(_data); _data = 0; } _n = 0; }

    typedef void* (*F_alloc)(_s_ll nbytes); typedef void (*F_free)(void* p);
    inline void* _alloc(_s_ll nbytes) throw() { if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { return _carray_tu_alloc_t<T>::_sf_alloc(nbytes); } return ((F_alloc)_pff[1])(nbytes); }
    inline void _free(void* p) throw() { if (!p) { return; } if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { _carray_tu_alloc_t<T>::_sf_free(p); return; } ((F_free)_pff[2])(p); }

    inline void _destroy(T* pd_, _t_size i0, _t_size i2) throw() { (void)pd_; while (i0 < i2) { try { for (; i0 < i2; ++i0) { (pd_ + i0)->~T(); } } catch (...) { ++i0; } } }
  private:
    _carray_base_t(const _carray_base_t&); _carray_base_t& operator=(const _carray_base_t&);
  };

  template<class T, bool no_exc_asg>
  struct cpparray_t;

    // C-style array wrapper. For T allowing bytewise copying and not needing constructor/destructor calls.
    // Behavior:
    //    1. Any elements copying copies just bytes.
    //      If storage allocation fails, no exceptions (dflt., no_exc_asg == true):
    //        a) assignment returns false, no changes to target;
    //        b) copy ctor. creates an empty object.
    //
    //    2. resize(n) preserves existing elements.
    //      If size grows, new elements are initialized by writing 0 to each byte.
    //      For pure storage reallocation without copying existing elements:
    //        array.realloc(n, 0, 0, 0);
    //      For storage growth with new elements initialization:
    //        a) T src; array.realloc(n, 0, 4, &src);
    //        b) array.realloc(n, 0, 1, 0); // using T() for new elements.
    //
    //    3. On carray_t destruction, individual elements destructors ARE NOT called.
    //
    //    4. carray_t, created in binary module A, may be passed as bytes or as container element,
    //        manipulated and destroyed locally in binary module B.
    //      a) WinAPI: any A, B / POSIX: A, B using standard malloc() family:
    //        carray_t destruction in B is safe even after A being unloaded.
    //      b) POSIX: A or B using custom malloc() family: carray_t destruction in B is safe only before unloading A.
  template<class T, bool no_exc_asg = true>
  struct carray_t : _carray_base_t<T>
  {
    typedef T t_value;
    typedef carray_t t_a;
    typedef typename _carray_base_t<T>::_t_size _t_size; // _t_size: private; client uses s_ll as index as size type

    carray_t() throw() {}
    carray_t(const carray_t& x) : _carray_base_t<T>() { if (!this->realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return; } if (x.n()) { std::memcpy(this->_data, x._data, _t_size(x.n()) * sizeof(T)); } }
    ~carray_t() throw() {}

      // If this != &src: clears this, moves src to this, sets src to be empty.
    void move(carray_t& src) throw() { if (this == &src) { return; } this->~carray_t(); std::memcpy(this, &src, sizeof(carray_t)); src._data = 0; src._n = 0; }
    void swap(carray_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; std::memcpy(x, &src, sizeof(t_a)); std::memcpy(&src, this, sizeof(t_a)); std::memcpy(this, x, sizeof(t_a)); }
    bool resize(_s_ll n) throw() { if (n == this->_n) { return true; } return this->realloc(n, 0, 4, 0); }
    void clear() throw() { this->realloc(0, 0, 4, 0); }
    bool operator=(const carray_t& x)
      { if (this == &x) { return true; } carray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; } if (x.n()) { std::memcpy(x2._data, x._data, size_t(x.n()) * sizeof(T)); } this->move(x2); return true; }

    template<bool _ne> inline bool operator==(const carray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const cpparray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const cpparray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }
  };

    // C++-style array wrapper. For T with non-trivial construction, copying and destructor calls.
    // Behavior:
    //    1. Any copying is via T(const T&). If operation fails, no exceptions (dflt., no_exc_asg == true):
    //      a) assignment returns false, no changes to target; b) copy ctor. creates an empty object.
    //
    //    2. resize(n) preserves elements.
    //      If size grows, new elements ARE initialized as T().
    //
    //    3. On cpparray_t destruction, individual elements destructors ARE called.
    //
    //    4. cpparray_t, created in binary module A, may be passed as bytes or as container element,
    //        manipulated and destroyed locally in binary module B if all the following conditions are true:
    //      1) T(), T(const T&), ~T() in B behave exactly as in A, i.e. have module-independent data, like flstr_t,
    //        or are aware of the current binary module, like carray_t, cpparray_t, vec2_t, unity, threadctl, hashx etc.
    //        NOTE STL's and other objects with dynamic data, like string, vector etc. can safely be elements of cpparray_t
    //        only in the binary module where they are constructed.
    //      2) cpparray_t destruction in B is safe only before unloading A.
  template<class T, bool no_exc_asg = true>
  struct cpparray_t : _carray_base_t<T>
  {
    typedef T t_value;
    typedef cpparray_t t_a;
    typedef typename _carray_base_t<T>::_t_size _t_size; // _t_size: private; client uses s_ll as index as size type

    cpparray_t() throw() {}
    cpparray_t(const cpparray_t& x) : _carray_base_t<T>() { *this = x; }
    ~cpparray_t() throw() { this->realloc(0, 1, 0, 0); }

      // If this != &src: clears this, moves src to this, sets src to be empty.
    void move(cpparray_t& src) throw() { if (this == &src) { return; } this->~cpparray_t(); std::memcpy(this, &src, sizeof(t_a)); src._data = 0; src._n = 0; }
    void swap(cpparray_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; std::memcpy(x, &src, sizeof(t_a)); std::memcpy(&src, this, sizeof(t_a)); std::memcpy(this, x, sizeof(t_a)); }
    bool resize(_s_ll n) throw() { if (n == this->_n) { return true; } return this->realloc(n, 1, 1, 0); }
    void clear() throw() { this->realloc(0, 1, 1, 0); }
    bool operator=(const cpparray_t& x)
    {
      if (this == &x) { return true; } cpparray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      T* pd2 = x2.pd(); _t_size i0 = 0; _t_size n0 = x2.n();
      try { for (; i0 < n0; ++i0) { new (pd2 + i0) T(x[i0]); } } catch (...) {}
      if (i0 < n0) { this->_destroy(pd2, 0, i0); x2.realloc(0, 0, 0, 0); _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      this->move(x2);
      return true;
    }

    template<bool _ne> inline bool operator==(const carray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const cpparray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const cpparray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }
  };

    // Simple string class.
    //  Difference from std::string:
    //    1. Default constructor does not generate exceptions in principle.
    //    2. The object may be used in other binary module (not where it was created).
    //    3. To create C-string, use ensure_cstr(). This reserves 1 elem. and sets it to T().
    //  Difference from carray_t, cpparray_t:
    //    1. The object is automatically resizable.
    //      May be appended with single element (T) or some number of elements (from T*).
    //    2. The place may be pre-reserved, to ensure faultless appending.
    //    3. By default, all allocation exceptions are enabled (array copying, construction, assignment).
  template<class T, bool no_exc_asg = false>
  struct carray_r_t : protected carray_t<T, no_exc_asg>
  {
    typedef T t_value;
    typedef carray_r_t<T, no_exc_asg> t_a; typedef carray_t<T, no_exc_asg> t_a0; typedef typename _carray_base_t<T>::_t_size _t_size;


    _s_ll nrsv() const throw() { return _nrsv; }
    _s_ll n() const throw() { return this->_n; }

      // NOTE Normally, pd() == 0 if n() == 0.
    const t_value* pd() const throw() { return this->_data; }
    t_value* pd() throw() { return this->_data; }

    const t_value& operator[] (_s_ll i) const throw() { return this->_data[_t_size(i)]; }
    t_value& operator[] (_s_ll i) throw() { return this->_data[_t_size(i)]; }


    carray_r_t() throw() : _nrsv(0) {}
      // NOTE on copying, new nrsv() == x.n().
    carray_r_t(const t_a& x) : carray_t<T, no_exc_asg>(), _nrsv(0) { if (!this->resize(x.n(), false, true)) { _carr_asgx_t<t_a0>::check_exc_alloc(); return; } if (x.n()) { std::memcpy(this->_data, x._data, size_t(x.n()) * sizeof(T)); } }
    carray_r_t(_s_ll n, const t_value& x) : _nrsv(0) { if (!append_nx(n, x)) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    carray_r_t(const t_value* px, _s_ll n) : _nrsv(0) { if (!append_n(px, n)) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    carray_r_t(const arrayref_t<t_value>& x) : _nrsv(0) { if (!append_n(x.pd(), x.n())) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    ~carray_r_t() throw() {}


    void move(t_a& src) throw() { if (this == &src) { return; } this->~carray_r_t(); std::memcpy(this, &src, sizeof(t_a)); src._data = 0; src._n = 0; src._nrsv = 0; }
    void swap(t_a& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; std::memcpy(x, &src, sizeof(t_a)); std::memcpy(&src, this, sizeof(t_a)); std::memcpy(this, x, sizeof(t_a)); }
    void clear() throw() { this->resize(0, false, true); } // deallocate the array (nrsv() becomes 0)
      // NOTE On assignment, if old reserve is enough to hold values, no memory reallocation occurs.
      //    Otherwise, new nrsv() == x.n(). Exact behavior: see also resize() with b_minimize_rsv true.
    bool operator=(const carray_r_t& x) { if (this == &x) { return true; } if (!this->resize(x.n(), false, true)) { _carr_asgx_t<t_a0>::check_exc_alloc(); return false; } if (x.n()) { std::memcpy(this->_data, x._data, size_t(x.n()) * sizeof(T)); } return true; }

    std::basic_string<t_value> str() const { const T z = T(); return std::basic_string<T>(this->_data ? this->_data : &z, size_t(this->_n)); }

      // Resize to n2.
      //  b_copy (acts only if the array is reallocated):
      //    true: copy all elements from the old array,
      //    false: leave filled with zero bytes.
      //  b_minimize_rsv true:
      //    - if n2 == 0, the array is completely deallocated, and carray_r_t occupies totally only sizeof(carray_r_t) bytes of memory.
      //    - if n2 < min(n(), nrsv() / 2), new nrsv = n2. (But if reallocation fails, the reserve is not shrunk, and the function returns true (success).)
      //    - if n2 > nrsv(), new nrsv = n2.
      //    - otherwise (i.e. n2 == [0.5..1]*nrsv() and >= n()), no memory reallocation occurs.
      //    This mode should be used to minimize both memory overhead and reallocations,
      //      esp. when the array size is set once and increased rarely or never.
      //  b_minimize_rsv false:
      //    - if n2 > nrsv(), new nrsv = 2 * n2 (similar to std vector).
      //    - if n2 <= nrsv(), no memory reallocation or deallocation occurs, even if n2 == 0.
      //    This mode should be used when the array size changes frequently and unpredictibly.
      //    For finer control over the reserve, consider using reserve() together with resize().
      //  px
      //    != 0: if resizing to larger size, write *px to the newly added positions.
      //    == 0: leave the added or created poisitions zero-initialized:
      //      a) If reallocation occurs due to reserve change,
      //        1) whole the new array is filled with zero bytes,
      //        2) b_copy true copies elements from the prev. array.
      //      b) If no reallocation done, the existing array is not modified in any way.
      // NOTE If new size is within the current reserve, the function always succeeds.
    bool resize(_s_ll n2, bool b_copy, bool b_minimize_rsv, const t_value* px = 0) throw()
    {
      if (n2 < 0) { return false; }
      T* pd2 = this->_data; _s_ll nrsv2 = _nrsv;
      if (n2 == 0 && b_minimize_rsv) { pd2 = 0; nrsv2 = 0; }
        else if (n2 > _nrsv || (b_minimize_rsv && n2 < _nrsv / 2 && n2 < this->_n)) { nrsv2 = n2; if (n2 > _nrsv && !b_minimize_rsv) { nrsv2 = 2 * n2; } pd2 = (T*)this->_alloc(nrsv2 * sizeof(T)); if (!pd2) { if (n2 > _nrsv) { return false; } nrsv2 = _nrsv; pd2 = this->_data; } }
      const _t_size ncp = _t_size(n2 < this->_n ? n2 : this->_n);
      if (b_copy && ncp && pd2 != this->_data) { std::memcpy(pd2, this->_data, ncp * sizeof(T)); }
      if (px && n2 > this->_n) { for (_t_size i0 = ncp; i0 < _t_size(n2); ++i0) { std::memcpy(pd2 + i0, px, sizeof(T)); } }
      if (pd2 != this->_data) { if (this->_data) { this->_free(this->_data); } this->_data = pd2; _nrsv = nrsv2; }
      this->_n = n2;
      return true;
    }

      // The function sets new number of reserved elements to max(n(), nrsv2).
      //    If this requires array reallocation, the existing elements are copied into the new array.
      //    (New elements are filled with zero bytes.)
      // NOTE If new reserve is within the current reserve,
      //    the function returns true (success) even if failed to shrink the storage.
    bool reserve(_s_ll nrsv2) throw()
    {
      if (nrsv2 < 0) { return false; }
      if (nrsv2 < this->_n) { nrsv2 = this->_n; }
      if (nrsv2 == _nrsv) { return true; }
      T* pd2 = this->_data;
      if (nrsv2 == 0) { pd2 = 0; }
        else { pd2 = (T*)this->_alloc(nrsv2 * sizeof(T)); if (!pd2) { if (nrsv2 > _nrsv) { return false; } nrsv2 = _nrsv; pd2 = this->_data; } }
      const _t_size ncp = _t_size(this->_n);
      if (ncp > 0 && pd2 != this->_data) { std::memcpy(pd2, this->_data, ncp * sizeof(T)); }
      if (pd2 != this->_data) { if (this->_data) { this->_free(this->_data); } this->_data = pd2; _nrsv = nrsv2; }
      return true;
    }

      // Ensure resirving n() + 1 element. Set element at n()+1 to T().
    bool ensure_cstr() throw() { if (nrsv() <= n()) { if (!reserve(n() + 1)) { return false; } } pd()[n()] = T(); return true; }

      // Appending, similar to basic_string operator +=.
      //  Keeps reserved space >= n() + new elements + 1.
      //  The after-end element is kept = bytewise 0.
      //  const t_value* is interpreted as a C-string.
      // If n() + new elements + 1 is within the current reserve, the functions always succeed.
    bool append_1(const t_value& x) throw()
    {
      const _s_ll nr = this->_n + 2;
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n; *p = x; this->_n += 1; p += 1; *p = T();
      return true;
    }
    bool append_n(const t_value* px, _s_ll n) throw()
    {
      if (n > 0 && !px) { return false; }
      if (n < 0) { n = 0; }
      const _s_ll nr = this->_n + n + 1;
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n;
      if (n > 0) { std::memcpy(p, px, size_t(n) * sizeof(T)); }
      this->_n += n; p += n; *p = T();
      return true;
    }
    bool append_nx(_s_ll n, const t_value& x) throw()
    {
      if (n < 0) { n = 0; }
      const _s_ll nr = this->_n + n + 1;
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n;
      T* p2 = p + n;
      if (sizeof(T) <= 8) { _s_ll z = 0; *(T*)&z = x; if (z == 0) { std::memset(p, 0, size_t(n) * sizeof(T)); *p2 = T(); this->_n += n; return true; } }
      while (p < p2) { *p++ = x; } *p2 = T(); this->_n += n;
      return true;
    }
      // append_z: appends px[i] until meets z. (For t_value == char, z == 0 this is equivalent to appending C string.)
    bool append_z(const t_value* px, const t_value& z = t_value()) throw()
    {
      _s_ll n = 0;
      if (px) { const t_value* px2 = px; while (*px2 != z) { ++px2; } n = _s_ll(px2 - px); }
      return append_n(px, n);
    }
    bool append(const arrayref_t<t_value>& x) throw() { return append_n(x.pd(), x.n()); }


    struct exc_carray_r_oppluseq : std::exception { const char* what() const throw() { return "carray_r_t:op+="; } };

      // operator+= succeeds if:
      //    a) n() + number of appended elements in x or px is within the current reserve.
      //    b) the automatic reserving during appending succeeded.
      // On failure, exc_carray_r_oppluseq is generated.
      //    The array changes are reverted, except not returning the additionally pre-reserved space.
    void operator+=(const t_value& x) { if (!append_1(x)) { throw exc_carray_r_oppluseq(); } }
    void operator+=(const arrayref_t<t_value>& x) { if (!append(x)) { throw exc_carray_r_oppluseq(); } }
    void operator+=(const t_value* px) { if (!append_z(px)) { throw exc_carray_r_oppluseq(); } }



    template<bool _ne> inline bool operator==(const carray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const cpparray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const cpparray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }


    const carray_t<t_value, no_exc_asg>& _base() const { return *this; }
  private:
    _s_ll _nrsv;
  };



    // This may be specialized to enable arrayref_t construction from arbitrary storage with contiguous elements.
  template<class Q, class _ = __vecm_tu_selector>
  struct _arrayref_adapter_t
    { static void* Fp(const Q& x) { if (sizeof(x)) {} return 0; } static _s_ll Fn(const Q& x) { if (sizeof(x)) {} return 0; } };

    // Weak reference to an array of elements in memory.
    //  The number of referenced elements is specified when the reference object is created.
  template<class T>
  struct arrayref_t
  {
    typedef T t_value;
    typedef size_t t_size;
    typedef arrayref_t t_a;

    inline _s_ll n() const throw()  { return _n; }

    inline const t_value* pd() const throw() { return _data; }
    inline t_value* pd() throw() { return _data; }

    inline const t_value& operator[] (_s_ll i) const throw() { return _data[t_size(i)]; }
    inline t_value& operator[] (_s_ll i) throw() { return _data[t_size(i)]; }

    arrayref_t() throw() { __pad1 = 0; unlink(); }
    ~arrayref_t() throw() { unlink(); }

    bool link(const t_value* px, _s_ll n) throw() { _n = n; if (_n < 0 || (_n > 0 && !px)) { _n = 0; _data = 0; return false; } _data = const_cast<T*>(px); return true; }
    void unlink() throw() { _n = 0;_data = 0; }

    arrayref_t(const t_value* px) throw() { __pad1 = 0; link(0, 0); if (px) { while (*px++ != T()) { ++_n; } } }
    arrayref_t(const t_value* px, _s_ll n) throw() { __pad1 = 0; link(px, n); }
    arrayref_t(const arrayref_t& x) throw() : _n(x._n) { __pad1 = 0; _data = x._data; }
    template<bool _ne> arrayref_t(const carray_t<t_value, _ne>& x) throw() : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    template<bool _ne> arrayref_t(const cpparray_t<t_value, _ne>& x) throw() : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    template<bool _ne> arrayref_t(const carray_r_t<t_value, _ne>& x) throw() : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    arrayref_t(const std::basic_string<t_value>& x) throw() { __pad1 = 0; *this = x; }
    template<class Q> explicit arrayref_t(const Q& x __bmdx_noarg) { __pad1 = 0; _data = (T*)_arrayref_adapter_t<T>::Fp(x); _n = _arrayref_adapter_t<T>::Fn(x); }

    arrayref_t& operator=(const arrayref_t& x) throw() { _data = x._data; _n = x._n; return *this; }
    template<bool _ne> arrayref_t& operator=(const carray_t<t_value, _ne>& x) throw() { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    template<bool _ne> arrayref_t& operator=(const cpparray_t<t_value, _ne>& x) throw() { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    template<bool _ne> arrayref_t& operator=(const carray_r_t<t_value, _ne>& x) throw() { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    arrayref_t& operator=(const std::basic_string<t_value>& x) throw() { _n = _s_ll(x.size()); if (_n <= 0) { _n = 0; _data = 0; return *this; } _data = const_cast<T*>(&x[0]); return *this; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) throw()
    {
      t_size i0 = 0; t_size nf(0); while (i0 < t_size(_n)) { try { for (; i0 < t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) throw()
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      t_size _i0 = t_size(i0); const t_size _i2 = t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const t_size nset = _i2 - _i0;
      t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

      // Element-by-element equality check.
    inline bool is_eq(const arrayref_t<t_value>& x) const { if (this->_n != x._n) { return false; } if (this->_data == x._data) { return true; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }

    inline void swap(arrayref_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; std::memcpy(x, &src, sizeof(t_a)); std::memcpy(&src, this, sizeof(t_a)); std::memcpy(this, x, sizeof(t_a)); }

    inline bool operator==(const arrayref_t<t_value>& x) const { return this->is_eq(x); }
    inline bool operator!=(const arrayref_t<t_value>& x) const { return !this->is_eq(x); }
  private:
    _s_ll _n; union { T* _data; _s_ll __pad1; };
  };
}








#ifdef _bmdxpl_Wnds
namespace bmdx
{
  #ifndef __bmdx__clock_ms
    #define __bmdx__clock_ms
    static double clock_ms()
    {
      static double _msclk_freq(1.); static bool _msclk_is_init(false);
      if (!_msclk_is_init) { LARGE_INTEGER f; if (QueryPerformanceFrequency(&f)) { double v[2] = { 0, 0 }; v[0] = (_s_ll(*(DWORD*)&f) + (_s_ll(*(LONG*)((DWORD*)&f + 1)) << 32)) / 1000.; _msclk_freq = v[0]; _msclk_is_init = true; } }
      LARGE_INTEGER cnt;
      if (_msclk_is_init && QueryPerformanceCounter(&cnt)) { _s_ll x = _s_ll(*(DWORD*)&cnt) + (_s_ll(*(LONG*)((DWORD*)&cnt + 1)) << 32); return x / _msclk_freq; }
        else { return GetTickCount(); }
    }
  #endif

    // NOTE avg. max. sleep duration error depends on thread priority.
    //  On overloaded CPU, order of 160..40 ms for normal..time-critical priority.
    //  In normal conditions, order of 1 mcs.
  static void sleep_mcs(_s_ll t)
  {
    if (t < 0) { return; }
    if (t == 0) { SleepEx(0, 1); return; }
    static _s_ll _tlagmcs(1000); // (mcs) max. detected lag of SleepEx call (real sleep time minus required)
    double ft0 = clock_ms();
    if (t - _tlagmcs < 1000) { do { SleepEx(0, 1); } while (clock_ms() - ft0 < t / 1000.); return; }
    _s_ll t1 =  (t - _tlagmcs) / 1000; // in milliseconds
    SleepEx(DWORD(t1), 1);
    ft0 = clock_ms() - ft0;
    _s_ll t2 = _s_ll(ft0 * 1000);
    _s_ll t3 = t2 - t1 * 1000;
    if (t3 > _tlagmcs) { _tlagmcs = t3; }
    t3 = t - t2;
    if (t3 > 0) { ft0 = clock_ms(); do { SleepEx(0, 1); } while (clock_ms() - ft0 < t3 / 1000.); }
  }


typedef HANDLE _t_threadctl_nat_handle;
static void _threadctl_thproc_impl(void* _p) throw();
static DWORD WINAPI _threadctl_thproc(LPVOID _p) throw()  { _threadctl_thproc_impl(_p); return 1; }
  // NOTE this object may be implemented differently (except the 1st member) in different binary modules.
template<class T> struct _critsec_data0_t
{
  typedef void* (*Psm)(_s_long ind);
  Psm psm; LONG cnt; LONG cnt2; DWORD tid;
  struct t_flags { _s_long _tms; char _bl; char __pad2, __pad3, __pad4; };
};
  // NOTE this object may not be relocated during lifetime.
  // NOTE this object may be implemented differently (except the 1st 9 members) in different binary modules.
struct _threadctl_ctx_data
{
  typedef void* (*Psm)(_s_long ind);

  Psm pthsm;
  void* pctxbase;
  void* pthdata;
  mutable volatile _s_long bs;
  volatile _s_long in_ctl;
  volatile char in_thread, b_own, __pad1, __pad2;
  _t_threadctl_nat_handle th;
  _critsec_data0_t<_threadctl_ctx_data> csd;
};
struct _threadctl_tid_data
{
  typedef void* (*Psm)(_s_long ind);
  typedef DWORD t_native_tid;

  union { Psm psm; _s_ll __pad1; };
  _s_ll stg1, stg2;
};
template<class T, class _ = __vecm_tu_selector> struct _critsec_tu_static_t
{
  static _critsec_data0_t<T> dat;
  static void csdata_init(_critsec_data0_t<T>* p) throw() { if (p) { p->psm = sm; p->cnt = 0; p->cnt2 = 0; p->tid = 0; } }
  static void csdata_deinit(_critsec_data0_t<T>*) throw() { }
  static void cs_init(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags, _s_ll check_period_mcs, _s_long timeout_ms) throw()
  {
    if (!pflags) { return; }
    pflags->_tms = timeout_ms; pflags->_bl = 0;
    if (check_period_mcs < 0) { return; }
    if (!_p) { return; }

    if (_p->tid != GetCurrentThreadId())
    {
      double t0(0.);
      if (timeout_ms >= 0) { t0 = clock_ms(); }
      while (1)
      {
        if (_p->cnt == 0)
        {
          if (InterlockedIncrement(&_p->cnt) == 1) { break; }
          InterlockedDecrement(&_p->cnt);
        }
        if (timeout_ms >= 0) { double dt = clock_ms() - t0; if (dt < 0 || dt >= double(timeout_ms)) { return; } }
        sleep_mcs(check_period_mcs);
      }
    }
    _p->tid = GetCurrentThreadId(); ++_p->cnt2; pflags->_bl = 1;
  }
  static void cs_deinit(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags) throw()
  {
    if (!pflags) { return; }
    if (!!pflags->_bl && _p) { if (--_p->cnt2 == 0) { _p->tid = 0; InterlockedDecrement(&_p->cnt); } pflags->_bl = 0; }
  }
  static _critsec_data0_t<T>* cs_pdefcsd() throw() { return &_critsec_tu_static_t<T>::dat; }

  static void* sm(_s_long ind)
  {
    enum { size = 6 };
    static void* smt[size] =
    {
      &dat, // 0
      (void*)csdata_init, // 1
      (void*)csdata_deinit, // 2
      (void*)cs_init, // 3
      (void*)cs_deinit, // 4
      (void*)cs_pdefcsd, // 5
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }
};

template<class _ = __vecm_tu_selector> struct _threadctl_tu_static_t
{
  struct _threadctl_thproc_cleanup;
  static void _tid_set(_threadctl_tid_data& x, const _threadctl_tid_data::t_native_tid& nat_id0) throw()
  {
    _threadctl_tid_data::t_native_tid nat_id = nat_id0;
    x.__pad1 = 0;
    x.psm = sm;
    char* p = (char*)&x.stg1;
    const char* p0 = (const char*)&nat_id;
    x.stg1 = 1; bool b = *p == 1; x.stg1 = 0;
    x.stg2 = 0;
    const int n = sizeof(nat_id) <= 16 ? sizeof(nat_id) : 16;
    if (b || n >= 8) { std::memcpy(p, p0, n); }
      else { std::memcpy(p + 8 - n, p0, n); }
  }
  static _s_long tid_is_eq(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2) throw()
  {
    if (!(p_my && p2)) { return p_my == p2; }
    if (p_my == p2) { return 1; }
    if (p_my->stg1 == 0 && p_my->stg2 == 0) { return p2->stg1 == 0 && p2->stg2 == 0; }
    if (p2->psm != sm)
    {
      if (!p2->psm) { return 0; }
      try {
        _s_ll t_my = (_s_ll)((char*)sm(16) - (char*)0); _s_ll t = (_s_ll)((char*)p2->psm(16) - (char*)0); _s_ll s = (_s_ll)((char*)p2->psm(17) - (char*)0);
        if (t != t_my) { return 0; } if (s != sizeof(_threadctl_tid_data::t_native_tid)) { return 0; }
      } catch (...) { return 0; }
    }
    return p_my->stg1 == p2->stg1 && p_my->stg2 == p2->stg2;
  }

    // ret. 2..0
  static _s_long th_state(_threadctl_ctx_data* p) throw()
  {
    if (!p) { return 0; }
    DWORD c(0); GetExitCodeThread(p->th, &c);
    return c == STILL_ACTIVE ? 2 : 1;
  }
    // ret. 1, 0
  static _s_long th_start(_threadctl_ctx_data* p, _threadctl_tid_data* pret_tidobj) throw()
  {
    if (!p) { return 0; }
    if (p->in_ctl > 0 || p->in_thread) { return false; }
    _threadctl_tid_data::t_native_tid tid1;
    do
    {
      p->in_ctl = 1; p->in_thread = 1;
      try { p->th = CreateThread(0, 0, &_threadctl_thproc, p, 0, &tid1); } catch (...) {}
      if (!p->th) { p->in_ctl = 0; p->in_thread = 0; break; }

      if (pret_tidobj) { _tid_set(*pret_tidobj, tid1); }
      return 1;
    } while (0);
    return 0;
  }
    // ret. 1, 0
  static _s_long th_set_priority(_threadctl_ctx_data* p, _s_long prio) throw()
  {
    if (!prio) { return 0; }
    switch (prio)
    {
      case 4: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_NORMAL)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 5: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_ABOVE_NORMAL)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 6: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_HIGHEST)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 7: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_TIME_CRITICAL)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 3: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_BELOW_NORMAL)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 2: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_LOWEST)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      case 1: { _s_long b = !!(SetThreadPriority(p->th, THREAD_PRIORITY_IDLE)); SetThreadPriorityBoost(p->th, FALSE); return b; }
      default: return 0;
    }
  }
    //  timeout_ms:
    //    >= 0 - set b_stop() flag if it was 0, wait for thread exiting up to timeout, release thread context.
    //    -1 or <= -4: set b_stop() flag if it was 0, then immediately release thread context (it will be destroyed when the last reference has gone).
    //    -2: immediately release thread context, but set b_stop() flag only if the current context reference is the last one from side of threadctl.
    //    -3: immediately release thread context, do not modify b_stop() flag.
    //  Returns:
    //    2 - thread has exited normally. threadctl is cleared.
    //    1 - thread already exited. threadctl is cleared.
    //    0 - thread was not running (threadctl is empty).
    //    -1 - detached. threadctl is cleared.
    //    -2 - still running (timeout). threadctl continues to reference the thread.
  static _s_long th_stop(_threadctl_ctx_data* p, _s_long timeout_ms) throw()
  {
    if (!p) { return 0; }
    if (!p->bs && (timeout_ms >= -1 || timeout_ms <= -4 || (timeout_ms == -2 && p->in_ctl <= 1))) { p->bs = 1; }
    if (!p->in_thread) { th_ctx_release(p, 2); return 1; }
    if (timeout_ms < 0) { th_ctx_release(p, 2); return -1; }
    _s_long t = timeout_ms;
    while (t >= 0) { sleep_mcs(t > 10 ? 10000 : t * 1000); if (!p->in_thread) { th_ctx_release(p, 2); return 2; } t -= 10; }
    return -2;
  }

  struct exc1 : std::exception {};
  static VOID NTAPI _th_apc_cleanup(ULONG_PTR x)
  {
    _threadctl_ctx_data* p = (_threadctl_ctx_data*)x;
    th_ctx_release(p, 1); // assumes that thread context data is kept invariant during APC (i.e. may be safely destroyed while thread is sleeping)
    ExitThread(1);
  }
    //    2 - terminated. threadctl is cleared.
    //    1 - thread already exited. threadctl is cleared.
    //    0 - thread was not running (threadctl is empty).
    //    -3 - failure (flags specify no valid method, or by other reason).
  static _s_long th_terminate(_threadctl_ctx_data* p, _s_long flags) throw()
  {
    if (!p) { return 0; }
    if (!p->bs && !(flags & 0x1000)) { p->bs = 1; }
    if (!p->in_thread) { th_ctx_release(p, 2); return 1; }

    if ((flags & 1) && QueueUserAPC(_th_apc_cleanup, p->th, (ULONG_PTR)p)) { }
      else if (flags & 2) { TerminateThread(p->th, 1); }
      else { return -3; }
    th_ctx_release(p, 2);
    return 2;
  }
  static _s_long th_in_ctl_incr(_threadctl_ctx_data* p) throw();

  static _s_long th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) throw();
  static _s_long th_ctx_release(_threadctl_ctx_data* p, _s_long what) throw();
  static _s_long _th_ctx_rel_handle(_threadctl_ctx_data* p) throw() { if (!p) { return 1; } return CloseHandle(p->th) != 0; }

  static void* sm(_s_long ind)
  {
    enum { size = 20 };
    static void* smt[size] =
    {
      0, // 0
      0, // 1
      0, // 2
      0, // 3
      0, // 4
      0, // 5
      (void*)th_ctx_init, // 6
      (void*)th_ctx_release, // 7
      (void*)th_state, // 8
      (void*)th_start, // 9
      (void*)th_set_priority, // 10
      (void*)th_stop, // 11
      (void*)th_terminate, // 12
      (void*)th_in_ctl_incr, // 13
      0, // 14
      0, // 15
      (void*)((char*)0 + -942001156), // 16 // cmti_base_t<int,2017,7,11,19,4>::ind()
      (void*)((char*)0 + sizeof(_threadctl_tid_data::t_native_tid)), // 17
      0, // 18
      (void*)tid_is_eq, // 19
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }
};
template<class T, class _> _critsec_data0_t<T> _critsec_tu_static_t<T, _>::dat = { sm, 0, 0, 0 };



}
#endif

#ifdef _bmdxpl_Psx

#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/wait.h>

#ifndef __ANDROID__
  #include <spawn.h>
#endif

#if __APPLE__ && __MACH__
  #include <mach/mach_time.h>
  #include <TargetConditionals.h>
#endif
extern char **environ;
namespace bmdx
{
  #ifndef __bmdx__clock_ms
    #define __bmdx__clock_ms

    static double clock_ms()
    {
      #if defined(__SUNPRO_CC)
        return gethrtime() * 1.e-6;
      #elif __APPLE__ && __MACH__
        static mach_timebase_info_data_t rate = { 0, 1 }; if (!rate.numer) { mach_timebase_info(&rate); } // mach_timebase_info always succeeds
        return mach_absolute_time() * 1.e-6 * rate.numer / rate.denom;
      #else
        #ifdef CLOCK_MONOTONIC_RAW
          static const clockid_t t_timer = CLOCK_MONOTONIC_RAW;
        #elif defined(CLOCK_MONOTONIC_FAST)
          static const clockid_t t_timer = CLOCK_MONOTONIC_FAST;
        #else
          static const clockid_t t_timer = CLOCK_MONOTONIC;
        #endif
        static bool bfall(false); if (bfall) { return std::clock() * (1000. / CLOCKS_PER_SEC); }
        timespec t; if (clock_gettime(t_timer, &t) !=0) { bfall = true; return clock_ms(); }
        return double(t.tv_sec) * 1000. + double(t.tv_nsec) * 1.e-6;
      #endif
    }
  #endif

  static void sleep_mcs(_s_ll t)
  {
    #ifdef __FreeBSD__
      if (t < 0) { return; }
      timespec ts; ts.tv_sec = 0; ts.tv_nsec = 0;
      if (t == 0) { nanosleep(&ts, 0); return; }
      const double ft0 = clock_ms();
      nanosleep(&ts, 0);
      while (1)
      {
        _s_ll dt = t - _s_ll((clock_ms() - ft0) * 1000);
        if (dt <= 0) { return; }
        ts.tv_sec = dt / 1000000; ts.tv_nsec = (dt % 1000000) * 1000;
// NOTE Frequent calls with small time hang/crash.
//        if (dt >= 20) { ts.tv_sec = dt / 1000000; ts.tv_nsec = (dt % 1000000) * 1000; }
//          else { ts.tv_sec = 0; ts.tv_nsec = 100; }
        nanosleep(&ts, 0);
      }
    #else
      if (t < 0) { return; }
      timespec ts;
      if (t == 0) { ts.tv_sec = 0; ts.tv_nsec = 0; nanosleep(&ts, 0); return; }
      double ft0 = clock_ms();
      static _s_ll _tlagmcs(1); // (mcs) max. detected lag of nanosleep call (real sleep time minus required)
      if (t - _tlagmcs < 10) { ts.tv_sec = 0; ts.tv_nsec = _s_long(t >= 10000000 ?  500000000 : t * 50); do { nanosleep(&ts, 0); } while (clock_ms() - ft0 < t / 1000.); return; }
      _s_ll t1s =  (t - _tlagmcs) / 1000000; // in seconds
      ts.tv_sec = time_t(t1s); ts.tv_nsec = 0;
      nanosleep(&ts, 0);
      ft0 = clock_ms() - ft0;
      _s_ll t2 = _s_ll(ft0 * 1000);
      _s_ll t3 = t2 - t1s * 1000000;
      if (t3 > _tlagmcs) { _tlagmcs = t3; }
      t3 = t - t2;
      if (t3 > 0) { ft0 = clock_ms(); ts.tv_sec = 0; ts.tv_nsec = _s_long(t3 >= 10000000 ?  500000000 : t3 * 50); do { nanosleep(&ts, 0); } while (clock_ms() - ft0 < t3 / 1000.); }
    #endif
  }




typedef pthread_t _t_threadctl_nat_handle;
static void _threadctl_thproc_impl(void* _p) throw();
extern "C" typedef void* (*PF_threadctl_thproc)(void* _p);
static void* _threadctl_thproc(void* _p) throw() { _threadctl_thproc_impl(_p); return 0; }
static pthread_key_t* _threadctl_psjstg()
{
  static char init(0);
  union _u { pthread_key_t x; _s_ll __1; };
  static _u x;
  if (!init)
  {
    std::memset(&x, 0, sizeof(x));
    static const char* k = "__BMDX_threadctl_psjstg";
    const char* pe = getenv(k);
    if (pe)
    {
      _u x2; std::memset(&x2, 0, sizeof(x2));
      try { x2.__1 = bmdx_str::conv::strn2i(pe, -1, 0, 0); }
        catch (...) { pe = 0; }
      if (pe) { std::memcpy(&x, &x2, sizeof(_u)); }
    }
    if (!pe)
    {
      if (0 != pthread_key_create(&x.x, 0)) { return 0; }
      bmdx_str::flstr_t<30> s((x.__1));
      setenv(k, s.c_str(), 1);
    }
    init = 1;
  }
  return init ? &x.x : 0;
}
namespace { struct _threadctl_psjstg_init { _threadctl_psjstg_init() { _threadctl_psjstg(); } } __threadctl_psjstg_init_inst; }




  // NOTE this object may be implemented differently (except the 1st member) in different binary modules.
template<class T> struct _critsec_data0_t
{
  typedef void* (*Psm)(_s_long ind);
  Psm psm;
  _s_long _kind; char _b_lk1; char __pad1, __pad2, __pad3;
  pthread_mutex_t _m;
  _s_ll _tid[(sizeof(pthread_t) + 7) / 8];
  struct t_flags { _s_long _tms; char _bl; char _bignore; char _brec; char __pad4; };
};
template<class T> struct _critsec_data1_t
{
  _critsec_data0_t<T> _trecur;
  _critsec_data0_t<T> _tglobal;
  _s_long _cnt;
};
  // NOTE this object may not be relocated during lifetime.
  // NOTE this object may be implemented differently (except the 1st 9 members) in different binary modules.
struct _threadctl_ctx_data
{
  typedef void* (*Psm)(_s_long ind);

  Psm pthsm;
  void* pctxbase;
  void* pthdata;
  mutable volatile _s_long bs;
  volatile _s_long in_ctl;
  volatile char in_thread, b_own, __pad1, __pad2;
  _t_threadctl_nat_handle th;
  _critsec_data0_t<_threadctl_ctx_data> csd;
  struct _sjctx
  {
    union { _s_ll __1; sigjmp_buf buf; };
    char state; // thread init. state (0 - before user proc. call, 1 - right after setjmp and calling user proc., 2 - right before thread ctx. release)
    char trq; // 1 == termination request (checked by SIGUSR1 handler)
  };
  _sjctx sjctx;
};
struct _threadctl_tid_data
{
  typedef void* (*Psm)(_s_long ind);
  typedef pthread_t t_native_tid;

  union { Psm psm; _s_ll __pad1; };
  _s_ll stg1, stg2; // pthread_t tid;
};
template<class T, class _ = __vecm_tu_selector> struct _critsec_tu_static_t
{
  static _critsec_data1_t<T> dat;
  static void csdata_init(_critsec_data0_t<T>* p) throw()
  {
    if (!p) { return; }
    p->psm = sm;
    p->_kind = 1; pthread_mutex_t _m_temp = PTHREAD_MUTEX_INITIALIZER; p->_m = _m_temp; p->_b_lk1 = 0;
    bool b(true);
    pthread_mutexattr_t a;
      if (b && pthread_mutexattr_init(&a) != 0) { b = false; }
      if (b && pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE) != 0) { pthread_mutexattr_destroy(&a); b = false; }
      if (b && pthread_mutex_init(&p->_m, &a) != 0) { pthread_mutexattr_destroy(&a); b = false; }
    if (b) { p->_kind = 2; } else { pthread_mutex_t _m_temp = PTHREAD_MUTEX_INITIALIZER; p->_m = _m_temp; }
  }
  static void csdata_deinit(_critsec_data0_t<T>* p) throw()
  {
    if (!p) { return; }
    if (p->_kind == 2) { pthread_mutex_destroy(&p->_m); }
  }
  static void cs_init(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags, _s_ll check_period_mcs, _s_long timeout_ms) throw()
  {
    if (!pflags) { return; }
    pflags->_tms = timeout_ms; pflags->_bl = 0;  pflags->_bignore = 0; pflags->_bl = 0; pflags->_brec = 0;
    if (check_period_mcs < 0) { pflags->_bignore = 1; return; }
    if (!(_p && _p->psm)) { return; }
    _critsec_data1_t<T>* _pg_dat = (_critsec_data1_t<T>*)_p->psm(0); if (!_pg_dat) { return; }
    if (_p == &_pg_dat->_trecur) { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (++_pg_dat->_cnt == 1) { csdata_init(&_pg_dat->_trecur); } pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    if (_p->_kind == 1)  { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (_p->_b_lk1 && pthread_equal((*(pthread_t*)&_p->_tid), pthread_self()) == 0) { pflags->_bl = 1; pflags->_brec = 1; } pthread_mutex_unlock(&_pg_dat->_tglobal._m); if (pflags->_brec) { return; } }
    double t0(0.); bool b(false);
    while (pthread_mutex_trylock(&_p->_m) != 0)
    {
      if (timeout_ms >= 0) { if (!b) { t0 = clock_ms(); b = true; } double dt = clock_ms() - t0; if (dt < 0 || dt >= double(timeout_ms)) { return; } }
      sleep_mcs(check_period_mcs);
    }
    pflags->_bl = 1; if (_p->_kind == 1) { new (&_p->_tid) pthread_t(pthread_self()); _p->_b_lk1 = 1; }
  }
  static void cs_deinit(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags) throw()
  {
    if (!pflags) { return; }
    if (!!pflags->_bignore || !_p) { return; }
    _critsec_data1_t<T>* _pg_dat = (_critsec_data1_t<T>*)_p->psm(0); if (!_pg_dat) { return; }
    if (_p->_kind == 1) { if (pflags->_brec) { return; } pthread_mutex_lock(&_pg_dat->_tglobal._m); ((pthread_t*)&_p->_tid)->~pthread_t(); _p->_b_lk1 = 0; pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    if (pflags->_bl) { pthread_mutex_unlock(&_p->_m); pflags->_bl = 0; }
    if (_p == &_pg_dat->_trecur) { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (--_pg_dat->_cnt == 0) { csdata_deinit(&_pg_dat->_trecur); } pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    pflags->_bignore = 1;
  }
  static _critsec_data0_t<T>* cs_pdefcsd() throw() { return &_critsec_tu_static_t<T>::dat._trecur; }

  static void* sm(_s_long ind)
  {
    enum { size = 6 };
    static void* smt[size] =
    {
      &dat, // 0
      (void*)csdata_init, // 1
      (void*)csdata_deinit, // 2
      (void*)cs_init, // 3
      (void*)cs_deinit, // 4
      (void*)cs_pdefcsd, // 5
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }
};

template<class _ = __vecm_tu_selector> struct _threadctl_tu_static_t
{
  struct _threadctl_thproc_cleanup;
  static void _tid_set(_threadctl_tid_data& x, const _threadctl_tid_data::t_native_tid& nat_id0) throw()
  {
    _threadctl_tid_data::t_native_tid nat_id = nat_id0;
    x.__pad1 = 0;
    x.psm = sm;
    char* p = (char*)&x.stg1;
    const char* p0 = (const char*)&nat_id;
    x.stg1 = 1; bool b = *p == 1; x.stg1 = 0;
    x.stg2 = 0;
    const int n = sizeof(nat_id) <= 16 ? sizeof(nat_id) : 16;
    if (b || n >= 8) { std::memcpy(p, p0, n); }
      else { std::memcpy(p + 8 - n, p0, n); }
  }
  static _s_long tid_is_eq(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2) throw()
  {
    if (!(p_my && p2)) { return p_my == p2; }
    if (p_my == p2) { return 1; }
    if (p_my->stg1 == 0 && p_my->stg2 == 0) { return p2->stg1 == 0 && p2->stg2 == 0; }
    const int n = sizeof(_threadctl_tid_data::t_native_tid);
    if (n > 16) { return 0; }
    if (p2->psm != sm)
    {
      if (!p2->psm) { return 0; }
      try {
        _s_ll t_my = (_s_ll)((char*)sm(16) - (char*)0); _s_ll t = (_s_ll)((char*)p2->psm(16) - (char*)0); _s_ll s = (_s_ll)((char*)p2->psm(17) - (char*)0);
        if (t != t_my) { return 0; } if (s != sizeof(_threadctl_tid_data::t_native_tid)) { return 0; }
      } catch (...) { return 0; }
    }

    _threadctl_tid_data::t_native_tid t1, t2;
    _s_ll x = 1; bool b = *(char*)&x == 1;
    if (b || n >= 8) { std::memcpy(&t1, &p_my->stg1, n); std::memcpy(&t2, &p_my->stg2, n); }
      else { std::memcpy(&t1, (char*)&p_my->stg1 + 8 - n, n); std::memcpy(&t2, (char*)&p_my->stg2 + 8 - n, n); }

    return pthread_equal(t1, t2) == 0;
  }


    // ret. 2..0
  static _s_long th_state(_threadctl_ctx_data* p) throw()
  {
    if (!p) { return 0; }
    return p->in_thread ? 2 : 1;
  }
    // ret. 1, 0
  static _s_long th_start(_threadctl_ctx_data* p, _threadctl_tid_data* pret_tidobj) throw()
  {
    if (!p) { return 0; }
    if (p->in_ctl > 0 || p->in_thread) { return false; }
    _threadctl_tid_data::t_native_tid tid1;
    do
    {
      std::memset(&tid1, 0, sizeof(tid1));
      std::memset(&p->th, 0, sizeof(p->th));
      pthread_attr_t a; pthread_attr_init(&a);
      pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
      int res(0);
      p->in_ctl = 1; p->in_thread = 1;
      res = pthread_create(&tid1, &a, (PF_threadctl_thproc)&_threadctl_thproc, p);
      if (res != 0) { p->in_ctl = 0; p->in_thread = 0; break; }
      p->th = tid1;
      if (pret_tidobj) { _tid_set(*pret_tidobj, tid1); }
      return 1;
    } while (0);
    return 0;
  }
    // ret. 1, 0
  static _s_long th_set_priority(_threadctl_ctx_data* p, _s_long prio) throw()
  {
    if (!p) { return 0; }
    struct __local
    {
        // level: 0 - min, 1 - avg, 2 - high, 3 - highest.
      bool set_sp(int pl, sched_param& sp, int level)
      {
        int sp1 = sched_get_priority_min(pl); int sp2 = sched_get_priority_max(pl); if (sp1 < 0 || sp2 < 0) { return false; }
        int spx;
        switch (level)
        {
          case 3: sp.sched_priority = sp2; break;
          case 2: spx = sp2 - 1; if (spx < sp1) { spx = sp1; } sp.sched_priority = spx; break;
          case 1: spx = (sp2 + sp1 + 1) / 2; if (spx > sp2 - 1) { spx = sp2 - 1; } if (spx < sp1) { spx = sp1; } sp.sched_priority = spx; break;
          default: sp.sched_priority = sp1; break;
        }
        return true;
      }
    };

    int pl; struct sched_param sp; int lev;
    if (pthread_getschedparam(p->th, &pl, &sp) != 0) { return 0; }
    int pl_low = SCHED_OTHER;
    #ifdef SCHED_IDLE
      pl_low = SCHED_IDLE;
    #endif

    switch (prio)
    {
      case 4: { pl = SCHED_OTHER; lev = 3; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 5: { pl = SCHED_RR; lev = 2; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 6: { pl = SCHED_RR; lev = 3; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 7: { pl = SCHED_FIFO; lev = 3; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 1: { pl = pl_low; lev = 0; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 2: { pl = SCHED_OTHER; lev = 1; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      case 3: { pl = SCHED_OTHER; lev = 2; _s_long b = __local().set_sp(pl, sp, lev) && 0 == pthread_setschedparam(p->th, pl, &sp); return b; }
      default: return 0;
    }
  }
    //  timeout_ms:
    //    >= 0 - set b_stop() flag if it was 0, wait for thread exiting up to timeout, release thread context.
    //    -1 or <= -4: set b_stop() flag if it was 0, then immediately release thread context (it will be destroyed when the last reference has gone).
    //    -2: immediately release thread context, but set b_stop() flag only if the current context reference is the last one from side of threadctl.
    //    -3: immediately release thread context, do not modify b_stop() flag.
    //  Returns:
    //    2 - thread has exited normally. threadctl is cleared.
    //    1 - thread already exited. threadctl is cleared.
    //    0 - thread was not running (threadctl is empty).
    //    -1 - detached. threadctl is cleared.
    //    -2 - still running (timeout). threadctl continues to reference the thread.
  static _s_long th_stop(_threadctl_ctx_data* p, _s_long timeout_ms) throw()
  {
    if (!p) { return 0; }
    if (!p->bs && (timeout_ms >= -1 || timeout_ms <= -4 || (timeout_ms == -2 && p->in_ctl <= 1))) { p->bs = 1; }
    if (!p->in_thread) { _threadctl_tu_static_t<_>::th_ctx_release(p, 2); return 1; }
    if (timeout_ms < 0) { _threadctl_tu_static_t<_>::th_ctx_release(p, 2); return -1; }
    _s_long t = timeout_ms;
    while (t >= 0) { sleep_mcs(t > 10 ? 10000 : t * 1000); if (!p->in_thread) { th_ctx_release(p, 2); return 2; } t -= 10; }
    return -2;
  }

  static _s_long th_terminate(_threadctl_ctx_data* p, _s_long flags) throw();

  static _s_long th_in_ctl_incr(_threadctl_ctx_data* p) throw();

  static _s_long th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) throw();
  static _s_long th_ctx_release(_threadctl_ctx_data* p, _s_long what) throw();
  static _s_long _th_ctx_rel_handle(_threadctl_ctx_data*) throw() { return 1; }

  static void* sm(_s_long ind)
  {
    enum { size = 20 };
    static void* smt[size] =
    {
      0, // 0
      0, // 1
      0, // 2
      0, // 3
      0, // 4
      0, // 5
      (void*)th_ctx_init, // 6
      (void*)th_ctx_release, // 7
      (void*)th_state, // 8
      (void*)th_start, // 9
      (void*)th_set_priority, // 10
      (void*)th_stop, // 11
      (void*)th_terminate, // 12
      (void*)th_in_ctl_incr, // 13
      0, // 14
      0, // 15
      (void*)((char*)0 + -942002180), // 16 // cmti_base_t<int,2017,7,11,20,4>::ind()
      (void*)((char*)0 + sizeof(_threadctl_tid_data::t_native_tid)), // 17
      0, // 18
      (void*)tid_is_eq, // 19
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }
};
template<class T, class _> _critsec_data1_t<T> _critsec_tu_static_t<T, _>::dat =
{
  { sm, 1, 0, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, { 0 } },
  { sm, 1, 0, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, { 0 } },
  0
};


}

#endif

#ifndef __bmdx__clock_ms
  #define __bmdx__clock_ms
  static double clock_ms() { return std::clock() * (1000. / CLOCKS_PER_SEC); }
#endif



  // Crit. sec. and thread control wrappers.
  //    1. Wrapper code is platform-independent.
  //    2. Wrapper size is fixed.
  //    3. Wrapper object may be passed and used in all binary modules in the process.
namespace bmdx
{
    // NOTE 1
    //    critsec_t is working correctly both in main program and static init./deinit.
    // NOTE 2
    //    critsec_t may safely operate (lock, unlock) on crit. sec. data objects (_critsec_data0_t<T>), passed from other binary modules.
    //    The locking may be done in any module of the process.
    // NOTE 3
    //    pdefcsd() - default per-type crit. sec. data object. Its uniqueness is limited to
    //      a) parent binary module, or
    //      b) to several modules created with same compiler (compiler-dependent behavior).
    //    If you need to bind lock to particular type, unique in whole process,
    //      create global _critsec_data0_t<T> object and pass it as argument to critsec_t.
  template<class T> struct critsec_t
  {
      // check_period_mcs:
      //    >= 1: thread sleep duration between two lock tries. The number of lock tries is roughly (timeout_ms * 1000 / check_period_mcs).
      //    0: lock tries are made between sleep_mcs(0) (i.e. the thread only yields execution between lock tries, but does not sleep).
      //    < 0: the constructor exits immediately w/o any attempt of locking (use it to allow/disable locking with a condition).
      // timeout_ms: max. period of sleeping, during which the constructor tries to set a lock.
      //    >= 1: the constructor will sleep until the lock is set or timeout occurs.
      //    0:  the constructor tries to lock once, and exits anyway. b_locked() reflects the result (true if the lock has been acquired).
      //    any < 0:  wait until the lock is acquired. This may block for indefinite time, but when the constructor exits, b_locked() == true anyway.
      // pdata: csdata object or correctly initialized _critsec_data0_t.
      //    Allows for creating individual crit. sec. independent on T and default lock associated with T.
      //    NOTE pdata is weak-referenced, i.e. the crit. sec. must be valid until the last lock on it is destroyed.
    critsec_t(_s_ll check_period_mcs, _s_long timeout_ms, _critsec_data0_t<T>* pdata = 0 __bmdx_noarg) throw()
    {
      _p = pdata ? pdata : ff_mc().pdefcsd();
      _flags._tms = timeout_ms; _flags._bl = 0;
      if (!_p->psm) { return; }
      typedef void (*PF)(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags, _s_ll check_period_mcs, _s_long timeout_ms);
      try { PF f = (PF)_p->psm(3); if (f) { f(_p, &_flags, check_period_mcs, timeout_ms); } } catch (...) {}
    }

      // NOTE csdata object size may vary. Use it in local binary module only, or pass by pointer.
    struct csdata : _critsec_data0_t<T>
    {
      csdata(__bmdx_noarg1) throw() { _critsec_tu_static_t<T>::csdata_init(this); }
      ~csdata() throw() { if (this->psm) { typedef void (*PF)(_critsec_data0_t<T>*); try { PF f = (PF)this->psm(2); if (f) { f(this); } } catch (...) {} } }
    private: csdata(const csdata&); void operator=(const csdata&);
    };

      // Critical section data object, refrenced by the current lock.
      //  The returned pointer is non-zero, referring to the valid object,
      //  provided that critsec_t had been constructed correctly.
    _critsec_data0_t<T>* pcsd0() const { return _p; }
    csdata* pcsd() const { return static_cast<csdata*>(_p); }

    struct ff_mc
    {
        // Default csdata object, used by critsec_t for type T (per binary module or per compiler).
      inline csdata* pdefcsd() throw() { typedef _critsec_data0_t<T>* (*PF)(); PF f = (PF)_critsec_tu_static_t<T>::sm(5); return static_cast<csdata*>(f()); }
    };


      // Return value: a) true, b) false - lock tries were timed out (may occur ONLY on constructor timeout_ms >= 0).
    inline bool is_locked() const throw() { return !!_flags._bl; }
    inline operator bool() const throw() { return !!_flags._bl; }


    ~critsec_t() throw()
    {
      if (!(_p && _p->psm)) { return; }
      typedef void (*PF)(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags);
      try { PF f = (PF)_p->psm(4); if (f) { f(_p, &_flags); } } catch (...) {}
    }

    critsec_t(const critsec_t& x __bmdx_noarg) { new (this) critsec_t(x._flags._bl ? 10 : -1, x._flags._tms, x._p); }
    critsec_t& operator=(const critsec_t& x) throw(__bmdx_noargt1) { critsec_t cs3(x); bmdx_str::words::swap_bytes(*this, cs3); return *this; }

    private:
      _critsec_data0_t<T>* _p; typename _critsec_data0_t<T>::t_flags _flags;
  };


    // NOTE 1
    //    threadctl object may be safely passed and operated between binary modules.
    // NOTE 2
    //    threadctl may be copied, the copy references the same thread as original.
    // NOTE 3
    //    threadctl functions as such (including copying) are not thread-safe.
    //    To access threadctl concurrently, use a lock, associated with all involved threadctl objects.
  struct threadctl
  {

    struct ctx_base
    {
    protected:
      virtual void _thread_proc() = 0;

    public:
        // Thread proc. must check this flag and return when it's true.
      volatile _s_long& b_stop() const { return __dat.bs; }
        // Thread proc. must know the real type of data (T must match the real type).
      template<class T> T* pdata() const { return reinterpret_cast<T*>(__dat.pthdata); }


      void _set_b_own(char x) { __dat.b_own = x; } // should be set only if calling threadctl::start directly
      void _set_pthdata(void* p) { __dat.pthdata = p; } // should be set only if calling threadctl::start directly

      virtual ~ctx_base() {}
      ctx_base() { _threadctl_tu_static_t<>::th_ctx_init(&__dat, this, 0); }
      ctx_base(const ctx_base&) { _threadctl_tu_static_t<>::th_ctx_init(&__dat, this, 0); } // NOTE the new context does not copy anything from the source object
      _threadctl_ctx_data& _dat() { return __dat; }
    private:
      friend void _threadctl_thproc_impl(void* _p) throw();
      friend struct threadctl;
      _threadctl_ctx_data __dat;
    };

    template <class Data, class _ = __vecm_tu_selector> struct _data_container_tu_t { Data _x; _data_container_tu_t(const Data& x) : _x(x) {} };
    template <class Ctx, class Thdata, class _ = __vecm_tu_selector>
    struct ctx_wrapper : _data_container_tu_t<Thdata, _>, Ctx
    {
      ctx_wrapper(const Thdata& dat_) : _data_container_tu_t<Thdata, _>(dat_), Ctx()
      {
        ctx_base* pbase = this; // Ctx must inherit from threadctl::ctx_base
        _data_container_tu_t<Thdata>* pthdata = this;
        pbase->_set_pthdata(&pthdata->_x);
      }
      ctx_wrapper(const ctx_wrapper& x) : _data_container_tu_t<Thdata, _>(x), Ctx(x)
        { this->_set_pthdata(&this->_x); }
    };

    typedef _threadctl_tid_data::t_native_tid t_native_tid;
    struct tid
    {
      tid() throw()    { clear(); }
      ~tid() throw()    { }
      void clear() throw()    { _dat.psm = _threadctl_tu_static_t<>::sm; _dat.stg1 = 0; _dat.stg2 = 0; }

      tid(const t_native_tid& x_ __bmdx_noarg) throw()    { _threadctl_tu_static_t<>::_tid_set(_dat, x_); }
      tid(const tid& x_) throw()    { _dat = x_._dat; }
      tid& operator=(const t_native_tid& x_) throw()    { _threadctl_tu_static_t<>::_tid_set(_dat, x_); return *this; }
      tid& operator=(const tid& x_) throw()    { _dat = x_._dat; return *this; }

        // Comparison is not trivial.
      bool operator==(const tid& x_) const throw() { if (!_dat.psm) { return !x_._dat.psm; } if (!x_._dat.psm) { return false; } typedef _s_long (*Peq)(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2); try { Peq f_eq = (Peq)_dat.psm(19); if (!f_eq) { return false; } return !!f_eq(&_dat, &x_._dat); } catch (...) {} return false; }
      bool operator!=(const tid& x_) const throw() { return !this->operator==(x_); }

        // If tid object is valid in the current context, returns ptr. to native handle, otherwise 0.
      t_native_tid* pnat_id(__bmdx_noarg1) const throw()
      {
        if (!_dat.psm) { return 0; }
        const int n = sizeof(_threadctl_tid_data::t_native_tid);
        if (n > 16) { return 0; }
        try {
          _s_ll t_my = (_s_ll)((char*)_critsec_tu_static_t<tid>::sm(16) - (char*)0); _s_ll t = (_s_ll)((char*)_dat.psm(16) - (char*)0); _s_ll s = (_s_ll)((char*)_dat.psm(17) - (char*)0);
          if (t != t_my) { return 0; } if (s != n) { return 0; }
        } catch (...) { return 0; }
        _s_ll x = 1; bool b = *(char*)&x == 1;
        if (b || n >= 8) { return (t_native_tid*)&_dat.stg1; }
          else { return (t_native_tid*)((char*)&_dat.stg1 + 8 - n); }
      }

      bool is_null() const throw()    { return _dat.stg1 == 0 && _dat.stg2 == 0; }
      operator bool() const throw()    { return is_null(); }

        // Numeric value of tid. 0 if tid is null.
        //  The value is most suitable for displaying.
      _s_ll num_id() const throw()    { if (is_null()) { return 0; } _s_ll n = _dat.stg1 ^ _dat.stg2; if (!n) { n = _dat.stg1; } return n; }

    private: friend struct threadctl; _threadctl_tid_data _dat;
    };


    threadctl(__bmdx_noarg1) throw() : _tid() { __pad1 = 0; _pctx = 0; }
    ~threadctl() throw() { clear(); }

      // If threadctl fails to increment ref. cnt, the new object will have state() == 0 and th_id().is_null() == true.
      //  Normally, this occurs only if x is empty.
    threadctl(const threadctl& x __bmdx_noarg) throw()
      {
        __pad1 = 0; _pctx = 0; if (!x._pctx) { return; }
        typedef _s_long (*Pincr)(_threadctl_ctx_data* p); if (!x._pctx->__dat.pthsm) { return; }
        try { Pincr f_incr = (Pincr)x._pctx->__dat.pthsm(13); if (!f_incr) { return; } f_incr(&x._pctx->__dat); } catch (...) { return; }
        _pctx = x._pctx;
        _tid = x._tid;
      }
    threadctl& operator=(const threadctl& x) throw(__bmdx_noargt1)    { threadctl x3(x); swap(x3); return *this; }

      // !is_null() when thread is running.
    const tid& th_id() const throw() { return _tid; }


      // 2 - context is referenced, thread is running (volatile state!).
      // 1 - context is referenced, thread has already exited.
      // 0 - no context, thread state is unknown (e.g. the initial value, or threadctl has been cleared).
    _s_long state() const throw()
    {
      if (!_pctx) { return 0; }
      typedef _s_long (*Pstate)(_threadctl_ctx_data* p); Pstate f_state = (Pstate)_pctx->__dat.pthsm(8);
      if (!f_state) { return 0; }
      try { return f_state(&_pctx->__dat); } catch (...) {}
      return 0;
    }
    operator bool() const throw() { return state() == 2; }

    ctx_base* pctx() const throw() { return _pctx; }

      // Starts a new thread, using the given context.
      //    Succeeds only in state() == 0, if the new thread has been created and started successfully.
      //    state() becomes > 0.
      //  The client should use the following functions to additionally initialize ctx_base:
      //    p->_set_b_own(char flag)
      //        Informs about the way the context object (*p) destruction should be done.
      //        a) _set_b_own(1):
      //          threadctl assumes that the object is created dynamically (new),
      //          and takes ownership on p. When thread ends, p is deleted automatically.
      //          NOTE If the thread fails to start, ownership remains on the client side.
      //        b) _set_b_own(0)  (this is set by dflt. init.):
      //          threadctl does not manage the context object's lifetime. The client is responsible for that.
      //          The context object must remain valid during whole thread run time.
      //    p->_set_pthdata(void* data)
      //        If ctx_base subclass contains a data member to pass to thread proc.,
      //        it has to call _set_pthdata to make the member available via ctx_base::pdata<>().
      // Returns:
      //    true - thread started successfully.
      //    false - thread failed to start.
    bool start(ctx_base* p __bmdx_noarg) throw()
    {
      if (_pctx || !p || p->__dat.in_thread || p->__dat.in_ctl > 0) { return false; }
      typedef _s_long (*Pstart)(_threadctl_ctx_data* p, _threadctl_tid_data* pret_tidobj); Pstart f_start = (Pstart)p->__dat.pthsm(9);
      if (!f_start) { return false; }
      _tid.clear();
      bool b = false;
      try { b = !! f_start(&p->__dat, &_tid._dat); } catch (...) {}
      if (b) { _pctx = p; }
      return b;
    }

      // Dynamically creates a thread context object of the specified type,
      //    starts thread with it's _thread_proc().
      //    Passes a copy of d as additional data member,
      //    accessible via ctx_base::pdata<Data>().
      //  Ctx must:
      //    1. Inherit from ctx_base.
      //    2. Override ctx_base::_thread_proc().
      //    3. Have dflt Ctx().
      //    4. Know Data type to correctly use ctx_base::pdata(),
      //      or ignore pdata at all.
      //  NOTE Additional data member is destroyed later than Ctx.
      //  Returns:
      //    true on successful thread start.
      //    false on any failure. All temp. objects are deleted.
    template<class Ctx, class Data>
    bool start_auto(const Data& d = Data() __bmdx_noarg) throw()
    {
      if (_pctx) { return false; }
      ctx_wrapper<Ctx, Data>* p(0);
      try { p = new ctx_wrapper<Ctx, Data>(d); } catch (...) {} if (!p) { return false; }
      ctx_base* pbase = p; // Ctx must inherit from threadctl::ctx_base
      pbase->_set_b_own(1);
      bool b = start(pbase);
      if (!b) { try { delete pbase; } catch (...) {} }
      return b;
    }

      // Sets thread priority in realtion to process priority class.
      // Allowed values:
      //  4 - normal priority (same as assigned by default),
      //  5 - above normal,
      //  6 - high,
      //  7 - time-critical,
      //  3 - below normal,
      //  2 - low,
      //  1 - background.
    bool set_priority(_s_long p) throw()
    {
      if (!_pctx) { return false; }
      typedef _s_long (*Pset_priority)(_threadctl_ctx_data* p, _s_long prio); Pset_priority f_set_priority = (Pset_priority)_pctx->__dat.pthsm(10);
      if (!f_set_priority) { return false; }
      bool b = false;
      try { b = !! f_set_priority(&_pctx->__dat, p); } catch (...) {}
      return b;
    }

      // Sets b_stop() flag of the target thread to 1.
      // Returns:
      //    true - successful, the client may wait until state() becomes != 2.
      //    false - could not set the flag, threadctl has no context. state() will return 0.
    bool signal_stop() throw() { if (!_pctx) { return false; } if (!_pctx->__dat.bs) { _pctx->__dat.bs = 1; } return true; }

      // Returns a pointer to 4-byte flag variable available for r/w inside thread procedure.
      //    By default, signal_stop() sets the variable to 1 to ask thread to exit.
      //    pflag() allows to use the flag in any other way.
    volatile _s_long* pflag() const throw() { if (!_pctx) { return 0; } return &_pctx->__dat.bs; }

      // 1) If this threadctl is the last one referencing the thread, set b_stop() flag to 1 if it was 0.
      // 2) Detach from the current thread object.
    void clear() throw() { stop(-2); _pctx = 0; _tid.clear(); }

      // Detach from the current thread object.
    void detach() throw() { stop(-3); _pctx = 0; _tid.clear(); }

      // Sets pctx()->b_stop() flag to 1 (only if it was 0), and possibly waits (depending on timeout) for thread exiting.
      //    If thread had exited, detaches from its context
      //    (which is automatically destroyed if this was the last reference).
      // timeout_ms:
      //    >0 - sleeps approx.  (timeout_ms / 10) times * 10 ms,
      //      checking thread state and returns on timeout or thread having exited.
      //    0 - yields once (sleep(0)), then checks thread state and returns.
      //    -1 - Sets b_stop() to 1 if it was 0. Detaches from thread context.
      //    -2 - Sets b_stop() to 1 if it was 0 -- only if the current threadctl is the last one
      //      referencing the thread context. Detaches from thread context.
      //    -3 - detaches from thread context without modifying b_stop() flag.
      //      Thread signaling and ending is responsibility of the client and the thread procedure.
      //  Returns:
      //    2 - thread has exited normally. threadctl is cleared.
      //    1 - thread already exited. threadctl is cleared.
      //    0 - thread was not running (threadctl is empty).
      //    -1 - detached. threadctl is cleared.
      //    -2 - still running (timeout). threadctl continues to reference the thread.
      //    -3 - failure (normally, it does not occur). threadctl is not changed.
    int stop(_s_long timeout_ms) throw()
    {
      if (!_pctx) { return 0; }
      typedef _s_long (*Pstop)(_threadctl_ctx_data* p, _s_long timeout_ms); Pstop f_stop = (Pstop)_pctx->__dat.pthsm(11);
      if (!f_stop) { return -3; }
      _s_long res = -3;
      try { res = f_stop(&_pctx->__dat, timeout_ms); } catch (...) {}
      if (res >= -1) { _pctx = 0; _tid.clear(); }
      return res;
    }

      // Attempts to terminate the thread.
      //  NOTE This is unsafe and platform-dependent:
      //      1. Part of resources, associated with the thread, remains unreleased.
      //      2. terminate() call may cause process crash on any kind of system except explicitly tested ones (listed in arch_notes.txt).
      //      3. On older generation mobile devices, terminate() call may hang whole application (possibility ~= 0.01).
      //  flags OR-ed: determine platform-specific method allowed to terminate the thread:
      //    1 -- QueueUserAPC --> release thread context --> ExitThread --> system release: stack area, thread object.
      //          1) Thread context and data objects, passed via start_auto, ARE released.
      //          2) Any dynamic allocations and resources acquired, except the above, are NOT released.
      //    2 (ignored if 1 is set) -- TerminateThread --> system release: stack area, thread object.
      //          All resouces, allocated by the thread,
      //          + thread data, passed via start_auto,
      //          + thread context object, are NOT released.
      //    0x100 - pthread_kill --> SIGUSR1 --> siglongjmp --> release thread context --> return -->
      //            --> system release: stack area, thread object. (May slightly vary depending on platform.)
      //          1) Thread context and data objects, passed via start_auto, ARE released.
      //          2) Any dynamic allocations and resources acquired, except the above, are NOT released.
      //    0x1000 - terminate only, but do not call signal_stop() or modify b_stop() flag in any way.
      //  Returns:
      //    2 - termination initiated. threadctl is cleared.
      //      NOTE Full completion of thread termination may be deferred, depending
      //        on the chosen method.
      //    1 - thread already exited. threadctl is cleared.
      //    0 - thread was not running (threadctl is empty).
      //    -3 - failure (flags specify no valid method, or by other reason). threadctl is not modifed.
    int terminate(_s_long flags = 0x101) throw()
    {
      if (!_pctx) { return 0; }
      typedef _s_long (*Pterminate)(_threadctl_ctx_data* p, _s_long flags); Pterminate f_terminate = (Pterminate)_pctx->__dat.pthsm(12);
      if (!f_terminate) { return -3; }
      _s_long res = -3;
      try { res = f_terminate(&_pctx->__dat, flags); } catch (...) {}
      _pctx = 0; _tid.clear();
      return res;
    }

    void swap(threadctl& b) { bmdx_str::words::swap_bytes(*this, b); }

  private:
    union { ctx_base* _pctx; _s_ll __pad1; }; tid _tid;
  };



template<class _>
struct _threadctl_tu_static_t<_>::_threadctl_thproc_cleanup
{
  void* _p;
  _threadctl_thproc_cleanup(void* p_) throw() : _p(p_) {}
  ~_threadctl_thproc_cleanup() throw() { exec(); }
  void exec() throw()
  {
    if (!_p) { return; }
    threadctl::ctx_base* pbase = (threadctl::ctx_base*)((_threadctl_ctx_data*)_p)->pctxbase;
    typedef _s_long (*Pctx_release)(_threadctl_ctx_data* p, _s_long what);
    Pctx_release f_rel = 0;
    try { if (pbase->_dat().pthsm) { f_rel = (Pctx_release)pbase->_dat().pthsm(7); } } catch (...) {}
    if (f_rel) { try { f_rel(&pbase->_dat(), 1); } catch (...) {} }
    _p = 0;
  }
};
  // what: 1 - unset thread proc. flag (in_thread), 2 - unset threadctl flag (in_ctl).
  //  Returns:
  //    2 - the last flag has been unset, the context data (p) and all associated objects are deleted and no longer valid.
  //    1 - the specified flag has been unset (now or already), p remains valid.
  //    0 - what is not 1 or 2.
  //    -1 - failure, p == 0 or *p is not valid.
template<class _>
_s_long _threadctl_tu_static_t<_>::th_ctx_release(_threadctl_ctx_data* p, _s_long what) throw()
{
  if (!p) { return -1; }
  if (!p->in_thread && p->in_ctl <= 0) { return -1; }
  _s_long res = 0;
  if (what == 1)
  {
    if (!p->in_thread) { return 1; }
    critsec_t<_threadctl_ctx_data> __lock(10, -1, &p->csd); if (sizeof(__lock)) {}
    p->in_thread = 0; if (p->in_ctl <= 0) { res = 2; }
  }
  else if (what == 2)
  {
    if (p->in_ctl <= 0) { return 1; }
    critsec_t<_threadctl_ctx_data> __lock(10, -1, &p->csd); if (sizeof(__lock)) {}
    --p->in_ctl; if (!p->in_thread && p->in_ctl == 0) { res = 2; }
  }
  if (res == 2)
  {
      // p->th is not valid after _th_ctx_rel_handle.
    _th_ctx_rel_handle(p);
      // p will not be valid after deleting pctxbase.
    if (p->b_own) { try { threadctl::ctx_base* pbase = (threadctl::ctx_base*)p->pctxbase; delete pbase; } catch (...) {} }
  }
  return res;
}
template<class _>
_s_long _threadctl_tu_static_t<_>::th_in_ctl_incr(_threadctl_ctx_data* p) throw()
{
  if (!p) { return 0; }
  critsec_t<_threadctl_ctx_data> __lock(10, -1, &p->csd); if (sizeof(__lock)) {}
  ++p->in_ctl;
  return 1;
}

}



#ifdef _bmdxpl_Wnds

  namespace bmdx
  {
    struct processctl
    {
      struct ff_mc;

      bool has_ref() const throw() { return !!_hp; }
      operator bool() const throw() { return !!_hp; }

        // On has_ref() == true, pid() has meaningful value.
      typedef DWORD t_pid;
      t_pid pid() const throw() { return _pid; }

        // OS-dependent handles (not cross-platform).
      HANDLE _phandle() const throw() { return _hp; }
      HANDLE _thandle() const throw() { return _ht; }

      processctl() throw() : _hp(0), _ht(0), _pid(0), _exited(false) {}
      ~processctl() throw() { clear(); }

      processctl(const processctl& x) throw() : _hp(0), _ht(0), _pid(0), _exited(false)
      {
        if (x._hp) { DuplicateHandle(GetCurrentProcess(), x._hp, GetCurrentProcess(), &_hp, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        if (x._ht) { DuplicateHandle(GetCurrentProcess(), x._ht, GetCurrentProcess(), &_ht, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        _pid = x._pid; _exited = x._exited;
      }
      processctl& operator=(const processctl& x) { if (this == &x) { return *this; } clear(); new (this) processctl(x); return *this; }

      void clear() throw() { _pid = 0; _exited = false; if (_hp) { CloseHandle(_hp); _hp = 0; } if (_ht) { CloseHandle(_ht); _ht = 0; } }

        // Creates new process and returns immediately.
        //
        // fnp_process with b_shell == false: must be full path and executable name (for CreateProcessA()).
        //    fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        //
        // args: program arguments, separated with null character ('\0').
        //    Arguments must not include executable name (i.e. first of args is equivalent to argv[1]).
        //    Each argument will be pre-processed with arg1(),
        //    so the client must not do any special processing (escaping etc.).
        //
        // b_shell false (dflt.):
        //    use CreateProcessA() to launch process.
        //  Returned value:
        //    On success, launch() returns true. Also, has_ref() and pid() reflect the result.
        //    On failure, launch() returns false. Also, has_ref() == false, and pid() value is not valid.
        //
        // b_shell true:
        //    use system("start ...") to launch process (asynchronously).
        //    In this case, has_ref() == false, and pid() value is not valid.
        //  Returned value:
        //    On success, launch() returns true.
        //    On failure, launch() returns false.
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false) throw()
      { try {
        clear();
        if (fnp_process.empty()) { return false; }

        std::string a2; size_t pos = 0, pos2 = 0, end = std::string::npos;
        while (pos < args.length())
        {
          pos2 = args.find('\0', pos);
          if (pos2 == end) { pos2 = args.length(); }
          if (pos2 - pos >= 1) { if (a2.length()) { a2 += ' '; } a2 += ff_mc().arg1(args.substr(pos, pos2 - pos), b_shell); }
          pos = pos2 + 1;
        }
        if (b_shell)
        {
          std::string s = "start \"\" ";
          if (fnp_process[0] != '\"') { s += '\"'; }
          s += fnp_process;
          if (fnp_process[fnp_process.length() - 1] != '\"') { s += '\"'; }
          if (a2.length()) { s += ' '; s += a2; }
          int res = system(s.c_str());
          return res == 0;
        }
        else
        {
          std::string s;
          if (fnp_process.find(' ') != end) { s = "\"" + fnp_process + "\""; } else { s = fnp_process; }
          if (a2.length()) { s += ' '; s += a2; }
          STARTUPINFOA si; PROCESS_INFORMATION pi; ZeroMemory(&si, sizeof(si)); ZeroMemory(&pi, sizeof(pi));
          BOOL res = CreateProcessA(fnp_process.c_str(), &s[0], 0, 0, FALSE, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
          if (!res) { return false; }
          _hp = pi.hProcess;
          _ht = pi.hThread;
          _pid = pi.dwProcessId;
          _exited = false;
          return true;
        }
      } catch (...) { return false; } }

        // Waits for the launched process completion, return its exit code or termination code.
        //  NOTE (Platform-dependent: This function is not protected against PID reuse.)
        //  Returned value:
        //    2 - the process has exited during waiting. *ret_pec = the process exit code.
        //    (Platform-dependent: 1 - the process has been terminated during waiting. *ret_pec = the process termination code.)
        //    0 - the process had been completed before wait_exit() was called. *ret_pec = 0.
        //    -1 - the process is not started, nothing to wait. *ret_pec not changed.
        //    -2 - failure, process may exist, but its state is undefined. *ret_pec not changed.
      int wait_exit(long* ret_pec = 0) throw()
      {
        if (!_hp) { return -1; }
        if (_exited) { if (ret_pec) { *ret_pec = 0; } return 0; }

        WaitForSingleObject(_hp, INFINITE);
        DWORD ec = 0;
        if (!GetExitCodeProcess(_hp, &ec)) { return -2; }
        if (ret_pec) { *ret_pec = long(ec); }
        _exited = true;
        return 2;
      }

        //  NOTE (Platform-dependent: This function is not protected against PID reuse.)
      bool is_running() const throw()
      {
        if (!has_ref()) { return false; }
        DWORD res = WaitForSingleObject(_hp, 0);
        return res == WAIT_TIMEOUT;
      }

      struct ff_mc
      {
        t_pid pid_self() throw() { return t_pid(GetCurrentProcessId()); }

          // Represent s as single argument for passing into program command line.
          //  (Appropriate quoting and escaping is made as necessary).
          //  b_shell false (dflt.) preserves CR and LF characters - intended for passing into CreateProcess().
          //  b_shell true replaces CR and LF characters with ' ', intended for passing single line into system().
          //  In all cases, horz./vert. tab. and formfeed characters are kept.
          //  Other ASCII control characters (0..31) are replaced with '?'.
        std::string arg1(const std::string& s, bool b_shell = false)
        {
          bool b = false;
          for (size_t i = 0; i < s.length(); ++i)
            { if ( ! ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_' || s[i] == '.') )
              { b = true; break; } }
          if (!b) { return s; }

          std::string z = s; b = b_shell;
          for (size_t i = 0; i < z.length(); ++i)
            { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\r': case '\n': if (b) { z[i] = ' '; } break; case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }

          std::string z2 = "\"";
            size_t pos = 0, pos2 = 0, end = std::string::npos;
            while (pos < z.length())
            {
              pos2 = z.find_first_of("\\\"", pos);
                if (pos2 == end) { z2 += z.substr(pos, z.length() - pos); break; }
              z2 += z.substr(pos, pos2 - pos); pos = pos2;
                if (z[pos2] == '\"') { z2 += "\\\""; pos = pos2 + 1; continue; }
              pos2 = z.find_first_not_of("\\", pos);
                if (pos2 == end) { z2 += std::string(2 * (z.length() - pos) + 1, '\\'); break; }
                  else if (z[pos2] == '\"') { z2 += std::string(2 * (pos2 - pos) + 1, '\\'); z2 += '\"'; pos = pos2 + 1; continue; }
              z2 += z.substr(pos, pos2 - pos); pos = pos2; // slashes are not followed by ", so no escaping
            }
          z2 += '\"';
          return z2;
        }
      };

    private:
      HANDLE _hp, _ht; DWORD _pid; bool _exited;
    };


    struct console_io
    {
      unsigned int ugetch(unsigned int no_char = 0) throw()
      #if defined(__BORLANDC__) // && defined(__clang__) && defined(_WIN64)
        { return kbhit() ? getch() : no_char; }
      #else
        { return _kbhit() ? _getch() : no_char; }
      #endif
    };


static void _threadctl_thproc_impl(void* _p) throw()
{
  _threadctl_tu_static_t<>::_threadctl_thproc_cleanup __cln(_p); if (sizeof(__cln)) {}
  threadctl::ctx_base* pbase = (threadctl::ctx_base*)((_threadctl_ctx_data*)_p)->pctxbase;

  try { pbase->_thread_proc(); } catch (...) {}
}
  // NOTE Both in_thread and in_ctl are set to 0.
  // NOTE b_own is set to 0.
  //  To enable autodeleting the context object (factual pctx type) by th_ctx_release,
  //    set p->b_own = 1 after th_ctx_init.
  // 1 - success (succeeds p and pctx are non-0). NOTE
  //  -1 - p or pctx is 0.
template<class _>
_s_long _threadctl_tu_static_t<_>::th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) throw()
{
  if (!(p && pctxbase)) { return -1; }
  p->pthsm = _threadctl_tu_static_t<>::sm; p->pctxbase = pctxbase; p->pthdata = pthdata;
  p->bs = 0; p->in_ctl = 0; p->in_thread = 0; p->b_own = 0; p->__pad1 = 0; p->__pad2 = 0;
  std::memset(&p->th, 0, sizeof(p->th));
  _critsec_tu_static_t<_threadctl_ctx_data>::csdata_init(&p->csd);
  return 1;
}
  }

#endif



#ifdef _bmdxpl_Psx

  namespace bmdx
  {
    extern "C" typedef void (*PF_threadctl_terminate_hsig)(int);

    extern "C" typedef void (*PF_processctl_handler_sigchld)(int, siginfo_t*, void*);
    static void _processctl_handler_sigchld(int, siginfo_t*, void*) { pid_t p; do { p = waitpid(-1, NULL, WNOHANG); } while (p != (pid_t)0 && p != (pid_t)-1); }
    static bool& _processctl_has_sigchld() { struct _deinit { ~_deinit() { _processctl_handler_sigchld(0, 0, 0); } }; static bool b(false); static _deinit x1; if (sizeof(x1)) {} return b; }

    struct processctl
    {
      struct ff_mc;

      bool has_ref() const throw() { return _pid > 0; }
      operator bool() const throw() { return _pid > 0; }

        // On has_ref() == true, pid() has meaningful value.
      typedef pid_t t_pid;
      t_pid pid() const throw() { return t_pid(_pid); }

      processctl() throw() : _pid(-1) {}
      ~processctl() throw() { clear(); }

      void clear() throw() { _pid = -1; if (_processctl_has_sigchld()) { _processctl_handler_sigchld(0, 0, 0); } }

        // Creates new process and returns immediately.
        //
        // fnp_process with b_shell == false: must be full path and executable name (for posix_spawn()).
        //    fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        //
        // args: all program arguments, separated with null character ('\0').
        //    Arguments must not include executable name (i.e. first of args is equivalent to argv[1]).
        //    Each argument will be pre-processed with arg1() as necessary,
        //    so the client must not do any special processing (escaping etc.).
        //
        // b_shell false (dflt.):
        //    use posix_spawn() to launch process.
        //    NOTE launch() with b_shell == false sets custom SIGCHLD handler.
        //  Returned value:
        //    On success, launch() returns true. Also, has_ref() and pid() reflect the result.
        //    On failure, launch() returns false. Also, has_ref() == false, and pid() value is not valid.
        //
        // b_shell true:
        //    use system("... &") to launch process (asynchronously).
        //    In this case, has_ref() == false, and pid() value is not valid.
        //  Returned value:
        //    On success, launch() returns true.
        //    On failure, launch() returns false.
      #ifndef bmdx_processctl_allowshell
        #if TARGET_OS_IPHONE
          #define bmdx_processctl_allowshell 0
        #else
          #define bmdx_processctl_allowshell 1
        #endif
      #endif
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false) throw()
      { try {
        clear();
        if (fnp_process.empty()) { return false; }

        #if !bmdx_processctl_allowshell
          b_shell = false;
        #endif

        if (b_shell)
        {
          #if bmdx_processctl_allowshell
            std::string a2; size_t pos = 0, pos2 = 0, end = std::string::npos;
            while (pos < args.length())
            {
              pos2 = args.find('\0', pos);
              if (pos2 == end) { pos2 = args.length(); }
              if (pos2 - pos >= 1) { if (a2.length()) { a2 += ' '; } a2 += ff_mc().arg1(args.substr(pos, pos2 - pos), b_shell); }
              pos = pos2 + 1;
            }
            std::string s;
            pos = 0, pos2 = 0, end = std::string::npos;
            while (pos < fnp_process.length())
            {
              pos2 = fnp_process.find_first_of("' ", pos);
                if (pos2 == end) { s += fnp_process.substr(pos, fnp_process.length() - pos); break; }
              s += fnp_process.substr(pos, pos2 - pos);
              s += '\\';
              s += fnp_process[pos2];
              pos = pos2 + 1;
            }
            if (a2.length()) { s += ' '; s += a2; }
            s += " &";
            int res = system(s.c_str());
            return res == 0;
          #else
            return false;
          #endif
        }
        else
        {
          std::string z = args;
              for (size_t i = 0; i < z.length(); ++i)
                { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\0': case '\r': case '\n': case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }
          std::basic_string<char*> a2;
          std::string sp = fnp_process;
          a2 += &sp[0];
          size_t pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < z.length())
          {
            pos2 = z.find('\0', pos);
            if (pos2 == end) { pos2 = z.length(); }
            if (pos2 - pos >= 1) { a2 += &z[pos]; }
            pos = pos2 + 1;
          }
          a2 += (char*)0;

          struct sigaction act; std::memset(&act, 0, sizeof(act)); act.sa_sigaction = (PF_processctl_handler_sigchld)&_processctl_handler_sigchld; sigemptyset(&act.sa_mask); act.sa_flags = SA_NOCLDSTOP|SA_RESTART|SA_SIGINFO;
          sigaction(SIGCHLD, &act, NULL);
          _processctl_has_sigchld() = true;

          struct _local1
          {
            static inline int spawn(pid_t* pid, const char* path, char* const argv[], char* const envp[])
            #ifdef __ANDROID__
              {
                volatile int error = 0; pid_t p = vfork();
                switch (p)
                {
                  case -1: return errno;
                  case 0: execve(path, argv, envp); error = errno; _exit(127);
                  default:
                    if (error != 0) { waitpid(p, NULL, WNOHANG); } else { if (pid != NULL) { *pid = p; } }
                    return error;
                }
              }
            #else
              { return posix_spawn(pid, path, 0, 0, argv, envp); }
            #endif
          };
          pid_t __pid;
          int res = _local1::spawn(&__pid, &sp[0], &a2[0], environ);
          if (res != 0) { return false; }
          _pid = __pid;
          return true;
        }
      } catch (...) { return false; } }

        // Waits for the launched process completion, return its exit code or termination code.
        //  NOTE This function is not protected against PID reuse.
        //  Returned value:
        //    2 - the process has exited during waiting. *ret_pec = the process exit code.
        //    1 - the process has been terminated during waiting. *ret_pec = the process termination code.
        //    0 - the process had been completed before wait_exit() was called. *ret_pec = 0.
        //    -1 - the process is not started, nothing to wait. *ret_pec not changed.
        //    -2 - failure, process may exist, but its state is undefined. *ret_pec not changed.
      int wait_exit(long* ret_pec = 0) throw()
      {
        if (_pid == -2) { if (ret_pec) { *ret_pec = 0; } return 0; }
        if (_pid < 0) { return -1; }
        int st;
        long res = waitpid(pid_t(_pid), &st, 0);
        if (res == _pid)
        {
          _pid = -2;
          if (WIFEXITED(st)) { if (ret_pec) { *ret_pec = long(WEXITSTATUS(st)); } return 2; }
          if (WIFSIGNALED(st)) { if (ret_pec) { *ret_pec = long(WTERMSIG(st)); } return 1; }
          return -2;
        }
        if (res != -1) { return -2; }
        _pid = -2; if (ret_pec) { *ret_pec = 0; }
        return 0;
      }

      //  NOTE This function is not protected against PID reuse.
      bool is_running() const throw()
      {
        if (!has_ref()) { return false; }
        int res = kill(pid_t(_pid), 0);
        return res == 0;
      }

      struct ff_mc
      {
        t_pid pid_self() throw() { return t_pid(getpid()); }

          // Represent s as single argument for passing into program command line.
          //  (Appropriate quoting and escaping is made as necessary).
          //  b_shell false (dflt.) preserves CR and LF characters, intended for passing into posix_spawn() as is.
          //  b_shell true replaces CR and LF characters with ' ', intended for passing single line into system().
          //  In all cases, horz./vert. tab. and formfeed characters are kept.
          //  Other ASCII control characters (0..31) are replaced with '?'.
        std::string arg1(const std::string& s, bool b_shell = false)
        {
          bool b = false;
          for (size_t i = 0; i < s.length(); ++i)
            { if ( ! ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_' || s[i] == '.') )
              { b = true; break; } }
          if (!b) { return s; }

          std::string z = s; b = b_shell;
              for (size_t i = 0; i < z.length(); ++i)
                { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\r': case '\n': if (b) { z[i] = ' '; } break; case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }

          if (!b_shell) { return z; }

          std::string z2 = "'";
            size_t pos = 0, pos2 = 0, end = std::string::npos;
            while (pos < z.length())
            {
              pos2 = z.find("'", pos);
                if (pos2 == end) { z2 += z.substr(pos, z.length() - pos); break; }
              z2 += z.substr(pos, pos2 - pos);
              z2 += "'\\''";
              pos = pos2 + 1;
            }
          z2 += '\'';
          return z2;
        }
      };

    private:
      long _pid;
    };


    struct console_io
    {
      console_io() throw() { static termios t; static _s_long n(0); __pt = &t; __pnr = &n;_set_unbuf(true); }
      ~console_io() throw() { _set_unbuf(false); }

      unsigned int ugetch(unsigned int no_char = 0) throw()
      {
        critsec_t<console_io> __lock(50,-1); if (sizeof(__lock)) {}
        int f0 = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, f0 | O_NONBLOCK);
          int ch = std::getchar();
        fcntl(STDIN_FILENO, F_SETFL, f0);
        if (ch != EOF) { return ch; } else { return no_char; }
      }

    private:
      termios* __pt; _s_long* __pnr;
      void _set_unbuf(bool on) throw()
      {
        critsec_t<console_io> __lock(50,-1); if (sizeof(__lock)) {}
        _s_long n = *__pnr; termios& t0 = *__pt;
        n += on ? 1 : -1;
        if (n == 1)
        {
          tcgetattr(STDIN_FILENO, &t0);
            termios t1 = t0;
            t1.c_lflag &= ~(ICANON | ECHO);
            t1.c_cc[VMIN] = 1; t1.c_cc[VTIME] = 0;
          tcsetattr(STDIN_FILENO, TCSANOW, &t1);
          *__pnr = n;
        }
        else if (n <= 0)
        {
          tcsetattr(STDIN_FILENO, TCSANOW, &t0);
          *__pnr = 0;
        }
      }
    };


static void _threadctl_thproc_impl(void* _p) throw()
{
  pthread_key_t* pk = _threadctl_psjstg();
    if (!pk) { return; }

  threadctl::ctx_base* pbase = (threadctl::ctx_base*)((_threadctl_ctx_data*)_p)->pctxbase;
    if (!pbase) { return; }

  _threadctl_ctx_data& d = pbase->__dat;

  pthread_setspecific(*pk, &d.sjctx);

  _threadctl_tu_static_t<>::_threadctl_thproc_cleanup __cln(_p); if (sizeof(__cln)) {}

  if (sigsetjmp(d.sjctx.buf, 1) == 0)
  {
    d.sjctx.state = 1;
    try { pbase->_thread_proc(); } catch (...) {}
  }
  d.sjctx.state = 2;
}
  // NOTE Both in_thread and in_ctl are set to 0.
  // NOTE b_own is set to 0.
  //  To enable autodeleting the context object (factual pctx type) by th_ctx_release,
  //    set p->b_own = 1 after th_ctx_init.
  // 1 - success (succeeds p and pctx are non-0). NOTE
  //  -1 - p or pctx is 0.
template<class _>
_s_long _threadctl_tu_static_t<_>::th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) throw()
{
  if (!(p && pctxbase)) { return -1; }
  p->pthsm = _threadctl_tu_static_t<>::sm; p->pctxbase = pctxbase; p->pthdata = pthdata;
  p->bs = 0; p->in_ctl = 0; p->in_thread = 0; p->b_own = 0; p->__pad1 = 0; p->__pad2 = 0;
  std::memset(&p->th, 0, sizeof(p->th));
  _critsec_tu_static_t<_threadctl_ctx_data>::csdata_init(&p->csd);
  std::memset(&p->sjctx, 0, sizeof(p->sjctx));
  return 1;
}
  //    2 - terminated. threadctl is cleared.
  //    1 - thread already exited. threadctl is cleared.
  //    0 - thread was not running (threadctl is empty).
  //    -3 - failure (flags specify no valid method, or by other reason).
template<class _>
_s_long _threadctl_tu_static_t<_>::th_terminate(_threadctl_ctx_data* p, _s_long flags) throw()
{
  #ifdef __ANDROID__
    enum { sig_th_term = SIGUSR2 };
  #else
    enum { sig_th_term = SIGUSR1 };
  #endif
  if (!p) { return 0; }
  if (!p->bs && !(flags & 0x1000)) { p->bs = 1; }
  if (!p->in_thread) { _threadctl_tu_static_t<_>::th_ctx_release(p, 2); return 1; }
  struct _local1
  {
    static void hsig(int sig)
    {
      if (sig != sig_th_term) { return; }

      static pthread_key_t* pk(0);
        if (!pk) { pk = _threadctl_psjstg(); }
        if (!pk) { return; }

      _threadctl_ctx_data::_sjctx* psj = (_threadctl_ctx_data::_sjctx*)pthread_getspecific(*pk);
      if (psj && psj->trq && psj->state == 1)
      {
        #if __APPLE__ && __MACH__
          psj->state = 2;
          int temp;
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &temp);
          pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &temp);
          pthread_cancel(pthread_self());
        #else
          siglongjmp(psj->buf, 1);
        #endif
      }
    }
    static void enable()
    {
      struct sigaction act; std::memset(&act, 0, sizeof(act));
      sigemptyset(&act.sa_mask);
      act.sa_flags = 0;
      act.sa_handler = (PF_threadctl_terminate_hsig)hsig;
      sigaction(sig_th_term, &act, NULL);
    }
  };

  if (flags & 0x100)
  {
    critsec_t<_threadctl_ctx_data::_sjctx> __lock(10, -1); if (sizeof(__lock)) {}
    double t0 = clock_ms();
    while (p->sjctx.state == 0)
    {
      if (clock_ms() - t0 >= 1000) { return -3; }
      sleep_mcs(1000);
    }
    if (p->sjctx.state == 1)
    {
      _local1::enable();
      p->sjctx.trq = 1;
      pthread_kill(p->th, sig_th_term);
    }

    #if __APPLE__ && __MACH__
      while (p->sjctx.state < 2)
      {
        if (clock_ms() - t0 >= 1000)
        {
          _threadctl_tu_static_t<_>::th_ctx_release(p, 2);
          return 2;
        }
        sleep_mcs(1000);
      }
      _threadctl_tu_static_t<_>::th_ctx_release(p, 1);
    #endif
    _threadctl_tu_static_t<_>::th_ctx_release(p, 2);
    return 2;
  }
  return -3;
}
  }
#endif



#if defined(yk_c_vecm_hashx_H) && !defined(__bmdx_cpiomt_threadctl_spec)
  #define __bmdx_cpiomt_threadctl_spec
  namespace yk_c
  {
    template<> struct vecm::spec<bmdx::threadctl> { struct aux : vecm::config_aux<bmdx::threadctl> { };  typedef config_t<bmdx::threadctl, 1, 4, 1, aux> config; };
    template<> struct vecm::spec<bmdx::threadctl::tid> { struct aux : vecm::config_aux<bmdx::threadctl::tid> { };  typedef config_t<bmdx::threadctl::tid, 1, 4, 1, aux> config; };
    template<> struct bytes::type_index_t<bmdx::threadctl> : cmti_base_t<bmdx::threadctl, 2017, 6, 7, 12> {};
    template<> struct bytes::type_index_t<bmdx::threadctl::tid> : cmti_base_t<bmdx::threadctl::tid, 2017, 6, 7, 13> {};
    namespace { bytes::type_index_t<bmdx::threadctl> __cmti_inst_threadctl; }
    namespace { bytes::type_index_t<bmdx::threadctl::tid> __cmti_inst_threadctl_tid; }
  }
#endif


#if defined(_MSC_VER)
  #define __bmdx_std_fopen_s fopen_s
#else
  #define __bmdx_std_fopen_s std::fopen_s
#endif
#ifdef _bmdxpl_Wnds
  #define __bmdx_std_getcwd _getcwd
  #define __bmdx_std_mkdir _mkdir
  #define __bmdx_std_access _access
#endif
#ifdef _bmdxpl_Psx
  #define __bmdx_std_getcwd getcwd
  #define __bmdx_std_mkdir mkdir
  #define __bmdx_std_access access
#endif
namespace bmdx
{
  struct file_io
  {

      // Platform-dependent file handle type.
    typedef std::FILE* t_handle;

      // Factual type of the offset in file.
      // NOTE OS X: consider compiling with -D__DARWIN_64_BIT_INO_T
      //    to ensure 64-bit offsets and correct file r/w beyond 4 GB limit.
    #if defined(_MSC_VER) || defined(__BORLANDC__)
      typedef __int64 t_offset;
    #elif defined(__FreeBSD__)
      typedef __off_t t_offset;
    #elif __APPLE__ && __MACH__
      typedef off_t t_offset;
    #else
       typedef off64_t t_offset;
    #endif


      // Returns true if the specified path points to the existing file, false in all other cases.
    static bool is_ex_file(const char* ppath __bmdx_noarg) throw()        { if ( ppath && *ppath != '\0' && 0 == __bmdx_std_access(ppath, __F_OK) ) { struct stat st; return 0 == stat(ppath, &st) && !!(st.st_mode & S_IFREG); } else { return false; } }
    static inline bool is_ex_file(const std::string& path __bmdx_noarg) throw()        { return is_ex_file(path.c_str()); }

      // Returns true if the specified path points to the existing directory, false in all other cases.
    static bool is_ex_dir(const char* ppath __bmdx_noarg) throw()        { if ( ppath && *ppath != '\0' && 0 == __bmdx_std_access(ppath, __F_OK) ) { struct stat st; return 0 == stat(ppath, &st) && !!(st.st_mode & S_IFDIR); } else { return false; } }
    static inline bool is_ex_dir(const std::string& path __bmdx_noarg) throw()        { return is_ex_dir(path.c_str()); }





    //  is_open, result.
    //  open, close, seek, seek_end, tell, read, write, flush.
    //  static: is_ex_file, is_ex_dir, load_bytes, save_bytes

    file_io() throw() : _desc(0), _nwr(0), _mode(0), _res(0) { __pad4 = 0; _last_op_wr = 0; }
    ~file_io() throw() { if (is_open()) { close(); } }

      // NOTE Copy constructor does not actually copy.
      //    Defined for compatibility only.
    file_io(const file_io&) throw() { new (this) file_io(); }


    inline bool is_open() const throw() { return !!_desc; }
      // mode 0: file is not open, mode 1: file is open read-only, mode 2: file is open read-write.
    inline _s_long mode() const throw() { return _desc ? _mode : 0; }
    inline _s_long result() const throw() { return _res; }

      // Platform-dependent file handle.
    inline t_handle _handle() const { return _desc; }

      // Opens or reopens a file with the specified name and parameters.
      // result():
      //    1 - success,
      //    -1 opening existing file for reading failed,
      //    -2 opening existing file for r/w failed,
      //    -3 file does not exist, cannot open for reading.
      //    -4 file does not exist, file creation (for r/w) failed.
      //    -5 pfilename is null.
    inline void open(const char* pfilename, bool can_wrcr, bool wr_trunc = false) throw()
    {
      if (!pfilename || *pfilename == '\0') { _res = -5; }
      if (is_open()) { close(); }
      #if defined(_MSC_VER) || defined(__BORLANDC__)
        if (is_ex_file(pfilename)) { if (can_wrcr) { __bmdx_std_fopen_s(&_desc, pfilename, wr_trunc ? "w+b" : "r+b"); _res = _desc ? 1 : -2; } else { __bmdx_std_fopen_s(&_desc, pfilename, "rb"); _res = _desc ? 1 : -1; } }
          else { if (can_wrcr) { __bmdx_std_fopen_s(&_desc, pfilename, "w+b"); _res = _desc ? 1 : -4; } else { _res = -3; } }
      #else
        if (is_ex_file(pfilename)) { if (can_wrcr) { _desc = ::fopen(pfilename, wr_trunc ? "w+b" : "r+b"); _res = _desc ? 1 : -2; } else { _desc = ::fopen(pfilename, "rb"); _res = _desc ? 1 : -1; } }
          else { if (can_wrcr) { _desc = ::fopen(pfilename, "w+b"); _res = _desc ? 1 : -4; } else { _res = -3; } }
      #endif
      _mode = can_wrcr ? 2 : 1;
      if (_res == 1) { std::setvbuf(_desc, 0, _IOFBF, _nwrchunk); }
    }

      // result():
      //    1 - success,
      //    0 - the file was not open,
      //    -1 - an error, the file is regarded closed.
    inline void close() throw()
    {
      if (!is_open()) { _res = 0; return; }
      _res = fclose(_desc) == 0 ? 1 : -1;
      _desc = 0;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - seek error.
    inline void seek(_s_ll pos) throw()
    {
      if (!is_open()) { _res = -1; return; }
      if (pos < 0) { _res = -2; return; }
      if (_last_op_wr && _nwr) { fflush(_desc); _nwr = 0; }
      _res = _seek_u(pos) == 0 ? 1 : -2;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - seek error.
    inline void seek_end() throw()
    {
      if (!is_open()) { _res = -1; return; }
      if (_last_op_wr && _nwr) { fflush(_desc); _nwr = 0; }
      #if defined(_MSC_VER) || defined(__BORLANDC__)
        _res = _fseeki64(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        _res = fseeko(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #else
        _res = fseeko64(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #endif
    }

      // Returns valid pos. >=0, or -1 on error.
      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - error.
    inline _s_ll tell() const throw()
    {
      if (!is_open()) { _res = -1; return -1; }
      _s_ll pos = _tell_u();
      _res = pos >= 0 ? 1 : -2;
      return pos;
    }

      // Returns: number of bytes factually read.
      // result():
      //    1 - success, size bytes have been read
      //    -1 - the file is not open,
      //    -2 - read error, pos. is not changed,
      //    -3 - read error, pos. is changed.
    inline size_t read(void* dest, size_t size) throw()
    {
      if (!is_open()) { _res = -1; return 0; }
      if (_last_op_wr)
      {
        if (_nwr) { fflush(_desc); _nwr = 0; }
        _s_ll pos = _tell_u();
        if (pos < 0) { _res = -2; return 0; }
        rewind(_desc);
        if (_seek_u(pos) != 0) { _res = -3; return 0; }
      }
      size_t s2 = fread(dest, 1, size, _desc);
      _last_op_wr = 0;
      _res = s2 == size ? 1 : (s2 == 0 ? -2 : -3);
      return s2;
    }

      // Returns: number of bytes factually written.
      // result():
      //    1 - success, size bytes have been written
      //    -1 - the file is not open,
      //    -2 - write error, file/pos is not changed,
      //    -3 - write error, file/pos is changed.
    inline size_t write(const void* src, size_t size) throw()
    {
      if (!is_open()) { _res = -1; return 0; }
      if (!_last_op_wr)
      {
        _s_ll pos = _tell_u();
        if (pos < 0) { _res = -2; return 0; }
        rewind(_desc);
        if (_seek_u(pos) != 0) { _res = -3; return 0; }
      }
      if (_nwr + size >= _nwrchunk) { fflush(_desc); _nwr = 0; }
      size_t s2 = fwrite(src, 1, size, _desc);
      _last_op_wr = 1;
      _res = s2 == size ? 1 : (s2 == 0 ? -2 : -3);
      _nwr += s2;
      return s2;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - operation error.
      // NOTE Due to system caching, no guarantee that flush() writes to disk immediately.
    inline void flush() throw()
    {
      if (!is_open()) { _res = -1; return; }
      _res = fflush(_desc) == 0 ? 1 : -2;
    }





  private:
    file_io& operator=(const file_io&);

    static const int __F_OK = 0;

    t_handle _desc;
    mutable size_t _nwr;
    _s_long _mode;
    mutable _s_long _res;
    union { mutable char _last_op_wr; _s_ll __pad4; };
    static const size_t _nwrchunk = 2048;

    inline _s_ll _tell_u() const throw()
    {
      t_offset pos;
      #if defined(_MSC_VER) || defined(__BORLANDC__)
        pos = _ftelli64(_desc);
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        pos = ftello(_desc);
      #else
        pos = ftello64(_desc);
      #endif
      if (sizeof(_s_ll) > sizeof(t_offset)) { _s_ll m = 1; _s_ll q = sizeof(t_offset); m <<= 4 * q; m <<= 4 * q; m -= 1; if (pos == t_offset(m)) { return -1; } return _s_ll(pos) & m; }
      return _s_ll(pos);
    }
    inline int _seek_u(_s_ll pos) throw()
    {
      if (sizeof(_s_ll) > sizeof(t_offset)) { _s_ll m = 1; _s_ll q = sizeof(t_offset); m <<= 4 * q; m <<= 4 * q; m -= 1; if ((pos & m) != pos) { return -2; } }
      int res;
      #if defined(_MSC_VER) || defined(__BORLANDC__)
        res = _fseeki64(_desc, t_offset(pos), SEEK_SET);
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        res = fseeko(_desc, t_offset(pos), SEEK_SET);
      #else
        res = fseeko64(_desc, t_offset(pos), SEEK_SET);
      #endif
      return res;
    }




    private: struct _stra_base; struct _stra_ca;  struct _stra_str;
    public:


          // Loads bytes from the given file into the dest. container. Resizes the container as necessary.
          // 1 - success.
          // 0 - file does not exist.
          // -1 - memory alloc. error, or wrong arguments.
          // -2 - file i/o error. NOTE On i/o error, dest may be left modified.
      static inline int load_bytes(const std::string& fnp, std::string& dest __bmdx_noarg) throw() { return _load_bytes(fnp.c_str(), _stra_str(), &dest); }
      static inline int load_bytes(const char* fnp, std::string& dest __bmdx_noarg) throw() { return _load_bytes(fnp, _stra_str(), &dest); }

      static inline int load_bytes(const std::string& fnp, _carray_base_t<char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }
      static inline int load_bytes(const std::string& fnp, _carray_base_t<unsigned char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<unsigned char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }
      static inline int load_bytes(const std::string& fnp, _carray_base_t<signed char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<signed char>& dest __bmdx_noarg) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }

          // Saves bytes from src to the given file.
          //    b_append == false truncates the file before writing, if it exists.
          //    if n == 0, pdata may be 0.
          // 1 - success.
          // 0 - failed to create file (or open the existing file for writing).
          // -1 - data size too large, or memory alloc. error, or wrong arguments.
          // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
      static inline int save_bytes(const char* fnp, const std::string& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp, src.c_str(), src.length(), b_append); }
      static inline int save_bytes(const std::string& fnp, const std::string& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp.c_str(), src.c_str(), _s_ll(src.length()), b_append); }

      static inline int save_bytes(const char* fnp, const char* pdata, _s_ll n0, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp, pdata, n0, b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<char>& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp, src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<char>& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp.c_str(), src.pd(), src.n(), b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<unsigned char>& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<unsigned char>& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp.c_str(), (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<signed char>& src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<signed char>&  src, bool b_append __bmdx_noarg) throw() { return _save_bytes(fnp.c_str(), (const char*)src.pd(), src.n(), b_append); }



    private:
      struct _stra_base { typedef std::string::size_type _t_sz; virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const throw() = 0; virtual char* _pd(void* ps __bmdx_noarg) const throw() = 0; };
      struct _stra_str : _stra_base { virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const throw() { typedef std::string T; _t_sz n = _t_sz(n0); if (_s_ll(n) != n0) { return false; } try { if (n) { ((T*)ps)->resize(n); } else { ((T*)ps)->clear(); } return true; } catch (...) { return false; } } virtual char* _pd(void* ps __bmdx_noarg) const throw() { typedef std::string T; return &(*(T*)ps)[0]; } };
      struct _stra_ca : _stra_base { virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const throw() { typedef _carray_base_t<char> T; return ((T*)ps)->realloc(n0, 0, 0, 0); } virtual char* _pd(void* ps __bmdx_noarg) const throw() { typedef _carray_base_t<char> T; return ((T*)ps)->pd(); } };

      static int _load_bytes(const char* fnp, const _stra_base& a, void* ps __bmdx_noarg) throw()
      {
        if (!fnp) { return -1; }
        file_io f; f.open(fnp, false); if (!f.is_open()) { return f.result() == -3 ? 0 : -2; }
        f.seek_end(); if (f.result() < 1) { return -2; }
        _s_ll n0 = f.tell(); if (n0 < 0) { return -2; }
        f.seek(0); if (f.result() < 1) { return -2; }
        if (_s_ll(size_t(n0)) != n0) { return -1; }
        if (!a._resize(ps, n0)) { return -1; }
        if (!n0) { return 1; }
        f.read(a._pd(ps), size_t(n0)); if (f.result() < 1) { a._resize(ps, 0); return -2; }
        return 1;
      }
      static int _save_bytes(const char* fnp, const char* pdata, _s_ll n0, bool b_append __bmdx_noarg) throw()
      {
        if (!(fnp && n0 >= 0 && !(n0 && !pdata))) { return -1; }
        size_t n = size_t(n0); if (_s_ll(n) != n0) { return -1; }
        file_io f; f.open(fnp, true, !b_append); if (!f.is_open()) { return 0; }
        if (!n) { return 1; }
        if (b_append) { f.seek_end(); if (f.result() < 1) { return -2; } }
        f.write(pdata, n); if (f.result() < 1) { return -2; }
        return 1;
      }
  };




  template<class T, class LockSelector> struct cref_t;

  template<class Selector> struct _cref_lock_t : critsec_t<Selector>
  {
    typedef critsec_t<Selector> t_impl;

      // b_wait true - block until the lock is acquired,
      //  false - try lock once and immediately exit (use operator bool and/or try_lock after that).
    _cref_lock_t(bool b_wait, typename critsec_t<Selector>::csdata* pcsd)
      : t_impl(-1, 0, pcsd) { try_lock(b_wait); }

    bool try_lock(bool b_wait)
      { if (this->is_locked()) { return true; } _critsec_data0_t<Selector>* ps = this->pcsd0(); this->~_cref_lock_t(); new (this) t_impl(5, b_wait ? -1 : 0, ps); return this->is_locked(); }

      // This constructor ensures that cref_t object at the address &x
      //  is locked
      //  even in conditions when it may be concurrently overwritten
      //  with cref_t object from this or another binary module.
    template<class T> _cref_lock_t(const cref_t<T, Selector>& x)
      : t_impl(-1, 0, x._ps)
      { while (true) { _critsec_data0_t<Selector>* ps1 = x._ps; this->~_cref_lock_t(); new (this) t_impl(5, -1, ps1); if (x._ps == ps1) { break; } sleep_mcs(50); } }

    operator bool() const        { return this->is_locked(); }
  };

    // This selector disables locking. (Anyway, such lock object returns true, as if the lock has been really set successfully.)
  struct cref_nonlock {};
  template<> struct _cref_lock_t<cref_nonlock>
  {
    typedef bmdx_meta::nothing t_impl;
    _cref_lock_t(bool b_wait)        { (void)b_wait; }
    template<class T> _cref_lock_t(const cref_t<T, cref_nonlock>&)        {}
    bool try_lock()       { return true; }
    operator bool() const        { return true; }
  };

  struct _cref_t_exceptions
  {
    struct exc_ref : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::ref"; } };
    struct exc_ref_ts : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::ref_ts"; } };
    struct exc_create0 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create0"; } };
    struct exc_create1 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create1"; } };
    struct exc_create2 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create2"; } };
    struct exc_create3 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create3"; } };
    struct exc_create4 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create4"; } };
    struct exc_create5 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create5"; } };
    struct exc_create6 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::create6"; } };
    struct exc_iref0 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create0"; } };
    struct exc_iref1 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create1"; } };
    struct exc_iref2 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create2"; } };
    struct exc_iref3 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create3"; } };
    struct exc_iref4 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create4"; } };
    struct exc_iref5 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create5"; } };
    struct exc_iref6 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::iref::create6"; } };
    struct exc_assign : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::assign"; } };
    struct exc_copy : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::copy"; } };
    struct exc_cm_assign : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::cm_assign"; } };
    struct exc_cm_copy : std::exception { inline const char* what() const throw() { return "bmdx::cref_t::cm_copy"; } };
    struct exc_cc3 : std::exception { inline const char* what() const throw() { return "bmdx::cref_t(x,is_own,false)"; } };
  };



    // struct cref_t
    //
    // Thread-safe const reference (same as smart pointer),
    //  with optional access by non-const pointer (_pnonc_u).
    //
    // T:
    //    the type of referenced object. By default, the object is assumed to be constant,
    //      accessed via ref(), ptr() or ref_ts()().
    //    Alternatively, the referenced object may be treated as non-constant,
    //      accessed safely (ref_ts().xnc) or unsafely (_pnonc_u()). Unsafe access is faster.
    // LockSelector:
    //    a) With default lock selector, all cref_t objects with the same type of referenced value
    //      (T) share the same lock.
    //    b) cref_nonlock creates non-locking cref_t object.
    //    c) Any other type of lock selector allows to group objects sharing the same lock.
    //    d) cm_copy, cm_assign, cm_create* functions
    //      allow to specify any crit. sec. data object to use for locking.
    //      Such cref_t object becomes independent on LockSelector,
    //      and may be thread-safely passed between binary modules.
    //    NOTE If the referenced object is created in a shared library,
    //      that library must not be unloaded until the last object reference is released
    //      (i.e. the last cref_t for that object is destroyed).

  template<class T, class LockSelector = T>
  struct cref_t
  {

    typedef T t_value;


    typedef _cref_lock_t<LockSelector> t_lock;


      // In 32-bit program: s_long, in 64-bit program: s_ll.
    typedef typename bmdx_meta::if_t<(sizeof(void*) > 4), _s_ll, _s_long>::result
      t_cnt;

    struct exc_ref : _cref_t_exceptions::exc_ref {};
    struct exc_ref_ts : _cref_t_exceptions::exc_ref_ts {};
    struct exc_create0 : _cref_t_exceptions::exc_create0 {};
    struct exc_create1 : _cref_t_exceptions::exc_create1 {};
    struct exc_create2 : _cref_t_exceptions::exc_create2 {};
    struct exc_create3 : _cref_t_exceptions::exc_create3 {};
    struct exc_create4 : _cref_t_exceptions::exc_create4 {};
    struct exc_create5 : _cref_t_exceptions::exc_create5 {};
    struct exc_create6 : _cref_t_exceptions::exc_create6 {};
    struct exc_iref0 : _cref_t_exceptions::exc_iref0 {};
    struct exc_iref1 : _cref_t_exceptions::exc_iref1 {};
    struct exc_iref2 : _cref_t_exceptions::exc_iref2 {};
    struct exc_iref3 : _cref_t_exceptions::exc_iref3 {};
    struct exc_iref4 : _cref_t_exceptions::exc_iref4 {};
    struct exc_iref5 : _cref_t_exceptions::exc_iref5 {};
    struct exc_iref6 : _cref_t_exceptions::exc_iref6 {};
    struct exc_assign : _cref_t_exceptions::exc_assign {};
    struct exc_copy : _cref_t_exceptions::exc_copy {};
    struct exc_cm_assign : _cref_t_exceptions::exc_cm_assign {};
    struct exc_cm_copy : _cref_t_exceptions::exc_cm_copy {};
    struct exc_cc3 : _cref_t_exceptions::exc_cc3 {};

  private:
    friend struct _cref_lock_t<LockSelector>;
    typedef _carray_tu_alloc_t<T> _a; typedef void (*_F_free)(void* p); typedef void (*_F_des)(void* p);
    static const t_cnt _f3 = t_cnt(1) << (sizeof(t_cnt) * 8 - 3); // "cross-module object handling v2" (reserved, kept 0)
    static const t_cnt _f2 = _f3 << 1;
    static const t_cnt _f23 = _f2 | _f3; // != 0 <=> "cross-module object handling", i.e. *_ph is a kind of _cref_handle
    static const t_cnt _f1 = _f3 << 2; // "detached (== destructor disabled)"
    static const t_cnt _m = ~(_f1 | _f2 | _f3); // mask for reference counting bits in _ph->cnt
    struct _cref_handle2
    {
      struct _cref_handle
      {
        volatile t_cnt cnt; t_cnt flags; _F_free _f_free; _F_des _f_des1;

        _cref_handle(t_cnt n0, _s_long flags_) { cnt = (n0 & _m) | _f2; flags = flags_; _f_free = (_F_free)_a::_spff[2]; _f_des1 = (_F_des)_a::_spff[3]; }
        bool b_v0() const throw() { return (cnt & _f23) == 0; } // *this is just a t_cnt
        bool b_v1() const throw() { return (cnt & _f23) == _f2; } // *this is a _cref_handle
        bool b_detached() const throw() { return !!(cnt & _f1); }
        void _del_onrc0(T* p __bmdx_noarg) { if (b_detached() || !!(cnt & _m)) { return; } if (b_v0()) { try { delete p; } catch (...) {} return; } if (b_v1()) { if (!(flags & 0x2)) { _f_des1(p); } return; } ((_cref_handle2*)this)->_f_des2(p); }
        static void _del_ph(_cref_handle* ph __bmdx_noarg) throw() { if (!ph) { return; } if (ph->b_v0()) { _a::_sf_free(ph); return; } ph->_f_free(ph); }
      };
      _cref_handle h; _F_des _f_des2;
      _cref_handle2(t_cnt n0, _s_long flags_) : h(n0, flags_), _f_des2(h._f_des1) {}
    };
    typedef typename _cref_handle2::_cref_handle _t_cref_handle;
  public:


      // ref():
      //  a) on has_ref() == true, returns a ref. to valid object.
      //  b) on has_ref() == false, generates an exception.
      // The reference, returned by ref(), may be unsafe at the time
      //  when cref_t object is overwritten by another thread.
    const t_value& ref() const        throw(exc_ref) { t_value* p = const_cast<t_value*>(_p); if (!p) { throw exc_ref(); } return *p; }
    const t_value* ptr() const        throw() { return const_cast<const t_value*>(_p); }

      // ref_ts()():
      //  same as ref(), but the wrapped object reference
      //  remains valid and thread-safe until one of
      //    a) safe_refnc object, returned by ref_ts(), is destroyed,
      //    b) originating cref_t is destroyed.
      // ref_ts().xnc supplies the valid non-const reference to the wrapped object,
      //    for case of using cref_t as non-const reference handler.
      //    NOTE Original meaning of cref_t is "reference to constant object".
      //      Treating constant as variable is an agreement and may be unsafe.
      //      Only the client is responsible for logically correct modifications.
      // NOTE Accessing T with ref_ts()() or ref_ts().xnc holds the object lock
      //  until the current expression ends. If the method called is long operation,
      //  all other threads that access it via ref_ts() or manipulate cref_t of the same type
      //  will block until safe_refnc is destroyed and the lock it holds is released.
    struct safe_refnc
    {
      t_lock __lock;
      const cref_t& ref;
      t_value& xnc;

      const t_value& operator()() const        throw() { return xnc; }

      safe_refnc(const cref_t& r)        throw(exc_ref_ts) : __lock(r), ref(r), xnc(_ref(r)) {}

        // Copying and assignment do not generate exceptions,
        //  because the reference remains valid and locked.
      safe_refnc(const safe_refnc& x)        throw() : __lock(x.ref), ref(x.ref), xnc(x.xnc) {}
      void operator=(const safe_refnc& x)        throw() { safe_refnc temp(x); bmdx_str::words::swap_bytes(*this, temp); }
    private:
      static t_value& _ref(const cref_t& r __bmdx_noarg)        throw(exc_ref_ts) { t_value* p = r._pnonc_u(); if (!p) { throw exc_ref_ts(); } return *p; }
    };
    safe_refnc ref_ts() const        throw(exc_ref_ts) { return *this; }


      // Non-const pointer to object, returned without any locking.
      // NOTE Original meaning of cref_t is "reference to constant object".
      //   Treating constant as variable is an agreement and may be unsafe.
      //   Only the client is responsible for logically correct and synchronized modifications.
    t_value* _pnonc_u() const        throw() { return const_cast<t_value*>(_p); }


      // Smart pointer functionality.
      //   Allows to call the wrapped object's methods. If cref_t is empty, generates an exception.
      // NOTE Original meaning of cref_t is "reference to constant object".
      //   Treating constant as variable is an agreement and may be unsafe.
      //   Only the client is responsible for logically correct and synchronized modifications.
      // NOTE If cref_t may be concurrently overwritten, use p--->method() notation.
      //   See operator--.
    t_value* operator->() const        { t_value* p = const_cast<t_value*>(_p); if (!p) { throw exc_ref(); } return p; }


      // Returns a copy of this cref_t. Useful to safely call the wrapped object,
      // when the original cref_t may be concurrently modified.
      // cref_t<T> p; ... p--->method(); ... p--.ref().constant_method();
    cref_t operator--(int) const        throw() { return *this; }


      // false only if
      //  a) no-exception construction failed,
      //  b) after cref_t().
    bool has_ref() const        throw() { return !!_p; }
    operator bool() const        throw() { return !!_p; }

      // true if this object contains a strong reference, false - weak reference.
    bool is_own() const        throw() { return !!_ph; }

      // true if this object contains a strong reference in detached state (i.e. ref. counting continues, but object pointer is not deleted automatically, see _detach_u()).
    bool is_detached() const        throw() { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && _ph->b_detached(); }

      // true if this object is strong-referenced and may be passed between binary modules
      //  (ref. counting and destructor call, when needed, will be done correctly regardless of the module).
    bool is_cm() const        throw() { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && !_ph->b_v0(); }

      // n_refs():
      //  0 - unknown (weak ref., is_own() == false).
      //  >=1 - number of strong references (on is_own() == true).
    t_cnt n_refs() const        throw() { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph ? _ph->cnt & _m : 0; }

      // Return a pointer to the crit. sec. data object, used by this cref_t for locking.
    _critsec_data0_t<LockSelector>* pcsd0() const { return _ps; }
    typename critsec_t<LockSelector>::csdata* pcsd() const { return static_cast<typename critsec_t<LockSelector>::csdata*>(_ps); }


      // Object creation with 0..6 arguments.
      //    On success, the previous object reference is correctly removed, and the new is set on its place.
      //    On failure, the previous object reference remains unchanged.
      // NOTE create1 with Arg1 == t_value behaves same as copy().
    bool create0(bool no_exc)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create0(); } return false; }
    template<class Arg1> bool create1(bool no_exc, const Arg1& x1)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create1(); } return false; }
    template<class Arg1, class Arg2> bool create2(bool no_exc, const Arg1& x1, const Arg2& x2)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create2(); } return false; }
    template<class Arg1, class Arg2, class Arg3> bool create3(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create3(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4> bool create4(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3, x4); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create4(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5> bool create5(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3, x4, x5); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create5(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6> bool create6(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5, const Arg6& x6)        { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3, x4, x5, x6); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create6(); } return false; }

      // Cross-module object creation with 0..6 arguments.
      // pcsd, flags, no_exc: see cm_copy.
    bool cm_create0(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(); } catch (...) {} } else { try { p2 = new t_value(); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create0(); } return false; }
    template<class Arg1> bool cm_create1(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1); } catch (...) {} } else { try { p2 = new t_value(x1); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create1(); } return false; }
    template<class Arg1, class Arg2> bool cm_create2(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1, x2); } catch (...) {} } else { try { p2 = new t_value(x1, x2); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create2(); } return false; }
    template<class Arg1, class Arg2, class Arg3> bool cm_create3(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1, x2, x3); } catch (...) {} } else { try { p2 = new t_value(x1, x2, x3); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create3(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4> bool cm_create4(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1, x2, x3, x4); } catch (...) {} } else { try { p2 = new t_value(x1, x2, x3, x4); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create4(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5> bool cm_create5(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1, x2, x3, x4, x5); } catch (...) {} } else { try { p2 = new t_value(x1, x2, x3, x4, x5); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create5(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6> bool cm_create6(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5, const Arg6& x6)        { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _t_cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const t_value* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x1, x2, x3, x4, x5, x6); } catch (...) {} } else { try { p2 = new t_value(x1, x2, x3, x4, x5, x6); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create6(); } return false; }

    template<class I> struct iref
    {
        // Create object of type T, which is subclass of I. Assign it as strong reference to cref_t<I>.
        //  Return the result. If creation failed, an empty cref_t is returned (or exception generated on no_exc == false).
      static cref_t<I> create0(bool no_exc __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref0(); } return rx; }
      template<class Arg1> static cref_t<I> create1(bool no_exc, const Arg1& x1 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref1(); } return rx; }
      template<class Arg1, class Arg2> static cref_t<I> create2(bool no_exc, const Arg1& x1, const Arg2& x2 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref2(); } return rx; }
      template<class Arg1, class Arg2, class Arg3> static cref_t<I> create3(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref3(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4> static cref_t<I> create4(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref4(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5> static cref_t<I> create5(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4, x5); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref5(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6> static cref_t<I> create6(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5, const Arg6& x6 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref6(); } return rx; }

        // Create cross-module object of type T, which is subclass of I. Assign it as strong reference to cref_t<I>.
        //  Return the result. If creation failed, an empty cref_t is returned (or exception generated on no_exc == false).
        //  pcsd, flags: see cm_copy.
      static cref_t<I> cm_create0(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref0(); } return rx; }
      template<class Arg1> static cref_t<I> cm_create1(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref1(); } return rx; }
      template<class Arg1, class Arg2> static cref_t<I> cm_create2(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref2(); } return rx; }
      template<class Arg1, class Arg2, class Arg3> static cref_t<I> cm_create3(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref3(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4> static cref_t<I> cm_create4(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref4(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5> static cref_t<I> cm_create5(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4, x5); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref5(); } return rx; }
      template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6> static cref_t<I> cm_create6(typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5, const Arg6& x6 __bmdx_noarg)        { cref_t<I> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6); if (!rx.cm_assign(*pbase, (typename critsec_t<I>::csdata*)pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_iref6(); } return rx; }
    };


      // Copy object, and own the new copy.
      //  (The copy is automatically deleted when the last owning cref_t object is cleared, deleted or overwritten.)
    cref_t(const t_value& x, bool no_exc)
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { copy(x, no_exc); }

      // Set a strong (is_own_ == true), or a weak reference to an object (is_own_ == false).
      //  NOTE: on is_own_ == true,
      //    1) x must have been created dynamically.
      //    2) the current cref object must be the first existing owner.
    cref_t(const t_value& x, bool is_own_, bool no_exc)
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { if (is_own_) { _ph = (_t_cref_handle*)_new_pcnt(1); if (!_ph) { if (!no_exc) { throw exc_cc3(); }  return; } } _p = &x; }

      // Create a cross-module reference. Copy x, and own the new copy.
      //    The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      // pcsd, flags: see cm_copy.
    cref_t(const t_value& x, typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc)
      : _ph(0), _p(0), _ps(pcsd ? pcsd : typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { cm_copy(x, pcsd, flags, no_exc); }


      // NOTE The following 6 functions do not throw exceptions.
    cref_t()         throw()
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      {}

    cref_t(const cref_t& x)        throw()
      : _ph(0), _p(0), _ps(0)
      { t_lock __lock(x); _ps = x._ps; if (sizeof(__lock)) {} if (x._ph) { _ph = x._ph; ++_ph->cnt; } _p = x._p; }

    ~cref_t()        throw()
      { if (!_ph) { _p = 0; return; } t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); }

    cref_t& operator=(const cref_t& x)        throw()
    {
      cref_t temp(x);
      t_lock __lock(*this); if (sizeof(__lock)) {}
      _reset(); _p = temp._p; _ph = temp._ph; temp._p = 0; temp._ph = 0;
      _ps = temp._ps;
      return *this;
    }

      // NOTE Clearing does not reset the critical section used for object locking.
      //  (Assign another empty cref_t for that.)
    void clear()        throw()
      { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); }

      // Same as clear(), but does not delete the object if it's strongly referenced.
      //    The client becomes responsible for object lifetime.
      // NOTE Detaching influences all other cref_t's with strong reference to the same object -
      //    they continue to reference that object, but do not delete it when the counter becomes 0.
      //    This allows for ref.-counting with "manual" control of object lifetime.
      //    For example, the client may wait for n_refs() == 1, then delete the object.
      //    (cref_t may safely go out of scope any time.)
    void _detach_u()        throw()
      { t_lock __lock(*this); if (sizeof(__lock)) {} if (_ph) { _ph->cnt |= _f1; } _reset(); }


      // Assigns x (by reference) to this cref_t, and/or changes ownership on x.
      //    If weak ref. is created (is_own_ == false), the function always succeeds.
      //    If is_own_ == true, and the currently owned object is strong-referenced x, assign() does nothing (if cross-module ownership is enabled, it's not changed).
      // Returns:
      //    true on success,
      //    false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      // NOTE assign() may be used to change the kind of ownership on its referenced object.
      //    If the last strong ref. was changed to weak, the object may be deleted,
      //    but its pointer is still kept.
      // NOTE assign() does not copy x in any way. Do not assign references to temporary objects.
    bool assign(const t_value& x, bool is_own_, bool no_exc)
    {
      t_lock __lock(*this); if (sizeof(__lock)) {}
      if (&x == _p)
      {
        if (is_own_ == is_own()) { return true; } if (!is_own_) { _reset(); _p = &x; return true; }
        _ph = (_t_cref_handle*)_new_pcnt(1); if (!_ph) { if (!no_exc) {  throw exc_assign(); } return false; }
        return true;
      }
      else
      {
        if (!is_own_) { _reset(); _p = &x; return true; }
        t_cnt* pcnt2 = _new_pcnt(1); if (!pcnt2) { if (!no_exc) {  throw exc_assign(); } return false; }
        _reset(); _p = &x; _ph = (_t_cref_handle*)pcnt2;
        return true;
      }
    }

      // Copies the object and the copy becomes cref_t's own object.
      // Returns true on success, false on failure with no_exc == true,
      //    or generates exception (exc_copy) on failure with no_exc == false.
      // On failure, the current reference is kept.
      // NOTE x is copied without any locking.
      //  The present cref_t object lock is set only after,
      //  when setting the new object reference in the cref_t object.
      //  If locking during copying is necessary, use cm_copy with pcsd == 0.
    bool copy(const t_value& x, bool no_exc)
    {
      t_cnt* pcnt2(0); const t_value* p2(0);
      try { p2 = new t_value(x); } catch (...) {} pcnt2 = _new_pcnt(1);
      if (p2 && pcnt2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = (_t_cref_handle*)pcnt2; return true; }
      try { delete p2; } catch (...) {} _del_pcnt(pcnt2);
      if (!no_exc) { throw exc_copy(); }
      return false;
    }

      // Assigns x (by reference) to this cref_t, and takes ownership on x.
      //    The previous object, if existed, is released.
      //    (If the previous object was already strong-referenced x, it's detached instead of being released,
      //    so that the current cref_t becomes single owner of the object.)
      // pcsd, flags: see cm_copy.
      // The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      // Returns:
      //    true on success,
      //    false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      //    On failure, the current reference is kept.
      // NOTE cm_assign() does not copy x in any way. Do not assign references to temporary objects.
    bool cm_assign(const t_value& x, typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc)
    {
      if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
      _t_cref_handle* ph2 = _new_ph(1, flags);
      if (ph2) { t_lock __lock(*this); if (sizeof(__lock)) {} if (_p == &x) { _detach_u(); } _reset(); _p = &x; _ph = ph2; _ps = pcsd; return true; }
      _del_ph(ph2);
      if (!no_exc) { throw exc_cm_assign(); }
      return false;
    }

      // Copies x, and the copy becomes cref_t's own object.
      //    The previous object, if existed, is released.
      //    The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      //  pcsd (0 for default: use common per-type critsec_t<T>).
      //      pcsd != 0: specific crit. sec. data instance. Must remain valid until the last referring cref_t is released.
      //      pcsd == 0: use critsec_t<T>'s ::pdefcsd().
      //  flags OR-ed (0 for default):
      //    0x1 - reserved, must be 0.
      //    0x2 - disable ~T(). On ref. count reaching 0, only memory occupied by the object
      //      and ref. counter (_t_cref_handle) is freed.
      //    0x4 - protect x copying by pcsd.
      //      When copying completed, the lock on pcsd is released,
      //      and cref_t modification is done under lock on cref_t's csd.
      //      The last step is setting cref_t's csd = pcsd.
      // Returns:
      //    true on success,
      //    false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      //    On failure, the current reference is kept.
      //  NOTE The binary module that has created the object copy, must remain valid (prevent unloading)
      //  until its last object is released, no matter in which module it's used.
    bool cm_copy(const t_value& x, typename critsec_t<T>::csdata* pcsd, _s_long flags, bool no_exc)
    {
      if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
      _t_cref_handle* ph2 = _new_ph(1, flags);
      if (ph2)
      {
        const t_value* p2(0);
        if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new t_value(x); } catch (...) {} } else { try { p2 = new t_value(x); } catch (...) {} }
        if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; }
        _del_ph(ph2);
      }
      if (!no_exc) { throw exc_cm_copy(); }
      return false;
    }

  private:
    _t_cref_handle* _ph;
    mutable volatile const t_value* _p;
    _critsec_data0_t<LockSelector>* _ps;

    void _reset(__bmdx_noarg1) throw() { if (_ph) { --_ph->cnt; _ph->_del_onrc0(const_cast<t_value*>(_p)); if ((_ph->cnt & _m) == 0) { _del_ph(_ph); } _ph = 0; } _p = 0; }
    static t_cnt* _new_pcnt(t_cnt n0 __bmdx_noarg) throw() { t_cnt* p = (t_cnt*)_a::_sf_alloc(sizeof(t_cnt)); if (p) { *p = n0; }  return p; }
    static _t_cref_handle* _new_ph(t_cnt n0, _s_long flags __bmdx_noarg) throw() { _t_cref_handle* p = (_t_cref_handle*)_a::_sf_alloc(sizeof(_t_cref_handle)); if (p) { new (p) _t_cref_handle(n0, (flags & 0x3)); } return p; }
    static void _del_pcnt(t_cnt* pcnt __bmdx_noarg) throw() { if (!pcnt) { return; } _a::_sf_free(pcnt); }
    static void _del_ph(_t_cref_handle* ph __bmdx_noarg) throw() { if (!ph) { return; } _t_cref_handle::_del_ph(ph); }
  };




    // Multithread creates and holds several threads each executing the same user routine (defined as multithread::ictx::mt_proc override),
    //    with same input argument, but different thread index.
    //  Depending on specified mode, multithread waits for all threads to complete or returns immediately.
    // NOTE Unlike threadctl, multithread may not be copied or assigned.
    //  If multithread is passed by reference or as wrapped object into another binary module,
    //    only the following functions are safe to call: operator bool(), n_run(), nth(), b_exc(), signal_stop().
  struct multithread
  {
    struct ictx; struct icb; struct cbarg;
  private: template<class C> struct _starter_t; public:

      // Polymorphic callback reference, for passing into start() and multithread().
      //  Default-constructed value (cbarg()) may be used to skip cb argument in start(...) and multithread(...).
    struct cbarg
    {
      cref_t<icb> rx;
      bool b_ok; // true mans successful construction
      cbarg(__bmdx_noarg1) throw() : b_ok(true) {}
      cbarg(const cbarg& x_ __bmdx_noarg) throw() : rx(x_.rx), b_ok(x_.b_ok) {}
      template<class Cb> cbarg(const Cb& x_ __bmdx_noarg) throw() : b_ok(false) { try { icb* pbase = new Cb(x_); if (!rx.cm_assign(*pbase, 0, 0, true)) { delete pbase; } } catch (...) {} b_ok = !!rx; }
    };


    multithread(__bmdx_noarg1) : _ps(0) {}

      // NOTE On destruction, the multithread object automatically sets b_stop() flag to all its running threads
      //  (same as signal_stop()) and then detaches from them.
      //  If this has is not desired, the client should call detach() before multithread object is destroyed.
    ~multithread() throw(__bmdx_noargt1) { _clear(); }

      // Start multithread.
      //  nth: 1..5000.
      //  mode:
      //    1 - initialize only, 2 - initialize+start threads, 3 - initialize+start+detach (use custom callbacks for interop.),
      //    4 - strong call (initialize+start+wait for completion).
      //  c (custom context):
      //    1) inherits from multithread::ictx, 2) implements mt_proc, 3) contains references or copies of all user data needed for mt_proc.
      //  cb (client callback):
      //    a) cb is automatically created from anything inheriting from icb.
      //    b) if cb is not specified or empty, c.rcb() is used as callback. E.g. it may have been set with ictx::set_cb.
    template<class C>
    multithread(_s_long nth, _s_long mode, const C& c, cbarg cb = cbarg(__bmdx_noargv1), _s_long priority = 4 __bmdx_noarg) : _ps(0)
      { this->start(nth, mode, c, cb, priority); }

      // See constructor for args.
      // Returns:
      //    1 - success, 0 - already started, -1 - bad args, -2 - failed,
      //    -4 (only in strong call (mode 4)) - at least one of mt_proc caused C++ exception. See also b_exc().
    template<class C>
    _s_long start(_s_long nth, _s_long mode, const C& c, cbarg cb = cbarg(__bmdx_noargv1), _s_long priority = 4 __bmdx_noarg)
    {
      const ictx* pcbase = &c; // NOTE C must inherit from multithread::ictx
      if (pcbase) {}
      if (!(nth >= 1 && nth <= 5000 && mode >= 1 && mode <= 4)) { return -1; }
      if (!cb.b_ok) { return -2; }
      if (_rth) { return 0; }
      if (!_rth.cm_create0(0, 0, 1)) { return -2; }
      _rb_run.clear();
      if (!_rb_run.cm_create0(0, 0, 1)) { return -2; }
      cpparray_t<threadctl>& a = *_rth._pnonc_u();
      if (!a.resize(nth)) { _clear(); return -2; }
      try { _ps = new _starter_t<C>(c, priority); _ps->pc()->_pm = this; if (cb.rx) { _ps->pc()->_cb = cb.rx; } _ps->pc()->_nth = nth; _ps->pc()->_rth = _rth; _ps->pc()->_pr = priority; _ps->pc()->_rb_run = _rb_run; } catch (...) {}
        if (!_ps) { _clear(); return -2; }
      if (mode != 1) { return start(mode); }
      return 1;
    }

      // Start multithread, constructed in mode 1.
      //    On failure, threads that have been created, are stopped, but the initialization context is not removed,
      //    so the client may a) retry start, or b) detach and reuse multithread object.
      //  mode:
      //    2 - start threads, 3 - start+detach (use custom callbacks for interop.),
      //    4 - strong call (start+wait for completion).
      // Returns:
      //    1 - success, 0 - already started, -1 - bad args, -2 - failed,
      //    -4 (only in strong call (mode 4)) - at least one of mt_proc caused C++ exception. See also b_exc().
    _s_long start(_s_long mode __bmdx_noarg)
    {
      if (!(_ps && _rb_run)) { return _rth ? 0 : -2; }
      if (!(mode >= 2 && mode <= 4)) { return -1; }
      char& brun = *_rb_run._pnonc_u();
      cpparray_t<threadctl>& a = *_rth._pnonc_u();
      cpparray_t<ictx*> cc; if (!cc.resize(a.n())) { return -2; }
        _s_long n = 0;
        for (_s_ll i = 0; i < a.n(); ++i) { cc[i] = _ps->start(a[i], _s_long(i)); n += !!cc[i]; }
          if (n < a.n()) { for (_s_ll i = 0; i < a.n(); ++i) { a[i].stop(-1); } return -2; }
        for (_s_ll i = 0; i < a.n(); ++i) { cc[i]->_rb_run = _rb_run; }
        if (_ps->pr() <= 4) { for (_s_ll i = 0; i < a.n(); ++i) { a[i].set_priority(_ps->pr()); } }
        brun = 1;
      _clear_ps();
      if (mode == 2) { return 1; }
      if (mode == 3) { detach(); return 1; }
      while (*this) { sleep_mcs(100); }
      return brun == 2 ? -4 : 1;
    }

      // True if at least one thread runs.
      //  After successful start in mode 2 or 3,
      //  the client may wait for the value to become false as signal for full job completion.
    operator bool() const throw(__bmdx_noargt1) { return n_run() > 0; }

      // The number of currently running threads [0..nth()].
      //  After successful start in mode 2 or 3,
      //  the client may wait for n_run() == 0 as signal for full job completion.
    _s_long n_run(__bmdx_noarg1) const { if (!_rth) { return 0; } _s_long n = 0; for (_s_long i = 0; i < _rth.ref().n(); ++i) { n += bool(_rth.ref()[i]); } return n; }

      // The number of thread controls created.
      //  The value is set by start(...) or multithread(...), and does not change until restart.
    _s_long nth(__bmdx_noarg1) const { if (!_rth) { return 0; } return _s_long(_rth.ref().n()); }

      // Reference to all thread controls. May be empty, depending on multithread state.
      //  The value is set by start(...) or multithread(...), and does not change until restart.
    cref_t<cpparray_t<threadctl> > rth(__bmdx_noarg1) const { return _rth; }

      // Becomes true if, during threads operation, any of them (ictx::mt_proc) causes a C++ exception.
      //  After all threads exit, the value does not change till new call to start().
    bool b_exc(__bmdx_noarg1) const { return _rb_run ? _rb_run.ref() == 2 : false; }

      // Sets b_stop() flag in all referenced threads.
      //  May be used to notify ictx::mt_proc that it should to exit w/o completing its task.
      //  NOTE signal_stop() does not detach from thread controls. Call clear() for that.
    void signal_stop(__bmdx_noarg1) { if (_rth) { for (_s_long i = 0; i < _rth.ref().n(); ++i) { (*_rth._pnonc_u())[i].signal_stop(); } } }

      // Detaches from all previous threads and data.
      //    Even if any threads are still running, they are not notified, and the multithread object
      //    is not responsible for their operation anymore.
      //    nth() becomes 0.
      //    After detach(), the multithread object may be reused.
    void detach(__bmdx_noarg1) { if (_rth) { for (_s_long i = 0; i < _rth.ref().n(); ++i) { (*_rth._pnonc_u())[i].stop(-3); } } _clear(); }

      // Detaches from all previous threads and data.
      //    Before that, threads that are still running are notified by setting their b_stop() flag.
      //    nth() becomes 0.
      //    After clear(), the multithread object may be reused.
    void clear(__bmdx_noarg1) { _clear(); }

      // Base class for user-defined routine for parallel task.
      //    A custom context (ictx subclass) must implement only void mt_proc().
      // NOTE Most custom contexts require access to user-specific data and parameters.
      //    The parameters are simply data fields in the context itself.
      //    Since custom context is many times (at least nth() + 1) copied by value,
      //    it should address any large data by reference.
      //    The client ensures that by-reference objects live longer than multithread that uses them.
      // NOTE The client may create an ictx and call mt_proc directly (strong call without multithread).
      //    nth() == 1, ith() == 0 by default.
      //    This is especially useful if the client code has to handle case of multithread failure,
      //    and retry in single-threaded mode.
    struct ictx : threadctl::ctx_base
    {
      typedef _s_long s_long; typedef _s_ll s_ll;


      virtual void mt_proc() = 0;


      ictx(__bmdx_noarg1) throw() : _pm(0), _cb(), _nth(1), _ith(0), _pr(4), _rb_run(), _rth()  {}

        // Parent multithread object.
        //  This pointer is valid only if the client did not detach from the running threads.
      const multithread* pm(__bmdx_noarg1) const throw() { return _pm; }

        // Total number of running threads.
      _s_long nth(__bmdx_noarg1) const throw() { return _nth; }

        // The current thread number [0..nth()).
      _s_long ith(__bmdx_noarg1) const throw() { return _ith; }

        // Controls for all the running threads.
        //  NOTE The array may be empty if the client called mt_proc directly (strong call without multithread).
      cref_t<cpparray_t<threadctl> > rth(__bmdx_noarg1) const throw() { return _rth; }

        // User callback, may be set or replaced at any time (before/during thread run or on direct mt_proc call).
      cref_t<icb> rcb(__bmdx_noarg1) const throw() { return _cb; }
      void set_cb(const cref_t<icb>& x __bmdx_noarg) throw() { _cb = x; }

        // Helper function. Divides n into nth() ranges.
        //  Calculates and sets i0 and i2 to half-open range [i0..i2),
        //  corresponding to the current thread index (ith()).
        //  If such i0 or i2 is out of [0..n), they are adjusted to fit into [0..n),
        //  so the resulting range may be empty or shorter than in other threads.
        // Returns:
        //    true - the resulting [i0..i2) is not empty.
        //    false - in any other case.
      template<class N> inline bool range(s_ll n, N& i0, N& i2) const
      {
        i0 = 0; i2 = 0;
        if (n <= 0) { return false; }
        s_ll dn = (n + _nth - 1) / _nth;
        i0 = N(_ith * dn); if (i0 >= N(n)) { i0 = N(n); }
        i2 = N((_ith + 1) * dn); if (i2 >= N(n)) { i2 = N(n); }
        return i2 > i0;
      }

    private:
      friend struct multithread;
      virtual void _thread_proc()
      {
        while (!(_rb_run && _rb_run.ref() != 0)) { if (b_stop()) { return; } sleep_mcs(_ith < _nth - 1 ? 100 : 10); }
        if (_pr > 4 && _ith == _nth - 1) { for (_s_long i = _nth - 1; i >= 0; --i) { (*_rth._pnonc_u())[i].set_priority(_pr); } }
        cref_t<icb> _rcb0 = _cb; icb* pcb0 = _rcb0._pnonc_u();
        if (pcb0) { try { pcb0->cb_proc(this, _ith, -1, 0); } catch (...) {} }
        bool b = false; try { mt_proc(); b = true; } catch (...) {}
        if (!b) { *_rb_run._pnonc_u() = 2; }
        if (pcb0) { try { pcb0->cb_proc(this, _ith, -2, b ? 1 : 0); } catch (...) {} }
        if (pcb0 && _ith == _nth - 1) { while (true) { if (b_stop()) { return; } _s_long n = 0; for (_s_long i = 0; i < _nth; ++i) { n += bool(_rth.ref()[i]); } if (n <= 1) { break; } sleep_mcs(100); } pcb0->cb_proc(this, _ith, -3, 0); }
      }
      multithread* _pm; cref_t<icb> _cb; _s_long _nth, _ith, _pr; cref_t<char> _rb_run; cref_t<cpparray_t<threadctl> > _rth;
    };

      // Base class for user-defined callback implementation.
      //  Called multiple times from multiple threads (unless each thread replaces it using ictx::set_cb).
      //  A user-defined callback (ictx subclass) must implement only void cb_proc(...).
      // NOTE Callbacks may contain user-specific data and parameters.
      //  Implementation should take into account that callback object, passed into
      //  start(...) or multithread(...), is once copied, to create a polymorphic reference common to all threads.
    struct icb
    {
      typedef ictx t_ictx;
      typedef _s_long s_long; typedef _s_ll s_ll;


        // Callback from a thread to client.
        //  pctx - sending ictx, may be cast into user thread proc. class if necessary.
        //  msgtype <= 0 - multithread events, > 0 - user events.
        //  Multithread events msgtypes:
        //    -1 - i-th job started,
        //    -2 - i-th job completed (msg == 1: success, 0: failure),
        //    -3 - all jobs completed.
      virtual void cb_proc(t_ictx* pctx, s_long ith, s_long msgtype, s_ll msg) = 0;


      virtual ~icb() {}
    };

  private:
    struct _starter_base { virtual ictx* pc() = 0; virtual _s_long pr() const = 0; virtual ictx* start(threadctl& tc, _s_long ith) = 0; virtual ~_starter_base() {} };
    template<class C> struct _starter_t : _starter_base
    {
      C c;
      _s_long priority;
      _starter_t(const C& c_, _s_long priority_ __bmdx_noarg) : c(c_), priority(priority_) { }
      virtual ictx* pc() { return &c; }
      virtual _s_long pr() const { return priority; }
      virtual ictx* start(threadctl& tc, _s_long ith)
      {
        ictx* pbase = 0;
        try { pbase = new C(c); } catch (...) {} // C must inherit from multithread::ictx
          if (!pbase) { return 0; }
        pbase->_ith = ith;
        pbase->_set_b_own(1);
        bool b = tc.start(pbase);
        if (!b) { try { delete pbase; } catch (...) {} pbase = 0; }
        return pbase;
      }
    };
    cref_t<cpparray_t<threadctl> > _rth; _starter_base* _ps; cref_t<char> _rb_run;
    void _clear_ps(__bmdx_noarg1) { if (_ps) { try { delete _ps; } catch (...) {} _ps = 0; } }
    void _clear(__bmdx_noarg1) { _rth.clear(); _clear_ps(); _rb_run.clear(); }
    multithread(const multithread&); void operator=(const multithread&);
  };


}




#undef _s_long
#undef _s_ll
#undef _u_ll

#endif // bmdx_cpiomt_H



// NOTE This is EXPERIMENTAL part of the library.
//  Define bmdx_part_shm 1 to use this part.
//  Differences from other parts:
//  1. Interfaces will change in the newer versions.
//  2. The code is not automatically tested and may contain errors
//    or be incompatible with certain platforms or with its own prev. versions.

#if bmdx_part_shm
#ifndef bmdx_shmem_H
#define bmdx_shmem_H

#include <deque>
#include <map>


#ifdef _bmdxpl_Psx
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>

  #if 0
  #elif __APPLE__ && __MACH__
  #elif defined(__FreeBSD__)
  #elif defined(__SUNPRO_CC) || defined (__sun)
    #include <thread.h>
    #include <errno.h>
  #elif defined(__ANDROID__)
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <linux/ashmem.h>
    #include <linux/ioctl.h>
  #else
    #include <sys/file.h>
  #endif
#endif



  // Main namespace.
namespace bmdx_shm
{


namespace _bmdx_shm { namespace _api {} }
using namespace _bmdx_shm::_api;



typedef bmdx_str::flstr_t<300> t_name_shm;
typedef t_name_shm (*f_shm_name_prefix)();

  // System-global object name prefixes may be customized at compile time.
#ifndef __bmdx_shm_f_prefix
#define __bmdx_shm_f_prefix

  #ifdef _bmdxpl_Wnds
    static t_name_shm f_prefix_critsec_gn() { return "CS\t"; }
    static t_name_shm f_prefix_shmobj2s() { return "Local\\"; }
  #endif
  #ifdef _bmdxpl_Psx
    #if 0
    #elif __APPLE__ && __MACH__ && TARGET_OS_IPHONE
      static t_name_shm f_prefix_critsec_gn() { return "/private/var/tmp/CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return "/"; }
    #elif __APPLE__ && __MACH__
      static t_name_shm f_prefix_critsec_gn() { return "/tmp/CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return "/"; }
    #elif defined(__FreeBSD__)
      static t_name_shm f_prefix_critsec_gn() { return "/tmp/CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return "/"; }
    #elif defined(__SUNPRO_CC) || defined (__sun)
      static t_name_shm f_prefix_critsec_gn() { return "/tmp/CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return "/tmp/"; }
    #elif defined(__ANDROID__)
      static t_name_shm f_prefix_critsec_gn() { return t_name_shm() << '\0' << "CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return ""; }
    #else
      static t_name_shm f_prefix_critsec_gn() { return "/dev/shm/CS\t"; }
      static t_name_shm f_prefix_shmobj2s() { return "/dev/shm/"; }
    #endif
  #endif

#endif


  // Private part.
namespace _bmdx_shm
{

using namespace bmdx;
using bmdx_meta::u_long;
using bmdx_meta::s_long;
using bmdx_meta::s_ll;
using namespace bmdx_str::words;


#if defined(__ANDROID__)

    // Data structure for transferring a single file descriptor via socket.
  struct _socket_msg_1fd
  {
      // fd specifies the initial value of the descriptor field in the message.
    _socket_msg_1fd(int fd = -1)        { _ndesc[0] = 1; _v1.iov_base = _ndesc; _v1.iov_len = sizeof(_ndesc); _m.msg_name = 0; _m.msg_namelen = 0; _m.msg_iov = &_v1; _m.msg_iovlen = 1; _m.msg_flags = MSG_DONTWAIT; _m.msg_control = _cmsg.buf; _m.msg_controllen = sizeof(_cmsg.buf);  struct cmsghdr* pcm1 = CMSG_FIRSTHDR(&_m);  pcm1->cmsg_level = SOL_SOCKET; pcm1->cmsg_type = SCM_RIGHTS; pcm1->cmsg_len = CMSG_LEN(sizeof(int) * 1);  int* pdata = (int*)CMSG_DATA(pcm1); pdata[0] = fd; }

    int fd() const        { int* pdata = (int*)CMSG_DATA(CMSG_FIRSTHDR(&_m)); return pdata[0]; }
    int& fd()        { int* pdata = (int*)CMSG_DATA(CMSG_FIRSTHDR(&_m)); return pdata[0]; }

    struct msghdr& m()        { return _m; }
    const struct msghdr& m() const        { return _m; }

  private:
    char _ndesc[1]; // 1 byte of data (== 1, indicating that 1 file descriptor is sent together with that byte, as "ancillary" data)
    union { char buf[CMSG_SPACE(sizeof(int) * 1)]; struct cmsghdr __align; } _cmsg; struct iovec _v1; struct msghdr _m;
  };



    // Data transfer via abstract socket (non-blocking).
  struct _asocket_io
  {
    typedef t_name_shm t_name;

    bool b_srv() const { return _b_srv; }
    bool b_connected() const { return _state == 2; }
    s_long res() const { return _res; }

      // Factual socket name, used in socket creation. Starts with char '\0'. See key() for original name processing scheme.
    const t_name& name() const        { return _name; }
      // -1..99999. See key() for original socket name processing scheme.
    s_long n_role() const { return _n_role; }

      // Socket name: limit_length('\0' + name_prefix + str(n_role >= 0 ? n_role % 100000 : "") + name, sizeof(sockaddr_un::sun_path)).
    static t_name key(const t_name& name_orig, s_long n_role = -1, const char* name_prefix = "")        { t_name _name; _name += '\0'; _name += name_prefix; if (n_role >= 0) { _name += n_role % 100000; } _name += name_orig; if (_name.n() > s_long(sizeof(_a.sun_path))) { _name.resize(s_long(sizeof(_a.sun_path))); } return _name; }

      // b_srv true: server side, false: client side. See key() for name processing scheme.
    _asocket_io(bool b_srv, const t_name& name_orig, s_long n_role = -1, const char* name_prefix = "")        { _name = key(name_orig, n_role, name_prefix); _n_role = n_role < 0 ? -1 : n_role % 100000; _b_srv = b_srv; _h1 = _h2 = -1; _state = 0;  _state = 0; _res = -4; _a.sun_family = PF_LOCAL; memcpy(&_a.sun_path, _name.c_str(), size_t(_name.n())); }


      // Automatically connects or accepts if necessary, then sends message m.
      //  b_close == true: close connection after message being sent successfully.
      //  Returns:
      //    1 - completed.
      //    0 - partially completed. Sleep and retry.
      //    -1 - Server: no peer connected yet. Client: server is not reponding (does not exist?). Maybe sleep and retry.
      //    -2 - peer socket failure during sending. close_h2() and retry.
      //    -3 - server socket failure. close_h1h2() and retry.
      // The result is also returned by res().
    int try_send(struct msghdr& m, bool b_close)
    {
      if (_state == 0 || _state == 1) { try_connect(); }
      if (_state == 1) { _res = -1; return _res; }
      if (_state != 2) { _res = -3; return _res; }
      int res = sendmsg(_h2, &m, MSG_DONTWAIT | MSG_NOSIGNAL);
      if (res >= 0) { _res = 1; if (b_close) { close_h2(); } return _res; }
      if (res != -1) { _res = -2; return _res; }
      if (errno == EAGAIN || errno == EWOULDBLOCK) { _res = 0; return _res; }
      _res = -2; return _res;
    }

      // Automatically connects or accepts if necessary, then receives a new message into m.
      //  b_close == true: close connection after message being received in full.
      //  Returns:
      //    1 - completed.
      //    0 - no message yet sent by peer on established connection, or receival is partially completed. Sleep and retry.
      //    -1 - Server: no peer connected yet. Client: server is not reponding (does not exist?). Maybe sleep and retry.
      //    -2 - peer socket failure during receival. close_h2() and retry.
      //    -3 - server socket failure. close_h1h2() and retry.
      //  The result is also returned by res().
    int try_receive(msghdr& m, bool b_close)
    {
      if (_state == 0 || _state == 1) { try_connect(); }
      if (_state == 1) { _res = -1; return _res; }
      if (_state != 2) { _res = -3; return _res; }
      int res = recvmsg(_h2, &m, 0);
      if (res > 0) { if (b_close) { close_h2(); } _res = 1; return _res; }
      if (res == 0) { _res = 0; return _res; }
      if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { _res = 0; return _res; }
      _res = -2; return _res;
    }

      // Sets non-blocking behavior for file or socket descriptor.
    static bool fcntl_set_nonblock(int fd)        { int f = fcntl(fd, F_GETFL); if (f != -1) { f = fcntl(fd, F_SETFL, f | O_NONBLOCK); } return f == 0; }

      // Try to automatically create a socket, bind/listen/accept or connect.
      // 1 - done. try_send, try_receive ready to transfer data.
      // -1 - Server: no peer connected yet (accept returned "wait" state). Client: server is not reponding (does not exist?), or connection pending. Maybe sleep and retry.
      // -2 - Socket accept failed (permission or other error). h2 is closed, h1 may be valid.
      // -3 - Socket create/bind/listen failed. h1, h2 are closed.
    int try_connect()
    {
      if (_b_srv)
      {
        if (_state == 0)
        {
          _h1 = ::socket(PF_LOCAL, SOCK_STREAM, 0);
          if (!fcntl_set_nonblock(_h1)) { close_h1h2(); _res = -3; return _res; }
          int res_b = ::bind(_h1, (sockaddr*)&_a, sizeof(sa_family_t) + size_t(_name.n()));
          int res_l = ::listen(_h1, 20);
          if (res_b || res_l) { close_h1h2(); _res = -3; return _res; }
          _state = 1;
        }
        if (_state == 1)
        {
          _h2 = ::accept(_h1, 0, 0);
          if (_h2 == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { _res = -1; return _res; }
          if (_h2 == -1) { close_h2(); _res = -2; return _res; }
          _state = 2;
        }
      }
      else
      {
        if (_state == 0)
        {
          _h2 = ::socket(PF_LOCAL, SOCK_STREAM, 0);
          if (!fcntl_set_nonblock(_h2)) { close_h1h2(); _res = -3; return _res; }
          _state = 1;
        }
        if (_state == 1)
        {
          int res_c = ::connect(_h2, (sockaddr*)&_a, sizeof(sa_family_t) + size_t(_name.n()));
          if (res_c != 0 && (errno == ECONNREFUSED || errno == EALREADY || errno == EAGAIN)) { _res = -1; return _res; }
          if (res_c != 0) { close_h2(); _state = 0; _res = -2; return _res; }
          _state = 2;
        }
      }
      if (_state == 2) { _res = 1; return _res; }
      close_h1h2(); _state = 0; _res = -3;
      return _res;
    }

      // close_h2, close_h1h2 change state, but don't modify res(). Assume they always succeed.
    void close_h2() { if (_h2 != -1) { ::close(_h2); } _h2 = -1; if (_state > 1) { _state = 1; } }
    void close_h1h2() { if (_h2 != -1) { ::close(_h2); } _h2 = -1; if (_h1 != -1) { ::close(_h1); _h1 = -1; } if (_state != 0) { _state = 0; } }
    ~_asocket_io() { close_h1h2(); }

  private: bool _b_srv; int _h1, _h2; t_name _name; sockaddr_un _a; mutable s_long _state; mutable s_long _res; s_long _n_role;
  };



    // Service for sharing file descriptors between processes.
    // Associates a name with a file descriptor.
    // Usage: create a temp. instance of _fd_sharing, call its functions.
    //  (Multiple _fd_sharing refer to the same service.)
  struct _fd_sharing
  {
    typedef t_name_shm t_name;

      // n_role_max + 1: max. number of concurrent sources with different names, tried by fd_search when looking for existing fd in the system.
    enum { n_role_max = 2 };

      // Get a duplicate of fd, associated with the given name.
      //  t_retry_mcs:
      //      < 0 - check local hash only, do not create search task,
      //      == 0 - check local hash, create a search task if name not found, try once connect and get fd w/o sleeping,
      //      > 0 - create a search task, try connect / get fd during this period.
      //  Once the search task is created, after it got a file descriptor from other process,
      //      it starts exposing it automatically.
      //  n_role_expose: used only if creating search task.
      //      When search completes successfully, the job is automatically switched into expose mode,
      //      with the specified role number.
      // Returns:
      //    >= 0 - valid fd. NOTE The client takes ownership on that fd, and should close it when finished its use.
      //    -1 - "invalid" fd is associated with this name.
      //    -2 - failure (multiple cases).
      //    -3 (only on t_retry_mcs < 0)  - name not found.
      //    -4 - search in progress, file descriptor is still unknown. On t_retry_mcs == 0, this means that curently no fd exposed by any process.
      //    -5 - file descriptor exists, but too large delay during transfer occured. The client must assume that the descriptor exists and call fd_search again later.
    int fd_search(const t_name& name_orig, s_long t_retry_mcs, s_long n_role_expose)        throw() { return ((p_fd_search)pff[0])(*this, name_orig, t_retry_mcs, n_role_expose); }

      // Try to share a duplicate of the given *io_fd with other processes under the given name.
      //  fd:
      //    >= 0. Associate a duplicate of this fd with the name.
      //    -1. -1 Associate an "invalid" (-1) descriptor value with the name.
      //    -2. Do not process fd, n_role, b_overwrite, instead, if name exists, set only b_expose for this name.
      //  n_role: [-1..n_role_max]. A number to prepend to original name. -1 - do not prepend anything.
      //  b_expose:
      //    true - if an association exists or just created, enable exposing the asociated fd to other processes.
      //    false - disable exposing is it was enabled (but do not remove name-fd association as such).
      // Returns:
      //  1 - success.
      //  0 (only on b_overwrite == false) - the name-descriptor association already exists, use fd_search to get it.
      //  -1 - wrong fd (<= -3).
      //  -2 - failure (multiple cases).
      //  -3 (only on fd == -2) - failed to enable exposing - name not found.
    int fd_expose(const t_name& name_orig, int fd, s_long n_role, bool b_overwrite, bool b_expose)        throw() { return ((p_fd_expose)pff[1])(*this, name_orig, fd, n_role, b_overwrite, b_expose); }

      // Remove the name and all associated tasks, close file descriptor if it exists.
      //  1 - removed.
      //  0 - object with the given name does not exist.
      //  -2 - failure.
    int fd_remove(const t_name& name_orig)        throw() { return ((p_fd_remove)pff[2])(*this, name_orig); }

      // Remove all names and search tasks, stop (join with) service thread.
    void reset(bool b_wait)        throw() { ((p_reset)pff[3])(*this, b_wait); }

    _fd_sharing() { pff[0] = (void*)&_fd_search; pff[1] = (void*)&_fd_expose; pff[2] = (void*)&_fd_remove; pff[3] = (void*)&_reset; }
private:
    typedef int (*p_fd_search)(_fd_sharing& par, const t_name& name_orig, s_long t_retry_mcs, s_long n_role_expose);
    typedef int (*p_fd_expose)(_fd_sharing& par, const t_name& name_orig, int fd, s_long n_role, bool b_overwrite, bool b_expose);
    typedef int (*p_fd_remove)(_fd_sharing& par, const t_name& name_orig);
    typedef void (*p_reset)(_fd_sharing& par, bool b_wait);
    void* pff[4];

    struct job
    {
      s_long state; // 0 - fd is unknown and search disabled, 1 - fd is unknown, but searching, 2 - fd is known (+ sending fd to other processes on b_mayexpose == true)
      volatile int fd; // may be read w/o locking; -1 - "no file", -2 - "fd still unknown", >=0 - known fd
      bool b_mayexpose;
      bool b_conn1;
      s_long nrexp;
      s_long nrsrch;
      critsec_t<job>::csdata csd; // individual lock for this job instance
      t_name name; // original name, associated with file descriptor (not formatted/prefixed to serve as socket name)
      _asocket_io s;
      _socket_msg_1fd m1;
      job(const t_name& name_, s_long n_role_expose_) : name(name_), s(false, name_, -1, "F\t") { state = 0; fd = -2; b_mayexpose = true; nrexp = n_role_expose_; b_conn1 = true; nrsrch = -1; if (nrexp < 0) { nrexp = -1; } else if (nrexp > n_role_max) { nrexp = n_role_max; } } // cm_create0 may gen. exc.
      ~job() { if (fd >= 0) { ::close(fd); } }
      int periodic() // ret. 1 if done something, 0 if done nothing
      {
        critsec_t<job> __lock(5, 0, &csd); if (!__lock.is_locked()) { return 0; }
        if (state == 1)
        {
            // Sequence of role numbers to connect (scan in cycle): -1, 0 ... n_role_max.
          if (s.b_srv() || !s.b_connected()) { s.~_asocket_io(); if (b_conn1) { b_conn1 = false; } else { ++nrsrch; if (nrsrch > n_role_max) { nrsrch = -1; } } new (&s) _asocket_io(false, name, nrsrch, "F\t"); }

          s.try_receive(m1.m(), true);
          if (s.res() >= 1) { fd = m1.fd(); s.close_h1h2(); state = 2; return 1; }
          if (s.res() == 0) { }
            else if (s.res() == -2) { s.close_h2(); }
            else if (s.res() == -3) { s.close_h1h2(); }
          return 0;
        }
        else if (state == 2)
        {
          if (fd <= -2) { s.close_h1h2(); state = 0; return 0; }
          if (!b_mayexpose) { s.close_h1h2(); return 0; }
          if (!s.b_srv()) { s.~_asocket_io(); if (b_conn1) { b_conn1 = false; } new (&s) _asocket_io(true, name, nrexp, "F\t"); }
          if (!s.b_connected()) { m1.fd() = fd; }

          s.try_send(m1.m(), true);
          if (s.res() >= 1) { s.close_h2(); return 1; }
          if (s.res() == 0) { }
            else if (s.res() == -2) { s.close_h2(); }
            else if (s.res() == -3) { s.close_h1h2(); }
          return 0;
        }
        return 0;
      }
    };
    typedef std::map<t_name, cref_t<job> > t_map1;
    inline static cref_t<t_map1> rmap1() { static bool b_des = false; if (b_des) { return cref_t<t_map1>(); } static cref_t<t_map1> x; static struct _local_des { ~_local_des() { b_des = true; } } __des; if (!x) { x.cm_create0(0, 0, 1); } return x; }
    inline static cref_t<threadctl> rth() { static bool b_des = false; if (b_des) { return cref_t<threadctl>(); } static cref_t<threadctl> x; static struct _local_des { ~_local_des() { b_des = true; _fd_sharing().reset(true); } } __des; if (!x) { x.cm_create0(0, 0, 1); } if (x && !x.ref()) { x._pnonc_u()->start_auto<_th>(int(0)); } return x; }
    static int _fd_search(_fd_sharing& par, const t_name& name_orig, s_long t_retry_mcs, s_long n_role_expose)
    {
      rth();
      bool b_insert = t_retry_mcs >= 0;
      bool b_inserted = false;
      t_name k = _asocket_io::key(name_orig, -1, "F\t");
      cref_t<t_map1> rm = rmap1(); if (!rm) { return -2; }
      cref_t<job> rj;
      if (1)
      {
        cref_t<t_map1>::t_lock __lock(rm); if (sizeof(__lock)) {}
        t_map1& m = *rm._pnonc_u();
        try {
          if (m.count(k) == 1) { rj = m[k]; }
            else if (b_insert) { rj.cm_create2(0, 0, 0, name_orig, n_role_expose); m[k] = rj; b_inserted = true; }
        } catch (...) { return -2; }
      }
      if (!rj) { return -3; }
      job& j = *rj._pnonc_u();
      double t0 = clock_ms();
      if (1) // query whole range of roles once before exit
      {
        critsec_t<job> __lock(5, -1, &j.csd); if (sizeof(__lock)) {}
        if (j.state == 0) { j.state = 1; }
        s_long nrprev;
        while (true)
        {
          nrprev = j.nrsrch;
          j.periodic();
          if (j.state == 2) { int fd = j.fd; if (fd < 0) { return -1; } int fd2 = ::dup(fd); if (fd2 < 0) { return -2; } return fd2; }
          if (j.nrsrch != nrprev && nrprev == n_role_max) { if (t_retry_mcs > 0) { break; } return -4; }
//if (clock_ms() - t0 > 200) { cout << "Unexp. DELAY!!! " << (j.s.name().c_str() + 1) << endl; }
          if (clock_ms() - t0 > 200) { return -5; } // should not occur, because connections scanning is very fast
          if (j.nrsrch == nrprev) { sleep_mcs(10); }
        };
      }
      while (true)
      {
        if (j.state == 2) { int fd = j.fd; if (j.state != 2) { return -2; } if (fd < 0) { return -1; } int fd2 = ::dup(fd); if (fd2 < 0) { return -2; } return fd2; }
        if (clock_ms() - t0 > t_retry_mcs / 1000.) { break; }
        sleep_mcs(10000);
      }
      return -4;
    }
    static int _fd_expose(_fd_sharing& par, const t_name& name_orig, int fd, s_long n_role, bool b_overwrite, bool b_expose)
    {
      rth();
      if (fd <= -3) { return -1; }
      if (n_role < 0) { n_role = -1; } else if (n_role > n_role_max) { n_role = n_role_max; }
      t_name k = _asocket_io::key(name_orig, -1, "F\t");
      cref_t<t_map1> rm = rmap1(); if (!rm) { return -2; }
      cref_t<job> rj;
      if (1)
      {
        cref_t<t_map1>::t_lock __lock(rm); if (sizeof(__lock)) {}
        t_map1& m = *rm._pnonc_u();
        try {
          if (m.count(k) == 1) { rj = m[k]; }
            else if (fd >= -1) { rj.cm_create2(0, 0, 0, name_orig, n_role); m[k] = rj; }
            else { return -3; }
        } catch (...) { return -2; }
      }
      if (!rj) { return -2; }
      job& j = *rj._pnonc_u();
      critsec_t<job> __lock(5, -1, &j.csd); if (sizeof(__lock)) {}
      if (fd >= -1)
      {
        if (j.fd >= -1 && !b_overwrite) { return 0; }
        int fd2 = fd >= 0 ? ::dup(fd) : -1; if (fd >= 0 && fd2 == -1) { return -2; }
        if (j.fd >= 0) { ::close(j.fd); }
        j.fd = fd2;

        if (j.state != 2 || n_role != j.nrexp) { j.s.close_h1h2();  j.state = 2; }
        j.nrexp = n_role;
      }
      j.b_mayexpose = b_expose;
      return 1;
    }
    static int _fd_remove(_fd_sharing& par, const t_name& name_orig)
    {
      rth();
      t_name k = _asocket_io::key(name_orig, -1, "F\t");
      cref_t<t_map1> rm = rmap1(); if (!rm) { return -2; }
      cref_t<t_map1>::t_lock __lock(rm); if (sizeof(__lock)) {}
      t_map1& m = *rm._pnonc_u();
      try { if (m.count(k) == 1) { m.erase(k); return 1; } } catch (...) { return -2; }
      return 0;
    }
    static void _reset(_fd_sharing& par, bool b_wait)
    {
      cref_t<t_map1> rm = rmap1();
      if (rm) { cref_t<t_map1>::t_lock __lock(rm); if (sizeof(__lock)) {} rm._pnonc_u()->clear(); rm.clear(); }
      cref_t<threadctl> rt = rth();
      if (rt) { rt._pnonc_u()->signal_stop(); if (b_wait && rt.ref()) { sleep_mcs(0); while (rt.ref()) { sleep_mcs(10000); } } rt.clear(); }
    }
    struct _th : threadctl::ctx_base
    {
      void _thread_proc()
      {
        while (!b_stop())
        {
          int nj = 0;
          do { try { // once
            cpparray_t<cref_t<job> > jj;
            if (1)
            {
              cref_t<t_map1> rm = rmap1();
              cref_t<t_map1>::t_lock __lock(false, rm.pcsd());
              if (!__lock) { break; }
              const t_map1& m = rm.ref();
              if (!jj.resize(s_ll(m.size()))) { break; }
              s_ll i = 0;
              for (t_map1::const_iterator q = m.begin(); q != m.end(); ++q, ++i) { jj[i] = q->second; }
            }
            for (s_ll i = 0; i < jj.n(); ++i) { if (jj[i]) { nj += jj[i]._pnonc_u()->periodic(); } }
          } catch (...) {} } while (false);
          if (nj == 0) { sleep_mcs(10000); }
        }
      }
    };
  };

#endif



  // Implementation part, exposed into main namespace (bmdx_shm).
namespace _api
{

    // Non-blocking, non-locking, by-reference queue for bytes, for two threads (sender, receiver).
    // Designed to work as shared object in IPC.
    // NOTE Size of rfifo_nbl11 as such is fixed: 32 bytes, this includes 12 bytes for data.
    //    When used by reference (on a memory block), the number of bytes for data depends on that block size.
    //    See init() for details.
  struct rfifo_nbl11
  {
    typedef bmdx_meta::s_ll s_ll; // 64-bit signed integer
    static inline s_ll myllmin(s_ll a, s_ll b) { return a < b ? a : b; }

      // NOTE Buffer object, created with constructor, will have size == n0 bytes.
      //  Buffer object, referenced by pointer cast, will have size >= n0 bytes.
    enum { n0 = 8 };

      // NOTE Copying and assignment do not copy anything from source object.
      //   This is not the main intended way of using rbufipc. See init().
    rfifo_nbl11() : _ipush(0), _ipop(0), _n(n0) {}
    rfifo_nbl11(const rfifo_nbl11&) : _ipush(0), _ipop(0), _n(n0) {}
    void operator=(const rfifo_nbl11&) {}



    // Initializer side.

      // rbufipc should be used as reference (cast from pre-allocated memory area).
      // When the client has allocated a memory of size N at location P to use as rbufipc,
      // it must call ((rbufipc*)P)->init(N-20, ...) to construct the buffer.
      // Byte data offset is P+20 (in case when priming is needed).
    void init_ref(s_ll n_, s_ll ipush_ = 0, s_ll ipop_ = 0) { if (sizeof(size_t) < 8) { n_ = myllmin(n_, 1ll << 31); } _n = n_ < 1 ? 1 : n_; _ipush = ipush_; _ipop = ipop_; }

      // Data buffer size.
    s_ll n() const { return _n; }



    // Supplier side.

      // The current push position, and the number of bytes (volatile) that may be pushed now.
    s_ll ipush() const { return (s_ll)_ipush; }
    s_ll npush() const { return _n - npop(); }

      // If nmax > 0, pushes min(nmax, npush()) bytes from p into this ipcbuf.
      // Returns the number of bytes actually pushed (>=0).
      // NOTE if p != 0, and the buffer is valid (not damaged), push() guarantees the correct pushing for at least npush() bytes.
      // NOTE pushing is transactional (ipush() is increased only once, at the end of successful operation).
    s_ll push(const char* p, s_ll nmax)
    {
      if (!p) { return 0; }
      if (nmax <= 0) { return 0; }
      s_ll i2 = (s_ll)_ipush, i1 = (s_ll)_ipop;
      s_ll n = _n - (i2 - i1);
      s_ll __np = nmax >= n ? n : s_ll(nmax);
      s_ll n1 = myllmin(__np, myllmin(n, _n - s_ll(i2 % _n)));
      if (n1 > 0)
      {
        std::memcpy(&d[0] + _ipush % _n, p, size_t(n1));
        p += n1;
        s_ll n2 = __np - n1;
        if (n2 > 0) { std::memcpy(&d[0] + (_ipush + n1) % _n, p, size_t(n2)); }
        _ipush += __np; // the last action "commits" pushing (important if the object is in shared memory)
       }
      return __np;
    }

      // Same as push(), only pushes byte b up to nmax times.
      // NOTE if the buffer is valid (not damaged), push() guarantees the correct pushing for at least npush() bytes.
      // NOTE pushing is transactional (ipush() is increased only once, at the end of successful operation).
    s_ll push_bytes(char b, s_ll nmax)
    {
      if (nmax <= 0) { return 0; }
      s_ll i2 = (s_ll)_ipush, i1 = (s_ll)_ipop;
      s_ll n = _n - (i2 - i1);
      s_ll __np = nmax >= n ? n : s_ll(nmax);
      s_ll n1 = myllmin(__np, myllmin(n, _n - i2 % _n));
      if (n1 > 0)
      {
        std::memset(&d[0] + _ipush % _n, b, size_t(n1));
        s_ll n2 = __np - n1;
        if (n2 > 0) { std::memset(&d[0] + (_ipush + n1) % _n, b, size_t(n2)); }
        _ipush += __np; // the last action "commits" pushing (important if the object is in shared memory)
      }
      return __np;
    }


    // Receiver side.

      // The current pop position, and the number of bytes (volatile) that may be popped now.
    s_ll ipop() const { return (s_ll)_ipop; }
    s_ll npop() const { return _ipush - _ipop; }

      // How many bytes, which may be popped, are located contiguously. [0..npop()].
    s_ll npop_contig() const { s_ll i2 = (s_ll)_ipush, i1 = (s_ll)_ipop; return myllmin(i2 - i1, _n - i1 % _n); }

      // Pointer to the next byte that would be popped.
    const char* peek1() const { return &d[0] + _ipop % _n; }

      // If nmax > 0, pops min(nmax, npop()) bytes from ipcbuf into pdest.
      // If b_do_pop == false, ipop() is not increased, so the client just gets a copy of buffer data.
      // Returns the number of bytes actually popped (>=0).
      // NOTE popping is transactional (ipop() is increased only once, at the end of successful operation).
    s_ll pop(char* pdest, s_ll nmax, bool b_do_pop = true)
    {
      if (!pdest) { return 0; }
      if (nmax <= 0) { return 0; }
      s_ll i1 = (s_ll)_ipop, i2 = (s_ll)_ipush;
      s_ll n = i2 - i1;
      s_ll __np = nmax >= n ? n : s_ll(nmax);
      s_ll n1 = myllmin(__np, myllmin(n, _n - i1 % _n));
      if (n1 > 0)
      {
        std::memcpy(pdest, &d[0] + _ipop % _n, size_t(n1));
        pdest += n1;
        s_ll n2 = __np - n1;
        if (n2 > 0) { std::memcpy(pdest, &d[0] + (_ipop + n1) % _n, size_t(n2)); }
        if (b_do_pop) { _ipop += __np; } // the last action "commits" popping (important if the object is in shared memory)
      }
      return __np;
    }

      // If nmax > 0, skip min(nmax, npop()) bytes.
      // Returns the number of bytes actually skipped (>=0).
      // NOTE discarding is transactional (ipop() is increased only once, at the end of successful operation).
    s_ll discard(s_ll nmax)
    {
      if (nmax <= 0) { return 0; }
      s_ll i2 = (s_ll)_ipush, i1 = (s_ll)_ipop;
      s_ll n = i2 - i1;
      s_ll __np = nmax >= n ? n : nmax;
      _ipop += __np; // the last action "commits" discarding (important if the object is in shared memory)
      return __np;
    }

  private:
    volatile s_ll _ipush;
    volatile s_ll _ipop;
    s_ll _n;
    char d[n0]; // first bytes of the buffer
  };





    // Global (inter-process) named lock (mutex) control.
    //    The lock works between processes, threads, and inside a thread.
    //    Only one critsec_gn object with particular name may have its lock set at any moment.
    //    At the same time, locking any other objects with same name will fail.
    //    All operations are non-blocking.
    // NOTE critsec_gn object is not associated with the thread that created it or successfully locked it.
    //  It can be shared between threads, e.g. with smart pointer,
    //    and deleted by thread other than one that had set successful lock.
    //  The current limitations:
    //    1. critsec_gn itself is not protected from concurrent access.
    //      This is responsibility of the client.
    //    2. critsec_gn lock() and unlock() are not counted.
    //      This should be done by the client if it needs.
    //      lock() tries to lock is the object is not locked, and does nothing if it's already locked.
    //      unlock() does unlocking if the object is locked.
    //    3. If a thread contains or references locked critsec_gn,
    //      and that thread has been terminated such that the object is not deleted,
    //      the lock is not released until the process exits or terminates.
    //      (After process ending, the lock may be set by any thread in any process, as normal.)
  struct critsec_gn
  {
    typedef t_name_shm t_name;

    critsec_gn(const char* name, f_shm_name_prefix prefix_cs = &f_prefix_critsec_gn __bmdx_noarg) { __pad1 = 0; _b_locked = false;  _setname(name, prefix_cs ? prefix_cs() : f_prefix_critsec_gn()); _init0(); } // critsec_gn(name): copy object name, do nothing else.
    const t_name& name() const { return _name; } // factual object name (n() > 0, includes system-dependent prefix and maybe few other characters, length limited to allowed by system, may contain null characters)
    bool b_opened(__bmdx_noarg1) const { return _b_handle(); } // true: the lockable object is opened or created (may still be unnamed)
    bool b_locked(__bmdx_noarg1) const { return _b_locked; } // true: the object is named and locked; false: anything else
    bool cr_op(__bmdx_noarg1) { _init1(); return b_opened(); } // try to create or open existing named object and associated objects if any, but do not lock yet
    bool lock(__bmdx_noarg1) { _lock(); return _b_locked; } // try lock (if not opened, tries to create or open automatically)
    void unlock(__bmdx_noarg1) { _unlock(); } // unlock, but do not close the named object
    void close(__bmdx_noarg1) { _reset(); } // unlock and close the named object
    ~critsec_gn() throw(__bmdx_noargt1) { _reset(); } // unlock and close the named object

  private:
    critsec_gn(const critsec_gn&); void operator=(const critsec_gn&);
    t_name _name;
    union { bool _b_locked; s_long __pad1; };

    #ifdef _bmdxpl_Wnds
      HANDLE hs;
      bool _b_handle(__bmdx_noarg1) const { return !!hs; } // true: named object created or opened
      void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '\\') { c = '_'; } _name += c; } }
      void _init0(__bmdx_noarg1) { hs = 0; } // platf.-dependent constructor
      void _init1(__bmdx_noarg1) { if (!hs) { hs = CreateSemaphoreA(0, 1, 1, _name.c_str()); } if (!hs) { return; } }
      void _lock(__bmdx_noarg1) { _init1(); if (_b_locked || !_b_handle()) { return; } DWORD res_s = WaitForSingleObjectEx(hs, 0, FALSE); _b_locked = res_s == WAIT_OBJECT_0; }
      void _unlock(__bmdx_noarg1) { if (_b_locked) { BOOL b = ReleaseSemaphore(hs, 1, 0); (void)b; _b_locked = false; } }
      void _reset(__bmdx_noarg1) { if (hs) { _unlock(); CloseHandle(hs); hs = 0; } }
    #endif
    #ifdef _bmdxpl_Psx
    #if 0
    #elif defined(__SUNPRO_CC) || defined (__sun)
      int h; int _perm; mutex_t* pm; s_long uid; s_long uid_fsh;
      bool _b_handle(__bmdx_noarg1) const { return !!pm; } // true: named object created or opened
      void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '/') { c = '_'; } _name += c; } }
      void _init0(__bmdx_noarg1) { h = 0; pm = 0; _perm = 0666; uid = 0; } // platf.-dependent constructor
      void _init1(__bmdx_noarg1)
      {
        const size_t lkfsize = sizeof(mutex_t) + 3 * 4;
        bool b_creating = false;
        if (pm) { return; }
        int h2 = ::open(_name.c_str(), O_CREAT | O_EXCL | O_RDWR , _perm);
        if (h2 != -1) { b_creating = true; if (0 != ftruncate(h2, lkfsize)) { ::unlink(_name.c_str()); ::close(h2); return; } }
        else
        {
          h2 = ::open(_name.c_str(), O_RDWR , _perm);
          if (h2 == -1) { return; }
          struct stat s; std::memset(&s, 0, sizeof(s));
          if (0 != fstat(h2, &s)) { ::close(h2); return; }
          b_creating = size_t(s.st_size) < lkfsize;
          if (b_creating) { if (0 != ftruncate(h2, lkfsize)) { ::unlink(_name.c_str()); ::close(h2); return; } }
        }

        mutex_t* pm2 = (mutex_t*)mmap(NULL, lkfsize, PROT_READ|PROT_WRITE, MAP_SHARED, h2, 0);
        if (!pm2) { ::close(h2); return; }

        s_long* puidgen = (s_long*)(pm2+1); s_long* pflag = puidgen + 1;
        s_long pid1 = pflag[1];
        if (b_creating  || (pid1 != 0 && kill(pid1, 0) != 0))
        {
          pflag[0] = 0;
          pflag[1] = 0;
          mutex_init(pm2, USYNC_PROCESS|LOCK_ROBUST|LOCK_ERRORCHECK|LOCK_PRIO_INHERIT, 0);
        }
        int res = mutex_lock(pm2); if (res == EOWNERDEAD) { mutex_consistent(pm2); pflag[0] = 0; pflag[1] = 0; mutex_unlock(pm2); res = mutex_lock(pm2); }
          if (res != 0) { ::close(h2); return; }

        h = h2; pm = pm2;
        uid = ++*puidgen;

        mutex_unlock(pm2);
      }
      void _lock(__bmdx_noarg1)
      {
        _init1(); if (_b_locked || !_b_handle()) { return; }
        int h2 = h; mutex_t* pm2 = pm; s_long* puidgen = (s_long*)(pm2+1); s_long* pflag = puidgen + 1;

        int res = mutex_lock(pm2); if (res == EOWNERDEAD) { mutex_consistent(pm2); pflag[0] = 0; mutex_unlock(pm2); res = mutex_lock(pm2); }
        if (res != 0) { _b_locked = false; return; }
        if (pflag[0] != 0) { mutex_unlock(pm2); _b_locked = false; return; }
        pflag[1] = getpid();
        pflag[0] = uid;
        mutex_unlock(pm2);
        _b_locked = true; return;
      }
      void _unlock2(__bmdx_noarg)
      {
        if (!_b_locked) { return; }
        if (!_b_handle()) { _b_locked = false; return; }
        int h2 = h; mutex_t* pm2 = pm; s_long* puidgen = (s_long*)(pm2+1); s_long* pflag = puidgen + 1;

        int res = mutex_lock(pm2); if (res == EOWNERDEAD) { mutex_consistent(pm2); pflag[0] = 0; pflag[1] = 0; mutex_unlock(pm2); _b_locked = false; return; }
        if (pflag[0] == uid) { pflag[0] = 0; pflag[1] = 0; }
        if (res == 0) { mutex_unlock(pm2); }
        _b_locked = false;
        return;
      }
      void _unlock(__bmdx_noarg1) { _unlock2(); }
      void _reset(__bmdx_noarg1) { _unlock2(); if (pm) { munmap(pm, sizeof(mutex_t)); pm = 0; } if (h) { ::close(h); h = 0; } uid = 0; }
    #elif defined(__ANDROID__)
      int h; bool _bsock;
      bool _b_handle(__bmdx_noarg1) const { return _bsock; } // true: named object created or opened
      void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '/') { c = '_'; } _name += c; } s_long nmax = sizeof(sockaddr_un) - sizeof(sa_family_t); if (_name.n() > nmax - 1) { _name.resize(nmax - 1); } }
      void _init0(__bmdx_noarg1) { h = -1; _bsock = false; } // platf.-dependent constructor
      void _init1(__bmdx_noarg1) { if (h != -1) { return; } int h2 = ::socket(PF_LOCAL, SOCK_STREAM, 0); if (h2 != -1) { h = h2; _bsock = true; } } // named object creation
      void _lock(__bmdx_noarg1) { _init1(); if (_b_locked || !_b_handle()) { return; } sockaddr_un a; a.sun_family = PF_LOCAL; memcpy(&a.sun_path, _name.c_str(), size_t(_name.n())); _b_locked = ::bind(h, (sockaddr*)&a, sizeof(sa_family_t) + size_t(_name.n())) != -1; } // check b_locked() after return
      void _unlock(__bmdx_noarg1) { if (_b_locked) { ::close(h); h = -1; _b_locked = false; } }
      void _reset(__bmdx_noarg1) { if (h != -1) { ::close(h); h = -1; _b_locked = false; _bsock = false; } } // unlock mutex, release (close) the named object
    #else
      int h; int _perm;
      bool _b_handle(__bmdx_noarg1) const { return !!h; } // true: named object created or opened
      void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '/') { c = '_'; } _name += c; } }
      void _init0(__bmdx_noarg1) { h = 0; _perm = 0666; } // platf.-dependent constructor
      void _init1(__bmdx_noarg1) { if (h) { return; } int h2 = ::open(_name.c_str(), O_CREAT, _perm); if (h2 != -1) { h = h2; } } // named object creation
      void _lock(__bmdx_noarg1) { _init1(); if (_b_locked || !_b_handle()) { return; } _b_locked = flock(h, LOCK_NB | LOCK_EX) == 0; } // check b_locked() after return
      void _unlock(__bmdx_noarg1) { if (_b_locked) { flock(h, LOCK_NB | LOCK_UN); _b_locked = false; } }
      void _reset(__bmdx_noarg1) { if (h) { _unlock(); ::close(h); h = 0; } } // unlock mutex, release (close) the named object
    #endif
    #endif
  };




    // Wrapper for an object in shared memory, with two-sided access (2 separate locks).
  template<class T>
  struct shmobj2s_t
  {
    private: struct _shm_sems;
    public:
    typedef t_name_shm t_name;
    struct exc_shmobj2s : std::exception { exc_shmobj2s(__bmdx_noarg1) {} const char* what() const throw() { return "exc_shmobj2s::operator->: p==0"; } };


      // The pointer to nb bytes of shared memory, if shared memory is opened and compatible,
      //   or created and successfully initialized.
      // Otherwise 0.
    T* p() const { return _p; }
    T* operator->() const throw (exc_shmobj2s __bmdx_noargt)        { if (!_p) { throw exc_shmobj2s(); } return _p; }


      // (Constructor-defined parameter.)
      // Role (assigned on shmobj2s_t construction).
    bool b_side1(__bmdx_noarg1) const        { return _b_side1; }

      // Returns:
      //    >= 0 - the amount of memory for placement of T, allocated on shared memory area creation.
      //    0 - a) the shared memory cannot be accessed now, or b) not initialized yet.
      // NOTE Positive nb() >= nb arg., passed to shmobj2s_t constructor.
    s_ll nb(__bmdx_noarg1) const        { return _pdesc() ? _pdesc()->nb : 0; }

      // Factual system name of the shared object.
      //  May differ from name, passed to shmobj2s_t constructor.
      //  Also, system may ignore part of the name. ~!!! check MAX_PATH
      //  The pointer is always != 0.
    const char* name() const       { return _name.c_str(); }


      // Determines if this shmobj2s_t holds the specified lock on the referenced object.
      //    (This does not include any locks created on the client side.)
      // Returns:
      // a) True if the object lock is set:
      //    a)) side == 1 - the lock of side 1 is checked.
      //    b)) side == 2 - the lock of side 2 is checked.
      //    c)) side == 4 on b_side1() == true - the lock of side 1 is checked.
      //    d)) side == 4 on b_side1() == false - the lock of side 2 is checked.
      // b) False is returned if the lock is not valid or side arg. wrong.
    bool b_locked(s_long side = 4 __bmdx_noarg) const        { if (side == 4) { side = b_side1() ? 1 : 2; } if (side == 1) { return _lkmyside.b_lk1(); } if (side == 2) { return _lkmyside.b_lk2(); } return false; }

      // The contained object state:
      //  0 - not constructed yet, 1 - constructed (valid), 2 - destroyed. p() != 0.
      //  -1 - unknown (failure). p() == 0.
    s_long f_constructed(__bmdx_noarg1) const        { if (!_pdesc()) { return -1; } if (_pdesc()->b_destroyed) { return 2; } if (_pdesc()->b_constructed) { return 1; } return 0; }



      // (Informational) Shared memory overhead to keep shmobj2s_t's _shmdesc structure.
    static s_long nbover(__bmdx_noarg1)        { return sizeof(_shmdesc); }



      // Hints on shared memory area creation and handling.
    struct hints
    {
      typedef t_name_shm t_name;

        // htype (!=0): unique identifier for type of T to be recognized in any other process.
        //        4-byte signed integer, stored in the beginning of allocated shared memory area.
        //        Default is sizeof(T).
        //        Customizing: supply a negative value, uniquely specifying custom category of the referenced object,
        //          known to all processes that are using it.
      s_long htype;

        // prefix_cs(): returns system and application-dependent semaphore name prefix.
        //        Default: == f_prefix_critsec_gn.
        //        Customizing: may be set to another value if particular system settings are different from its standard.
      t_name (*prefix_cs)();

        // prefix_m(): returns system and application-dependent shared memory name prefix.
        //        Default: == f_prefix_shmobj2s.
        //        Customizing: may be set to another value if particular system settings are different from its standard.
      t_name (*prefix_m)();


      hints() : htype(s_long(sizeof(T))), prefix_cs(&f_prefix_critsec_gn), prefix_m(&f_prefix_shmobj2s) {}
      hints(s_long htype_) : htype(htype_), prefix_cs(&f_prefix_critsec_gn), prefix_m(&f_prefix_shmobj2s) {}
    };




      // Remembers the name, role, memory size, parameters. Does nothing else.
      //    name: unique object name. (0 or "" is acceptable, but not recommended.)
      //      NOTE To create global system object, the name is pre-pended by platform-dependent string,
      //        which can be customized (see "prefix" arg.) by the client.
      //    Role (b_side1):
      //      a) true: on prepare(), side 1 will automatically create/initialize shared memory area.
      //      b) false: on prepare(), side 2 only tries to open the memory and check if it's already initialized with compatible
      //          shmobj2s_t from this or other project.
      //    nb: number of bytes to allocate for T.
      //        May be unrelated to sizeof(T), but prepare() will fail f the shared memory is already initialized
      //        NOTE Total number of allocated bytes for an object is max(0, nb) + nbover().
      // Work sequence example, from the side that creates the object:
      //    shmobj2s_t s(...);
      //    prepare();
      //    check p() != 0;
      //    (optional) if b_bad(): do_recover();
      //    (optional) if f_constructed() != 1: obj_create*(), check f_constructed() == 1;
      //    use p or operator->
      // Work sequence example, from the side that only uses the object:
      //    shmobj2s_t sm(...);
      //    while (prepare() <= 0): sleep
      //    (optional) while !(f_constructed() == 1 || b_bad()): sleep
      //    (optional) check !b_bad();
      //    use p or operator->
    shmobj2s_t(const char* name, bool b_side1, s_ll nb = sizeof(T), const hints& hints_ = hints() __bmdx_noarg) throw()        { _p = 0; _b_side1 = b_side1; _hh = hints_; _hh.htype = hints_.htype != 0 ? hints_.htype : -1; _name = hints_.prefix_m ? hints_.prefix_m() : f_prefix_shmobj2s(); _name += name; _local_nb = nb > 0 ? nb : 0; _rsems.cm_create2(0, 0, 1, _name.c_str(), hints_.prefix_cs); _shm_init0(); }


      // Open/create/initialize/lock the shared memory in role as specified by b_side1 on shmobj2s_t construction.
      //  b_keeplk true:
      //      a) for the side that may initialize the object: try to set that side's lock, allocate shared memory if not yet, initialize state variables if not yet, return results.
      //      b) for the side that may NOT initialize the object: try to set that side's lock, try to get shared memory pointer, check initialization, return results.
      //  b_keeplk false:
      //      a) for the side that may initialize the object: try to set that side's lock, allocate shared memory if not yet, initialize state variables if not yet, release the lock, return results.
      //      b) for the side that may NOT initialize the object: try to get shared memory pointer, check initialization, return results.
      //  sides_cr: OR-ed flags, showing which sides may create and initialize shared memory area if it does not exist.
      //      0x1 - side 1 (by default).
      //      0x2 - side 2.
      //      If the current side (b_side1 ? 1 : 2) matches with sides_cr, prepare() creates its lock
      //        before any further operation.
      //      NOTE prepare() never creates a lock of the opposite side, so if (by design) both sides may initialize the object
      //        (sides_cr == 3), the client must additionally synchronize such access.
      // Returns:
      //    1 - the memory is available (created and initialized, or already opened). b_opened() == true.
      //    -1 - the shared memory exists, but its structure is not compatible (see htype arg. in shmobj2s_t()).
      //        The memory handle (in the current shmobj2s_t) has been closed.
      //    -2 - failure during operation (it's denerally unknown if the shared object is available).
      //    -3 - lock of side 1 (for b_side1()==true) or side 2 (for b_side1()==false) is set by someone else.
      //        The memory has been closed (in the current process).
      //    -4 - the memory exists, but not accessible yet: a) pending initialization by other side, b) pending memory handle transfer from other side.
      // PLATFORM NOTES
      //    1. On all platforms, shared memory operation may depend on system settings and security level.
      //      The default shmobj2s_t behavior is tuned to standard settings in all systems,
      //      allowing for all user processes to freely share memory and communicate.
      //    2. Particular: on Android, sharing memory descriptors requires one of
      //      a) the communicating processes have the same user (though not needed to be in parent-child relations),
      //      b) the communicating processes have root privileges,
      //      c) permissive SELinux policy mode is set in the system. E.g. "su 0 setenforce 0", which implies root access as well.
      //      If all conditions are false, shmobj2s_t will work only between different threads of its own process.
    s_long prepare(bool b_keeplk, s_long sides_cr = 1 __bmdx_noarg)        { return _prep(b_keeplk, sides_cr); }



      // Construct T at this->p if it can be done (!b_bad() && f_constructed() == 0 or 2).
      //    This may require a) synchronization, and/or b) agreement of what side creates the object.
      //    (This is responsibility of the client, depending on T and client implementation.)
      // Returns:
      //    true: shared memory is open and this->p now points to valid object (constructed or already existed).
      //    false: otherwise.
      //    exception: only if no_exc == false, and T() generated an exception.
    bool obj_create0(bool no_exc) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1> bool obj_create1(bool no_exc, const Arg1& x1) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1, class Arg2> bool obj_create2(bool no_exc, const Arg1& x1, const Arg2& x2) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1, class Arg2, class Arg3> bool obj_create3(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4> bool obj_create4(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5> bool obj_create5(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6> bool obj_create6(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4, const Arg5& x5, const Arg6& x6) { s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5, x6); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }

      // Explicitly call the destructor, if it can be done (!b_bad() && f_constructed() == 1).
      // If T is accessible and currently constructed, obj_destroy calls ~T(), and after, marks the object it destroyed.
      //    This may require a) synchronization, and/or b) agreement of what side destroys the object.
      //    (This is responsibility of the client, depending on T and client implementation.)
      // The object state may be checked after with f_constructed().
      // NOTE 1) Calling ~T most probably is not needed, because the object T disappears once shared memory is released,
      //      and, by design, does not keep any resource handles.
      //      (Such keeping would lead to inconsistent state in case of failure of the process responsible for destructor calling.)
      //    2) From other side, creating/destroyng an object may be used as simple 1-element queue between processes,
      //      because of clear tracking via f_constructed().
    void obj_destroy(__bmdx_noarg1) { if (!(!b_bad() && f_constructed() == 1)) { return; } try { _p->~T(); } catch (...) {} set_f_constructed(2); }

      // Sets f_constructed to new value (0, 1, 2) if possible. Check f_constructed() after that.
      //  NOTE set_f_constructed is not synchronized (this is responsibility of the client, depending on its implementation).
    void set_f_constructed(s_long state) const        { if (!_pdesc()) { return; } if (state == 0) { _pdesc()->b_constructed = 0; _pdesc()->b_destroyed = 0; } if (state == 1) { _pdesc()->b_constructed = 1; _pdesc()->b_destroyed = 0; } if (state == 2) { _pdesc()->b_destroyed = 1; _pdesc()->b_constructed = 1; } }



      // Unmap shared memory from this process, close shared memory handle,
      //  release locks held by this shmobj2s_t.
      // Side effects:
      //  1. If any local lock objects exist, and they continue holding their lock, when if the memory is reopened
      //    by the same shmobj2s_t, the process continues to hold the lock, instead of trying to re-lock.
      //    NOTE In contrary, ~shmobj2s_t does unlock unconditionally,
      //      and any other lock objects do not protect anything since that time.
      //      (The new shmobj2s_t would set the new instance of lock.)
      //  2. close() does NOT execute ~T(), so if the referenced object exists, it continues to exist.
      //  3. The shared memory is closed and removed by the system
      //    exactly when the last handle in any process is released,
      //    unrelated to T constructor or destructor calls.
    void close(__bmdx_noarg1) { _close(false); }

      // shmobj2s_t destructor automatically closes shared memory,
      //    and releases locks held by this shmobj2s_t.
      //    See also close().
    ~shmobj2s_t() throw(__bmdx_noargt1) { _close(true); }


    struct shared_lock
    {
        // true if side 1 lock is set.
      bool b_lk1() const throw()        { return vlk1 >= 0; }
        // true if side 2 lock is set.
      bool b_lk2() const throw()        { return vlk2 >= 0; }
        // true if any of he locks is set.
      bool b_lk_any() const throw()        { return b_lk1() || b_lk2(); }

        // Create an empty object (no any locks).
      shared_lock(__bmdx_noarg1) : vlk1(-1), vlk2(-1) {}

        // Try to do global locking of shared memory, from the specified side (or sides).
        //    b_lk_any() reports if locking succeeded.
        //    sides: OR-ed flags:
        //      (sides & 1) != 0: lock side 1.
        //      (sides & 2) != 0: lock side 2.
        //      (sides & 4) != 0: lock (parent.b_side1() ? side 1 : side 2).
        //    If both sides must be locked (e.g. OR-ed sides == 3), but only one lock is acquired, it's released at once.
        //  timeout_ms (milliseconds):
        //    a) == 0: the constructor exits immediately.
        //    b) > 0: the constructor will sleep/retry locking until the required locks are set, or timeout occurs.
        //    c) < 0: the constructor blocks until the required locks are set.
        //      (c) is not recommended, because locks in other processes may live indefinitely long.
        // NOTE shared_lock is associated with its parent shmobj2s_t only, so that
        //    1) It is recursive (multiple successfully locked instances from the same parent may co-exist).
        //    2) It can be held by more than one thread at once.
        //      If this is not acceptable, it must be additionally protected with conventional multi-thread critical section.
        // NOTE shared_lock is useful only if prepare() was called with b_keeplk == false,
        //    or if the client needs to additionally set a lock for side other than (b_side1() ? 1 : 2) of the parent shmobj2s_t.
      shared_lock(shmobj2s_t& parent, s_long sides = 4, s_long timeout_ms = 0 __bmdx_noarg) throw();

      ~shared_lock() throw(__bmdx_noargt1)        { clear(); }

        // After clear(), the client may safely execute new (p) shared_lock(...).
      void clear(__bmdx_noarg1) throw()        { if (r) { r._pnonc_u()->unlock_normal(*this); } vlk1 = -1; vlk2 = -1; r.clear(); }

        // May be used if a new object is created with another set of locks than some other existing one.
      void swap(shared_lock& x __bmdx_noarg) throw()        { bmdx_str::words::swap_bytes(*this, x); }

    private: friend struct _shm_sems; cref_t<_shm_sems> r; s_ll vlk1, vlk2; shared_lock(const shared_lock&); void operator=(const shared_lock&);
    };


      // This flag indicates that both sides are recognized that the object is damaged.
      //  The flag resides in shared memory.
      // Recovery sequence (optional):
      // When b_bad() == true (means "both sides recognized data damage by setting f_state1 and f_state2 to -1"):
      //    1) Statically assigned side (e.g. side 1) calls do_recover().
      //        1)) do_recover sets all T bytes to 0,
      //        2)) initializes _shmdesc area according to the current shmobj2s_t state,
      //        3)) at last, sets (b_constructed, b_destroyed, f_state1, f_state2) = 0.
      //    2) The same side creates an object T as agreed, using obj_create* or other method.
      //    3) Another side simply waits until (!b_bad() && f_constructed() == 1).
      // NOTE Necessity of locking on any side is optional, it depends on client implementation.
    bool b_bad(__bmdx_noarg1) const        { return _pdesc() ? _pdesc()->f_state1 == -1 && _pdesc()->f_state2 == -1 : false; }
    void do_recover(__bmdx_noarg1) const        { if (!b_bad()) { return; } _reset_mem(); }

      // Pointers to f_state1, f_state2 flag variables.
      //  The flags reside in shared memory.
      //  The pointers are 0 or non-0 together with p(). (0 only when shared memory is not open.)
      //  Initial flags value: 0.
      //  Special value: -1, meaning "invariant lost". When both flags are -1, b_bad() returns true,
      //    and side 1 may perform an attempt to reinitialize shared area.
      //  Other values meaning: user-defined.
      //    Sides may use state flags for keeping object states and simple communication.
      // NOTE Each signed char* here points to 1 char only.
    signed char* pf_state1() const        { return _pdesc() ? &_pdesc()->f_state1 : 0; }
    signed char* pf_state2() const        { return _pdesc() ? &_pdesc()->f_state2 : 0; }

  private:
    shmobj2s_t(const shmobj2s_t&); void operator=(const shmobj2s_t&);

    struct _shmdesc_base
    {
      s_long handler_type; // in the just opened shared memory, this is initialized last
      s_long n_shmdesc; // == sizeof(_shmdesc)
      s_ll nb; // number of bytes allocated in shared memory for T
    };
    struct _shmdesc : _shmdesc_base
    {
      unsigned char b_constructed; // 0 - not constructed, 1 - constructed (and does not change when b_destroyed becomes 1)
      unsigned char b_destroyed; // 0 - not constructed or still constructed, 1 - destroyed
      signed char f_state1; // 0 - normal, char(-1) - side 1 found data damage, any else - user-defined
      signed char f_state2; // 0 - normal, char(-1) - side 2 found data damage, any else - user-defined
      // the below fields are version-dependent
      s_long __pad1;
      s_ll __pad2;
    };
    _shmdesc* _pdesc(__bmdx_noarg1) const { return _p ? (_shmdesc*)_p - 1 : 0; }
    void _reset_mem(__bmdx_noarg1) { if (!_pdesc()) { return; } if (_pdesc()->nb > 0) { std::memset(_p, 0, size_t(_pdesc()->nb)); } *(s_long*)&_pdesc()->b_constructed = 0; }


    struct _shm_sems
    {
      typename critsec_t<_shm_sems>::csdata lkd;
      critsec_gn lkside1;
      critsec_gn lkside2;
      s_long nlk1, nlk2;
      s_ll vlk1, vlk2;
      _shm_sems(const char* name, f_shm_name_prefix prefix_cs __bmdx_noarg) throw() : lkside1((critsec_gn::t_name("M1\t") += name).c_str(), prefix_cs), lkside2((critsec_gn::t_name("M2\t") += name).c_str(), prefix_cs), nlk1(0), nlk2(0), vlk1(0), vlk2(0) {}

        // If sides (ORed 0x1, 0x2) specify at least one lock,
        //  lock_normal removes all previous locks, then tries to set the new locks as specified.
      static void lock_normal(shared_lock& target, s_long sides __bmdx_noarg) throw()
      {
        if (!(target.r && !!(sides & 3))) { return; }
        unlock_normal(target);
        _shm_sems* ps = target.r._pnonc_u();
        critsec_t<_shm_sems> __lock(10, -1, &ps->lkd); if (sizeof(__lock)) {}
        if (!!(sides & 1)) { s_long n = ps->nlk1; if (ps->lkside1.lock()) { n += 1; target.vlk1 = ps->vlk1; ps->nlk1 = n; } }
        if (!!(sides & 2)) { s_long n = ps->nlk2; if (ps->lkside2.lock()) { n += 1; target.vlk2 = ps->vlk2; ps->nlk2 = n; } }
      }

        // unlock_normal removes any locks held by target, then clears target's locks states.
        //  Parent object refernce (target.r) is not cleared.
        //  NOTE _shm_sems works as counted shared lock,
        //    so only the last unlock does release the global mutex.
      static void unlock_normal(shared_lock& target __bmdx_noarg) throw()
      {
        _shm_sems* ps = target.r._pnonc_u();
        if (!(ps && (target.vlk1 == ps->vlk1 || target.vlk2 == ps->vlk2))) { target.vlk1 = -1; target.vlk2 = -1; return; }
        critsec_t<_shm_sems> __lock(10, -1, &ps->lkd); if (sizeof(__lock)) {}
        if (target.vlk1 == ps->vlk1) { s_long n = ps->nlk1; if (n == 1) { ps->lkside1.unlock(); } if (n > 0) { ps->nlk1 = n - 1; } }
        if (target.vlk2 == ps->vlk2) { s_long n = ps->nlk2; if (n == 1) { ps->lkside2.unlock(); } if (n > 0) { ps->nlk2 = n - 1; } }
        target.vlk1 = -1; target.vlk2 = -1;
      }

        // Unconditional unlocking, only for ~shmobj2s_t / _close(true).
      void unlock_forced(s_long sides __bmdx_noarg) throw()
      {
        if (!(sides & 3)) { return; }
        critsec_t<_shm_sems> __lock(10, -1, &lkd); if (sizeof(__lock)) {}
        if (!!(sides & 1)) { ++vlk1; nlk1 = 0; lkside1.unlock(); }
        if (!!(sides & 2)) { ++vlk2; nlk2 = 0; lkside2.unlock(); }
      }
    };


    T* _p;
    union { bool _b_side1; s_long __pad1; };
    hints _hh;
    t_name _name;
    s_ll _local_nb;
    cref_t<_shm_sems> _rsems;
    shared_lock _lkmyside;



    s_long _prep(bool b_keeplk, s_long sides_cr __bmdx_noarg)
    {
      // if b_keeplk or b_side1: _rsems ensure init (may fail), _sems ensure lock (may fail)
      // _shm_open
      // check init
      //  a) check compat and size, if fact. size is larger, _shm_close, _shm_open(fact. size), (fail if size changed between close-open)
      //  b) initialize, set size and type tag
      // set p
      // if !b_keeplk: release lock

      bool b_may_cr = !!((s_long(_b_side1) | 2 * s_long(!_b_side1)) & sides_cr);
      shared_lock __slk(*this, (b_keeplk ? 4 : 0) | (b_may_cr ? sides_cr : 0));
      if ((b_keeplk || b_may_cr) && !__slk.b_lk_any()) { return _rsems ? -3 : -2; }
      if (_p) { if (b_keeplk) { _lkmyside.swap(__slk); } return 1; }
      _shmdesc* h2;
      h2 = (_shmdesc*)_shm_open(b_may_cr, _local_nb + nbover());
      if (!h2) { return -2; }
      if (h2 == (void*)1) { return -4; } // memory handle tranfer pending
      if (h2->handler_type == 0) // the memory area is accessible, but not initialized yet
      {
        if (!b_may_cr) { _shm_close(); return -4; }
        std::memset(&h2->b_constructed, 0, (char*)(h2 + 1) - (char*)&h2->b_constructed);
        h2->n_shmdesc = s_long(sizeof(_shmdesc));
        h2->nb = _local_nb;
        h2->handler_type = _hh.htype;
      }
      else
      {
        if (!(h2->handler_type == _hh.htype && h2->n_shmdesc == s_long(sizeof(_shmdesc)))) { _shm_close(); return -1; }
        if (_local_nb < h2->nb)
        {
          const s_ll nb2 = h2->nb;
          _shm_close();
          h2 = (_shmdesc*)_shm_open(b_may_cr, nb2 + nbover());
          if (!(h2->handler_type == _hh.htype && h2->n_shmdesc == s_long(sizeof(_shmdesc)))) { _shm_close(); return -1; }
          if (nb2 < h2->nb) { return -2; }
          h2->nb = nb2;
        }
      }
      _p = (T*)(h2 + 1);
      if (b_keeplk) { _lkmyside.swap(__slk); }
      return 1;
    }
    void _close(bool b_forced_unlock __bmdx_noarg)
    {
      _shm_close();
      _p = 0;
      if (b_forced_unlock) { if (_rsems) { _rsems._pnonc_u()->unlock_forced(_b_side1 ? 1 : 2); } }
        else { _lkmyside.clear(); }
    }


    #if 0
        // Clear platform-specific handles (not including this->p).
      void _shm_init0() throw();

        // a) Open (create if necessary) a shared memory area, map into this process address space.
        //    Set platform-specific handles as necessary.
        //    If the memory is not zero-initialized by default, set first 4 bytes to 0.
        //    Return a pointer to the memory.
        // b) If memory is already opened and mapped, just return the pointer.
        //    Optionally, check mapping size and resize if necessary (growth only) to nbtotal.
        // c) If the memory exists, but not yet accessible
        //    (not initialized by other side, or memory handle transfer pending),
        //    return (void*)1.
        // d) On failure, close and clear platform-specific handles. Return 0.
      void* _shm_open(bool b_maycreate, s_ll nbtotal) throw();

        // Close the shared memory area, if it's open. Clear platform-specific handles.
        // NOTE on ~shmobj2s_t, _shm_close is called automatically.
      void _shm_close() throw();
    #endif

    #ifdef _bmdxpl_Wnds
      HANDLE __h; void* __p;
      void _shm_init0(__bmdx_noarg1) throw() { __h = 0; __p = 0; }
      void* _shm_open(bool b_maycreate, s_ll nbtotal __bmdx_noarg) throw()
      {
        if (nbtotal < 0) { nbtotal = 0; }
        bool b_reset = false;
        HANDLE h2 = 0;
        if (__h) { if (__p) { return __p; } h2 = __h; }
        else
        {
          if (__p) { UnmapViewOfFile(__p); __p = 0; }
          if (b_maycreate)
          {
            h2 = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, DWORD(nbtotal >> 32), DWORD(nbtotal & 0xffffffffll), _name.c_str());
            b_reset = h2 && GetLastError() != ERROR_ALREADY_EXISTS;
          }
          else
          {
            h2 = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, _name.c_str());
          }
          if (!h2) { return 0; }
        }
        s_long* p2 = (s_long*)MapViewOfFile(h2, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_T(nbtotal));
        if (!p2) { CloseHandle(h2); __h = 0; return 0; }
        if (b_reset) { p2[0] = 0; }
        __h = h2; __p = p2;
        return __p;
      }
      void _shm_close(__bmdx_noarg1) throw() { if (__p) { UnmapViewOfFile(__p); __p = 0; } if (__h) { CloseHandle(__h); __h = 0; } }
    #endif
    #ifdef _bmdxpl_Psx
      #if 0
      #elif defined(__ANDROID__)
        int __fd; size_t __nb; void* __p; _fd_sharing __sh;
        int __shmfd_getnb(int fd) throw() { int res = ioctl(fd, ASHMEM_GET_SIZE, 0); if (res < 0) { return -1; } return res; }
        bool __shmfd_resize(int fd, s_ll nb) throw() { int res = ioctl(fd, ASHMEM_SET_SIZE, size_t(nb)); return res >= 0; }
        int __shmfd_create(s_ll nb) throw()
        {
          int fda = ::open("/dev/ashmem", O_RDWR);
            if (fda < 0) { return -1; }
          if (!__shmfd_resize(fda, nb)) { ::close(fda); return -1; }
          if (ioctl(fda, ASHMEM_SET_PROT_MASK, (unsigned long)(PROT_READ | PROT_WRITE)) < 0) { ::close(fda); return -1; }
          return fda;
        }
        void __shmfd_close(int fd __bmdx_noarg) throw() { ::close(fd); }
        void _shm_init0(__bmdx_noarg1) throw() { __fd = -1; __p = 0; __nb = 0; }
        void* _shm_open(bool b_maycreate, s_ll nbtotal __bmdx_noarg) throw()
        {
          bool b_reset = false;
          if (nbtotal < 0) { nbtotal = 0; }
          if (__fd != -1)
          {
            if (nbtotal > __nb || !__p) { if (__p) { munmap(__p, __nb); __p = 0; } if (!__shmfd_resize(__fd, nbtotal)) { _shm_close(); return 0; } __nb = nbtotal; }
            if (__p) { return __p; }
          }
          else
          {
            if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
            __fd = __sh.fd_search(_name, 0, _b_side1 ? 1 : 2);
  //cout << "__sh.fd_search " << _name.c_str() << " " << __fd << endl;
              if (__fd == -5) { __fd = -1; return (void*)1; } // delay during descriptor transfer, it will be available later
              if (__fd < 0) { __fd = -1; if (!b_maycreate) { return 0; } }
            if (__fd < 0)
            {
              __fd = __shmfd_create(nbtotal);
                if (__fd < 0) { __fd = -1; return 0; }
              s_long res = __sh.fd_expose(_name, __fd, _b_side1 ? 1 : 2, false, true);
  //cout << "CREATED/exposed name fd res " << _name.c_str() << " " << __fd << " " << res << endl;
                if (res < 1) { _shm_close(); return 0; }
              b_reset = true;
            }
            else
            {
              int n2 = __shmfd_getnb(__fd); if (n2 < 0) { _shm_close(); return 0; }
              if (nbtotal > n2) { if (!__shmfd_resize(__fd, nbtotal)) { _shm_close(); return 0; } }
            }
          }
          __p = mmap(0, nbtotal, PROT_READ|PROT_WRITE, MAP_SHARED, __fd, 0);
            if (!__p) { _shm_close(); return 0; }
          if (b_reset) { ((s_long*)__p)[0] = 0; }
          __nb = nbtotal;
          return __p;
        }
        void _shm_close(__bmdx_noarg1) throw()
        {
          if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
          if (__fd != -1) { __shmfd_close(__fd); __fd = -1; }
        }
      #else
        int __fd; int __perm; size_t __nb; void* __p;
        int __shmfile_open(int flags) throw()
        {
          #if 0
            return -1;
          #elif __APPLE__ && __MACH__
            return shm_open(_name.c_str(), flags, __perm);
          #elif defined(__FreeBSD__)
            return shm_open(_name.c_str(), flags, __perm);
          #elif defined(__SUNPRO_CC) || defined (__sun)
            return ::open(_name.c_str(), flags, __perm);
          #else
            return ::open(_name.c_str(), flags, __perm);
          #endif
        }
        void __shmfd_close(int fd __bmdx_noarg) throw() { if (fd != -1) { ::close(fd); } }
        void _shm_init0(__bmdx_noarg1) throw() { __fd = -1; __p = 0; __nb = 0; __perm = 0666; }
        void* _shm_open(bool b_maycreate, s_ll nbtotal __bmdx_noarg) throw()
        {
          bool b_reset = false;
          int fd2 = -1;
          if (__fd != -1) { if (__p) { return __p; } fd2 = __fd; }
          else
          {
            if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
            fd2 = __shmfile_open(O_RDWR);
            if (fd2 == -1 && b_maycreate) { b_reset = true; fd2 = __shmfile_open(O_CREAT|O_RDWR); }
            if (fd2 == -1) { return 0; }
          }
          size_t n2 = nbtotal > 0 ? size_t(nbtotal) : 0;
          struct stat s; std::memset(&s, 0, sizeof(s));
          if (0 != fstat(fd2, &s)) { __shmfd_close(fd2); __fd = -1; return 0; }
          if (size_t(s.st_size) < n2) { if (0 != ftruncate(fd2, n2)) { __shmfd_close(fd2); __fd = -1; return 0; } }
          #ifdef MAP_SHARED_VALIDATE
            int f = MAP_SHARED_VALIDATE;
          #else
            int f = MAP_SHARED;
          #endif
          s_long* p2 = (s_long*)mmap(0, n2, PROT_READ|PROT_WRITE, f, fd2, 0);
          if (!p2) { __shmfd_close(fd2); __fd = -1; return 0; }
          if (b_reset) { p2[0] = 0; }
          __fd = fd2; __p = p2; __nb = n2;
          return __p;
        }
        void _shm_close(__bmdx_noarg1) throw()
        {
          if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
          if (__fd != -1) { __shmfd_close(__fd); __fd = -1; }
        }
      #endif
    #endif
  };

  template<class T> shmobj2s_t<T>::shared_lock::shared_lock(shmobj2s_t& parent, s_long sides, s_long timeout_ms __bmdx_noargt) throw()
    : vlk1(-1), vlk2(-1)
  {
    double t0 = clock_ms();
    if (!parent._rsems) { parent._rsems.cm_create2(0, 0, 1, parent._name.c_str(), parent._hh.prefix_cs); }
    if (!parent._rsems) { return; }
    if (!!(sides & 4)) { sides |= parent.b_side1() ? 1 : 2; }
    r = parent._rsems;
    const double dt = timeout_ms;
    do { _shm_sems::lock_normal(*this, sides); if ((!(sides & 1) || b_lk1()) && (!(sides & 2) || b_lk2())) { return; } clear(); if (dt != 0) { sleep_mcs(100); } }
      while (dt < 0 || (dt != 0 && clock_ms() - t0 < dt));
  }
}




#ifndef __bmdx_shmfifo_valtype_string
  #define __bmdx_shmfifo_valtype_string 0
#endif
#if __bmdx_shmfifo_valtype_string
  typedef std::string t_shmfifo_string;
#else
  typedef carray_r_t<char> t_shmfifo_string;
#endif

  // Must be >= ( 100 (MIN_RINGBUF_NBYTES) + sizeof(ripc_shmem) - rfifo_nbl11::n0) ).
  //  ~!!! make it a hint in shmfifo_s ctor.
const s_ll SHMEM_NBYTES = 100 * 1024;
const s_ll MAX_MSG_NBYTES = ((1ull << 39) - 1); // (n_len * 8 - 1) bits

struct msg_queue
{
  std::deque<cref_t<t_shmfifo_string> > msgs;
  cref_t<t_shmfifo_string> rmsg1;
  bool b_just_started; // initially true, used to check if the process was restarted, while buf was already existing in some state
    // Size of structure: 96 (8*8 + 1*32)
  struct ripc_shmem
  {
    volatile s_ll ipush2; // planned end pos. of the currently popped message (used for recovery if the sending process is restarted)
    volatile s_ll ipop2; // planned end pos. of the currently popped message (used for recovery if the receiving process is restarted)
    volatile s_ll __ripc_shmem_reserved[6];

      // NOTE Let total shared memory size == nb().
      //  Then, data buffer size == nb() - sizeof(_shmdesc) - (sizeof(ripc_shmem) - rfifo_nbl11::n0)
    rfifo_nbl11 ringbuf;

  private: ripc_shmem(); ripc_shmem(const ripc_shmem&); void operator=(const ripc_shmem&); // this object may be used only by reference
  };
  shmobj2s_t<ripc_shmem> buf; // fully valid buf has f_constructed() == 1

    // Returns size of string in rmsg1 [0..MAX_MSG_NBYTES]. If no string object, returns 0.
  s_ll msg1_nmax() { if (!rmsg1) { return 0; } s_ll n =
  #if __bmdx_shmfifo_valtype_string
      s_ll(rmsg1.ref().size())
  #else
      rmsg1.ref().n()
  #endif
  ; return n > MAX_MSG_NBYTES ? MAX_MSG_NBYTES : n; }

    // mode_: see this->mode above.
    // For mode_ > 0, p_mapname must be valid non-empty string, specifying buffer name
    //    in the format required by operating system.
    // Returns:
    //    1 - success.
    //    <= 0 - failure (only for b_enable == true), values: see shmobj2s_t::prepare
  int reset(bool b_enable)
  {
    if (b_enable)
    {
      s_long res = buf.prepare(true);
      #if SMART_IPC_DEBUG
        static int nerr(0);
        if (res <= 0) { cerr << ++nerr << " " << "ERR buf.prepare(true) failed. res name " << res << " " << buf.name() << endl; sleep_mcs(100000); }
      #endif
      if (res >= 1 && buf.f_constructed() != 1)
      {
        if (buf.b_side1() == false) { return 0; }
        const s_ll nbuf = buf.nb() - s_ll(sizeof(ripc_shmem) - rfifo_nbl11::n0);
          if (nbuf < 0 || s_long(nbuf) != nbuf) { return -2; }
        buf.p()->ringbuf.init_ref(nbuf);
        buf.set_f_constructed(1);
      }
      return res;
    }
    else
    {
      buf.close();
      msgs.clear();
      rmsg1.clear();
      return 1;
    }
  }
  //~!!! update msg_queue type index (now yk_c::bytes::cmti_base_t<bmdx_shm::shmfifo_s, 2018, 7, 31, 12>::ind())
  msg_queue(const t_name_shm& name, bool b_receiver) : b_just_started(true), buf(name.c_str(), b_receiver, SHMEM_NBYTES, -943698244)  {}
private:
  msg_queue(const msg_queue&); void operator=(const msg_queue&); // the object cannot be copied
};

  // { shared memory buffer name (for system); ref. to associated objects }
  // ~!!! implementation will change to be more compiler-independent.
inline std::map<t_name_shm, cref_t<msg_queue> >& mqueues() { static std::map<t_name_shm, cref_t<msg_queue> > x; return x; }
  // the current size of mqueues (ipc_thread copies mqueues under locks when notices that size increased aka new buffer added)
inline size_t& nmqueues() { static size_t x(0); return x; }

struct ipc_thread_handler
{
  static inline s_long mylmin(s_long a, s_long b) { return a < b ? a : b; }
  static inline s_ll myllmin(s_ll a, s_ll b) { return a < b ? a : b; }

  struct ipc_thread : threadctl::ctx_base
  {
    void _thread_proc()
    {
      std::map<t_name_shm, cref_t<msg_queue> > mqueues2;
      while (!b_stop())
      {
          // mqueues size changes => new buffers added by client => try lock (if failed, will try on next iteration)
          //  => copy mqueues as is for now => work on the updated set of queues
        if (nmqueues() != mqueues2.size()) { critsec_t<msg_queue> __lock(10, 0); if (__lock.is_locked()) { mqueues2 = mqueues(); } }

        bool b_changed = false;
        const s_ll n_len = 5;
        for (std::map<t_name_shm, cref_t<msg_queue> >::iterator i = mqueues2.begin(); i != mqueues2.end(); ++i)
        {
          if (!i->second) { continue; }
          msg_queue& q = *i->second._pnonc_u();
          if (q.buf.f_constructed() != 1)
          {
            critsec_t<msg_queue> __lock(10, 0);
            if (!__lock.is_locked()) { continue; }
            if (q.buf.f_constructed() != 1) { if (q.reset(true) < 1) { continue; } }
            b_changed = true;
          }
          if (!q.buf.p()) { continue; } // should not occur
          msg_queue::ripc_shmem& shm = *q.buf.p();
          rfifo_nbl11& buf = shm.ringbuf;

          try {
            if (!q.buf.b_side1())
            {
              signed char& tr_send = *q.buf.pf_state2();
                // Sender.
                // tr_send
                //    0 - initialize, goto 1
                //    1 - wait while a message appears on the queue, pop, goto 2
                //    2 - push message length, bytes, end byte into the buffer, goto 1
                //    -3 - probably process was terminated, try to recover state (push zero bytes up to the end of incomplete message, push end byte indicating an error)
                //    -1 - permanent problem, the buffer cannot be used (invariant lost)
                //    -2 - invariant lost, going to state -3
              if (q.b_just_started)
              {
                q.b_just_started = false;
                if (tr_send == 2 || (tr_send == 1 && shm.ipush2 > buf.ipush())) { tr_send = -3; continue; }
              }

              if (tr_send == 0 || tr_send == 1)
              {
                if (tr_send == 0) { tr_send = 1; b_changed = true; }
                if (!q.rmsg1) // try to make the first queued message the current, if not yet
                {
                  critsec_t<msg_queue> __lock(10, 0);
                    if (!__lock.is_locked()) { continue; }
                  if (q.msgs.empty()) { continue; }
                  q.rmsg1 = q.msgs.front();
                  try { q.msgs.pop_front(); } catch (...) { q.rmsg1.clear(); continue; }
                  b_changed = true;
                  if (!q.rmsg1) { continue; }
                }
                if (buf.npush() >= n_len) // the buffer has enough place to accept message length, so go to next stage
                  { shm.ipush2 = (buf.ipush() + n_len + q.msg1_nmax() + 1); tr_send = 2; b_changed = true; }
                continue;
              }
              else if (tr_send == 2)
              {
                if (!q.rmsg1) { tr_send = -3; b_changed = true; continue; } // unexpected, try to recover
                s_ll n = q.msg1_nmax();
                s_ll i1 = shm.ipush2 - 1 - n;
                s_ll i0 = i1 - n_len;
                if (buf.ipush() < i0 || (buf.ipush() < i1 && buf.ipush() > i0) || buf.ipush() > shm.ipush2) { tr_send = -2; b_changed = true; continue; } // invariant

                if (buf.ipush() < i1) // // push user message string length
                {
                  if (buf.npush() < n_len) { continue; }
                  char z[n_len]; z[0] = char(n >> 32) & 127; set_be4(z, 1, s_long(n));
                  buf.push(z, n_len);
                  b_changed = true;
                }

                if (buf.ipush() < shm.ipush2 - 1) // push data
                {
                  s_ll j = buf.ipush() - i1;
                  if (j >= n) { tr_send = -3; b_changed = true; continue; } // unexpected, try to recover
                  b_changed = 0 != buf.push(&q.rmsg1.ref()[j], n - j) || b_changed;
                }

                if (buf.ipush() == shm.ipush2 - 1) // push end byte == 1, indicating successful msg. completion
                {
                  b_changed = 0 != buf.push_bytes(1, 1) || b_changed;
                }

                if (buf.ipush() == shm.ipush2)
                {
                  q.rmsg1.clear();
                  tr_send = 1;
                  b_changed = true;
                }

                continue;
              }
              else if (tr_send == -3) // attempt to recover (push zero bytes until ipush2), then return to tr_send 1
              {
                if (q.rmsg1) { q.rmsg1.clear(); }
                if (buf.ipush() < shm.ipush2) { if (buf.npush() > 0) { b_changed = 0 != buf.push_bytes(0, shm.ipush2 - buf.ipush()) || b_changed; } }
                  else if (buf.ipush() == shm.ipush2) { tr_send = 1; b_changed = true; }
                  else { tr_send = -2; } // invariant drop
                continue;
              }
              else if (tr_send == -1) { continue; } // ignore this buffer (invariant lost, cannot be recovered until both sender and receiver exit and restart)
              else // -2 or anything else, print message once, set bad flag and go to passive state
              {
                #if SMART_IPC_DEBUG
                  cerr << "ERR The following output buffer is damaged (to fix, stop both client and server, and then restart): " << i->first << endl;
                #endif
                q.rmsg1.clear();
                tr_send = -1;
                continue;
              }
            }
            else
            {
              signed char& tr_rcv = *q.buf.pf_state1();
                // Recevier.
                // tr_rcv
                //    0 - initialize, goto 1
                //    1 - wait until message length (fields of n_len bytes) appears in the buffer, peek into it, prep. string for input, goto 2
                //    2 - discard msg. length, read msg. bytes, read end byte, [if end succeeded: push string into the queue], clear string, goto 1
                //    -1 - recover from process restart (skip all bytes in buf. until ipop2)
                //    -2, -3 - permanent problem, the buffer cannot be used (invariant dropped)
              if (q.b_just_started)
              {
                q.b_just_started = false;
                if (tr_rcv == 2) { tr_rcv = -3; continue; }
              }

              if (tr_rcv == 0 || tr_rcv == 1)
              {
                if (tr_rcv == 0) { tr_rcv = 1; b_changed = true; }
                if (buf.npop() < n_len) { continue; }
                char z[n_len];
                buf.pop(z, n_len, false); // peek into msg. length
                s_ll n = (s_ll(z[0] & 127) << 32) + be4(z, 1); // user message string length
                if (n < 0) { tr_rcv = -2; continue; } // invariant drop
                shm.ipop2 = buf.ipop() + n_len + n + 1;
                b_changed = true;
                q.rmsg1.create2(1, n, char(0));
                if (q.rmsg1) { tr_rcv = 2; }
                continue;
              }
              else if (tr_rcv == 2)
              {
                if (!q.rmsg1) { tr_rcv = -3; b_changed = true; continue; } // unexpected, try to recover
                s_ll n = q.msg1_nmax();
                s_ll i1 = shm.ipop2 - 1 - n;
                s_ll i0 = i1 - n_len;
                if (buf.ipop() < i0 || (buf.ipop() < i1 && buf.ipop() > i0) || buf.ipop() > shm.ipop2) { tr_rcv = -2; b_changed = true; continue; } // invariant

                if (buf.ipop() < i1) // pop length
                {
                  if (buf.npop() < n_len) { continue; }
                  buf.discard(n_len);
                  b_changed = true;
                }

                if (buf.ipop() < shm.ipop2 - 1) // pop data
                {
                  s_ll j = buf.ipop() - i1;
                  if (j >= n) { tr_rcv = -1; b_changed = true; continue; } // unexpected, try to recover
                  b_changed = 0 != buf.pop((char*)&q.rmsg1.ref()[j], n - j) || b_changed;
                }

                if (buf.ipop() == shm.ipop2 - 1) // pop end byte, check if it's 1 (if the message is correct), or otherwise
                {
                  if (buf.npop() < 1) { continue; }
                  char c_end(0);
                  buf.pop(&c_end, 1);
                  if (c_end != 1) { q.rmsg1.clear(); tr_rcv = 1; continue; }
                  b_changed = true;
                }

                if (buf.ipop() == shm.ipop2)
                {
                  critsec_t<msg_queue> __lock(10, 0);
                    if (!__lock.is_locked()) { continue; } // if cannot lock the message queue, try again later
                  try { q.msgs.push_back(q.rmsg1); } catch (...) { continue; } // if queue push fails, try again later
                  q.rmsg1.clear();
                  tr_rcv = 1;
                  b_changed = true;
                }

                continue;
              }
              else if (tr_rcv == -3) // attempt to recover (discard bytes until ipop2), then return to tr_rcv 1
              {
                if (q.rmsg1) { q.rmsg1.clear(); }
                if (buf.ipop() < shm.ipop2) { b_changed = 0 != buf.discard(shm.ipop2 - buf.ipop()) || b_changed; }
                  else if (buf.ipop() == shm.ipop2) { tr_rcv = 1; b_changed = true; }
                  else { tr_rcv = -2; } // invariant drop
                continue;
              }
              else if (tr_rcv == -1) { continue; } // ignore this buffer (invariant lost, cannot be recovered until both sender and receiver exit and restart)
              else // -2 or anything else, print message once, set bad flag and go to passive state
              {
                #if SMART_IPC_DEBUG
                  cerr << "ERR The following input buffer is damaged (to fix, stop both client and server, and then restart): " << i->first << endl;
                #endif
                q.rmsg1.clear();
                tr_rcv = -1;
                continue;
              }
            }
          } catch (...) { b_changed = true; }
        }
        if (!b_changed) { sleep_mcs(5000); }
      }
    }
  };

  threadctl tc;
  bool b_started;

    // Starts thread. If succeeds or thread already started, b_started will be true.
  void start() { if (!b_started) { b_started = tc.start_auto<ipc_thread>(int()); } }
  ipc_thread_handler() { b_started = false; start(); }
    // Handler destruction waits until thread has exited, because it accesses static objects.
    //  Only 1 instance of ipc_thread_handler is created, as static variable (see th() below).
  ~ipc_thread_handler() { if (b_started && tc) { tc.signal_stop(); while (tc) { sleep_mcs(5000); } b_started = false; } }
};


  // Calling th() ensures single IPC thread (struct ipc_thread) start.
  // NOTE th() must be called under lock.
inline ipc_thread_handler& th()
{
  static ipc_thread_handler x; // automatically initialized on first call
  if (!x.b_started) { x.start(); }
  return x;
}

namespace _api // public declarations (merged into namespace smart_ipc)
{

    // IPC queue (FIFO buffer) of strings, based on shared memory.
  struct shmfifo_s
  {
    typedef t_name_shm t_name;

      // Global name of the queue.
    const t_name name;

      // Cached result of the last operation.
    mutable s_long res;

      // Cached data reference of the last peek operation (pop_message with mode 0 or 1).
      // NOTE If the client uses pop_str to retrieve messages, d is kept empty.
    mutable cref_t<t_shmfifo_string> d;

      // name: global name of the queue.
    shmfifo_s(const t_name& name_) : name(name_.n() ? name_ : "_"), res(0) {}

      // timeout_ms:
      //    <0 - block until shared memory buffer creation by the receiving side, or till an error.
      //    0 - try send message, and return the result immediately.
      //    >0 - same as timeout_ms < 0, but returns 0 when timeout occured.
      //      or any message is received, or error occurs.
      // Returns:
      //  2 - the message successfully pushed into the output queue.
      //    (The receiving side exists, shared memory buffer is available. The message will be sent by servicing thread.)
      //  1 (only on timeout_ms == 0) - the message is queued,
      //      but existing shared memory buffer is not yet initialized.
      //      This is optimistic stategy, relying on normal workflow.
      //      Alternatively, small timeout_ms (e.g. 1) may be used, causing 0 to be returned in "not yet initialized" case.
      //  0 (only on timeout_ms >= 1) - failure: timeout during shared memory initialization
      //      by the receiving side (the program is not run, or more time needed?).
      //  -1 - attempt to push message into the input queue.
      //  -2 - failure (memory allocation, incompatibility, message too long etc.).
      //  -3 - shared memory is locked by some other side.
    s_long push_message(const arrayref_t<char>& msg, long timeout_ms = 0) const
    {
      if (!th().b_started) { res = -2; return -2; }
      if (msg.n() > MAX_MSG_NBYTES) { res = -2; return -2; }

      double t0 = clock_ms();

      cref_t<msg_queue> rq_prot;
      msg_queue* pq = 0;
      try {
        critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
        if (1)
        {
          cref_t<msg_queue>& rq = mqueues()[name];
          pq = rq._pnonc_u();
          nmqueues() = mqueues().size();
          if (!pq) { rq.create2(1, name, false); } // false: sender
          rq_prot = rq;
        }
        pq = rq_prot._pnonc_u();
        if (pq)
        {
          int res2 = 0;
          if (pq->buf.b_side1() != false) { res = -1; return -1; } // the queue already works in opposite direction
          if (pq->buf.f_constructed() != 1)
          {
            res2 = pq->reset(true);
            if (res2 == -4) { res2 = 0; }
            if (res2 < 0) { res = res2 == -1 ? -2 : res2; return res; }
          }

          if (pq->buf.f_constructed() == 1 || (res2 == 0 && timeout_ms == 0)) // buffer is available, or initialization pending (push message optimistically)
          {
              // Push message into the queue under same lock as used to access mqueues.
            try {
              cref_t<t_shmfifo_string> rmsg;
              char z = 0;
              rmsg.create2(0, msg.pd() ? msg.pd() : &z, msg.n()); // create1 may throw
              pq->msgs.push_back(rmsg); // push_back may throw
              res = res2 > 0 ? 2 : 1; return res;
            } catch (...) {}
            res = -2; return -2;
          }
          // else reset() attempts will continue in a loop (see below)
        }
      } catch (...) {}
      if (!pq) { res = -2; return -2; }

        // Wait until the receiving side has created its input buffer (inside IPC thread), or exit on timeout.
      if (pq->buf.f_constructed() != 1)
      {
        while (true)
        {
          if (1)
          {
            critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
            if (pq->buf.f_constructed() == 1) { break; }
          }
          if (timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { res = 0; return 0; }
          sleep_mcs(5000);
        }
      }

      try {
        cref_t<t_shmfifo_string> rmsg;
        char z = 0;
        rmsg.create2(0, msg.pd() ? msg.pd() : &z, msg.n()); // create1 may throw
        critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
        pq->msgs.push_back(rmsg); // push_back may throw
        res = 2; return 2;
      } catch (...) {}
      res = -2; return -2;
    }

      // timeout_ms:
      //    <0 - block until any message is received or error occurs.
      //    0 - check once (no sleep), and return a message if available.
      //    >0 - while the queue is empty, check with regular intervals (5 ms), until timeout occurs,
      //      or any message is received, or error occurs.
      // b_do_pop:
      //      true: pop message (remove from the queue).
      //      false: do not pop (i.e. peek only).
      // Returns:
      //    1 - the message is available (popped or peeked as specified in mode).
      //      this->d contains valid object.
      //    0 - no message in the queue (if timeout_ms > 0 was specified, then it has passed already)
      //      this->d is cleared.
      //    -1 - attempt to pop message from the output queue
      //    -2 - failure (memory allocation, incompatibility etc.).
      //    -3 - shared memory is locked by some other side.
      //  On any result < 0, this->d is not modified.
    s_long get_message(long timeout_ms = 0, bool b_do_pop = true) const
    {
      double t0 = clock_ms();

      if (!th().b_started) { res = -2; return -2; }

      msg_queue* pq = 0;
      cref_t<msg_queue> rq_prot;
      try {
        critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
        if (1)
        {
          cref_t<msg_queue>& rq = mqueues()[name];
          pq = rq._pnonc_u();
          nmqueues() = mqueues().size();
          if (!pq) { rq.create2(1, name, true); } // true: receiver
          rq_prot = rq;
        }
        pq = rq_prot._pnonc_u();
        if (pq)
        {
          if (pq->buf.b_side1() != true) { res = -1; return -1; } // the queue already works in opposite direction
          if (pq->buf.f_constructed() != 1)
          {
            int res2 = pq->reset(true);
            if (res2 == -4) { res2 = 0; }
            if (res2 < 0) { res = res2 == -1 ? -2 : res2; return res; }
          }
          // else already initialized correctly
        }
      } catch (...) {}
      if (!pq) { res = -2; return -2; }

        // Wait until the sending side completes sending the shared memory descriptor, or exit on timeout.
      if (pq->buf.f_constructed() != 1 && timeout_ms != 0)
      {
        while (true)
        {
          if (1)
          {
            critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
            if (pq->buf.f_constructed() == 1) { break; }
          }
          if (timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { res = 0; return 0; }
          sleep_mcs(5000);
        }
      }

        // Wait until timeout, or error, or getting a non-empty object with message from the queue.
      while (true)
      {
        if (1)
        {
          critsec_t<msg_queue> __lock(10, -1); if (sizeof(__lock)) {}
          while (pq->msgs.size() > 0)
          {
            if (!pq->msgs.front()) { try { pq->msgs.pop_front(); } catch (...) { res = -2; return -2; } continue; }
            if (b_do_pop)
            {
              cref_t<t_shmfifo_string> d2;
              bmdx_str::words::swap_bytes(pq->msgs.front(), d2);
              try { pq->msgs.pop_front(); } catch (...) { bmdx_str::words::swap_bytes(pq->msgs.front(), d2); res = -2; return -2; }
              this->d = d2;
            }
            else
            {
              this->d = pq->msgs.front();
            }
            res = 1; return 1;
          }
        }
        if (timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { this->d.clear(); res = 0; return 0; }
        sleep_mcs(5000);
      }
    }

    struct exc__pop_str : std::exception { const char* what() const throw() { return "shmfifo_s::_pop_str"; } };

      // Convenience function.
      //  Pops message into this->d, then returns its copy as std string.
      //  On no message, the returned string is empty, as well as this->d.
      //  On any error, an exception is generated.
      //  After exception, this->d may still contain a message, which will be returned on the next call to pop_str.
    std::string pop_str(long timeout_ms = 0)
    {
      struct __local_des { shmfifo_s* p; __local_des(shmfifo_s& x_) : p(&x_) {} ~__local_des() { if (p) { p->d.clear(); } } };
      if (!this->d)
      {
        s_long res2 = get_message(timeout_ms);
        if (res2 < 0) { throw exc__pop_str(); }
        if (res2 == 0) { return std::string(); }
      }
      __local_des des(*this);
      #if __bmdx_shmfifo_valtype_string
        try { return this->d.ref(); } catch (...) { des.p = 0; }
      #else
        try { return this->d.ref().str(); } catch (...) { des.p = 0; }
      #endif
      throw exc__pop_str();
    }
  };


}

} using namespace _bmdx_shm::_api; } // end all namespaces

#endif // bmdx_shmem_H
#endif // bmdx_part_shm
