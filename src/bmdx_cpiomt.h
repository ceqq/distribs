// BMDX library 1.4 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
//  Cross-platform input/output, IPC, multithreading. Standalone header.
// rev. 2020-07-30
//
// Contacts: bmdx-dev [at] mail [dot] ru, z7d9 [at] yahoo [dot] com
// Project website: hashx.dp.ua
//
// Copyright 2004-2020 Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the Software or the use or other dealings in the Software.
// https://opensource.org/licenses/MIT


// In bmdx_cpiomt.h:
//
//    Automatic stdint-independent definition for 32, 64-bit signed integers.    (bmdx_meta::  s_long, u_long, s_ll, u_ll)
//    Read/write BE, LE data words from/into memory.    (bmdx_str::words::  le2 le4 le8 .. be8, set_le2 .. set_be8)
//    Conversions between strings and numbers.    (bmdx_str::conv::  str_from_*, ntocs, ntows, str2i, str2f)
//    Fixed-length string.    (bmdx_str::  flstr_t)
//
//    Timer with near-microseconds resolution.    (bmdx::  clock_ms)
//    Sleep with near-microseconds resolution.    (bmdx::  sleep_mcs)
//    Recursive mutex.    (bmdx::  struct critsec_t)
//    Unified thread control. Priorities, high-level data passing to thread, forced termination etc.    (bmdx::  struct threadctl)
//    Parallel task processing helper.     (bmdx::  struct multithread)
//    Simple process launcher. Execute, wait, check running state.    (bmdx::  struct processctl)
//    Thread-safe smart pointer with customizable allocations, embedded helper objects and events.    (bmdx::  struct cref_t)
//
//    Basic unbuffered console input.    (bmdx::  struct console_io)
//    Bytewise file i/o + save/load whole file as string.    (bmdx::  struct file_io)
//
//    Cross-module arrays and non-blocking, lockless queues:
//      bmdx::  carray_t, cpparray_t, carray_r_t, arrayref_t
//      bmdx::  cringbuf1_t, cppringbuf1_t, vnnqueue_t
//
//    IPC helpers (namespace bmdx_shm):
//      global named critical section    (struct critsec_gn),
//      container template for shared memory object with locks    (struct shmobj2s_t),
//      simple non-blocking queue of bytes at the given memory location   (struct rfifo_nbl11),
//      global named queue of string messages   (struct shmqueue_s).

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
#ifndef __bmdx_noex
#if (__cplusplus >= 201703) || (__APPLE__ && __MACH__ && __cplusplus >= 201406)
  #define __bmdx_noex noexcept
  #define __bmdx_exs(a) noexcept(false)
#else
  #define __bmdx_noex throw()
  #define __bmdx_exs(a) throw(a)
#endif
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

    // 16, 32, 64-bit integers.
  typedef bmdx_meta::find_size_n<2, signed int, signed short, wchar_t>::result s_short;
  typedef bmdx_meta::find_size_n<4, signed long int, signed int, signed short>::result s_long;
  typedef bmdx_meta::find_size_n<4, unsigned long int, unsigned int, unsigned short>::result u_long;
  typedef bmdx_meta::find_size_n<8, signed long long int, signed long int, signed int>::result s_ll;
  typedef bmdx_meta::find_size_n<8, unsigned long long int, unsigned long int, unsigned int>::result u_ll;

    // Signed integer, large enough to hold linear pointers difference.
  typedef if_t<(sizeof(void*) > 4), s_ll, s_long>::result t_pdiff;
}

#undef _s_long
#undef _s_ll
#undef _u_ll
#ifdef yk_c_vecm_hashx_H
  #define _s_long ::yk_c::s_long
  #define _s_ll ::bmdx_meta::s_ll
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
#include <cstddef>
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
      // NOTE Values are treated as signed integer.
      // NOTE pos is the offset in bytes from p.
    static inline _s_ll le8(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_ll x = _s_ll(*pc++); x |= _s_ll(*pc++) << 8; x |= _s_ll(*pc++) << 16; x |= _s_ll(*pc++) << 24; x |= _s_ll(*pc++) << 32; x |= _s_ll(*pc++) << 40; x |= _s_ll(*pc++) << 48; x |= _s_ll(*pc) << 56; return x; }
    static inline _s_long le4(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc++) << 8; x |= _s_long(*pc++) << 16; x |= _s_long(*pc) << 24; return x; }
    static inline _s_long le2(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc) << 8; return (x << 16) >> 16; }
    static inline void set_le8(void* p, _s_long pos, _s_ll x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_le4(void* p, _s_long pos, _s_long x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_le2(void* p, _s_long pos, _s_long x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc = char(x);  }

      // NOTE Values are treated as signed integer.
      // NOTE pos is the offset in bytes from p.
    static inline _s_ll be8(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 7; _s_ll x = _s_ll(*pc--); x |= _s_ll(*pc--) << 8; x |= _s_ll(*pc--) << 16; x |= _s_ll(*pc--) << 24; x |= _s_ll(*pc--) << 32; x |= _s_ll(*pc--) << 40; x |= _s_ll(*pc--) << 48; x |= _s_ll(*pc) << 56; return x; }
    static inline _s_long be4(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 3; _s_long x = _s_long(*pc--); x |= _s_long(*pc--) << 8; x |= _s_long(*pc--) << 16; x |= _s_long(*pc) << 24; return x; }
    static inline _s_long be2(const void* p, _s_long pos) __bmdx_noex { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 1; _s_long x = _s_long(*pc--); x |= _s_long(*pc) << 8; return (x << 16) >> 16; }
    static inline void set_be8(void* p, _s_long pos, _s_ll x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos + 7; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_be4(void* p, _s_long pos, _s_long x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos + 3; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    static inline void set_be2(void* p, _s_long pos, _s_long x) __bmdx_noex { _yk_reg char* pc = ((char*)p) + pos + 1; *pc-- = char(x); x >>= 8; *pc = char(x);  }

    #undef __bmdx_null_pchar
    #define __bmdx_null_pchar ((char*)1 - 1)
    template<class T, class Aux = bmdx_meta::nothing, class _ = yk_c::__vecm_tu_selector> struct memmove_t
    {
      typedef bmdx_meta::t_pdiff t_pdiff;
      typedef _s_ll s_ll;
      typedef _s_long s_long;
      static inline void sf_memmove(void* dest_, const void* src_, t_pdiff n) __bmdx_noex
      {
        if (dest_ == src_ || n <= 0) { return; }
        char* s = (char*)src_;
        char* d = (char*)dest_;
        if (s > d)
        {
          t_pdiff als = (s - __bmdx_null_pchar) & 7;
          t_pdiff ald = (d - __bmdx_null_pchar) & 7;
          char* const d3 = d + n;
          if (als == ald)
          {
            typedef s_ll word; enum { shift = 3 };
            if (als) { char* d2 = d + sizeof(word) - als; if (d2 > d3) { d2 = d3; } do { *d++ = *s++; } while (d != d2); }
            if (1) { word* d2 = (word*)d + ((d3 - d) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *qd++ = *qs++; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d3) { *d++ = *s++; }
            return;
          }
          if (!((als ^ ald) & 3))
          {
            als &= 3;
            typedef s_long word; enum { shift = 2 };
            if (als) { char* d2 = d + sizeof(word) - als; if (d2 > d3) { d2 = d3; } do { *d++ = *s++; } while (d != d2); }
            if (1) { word* d2 = (word*)d + ((d3 - d) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *qd++ = *qs++; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d3) { *d++ = *s++; }
            return;
          }
          if (0)
          {
            typedef s_ll word; enum { shift = 3 };
            if (ald) { char* d2 = d + sizeof(word) - ald; if (d2 > d3) { d2 = d3; } do { *d++ = *s++; } while (d != d2); }
            if (1) { word* d2 = (word*)d + ((d3 - d) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *qd++ = *qs++; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d3) { *d++ = *s++; }
            return;
          }
        }
        else
        {
          char* const d0 = d; d += n; s += n;
          t_pdiff als = (s - __bmdx_null_pchar) & 7;
          t_pdiff ald = (d - __bmdx_null_pchar) & 7;
          if (als == ald)
          {
            typedef s_ll word; enum { shift = 3 };
            if (als) { char* d2 = d - (als < n ? als : n); do { *--d = *--s; } while (d != d2); }
            if (d == d0) { return; }
            if (1) { word* d2 = (word*)d - ((d - d0) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *--qd = *--qs; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d0) { *--d = *--s; }
            return;
          }
          if (!((als ^ ald) & 3))
          {
            als &= 3;
            typedef s_long word; enum { shift = 2 };
            if (als) { char* d2 = d - (als < n ? als : n); do { *--d = *--s; } while (d != d2); }
            if (d == d0) { return; }
            if (1) { word* d2 = (word*)d - ((d - d0) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *--qd = *--qs; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d0) { *--d = *--s; }
            return;
          }
          if (0)
          {
            typedef s_ll word; enum { shift = 3 };
            if (ald) { char* d2 = d - (ald < n ? ald : n); do { *--d = *--s; } while (d != d2); }
            if (d == d0) { return; }
            if (d - s >= 8) { word* d2 = (word*)d - ((d - d0) >> shift); if ((word*)d != d2) { _yk_reg word* qd = (word*)d; _yk_reg word* qs = (word*)s; do { *--qd = *--qs; } while (qd != d2); d = (char*)qd; s = (char*)qs; } }
            while (d != d0) { *--d = *--s; }
            return;
          }
        }
        std::memmove(dest_, src_, n);
        return;
      }

        // pchs: accumulator (the calculated checksum will be added to *pchs.
      static void sf_memcpy(void* pdest_, const void* psrc_, _s_ll n, _s_long* pchs = 0)
      {
        if (n <= 0) { return; }
        if (pchs)
        {
          typedef _u_ll word; enum { shift = 3 };
          typedef const word* pcword;
          typedef const unsigned char* pcchar;
          pcchar s = (pcchar)psrc_;
          pcchar const s3 = s + n;
          const t_pdiff als = (s - (pcchar)__bmdx_null_pchar) & 7;
          if (als) { _s_long chs = 0; pcchar s2 = s + sizeof(word) - als; if (s2 > s3) { s2 = s3; } do { chs += *s++; } while (s != s2); *pchs += chs; }
          if (1) { pcword s2 = (pcword)s + ((s3 - s) >> shift); if ((pcword)s != s2) { _yk_reg pcword qs = (pcword)s;_yk_reg _s_long chs = 0; do { _yk_reg word w = *qs; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; chs += (unsigned char)w; w >>= 8; } while (++qs != s2); *pchs += chs; s = (pcchar)qs; } }
          if (1) { _s_long chs = 0; while (s != s3) { chs += *s++; } *pchs += chs; }
        }
        #if 1
          sf_memmove(pdest_, psrc_, t_pdiff(n));
        #else
          std::memcpy(pdest_, psrc_, size_t(n));
        #endif
      }
    };

     template<class T> static inline void swap_bytes(T& a, T& b) { enum { n = sizeof(T)  }; if (!n || &a == &b) { return; } _s_ll c[(n + 7) / 8]; memmove_t<char>::sf_memcpy(&c[0], &a, n); memmove_t<char>::sf_memcpy(&a, &b, n); memmove_t<char>::sf_memcpy(&b, &c[0], n); }
 }

  namespace conv
  {
    struct exc_str2i : std::exception { const char* what() const __bmdx_noex { return "bmdx_str::str2i"; } };
    struct exc_str2f : std::exception { const char* what() const __bmdx_noex { return "bmdx_str::str2f"; } };
    struct exc_conv_ws : std::exception { enum { nmax = 80 }; char msg[nmax ]; const char* what() const __bmdx_noex { return msg; } exc_conv_ws(const char* s1, const char* s2 = 0) { _s_long n = nmax - 1; char* p = msg; if (s1) { while (*s1 && n > 0) { *p++ = *s1++; } } if (s2) { while (*s2 && n > 0) { *p++ = *s2++; } } *p = '\0'; } };

    template<class _ = yk_c::__vecm_tu_selector>
    struct _bmdx_str_impl
    {
      static _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) __bmdx_noex;
      static _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax, bool b_nans) __bmdx_noex;
      static _s_ll str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans);
      static _s_ll str2i(const char* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const char* x, _s_ll xlen, double dflt, bool no_exc, bool b_nans);
      static std::string _wsbs_utf8(const wchar_t* ps, _s_ll n);
      static std::wstring _bsws_utf8(const char* ps, _s_ll n);
    };

    static inline bool is_finite(double x)
      { return __bmdx_isfinite(x); }

    template<class _>
    _s_long _bmdx_str_impl<_>::str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) __bmdx_noex
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
    _s_long _bmdx_str_impl<_>::str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax, bool b_nans) __bmdx_noex
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
      if (nfracmax < 0) { nfracmax = 0; } else if (nfracmax > nfrac_mm) { nfracmax = nfrac_mm; }
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
      _s_long n_int = 0; _s_ll round_mult = 1; if (true) { _s_long i2 = i; do { ++n_int; i2 /= 10; round_mult *= 10; } while (i2); }
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
        _s_ll fr = _s_ll(x2 * 1.e13 + 0.05 * round_mult), mult = 1000000000000ll;
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

    template<class _>
    std::string _bmdx_str_impl<_>::_wsbs_utf8(const wchar_t* ps, _s_ll n)
    {
      try {
        if (!ps) { throw exc_conv_ws("_wsbs_utf8.3"); }
        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
        std::string s; if (n <= 0) { return s; }
        _s_ll nrsv = n; s.reserve(size_t(nrsv));
        for (_s_ll i = 0; i < n; ++i)
        {
          _s_long q = _s_long(ps[i]) & 0xffff;
          if (q >= 0xd800 && q < 0xe000)
          {
            if (q >= 0xdc00 || i + 1 >= n) { q = L'?'; }
            else
            {
              _s_long q2 = _s_long(ps[i + 1]) & 0xffff;
              if (q2 >= 0xdc00 && q2 < 0xe000) { q = (((q - 0xd800) << 10) | (q2 - 0xdc00)) + 0x10000; ++i; }
                else { q = L'?'; }
            }
          }
          if (q <= 0x7f) { s += char(q); }
            else if (q <= 0x7ff) { s += char((q >> 6) | 0xc0); s += char((q & 0x3f) | 0x80); }
            else if (q <= 0xffff) { s += char((q >> 12) | 0xe0); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
            else { s += char((q >> 18) | 0xf0); s += char(((q >> 12) & 0x3f) | 0x80); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
          if ((i & 0xfff) == 0) { _s_ll n2 = _s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(size_t(nrsv)); } }
        }
        return s;
      }
      catch (exc_conv_ws&) { throw; }
      catch (const std::exception& e) { throw exc_conv_ws("_wsbs_utf8.1", e.what()); }
      catch (...) { throw exc_conv_ws("_wsbs_utf8.2"); }
    }

    template<class _>
    std::wstring _bmdx_str_impl<_>::_bsws_utf8(const char* ps, _s_ll n)
    {
      try {
        if (!ps) { throw exc_conv_ws("_bsws_utf8.3"); }
        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
        std::wstring s; if (n <= 0) { return s; }
        _s_ll nrsv = n / 2; s.reserve(size_t(nrsv));
        _s_ll i = 0, rsvi = 0;
        while (i < n)
        {
          unsigned char c = ps[i];
          if ((c & 0x80) == 0) { s += wchar_t(c); ++i; }
          else
          {
            _s_long nbchr = 0;
            while (((c << nbchr) & 0xc0) == 0xc0) { ++nbchr; }
            if (nbchr >= 1 && nbchr <= 5 && i + nbchr < n)
            {
              _s_long q(c & (0x3f >> nbchr)); ++i;
              do { c = ps[i]; if ((c & 0xc0) != 0x80) { q = L'?'; break; } q = (q << 6) | (c & 0x3f); ++i; } while (--nbchr);
              if (q > 0x10ffff) { s += L'?'; }
                else if (q >= 0x10000) { q -= 0x10000; s += wchar_t(0xd800 | (q >> 10)); s += wchar_t(0xdc00 | (q & 0x3ff)); }
                else { s += wchar_t(q); }
            }
            else { s += L'?'; ++i; }
          }
          if (i - rsvi > 0xfff) { rsvi = i; _s_ll n2 = _s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(size_t(nrsv)); } }
        }
        return s;
      }
      catch (exc_conv_ws&) { throw; }
      catch (const std::exception& e) { throw exc_conv_ws("_bsws_utf8.1", e.what()); }
      catch (...) { throw exc_conv_ws("_bsws_utf8.2"); }
    }
  }



    // Fixed-length string of char.
    //    Automatic conversion from std::string, std::wstring, integer types, floating-point types.
    //    Min., max. length: 4, 2^20 - 1.
  template<_s_long nmax_>
  struct flstr_t
  {
  private: enum { _m_size = 0xfffff, _sh_res = 20 };
  public:
    typedef flstr_t t_string; enum { nmin_c = 4, nmax_c = nmax_ >= nmin_c ? (nmax_ <= _m_size ? nmax_ : _s_long(_m_size)) : nmin_c, res_max = _m_size, res_min = -res_max };

    flstr_t() { _nr = 1 << _sh_res; _x[0] = 0; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already.
    flstr_t(const char* ps, _s_ll n = -1) __bmdx_noex { _nr = 0; _x[0] = 0; _set_res_u(_append_s(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::string& s) __bmdx_noex { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }
    flstr_t(const wchar_t* ps, _s_ll n = -1) __bmdx_noex { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::wstring& s) __bmdx_noex { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(s.c_str(), s.length())); }

    template<_s_long nmax2> flstr_t(const flstr_t<nmax2>& s) __bmdx_noex { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }

    flstr_t(double x, _s_long ndmmax = 6, _s_long nfracmax = 12, bool b_nans = true) __bmdx_noex { _nr = 0; _s_long n = conv::_bmdx_str_impl<>::str_from_double(x, _x, nmax(), ndmmax, nfracmax, b_nans); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 ? 1 : (n >= 0 ? 0 : -1)); }
    flstr_t(_s_ll x, bool b_signed) __bmdx_noex { _nr = 0; _s_long n = conv::_bmdx_str_impl<>::str_from_s_ll(x, _x, nmax(), b_signed); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 || (n == 1 && _x[0] != '-' && _x[0] != '+') ? 1 : (n >= 0 ? 0 : -1)); }

    flstr_t(signed short x) __bmdx_noex { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed int x) __bmdx_noex { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long x) __bmdx_noex { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long long x) __bmdx_noex { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(unsigned short x) __bmdx_noex { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned int x) __bmdx_noex { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long x) __bmdx_noex { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long long x) __bmdx_noex { new (this) flstr_t(_s_ll(x), false); }

    flstr_t(const void* x) __bmdx_noex { new (this) flstr_t((_s_ll)x, false); }

    _s_long nmax() const  __bmdx_noex { return nmax_c; }
    _s_long length() const __bmdx_noex { return _nr & _m_size; }
    _s_long size() const __bmdx_noex { return _nr & _m_size; }
    _s_long n() const __bmdx_noex { return _nr & _m_size; }
    _s_long is_full() const __bmdx_noex { return length() == nmax(); }
    _s_long is_empty() const __bmdx_noex { return length() == 0; }
    _s_long res() const __bmdx_noex { return _nr >> _sh_res; }
    const char* c_str() const __bmdx_noex { return _x; }
    const char* pd() const __bmdx_noex { return _x; }
    _s_long type() const __bmdx_noex { return 0; }

      // If n >= 0, chars [pos0..min(pos0+n, length())) are returned.
      //  If n < 0, chars [pos0..length()) are returned.
    t_string substr(_s_long pos0, _s_long n) const __bmdx_noex { _s_long n0 = length(); if (n <= 0) { if (n == 0) { return t_string(); } n = n0; } if (n > n0) { n = n0; } if (pos0 < 0) { pos0 = 0; } else if (pos0 > n0) { pos0 = n0; } if (pos0 + n > n0) { n = n0 - pos0; } return t_string(_x + pos0, n); }
      // Starts searching from pos0.
      //  (find_any) n_pcc >= 0 specifies num of chars. in pcc.
      // Returns [0..length()), or -1 if not found.
    _s_long find(char c, _s_long pos0 = 0) const __bmdx_noex { _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } while (pos0 < pos2) { if (_x[pos0] == c) { return pos0; } ++pos0; } return -1; }
    _s_long find_any(const char* pcc, _s_long pos0 = 0, _s_long n_pcc = -1) const __bmdx_noex { if (!pcc) { return -1; } _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } if (pos0 >= pos2) { return -1; } if (n_pcc < 0) { n_pcc = 0; const char* p = pcc; while (*p++) { ++n_pcc; } } while (pos0 < pos2) { char c = _x[pos0]; for (_s_long i = 0; i < n_pcc; ++i) { if (c == pcc[i]) { return pos0; } } ++pos0; } return -1; }

    #ifdef _MSC_VER
      inline operator std::string() const { return std::string(&_x[0], length()); }
      inline operator std::wstring() const { return wstr(); }
    #else
      inline operator std::string() const __bmdx_exs(std::exception) { return std::string(&_x[0], length()); }
      inline operator std::wstring() const __bmdx_exs(std::exception) { return wstr(); }
    #endif
    inline std::string str(__bmdx_noarg1) const { return std::string(&_x[0], length()); }
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
    void set_res(_s_long x) __bmdx_noex { if (x < res_min) { x = res_min; } else if (x > res_max) { x = res_max; } _set_res_u(x); }

      // res(): 1: success; 0: n > nmax(), so nmax() is set; -1: n is negative, nothing changed.
    void resize(_s_long n, char c = ' ') __bmdx_noex { if (n < 0) { _set_res_u(-1); return; } bool b = n > nmax(); if (b) { n = nmax(); } _s_long delta = n - length(); if (delta > 0) { _set_u(c, _x + length(), delta); } if (delta != 0) { _set_end_u(n); } _set_res_u(b ? 0 : 1); }

      // res(): 1.
    void clear() __bmdx_noex { resize(0); }


    char& operator[] (_s_long i) { return _x[i]; }
    const char& operator[] (_s_long i) const { return _x[i]; }

    bool operator == (const t_string& s) const __bmdx_noex    { const _s_long n = length(); if (n != s.length()) { return false; } for (_s_long i = 0; i < n; ++i) { if (_x[i] != s._x[i]) { return false; } } return true; }
    bool operator < (const t_string& s) const __bmdx_noex    { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() < s.n()); }
    bool operator <= (const t_string& s) const __bmdx_noex    { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() <= s.n()); }
    bool operator != (const t_string& s) const __bmdx_noex    { return !operator==(s); }
    bool operator > (const t_string& s) const __bmdx_noex    { return !operator<(s); }
    bool operator >= (const t_string& s) const __bmdx_noex    { return !operator<=(s); }

    template<_s_long n2> bool operator == (const flstr_t<n2>& s) const __bmdx_noex    { const _s_long n = length(); if (n != s.length()) { return false; } for (_s_long i = 0; i < n; ++i) { if (_x[i] != s._x[i]) { return false; } } return true; }
    template<_s_long n2> bool operator < (const flstr_t<n2>& s) const __bmdx_noex    { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() < s.n()); }
    template<_s_long n2> bool operator <= (const flstr_t<n2>& s) const __bmdx_noex    { _s_long nb = n(); if (nb > s.n()) { nb = s.n(); } int res = std::memcmp(_x, s._x, size_t(nb)); return res < 0 || (res == 0 && n() <= s.n()); }
    template<_s_long n2> bool operator != (const flstr_t<n2>& s) const __bmdx_noex    { return !operator==(s); }
    template<_s_long n2> bool operator > (const flstr_t<n2>& s) const __bmdx_noex    { return !operator<(s); }
    template<_s_long n2> bool operator >= (const flstr_t<n2>& s) const __bmdx_noex    { return !operator<=(s); }

    template<class T> t_string& operator << (const T& x) { *this += x; return *this; }


      // res(): 1: success; -1: length() == nmax() already.
    t_string& operator += (char c) __bmdx_noex { _s_long n = length(); if (n >= nmax()) { _set_res_u(-1); return *this; } _x[n] = c; ++n; _x[n] = 0; _nr = (1 << _sh_res) | n; return *this; }

    t_string& operator += (double x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (wchar_t x) __bmdx_noex { append(&x, 1); return *this; }

    t_string& operator += (signed short x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (signed int x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (signed long x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (signed long long x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (unsigned short x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (unsigned int x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (unsigned long x) __bmdx_noex { *this += t_string(x); return *this; }
    t_string& operator += (unsigned long long x) __bmdx_noex { *this += t_string(x); return *this; }

    t_string operator + (char c) const __bmdx_noex { t_string s2(*this); s2 += c; return s2; }

    t_string operator + (double x) const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (wchar_t x) const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }

    t_string operator + (signed short x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed int x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long long x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned short x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned int x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long long x)  const __bmdx_noex { t_string s2(*this); s2 += x; return s2; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already; -2: ps == 0.
    t_string& append (const char* ps, _s_ll n) __bmdx_noex { _set_res_u(_append_s(ps, n >= 0 ? n : -1)); return *this; }
    t_string& operator += (const char* ps) __bmdx_noex { _set_res_u(_append_s(ps, -1)); return *this; }
    t_string& operator += (const std::string& s) __bmdx_noex { _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    t_string& operator = (const char* ps) __bmdx_noex { resize(0); _set_res_u(_append_s(ps, -1)); return *this; }
    t_string& operator = (const std::string& s) __bmdx_noex { resize(0); _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    t_string operator + (const char* ps) const __bmdx_noex { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::string& s) const __bmdx_noex { t_string s2(*this); s2 += s; return s2; }

    t_string& append (const wchar_t* ps, _s_ll n) __bmdx_noex { _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); return *this; }
    t_string& operator += (const wchar_t* ps) __bmdx_noex {  _set_res_u(_append_wcs(ps, -1)); return *this; }
    t_string& operator += (const std::wstring& s) __bmdx_noex { _set_res_u(_append_wcs(s.c_str(), s.length())); return *this; }
    t_string& operator = (const wchar_t* ps) __bmdx_noex { resize(0); _set_res_u(_append_wcs(ps, -1)); return *this; }
    t_string& operator = (const std::wstring& s) __bmdx_noex { resize(0); _set_res_u(_append_wcs(s.c_str(), s.length())); return *this; }
    t_string operator + (const wchar_t* ps) const __bmdx_noex { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::wstring& s) const __bmdx_noex { t_string s2(*this); s2 += s; return s2; }

    template<_s_long nmax2> t_string& operator += (const flstr_t<nmax2>& s) __bmdx_noex { _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    template<_s_long nmax2> t_string operator + (const flstr_t<nmax2>& s) const __bmdx_noex { t_string s2(*this); s2 += s; return s2; }
    template<_s_long nmax2> t_string& operator = (const flstr_t<nmax2>& s) __bmdx_noex { resize(0); _set_res_u(_append_s(s.c_str(), s.length())); return *this; }

  private:
    _s_long _nr; char _x[nmax_c + 1];
    inline void _copy_u(const char* buf, char* dest, _s_long n) { while (n > 0) { *dest++ = *buf++; --n; } }
      // Copies <= nc chars., stops on null character (it's not copied).
      //  Returns num. of characters copied. Num. < nmax means "null character is reached".
    inline _s_long _copy_le_u(const char* buf, char* dest, _s_long nc) { _s_long n = 0; while (nc > 0) { char c = *buf++; if (!c) { break; } *dest++ = c; ++n; --nc; } return n; }
    inline void _set_u(char c, char* dest, _s_long n) { while (n > 0) { *dest++ = c; --n; } }
    inline void _set_end_u(_s_long n) { _x[n] = 0; _nr &= ~_s_long(_m_size); _nr |= n; }
    inline void _set_res_u(_s_long x) __bmdx_noex { _nr &= _m_size; _nr |= (x << _sh_res); }
      // 1: success, 0: partially copied, -1: string is full, cannot append, -2: invalid ps, nsrc.
    _s_long _append_s(const char* ps, _s_ll nsrc = -1) __bmdx_noex
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

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const char* ps, const flstr_t<nmax_>& s2) __bmdx_noex { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::string& s, const flstr_t<nmax_>& s2) __bmdx_noex { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const wchar_t* ps, const flstr_t<nmax_>& s2) __bmdx_noex { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::wstring& s, const flstr_t<nmax_>& s2) __bmdx_noex { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline std::ostream& operator << (std::ostream& stm, const flstr_t<nmax_>& s2) __bmdx_noex { stm << s2.str(); return stm; }

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
      //    -1 - invalid input parameter (buf == 0, nchars < 0).
    static inline _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed = true) __bmdx_noex { return _bmdx_str_impl<>::str_from_s_ll(x, buf, nchars, b_signed); }

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
      //    -1 - invalid input parameter (buf == 0, nchars < 0, x is not finite with b_nans == false).
    static inline _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax = 6, _s_long nfracmax = 12, bool b_nans = true) __bmdx_noex { return _bmdx_str_impl<>::str_from_double(x, buf, nchars, ndmmax, nfracmax, b_nans); }

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

      // Converts UTF-16 string to UTF-8 string.
      //  n < 0: autodetect x length based on null char.
    static inline std::string wsbs_utf8(const std::wstring& x) { return bmdx_str::conv::_bmdx_str_impl<>::_wsbs_utf8(x.c_str(), x.length()); }
    static inline std::string wsbs_utf8(const wchar_t* x, _s_ll n = -1 ) { return bmdx_str::conv::_bmdx_str_impl<>::_wsbs_utf8(x, n); }

      // Converts UTF-8 string to UTF-16 string.
      //  n < 0: autodetect x length based on null char.
    static inline std::wstring bsws_utf8(const std::string& x) { return bmdx_str::conv::_bmdx_str_impl<>::_bsws_utf8(x.c_str(), x.length()); }
    static inline std::wstring bsws_utf8(const char* x, _s_ll n = -1) { return bmdx_str::conv::_bmdx_str_impl<>::_bsws_utf8(x, n); }
  } }
}

namespace bmdx_minmax
{
  inline _s_ll myllmax(_s_ll a, _s_ll b) { return a > b ? a : b; }
  inline _s_ll myllmin(_s_ll a, _s_ll b) { return a < b ? a : b; }
  inline _s_ll myllmax(_s_ll a, _s_ll b, _s_ll c) { return myllmax(a, myllmax(b, c)); }
  inline _s_ll myllmin(_s_ll a, _s_ll b, _s_ll c) { return myllmin(a, myllmin(b, c)); }
  inline _s_ll myllmax(_s_ll a, _s_ll b, _s_ll c, _s_ll d) { return myllmax(myllmax(a, b), myllmax(c, d)); }
  inline _s_ll myllmin(_s_ll a, _s_ll b, _s_ll c, _s_ll d) { return myllmin(myllmin(a, b), myllmin(c, d)); }
  inline _s_ll myllrange_ub(_s_ll x, _s_ll a, _s_ll b) { if (x < a) { x = a; } return x > b ? b : x; } // b (max) has more priority than a
  inline _s_ll myllrange_lb(_s_ll x, _s_ll a, _s_ll b) { if (x > b) { x = b; } return x < a ? a : x; } // a (min) has more priority than b

  inline _s_long mylmax(_s_long a, _s_long b) { return a > b ? a : b; }
  inline _s_long mylmin(_s_long a, _s_long b) { return a < b ? a : b; }
  inline _s_long mylmax(_s_long a, _s_long b, _s_long c) { return mylmax(a, mylmax(b, c)); }
  inline _s_long mylmin(_s_long a, _s_long b, _s_long c) { return mylmin(a, mylmin(b, c)); }
  inline _s_long mylmax(_s_long a, _s_long b, _s_long c, _s_long d) { return mylmax(mylmax(a, b), mylmax(c, d)); }
  inline _s_long mylmin(_s_long a, _s_long b, _s_long c, _s_long d) { return mylmin(mylmin(a, b), mylmin(c, d)); }
  inline _s_long mylrange_ub(_s_long x, _s_long a, _s_long b) { if (x < a) { x = a; } return x > b ? b : x; } // b (max) has more priority than a
  inline _s_long mylrange_lb(_s_long x, _s_long a, _s_long b) { if (x > b) { x = b; } return x < a ? a : x; } // a (min) has more priority than b

  inline double myffmax(double a, double b) { return a > b ? a : b; }
  inline double myffmin(double a, double b) { return a < b ? a : b; }
  inline double myffmax(double a, double b, double c) { return myffmax(a, myffmax(b, c)); }
  inline double myffmin(double a, double b, double c) { return myffmin(a, myffmin(b, c)); }
  inline double myffmax(double a, double b, double c, double d) { return myffmax(myffmax(a, b), myffmax(c, d)); }
  inline double myffmin(double a, double b, double c, double d) { return myffmin(myffmin(a, b), myffmin(c, d)); }
  inline double myffrange_ub(double x, double a, double b) { if (x < a) { x = a; } return x > b ? b : x; } // b (max) has more priority than a
  inline double myffrange_lb(double x, double a, double b) { if (x > b) { x = b; } return x < a ? a : x; } // a (min) has more priority than b

  inline float myfmax(float a, float b) { return a > b ? a : b; }
  inline float myfmin(float a, float b) { return a < b ? a : b; }
  inline float myfmax(float a, float b, float c) { return myfmax(a, myfmax(b, c)); }
  inline float myfmin(float a, float b, float c) { return myfmin(a, myfmin(b, c)); }
  inline float myfmax(float a, float b, float c, float d) { return myfmax(myfmax(a, b), myfmax(c, d)); }
  inline float myfmin(float a, float b, float c, float d) { return myfmin(myfmin(a, b), myfmin(c, d)); }
  inline float myfrange_ub(float x, float a, float b) { if (x < a) { x = a; } return x > b ? b : x; } // b (max) has more priority than a
  inline float myfrange_lb(float x, float a, float b) { if (x > b) { x = b; } return x < a ? a : x; } // a (min) has more priority than b
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

  //== BASE PART FOR CROSS-MODULE CONTAINERS

    // operator= and copy constructor exception for array types with enabled exceptions. May be caught on the client.
  struct exc_carr_alloc_asg : std::exception { const char* what() const __bmdx_noex { return "c[pp]array_t/operator=|cc/alloc"; } };

    // Internal exceptions, do not reach the client.
  struct __exc_carr_asg : std::exception { const char* what() const __bmdx_noex { return "_carr_asgx_t::try_asg"; } };
  struct __exc_carr_cc : std::exception { const char* what() const __bmdx_noex { return "_carr_asgx_t:try_cc"; } };
  template<class T, bool no_exc_asg> struct carray_t;
  template<class T, bool no_exc_asg, class _bs> struct cpparray_t;
  template<class T, bool no_exc_asg> struct carray_r_t;
  template<class T> struct arrayref_t;
  template<class A, class Ax, class _ = __vecm_tu_selector> struct _carr_aux1_t { static inline void try_asg(A& dest, const A& src) { (Ax&)dest = (const Ax&)src; if (dest.n() != src.n()) { throw __exc_carr_asg(); } } static inline void try_cc(void* pdest, const A& src) { new (pdest) Ax((const Ax&)src); if (((A*)pdest)->n() != src.n()) { throw __exc_carr_cc(); } } };
    // _carr_asgx_t provides assignment and placement copy construction (try_asg, try_cc)
    //    with forced exceptions on error for all carray_t and cpparray_t variants.
    //    (Easier error processing for deeply nested structures.)
  template<class A, class Aux = bmdx_meta::nothing, class _ = __vecm_tu_selector> struct _carr_asgx_t { enum { is_carr_any = 0, is_cpparray = 0 }; typedef A t_ax; static inline void try_asg(A& dest, const A& src) { dest = src; } static inline void try_cc(void* pdest, const A& src) { new (pdest) A(src); } };
  template<class T, class _> struct _carr_asgx_t<carray_t<T, true>, bmdx_meta::nothing, _> : _carr_aux1_t<carray_t<T, true>, carray_t<T, false>, _> { enum { is_carr_any = 1, is_cpparray = 0 }; typedef carray_t<T, false> t_ax; static inline void check_exc_alloc() { } };
  template<class T, class _, class _bs> struct _carr_asgx_t<cpparray_t<T, true, _bs>, bmdx_meta::nothing, _> : _carr_aux1_t<cpparray_t<T, true, _bs>, cpparray_t<T, false, _bs>, _> { enum { is_carr_any = 1, is_cpparray = 1 }; typedef cpparray_t<T, false, _bs> t_ax; static inline void check_exc_alloc() { } };
  template<class T, class _> struct _carr_asgx_t<carray_t<T, false>, bmdx_meta::nothing, _> : _carr_asgx_t<carray_t<T, false>, int, _> { enum { is_carr_any = 1, is_cpparray = 0 }; static inline void check_exc_alloc() { throw exc_carr_alloc_asg(); } };
  template<class T, class _, class _bs> struct _carr_asgx_t<cpparray_t<T, false, _bs>, bmdx_meta::nothing, _> : _carr_asgx_t<cpparray_t<T, false, _bs>, int, _> { enum { is_carr_any = 1, is_cpparray = 1 }; static inline void check_exc_alloc() { throw exc_carr_alloc_asg(); } };

  template<class T, class _ = __vecm_tu_selector>
  struct _carray_tu_alloc_t
  {
      #ifdef calloc
        static inline void* _s_psf1() __bmdx_noex { return 0; }
      #else
        static inline void* _s_psf1() __bmdx_noex { return (void*)&std::calloc; }
      #endif
      static void* _sf_calloc(_s_ll nb) __bmdx_noex { if (nb < 0) { return 0; } size_t n2 = size_t(nb); if (_s_ll(n2) != nb) { return 0; } try { return std::calloc(n2, 1); } catch (...) { return 0; } }
      static void _sf_free(void* p) __bmdx_noex { try { std::free(p); } catch (...) {} }
      static void _sf_destroy1(T* p) __bmdx_noex { if (p) { try { p->~T(); } catch (...) {} } }
      static void _sf_delete1(T* p) __bmdx_noex { if (p) { try { delete p; } catch (...) {} } }
    static void* _spff[];
  };
  template<class T, class _> void* _carray_tu_alloc_t<T, _>::_spff[5] = { (void*)(3 + __bmdx_null_pchar), (void*)&_sf_calloc, (void*)&_sf_free, (void*)&_sf_destroy1, (void*)&_sf_delete1 };

  template<class T>
  struct _carray_base_t
  {
    typedef T t_value;
    typedef size_t _t_size; // _t_size: private; client uses s_ll as index as size type

    _carray_base_t() __bmdx_noex : _n(0), _data(0), _psf1(_carray_tu_alloc_t<T>::_s_psf1()) { __pad1 = 0; _pff = _carray_tu_alloc_t<T>::_spff; }

    inline _s_ll n() const __bmdx_noex    { return _n; }

      // NOTE Normally, pd() == 0 if n() == 0. See also _set_n_u.
    inline const t_value* pd() const __bmdx_noex { return _data; }
    inline t_value* pd() __bmdx_noex { return _data; }

    inline const t_value& operator[] (_s_ll i) const __bmdx_noex { return _data[_t_size(i)]; }
    inline t_value& operator[] (_s_ll i) __bmdx_noex { return _data[_t_size(i)]; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) __bmdx_noex
    {
      _t_size i0 = 0; _t_size nf(0); while (i0 < _t_size(_n)) { try { for (; i0 < _t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == _t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) __bmdx_noex
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      _t_size _i0 = _t_size(i0); const _t_size _i2 = _t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const _t_size nset = _i2 - _i0;
      _t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

      // Reallocate storage. Copy, destroy, initialize elements as specified.
      //  n2 >= 0: the new number of elements of type T.
      //  dmode: 1 - use local destructor, 0 - do not call destructor.
      //  imode: 4 - copy existing elems. as plain memory (bytes), 1 - copy with local T(const T&), 0 - no copying (new allocation only + initialize all bytes to 0).
      //  px:
      //    != 0 - if elements are added, initialize them as copy *px (method depends on imode).
      //    == 0 with imode == 0, 4 - initialize all bytes to 0.
      //    == 0 with imode == 1 - initialize as T().
      // Returns: true - success, false - failure, no changes.
      // NOTE realloc() proceeds with realloc/copy/init even if n2 == n().
    bool realloc(_s_ll n2, int dmode, int imode, const t_value* px) __bmdx_noex
    {
      if (!(n2 >= 0 && dmode == (dmode & 1) && (imode == 4 || imode == (imode & 1)))) { return false; }
      if (n2 > this->_n) { _s_ll q = n2 * _s_ll(sizeof(T)); if (q <= 0) { return false; } if (q / _s_ll(sizeof(T)) != n2) { return false; } if (!(_t_size(q) > 0 && _s_ll(_t_size(q)) == q)) { return false; } }
      T* pd2 = 0;
      if (n2 > 0)
      {
        pd2 = (T*)_alloc(n2 * sizeof(T)); if (!pd2) { return false; }
        _t_size n0 = _t_size(n2 < _n ? n2 : _n); _t_size i0 = 0;
        if (imode == 1)
        {
          try { for (; i0 < n0; ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, _data[i0]); } } catch (...) {}
            if (i0 < n0) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          if (n2 > _n)
          {
            if (px) { try { for (; i0 < _t_size(n2); ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, *px); } } catch (...) {} }
              else { try { for (; i0 < _t_size(n2); ++i0) { new (pd2 + i0) T(); } } catch (...) {} }
            if (i0 < _t_size(n2)) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          }
        }
        else if (imode == 4)
          { if (n0) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2, _data, n0 * sizeof(T)); } if (px && n2 > _n) { for (i0 = n0; i0 < _t_size(n2); ++i0) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2 + i0, px, sizeof(T)); } } }
      }
      if (_n > 0) { if (dmode == 1) { _destroy(_data, 0, _t_size(_n)); } _free(_data); }
      _data = pd2; _n = n2;
      return true;
    }
      // Same as realloc, only uses x to initialize array elements. t_value may have no default constructor.
    bool realloc_cp(_s_ll n2, int dmode, int imode, const t_value& x) __bmdx_noex
    {
      if (!(n2 >= 0 && dmode == (dmode & 1) && (imode == 4 || imode == (imode & 1)))) { return false; }
      if (n2 > this->_n) { _s_ll q = n2 * _s_ll(sizeof(T)); if (q <= 0) { return false; } if (q / _s_ll(sizeof(T)) != n2) { return false; } if (!(_t_size(q) > 0 && _s_ll(_t_size(q)) == q)) { return false; } }
      T* pd2 = 0;
      if (n2 > 0)
      {
        pd2 = (T*)_alloc(n2 * sizeof(T)); if (!pd2) { return false; }
        _t_size n0 = _t_size(n2 < _n ? n2 : _n); _t_size i0 = 0;
        if (imode == 1)
        {
          try { for (; i0 < n0; ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, _data[i0]); } } catch (...) {}
            if (i0 < n0) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          if (n2 > _n)
          {
            try { for (; i0 < _t_size(n2); ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, x); } } catch (...) {}
            if (i0 < _t_size(n2)) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          }
        }
        else if (imode == 4)
          { if (n0) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2, _data, n0 * sizeof(T)); } if (n2 > _n) { for (i0 = n0; i0 < _t_size(n2); ++i0) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2 + i0, &x, sizeof(T)); } } }
      }
      if (_n > 0) { if (dmode == 1) { _destroy(_data, 0, _t_size(_n)); } _free(_data); }
      _data = pd2; _n = n2;
      return true;
    }
      // Same as realloc, but only for clearing the container.
    bool realloc_0(int dmode) __bmdx_noex
    {
      if (!(dmode == (dmode & 1))) { return false; }
      if (_n > 0) { if (dmode == 1) { _destroy(_data, 0, _t_size(_n)); } _free(_data); }
      _data = 0; _n = 0;
      return true;
    }

      // Element-by-element equality check.
    bool is_eq(const _carray_base_t<t_value>& x) const { if (this == &x) { return true; } if (this->_n != x._n) { return false; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }
    template<class Iter> bool is_eq(Iter x, _s_ll n) const { if (this->_n != n) { return false; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == *x++)) { return false; } } return true; }
    bool is_eq_z(const t_value* px, const t_value& z = t_value()) const { if (!px) { return this->_n == 0; } for (_s_ll i = 0; i < this->_n; ++i) { if (*px == z) { return false; } if (!(*px == this->_data[i])) { return false; } ++px; } return *px == z; }

      // Sets _n field (number of elements), without memory reallocation or elements creation/destruction.
      //  May be used carefully to reserve/unreserve place.
    void _set_n_u(_s_ll n) { _n = n >= 0 ? n : 0; }

  protected:
    _s_ll _n; T* _data; void* _psf1; union { void** _pff; _s_ll __pad1; };
    ~_carray_base_t() __bmdx_noex { if (_data) { _free(_data); _data = 0; } _n = 0; }

    typedef void* (*F_alloc)(_s_ll nbytes); typedef void (*F_free)(void* p);
    inline void* _alloc(_s_ll nbytes) __bmdx_noex { if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { return _carray_tu_alloc_t<T>::_sf_calloc(nbytes); } return ((F_alloc)_pff[1])(nbytes); }
    inline void _free(void* p) __bmdx_noex { if (!p) { return; } if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { _carray_tu_alloc_t<T>::_sf_free(p); return; } ((F_free)_pff[2])(p); }

    inline void _destroy(T* pd_, _t_size i0, _t_size i2) __bmdx_noex { (void)pd_; while (i0 < i2) { try { for (; i0 < i2; ++i0) { (pd_ + i0)->~T(); } } catch (...) { ++i0; } } }
    inline void _destroy1(T* pd_) __bmdx_noex { try { pd_->~T(); } catch (...) {} }

  private:
    _carray_base_t(const _carray_base_t&); _carray_base_t& operator=(const _carray_base_t&);
  };







  //== struct carray_t

    // C-style array wrapper. For T allowing bytewise copying and not needing constructor/destructor calls.
    // Behavior:
    //    1. Any elements copying copies just bytes.
    //      In the default mode (no_exc_asg == true), no exceptions are generated on allocation failure:
    //        a) assignment returns false, without any changes to target;
    //        b) copy ctor. creates an empty array.
    //
    //    2. resize(n) preserves existing elements.
    //      If size grows, new elements ARE initialized by writing 0 to each byte.
    //      For pure storage reallocation without copying existing elements (top speed):
    //        array.realloc(n, 0, 0, 0);
    //      For storage growth with new elements initialization:
    //        a) T src; array.realloc(n, 0, 4, &src);
    //        b) array.realloc(n, 0, 1, 0); // using T() for new elements.
    //
    //    3. On carray_t destruction, individual elements destructors ARE NOT called.
    //
    //    4. carray_t, created in binary module A, may be passed as bytes or as other container's element,
    //        manipulated and destroyed locally in binary module B.
    //      a) WinAPI: any A, B / POSIX: A, B using standard malloc() family:
    //        carray_t destruction in B is safe even after A being unloaded.
    //      b) POSIX: A or B using custom malloc() family: carray_t destruction in B is safe only before unloading A.
    //
  template<class T, bool no_exc_asg = true>
  struct carray_t : _carray_base_t<T>
  {
    typedef T t_value;
    typedef carray_t t_a;
    typedef typename _carray_base_t<T>::_t_size _t_size; // _t_size: private; client uses s_ll as index as size type

    carray_t() __bmdx_noex {}
    carray_t(const carray_t& x) : _carray_base_t<T>() { if (!this->realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return; } if (x.n()) { bmdx_str::words::memmove_t<T>::sf_memcpy(this->_data, x._data, _t_size(x.n()) * sizeof(T)); } }
    ~carray_t() __bmdx_noex {}

      // If this != &src: clears this, moves src to this, sets src to be empty.
    void move(carray_t& src) __bmdx_noex { if (this == &src) { return; } this->~carray_t(); bmdx_str::words::memmove_t<T>::sf_memcpy(this, &src, sizeof(carray_t)); src._data = 0; src._n = 0; }
    void swap(carray_t& src) __bmdx_noex { if (this == &src) { return; } bmdx_str::words::swap_bytes(this, &src); }
    bool resize(_s_ll n) __bmdx_noex { if (n == this->_n) { return true; } return this->realloc(n, 0, 4, 0); }
    void clear() __bmdx_noex { this->realloc(0, 0, 4, 0); }
    bool operator=(const carray_t& x)
      { if (this == &x) { return true; } carray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; } if (x.n()) { bmdx_str::words::memmove_t<T>::sf_memcpy(x2._data, x._data, _t_size(x.n()) * sizeof(T)); } this->move(x2); return true; }

    template<bool _ne> inline bool operator==(const carray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne, class _bs> inline bool operator==(const cpparray_t<t_value, _ne, _bs>& x) const { return this->is_eq(x); }
    template<bool _ne, class _bs> inline bool operator!=(const cpparray_t<t_value, _ne, _bs>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }
  };



  //== struct cpparray_t

    // C++-style array wrapper. For T with non-trivial construction, copying and destructor calls.
    // Behavior:
    //    1. Any elements copying is via T(const T&).
    //      In the default mode (no_exc_asg == true), no exceptions are generated on allocation or copying failure:
    //        a) assignment returns false, without any changes to target;
    //        b) copy ctor. creates an empty array.
    //
    //    2. resize(n) preserves elements.
    //      If size grows, new elements ARE initialized as T().
    //
    //    3. On cpparray_t destruction, individual elements destructors ARE called.
    //
    //    4. cpparray_t, created in binary module A, may be passed as bytes or as container element,
    //        manipulated and destroyed locally in binary module B if all the following conditions are true:
    //      1) T(), T(const T&), ~T() in B behave exactly as in A, i.e. have purely module-independent data, like flstr_t,
    //        or are aware of the current binary module, like carray_t, cpparray_t, cref_t, threadctl etc.
    //        NOTE STL's and other objects with dynamic data, like string, vector etc. can safely be elements of cpparray_t
    //        ONLY in the binary module where they are constructed.
    //      2) cpparray_t destruction in B is safe only before unloading A.
    //
  template<class T, bool no_exc_asg = true, class _bs = bmdx_meta::nothing>
  struct cpparray_t : _carray_base_t<T>
  {
    typedef T t_value;
    typedef cpparray_t t_a;
    typedef typename _carray_base_t<T>::_t_size _t_size; // _t_size: private; client uses s_ll as index as size type

    cpparray_t() __bmdx_noex {}
    cpparray_t(const cpparray_t& x) : _carray_base_t<T>() { *this = x; }
    ~cpparray_t() __bmdx_noex { this->realloc_0(1); }

      // If this != &src: clears this, moves src to this, sets src to be empty.
    void move(cpparray_t& src) __bmdx_noex { if (this == &src) { return; } this->~cpparray_t(); bmdx_str::words::memmove_t<T>::sf_memcpy(this, &src, sizeof(t_a)); src._data = 0; src._n = 0; }
    void swap(cpparray_t& src) __bmdx_noex { if (this == &src) { return; } bmdx_str::words::swap_bytes(this, &src); }
    bool resize(_s_ll n) __bmdx_noex { if (n == this->_n) { return true; } return this->realloc(n, 1, 1, 0); }
    void clear() __bmdx_noex { this->realloc_0(1); }
    bool operator=(const cpparray_t& x)
    {
      if (this == &x) { return true; } cpparray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      T* pd2 = x2.pd(); _t_size i0 = 0; _t_size n0 = _t_size(x2.n());
      try { for (; i0 < n0; ++i0) { new (pd2 + i0) T(x[i0]); } } catch (...) {}
      if (i0 < n0) { this->_destroy(pd2, 0, i0); x2.realloc_0(0); _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      this->move(x2);
      return true;
    }

    template<bool _ne> inline bool operator==(const carray_t<t_value, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<t_value, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne, class _bs2> inline bool operator==(const cpparray_t<t_value, _ne, _bs2>& x) const { return this->is_eq(x); }
    template<bool _ne, class _bs2> inline bool operator!=(const cpparray_t<t_value, _ne, _bs2>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }
  };



  //== struct carray_r_t

    // Simple string class.
    //  Difference from std::string:
    //    1. Default constructor does not generate exceptions in principle.
    //    2. The object may be used in other binary module (not where it was created).
    //    3. To create C-string, use ensure_cstr(): this reserves 1 additional elem. and sets it to T().
    //      (Container size (n()) is not changed.)
    //      The additional elem. remains valid until the next modification of the container.
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


    _s_ll nrsv() const __bmdx_noex { return _nrsv; }
    _s_ll n() const __bmdx_noex { return this->_n; }

      // NOTE Normally, pd() == 0 if n() == 0.
    const t_value* pd() const __bmdx_noex { return this->_data; }
    t_value* pd() __bmdx_noex { return this->_data; }

    const t_value& operator[] (_s_ll i) const __bmdx_noex { return this->_data[_t_size(i)]; }
    t_value& operator[] (_s_ll i) __bmdx_noex { return this->_data[_t_size(i)]; }


    carray_r_t() __bmdx_noex : _nrsv(0) {}
      // NOTE on copying, new nrsv() == x.n().
    carray_r_t(const t_a& x) : carray_t<T, no_exc_asg>(), _nrsv(0) { if (!this->resize(x.n(), false, true)) { _carr_asgx_t<t_a0>::check_exc_alloc(); return; } if (x.n()) { bmdx_str::words::memmove_t<T>::sf_memcpy(this->_data, x._data, _t_size(x.n()) * sizeof(T)); } }
    carray_r_t(_s_ll n, const t_value& x) : _nrsv(0) { if (!append_nx(n, x)) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    carray_r_t(const t_value* px, _s_ll n) : _nrsv(0) { if (!append_n(px, n)) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    explicit carray_r_t(const arrayref_t<t_value>& x) : _nrsv(0) { if (!append_n(x.pd(), x.n())) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    template<class Tr, class A> carray_r_t(const std::basic_string<t_value, Tr, A>& x) : _nrsv(0) { if (x.size() > 0 && !append_n(x.c_str(), x.size())) { _carr_asgx_t<t_a0>::check_exc_alloc(); } }
    ~carray_r_t() __bmdx_noex {}


    void move(t_a& src) __bmdx_noex { if (this == &src) { return; } this->~carray_r_t(); bmdx_str::words::memmove_t<T>::sf_memcpy(this, &src, sizeof(t_a)); src._data = 0; src._n = 0; src._nrsv = 0; }
    void swap(t_a& src) __bmdx_noex { if (this == &src) { return; } bmdx_str::words::swap_bytes(*this, src); }
    void clear() __bmdx_noex { this->resize(0, false, true); } // deallocate the array (nrsv() becomes 0)
      // NOTE On assignment, if old reserve is enough to hold values, no memory reallocation occurs.
      //    Otherwise, new nrsv() == x.n(). Exact behavior: see also resize() with b_minimize_rsv true.
    bool operator=(const carray_r_t& x) { if (this == &x) { return true; } if (!this->resize(x.n(), false, true)) { _carr_asgx_t<t_a0>::check_exc_alloc(); return false; } if (x.n()) { bmdx_str::words::memmove_t<T>::sf_memcpy(this->_data, x._data, _t_size(x.n()) * sizeof(T)); } return true; }

    std::basic_string<t_value> str() const { if (this->_data) { return std::basic_string<T>(this->_data, _t_size(bmdx_minmax::myllmax(0, this->_n))); } return std::basic_string<T>(); }

      // Resize to n2.
      //  b_copy (acts only if the array is reallocated):
      //    true: copy (or preserve) min(n2, n()) elements from the old array,
      //    false: ensure min(n2, n()) elements to be zero-filled.
      //  b_minimize_rsv true:
      //    - if n2 == 0, the array is completely deallocated, and carray_r_t occupies totally only sizeof(carray_r_t) bytes of memory.
      //    - if n2 < min(n(), nrsv() / 2), new nrsv = n2. (But if reallocation fails, the reserve is not shrunk, and the function returns true (success).)
      //    - if n2 > nrsv(), new nrsv = n2.
      //    - otherwise (i.e. n2 == [0.5..1]*nrsv() and >= n()), no memory reallocation occurs.
      //    This mode should be used to minimize both memory overhead and reallocations,
      //      esp. when the array size is set once and increased rarely or never.
      //  b_minimize_rsv false:
      //    - if n2 > nrsv(), new nrsv = 2 * n2 (similar to std::vector).
      //    - if n2 <= nrsv(), no memory reallocation or deallocation occurs, even if n2 == 0.
      //    This mode should be used when the array size changes frequently and unpredictably.
      //    For finer control over the reserve, consider using reserve() together with resize().
      //  px
      //    != 0: if resizing to larger size, write *px to the newly added positions.
      //    == 0: keep the added or created positions zero-initialized:
      //      a) If reallocation occurs due to reserve change,
      //        1) whole the new array is filled with zero bytes,
      //        2) b_copy true copies elements from the prev. array.
      //      b) If the array grows for the account of reserve, w/o reallocation,
      //        the existing values are kept on b_copy == true, otherwise zeroed,
      //        and the new positions are anyway zeroed.
      // NOTE If new size is within the current reserve, the function always succeeds.
    bool resize(_s_ll n2, bool b_copy, bool b_minimize_rsv, const t_value* px = 0) __bmdx_noex
    {
      if (n2 < 0) { return false; }
      if (n2 > this->_n) { _s_ll q = n2 * _s_ll(sizeof(T)); if (q <= 0) { return false; } if (q / _s_ll(sizeof(T)) != n2) { return false; } if (!(_t_size(q) > 0 && _s_ll(_t_size(q)) == q)) { return false; } }
      T* pd2 = this->_data; _s_ll nrsv2 = _nrsv;
      if (n2 == 0 && b_minimize_rsv) { pd2 = 0; nrsv2 = 0; }
        else if (n2 > _nrsv || (b_minimize_rsv && n2 < _nrsv / 2 && n2 < this->_n)) { nrsv2 = n2; if (n2 > _nrsv && !b_minimize_rsv) { nrsv2 = 2 * n2; } pd2 = (T*)this->_alloc(nrsv2 * sizeof(T)); if (!pd2) { if (n2 > _nrsv) { return false; } nrsv2 = _nrsv; pd2 = this->_data; } }
      const _s_ll ncp = n2 < this->_n ? n2 : this->_n;
      if (pd2 != this->_data) // reallocation done, all is 0 now
      {
        if (b_copy && ncp) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2, this->_data, _t_size(ncp) * sizeof(T)); }
        if (this->_data) { this->_free(this->_data); }
        this->_data = pd2; _nrsv = nrsv2;
      }
      else // no reallocation
      {
        if (pd2 && !b_copy) { std::memset(pd2, 0, _t_size(ncp * sizeof(T))); } // erase existing values if !b_copy
      }
      if (n2 > this->_n) // init. new positions
      {
        if (px) { for (_s_ll i0 = this->_n; i0 < n2; ++i0) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2 + _t_size(i0), px, sizeof(T)); } }
          else { std::memset(pd2 + this->_n, 0, _t_size(n2  - this->_n) * sizeof(T)); }
      }
      this->_n = n2;
      return true;
    }

      // The function sets new number of reserved elements to max(n(), nrsv2).
      //    If this requires array reallocation, the existing elements are copied into the new array.
      //    (New elements are filled with zero bytes.)
      // NOTE If new reserve is within the current reserve,
      //    the function returns true (success) even if failed to shrink the storage.
    bool reserve(_s_ll nrsv2) __bmdx_noex
    {
      if (nrsv2 < 0) { return false; }
      if (nrsv2 == _nrsv) { return true; }
      if (nrsv2 > this->_nrsv) { _s_ll q = nrsv2 * _s_ll(sizeof(T)); if (q <= 0) { return false; } if (q / _s_ll(sizeof(T)) != nrsv2) { return false; } if (!(_t_size(q) > 0 && _s_ll(_t_size(q)) == q)) { return false; } }
      if (nrsv2 < this->_n) { nrsv2 = this->_n; }
      T* pd2 = this->_data;
      if (nrsv2 == 0) { pd2 = 0; }
        else { pd2 = (T*)this->_alloc(nrsv2 * sizeof(T)); if (!pd2) { if (nrsv2 > _nrsv) { return false; } nrsv2 = _nrsv; pd2 = this->_data; } }
      const _t_size ncp = _t_size(this->_n);
      if (ncp > 0 && pd2 != this->_data) { bmdx_str::words::memmove_t<T>::sf_memcpy(pd2, this->_data, ncp * sizeof(T)); }
      if (pd2 != this->_data) { if (this->_data) { this->_free(this->_data); } this->_data = pd2; _nrsv = nrsv2; }
      return true;
    }

      // Ensure reserving n() + 1 element. Set element at n()+1 to T().
    bool ensure_cstr() __bmdx_noex { if (nrsv() <= n()) { if (!reserve(n() + 1)) { return false; } } pd()[n()] = T(); return true; }

      // Appending, similar to basic_string operator +=.
      //  Keeps reserved space >= n() + new elements + 1.
      //  The after-end element is kept = bytewise 0.
      //  const t_value* px arg.:
      //    a) if taken without additional args, it is interpreted as a C-string (null-terminated).
      //    b) if taken with arg. z, it is interpreted as a z-terminated.
      //    c) if taken with arg. n, it is interpreted as string of n values.
      // If n() + new elements + 1 is within the current reserve, the functions always succeed.
    bool append_1(const t_value& x) __bmdx_noex
    {
      const _s_ll nr = this->_n + 2;
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n; *p = x; this->_n += 1; p += 1; *p = T();
      return true;
    }
      // append_n: appends *px++ max(0, n) times.
    bool append_n(const t_value* px, _s_ll n) __bmdx_noex
    {
      if (n > 0 && !px) { return false; }
      if (n < 0) { n = 0; }
      const _s_ll nr = this->_n + n + 1;
      if (nr < 0) { return false; }
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n;
      if (n > 0) { bmdx_str::words::memmove_t<T>::sf_memcpy(p, px, _t_size(n) * sizeof(T)); }
      this->_n += n; p += n; *p = T();
      return true;
    }
      // append_nx: appends x max(0, n) times.
    bool append_nx(_s_ll n, const t_value& x) __bmdx_noex
    {
      if (n < 0) { n = 0; }
      const _s_ll nr = this->_n + n + 1;
      if (nr < 0) { return false; }
      if (nr > _nrsv && !reserve(2 * nr)) { return false; }
      T* p = this->_data + this->_n;
      T* p2 = p + n;
      if (sizeof(T) <= 8) { _s_ll z = 0; *(T*)&z = x; if (z == 0) { std::memset(p, 0, _t_size(n) * sizeof(T)); *p2 = T(); this->_n += n; return true; } }
      while (p < p2) { *p++ = x; } *p2 = T(); this->_n += n;
      return true;
    }
      // append_z: appends px[i] until meets z. (For t_value == char, z == 0 this is equivalent to appending C string.)
    bool append_z(const t_value* px, const t_value& z = t_value()) __bmdx_noex
    {
      _s_ll n = 0;
      if (px) { const t_value* px2 = px; while (*px2 != z) { ++px2; } n = _s_ll(px2 - px); }
      return append_n(px, n);
    }
    bool append(const arrayref_t<t_value>& x) __bmdx_noex { return append_n(x.pd(), x.n()); }


    struct exc_carray_r_oppluseq : std::exception { const char* what() const __bmdx_noex { return "carray_r_t:op+="; } };

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
    template<bool _ne, class _bs> inline bool operator==(const cpparray_t<t_value, _ne, _bs>& x) const { return this->is_eq(x); }
    template<bool _ne, class _bs> inline bool operator!=(const cpparray_t<t_value, _ne, _bs>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const carray_r_t<t_value, _ne>& x) const { return this->is_eq(x._base()); }
    template<bool _ne> inline bool operator!=(const carray_r_t<t_value, _ne>& x) const { return !this->is_eq(x._base()); }
    template<class Tr, class A> bool operator==(const std::basic_string<t_value, Tr, A>& x) const { return this->is_eq(x.begin(), x.size()); }
    template<class Tr, class A> bool operator!=(const std::basic_string<t_value, Tr, A>& x) const { return !this->is_eq(x.begin(), x.size()); }
    bool operator==(const t_value* px) const { return this->is_eq_z(px, t_value()); }
    bool operator!=(const t_value* px) const { return !this->is_eq_z(px, t_value()); }


    const carray_t<t_value, no_exc_asg>& _base() const { return *this; }
  private:
    _s_ll _nrsv;
  };



    // This may be specialized to enable arrayref_t construction from arbitrary storage with contiguous elements.
  template<class Q, class _ = __vecm_tu_selector>
  struct _arrayref_adapter_t
    { static void* Fp(const Q& x) { if (sizeof(x)) {} return 0; } static _s_ll Fn(const Q& x) { if (sizeof(x)) {} return 0; } };




  //== struct arrayref_t

    // Simplistic weak reference to an array of elements in memory.
    //    Contains a pointer to the first element, and the number of elements.
    // The object may be automatically constructed
    //    from std strings, C strings, BMDX arrays (carray_t, cpparray_t, carray_r_t).
  template<class T>
  struct arrayref_t
  {
    typedef T t_value;
    typedef size_t t_size;
    typedef arrayref_t t_a;

    inline _s_ll n() const __bmdx_noex    { return _n; }
    inline _s_ll size() const __bmdx_noex    { return _n; }
    inline _s_ll length() const __bmdx_noex    { return _n; }

    inline const t_value* pd() const __bmdx_noex { return _data; }
    inline t_value* pd() __bmdx_noex { return _data; }

    inline const t_value& operator[] (_s_ll i) const __bmdx_noex { return _data[t_size(i)]; }
    inline t_value& operator[] (_s_ll i) __bmdx_noex { return _data[t_size(i)]; }

          // Same as operator[].
        inline const t_value& opsub (_s_ll i) const __bmdx_noex { return _data[t_size(i)]; }
        inline t_value& opsub (_s_ll i) __bmdx_noex { return _data[t_size(i)]; }


      // True if *this correctly references a) an empty string, b) non-empty string.
    inline bool is_valid() const __bmdx_noex { return (_data && _n >= 0) || (!_data && _n == 0); }

      // True if *this is not valid, or references an empty string.
    inline bool is_empty() const __bmdx_noex { return !_data || _n <= 0; }

      // True is *this and x are equal element-by-element.
    bool is_eq(const arrayref_t<t_value>& x)            const { if (this->_n != x._n) { return false; } if (this->_data == x._data) { return true; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }

      // True is *this and x point to same memory location, with same number of elements (n()).
    bool is_eq_pn(const arrayref_t<t_value>& x)        const { return x.pd() == this->pd() && x.n() == this->n(); }

      // NOTE operator==, operator!= do comparison by value. See also is_eq, is_eq_pn.
    inline bool operator==(const arrayref_t<t_value>& x) const { return this->is_eq(x); }
    inline bool operator!=(const arrayref_t<t_value>& x) const { return !this->is_eq(x); }


    arrayref_t() __bmdx_noex { __pad1 = 0; unlink(); }
    ~arrayref_t() __bmdx_noex { unlink(); }

    bool link(const t_value* px, _s_ll n) __bmdx_noex { _n = n; if (_n < 0 || (_n > 0 && !px)) { _n = 0; _data = 0; return false; } _data = const_cast<T*>(px); return true; }
    void _link_u(const t_value* px, _s_ll n) __bmdx_noex { _n = n; _data = const_cast<T*>(px); }
    void unlink() __bmdx_noex { _n = 0;_data = 0; }
    void clear() __bmdx_noex { unlink(); }

    arrayref_t(const t_value* px) __bmdx_noex { __pad1 = 0; link(px, 0); if (px) { while (*px++ != T()) { ++_n; } } }
    arrayref_t(const t_value* px, _s_ll n) __bmdx_noex { __pad1 = 0; link(px, n); }
    arrayref_t(const arrayref_t& x) __bmdx_noex : _n(x._n) { __pad1 = 0; _data = x._data; }
    template<bool _ne> arrayref_t(const carray_t<t_value, _ne>& x) __bmdx_noex : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    template<bool _ne, class _bs> arrayref_t(const cpparray_t<t_value, _ne, _bs>& x) __bmdx_noex : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    template<bool _ne> arrayref_t(const carray_r_t<t_value, _ne>& x) __bmdx_noex : _n(x.n()) { __pad1 = 0; _data = const_cast<T*>(x.pd()); }
    template<class Tr, class A> arrayref_t(const std::basic_string<t_value, Tr, A>& x) __bmdx_noex { __pad1 = 0; *this = x; }
    template<class Q> explicit arrayref_t(const Q& x __bmdx_noarg) { __pad1 = 0; _data = (T*)_arrayref_adapter_t<T>::Fp(x); _n = _arrayref_adapter_t<T>::Fn(x); }

    arrayref_t& operator=(const arrayref_t& x) __bmdx_noex { _data = x._data; _n = x._n; return *this; }
    template<bool _ne> arrayref_t& operator=(const carray_t<t_value, _ne>& x) __bmdx_noex { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    template<bool _ne, class _bs> arrayref_t& operator=(const cpparray_t<t_value, _ne, _bs>& x) __bmdx_noex { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    template<bool _ne> arrayref_t& operator=(const carray_r_t<t_value, _ne>& x) __bmdx_noex { _data = const_cast<T*>(x.pd()); _n = x.n(); return *this; }
    template<class Tr, class A> arrayref_t& operator=(const std::basic_string<t_value, Tr, A>& x) __bmdx_noex { _n = _s_ll(x.size()); if (_n <= 0) { _n = 0; } _data = const_cast<T*>(&x[0]); return *this; }

      // Returns:
      //  a) 0-based position of x in *this,
      //    with search starting from (i0 in [0..n()-1] ? i0 : n())
      //    and until, not including, (i2 in [0..n()-1] ? i2 : n()).
      //  b) If not found or *this is not valid, returns n().
    _s_ll find1(const t_value& x, _s_ll i0 = 0, _s_ll i2 = -1) { if (!is_valid()) { return _n; } _yk_reg _s_ll i = i0 >= 0 && i2 < _n ? i0 : _n; _yk_reg _s_ll iend = i2 >= 0 && i2 < _n ? i2 : _n; while (i < iend) { if (_data[i] == x) { return i; } ++i; } return _n; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) __bmdx_noex
    {
      t_size i0 = 0; t_size nf(0); while (i0 < t_size(_n)) { try { for (; i0 < t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) __bmdx_noex
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      t_size _i0 = t_size(i0); const t_size _i2 = t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const t_size nset = _i2 - _i0;
      t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

    inline void swap(arrayref_t& src) __bmdx_noex { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; bmdx_str::words::memmove_t<t_value>::sf_memcpy(x, &src, sizeof(t_a)); bmdx_str::words::memmove_t<t_value>::sf_memcpy(&src, this, sizeof(t_a)); bmdx_str::words::memmove_t<t_value>::sf_memcpy(this, x, sizeof(t_a)); }

  private:
    _s_ll _n; union { T* _data; _s_ll __pad1; };
  };



  //== struct cringbuf1_t

    // Single-channel ringbuffer (fixed-size queue).
    //  Data supplier and consumer may run as pair of threads and use it concurrently, without any blocking.
    //  Data values are treated as plain C data (like in carray_t).
    //  The container allows for efficient random access to existing elements (O(1)).
    //  push*() and pop*() do NOT move the existing elements.
    //  The container is suitable for passing across binary modules of one process,
    //    under the same conditions as described for struct carray_t (item 4).
  template<class T>
  struct cringbuf1_t
  {
    typedef T t_value;

    // OWNER part

      // On default construction, the ringbuffer makes no allocations and cannot contain data.
      //  Copying and assignment of such object is exceptionless.
      // To make the working ringbuffer, able to contain > 0 values, resize it with set_cap().
    cringbuf1_t() __bmdx_noex { _ipush = _ipop = 0; }

      // Default copy ctor. and dtor. are working correctly.
    //cringbuf1_t(const cringbuf1_t& x);
    //~cringbuf1_t() __bmdx_noex;

    cringbuf1_t& operator=(const cringbuf1_t& x) { cringbuf1_t a2(x); swap(a2); return *this; }

      // Remove all values, release all dynamic allocations.
      //  Set the container capacity to 0. Set navl(), ipush(), ipop() = 0.
      //  The container copying becomes trivial and faultless.
      //  The container will not be able to contain data until resized again with set_cap().
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void clear() __bmdx_noex { a.clear(); _ipush = _ipop = 0; }

      // Change the container capacity (ncap()), i.e. max. number of elements that can be held at the same time.
      //  a) (b_rmv_el == true)
      //      Remove all existing elements - equivalent to pop_n(-1).
      //      Set the capacity to n.
      //      Keep ipush() unchanged, set ipop() = ipush().
      //      Return 1.
      //  b) (b_rmv_el == false)
      //      If n == ncap(): do nothing, return 1.
      //      Otherwise:
      //        Set the capacity to n.
      //        Move existing values (up to the last n values) so that they occupied the same positions
      //          related to ipush(), as in original container.
      //        Keep ipush() unchanged.
      //        Set ipop() = ipush() - min(navl(), n), where navl() is taken before resizing.
      //        Return 1.
      //  c) On failure, the object is not modified (logically).
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
      // Returns:
      //    1 - success.
      //    -1 - n < 0.
      //    -2 - failure (mem. alloc.).
    _s_long set_cap(_s_ll n, bool b_rmv_el) __bmdx_noex
    {
      if (n < 0) { return -1; }
      if (!b_rmv_el && n == a.n()) { return 1; }
      if (n == 0 && a.n() == 0) { return 1; }
      cringbuf1_t a2;
      if (!a2.a.resize(n)) { return -2; }
      if (b_rmv_el) { a2._ipush = a2._ipop = _ipush; swap(a2); return 1; }
      const _s_ll ncp = bmdx_minmax::myllmin(navl(), n);
      const _s_ll i0 = _ipush - ncp;
      _a_copy_n_u(a2.a, i0, a, i0, ncp);
      a2._ipush = _ipush; a2._ipop = i0; swap(a2);
      return 1;
    }

      // Swap this container with x.
      // NOTE The container is bytewise movable and swappable.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void swap(cringbuf1_t& x) __bmdx_noex { bmdx_str::words::swap_bytes(*this, x); }

    // COMMON part

      // ipush(): abs. index of the next of value to be pushed.
      // ipop(): abs. index of the next of value to be popped.
      // Concurrency:
      //    can change at any time as side effect of value pushing and popping by Supplier and Consumer, respectively.
    _s_ll ipush() const __bmdx_noex { return _ipush; }
    _s_ll ipop() const __bmdx_noex { return _ipop; }

      // Max. number of values that can be kept in the buffer.
      // Concurrency:
      //    can be changed as side effect of resize() by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    _s_ll ncap() const __bmdx_noex { return a.n(); }

    // SUPPLIER part

      // Max. number of values that can be currently pushed into the buffer w/o resizing.
      //  Value is equivalent to ncap() - navl().
      // Concurrency:
      //    navl() is mainly for use by Supplier.
      //    a) from Supplier side: nfree() may only increase, up to ncap(), at any time, as side effect of value popping by Consumer.
      //    b) from Consumer side: nfree() may only decrease, down to 0, at any time, as side effect of value pushing by Supplier.
    _s_ll nfree() const __bmdx_noex { return a.n() - navl(); }

      // Push new value (using operator=).
      // Returns:
      //    1 - success,
      //    -1 - no place in the buffer currently.
      //    -3 - failure on elem. assignment (not expected).
      // Concurrency: may be called by Supplier only.
    _s_long push_1(const t_value& s) __bmdx_noex { if (navl() >= a.n()) { return -1; } try { tail(0) = s; } catch (...) { return -3; } _ipush += 1; return 1; }

      // Push n (>= 0) values, using operator=. n must be [0..nfree()] to succeed.
      // Returns:
      //    1 - success. n values are pushed.
      //    0 - pss == 0.
      //    -1 - a) n < 0, b) there's no place for n elements in the buffer currently.
      //    -3 - failure on elem. assignment (not expected).
      // Concurrency: may be called by Supplier only.
    _s_long push_n(const t_value* pss, _s_ll n) __bmdx_noex
    {
      if (!pss) { return 0; }
      if (n < 0) { return -1; }
      if (n == 0) { return 1; }
      if (n > nfree()) { return -1; }
      _s_ll np2 = n;
      t_value* p2 = a.pd() + _ipush % a.n();
      const t_value* const pe = a.pd() + a.n();
      try { do { *p2++ = *pss++; if (p2 == pe) { p2 = a.pd(); } } while (--np2); } catch (...) { return -3; }
      _ipush += n;
      return 1;
    }

      // Returns ref. to absolute pos. (ipush() + ineg) (internally: by modulus ncap()).
      //    ineg must be negative, in [-navl()..-1], to point to valid position.
      //    I.e. ineg == -1 points to the last pushed value, -2 - one back from it etc.
      // NOTE tail may not be called on empty buffer (on ncap() == 0) - this would cause page fault.
      // Concurrency:
      //    this function is UNSAFE to call by any side (due to unexpected bounds modification and queue exhausting).
    t_value& tail(_s_ll ineg) __bmdx_noex { _s_ll i = (_ipush + ineg) % a.n(); if (i < 0) { i += a.n(); } return a[i]; }
    const t_value& tail(_s_ll ineg) const __bmdx_noex { return ((cringbuf1_t*)this)->tail(ineg); }

    // CONSUMER part

      // The currently available number of values.
      // Concurrency:
      //    navl() may be called by any side, but is mainly for use by Consumer.
      //    a) from Consumer side: navl() may only increase, up to ncap(), at any time, as side effect of value pushing by Supplier.
      //    b) from Supplier side: navl() may only decrease, down to 0, at any time, as side effect of value popping by Consumer.
    _s_ll navl() const __bmdx_noex { return _ipush - _ipop; }

      // Returns max. number of contiguous values, available for efficient reading and popping.
      //    A pointer to the first available value is &(*this)[0].
      //    Additionally (single-threaded mode only):
      //      Let n2 = navl() - navl_contig().
      //      If n2 > 0, the second contiguous part (n2 values), starts at &(*this)[navl_contig()].
      // Concurrency:
      //    same as navl(), only navl_contig() may not be used to access the second contiguous part of values.
    _s_ll navl_contig() const __bmdx_noex { return bmdx_minmax::myllmin(a.n() - _ipop % a.n(), navl()); }

      // Pop one value.
      // Returns:
      //    true - popped successfully,
      //    false - not popped because the buffer is currently empty.
      // Concurrency: may be called by Consumer only.
    bool pop_1() __bmdx_noex { if (_ipush <= _ipop) { return false; } _ipop += 1; return true; }

      // Pop multiple values.
      //    n >= 0 specifies max. number of values to be popped.
      //      Really, min(n, navl()) values will be popped.
      //    n == -1 pops all currently available values.
      //    n <= -2 does nothing (same as n == 0).
      // Concurrency: may be called by Consumer only.
      // Returns:
      //    the number of elements actually popped. The function does not fail.
    _s_ll pop_n(_s_ll n) __bmdx_noex { if (n == 0 || n <= -2) { return 0; } const _s_ll na = navl(); if (n == -1 || n > na) { n = na; } _ipop += n; return n; }

      // Returns ref. to absolute pos. (ipop() + i) (internally: by modulus ncap()).
      //    To point to valid position, i must be in [0..navl()).
      //    [0] - is the element that would be popped next.
      // NOTE operator[] may not be called on empty buffer (on ncap() == 0) - this would cause page fault.
      // Concurrency: may be called by Consumer only.
    t_value& operator[] (_s_ll i) __bmdx_noex { _s_ll j = (_ipop + i) % a.n(); if (j < 0) { j += a.n(); } return a[j]; }
    const t_value& operator[] (_s_ll i) const __bmdx_noex { return ((cringbuf1_t&)*this)[i]; }

  private:
    carray_t<t_value, false> a; // values
    volatile _s_ll _ipush, _ipop; // the next push position, corresponding pos. in bc == _ipush % ncap(); the next pop position; _ipop <= _ipush; corresponding position in bc == _ipop % ncap()

    static void _a_copy_n_u(carray_t<t_value, false>& adest, _s_ll idest, const carray_t<t_value, false>& asrc, _s_ll isrc, _s_ll n) // memory copying (without array capacity and other checks)
    {
      if (n <= 0) { return; }
      const _s_ll i1 = isrc % asrc.n(); const _s_ll i2 = idest % adest.n();
      _s_ll n1 = asrc.n() - i1; _s_ll n2 = adest.n() - i2; const t_value* p1 = asrc.pd() + i1; t_value* p2 = adest.pd() + i2;
      _s_ll ncp = bmdx_minmax::myllmin(n1, n2, n);
      bmdx_str::words::memmove_t<t_value>::sf_memcpy(p2, p1, sizeof(t_value) * ncp);
      if (n <= ncp) { return; }
      n1 -= ncp; n2 -= ncp; p1 += ncp; p2 += ncp; if (n1 <= 0) { p1 = asrc.pd(); n1 = asrc.n(); } if (n2 <= 0) { p2 = adest.pd(); n2 = adest.n(); }
      n -= ncp; ncp = bmdx_minmax::myllmin(n1, n2, n);
      bmdx_str::words::memmove_t<t_value>::sf_memcpy(p2, p1, sizeof(t_value) * ncp);
      if (n <= ncp) { return; }
      n1 -= ncp; n2 -= ncp; p1 += ncp; p2 += ncp; if (n1 <= 0) { p1 = asrc.pd(); n1 = asrc.n(); } if (n2 <= 0) { p2 = adest.pd(); n2 = adest.n(); }
      n -= ncp;
      bmdx_str::words::memmove_t<t_value>::sf_memcpy(p2, p1, sizeof(t_value) * n);
    }
  };



  //== struct cppringbuf1_t

    // Single-channel ringbuffer (fixed-size queue) for objects with non-trivial construction, destruction and copying.
    //  Data supplier and consumer may run as pair of threads and use it concurrently, without any blocking and locks.
    //  The container allows for efficient random access to existing elements (O(1)).
    //  push*() and pop*() do NOT move the existing elements.
    //  The container is suitable for passing across binary modules of one process,
    //    under the same conditions as described for struct cpparray_t (item 4).
  template<class T, class _bs = bmdx_meta::nothing>
  struct cppringbuf1_t
  {
    typedef T t_value;
    struct exc_cc : std::exception { const char* what() const __bmdx_noex { return "cppringbuf1_t(const cppringbuf1_t&)"; } };

    // OWNER part

      // On default construction, the ringbuffer makes no allocations and cannot contain data.
      //  Copying and assignment of such object is exceptionless.
      // To make the working ringbuffer, able to contain > 0 values, resize it with set_cap().
    cppringbuf1_t() __bmdx_noex { _ipush = _ipop = 0; }

    cppringbuf1_t(const cppringbuf1_t& x) { if (!a.resize(x.a.n())) { throw exc_cc(); } if (!_a_copy_n_u(a, x._ipop, x.a, x._ipop, x._ipush - x._ipop)) { throw exc_cc(); } _ipush = x._ipush; _ipop = x._ipop; }

    ~cppringbuf1_t() __bmdx_noex { _a_pop_n(navl()); }

    cppringbuf1_t& operator=(const cppringbuf1_t& x) { cppringbuf1_t a2(x); swap(a2); return *this; }

      // Remove all values, release all dynamic allocations.
      //  Set the container capacity to 0. Set navl(), ipush(), ipop() = 0.
      //  The container copying becomes trivial and faultless.
      //  The container will not be able to contain data until resized again with set_cap().
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void clear() __bmdx_noex { _a_pop_n(navl()); a.clear(); _ipush = _ipop = 0; }

      // Change the container capacity (ncap()), i.e. max. number of elements that can be held at the same time.
      //  a) (b_rmv_el == true)
      //      Remove all existing elements - equivalent to pop_n(-1).
      //      Set the capacity to n.
      //      Keep ipush() unchanged, set ipop() = ipush().
      //      Return 1.
      //  b) (b_rmv_el == false)
      //      If n == ncap(): do nothing, return 1.
      //      Otherwise:
      //        Set the capacity to n.
      //        Move existing values (up to the last n values) so that they occupied the same positions
      //          related to ipush(), as in original container.
      //        Keep ipush() unchanged.
      //        Set ipop() = ipush() - min(navl(), n), where navl() is taken before resizing.
      //        Return 1.
      //  c) On failure, the object is not modified (logically).
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
      // Returns:
      //    1 - success.
      //    -1 - n < 0.
      //    -2 - failure (mem. alloc.).
      //    -3 - failure (existing elements copying).
    _s_long set_cap(_s_ll n, bool b_rmv_el) __bmdx_noex
    {
      if (n < 0) { return -1; }
      if (!b_rmv_el && n == a.n()) { return 1; }
      if (n == 0 && a.n() == 0) { return 1; }
      cppringbuf1_t a2;
      if (!a2.a.resize(n)) { return -2; }
      if (b_rmv_el) { a2._ipush = a2._ipop = _ipush; swap(a2); return 1; }
      const _s_ll ncp = bmdx_minmax::myllmin(navl(), n);
      if(!_a_copy_n_u(a2.a, _ipush - ncp, a, _ipush - ncp, ncp)) { return -3; }
      a2._ipush = _ipush; a2._ipop = _ipush - ncp; swap(a2);
      return 1;
    }

      // Swap this container with x.
      // NOTE The container is bytewise movable and swappable.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void swap(cppringbuf1_t& x) __bmdx_noex { bmdx_str::words::swap_bytes(*this, x); }

    // COMMON part

      // ipush(): abs. index of the next of value to be pushed.
      // ipop(): abs. index of the next of value to be popped.
      // Concurrency:
      //    can change at any time as side effect of value pushing and popping by Supplier and Consumer, respectively.
    _s_ll ipush() const __bmdx_noex { return _ipush; }
    _s_ll ipop() const __bmdx_noex { return _ipop; }

      // Max. number of values that can be kept in the buffer.
      // Concurrency:
      //    can be changed as side effect of resize() by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    _s_ll ncap() const __bmdx_noex { return a.n(); }

    // SUPPLIER part

      // Max. number of values that can be currently pushed into the buffer w/o resizing.
      //  Value is equivalent to ncap() - navl().
      // Concurrency:
      //    navl() is mainly for use by Supplier.
      //    a) from Supplier side: nfree() may only INCREASE, up to ncap(), at any time, as side effect of value popping by Consumer.
      //    b) from Consumer side: nfree() may only decrease, down to 0, at any time, as side effect of value pushing by Supplier.
    _s_ll nfree() const __bmdx_noex { return a.n() - navl(); }

    struct new_dflt { void operator()(T* p) const { new (p) T(); } }; // creates a T() at p
    struct new_cp { const T& x; new_cp(const T& x_) : x(x_) {} void operator()(T* p) const { new (p) T(x); } }; // creates a T(x) at p
    struct new_cp_seq { mutable const T* psrc; new_cp_seq(const T* psrc_) : psrc(psrc_) {} void operator()(T* p) const { new (p) T(*psrc++); } }; // creates a T(*psrc++) at each call

      // push_1(x): push a copy of x.
      // push_1f(f): accepts a client-defined functor f that is able to create an element,
      //    see e.g. new_dflt, new_cp.
      // Returns:
      //    1 - success,
      //    -1 - no place in the buffer currently,
      //    -3 - element copying failed.
      // Concurrency: may be called by Supplier only.
    _s_long push_1(const t_value& x) __bmdx_noex { return _push_1(new_cp(x)); }
    template<class F> _s_long push_1f(const F& f) __bmdx_noex { return _push_1(f); }

      // Push n (>= 0) values. n must be [0..nfree()] to succeed.
      // push_n(n): uses copy constructor to create elements.
      // push_nf(f, n): accepts a client-defined functor f that is able to sequentially create n values,
      //    see e.g. new_dflt, new_cp, new_cp_seq.
      // Returns:
      //    1 - success. n values are pushed.
      //    0 - pss == 0.
      //    -1 - a) n < 0, b) there's no place for n elements in the buffer currently.
      //    -3 - failure (element creation or copying).
      // Concurrency: may be called by Supplier only.
    _s_long push_n(const t_value* pss, _s_ll n) __bmdx_noex { if (!pss) { return 0; } return _push_n(new_cp_seq(pss), n); }
    template<class F> _s_long push_nf(const F& f, _s_ll n) __bmdx_noex { return _push_n(f, n); }

      // Returns ref. to absolute pos. (ipush() + ineg) (internally: by modulus ncap()).
      //    ineg must be negative, in [-navl()..-1], to point to valid position.
      //    I.e. ineg == -1 points to the last pushed value, -2 - one back from it etc.
      // NOTE tail() may not be called on empty buffer (on ncap() == 0) - this would cause page fault.
      // Concurrency:
      //    this function is UNSAFE to call by any side (due to unexpected bounds modification and queue exhausting).
    t_value& tail(_s_ll ineg) __bmdx_noex { _s_ll i = (_ipush + ineg) % a.n(); if (i < 0) { i += a.n(); } return a[i]; }
    const t_value& tail(_s_ll ineg) const __bmdx_noex { return ((cppringbuf1_t*)this)->tail(ineg); }

    // CONSUMER part

      // The currently available number of values.
      // Concurrency:
      //    navl() may be called by any side, but is mainly for use by Consumer.
      //    a) from Consumer side: navl() may only increase, up to ncap(), at any time, as side effect of value pushing by Supplier.
      //    b) from Supplier side: navl() may only decrease, down to 0, at any time, as side effect of value popping by Consumer.
    _s_ll navl() const __bmdx_noex { return _ipush - _ipop; }

      // Returns max. number of contiguous values, available for efficient reading and popping.
      //    A pointer to the first available value is &(*this)[0].
      //    Additionally (single-threaded mode only):
      //      Let n2 = navl() - navl_contig().
      //      If n2 > 0, the second contiguous part (n2 values), starts at &(*this)[navl_contig()].
      // Concurrency:
      //    same as navl(), only navl_contig() may not be used to access the second contiguous part of values.
    _s_ll navl_contig() const __bmdx_noex { return bmdx_minmax::myllmin(a.n() - _ipop % a.n(), navl()); }

      // Pop one element.
      // Returns:
      //    true - popped successfully,
      //    false - not popped because the buffer is currently empty.
      // Concurrency: may be called by Consumer only.
    bool pop_1() __bmdx_noex { if (_ipush <= _ipop) { return false; } _a_pop_1(); return true; }

      // Pop multiple elements.
      //    n >= 0 specifies max. number of values to be popped.
      //      Really, min(n, navl()) values will be popped.
      //    n == -1 pops all currently available elements.
      //    n <= -2 does nothing (same as n == 0).
      // Concurrency: may be called by Consumer only.
      // Returns:
      //    the number of elements actually popped. The function does not fail.
    _s_ll pop_n(_s_ll n) __bmdx_noex { return _a_pop_n(n == -1 ? a.n() : n); }

      // Same as pop_1(), but does not call element destructor.
      //  (For use with efficient memory copying on popping elements of movable types.)
    bool discard_1() __bmdx_noex { if (_ipush <= _ipop) { return false; } _ipop += 1; return true; }

      // Same as pop_n(), but does not call elements' destructors.
      //  (For use with efficient memory copying on popping elements of movable types.)
    _s_ll discard_n(_s_ll n) __bmdx_noex { if (n == 0 || n <= -2) { return 0; } const _s_ll na = navl(); if (n == -1 || n > na) { n = na; } _ipop += n; return n; }

      // Returns ref. to absolute pos. (ipop() + i) (internally: by modulus ncap()).
      //    To point to valid position, i must be in [0..navl()).
      //    [0] - is the element that would be popped next.
      // NOTE operator[] may not be called on empty buffer (on ncap() == 0) - this would cause page fault.
      // Concurrency: may be safely called by Consumer only.
    t_value& operator[] (_s_ll i) __bmdx_noex { _s_ll j = (_ipop + i) % a.n(); if (j < 0) { j += a.n(); } return a[j]; }
    const t_value& operator[] (_s_ll i) const __bmdx_noex { return ((cppringbuf1_t&)*this)[i]; }

  private:
    carray_t<t_value, false> a; // certain part of entries contains C++ objects
    volatile _s_ll _ipush, _ipop; // the next push position, corresponding pos. in bc == _ipush % ncap(); the next pop position; _ipop <= _ipush; corresponding position in bc == _ipop % ncap()

    struct __a2 : carray_t<t_value, false> { typedef carray_t<t_value, false> t; void _destroy(T* pd_, typename t::_t_size i0, typename t::_t_size i2) { this->t::_destroy(pd_, i0, i2); } void _destroy1(T* pd_) { this->t::_destroy1(pd_); } };
    void _a_pop_1() { const _s_ll j = _ipop; static_cast<__a2&>(a)._destroy1(a.pd() + j % a.n()); _ipop = j + 1; }
    _s_ll _a_pop_n(_s_ll n) { const _s_ll j = _ipop; n = bmdx_minmax::myllmin(n, _ipush - j, a.n()); if (n <= 0) { return 0; } _a_destroy_n_u(a, j, n); _ipop = j + n; return n; }
    static void _a_destroy_n_u(carray_t<t_value, false>& a, _s_ll ind0, _s_ll n) { if (n <= 0) { return; } _s_ll i0 = ind0 % a.n(); _s_ll i2 = bmdx_minmax::myllmin(i0 + n, a.n()); static_cast<__a2&>(a)._destroy(a.pd(), i0, i2); i2 = n - (i2 - i0); if (i2 > 0) { static_cast<__a2&>(a)._destroy(a.pd(), 0, i2); } }
    static bool _a_copy_n_u(carray_t<t_value, false>& adest, _s_ll idest, const carray_t<t_value, false>& asrc, _s_ll isrc, _s_ll n) // transactional copying (without array capacity and other checks)
    {
      if (n <= 0) { return true; }
      _s_ll n2 = n;
      const t_value* p1 = asrc.pd() + isrc % asrc.n(); const t_value* p1e = asrc.pd() + asrc.n();
      t_value* p2 = adest.pd() + idest % adest.n(); const t_value* const p2e = adest.pd() + adest.n();
      try { do { new (p2) t_value(*p1); ++p2; ++p1; if (p1 == p1e) { p1 = asrc.pd(); } if (p2 == p2e) { p2 = adest.pd(); } } while (--n2); }
      catch (...) { _a_destroy_n_u(adest, idest, n - n2); return false; }
      return true;
    }
    template<class F> _s_long _push_1(const F& f) { if (navl() >= a.n()) { return -1; } try { f(&tail(0)); } catch (...) { return -3; } _ipush += 1; return 1; }
    template<class F> _s_long _push_n(const F& f, _s_ll n)
    {
      if (n < 0) { return -1; }
      if (n == 0) { return 1; }
      if (n > nfree()) { return -1; }
      _s_ll np2 = n;
      t_value* p2 = a.pd() + _ipush % a.n();
      const t_value* const pe = a.pd() + a.n();
      try { do { f(p2); ++p2; if (p2 == pe) { p2 = a.pd(); } } while (--np2); }
      catch (...) { _a_destroy_n_u(a, _ipush, n - np2); return -3; }
      _ipush += n;
      return 1;
    }
  };



  //== struct vnnqueue_t

    // Variable-size, non-blocking, non-moving (elements) queue.
    //
    //  Similarity to cppringbuf1_t (fixed-size queue).
    //    1. Data supplier and consumer may run as pair of threads and use it concurrently, without any blocking.
    //    2. push*() and pop*() do NOT move the existing elements.
    //    3. The container is suitable for passing across binary modules of one process,
    //        under the same conditions as described for struct cpparray_t (item 4).
    //
    //  Specific features.
    //    1. vnnqueue_t is multi-part container.
    //        By default, any push*() operation may cause automatic queue growth,
    //          and any pop*() operation may cause automatic queue shrinking.
    //          Thus dynamic memory allocation/freeing does not guarantee fixed time for pushing and popping.
    //        If growth/shrinking is disabled, pushing and popping take fixed time per element, same as in cppringbuf1_t.
    //    2. front(), back() take fixed time.
    //        Random access may be slower, depending on m() (column size).
    //
    //  Comparison with STL containers.
    //      1. Efficiency:
    //        Access, push, pop efficiency: average of STL vector, list, queue.
    //        Elements are not moved since construction, similar to std::list.
    //        Random access speed with small m() (column size) is O(N) as in std::list.
    //        Random access speed with considerably large m() is O(1) as in std::vector.
    //      2. Storage:
    //        by dflt., memory consumption is ~= that of std::list.
    //        Large excessive place, e.g. as in vector, can be reserved with explicit call (set_rsv()).
    //      3. Advanced features:
    //        1) non-blocking, lockless push/pop,
    //        2) same container object can be safely manipulated in different binary modules of the process,
    //        3) controllable automatic reserve management, according to the specified min./max. limits.
    //
  template<class T, class _bs = bmdx_meta::nothing>
  struct vnnqueue_t
  {
    #ifndef __bmdx_vnncol_alloc
      #define __bmdx_vnncol_alloc(m) this->_vnncol_alloc(m)
      #define __bmdx_vnncol_free(p) this->_free(p)
    #endif

    // COMMON part

    struct exc_iter : std::exception { const char* what() const __bmdx_noex { return "vnnqueue_t::iterator_t op."; } };
    struct exc_range : std::exception { const char* what() const __bmdx_noex { return "vnnqueue_t range chk."; } };
    struct exc_cc : std::exception { const char* what() const __bmdx_noex { return "vnnqueue_t(const vnnqueue_t&)"; } };

    struct column { T* data() { return (T*)(this + 1); } column* prev() const { return _prev; } column* next() const { return _next; } private: friend struct vnnqueue_t; column *_prev, *_next; column(); };

      // Iterator is recommended for efficient peeking or modifying (e.g. sorting) multiple pushed values.
      //  Any dereferencing is range-checked.
      //  NOTE Iterator remembers absolute bounds of data at the time when it's created.
      //    During iterator lifetime, it's safe to push additional elements (including concurrently!)
      //    but UNSAFE to pop any number of elements.
      // Concurrency:
      //    on concurrent access, iterator_t may be used by Consumer ONLY.
    template<bool is_const>
    class iterator_t : public std::iterator<std::random_access_iterator_tag, T>
    {
      vnnqueue_t* _ct; column* _c; _s_ll _i, _ic, _ipop, _ipush; // _ipop, _ipush == const during iterator lifetime
    public:
      typedef T value_type; typedef T t_value;
      typedef typename bmdx_meta::if_t<is_const, const t_value*, t_value*>::result  pointer;
      typedef typename bmdx_meta::if_t<is_const, const t_value&, t_value&>::result reference;
      typedef std::random_access_iterator_tag iterator_category; typedef iterator_t iterator_type; typedef _s_ll difference_type;

        // The default iterator is not bound to any container, thus not practically useful.
      inline iterator_t() __bmdx_noex { _ct = 0; _c = 0; _i = _ic = _ipop = _ipush = 0; }
        // Front or end pos. iterator creation.
      inline iterator_t(const vnnqueue_t& ct_, bool b_front) __bmdx_noex { _ct = (vnnqueue_t*)&ct_;  _ipop = ct_._ipop; _ipush = ct_._ipush; if (b_front) { _i = _ic = _ipop; _c = ct_._cpop; } else { _s_ll i1 = _ipush; column* c = ct_._cpush; _s_ll i2 = _ipush; if (i2 == i1) { _i = _ic = i1; _c = c; return; } i1 = _ipush; c = ct_._cpush; i2 = _ipush; if (i2 == i1) { _i = _ic = i1; _c = c; return; } *this = iterator_t(ct_, true); move_by(navl()); } }
        // Direct conversion from const to non-const and back, for simplicity.
      inline iterator_t(const iterator_t<false>& x) __bmdx_noex { *this = (iterator_t&)x; }
      inline iterator_t(const iterator_t<true>& x) __bmdx_noex { *this = (iterator_t&)x; }

        // (Constant)
        // Number of elements in the addressed sequence.
        //  Fixed at the moment when the iterator is created by parent container.
      _s_ll navl() const __bmdx_noex { return _ipush - _ipop; }
        // (Constant)
        // Number of elements in a column (single contiguous storage).
      _s_ll m() const __bmdx_noex { return _m; }
        // (Constant)
        // Absolute indices to the beginning and the end of sequence.
        //  Fixed at the moment when the iterator is created by parent container.
      _s_ll ipop() const __bmdx_noex { return _ipop; }
      _s_ll ipush() const __bmdx_noex { return _ipush; }

        // The absolute index of the addressed element.
      _s_ll ind() const __bmdx_noex { return _i; }

        // Index of the currently addressed element in the current column.
        //  The value is valid only if col() != 0. See also col().
      _s_ll irow() const __bmdx_noex { return _i % _ct->_m; }

        // Returns true if iterator points to valid element, false otherwise.
      bool b_elem() const __bmdx_noex { return _i >= _ipop && _i < _ipush; }

        // Returns true if the current iterator position is before the beginning of sequence.
      bool b_bbeg() const __bmdx_noex { return _i < _ipop; }

        // Returns true if the current iterator position >= end of sequence.
      bool b_aend() const __bmdx_noex { return _i >= _ipush; }

        // This function may be used to access the current element or any element in the currently addressed column.
        // Returns:
        //    a) The column, to which the currently addressed element belongs.
        //        In this case,
        //        col()->data()[irow()] is equivalent to iterator dereferencing.
        //        col()->data()[i], where i is in [0..m()), addresses elements in the same column (should be used only on existing elements).
        //    b) 0: iterator does not point to valid element.
      column* col() const __bmdx_noex { if (!b_elem()) { return 0; } return _c; }

        // The parent container.
        //  NOTE Iterators from different containers may not be compared or in any other way related.
      vnnqueue_t* ctnr() const __bmdx_noex { return _ct; }

      inline reference operator*() const { if (!(_i >= _ipop && _i < _ipush)) { throw exc_range(); } return _c->data()[_i % _ct->_m]; }
      inline pointer operator->() const { return &**this; }


        // Returns a ref. to element, relative to the current iterator position.
        // Concurrency:
        //    1. operator[] may be safely called by Consumer only.
        //    2. For Supplier, operator[] is UNSAFE, because the Consumer may exhaust the queue at any time, making cached index bounds invalid.
      inline reference operator[](difference_type delta) const { return *iterator_t(*this).move_by(delta); }

        // Iterator position change. Does not fail.
        //  Iterator position is always treated as plain 64-bit signed value.
        //  When iterator points to anywhere out of sequence (e.g. after-after-end), it can be safely moved back to valid position,
        //    and then dereferenced in usual way.
        // Time taken: proportional to abs(delta) / m().
        // NOTE All iterator changes are implemented via move_by().
      iterator_type& move_by(_s_ll delta) __bmdx_noex
      {
        if (delta == 0) { return *this; }
        const _s_ll i2 = _i + delta;
        const bool bl = i2 < _ipop, br = i2 > _ipush;
        if (!_c || bl || br || _ipush == _ipop) { _i = i2; return *this; } // if i2 is out of valid range, _ic continues to hold a real position
        const _s_ll nsteps = (bl ? _ipop : (br ? _ipush : i2)) - (_ic - _ic % _ct->_m); // valid pos. rel. to c beginning
        if (nsteps > 0) { _s_ll icol = nsteps / _ct->_m; while (icol--) { _c = _c->_next; } }
        else if (nsteps < 0) { _s_ll icol = (nsteps + 1 - _ct->_m) / _ct->_m; while (icol++) { _c = _c->_prev; } }
        _i = _ic = i2;
        return *this;
      }

        // Movement to absolute position.
        //  The position itself may be anything, but the valid element is addressed only by i in [ipop() .. ipush()).
      iterator_type& move_to(_s_ll i) __bmdx_noex { return move_by(i - _i); }

      inline iterator_type& operator++() __bmdx_noex { return move_by(1); }
      inline iterator_type& operator--() __bmdx_noex { return move_by(-1); }
      inline iterator_type operator++(int) __bmdx_noex { iterator_t i(*this); move_by(1); return i; }
      inline iterator_type operator--(int) __bmdx_noex { iterator_t i(*this); move_by(-1); return i; }

      inline iterator_type& operator+=(difference_type delta) __bmdx_noex { return move_by(delta); }
      inline iterator_type& operator-=(difference_type delta) __bmdx_noex { return move_by(-delta); }
      inline iterator_type operator+(difference_type delta) const __bmdx_noex { return iterator_t(*this).move_by(delta); }
      inline iterator_type operator-(difference_type delta) const __bmdx_noex { return iterator_t(*this).move_by(-delta); }

        // NOTE The operation generates an exception if operands belong to different containers.
      inline difference_type operator-(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return this->_i - x._i; }

        // NOTE Comparisons generate an exception if compared iterators belong to different containers.
      inline bool operator==(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i == x._i; }
      inline bool operator!=(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i != x._i; }
      inline bool operator>(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i > x._i; }
      inline bool operator<(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i < x._i; }
      inline bool operator>=(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i >= x._i; }
      inline bool operator<=(const iterator_type& x) const { if (_ct != x._ct) { throw exc_iter(); } return _i <= x._i; }
    };
    typedef iterator_t<false> iterator; typedef iterator_t<true> const_iterator;
    typedef T value_type; typedef T t_value;

    // OWNER part

      // m_ is automatically adjusted to be >= 1.
      // NOTE The default construction does not make any allocations.
      //  Such container copying is trivial and faultless.
      //  Capacity change (growth, auto-shrinking) is not limited in any way, equivalent to
      //    set_cap_hints(0, -2);
      //  After the default construction, the client may
      //    1) optionally call set_m() (set chunk size) + set_rsv() (reserve some place for elements) + set_cap_hints() (adjust capacity limits),
      //    2) push an element. NOTE Doing that without the above settings, at once allocates 2 columns (2 * max(m_, 1) places for elements).
    vnnqueue_t(_s_ll m_ = 10) __bmdx_noex { _m = m_ >= 1 ? m_ : 1; _scalars_reset(); _pff_reset(); }

    vnnqueue_t(const vnnqueue_t& q)
    {
      new (this) vnnqueue_t(q._m);
      _capmin = q._capmin; _capmax = -2;
      _ipop = q._ipop; _ipush = _ipop;
      _s_long res = set_rsv(q.ncapp());
      if (res != 1) { this->~vnnqueue_t(); throw exc_cc(); }
      for (iterator i = q.begin(); !i.b_aend(); ++i) { res = push_1(*i); if (res != 1) { this->~vnnqueue_t(); throw exc_cc(); } }
      _capmax = q._capmax;
    }

    vnnqueue_t& operator=(const vnnqueue_t& q) { vnnqueue_t q2(q); bmdx_str::words::swap_bytes(*this, q2); return *this; }

    ~vnnqueue_t() __bmdx_noex { pop_n(-1); _link_free_to_cpush(1, 0); _cols_release(); }

      // Remove all values, release all dynamic allocations.
      //  Set min. capacity to 0, max. capacity to unlimited. (May be changed later with set_cap_hints()).
      //  Keep m() unchanged. (May be changed later with set_m()).
      //  The current capacity becomes 0. The container copying becomes trivial and faultless.
      //  navl(), ipush(), ipop() become 0.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void clear() __bmdx_noex { pop_n(-1); _link_free_to_cpush(1, 0); _cols_release(); _scalars_reset(); }

      // Swap this container with x.
      // NOTE The container is bytewise movable and swappable.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    void swap(vnnqueue_t& x) __bmdx_noex { bmdx_str::words::swap_bytes(*this, x); }


      // Sets the new column (place reservation chunk) size.
      //    This operation may succeed only when the container contains no columns:
      //      a) default-initialized,
      //      b) after popping all elements + set_rsv(0);
      // Returns:
      //    1 - success.
      //    -1 - m_ < 1.
      //    -3 - cannot do because the container is not empty.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    _s_long set_m(_s_ll m_) __bmdx_noex { if (m_ < 1) { return -1; } if (_cpush || _cpop) { return -3; } _m = m_; return 1; }

      // Sets values of ncapmin(), ncapmax() - hints for container autoresizing on pop and push, respectively.
      //  Rules:
      //  1. Factual capacity (reserved place) is usually larger than the specified values, because the container allocates place
      //    in fixed-size chunks (see m()), and also because some part of chunks may be unused at the moment by both push and pop side.
      //    (They get used each time when pushing reaches the end of its current chain of chunks.)
      //  2. When new limited (nmax >= 0) max. capacity is set, it does not remove the existing elements, even if their number exceeds the limit.
      //    But the limits are approached as closely as possible during sequences of pop or push operations.
      //  nmin:
      //    a) -1 - the container may not shrink from the current reserve.
      //      In this mode, freeing the dynamic memory for columns is disabled (but this does not disable what T ctor/dtor may make),
      //      and the number of managed columns does not decrease.
      //    b) >= 0 the container may shrink on pop until nmin places (or the closest larger value) are allocated.
      //      See also ncapp(), ncapeff().
      //  nmax:
      //    a) -2 - no limit to queue growth.
      //    b) -1 - the number of elements cannot exceed the currently reserved place.
      //      In this mode allocating the dynamic memory for columns is disabled (but this does not disable what T ctor/dtor may make),
      //      and the number of managed columns does not increase.
      //    c) >= 0 (must be >= nmin if nmin >= 0) - exact limit of max. number of elements in the container.
      //      After nmax elements are pushed, any push operation is denied until some popping,
      //        even if the really reserved place is larger than nmax.
      //      Also, after the place reserve reaches nmax or pre-set to >= nmax (see set_rsv()),
      //      further allocating the dynamic memory for columns is disabled (because not necessary),
      //        same as with nmax == -1.
      // NOTE To imitate fixed-size queue (like cppringbuf1_t), use
      //    set_cap_hints(-1, cap); set_rsv(cap);
      //    Here, cap is the desired fixed capacity.
      // NOTE vnnqueue_t default constructor disables any capacity limits, equivalent to:
      //    set_cap_hints(0, -2);
      // Returns:
      //    1 - success: ncapmin() == nmin, ncapmax() == nmax.
      //    -1 - no changes: argument constraints are not satisfied.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    _s_long set_cap_hints(_s_ll nmin, _s_ll nmax) __bmdx_noex { if (!(nmin >= -1 && nmax >= -2)) { return -1; } if (nmin >= 0 && nmax >= 0 && nmax < nmin) { return -1; } _capmin = nmin; _capmax = nmax; return 1; }

      // Modify the current place reserve for elements to be as close as possible to nrsv.
      //    The container will be capable of holding approx. >= nrsv elements without additional dynamic allocations.
      //  Rules:
      //    1. The operation ignores ncapmin(), ncapmax().
      //    2. On nrsv > 0, the number of allocated places is ((nrsv + m() - 1) / m() * m()) + m().
      //    3. On nrsv == 0, all existing free columns are removed.
      //      In case if navl() == 0, all columns are removed (because no data at all),
      //      and the queue is put into default state:
      //      no memory allocated, queue copying is faultless, column size may be changed (see set_m()).
      // NOTE To imitate fixed-size queue (like cppringbuf1_t), use
      //    set_cap_hints(-1, cap); set_rsv(cap);
      //    Here, cap is the desired fixed capacity.
      // Returns:
      //    1 - success: the new real reserve is >=, but close to max(nrsv, navl()).
      //    -1 - nrsv < 0.
      //    -2 - failed to allocate memory.
      // Concurrency:
      //    may be called by Owner only.
      //    Both Supplier and Consumer must not be active at that time.
    _s_long set_rsv(_s_ll nrsv) __bmdx_noex
    {
      if (nrsv < 0) { return -1; }
      if (_cpush)
      {
        const _s_ll na = navl();
        _link_free_to_cpush(1, 1);
        if (nrsv == 0 && na == 0) { _cols_release(); _col_scalars_reset(); _caps_reset(); }
        else
        {
          const _s_ll n0 = na - _ipush % _m;
          _s_ll nrfact = n0 <= 0 ? 0 : (n0 + _m - 1) / _m * _m;
          column* cf = _cpush;
          while (nrfact < nrsv)
          {
            if (!cf->_next) { break; }
            nrfact += _m; cf = cf->_next;
          }
          _cfree_prv = _cfree_last = 0; _ilnknew = _ilnkused = 0;
          _cpop->_prev = 0;
          if (nrfact >= nrsv) { column* cf1 = cf->_next; cf->_next = 0; while (cf1) { column* cf2 = cf1->_next; __bmdx_vnncol_free(cf1); _cap_pop -= 1; cf1 = cf2; } _caps_normalize(); }
          else { while (nrfact < nrsv) { column* cnew = __bmdx_vnncol_alloc(_m); if (!cnew) { _caps_normalize(); return -2; } cnew->_prev = cf; cnew->_next = 0; cf->_next = cnew; cf = cnew; nrfact += _m; _cap_push += 1; } _caps_normalize(); }
        }
      }
      else { return _set_rsv_initial(nrsv); }
      return 1;
    }


      // (Constant, informational)
      // Number of elements in a column (single linear part of sequence).
      // Value: >= 1.
    _s_ll m() const __bmdx_noex { return _m; }

      // Min. and max. capacity, to which the container shrinks or grows automatically.
      //    ncapmax() >= 0 also serves as the exact limit for max. number of elements at push op.
      //    See also set_cap_hints().
      // Values:
      //    ncapmax: -2 (infinite growth allowed), -1 (no growth allowed), or >=0 (single push will succeed only if navl() < ncapmax()).
      //    ncapmin: -1 (no shrinking allowed), or >= 0.
    _s_ll ncapmax() const __bmdx_noex { return _capmax; }
    _s_ll ncapmin() const __bmdx_noex { return _capmin; }

      // The pessimistic estimate of the container capacity:
      //    the guaranteed min. number of places for elements,
      //    available at any time (after any popping), and can be used without additional dynamic allocations,
      //    unless limited by ncapmax() >= 0.
      // The exact effective value of capacity at the moment depends on elements placement (ipop()),
      //    see ncapeff().
      // Notes:
      //    1. ncapp() is multiple of m().
      //    2. The number of actually allocated places for elements is exactly (ncapp() > 0 ? ncapp() + m() : 0).
      //    3. ncapp() itself may be partially unused, if the specified ncapmax() limits the number of pushed elements.
      //    4. On popping, ncapp() will decrease unless the container shrinking is disabled. See also set_cap_hints (nmin arg.).
      // Concurrency:
      //    the returned value is reliable for Consumer only.
      //    (For Supplier, the guaranteed capacity is max(0, min(ncapp(), ncapmin())).
      //      Use pre-set reserve and cap. hints to ensure enough place.)
    _s_ll ncapp() const __bmdx_noex { _s_ll n = _cap_push; n += _cap_pop; n -= 1; n *= _m; if (n < 0) { n = 0; } return n; }

      // The exact effective value of the current container capacity:
      //    (ncapeff() - navl()) is the guaranteed min. number of places for elements,
      //    currently available for pushing without additional dynamic allocations.
      // The ncapeff() value is always in range
      //    min(  [ncapp() .. ncapp() + m() - 1], (ncapmax() >= 0 ? ncapmax() : infinity)  ).
      // Concurrency:
      //    the returned value is reliable for Consumer only.
      //    (For Supplier, the capacity may decrease at any time between ncapeff() and value pushing.
      //      Use pre-set reserve and cap. hints to ensure enough place.)
    _s_ll ncapeff() const __bmdx_noex { _s_ll n = ncapp(); if (n == 0) { return 0; } n = n + _m - 1 - _ipop % _m; if (_capmax >= 0) { n = bmdx_minmax::myllmin(n, _capmax); } return n; }

      // Absolute indices to the beginning and the end of sequence.
    _s_ll ipop() const __bmdx_noex { return _ipop; }
    _s_ll ipush() const __bmdx_noex { return _ipush; }

      // Number of elements available to pop.
    _s_ll navl() const __bmdx_noex { return _ipush - _ipop; }

    // SUPPLIER part

    struct new_dflt { void operator()(T* p) const { new (p) T(); } }; // creates a T() at p
    struct new_cp { const T& x; new_cp(const T& x_) : x(x_) {} void operator()(T* p) const { new (p) T(x); } }; // creates a T(x) at p
    struct new_cp_seq { mutable const T* psrc; new_cp_seq(const T* psrc_) : psrc(psrc_) {} void operator()(T* p) const { new (p) T(*psrc++); } }; // creates a T(*psrc++) at each call

      // push_1(x): push a copy of x.
      // push_1f(f): accepts a client-defined functor f that is able to create an element,
      //    see e.g. new_dflt, new_cp.
      // Returns:
      //    1 - success.
      //    -1 - not pushed because of limited capacity.
      //    -2 - failure (mem. alloc. or similar).
      //    -3 - failed to construct the new element.
      // Concurrency: may be called by Supplier only.
    _s_long push_1(const t_value& x) __bmdx_noex { return _push_1(new_cp(x)); }
    template<class Fcreate> _s_long push_1f(const Fcreate& f) __bmdx_noex { return _push_1(f); }

      // Push n (>= 0) values.
      // push_n(n): uses copy constructor to create elements.
      // push_nf(f, n): accepts a client-defined functor f that is able to sequentially create n values,
      //    see e.g. new_dflt, new_cp, new_cp_seq.
      // NOTE On error during operation, all new elements are removed, but the increased reserve (new columns) is not reverted.
      // Returns:
      //    1 - success. n values are pushed.
      //    0 - pss == 0.
      //    -1 - a) not pushed because of limited capacity, b) n < 0.
      //    -2 - failure (mem. alloc. or similar).
      //    -3 - failed to construct the new element.
      // Concurrency: may be called by Supplier only.
    _s_long push_n(const t_value* pss, _s_ll n) __bmdx_noex { if (!pss) { return 0; } return _push_n(new_cp_seq(pss), n); }
    template<class F> _s_long push_nf(const F& f, _s_ll n) __bmdx_noex { return _push_n(f, n); }

      // Returns ref. to the last pushed element.
      // Generates an exception on no elements in queue.
      // Concurrency:
      //    this function is UNSAFE to call by any side (due to unexpected bounds modification and queue exhausting).
    t_value& back() { if (navl() <= 0) { throw exc_range(); } const _s_ll j = _ipush % _m - 1; if (j >= 0) { return _cpush->data()[j]; } else { return _cpush->_prev->data()[_m-1]; } }
    const t_value& back() const { return ((vnnqueue_t*)this)->back(); }

      // Returns ref. to absolute pos. (ipush() + ineg).
      //    ineg must be negative, in [-navl()..-1], to point to valid position.
      //    I.e. ineg == -1 points to the last pushed value, -2 - one back from it etc.
      //    (If the position is invalid, an exception is generated.)
      // Concurrency:
      //    this function is UNSAFE to call by any side (due to unexpected bounds modification and queue exhausting).
    t_value& tail(_s_ll ineg) { return *end().move_by(ineg); }
    const t_value& tail(_s_ll ineg) const { return *cend().move_by(ineg); }

    // CONSUMER part

      // Returns ref. to the element that would be popped next.
      // Generates an exception on no elements in queue.
      // Concurrency: may be safely called by Consumer only.
    t_value& front() { if (navl() <= 0) { throw exc_range(); } return _cpop->data()[_ipop % _m]; }
    const t_value& front() const { return ((vnnqueue_t*)this)->front(); }

      // Iterators may be safely created and their position manipulated at any time
      //    between two successive pop operations.
      //    (Push operation adds elements, but they are simply ignored by previously created iterators.)
      // Concurrency: iterators may be safely used by Consumer only.
    iterator begin() const __bmdx_noex { return iterator(*this, true); }
    iterator end() const __bmdx_noex { return iterator(*this, false); }
    const_iterator cbegin() const __bmdx_noex { return const_iterator(*this, true); }
    const_iterator cend() const __bmdx_noex { return const_iterator(*this, false); }

      // Returns ref. to absolute pos. (ipop() + i).
      //    To point to valid position, i must be in [0..navl()).
      //    (If the position is invalid, an exception is generated.)
      //    [0] - is the element that would be popped next, same as front().
      // Access time: proportional to i / m(). Hint: use iterator for sequential access.
      // Concurrency: may be safely called by Consumer only.
    t_value& operator[](_s_ll i) { const _s_ll na = navl(); if (!(i >= 0 && i < na)) { throw exc_range(); } i += _ipop % _m; _s_ll icol = i / _m; column* c = _cpop; while (icol--) { c = c->_next; } return c->data()[i % _m]; }
    const t_value& operator[](_s_ll i) const { return ((vnnqueue_t&)*this)[i]; }

      // Removes one element.
      //  NOTE See pop_n() for comment on shrinking.
      //  Returns
      //    a) true if element existed (it was anyway removed).
      //    b) false otherwise.
    bool pop_1() __bmdx_noex { const _s_ll i0 = _ipop; const _s_ll i2 = _ipush; if (i2 > i0) { _pop_range(i0, i0 + 1); return true; } return false; }

      // Removes multiple elements.
      //    n >= 0 specifies max. number of values to be popped.
      //      Really, min(n, navl()) values will be popped.
      //    n == -1 pops all currently available elements.
      //    n <= -2 does nothing (same as n == 0).
      // Shrinking behavior:
      //    a) on factually popping one or more elements, with ncapmin() >= 0: the freed columns are deleted,
      //      until the reserve reaches a value closest to ncapmin().
      //    b) with ncapmin() == -1: the container capacity is not modified, i.e. no automatic column deletion occurs.
      //      The columns that are left unoccupied by removal, are treated as reserved space for later pushing.
      // Returns:
      //    the number of elements actually removed (>=0). The function does not fail.
      // Concurrency:
      //    the function may be safely called by Consumer only.
      //    Note that Supplier may safely continue pushing elements during any pop*() operation.
    _s_ll pop_n(_s_ll n) __bmdx_noex { if (n == 0 || n <= -2) { return 0; } const _s_ll i0 = _ipop; _s_ll na = _ipush - i0; if (n == -1 || n > na) { n = na; } if (n > 0) { _pop_range(i0, i0 + n); } return n; }

  private:
    typedef void* (*F_alloc)(_s_ll nbytes); typedef void (*F_free)(void* p);
    inline void* _alloc(_s_ll nbytes) __bmdx_noex { if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { return _carray_tu_alloc_t<T>::_sf_calloc(nbytes); } return ((F_alloc)_pff[1])(nbytes); }
    inline void _free(void* p) __bmdx_noex { if (!p) { return; } if (_psf1 && _psf1 == _carray_tu_alloc_t<T>::_s_psf1()) { _carray_tu_alloc_t<T>::_sf_free(p); return; } ((F_free)_pff[2])(p); }
    inline void _destroy1(T* pd_) __bmdx_noex { (void)pd_; try { pd_->~T(); } catch (...) {} }
    inline column* _vnncol_alloc(_s_ll m) __bmdx_noex { return (column*)_alloc(sizeof(column) + m * sizeof(T)); }

    void _pop_range(_s_ll i0, _s_ll i2) // constraints: i0 == _ipop, i2 <= _ipush
    {
      _s_ll ir = i0 % _m;
      _s_ll nccurr = _cap_push; nccurr += _cap_pop;
      const _s_ll ncmin = _capmin >= 0 ? 2 + _capmin / _m : 2;
      while (i0 < i2)
      {
        _destroy1(_cpop->data() + ir);
        ++i0;
        ++ir;
        if (ir < _m) { _ipop = i0; continue; }
        ir = 0;
        if (nccurr - 1 >= ncmin) { nccurr -= 1; _cap_pop -= 1; column* cf = _cpop; _cpop = cf->_next; _cpop->_prev = _cfree_last; __bmdx_vnncol_free(cf); }
        else
        {
            // NOTE the free columns are single-linked list: _prev are valid pointers, _next contains unique serial number of free column occurrence
          column* cnx = _cpop->_next;
          const _s_ll inew = _ilnknew + 1;
          _cpop->_prev = _cfree_last;
          _cpop->_next = (column*)(__bmdx_null_pchar + inew);
          _cfree_last = _cpop;
          _ilnknew = inew;
          _cpop = cnx;
        }
          _ipop = i0;
      }
    }

      // Reusable part of _push_1.
      //  1) Links all columns that are currently free, to the end of _cpush's chain. This is enough to hold at least one additional element.
      //  2) If no free columns, allocates a column (2 cols. if the container is empty), to be able to hold one additional element.
    _s_long _link_free_to_cpush(bool b_ignore_limit, bool b_may_alloc)
    {
      if (!_cpush) { if (!b_may_alloc) { return 0; } return _set_rsv_initial(1); }
      column* cend = _cpush;
      while (cend->_next) { cend = cend->_next; }
      _s_ll inew = _ilnknew; // written (by pop side) after changing _cfree_last, read (by push side) before reading _cfree_last
      column* const cfl = _cfree_last;
      if (inew == _ilnkused && cfl == _cfree_prv)
      {
        if (!b_may_alloc) { return 0; }
        if (!b_ignore_limit)
        {
          if (_capmax == -1) { return -1; }
          if (_capmax >= 0) { if (ncapp() - _capmax >= _m) { return -1; } }
        }
        column* cnew = __bmdx_vnncol_alloc(_m); if (!cnew) { return -2; }
        cnew->_prev = cend; cnew->_next = 0; cend->_next = cnew; _cap_push += 1;
        return 1;
      }
      // cfl != 0 here (but _cfree_prv may be anything)

      inew = (char*)cfl->_next - (char*)0;
      column* cf = cfl;
      if (1) { cfl->_next = 0; _s_ll i = inew; while (--i > _ilnkused) { column* c = cf->_prev; c->_next = cf; cf = c; } }
      cend->_next = cf; cf->_prev = cend; _cfree_prv = cfl; _ilnkused = inew;

      return 1;
    }

    _s_long _set_rsv_initial(_s_ll nrsv)
    {
      if (nrsv < 0) { return -1; }
      if (_cpush || _cpop) { return -2; }
      _cfree_last = _cfree_prv = 0; _ilnknew = _ilnkused = 0;
      _caps_reset();
      if (nrsv == 0) { return 1; }

      column* chead = 0; column* cf = 0; _s_ll nplaces = 0;
      while (nplaces - _m < nrsv) // NOTE one column cannot be counted as guaranteed reserve for client, because _ipush < _ipop in the same column is not allowed
      {
        column* cnew = __bmdx_vnncol_alloc(_m); if (!cnew) { break; }
        cnew->_prev = cf; cnew->_next = 0;
        if (cf) { cf->_next = cnew; } else { chead = cnew; }
        cf = cnew; nplaces += _m;
      }
      const bool bf = nplaces - _m < nrsv;
      if (bf && nplaces == _m) { __bmdx_vnncol_free(chead); chead = 0; }
      _cpush = chead; _cap_push = nplaces / _m; _cpop = chead;
      if (bf) { return -2; }
      return 1;
    }

    void _scalars_reset() { _col_scalars_reset(); _cap_limits_reset(); _caps_reset(); _ipp_reset(); }
    void _col_scalars_reset() { _cpush = _cpop = _cfree_prv = _cfree_last = 0; _ilnknew = _ilnkused = 0; }
    void _cap_limits_reset() { _capmin = 0; _capmax = -2; }
    void _ipp_reset() { _ipush = _ipop = 0; }
    void _pff_reset() { _psf1 = _carray_tu_alloc_t<T>::_s_psf1(); _pff = _carray_tu_alloc_t<T>::_spff; }
    void _cols_release() { column* cf = _cpop; while (cf) { column* cf2 = cf->_next; __bmdx_vnncol_free(cf); cf = cf2; } }
    void _caps_reset() { _cap_pop = _cap_push = 0; }
    void _caps_normalize() { _s_ll a = _cap_pop; _s_ll b = _cap_push; b += a; a = 0; _cap_push = b; _cap_pop = a; }

    template<class Fcreate>
    _s_long _push_1(const Fcreate& f)
    {
      const _s_ll na = navl();
      if (_capmax >= 0 && na >= _capmax) { return -1; }
      const _s_ll ir = _ipush % _m;
      const bool b_lastpos = ir + 1 == _m;
      if (!_cpush || (b_lastpos && !_cpush->_next)) { _s_long res = _link_free_to_cpush(0, 1); if (res != 1) { return res; } }
      try { f(_cpush->data() + ir); } catch (...) { return -3; }
      if (b_lastpos) { _cpush = _cpush->_next; }
      _ipush += 1;
      return 1;
    }

    _s_long _incr_rsv(_s_ll ndelta)
    {
      if (ndelta < 0) { return -1; }
      if (ndelta == 0) { return 1; }
      const _s_ll na = navl();
      if (_capmax >= 0 && na + ndelta > _capmax) { return -1; }
      const _s_ll ir = _ipush % _m;
      if (!_cpush || (ir + ndelta >= _m && !_cpush->_next)) { _s_long res = _link_free_to_cpush(0, 1); if (res != 1) { return res; } }
      ndelta -= (_m - ir); if (ndelta < 0) { return 1; }
      column* cf = _cpush;
      while (1)
      {
        column* cnx = cf->_next;
        if (!cnx) { if (_capmax == -1) { return -2; } cnx = __bmdx_vnncol_alloc(_m); if (!cnx) { return -2; } cnx->_prev = cf; cnx->_next = 0; cf->_next = cnx; _cap_push += 1; }
        ndelta -= _m; if (ndelta < 0) { return 1; }
        cf = cnx;
      }
      return 1;
    }

    template<class F> _s_long _push_n(const F& f, _s_ll n2)
    {
      if (n2 < 0) { return -1; }
      if (n2 == 0) { return 1; }
      _s_long res = _incr_rsv(n2); if (res != 1) { return res; }
      const _s_ll ir = _ipush % _m;
      column* cf = _cpush; _s_ll npushed = 0, ir2 = ir;
      for (; npushed < n2; ++npushed) { try { f(cf->data() + ir2); } catch (...) { break; } ir2 += 1; if (ir2 >= _m) { cf = cf->_next; ir2 = 0; } }
      if (npushed < n2) { for (_s_ll i = 0; i < npushed; ++i) { ir2 -= 1; if (ir2 < 0) { cf = cf->_prev; ir2 = _m - 1; } _destroy1(cf->data() + ir2); } return -3; }
      _cpush = cf; _ipush += n2;
      return 1;
    }

    column * volatile _cpush, * volatile _cfree_prv, * volatile _cfree_last, * volatile _cpop; // on _cpush == 0 (by dflt. construction or set_rsv(0)), all others are also kept 0
    void* _psf1; void** _pff;
    _s_ll _m; // column size (>=1)
    volatile _s_ll _cap_pop, _cap_push; // the current real allocated place == _m * (_cap_pop + _cap_push), >= 0; NOTE one column cannot be counted as guaranteed reserve for client, because reaching _ipush < _ipop in the same column is not allowed
    _s_ll _capmin, _capmax; // min. capacity to which auto-shrinking is allowed (-1: no shrinking allowed); max. capacity to which auto-growth is allowed (-2: no limit, -1: no growth allowed)
    volatile _s_ll _ipush, _ipop; // the next push position, if valid: _cpush->data()[_ipush % _m]; the next pop position, if valid: _cpop->data()[_ipop % _m]
    volatile _s_ll _ilnknew; _s_ll _ilnkused;
  };
  template<class T, bool b> typename vnnqueue_t<T>::template iterator_t<b> operator+(typename vnnqueue_t<T>::template iterator_t<b>::difference_type delta, const typename vnnqueue_t<T>::template iterator_t<b>& x) __bmdx_noex { typename vnnqueue_t<T>::template iterator_t<b> i(x); i += delta; return i; }

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
static void _threadctl_thproc_impl(void* _p) __bmdx_noex;
static DWORD WINAPI _threadctl_thproc(LPVOID _p) __bmdx_noex    { _threadctl_thproc_impl(_p); return 1; }
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
  static void csdata_init(_critsec_data0_t<T>* p) __bmdx_noex { if (p) { p->psm = sm; p->cnt = 0; p->cnt2 = 0; p->tid = 0; } }
  static void csdata_deinit(_critsec_data0_t<T>*) __bmdx_noex { }
  static void cs_init(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags, _s_ll check_period_mcs, _s_long timeout_ms) __bmdx_noex
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
  static void cs_deinit(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags) __bmdx_noex
  {
    if (!pflags) { return; }
    if (!!pflags->_bl && _p) { if (--_p->cnt2 == 0) { _p->tid = 0; InterlockedDecrement(&_p->cnt); } pflags->_bl = 0; }
  }
  static _critsec_data0_t<T>* cs_pdefcsd() __bmdx_noex { return &_critsec_tu_static_t<T>::dat; }

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
  static void _tid_set(_threadctl_tid_data& x, const _threadctl_tid_data::t_native_tid& nat_id0) __bmdx_noex
  {
    _threadctl_tid_data::t_native_tid nat_id = nat_id0;
    x.__pad1 = 0;
    x.psm = sm;
    char* p = (char*)&x.stg1;
    const char* p0 = (const char*)&nat_id;
    x.stg1 = 1; bool b = *p == 1; x.stg1 = 0;
    x.stg2 = 0;
    const int n = sizeof(nat_id) <= 16 ? sizeof(nat_id) : 16;
    if (b || n >= 8) { bmdx_str::words::memmove_t<char>::sf_memcpy(p, p0, n); }
      else { bmdx_str::words::memmove_t<char>::sf_memcpy(p + 8 - n, p0, n); }
  }
  static _s_long tid_is_eq(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2) __bmdx_noex
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
  static _s_long th_state(_threadctl_ctx_data* p) __bmdx_noex
  {
    if (!p) { return 0; }
    DWORD c(0); GetExitCodeThread(p->th, &c);
    return c == STILL_ACTIVE ? 2 : 1;
  }
    // ret. 1, 0
  static _s_long th_start(_threadctl_ctx_data* p, _threadctl_tid_data* pret_tidobj) __bmdx_noex
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
  static _s_long th_set_priority(_threadctl_ctx_data* p, _s_long prio) __bmdx_noex
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
  static _s_long th_stop(_threadctl_ctx_data* p, _s_long timeout_ms) __bmdx_noex
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
  static _s_long th_terminate(_threadctl_ctx_data* p, _s_long flags) __bmdx_noex
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
  static _s_long th_in_ctl_incr(_threadctl_ctx_data* p) __bmdx_noex;

  static _s_long th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) __bmdx_noex;
  static _s_long th_ctx_release(_threadctl_ctx_data* p, _s_long what) __bmdx_noex;
  static _s_long _th_ctx_rel_handle(_threadctl_ctx_data* p) __bmdx_noex { if (!p) { return 1; } return CloseHandle(p->th) != 0; }

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
    #if !defined(__FreeBSD__) || (__FreeBSD__ >= 12)
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
    #else
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
    #endif
  }




typedef pthread_t _t_threadctl_nat_handle;
static void _threadctl_thproc_impl(void* _p) __bmdx_noex;
extern "C" typedef void* (*PF_threadctl_thproc)(void* _p);
static void* _threadctl_thproc(void* _p) __bmdx_noex { _threadctl_thproc_impl(_p); return 0; }
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
      if (pe) { bmdx_str::words::memmove_t<char>::sf_memcpy(&x, &x2, sizeof(_u)); }
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
  static void csdata_init(_critsec_data0_t<T>* p) __bmdx_noex
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
  static void csdata_deinit(_critsec_data0_t<T>* p) __bmdx_noex
  {
    if (!p) { return; }
    if (p->_kind == 2) { pthread_mutex_destroy(&p->_m); }
  }
  static void cs_init(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags, _s_ll check_period_mcs, _s_long timeout_ms) __bmdx_noex
  {
    if (!pflags) { return; }
    pflags->_tms = timeout_ms; pflags->_bl = 0;  pflags->_bignore = 0; pflags->_bl = 0; pflags->_brec = 0;
    if (check_period_mcs < 0) { pflags->_bignore = 1; return; }
    if (!(_p && _p->psm)) { return; }
    _critsec_data1_t<T>* _pg_dat = (_critsec_data1_t<T>*)_p->psm(0); if (!_pg_dat) { return; }
    if (_p == &_pg_dat->_trecur) { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (++_pg_dat->_cnt == 1) { csdata_init(&_pg_dat->_trecur); } pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    if (_p->_kind == 1) { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (_p->_b_lk1 && pthread_equal((*(pthread_t*)&_p->_tid), pthread_self()) == 0) { pflags->_bl = 1; pflags->_brec = 1; } pthread_mutex_unlock(&_pg_dat->_tglobal._m); if (pflags->_brec) { return; } }
    double t0(0.); bool b(false);
    while (pthread_mutex_trylock(&_p->_m) != 0)
    {
      if (timeout_ms >= 0) { if (!b) { t0 = clock_ms(); b = true; } double dt = clock_ms() - t0; if (dt < 0 || dt >= double(timeout_ms)) { return; } }
      sleep_mcs(check_period_mcs);
    }
    pflags->_bl = 1; if (_p->_kind == 1) { new (&_p->_tid) pthread_t(pthread_self()); _p->_b_lk1 = 1; }
  }
  static void cs_deinit(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags) __bmdx_noex
  {
    if (!pflags) { return; }
    if (!!pflags->_bignore || !_p) { return; }
    _critsec_data1_t<T>* _pg_dat = (_critsec_data1_t<T>*)_p->psm(0); if (!_pg_dat) { return; }
    if (_p->_kind == 1) { if (pflags->_brec) { return; } pthread_mutex_lock(&_pg_dat->_tglobal._m); ((pthread_t*)&_p->_tid)->~pthread_t(); _p->_b_lk1 = 0; pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    if (pflags->_bl) { pthread_mutex_unlock(&_p->_m); pflags->_bl = 0; }
    if (_p == &_pg_dat->_trecur) { pthread_mutex_lock(&_pg_dat->_tglobal._m); if (--_pg_dat->_cnt == 0) { csdata_deinit(&_pg_dat->_trecur); } pthread_mutex_unlock(&_pg_dat->_tglobal._m); }
    pflags->_bignore = 1;
  }
  static _critsec_data0_t<T>* cs_pdefcsd() __bmdx_noex { return &_critsec_tu_static_t<T>::dat._trecur; }

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
  static void _tid_set(_threadctl_tid_data& x, const _threadctl_tid_data::t_native_tid& nat_id0) __bmdx_noex
  {
    _threadctl_tid_data::t_native_tid nat_id = nat_id0;
    x.__pad1 = 0;
    x.psm = sm;
    char* p = (char*)&x.stg1;
    const char* p0 = (const char*)&nat_id;
    x.stg1 = 1; bool b = *p == 1; x.stg1 = 0;
    x.stg2 = 0;
    const int n = sizeof(nat_id) <= 16 ? sizeof(nat_id) : 16;
    if (b || n >= 8) { bmdx_str::words::memmove_t<char>::sf_memcpy(p, p0, n); }
      else { bmdx_str::words::memmove_t<char>::sf_memcpy(p + 8 - n, p0, n); }
  }
  static _s_long tid_is_eq(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2) __bmdx_noex
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
    if (b || n >= 8) { bmdx_str::words::memmove_t<char>::sf_memcpy(&t1, &p_my->stg1, n); bmdx_str::words::memmove_t<char>::sf_memcpy(&t2, &p_my->stg2, n); }
      else { bmdx_str::words::memmove_t<char>::sf_memcpy(&t1, (char*)&p_my->stg1 + 8 - n, n); bmdx_str::words::memmove_t<char>::sf_memcpy(&t2, (char*)&p_my->stg2 + 8 - n, n); }

    return pthread_equal(t1, t2) == 0;
  }


    // ret. 2..0
  static _s_long th_state(_threadctl_ctx_data* p) __bmdx_noex
  {
    if (!p) { return 0; }
    return p->in_thread ? 2 : 1;
  }
    // ret. 1, 0
  static _s_long th_start(_threadctl_ctx_data* p, _threadctl_tid_data* pret_tidobj) __bmdx_noex
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
  static _s_long th_set_priority(_threadctl_ctx_data* p, _s_long prio) __bmdx_noex
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
  static _s_long th_stop(_threadctl_ctx_data* p, _s_long timeout_ms) __bmdx_noex
  {
    if (!p) { return 0; }
    if (!p->bs && (timeout_ms >= -1 || timeout_ms <= -4 || (timeout_ms == -2 && p->in_ctl <= 1))) { p->bs = 1; }
    if (!p->in_thread) { _threadctl_tu_static_t<_>::th_ctx_release(p, 2); return 1; }
    if (timeout_ms < 0) { _threadctl_tu_static_t<_>::th_ctx_release(p, 2); return -1; }
    _s_long t = timeout_ms;
    while (t >= 0) { sleep_mcs(t > 10 ? 10000 : t * 1000); if (!p->in_thread) { th_ctx_release(p, 2); return 2; } t -= 10; }
    return -2;
  }

  static _s_long th_terminate(_threadctl_ctx_data* p, _s_long flags) __bmdx_noex;

  static _s_long th_in_ctl_incr(_threadctl_ctx_data* p) __bmdx_noex;

  static _s_long th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) __bmdx_noex;
  static _s_long th_ctx_release(_threadctl_ctx_data* p, _s_long what) __bmdx_noex;
  static _s_long _th_ctx_rel_handle(_threadctl_ctx_data*) __bmdx_noex { return 1; }

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
      (void*)(__bmdx_null_pchar + -942002180), // 16 // cmti_base_t<int,2017,7,11,20,4>::ind()
      (void*)(__bmdx_null_pchar + sizeof(_threadctl_tid_data::t_native_tid)), // 17
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
    critsec_t(_s_ll check_period_mcs, _s_long timeout_ms, _critsec_data0_t<T>* pdata = 0 __bmdx_noarg) __bmdx_noex
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
      csdata(__bmdx_noarg1) __bmdx_noex { _critsec_tu_static_t<T>::csdata_init(this); }
      ~csdata() __bmdx_noex { if (this->psm) { typedef void (*PF)(_critsec_data0_t<T>*); try { PF f = (PF)this->psm(2); if (f) { f(this); } } catch (...) {} } }
    private: csdata(const csdata&); void operator=(const csdata&);
    };

      // Critical section data object, referenced by the current lock.
      //  The returned pointer is non-zero, referring to the valid object,
      //  provided that critsec_t had been constructed correctly.
    _critsec_data0_t<T>* pcsd0() const { return _p; }
    csdata* pcsd() const { return static_cast<csdata*>(_p); }

    struct ff_mc
    {
        // Default csdata object, used by critsec_t for type T (per binary module or per compiler).
      inline csdata* pdefcsd() __bmdx_noex { typedef _critsec_data0_t<T>* (*PF)(); PF f = (PF)_critsec_tu_static_t<T>::sm(5); return static_cast<csdata*>(f()); }
    };


      // Return value: a) true, b) false - lock tries were timed out (may occur ONLY on constructor timeout_ms >= 0).
    inline bool is_locked() const __bmdx_noex { return !!_flags._bl; }
    inline operator bool() const __bmdx_noex { return !!_flags._bl; }


    ~critsec_t() __bmdx_noex
    {
      if (!(_p && _p->psm)) { return; }
      typedef void (*PF)(_critsec_data0_t<T>* _p, typename _critsec_data0_t<T>::t_flags* pflags);
      try { PF f = (PF)_p->psm(4); if (f) { f(_p, &_flags); } } catch (...) {}
    }

    critsec_t(const critsec_t& x __bmdx_noarg) { new (this) critsec_t(x._flags._bl ? 10 : -1, x._flags._tms, x._p); }
    critsec_t& operator=(const critsec_t& x) { critsec_t cs3(x); bmdx_str::words::swap_bytes(*this, cs3); return *this; }

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
        //  From thread side, b_stop() may be set to 1 with associated threadctl's signal_stop(),
        //    or to any value via its *pflag().
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
      friend void _threadctl_thproc_impl(void* _p) __bmdx_noex;
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
      tid() __bmdx_noex    { clear(); }
      ~tid() __bmdx_noex    { }
      void clear() __bmdx_noex    { _dat.psm = _threadctl_tu_static_t<>::sm; _dat.stg1 = 0; _dat.stg2 = 0; }

      tid(const t_native_tid& x_ __bmdx_noarg) __bmdx_noex    { _threadctl_tu_static_t<>::_tid_set(_dat, x_); }
      tid(const tid& x_) __bmdx_noex    { _dat = x_._dat; }
      tid& operator=(const t_native_tid& x_) __bmdx_noex    { _threadctl_tu_static_t<>::_tid_set(_dat, x_); return *this; }
      tid& operator=(const tid& x_) __bmdx_noex    { _dat = x_._dat; return *this; }

        // Comparison is not trivial.
      bool operator==(const tid& x_) const __bmdx_noex { if (!_dat.psm) { return !x_._dat.psm; } if (!x_._dat.psm) { return false; } typedef _s_long (*Peq)(const _threadctl_tid_data* p_my, const _threadctl_tid_data* p2); try { Peq f_eq = (Peq)_dat.psm(19); if (!f_eq) { return false; } return !!f_eq(&_dat, &x_._dat); } catch (...) {} return false; }
      bool operator!=(const tid& x_) const __bmdx_noex { return !this->operator==(x_); }

        // If tid object is valid in the current context, returns ptr. to native handle, otherwise 0.
      t_native_tid* pnat_id(__bmdx_noarg1) const __bmdx_noex
      {
        if (!_dat.psm) { return 0; }
        const int n = sizeof(_threadctl_tid_data::t_native_tid);
        if (n > 16) { return 0; }
        try {
          _s_ll t_my = (_s_ll)((char*)_threadctl_tu_static_t<>::sm(16) - (char*)0); _s_ll t = (_s_ll)((char*)_dat.psm(16) - (char*)0); _s_ll s = (_s_ll)((char*)_dat.psm(17) - (char*)0);
          if (t != t_my) { return 0; } if (s != n) { return 0; }
        } catch (...) { return 0; }
        _s_ll x = 1; bool b = *(char*)&x == 1;
        if (b || n >= 8) { return (t_native_tid*)&_dat.stg1; }
          else { return (t_native_tid*)((char*)&_dat.stg1 + 8 - n); }
      }

      bool is_null() const __bmdx_noex    { return _dat.stg1 == 0 && _dat.stg2 == 0; }
      operator bool() const __bmdx_noex    { return is_null(); }

        // Numeric value of tid. 0 if tid is null.
        //  The value is most suitable for displaying.
      _s_ll num_id() const __bmdx_noex    { if (is_null()) { return 0; } _s_ll n = _dat.stg1 ^ _dat.stg2; if (!n) { n = _dat.stg1; } return n; }

    private: friend struct threadctl; _threadctl_tid_data _dat;
    };

    struct ff_mc
    {
        // Returns thread ID of the current (caller's) thread.
      tid tid_self() __bmdx_noex
      {
        #ifdef _bmdxpl_Wnds
          return GetCurrentThreadId();
        #endif
        #ifdef _bmdxpl_Psx
          return pthread_self();
        #endif
      }
    };


    threadctl(__bmdx_noarg1) __bmdx_noex : _tid() { __pad1 = 0; _pctx = 0; }
    ~threadctl() __bmdx_noex { clear(); }

      // If threadctl fails to increment ref. cnt, the new object will have state() == 0 and th_id().is_null() == true.
      //  Normally, this occurs only if x is empty.
    threadctl(const threadctl& x __bmdx_noarg) __bmdx_noex
      {
        __pad1 = 0; _pctx = 0; if (!x._pctx) { return; }
        typedef _s_long (*Pincr)(_threadctl_ctx_data* p); if (!x._pctx->__dat.pthsm) { return; }
        try { Pincr f_incr = (Pincr)x._pctx->__dat.pthsm(13); if (!f_incr) { return; } f_incr(&x._pctx->__dat); } catch (...) { return; }
        _pctx = x._pctx;
        _tid = x._tid;
      }
    threadctl& operator=(const threadctl& x)    { threadctl x3(x); swap(x3); return *this; }

      // !is_null() when thread is running.
    const tid& th_id() const __bmdx_noex { return _tid; }


      // 2 - context is referenced, thread is running (volatile state!).
      // 1 - context is referenced, thread has already exited.
      // 0 - no context, thread state is unknown (e.g. the initial value, or threadctl has been cleared).
    _s_long state() const __bmdx_noex
    {
      if (!_pctx) { return 0; }
      typedef _s_long (*Pstate)(_threadctl_ctx_data* p); Pstate f_state = (Pstate)_pctx->__dat.pthsm(8);
      if (!f_state) { return 0; }
      try { return f_state(&_pctx->__dat); } catch (...) {}
      return 0;
    }
    operator bool() const __bmdx_noex { return state() == 2; }

    ctx_base* pctx() const __bmdx_noex { return _pctx; }

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
    bool start(ctx_base* p __bmdx_noarg) __bmdx_noex
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
      //    starts thread with context's _thread_proc().
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
    bool start_auto(const Data& d = Data() __bmdx_noarg) __bmdx_noex
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

      // Sets thread priority in relation to process priority class.
      // Allowed values:
      //  4 - normal priority (same as assigned by default),
      //  5 - above normal,
      //  6 - high,
      //  7 - time-critical,
      //  3 - below normal,
      //  2 - low,
      //  1 - background.
    bool set_priority(_s_long p) __bmdx_noex
    {
      if (!_pctx) { return false; }
      typedef _s_long (*Pset_priority)(_threadctl_ctx_data* p, _s_long prio); Pset_priority f_set_priority = (Pset_priority)_pctx->__dat.pthsm(10);
      if (!f_set_priority) { return false; }
      bool b = false;
      try { b = !! f_set_priority(&_pctx->__dat, p); } catch (...) {}
      return b;
    }

      // Sets b_stop() flag of the target thread to 1 (or to given v), only in case if the current flag value is 0.
      // Returns:
      //    true - successful, the client may wait until state() becomes != 2.
      //    false - could not set the flag, threadctl has no context. state() will return 0.
    bool signal_stop(_s_long v = 1) __bmdx_noex { if (!_pctx) { return false; } if (!_pctx->__dat.bs) { _pctx->__dat.bs = v; } return true; }

      // Returns a pointer to 4-byte flag variable available for r/w inside thread procedure.
      //    By default, signal_stop() sets the variable to 1 to ask thread to exit.
      //    pflag() allows to use the flag in any other way.
    volatile _s_long* pflag() const __bmdx_noex { if (!_pctx) { return 0; } return &_pctx->__dat.bs; }

      // 1) If this threadctl is the last one referencing the thread, set b_stop() flag to 1 if it was 0.
      // 2) Detach from the current thread object.
    void clear() __bmdx_noex { stop(-2); _pctx = 0; _tid.clear(); }

      // Detach from the current thread object.
    void detach() __bmdx_noex { stop(-3); _pctx = 0; _tid.clear(); }

      // Sets pctx()->b_stop() flag to 1 (only if it was 0), and possibly waits (depending on timeout) for thread exiting.
      //    If thread had exited, detaches from its context
      //    (which is automatically destroyed if this was the last reference).
      // timeout_ms:
      //    >0 - sleeps approx.  (timeout_ms / 10) times * 10 ms,
      //      checking thread state and returns on timeout or thread having exited.
      //    0 - yields once (sleep(0)), then checks thread state and returns.
      //    -1 - Sets b_stop() to 1 if it was 0. Detaches from thread context.
      //    -2 - Sets b_stop() to 1 if it was 0 - only if the current threadctl is the last one
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
    int stop(_s_long timeout_ms) __bmdx_noex
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
      //    1 - QueueUserAPC --> release thread context --> ExitThread --> system release: stack area, thread object.
      //          1) Thread context and data objects, passed via start_auto, ARE released.
      //          2) Any dynamic allocations and resources acquired, except the above, are NOT released.
      //    2 (ignored if 1 is set) - TerminateThread --> system release: stack area, thread object.
      //          All resources, allocated by the thread,
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
      //    -3 - failure (flags specify no valid method, or by other reason). threadctl is not modified.
    int terminate(_s_long flags = 0x101) __bmdx_noex
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
  _threadctl_thproc_cleanup(void* p_) __bmdx_noex : _p(p_) {}
  ~_threadctl_thproc_cleanup() __bmdx_noex { exec(); }
  void exec() __bmdx_noex
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
_s_long _threadctl_tu_static_t<_>::th_ctx_release(_threadctl_ctx_data* p, _s_long what) __bmdx_noex
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
_s_long _threadctl_tu_static_t<_>::th_in_ctl_incr(_threadctl_ctx_data* p) __bmdx_noex
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

      bool has_ref() const __bmdx_noex { return !!_hp; }
      operator bool() const __bmdx_noex { return !!_hp; }

        //  NOTE (Platform-dependent: This function is not protected against PID reuse.)
      bool is_running() const __bmdx_noex
      {
        if (!has_ref()) { return false; }
        DWORD res = WaitForSingleObject(_hp, 0);
        return res == WAIT_TIMEOUT;
      }

        // On has_ref() == true, pid() has meaningful value.
      typedef DWORD t_pid;
      t_pid pid() const __bmdx_noex { return _pid; }

        // OS-dependent handles (not cross-platform).
      HANDLE _phandle() const __bmdx_noex { return _hp; }
      HANDLE _thandle() const __bmdx_noex { return _ht; }

      processctl() __bmdx_noex : _hp(0), _ht(0), _pid(0), _exited(false) {}
      ~processctl() __bmdx_noex { clear(); }

      processctl(const processctl& x) __bmdx_noex : _hp(0), _ht(0), _pid(0), _exited(false)
      {
        if (x._hp) { DuplicateHandle(GetCurrentProcess(), x._hp, GetCurrentProcess(), &_hp, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        if (x._ht) { DuplicateHandle(GetCurrentProcess(), x._ht, GetCurrentProcess(), &_ht, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        _pid = x._pid; _exited = x._exited;
      }
      processctl& operator=(const processctl& x) { if (this == &x) { return *this; } clear(); new (this) processctl(x); return *this; }

      void clear() __bmdx_noex { _pid = 0; _exited = false; if (_hp) { CloseHandle(_hp); _hp = 0; } if (_ht) { CloseHandle(_ht); _ht = 0; } }

        // Creates new process and returns immediately.
        //
        // fnp_process with b_shell == false: must be full path and executable name (for CreateProcessA()).
        //    fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        //
        // args: program arguments, separated with null character ('\0').
        //    Arguments must not include executable name (i.e. first of args is equivalent to argv[1]).
        //    Each argument will be pre-processed with arg1(),
        //    so the client must not do any special processing (escaping etc.).
        //    NOTE Empty args is treated as no arguments. There's no way to pass exactly one zero-length argument.
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
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false) __bmdx_noex
      { try {
        clear();
        if (fnp_process.empty()) { return false; }

        std::string a2; size_t pos = 0, pos2 = 0, end = std::string::npos;
        while (pos < args.length())
        {
          pos2 = args.find('\0', pos);
          if (pos2 == end) { pos2 = args.length(); }
          if (pos > 0) { a2 += ' '; } a2 += ff_mc().arg1(args.substr(pos, pos2 - pos), b_shell);
          if (pos2 + 1 == args.length()) { a2 += ' '; a2 += ff_mc().arg1(std::string(), b_shell); }
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
      int wait_exit(long* ret_pec = 0) __bmdx_noex
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

      struct ff_mc
      {
        t_pid pid_self() __bmdx_noex { return t_pid(GetCurrentProcessId()); }

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
          if (!b && !(b_shell && s.empty())) { return s; }

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
        // b_enabled_ == false makes all in console_io a no-op.
      console_io(bool b_enabled_ = true) { _b_enabled = b_enabled_; }

      unsigned int ugetch(unsigned int no_char = 0) __bmdx_noex
      {
        if (!b_enabled()) { return no_char; }
        critsec_t<console_io> __lock(50,-1); if (sizeof(__lock)) {}
        #if defined(__BORLANDC__)
          return kbhit() ? getch() : no_char;
        #else
          return _kbhit() ? _getch() : no_char;
        #endif
      }

      bool b_enabled() const { return _b_enabled; }
    private:
      bool _b_enabled;
    };


static void _threadctl_thproc_impl(void* _p) __bmdx_noex
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
_s_long _threadctl_tu_static_t<_>::th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) __bmdx_noex
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

      bool has_ref() const __bmdx_noex { return _pid > 0; }
      operator bool() const __bmdx_noex { return _pid > 0; }

        //  NOTE This function is not protected against PID reuse.
      bool is_running() const __bmdx_noex
      {
        if (!has_ref()) { return false; }
        int res = kill(pid_t(_pid), 0);
        return res == 0;
      }

        // On has_ref() == true, pid() has meaningful value.
      typedef pid_t t_pid;
      t_pid pid() const __bmdx_noex { return t_pid(_pid); }

      processctl() __bmdx_noex : _pid(-1) {}
      ~processctl() __bmdx_noex { clear(); }

      void clear() __bmdx_noex { _pid = -1; if (_processctl_has_sigchld()) { _processctl_handler_sigchld(0, 0, 0); } }

        // Creates new process and returns immediately.
        //
        // fnp_process with b_shell == false: must be full path and executable name (for posix_spawn()).
        //    fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        //
        // args: program arguments, separated with null character ('\0').
        //    Arguments must not include executable name (i.e. first of args is equivalent to argv[1]).
        //    Each argument will be pre-processed with arg1() as necessary,
        //    so the client must not do any special processing (escaping etc.).
        //    NOTE Empty args is treated as no arguments. There's no way to pass exactly one zero-length argument.
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
        #if __APPLE__ && __MACH__ && TARGET_OS_IPHONE
          #define bmdx_processctl_allowshell 0
        #else
          #define bmdx_processctl_allowshell 1
        #endif
      #endif
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false) __bmdx_noex
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
              if (pos > 0) { a2 += ' '; } a2 += ff_mc().arg1(args.substr(pos, pos2 - pos), b_shell);
              if (pos2 + 1 == args.length()) { a2 += ' '; a2 += ff_mc().arg1(std::string(), b_shell); }
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
      int wait_exit(long* ret_pec = 0) __bmdx_noex
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

      struct ff_mc
      {
        t_pid pid_self() __bmdx_noex { return t_pid(getpid()); }

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
          if (!b && !(b_shell && s.empty())) { return s; }

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
        // b_enabled_ == false makes all in console_io a no-op.
        //  NOTE Using console_io in the program running as co-process, detached from console input,
        //    may be unsafe, regardless of all internal checks.
        //    It's recommended to disable console_io in co-processes.
      console_io(bool b_enabled_ = true) __bmdx_noex { static termios t; static _s_long n(0); __pt = &t; __pnr = &n; _b_en = b_enabled_; _set_unbuf(true); }
      ~console_io() __bmdx_noex { _set_unbuf(false); }

      unsigned int ugetch(unsigned int no_char = 0) __bmdx_noex
      {
        if (!b_enabled()) { return no_char; }
        critsec_t<console_io> __lock(50,-1); if (sizeof(__lock)) {}
        int f0 = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, f0 | O_NONBLOCK);
          int ch = std::getchar();
        fcntl(STDIN_FILENO, F_SETFL, f0);
        if (ch != EOF) { return ch; } else { return no_char; }
      }

        // Returns true if console input is enabled and valid (working).
      bool b_enabled() const { return _b_en && *__pnr > 0; }

    private:
      termios* __pt; _s_long* __pnr; bool _b_en;
      void _set_unbuf(bool b_on) __bmdx_noex
      {
        if (!_b_en) { return; }
        critsec_t<console_io> __lock(50,-1); if (sizeof(__lock)) {}
        termios& t0 = *__pt; _s_long n = *__pnr;
          if (n < 0) { return; }
        bool b_valid = getpgrp() == tcgetpgrp(STDIN_FILENO);
        if (!b_valid) { *__pnr = n = -1; _b_en = false; }
          if (n < 0) { return; }
        if (b_on)
        {
          int res = 0;
          n += 1;
          if (n == 1)
          {
            res = tcgetattr(STDIN_FILENO, &t0);
            termios t1 = t0;
            if (res == 0)
            {
              t1.c_lflag &= ~(ICANON | ECHO);
              t1.c_cc[VMIN] = 1; t1.c_cc[VTIME] = 0;
              res = tcsetattr(STDIN_FILENO, TCSANOW, &t1);
            }
          }
          if (res == 0) { *__pnr = n; }
            else { _b_en = false; }
        }
        else
        {
          n -= 1;
          if (n == 0) { tcsetattr(STDIN_FILENO, TCSANOW, &t0); }
          *__pnr = n;
        }
      }
    };


static void _threadctl_thproc_impl(void* _p) __bmdx_noex
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
_s_long _threadctl_tu_static_t<_>::th_ctx_init(_threadctl_ctx_data* p, void* pctxbase, void* pthdata) __bmdx_noex
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
_s_long _threadctl_tu_static_t<_>::th_terminate(_threadctl_ctx_data* p, _s_long flags) __bmdx_noex
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

    template<class T, class L> struct vecm::spec<bmdx::cref_t<T, L> > { typedef bmdx::cref_t<T, L> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };

    template<class T, bool b> struct vecm::spec<bmdx::carray_t<T, b> > { typedef bmdx::carray_t<T, b> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T, bool b, class _bs> struct vecm::spec<bmdx::cpparray_t<T, b, _bs> > { typedef bmdx::cpparray_t<T, b, _bs> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T, bool b> struct vecm::spec<bmdx::carray_r_t<T, b> > { typedef bmdx::carray_r_t<T, b> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T> struct vecm::spec<bmdx::arrayref_t<T> > { typedef bmdx::arrayref_t<T> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T> struct vecm::spec<bmdx::cringbuf1_t<T> > { typedef bmdx::cringbuf1_t<T> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T, class _bs> struct vecm::spec<bmdx::cppringbuf1_t<T, _bs> > { typedef bmdx::cppringbuf1_t<T, _bs> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
    template<class T, class _bs> struct vecm::spec<bmdx::vnnqueue_t<T, _bs> > { typedef bmdx::vnnqueue_t<T, _bs> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
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
  #define __bmdx_std_mkdir_arg2
  #define __bmdx_std_access _access
#endif
#ifdef _bmdxpl_Psx
  #define __bmdx_std_getcwd getcwd
  #define __bmdx_std_mkdir mkdir
  #define __bmdx_std_mkdir_arg2 , 0777
  #define __bmdx_std_access access
#endif
namespace bmdx
{

  template<class T, class LockSelector> struct cref_t;

  template<class Selector> struct _cref_lock_t : critsec_t<Selector>
  {
    typedef critsec_t<Selector> t_impl;

      // b_wait true - block until the lock is acquired,
      //  false - try lock once and immediately exit (use operator bool and/or try_lock after that).
    _cref_lock_t(bool b_wait, _critsec_data0_t<Selector>* pcsd)        : t_impl(-1, 0, pcsd) { try_lock(b_wait); }
      //
      // Create the lock object only, do not try to lock until try_lock() call.
    _cref_lock_t(_critsec_data0_t<Selector>* pcsd)        : t_impl(-1, 0, pcsd) { }
      //
      // This constructor ensures that cref_t object at the address &x
      //  is locked
      //  even in conditions when it may be concurrently overwritten
      //  with cref_t object from this or another binary module.
      //  The client may not use concurrency on x if it is not lockable or the lock is disabled:
      //    a) LockSelector == cref_nonlock statically disables locking on x (see _cref_lock_t<cref_nonlock>).
      //    b) Unsetting use_critsec flag in multi-functional reference (see struct iref2 :: create_any) disables x locking.
      //      cref_t::_ps (critsec. pointer) is kept 0 (only) in this case.
      //  In both cases x may not be used concurrently as is.
    template<class T> _cref_lock_t(const cref_t<T, Selector>& x)
      : t_impl(-1, 0, x._ps)
    {
      while (true)
      {
        void* ph1 = x._ph; _critsec_data0_t<Selector>* ps1 = x._ps; void* ph2 = x._ph;
          if (!ps1 && ph2 && ph2 == ph1) { break; }
        t_impl* pi = this; pi->~t_impl(); new (pi) t_impl(5, -1, ps1);
          if (x._ps == ps1) { break; } sleep_mcs(50);
      }
    }

    bool try_lock(bool b_wait)
      { if (this->is_locked()) { return true; } _critsec_data0_t<Selector>* ps = this->pcsd0(); t_impl* pi = this; pi->~t_impl(); new (pi) t_impl(5, b_wait ? -1 : 0, ps); return this->is_locked(); }

    static bool does_locking()    { return true; }
    operator bool() const        { return this->is_locked(); }
  };

    // This selector disables locking. (Anyway, such lock object returns true, as if the lock has been really set successfully.)
  struct cref_nonlock {};
  template<> struct _cref_lock_t<cref_nonlock>
  {
    typedef bmdx_meta::nothing t_impl;

    _cref_lock_t(bool b_wait, _critsec_data0_t<cref_nonlock>* pcsd)        { (void)b_wait; (void)pcsd; }
    _cref_lock_t(_critsec_data0_t<cref_nonlock>* pcsd)        { (void)pcsd; }
    template<class T> _cref_lock_t(const cref_t<T, cref_nonlock>&)    {}

    bool try_lock()    { return true; }

    static bool does_locking()    { return false; }
    operator bool() const        { return true; }
  };

  struct _cref_t_exceptions
  {
    struct exc_ref : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::ref"; } };
    struct exc_ref_ts : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::ref_ts"; } };
    struct exc_create0 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create0"; } };
    struct exc_create1 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create1"; } };
    struct exc_create2 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create2"; } };
    struct exc_create3 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create3"; } };
    struct exc_create4 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create4"; } };
    struct exc_create5 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create5"; } };
    struct exc_create6 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create6"; } };
    struct exc_create7 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create7"; } };
    struct exc_create8 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::[cm_]create8"; } };
    struct exc_irefcr0 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create0"; } };
    struct exc_irefcr1 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create1"; } };
    struct exc_irefcr2 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create2"; } };
    struct exc_irefcr3 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create3"; } };
    struct exc_irefcr4 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create4"; } };
    struct exc_irefcr5 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create5"; } };
    struct exc_irefcr6 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create6"; } };
    struct exc_irefcr7 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create7"; } };
    struct exc_irefcr8 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref::[cm_]create8"; } };
    struct exc_assign : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::assign"; } };
    struct exc_copy : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::copy"; } };
    struct exc_cm_assign : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::cm_assign"; } };
    struct exc_cm_copy : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::cm_copy"; } };
    struct exc_cc3 : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t(x,is_own,false)"; } };
    struct exc_refcast_s_u : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref2::refcast_s_u"; } };
    struct exc_refcast_d_u : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref2::refcast_d_u"; } };
    struct exc_iref2_create_any : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref2::create_any"; } };
    struct exc_iref2_assign : std::exception { inline const char* what() const __bmdx_noex { return "bmdx::cref_t::iref2::assign"; } };
  };

  struct iref2_flags
  { enum e {
        // a) Cross-module strong reference: do not delete the object on ref. count == 0.
        //    (The flag may be used to refer to static objects. It disables "delete p" call in the original module
        //    where the object was created.)
        // b) Multi-functional strong reference: do not call handler with ev_des.
        //    Effective only in use_hst mode. Note that object memory is part of internal handler, and freed automatically.
        //    (To refer to static objects, i.e. avoid both destruction and memory freeing,
        //    as in case (a), employ use_release flag, instead of use_hst.)
      disable_des = 0x2,

        // a) Cross-module strong reference: in cm_copy only, protect the original object copying by the lock,
        //    in addition to ref. counting locks.
        //    NOTE Ref. counting locking is always enabled in this type of reference.
        // b) Multi-functional strong reference: protect all operations on the referenced and aux. objects by the lock.
        //    If flag is not set: do not use locking. In this case the reference, and objects, handled by it,
        //    may not be used concurrently.
      use_critsec = 0x4,

        // Multi-functional ref. only:
        //  1) Store the object (T) in the body of cref_t handler, together with ref. counter and other internal variables.
        //  2) Use "args(p)" call to initialize the object. ("args": 2nd argument of create_any, subclass of i_new.)
        //  3) On ref. count == 0, generate ev_des. (ev_delete, ev_release are disabled.)
        //    The client handler must destroy the object, without freeing its memory.
        // NOTE The default handler calls "~T()" on ev_des.
        // NOTE Exactly one of use_hst, use_delete, use_release may be set at once.
        // NOTE If disable_des is set, ev_des is not generated (== the client is not notified on releasing the last reference to object).
      use_hst = 0x10,

        // Multi-functional ref. only:
        //  1) The object is created with "p = args(0)" call. ("args": 2nd argument of create_any, subclass of i_new.)
        //    The impl. may use op. new or create an object in other way.
        //  2) On ref. count == 0, generate ev_delete. (ev_des, ev_release are disabled.)
        //    The client handler must delete the object or release it (destroy and free memory) as necessary.
        // NOTE The default handler calls "delete p" on ev_delete.
        // NOTE Exactly one of use_hst, use_delete, use_release may be set at once.
      use_delete = 0x20,

        // Multi-functional ref. only:
        //  1) The object pointer is obtained from customized arguments wrapper,
        //      using "p = args(0)" call. ("args": 2nd argument of create_any, subclass of i_new.)
        //      The object storage, creation and destruction is controlled by client.
        //  2) On ref. count == 0, ev_release is generated. (ev_des, ev_delete are disabled.)
        //    The client may handle or ignore this event as necessary.
        // NOTE The default handler does nothing on ev_release.
        //    This allows for referring to static objects by special arguments wrapper,
        //    as e.g. done by iref2::assign.
        //    (But, do not use default arguments wrapper from iref2_args_t + the default event handler,
        //    because the default event handler does not delete the object, dynamically created by dflt. args. wrapper.)
        // NOTE Exactly one of use_hst, use_delete, use_release may be set at once.
      use_release = 0x40,

        // Multi-functional ref. only:
        //  1) Create auxiliary object (Aux) with "args_aux(paux)" call. ("args_aux": 6th argument of create_any, subclass of i_new.)
        //    The object storage is controlled by cref_t. The client should only initialize the object.
        //    After initialization, on success, cref_t generates ev_aux_con event, which may be handled by the client.
        //  2) After the main object has been released, cref_t generates ev_aux_des event,
        //    that should be handled by the client to destroy the aux. object (~Aux()).
      use_aux = 0x1,

        // Multi-functional ref. only: do not call handler with ev_aux_des.
        //  Aux. object is not destroyed, and its memory is freed automatically by cref_t.
      disable_aux_des = 0x8,

        // Multi-functional ref. only: enable ref. count change events (ev_nrefs).
        //  ev_nrefs occurs
        //  1) after object construction (1-st ref. creation),
        //  2) after adding a reference (during cref_t copying),
        //  3) after removing a reference (during cref_t reset or overwriting).
      gen_nrefs = 0x80,

        // Multi-functional ref. only: enable pointer cast/replace events (ev_cast_*).
      gen_cast = 0x100,

      //  ======== ======== ======== ========

      ev_des = 0x100, // the handler should destroy the object (~T(), but not deallocate)
      ev_delete = 0x200, // the handler should call operator delete on the object, or release the object and free its memory in other way
      ev_release = 0x400, // the handler should do nothing, or release user object in specific way

      ev_nrefs = 0x800, // the handler is notified about ref. count change (given the last delta and the new value)

        // Notifies that aux. object has been just constructed.
        //  NOTE Main object (T) is not available at the time of this event.
        //  NOTE This events occurs only if aux. object is successfully created (on use_aux being set).
      ev_aux_con = 0x1000,
        // The handler should destroy the aux. object (~Aux(), but not free memory).
        //  NOTE Main object (T) is not available at the time of this event.
        //  NOTE This events occurs only when valid aux. object should be destroyed, unless disable_aux_des flag is set.
      ev_aux_des = 0x2000,

        // Cast events are called right after pointer casting/replacement.
      _ev_cast_mask = 0x4000, // matching any cast op.: (optype & _ev_cast_mask) == _ev_cast_mask
      ev_cast_static = 0x4000,
      ev_cast_dynamic = 0x4001,
      ev_cast_replace = 0x4002
  }; };

  template<class T, class _ = __vecm_tu_selector>
  struct iref2_args_t : iref2_flags
  {
      // a) If p != 0: initialize the given storage, e.g. new (p) T(), then return p.
      // b) If p == 0: create an object dynamically, e.g. return new T().
      // c) Special case, e.g. for use_release storage mode (see iref2::assign impl.).
      //    If p == 0: return a pointer to static or other existing object.
      // d) On any construction failure, generate an exception.
    struct i_new { virtual T* operator()(T* p) const = 0; virtual ~i_new() {} };

      // Trivial i_new impl. for constructors with different num. of args.
    struct args0_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(); return p; } else { return new T(); } } };
    template<class A1> struct args1_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1); return p; } else { return new T(a1); } } args1_t(const A1& a1_) : a1(a1_) {} const A1& a1; };
    template<class A1, class A2> struct args2_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2); return p; } else { return new T(a1, a2); } } args2_t(const A1& a1_, const A2& a2_) : a1(a1_), a2(a2_) {} const A1& a1; const A2& a2; };
    template<class A1, class A2, class A3> struct args3_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3); return p; } else { return new T(a1, a2, a3); } } args3_t(const A1& a1_, const A2& a2_, const A3& a3_) : a1(a1_), a2(a2_), a3(a3_) {} const A1& a1; const A2& a2; const A3& a3; };
    template<class A1, class A2, class A3, class A4> struct args4_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3, a4); return p; } else { return new T(a1, a2, a3, a4); } } args4_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; };
    template<class A1, class A2, class A3, class A4, class A5> struct args5_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3, a4, a5); return p; } else { return new T(a1, a2, a3, a4, a5); } } args5_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; };
    template<class A1, class A2, class A3, class A4, class A5, class A6> struct args6_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3, a4, a5, a6); return p; } else { return new T(a1, a2, a3, a4, a5, a6); } } args6_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; };
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> struct args7_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3, a4, a5, a6, a7); return p; } else { return new T(a1, a2, a3, a4, a5, a6, a7); } } args7_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_, const A7& a7_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_), a7(a7_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; const A7& a7; };
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> struct args8_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T(a1, a2, a3, a4, a5, a6, a7, a8); return p; } else { return new T(a1, a2, a3, a4, a5, a6, a7, a8); } } args8_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_, const A7& a7_, const A8& a8_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_), a7(a7_), a8(a8_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; const A7& a7; const A8& a8; };

    static args0_t args() { return args0_t(); }
    template<class A1> static args1_t<A1> args(const A1& a1) { return args1_t<A1>(a1); }
    template<class A1, class A2> static args2_t<A1, A2> args(const A1& a1, const A2& a2) { return args2_t<A1, A2>(a1, a2); }
    template<class A1, class A2, class A3> static args3_t<A1, A2, A3> args(const A1& a1, const A2& a2, const A3& a3) { return args3_t<A1, A2, A3>(a1, a2, a3); }
    template<class A1, class A2, class A3, class A4> static args4_t<A1, A2, A3, A4> args(const A1& a1, const A2& a2, const A3& a3, const A4& a4) { return args4_t<A1, A2, A3, A4>(a1, a2, a3, a4); }
    template<class A1, class A2, class A3, class A4, class A5> static args5_t<A1, A2, A3, A4, A5> args(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) { return args5_t<A1, A2, A3, A4, A5>(a1, a2, a3, a4, a5); }
    template<class A1, class A2, class A3, class A4, class A5, class A6> static args6_t<A1, A2, A3, A4, A5, A6> args(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) { return args6_t<A1, A2, A3, A4, A5, A6>(a1, a2, a3, a4, a5, a6); }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static args7_t<A1, A2, A3, A4, A5, A6, A7> args(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) { return args7_t<A1, A2, A3, A4, A5, A6, A7>(a1, a2, a3, a4, a5, a6, a7); }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static args8_t<A1, A2, A3, A4, A5, A6, A7, A8> args(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) { return args8_t<A1, A2, A3, A4, A5, A6, A7, A8>(a1, a2, a3, a4, a5, a6, a7, a8); }


          // Same as args(), but all arguments will be cast to non-const when passed to constructor by args#nc_t.
        template<class A1> struct args1nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1); return p; } else { return new T((A1&)a1); } } args1nc_t(const A1& a1_) : a1(a1_) {} const A1& a1; };
        template<class A1, class A2> struct args2nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2); return p; } else { return new T((A1&)a1, (A2&)a2); } } args2nc_t(const A1& a1_, const A2& a2_) : a1(a1_), a2(a2_) {} const A1& a1; const A2& a2; };
        template<class A1, class A2, class A3> struct args3nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3); } } args3nc_t(const A1& a1_, const A2& a2_, const A3& a3_) : a1(a1_), a2(a2_), a3(a3_) {} const A1& a1; const A2& a2; const A3& a3; };
        template<class A1, class A2, class A3, class A4> struct args4nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4); } } args4nc_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; };
        template<class A1, class A2, class A3, class A4, class A5> struct args5nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5); } } args5nc_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; };
        template<class A1, class A2, class A3, class A4, class A5, class A6> struct args6nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6); } } args6nc_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; };
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> struct args7nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6, (A7&)a7); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6, (A7&)a7); } } args7nc_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_, const A7& a7_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_), a7(a7_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; const A7& a7; };
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> struct args8nc_t : i_new { virtual T* operator()(T* p) const { if (p) { new (p) T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6, (A7&)a7, (A8&)a8); return p; } else { return new T((A1&)a1, (A2&)a2, (A3&)a3, (A4&)a4, (A5&)a5, (A6&)a6, (A7&)a7, (A8&)a8); } } args8nc_t(const A1& a1_, const A2& a2_, const A3& a3_, const A4& a4_, const A5& a5_, const A6& a6_, const A7& a7_, const A8& a8_) : a1(a1_), a2(a2_), a3(a3_), a4(a4_), a5(a5_), a6(a6_), a7(a7_), a8(a8_) {} const A1& a1; const A2& a2; const A3& a3; const A4& a4; const A5& a5; const A6& a6; const A7& a7; const A8& a8; };
        template<class A1> static args1nc_t<A1> argsnc(const A1& a1) { return args1nc_t<A1>(a1); }
        template<class A1, class A2> static args2nc_t<A1, A2> argsnc(const A1& a1, const A2& a2) { return args2nc_t<A1, A2>(a1, a2); }
        template<class A1, class A2, class A3> static args3nc_t<A1, A2, A3> argsnc(const A1& a1, const A2& a2, const A3& a3) { return args3nc_t<A1, A2, A3>(a1, a2, a3); }
        template<class A1, class A2, class A3, class A4> static args4nc_t<A1, A2, A3, A4> argsnc(const A1& a1, const A2& a2, const A3& a3, const A4& a4) { return args4nc_t<A1, A2, A3, A4>(a1, a2, a3, a4); }
        template<class A1, class A2, class A3, class A4, class A5> static args5nc_t<A1, A2, A3, A4, A5> argsnc(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) { return args5nc_t<A1, A2, A3, A4, A5>(a1, a2, a3, a4, a5); }
        template<class A1, class A2, class A3, class A4, class A5, class A6> static args6nc_t<A1, A2, A3, A4, A5, A6> argsnc(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) { return args6nc_t<A1, A2, A3, A4, A5, A6>(a1, a2, a3, a4, a5, a6); }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static args7nc_t<A1, A2, A3, A4, A5, A6, A7> argsnc(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) { return args7nc_t<A1, A2, A3, A4, A5, A6, A7>(a1, a2, a3, a4, a5, a6, a7); }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static args8nc_t<A1, A2, A3, A4, A5, A6, A7, A8> argsnc(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) { return args8nc_t<A1, A2, A3, A4, A5, A6, A7, A8>(a1, a2, a3, a4, a5, a6, a7, a8); }


    typedef void* (*F_alloc)(_s_ll nb);
    typedef void (*F_free)(void* p);
    typedef void (*F_des)(T* p);
    typedef void (*F_del)(T* p);
    static void* sf_alloc(_s_ll nb) __bmdx_noex { return _carray_tu_alloc_t<T>::_sf_calloc(nb); }
    static void sf_free(void* p) __bmdx_noex { return _carray_tu_alloc_t<T>::_sf_free(p); }
    static void sf_destroy1(T* p) __bmdx_noex { return _carray_tu_alloc_t<T>::_sf_destroy1(p); }
    static void sf_delete1(T* p) __bmdx_noex { return _carray_tu_alloc_t<T>::_sf_delete1(p); }
  };



    // struct cref_t
    //
    // Thread-safe const reference (.ref()),
    //  with optional access by non-const pointer (._pnonc_u()) or reference (.ref_ts().xnc).
    //  Also acts as smart pointer: member access (->member)
    //  and destruction-safe member access (--->member) are available.
    //
    // T:
    //    the type of referenced object. By default, the object is assumed to be constant,
    //      accessed only via ref(), ptr() or ref_ts()().
    //      Other methods may modify object as agreed in the client program.
    // LockSelector:
    //    a) With default lock selector, all cref_t objects with the same type of referenced value
    //      (T) share the same lock.
    //    b) cref_nonlock creates non-locking cref_t object.
    //    c) Any other type of lock selector allows to group objects sharing the same lock.
    //    d) cm_copy, cm_assign, cm_create* functions
    //      allow to specify any crit. sec. data object to use for locking.
    //      Such cref_t object becomes independent on LockSelector,
    //      and may be thread-safely passed between binary modules.
    // Multi-functional reference:
    //    It is possible to create customized reference to an object:
    //    choose way of storing, creation/deletion, locking, add auxiliary object,
    //    cast reference type etc.
    //    See struct iref2 for details.
    // NOTE If the referenced object is created in a shared library,
    //    that library must not be unloaded until the last object reference is released
    //    (i.e. the last cref_t for that object is destroyed).

  template<class T, class LockSelector = T>
  struct cref_t
  {

      // Type of the referenced value.
    typedef T
      t_value;


      // Smart lock, used by cref_t for ref. counting. _cref_lock_t must not be redefined by client.
    typedef _cref_lock_t<LockSelector>
      t_lock;


      // Type for ref. counting variable. In 32-bit program: s_long, in 64-bit program: s_ll.
    typedef typename bmdx_meta::if_t<(sizeof(void*) > 4), _s_ll, _s_long>::result
      t_cnt;


      // The type of common event handler for multi-functional references. See also iref2 :: handler_dflt.
      //    s_long (s_long optype, s_ll flags, s_ll nrefs, s_ll delta, _critsec_data0_t<>* pcsd, void* pobj, void* pinterface, void* paux)
    typedef _s_long (*F_ev_handler_iref2)(_s_long optype, _s_ll flags, _s_ll nrefs, _s_ll delta, _critsec_data0_t<LockSelector>* pcsd, void* pobj, void* pinterface, void* paux);



    struct exc_ref : _cref_t_exceptions::exc_ref {};
    struct exc_ref_ts : _cref_t_exceptions::exc_ref_ts {};
    struct exc_create0 : _cref_t_exceptions::exc_create0 {};
    struct exc_create1 : _cref_t_exceptions::exc_create1 {};
    struct exc_create2 : _cref_t_exceptions::exc_create2 {};
    struct exc_create3 : _cref_t_exceptions::exc_create3 {};
    struct exc_create4 : _cref_t_exceptions::exc_create4 {};
    struct exc_create5 : _cref_t_exceptions::exc_create5 {};
    struct exc_create6 : _cref_t_exceptions::exc_create6 {};
    struct exc_create7 : _cref_t_exceptions::exc_create7 {};
    struct exc_create8 : _cref_t_exceptions::exc_create8 {};
    struct exc_irefcr0 : _cref_t_exceptions::exc_irefcr0 {};
    struct exc_irefcr1 : _cref_t_exceptions::exc_irefcr1 {};
    struct exc_irefcr2 : _cref_t_exceptions::exc_irefcr2 {};
    struct exc_irefcr3 : _cref_t_exceptions::exc_irefcr3 {};
    struct exc_irefcr4 : _cref_t_exceptions::exc_irefcr4 {};
    struct exc_irefcr5 : _cref_t_exceptions::exc_irefcr5 {};
    struct exc_irefcr6 : _cref_t_exceptions::exc_irefcr6 {};
    struct exc_irefcr7 : _cref_t_exceptions::exc_irefcr7 {};
    struct exc_irefcr8 : _cref_t_exceptions::exc_irefcr8 {};
    struct exc_assign : _cref_t_exceptions::exc_assign {};
    struct exc_copy : _cref_t_exceptions::exc_copy {};
    struct exc_cm_assign : _cref_t_exceptions::exc_cm_assign {};
    struct exc_cm_copy : _cref_t_exceptions::exc_cm_copy {};
    struct exc_cc3 : _cref_t_exceptions::exc_cc3 {};
    struct exc_refcast_s_u : _cref_t_exceptions::exc_refcast_s_u {};
    struct exc_refcast_d_u : _cref_t_exceptions::exc_refcast_d_u {};
    struct exc_iref2_create_any : _cref_t_exceptions::exc_iref2_create_any {};
    struct exc_iref2_assign : _cref_t_exceptions::exc_iref2_assign {};
  private: friend struct _cref_lock_t<LockSelector>; typedef _carray_tu_alloc_t<T> _a; struct _cref_handle; struct _cref_handle2;
  public:



      // ref():
      //  a) on has_ref() == true, returns a ref. to valid object.
      //  b) on has_ref() == false, generates an exception.
      // The reference, returned by ref(), may be unsafe at the time
      //  when cref_t object is overwritten by another thread.
    const T& ref() const volatile __bmdx_exs(exc_ref)    { T* p = const_cast<T*>(_p); if (!p) { throw exc_ref(); } return *p; }
    const T* ptr() const volatile __bmdx_noex    { return const_cast<const T*>(_p); }



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
      T& xnc;

      const T& operator()() const __bmdx_noex    { return xnc; }

      safe_refnc(const cref_t& r) __bmdx_exs(exc_ref_ts)        : __lock(r), ref(r), xnc(_ref(r)) {}

        // Copying and assignment do not generate exceptions,
        //  because the reference remains valid and locked.
      safe_refnc(const safe_refnc& x) __bmdx_noex        : __lock(x.ref), ref(x.ref), xnc(x.xnc) {}
      void operator=(const safe_refnc& x) __bmdx_noex    { safe_refnc temp(x); bmdx_str::words::swap_bytes(*this, temp); }
    private:
      static T& _ref(const cref_t& r __bmdx_noarg) __bmdx_exs(exc_ref_ts)    { T* p = r._pnonc_u(); if (!p) { throw exc_ref_ts(); } return *p; }
    };
    safe_refnc ref_ts() const __bmdx_exs(exc_ref_ts)    { return *this; }



      // _pnonc_u(): non-const pointer to object, returned without any locking.
      // _rnonc(): non-const reference to object, returned without any locking (if ptr. is null, generates an exception).
      // NOTE Original meaning of cref_t is "reference to constant object".
      //   Treating constant as variable is an agreement and may be unsafe.
      //   Only the client is responsible for logically correct and synchronized modifications.
    T* _pnonc_u() const volatile __bmdx_noex    { return const_cast<T*>(_p); }
    T& _rnonc() const volatile { T* p = const_cast<T*>(_p); if (!p) { throw exc_ref(); } return *p; }



      // Smart pointer functionality.
      //   Allows to call the wrapped object's methods. If cref_t is empty, generates an exception.
      // NOTE Original meaning of cref_t is "reference to constant object".
      //   Treating constant as variable is an agreement and may be unsafe.
      //   Accessing object methods and data via "->" is very fast and roughly equivalent to built-in pointer operation.
      //   It does not make locks, allocations or other resource-costly operations.
      //   Only the client is responsible for logically correct and synchronized modifications.
      // NOTE If cref_t may be concurrently overwritten, use p--->method() notation.
      //   See operator--.
    T* operator->() const volatile        { T* p = const_cast<T*>(_p); if (!p) { throw exc_ref(); } return p; }



      // Returns a copy of this cref_t. Useful to safely call the wrapped object,
      //  when the original cref_t may be concurrently modified.
      // cref_t<T> p; ... p--->method(); ... p--.ref().constant_method();
      // NOTE Copying cref_t usually sets a lock for ref. counting,
      //    also, the object may be destroyed right after the call, because the temporary reference became
      //    the last one during call.
      //    This can lead to unpredictable delays and cannot be used in time-critical routines, like drive callbacks.
      //    I.e. in cref_t, "--->" action is quite different from "->".
    cref_t operator--(int) const __bmdx_noex    { return *this; }



      // false only if
      //  a) no-exception construction from const T& failed,
      //  b) after cref_t().
    bool has_ref() const __bmdx_noex    { return !!_p; }
    operator bool() const __bmdx_noex    { return !!_p; }

      // true if this object contains a strong reference, false - weak reference or empty.
    bool is_own() const __bmdx_noex    { return !!_ph; }

      // true if this object contains a strong reference in detached state (i.e. ref. counting continues, but object pointer is not deleted automatically, see _detach_u()).
    bool is_detached() const __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && _ph->b_detached(); }

      // true if cref_t contains a cross-module object, namely,
      //  a) cross-module strong reference (i.e. type 1), or
      //  b) multi-functional strong reference  (type 2).
      // "if (is_cm()) { ... }" is equivalent of "int type; flags(&type); if (type == 1 || type ==2) { .... }".
    bool is_cm() const __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && (_ph->b_v1() || _ph->b_v2()); }

      // n_refs():
      //  0 - unknown (weak ref., is_own() == false).
      //  >=1 - number of strong references (on is_own() == true).
    t_cnt n_refs() const __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph ? _ph->cnt & _m : 0; }

      // Return a pointer to the crit. sec. data object, that is used by this cref_t for locking (if locking is not disabled).
      //  The returned pointer is non-0.
    _critsec_data0_t<LockSelector>* pcsd0() const __bmdx_noex    { if (_ps) { return _ps; } if (_ph && _ph->b_v2()) { return _s_ph2(_ph)->pcsd; } return typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
    typename critsec_t<LockSelector>::csdata* pcsd() const __bmdx_noex    { return static_cast<typename critsec_t<LockSelector>::csdata*>(pcsd0()); }

      // Returns true is this object actually uses locking, false otherwise.
    bool b_cs() const { if (!t_lock::does_locking()) { return false; } t_lock __lock(*this); if (sizeof(__lock)) {} if (_ph && _ph->b_v2()) { return !!_ps; } return true; }

      // Return aux. object pointer or 0 if this cref_t does not contain aux. object.
      //  Only multi-functional cref_t may contain aux. object. See also struct iref2.
    void* paux() const __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && _ph->b_v2() && (_ph->flags & iref2_flags::use_aux) ? _s_paux(_s_ph2(_ph)) : 0; }

      // Multi-functional reference only: return the main pointer to the referenced object, that had been initially assigned
      //    to the first cref_t that holds this object. The object is deleted by the last destroyed cref_t by calling handler with this pointer.
      //  For all other ref. types: returns 0.
    void* pobj() const __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} return _ph && _ph->b_v2() ? (_s_ph2(_ph))->pobj : 0; }

      // Returns flags of the object, and optionally, reference type.
      //  *ptype -2: null reference. flags == 0.
      //  *ptype -1: weak non-null reference. flags == 0.
      //  *ptype 0: in-module strong reference, with ref. counting, no other functions. flags == 0.
      //  *ptype 1: cross-module strong reference, with ref. counting, no other functions. flags may contain disable_des (0x2), use_critsec (0x4).
      //  *ptype 2: multi-functional strong reference, with ref. counting. flags may contain anything from iref2_flags.
      //  (not expected) *ptype -3: corrupted object. flags == 0.
    t_cnt flags(int* ptype = 0) const __bmdx_noex
    {
      t_lock __lock(*this); if (sizeof(__lock)) {}
      if (!_ph) { if (ptype) { *ptype = _p ? -1 : -2; } return 0; }
      if (_ph->b_v0()) { if (ptype) { *ptype = 0; } return 0; }
      if (_ph->b_v1()) { if (ptype) { *ptype = 1; } return _ph->flags; }
      if (_ph->b_v2()) { if (ptype) { *ptype = 2; } return _ph->flags; }
      if (ptype) { *ptype = -3; } return 0;
    }



      // Copy object, and own the new copy.
      //  (The copy is automatically deleted when the last owning cref_t object is cleared, deleted or overwritten.)
    cref_t(const T& x, bool no_exc)
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { copy(x, no_exc); }

      // Set a strong (is_own_ == true), or a weak reference to an object (is_own_ == false).
      //  NOTE: on is_own_ == true,
      //    1) x must have been created dynamically.
      //    2) the current cref object must be the first existing owner.
    cref_t(const T& x, bool is_own_, bool no_exc)
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { if (is_own_) { _ph = (_cref_handle*)_new_pcnt(1); if (!_ph) { if (!no_exc) { throw exc_cc3(); }  return; } } _p = &x; }

      // Create a cross-module reference. Copy x, and own the new copy.
      //    The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      // pcsd, flags: see cm_copy.
    cref_t(const T& x, typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc)
      : _ph(0), _p(0), _ps(pcsd ? pcsd : typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      { cm_copy(x, pcsd, flags, no_exc); }


      // NOTE The following 6 functions do not throw exceptions.
    cref_t() __bmdx_noex
      : _ph(0), _p(0), _ps(typename critsec_t<LockSelector>::ff_mc().pdefcsd())
      {}

    cref_t(const cref_t& x) __bmdx_noex
      : _ph(0), _p(0), _ps(0)
      {
        t_lock __lock(x); _ps = x._ps; if (sizeof(__lock)) {}
        if (x._ph)
        {
          _ph = x._ph;
          ++_ph->cnt;
          try { struct f : iref2_flags {}; if (_ph->b_v2() && (_ph->flags & f::gen_nrefs)) { _cref_handle2* ph = _s_ph2(_ph); F_ev_handler_iref2 peh = (F_ev_handler_iref2)_ph->f_handler; peh(f::ev_nrefs, _ph->flags, _ph->cnt & _m, 1, ph->pcsd, ph->pobj, (void*)x._p, !!(_ph->flags & f::use_aux) ? _s_paux(ph) : 0); } } catch (...) {}
        }
        _p = x._p;
      }
    //cref_t(const volatile cref_t& x) __bmdx_noex : _ph(0), _p(0), _ps(0) { new (this) cref_t((const cref_t&)x); }

    ~cref_t() __bmdx_noex    { if (!_ph) { return; } t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); }

    cref_t& operator=(const cref_t& x) __bmdx_noex    { cref_t temp(x); t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = temp._p; _ph = temp._ph; temp._p = 0; temp._ph = 0; _ps = temp._ps; return *this; }

      // NOTE Clearing does not change the critical section used for object locking, only ensures it to be non-0.
      //  Assign another empty cref_t instead of clear(), for guaranteed setting the default critical section.
    void clear() __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); }

      // Same as clear(), but does not delete the object if it's strongly referenced.
      //    The client becomes responsible for object lifetime.
      //    Detaching is useful for regaining the ownership on the dynamically created object.
      // NOTE Detaching influences all other cref_t's with strong reference to the same object -
      //    they continue to keep a pointer to that object, but do not delete it when the refs. counter becomes 0.
      //    This allows for ref.-counting with "manual" control of object lifetime.
      //    For example, the client may wait for n_refs() == 1, then delete the object.
      //    (cref_t may safely go out of scope any time.)
      // NOTE In multi-functional reference, detaching only disables certain events (ev_des, ev_delete, ev_release)
      //    generation.
      //    The client must be aware of storage mode (use_hst, use_delete, use_release, custom handler),
      //    and ensure releasing the object correctly.
      //    For example, take into account that the object may be kept together with internal cref_t data (use_hst),
      //    in which case its memory is freed automatically when the last reference is destroyed.
    void _detach_u() __bmdx_noex    { t_lock __lock(*this); if (sizeof(__lock)) {} if (_ph) { _ph->cnt |= _f1; } _reset(); }



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
      // NOTE assign() does not copy x in any way. Do not pass temporary object as x.
    bool assign(const T& x, bool is_own_, bool no_exc)
    {
      t_lock __lock(*this); if (sizeof(__lock)) {}
      if (&x == _p)
      {
        if (is_own_ == is_own()) { return true; } if (!is_own_) { _reset(); _p = &x; return true; }
        _ph = (_cref_handle*)_new_pcnt(1); if (!_ph) { if (!no_exc) {  throw exc_assign(); } return false; }
        return true;
      }
      else
      {
        if (!is_own_) { _reset(); _p = &x; return true; }
        t_cnt* pcnt2 = _new_pcnt(1); if (!pcnt2) { if (!no_exc) {  throw exc_assign(); } return false; }
        _reset(); _p = &x; _ph = (_cref_handle*)pcnt2;
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
    bool copy(const T& x, bool no_exc)
    {
      t_cnt* pcnt2(0); const T* p2(0);
      try { p2 = new T(x); } catch (...) {} pcnt2 = _new_pcnt(1);
      if (p2 && pcnt2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; }
      try { delete p2; } catch (...) {} _del_pcnt(pcnt2);
      if (!no_exc) { throw exc_copy(); }
      return false;
    }

      // Assigns x (by reference) to this cref_t, and takes ownership on x.
      //    The previous object, if existed, is released.
      //    (If the previous object was already strong-referenced x, it's detached instead of being released,
      //    so that the current cref_t becomes single owner of the object.)
      // pcsd, flags: see cm_copy.
      // NOTE If flags contain disable_des (0x2), the client is responsible for deleting the object after the last reference destruction.
      // The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      // Returns:
      //    true on success,
      //    false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      //    On failure, the current reference is kept.
      // NOTE cm_assign() does not copy x in any way. Do not pass temporary object as x.
    bool cm_assign(const T& x, typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc)
    {
      if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
      _cref_handle* ph2 = _new_ph(1, flags);
      if (ph2) { t_lock __lock(*this); if (sizeof(__lock)) {} if (_p == &x) { _detach_u(); } _reset(); _p = &x; _ph = ph2; _ps = pcsd; return true; }
      _del_ph(ph2);
      if (!no_exc) { throw exc_cm_assign(); }
      return false;
    }

      // Copies x, and the copy becomes cref_t's own object.
      //    The previous object, if existed, is released.
      //    The resulting cref_t may be thread-safely copied and passed across binary module boundary.
      //  pcsd (0 for default: use common per-type critsec_t<LockSelector(dflt.==T)>).
      //      pcsd != 0: specific crit. sec. data instance. Must remain valid until the last referring cref_t is released.
      //      pcsd == 0: use critsec_t<LockSelector(dflt.==T)>'s ::pdefcsd().
      //  flags OR-ed (0 for default):
      //    0x1 - reserved, must be 0.
      //    0x2 - "disable_des" - disable "delete p" on ref. count reaching 0.
      //      The flag should be used mostly to refer to static objects.
      //      (But if the object is created dynamically, the client is responsible for releasing the object after ref. count going to 0.)
      //    0x4 - protect x copying by pcsd.
      //      When copying completed, the lock on pcsd is released,
      //      and cref_t modification is done under lock on cref_t's csd.
      //      The last step is setting cref_t's csd = pcsd.
      //      NOTE Even if 0x4 is unset, locking on ref. counting is anyway enabled.
      //        In cross-module type of reference, 0x4 flag only adds locking during the initial object copying.
      // Returns:
      //    true on success,
      //    false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      //    On failure, the current reference is kept.
      //  NOTE The binary module that has created the object copy, must remain valid (prevent unloading)
      //  until its last object is released, no matter in which module it's used.
    bool cm_copy(const T& x, typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc)
    {
      if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
      _cref_handle* ph2 = _new_ph(1, flags);
      if (ph2)
      {
        const T* p2(0);
        if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x); } catch (...) {} }
          else { try { p2 = new T(x); } catch (...) {} }
        if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; }
        _del_ph(ph2);
      }
      if (!no_exc) { throw exc_cm_copy(); }
      return false;
    }



      // Object creation with 0..8 arguments.
      //    On success, the previous object reference is correctly removed, and the new is set on its place.
      //    On failure, the previous object reference remains unchanged.
      // NOTE create1 with A1 == T behaves same as copy().
    bool create0(bool no_exc)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create0(); } return false; }
    template<class A1> bool create1(bool no_exc, const A1& x1)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create1(); } return false; }
    template<class A1, class A2> bool create2(bool no_exc, const A1& x1, const A2& x2)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create2(); } return false; }
    template<class A1, class A2, class A3> bool create3(bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create3(); } return false; }
    template<class A1, class A2, class A3, class A4> bool create4(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3, x4); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create4(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5> bool create5(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3, x4, x5); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create5(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6> bool create6(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3, x4, x5, x6); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create6(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool create7(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3, x4, x5, x6, x7); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create7(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool create8(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T(x1, x2, x3, x4, x5, x6, x7, x8); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create8(); } return false; }

          // Same as create#(), but calling T constructor with all arguments cast to non-const.
        template<class A1> bool create1nc(bool no_exc, const A1& x1)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create1(); } return false; }
        template<class A1, class A2> bool create2nc(bool no_exc, const A1& x1, const A2& x2)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create2(); } return false; }
        template<class A1, class A2, class A3> bool create3nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create3(); } return false; }
        template<class A1, class A2, class A3, class A4> bool create4nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create4(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5> bool create5nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create5(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6> bool create6nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create6(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool create7nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create7(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool create8nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { t_lock __lock(*this); if (sizeof(__lock)) {} t_cnt* pcnt2(0); const T* p2(0); try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); } catch (...) {} pcnt2 = _new_pcnt(1); if (p2 && pcnt2) { _reset(); _p = p2; _ph = (_cref_handle*)pcnt2; return true; } try { delete p2; } catch (...) {} _del_pcnt(pcnt2); if (!no_exc) { throw exc_create8(); } return false; }

      // Cross-module object creation with 0..8 arguments.
      // pcsd, flags, no_exc: see cm_copy.
      // NOTE If flags contain disable_des (0x2), the client is responsible for deleting the object after the last reference destruction.
    bool cm_create0(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(); } catch (...) {} } else { try { p2 = new T(); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create0(); } return false; }
    template<class A1> bool cm_create1(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1); } catch (...) {} } else { try { p2 = new T(x1); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create1(); } return false; }
    template<class A1, class A2> bool cm_create2(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2); } catch (...) {} } else { try { p2 = new T(x1, x2); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create2(); } return false; }
    template<class A1, class A2, class A3> bool cm_create3(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3); } catch (...) {} } else { try { p2 = new T(x1, x2, x3); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create3(); } return false; }
    template<class A1, class A2, class A3, class A4> bool cm_create4(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3, x4); } catch (...) {} } else { try { p2 = new T(x1, x2, x3, x4); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create4(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5> bool cm_create5(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3, x4, x5); } catch (...) {} } else { try { p2 = new T(x1, x2, x3, x4, x5); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create5(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6> bool cm_create6(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3, x4, x5, x6); } catch (...) {} } else { try { p2 = new T(x1, x2, x3, x4, x5, x6); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create6(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool cm_create7(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3, x4, x5, x6, x7); } catch (...) {} } else { try { p2 = new T(x1, x2, x3, x4, x5, x6, x7); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create7(); } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool cm_create8(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T(x1, x2, x3, x4, x5, x6, x7, x8); } catch (...) {} } else { try { p2 = new T(x1, x2, x3, x4, x5, x6, x7, x8); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create8(); } return false; }

          // Same as cm_create#(), but calling T constructor with all arguments cast to non-const.
        template<class A1> bool cm_create1nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1); } catch (...) {} } else { try { p2 = new T((A1&)x1); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create1(); } return false; }
        template<class A1, class A2> bool cm_create2nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create2(); } return false; }
        template<class A1, class A2, class A3> bool cm_create3nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create3(); } return false; }
        template<class A1, class A2, class A3, class A4> bool cm_create4nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create4(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5> bool cm_create5nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create5(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6> bool cm_create6nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create6(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool cm_create7nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create7(); } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool cm_create8nc(typename critsec_t<LockSelector>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); } _cref_handle* ph2 = _new_ph(1, flags); if (ph2) { const T* p2(0); if (!!(flags & 4)) { t_lock __lock(true, pcsd); if (sizeof(__lock)) {} try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); } catch (...) {} } else { try { p2 = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); } catch (...) {} } if (p2) { t_lock __lock(*this); if (sizeof(__lock)) {} _reset(); _p = p2; _ph = ph2; _ps = pcsd; return true; } _del_ph(ph2); } if (!no_exc) { throw exc_create8(); } return false; }

    template<class I, class LS = I, class _ = __vecm_tu_selector> struct iref
    {
        // Create object of type T, which is subclass of I. Assign it as strong reference to cref_t<I, LS>.
        //  Return the result. If creation failed, an empty cref_t is returned (or exception generated on no_exc == false).
        // NOTE The object lock is associated with interface type (I), not with object type (T).
        //  (This works because particular object is held only by pointer of only one interface type.)
        // NOTE LockSelector of the outer cref_t is ignored, critsec_t<LS>::csdata is used.
      static cref_t<I, LS> create0(bool no_exc)    { cref_t<I, LS> rx; try { I* pbase = new T(); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr0(); } return rx; }
      template<class A1> static cref_t<I, LS> create1(bool no_exc, const A1& x1)    { cref_t<I, LS> rx; try { I* pbase = new T(x1); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr1(); } return rx; }
      template<class A1, class A2> static cref_t<I, LS> create2(bool no_exc, const A1& x1, const A2& x2)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr2(); } return rx; }
      template<class A1, class A2, class A3> static cref_t<I, LS> create3(bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr3(); } return rx; }
      template<class A1, class A2, class A3, class A4> static cref_t<I, LS> create4(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr4(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5> static cref_t<I, LS> create5(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr5(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I, LS> create6(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr6(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I, LS> create7(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6, x7); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr7(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I, LS> create8(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6, x7, x8); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr8(); } return rx; }

            // Same as create#(), but calling T constructor with all arguments cast to non-const.
          template<class A1> static cref_t<I, LS> create1nc(bool no_exc, const A1& x1)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr1(); } return rx; }
          template<class A1, class A2> static cref_t<I, LS> create2nc(bool no_exc, const A1& x1, const A2& x2)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr2(); } return rx; }
          template<class A1, class A2, class A3> static cref_t<I, LS> create3nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr3(); } return rx; }
          template<class A1, class A2, class A3, class A4> static cref_t<I, LS> create4nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr4(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5> static cref_t<I, LS> create5nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr5(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I, LS> create6nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr6(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I, LS> create7nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr7(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I, LS> create8nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); if (!rx.assign(*pbase, true, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr8(); } return rx; }

        // Create cross-module object of type T, which is I or subclass of I. Assign it as strong reference to cref_t<I, LS>.
        //  Return the result. If creation failed, an empty cref_t is returned (or exception generated on no_exc == false).
        //  pcsd, flags: see cm_copy.
        // NOTE If I is base class of T, the destructor of I or any of its bases must be declared virtual.
        // NOTE LockSelector of the outer cref_t is ignored.
        // NOTE If flags contain disable_des (0x2), the client is responsible for deleting the object after the last reference destruction.
      static cref_t<I, LS> cm_create0(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc)    { cref_t<I, LS> rx; try { I* pbase = new T(); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr0(); } return rx; }
      template<class A1> static cref_t<I, LS> cm_create1(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1)    { cref_t<I, LS> rx; try { I* pbase = new T(x1); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr1(); } return rx; }
      template<class A1, class A2> static cref_t<I, LS> cm_create2(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr2(); } return rx; }
      template<class A1, class A2, class A3> static cref_t<I, LS> cm_create3(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr3(); } return rx; }
      template<class A1, class A2, class A3, class A4> static cref_t<I, LS> cm_create4(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr4(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5> static cref_t<I, LS> cm_create5(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr5(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I, LS> cm_create6(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr6(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I, LS> cm_create7(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6, x7); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr7(); } return rx; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I, LS> cm_create8(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { cref_t<I, LS> rx; try { I* pbase = new T(x1, x2, x3, x4, x5, x6, x7, x8); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr8(); } return rx; }

            // Same as cm_create#(), but calling T constructor with all arguments cast to non-const.
          template<class A1> static cref_t<I, LS> cm_create1nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr1(); } return rx; }
          template<class A1, class A2> static cref_t<I, LS> cm_create2nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr2(); } return rx; }
          template<class A1, class A2, class A3> static cref_t<I, LS> cm_create3nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr3(); } return rx; }
          template<class A1, class A2, class A3, class A4> static cref_t<I, LS> cm_create4nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr4(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5> static cref_t<I, LS> cm_create5nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr5(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I, LS> cm_create6nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr6(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I, LS> cm_create7nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr7(); } return rx; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I, LS> cm_create8nc(typename critsec_t<LS>::csdata* pcsd, _s_long flags, bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)    { cref_t<I, LS> rx; try { I* pbase = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); if (!rx.cm_assign(*pbase, pcsd, flags, true)) { delete pbase; } } catch (...) {} if (!rx && !no_exc) { throw exc_irefcr8(); } return rx; }
    }; // end of struct iref



      // Multi-functional reference creation and type casting. Features:
      //  1. The type of cref_t may be T or any superclass of T, i.e. cref_t may contain interface reference (const I&).
      //  2. Same object T may be referred by 2 or more cref_t of different interface types.
      //  3. T may be created (stored) a) inside the internal object handler (default), b) in dynamic memory, c) at user-specified location.
      //  4. Inside the internal object handler, an auxiliary user object (Aux) may be created, to assist in handling the main object (T).
      //  5. Destruction of the handled objects is customizable, done by user event handler.
      //  6. The multi-functional cref_t is safe to pass and use between binary modules boundary (inside the same process).
      // NOTE Compatibility: iref2 objects cannot be notified and destroyed correctly by library rev. less than 2019-06-23.
    template<class I = T, class Aux = int, class _ = __vecm_tu_selector>
    struct iref2
    {

        // Default event handler for multi-functional references.
        // For particular object or type,  may be replaced with custom handler.
        // Arguments:
        //    optype - type of operation, for which the handler is called.
        //      See iref2_flags :: ev_*.
        //      Custom handler should implement only the types of events that really occur
        //      on the references using that handler.
        //    flags - value of flags arguement, passed to create_any.
        //    nrefs, delta - for event ev_nrefs: the new value of ref. count,
        //      and the delta by which it has just been changed (+1 or -1).
        //      NOTE ev_nrefs occurs only if flags contain gen_nrefs.
        //      NOTE In other events, both parameters are always 0.
        //    delta - for events ev_cast_*:
        //      difference (in bytes) between the new pointer value (pinterface) and the original cref_t .ptr().
        //    pcsd (always != ) - critical section object, used to lock operations on this cref_t.
        //      May be 0 if the object reference functions in non-locking mode (use_critsec flag is unset).
        //      If pcsd != 0, all events are called already under lock on pcsd, and any additional locking is not required.
        //    pobj - for all events except ev_aux_con, ev_aux_des:
        //      a pointer to the main (real) object,
        //        a) created with create_any(),
        //        b) passed to assign() (here: the pointer may point to one of base objects, not to the most derived object).
        //    pinterface - for all events except ev_aux_con, ev_aux_des:
        //      the value of .ptr() for cref_t on which the event handler is called.
        //      NOTE In certain cases .ptr() may be different from any subobject of the main (real) contained object.
        //        See also _refcast_replace_u.
        //    paux - for all events:
        //      the pointer to auxiliary object, if such exists together with the main contained object.
        //      (Aux object creation is enabled with iref2_flags :: use_aux flag.)
        // Typical return values:
        //  1 - the event is handled.
        //  0 - the event is ignored or not recognized.
        //  -1 - bad argument.
        //  <= -2 failure.
      static _s_long handler_dflt(_s_long optype, _s_ll flags, _s_ll nrefs, _s_ll delta, _critsec_data0_t<LockSelector>* pcsd, void* pobj, void* pinterface, void* paux)
      {
        (void)flags; (void)nrefs; (void)delta; (void)pinterface; (void)pcsd;
        struct f : iref2_flags {};
        if (optype == f::ev_delete) { delete (T*)pobj; return 1; }
        if (optype == f::ev_des) { ((T*)pobj)->~T(); return 1; }
        if (optype == f::ev_release) { return 1; }
        if (optype == f::ev_aux_des) { ((Aux*)paux)->~Aux(); return 1; }
        return 0;
      }

        // Creates multi-functional reference with the given object of type T, constructed of any arguments,
        //    with optional auxiliary object construction.
        //    Aux. object is stored/created/destroyed together with reference internal data.
        // NOTE Even if I is base class of T, the destructor of I does not need to be virtual,
        //    because object destruction occurs on the original type (~T()).
        // no_exc:
        //    true: on any failure, return empty object.
        //    false: on any failure, generate an exception.
        // args: anything, inheriting from i_new. May be usual constructor invocation (via new),
        //    or ready pointer passing etc.
        // pcsd: if 0 - use the default section for T, otherwise use the given pcsd.
        //    NOTE pcsd (either default or user-specified) is later interpreted as critsec_t<I>::csdata*.
        //      In all client code, crit. sec. data (csdata), associated with LockSelector and with any subclass I of T,
        //      is expected to be of the same default structure, not modified by user template specialization.
        // flags: see iref2_flags.
        //    Exactly one of storage flags must be set: use_hst, use_delete, use_release.
        //    Setting use_critsec is always recommended.
        //    Also, any user flags may be stored
        //    under mask 0xffffffffff000000 on 64-bit system,
        //    and 0xff000000 on 32-bit system.
        // h: a function that should specifically process events: object destruction and optionally,
        //    ref. counting and aux object creation/destruction.
        //    See, for example, handler_dflt. Also iref2_flags.
        // args_aux: same as args, but for auxiliary object. If no use_aux flag specified, args_aux is ignored.
        // nb_aux: if >= 0, allocate nb_aux bytes of storage for Aux, instead of sizeof(Aux).
        //    Use it to create object whose size may be different depending on context.
        //    NOTE Be careful if nb_aux >= 0, but < sizeof(Aux): Aux constructor may attempt
        //      to modify fields, for which no place has been allocated.
        //      The safest way is to specify at least nb_aux == sizeof(Aux), even if Aux object won't be used in full.
        //      For example: see make_rba fn. below.
      static cref_t<I> create_any(
        bool no_exc,
        const typename iref2_args_t<T>::i_new& args,
        typename critsec_t<LockSelector>::csdata* pcsd = 0,
        t_cnt flags = 0x14, // use_hst | use_critsec
        F_ev_handler_iref2 h = handler_dflt,
        const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(),
        _s_ll nb_aux = -1
      ) {
        flags &= ~t_cnt(0xfffe00);
        struct f : iref2_flags {};
        const bool b_aux = !!(flags & f::use_aux);
        const bool b_nrefs = !!(flags & f::gen_nrefs);
        const bool b_hst = !!(flags & f::use_hst);
        const bool b_del = !!(flags & f::use_delete);
        const bool b_rel = !!(flags & f::use_release);
        const bool b_cs = !!(flags & f::use_critsec);
        if (int(b_hst) + b_del + b_rel != 1) { if (!no_exc) { throw exc_iref2_create_any(); } return cref_t<I>(); }
        if (!h) { h = handler_dflt; }
        if (!pcsd) { pcsd = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
        _cref_handle2* ph = _new_ph2<Aux>(1, pcsd, flags, h, args_aux, nb_aux);
        if (ph)
        {
          T* p2(0);
          t_lock __lock(pcsd); if (sizeof(__lock)) {}
          if (b_cs) { __lock.try_lock(true); }
          try {
            if (!b_hst) { p2 = args(0); } else { p2 = args((T*)_s_pobj(ph, b_aux, nb_aux >= 0 ? nb_aux : _s_ll(sizeof(Aux)))); }
            ph->pobj = p2;
          } catch (...) {}
          if (p2)
          {
            cref_t<I> rv;
            I* pint = p2; // p2 of T must be I or subclass of I
            ((void**)&rv)[0] = &ph->h; ((void**)&rv)[1] = pint; ((void**)&rv)[2] = b_cs ? pcsd : 0;
            if (b_nrefs) { try { F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->h.f_handler; peh(f::ev_nrefs, ph->h.flags, ph->h.cnt & _m, ph->h.cnt & _m, pcsd, p2, pint, b_aux ? _s_paux(ph) : 0); } catch (...) {} }
            return rv;
          }
          _del_ph(&ph->h);
        }
        if (!no_exc) { throw exc_cm_copy(); }
        return cref_t<I>();
      }

        // See create_any.
      static cref_t<I> create0(bool no_exc, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::args(), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1> static cref_t<I> create1(bool no_exc, const A1& x1, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1>(x1), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2> static cref_t<I> create2(bool no_exc, const A1& x1, const A2& x2, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2>(x1, x2), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3> static cref_t<I> create3(bool no_exc, const A1& x1, const A2& x2, const A3& x3, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3>(x1, x2, x3), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3, class A4> static cref_t<I> create4(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3, A4>(x1, x2, x3, x4), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3, class A4, class A5> static cref_t<I> create5(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3, A4, A5>(x1, x2, x3, x4, x5), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I> create6(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3, A4, A5, A6>(x1, x2, x3, x4, x5, x6), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I> create7(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3, A4, A5, A6, A7>(x1, x2, x3, x4, x5, x6, x7), pcsd, flags, h, args_aux, nb_aux); }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I> create8(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template args<A1, A2, A3, A4, A5, A6, A7, A8>(x1, x2, x3, x4, x5, x6, x7, x8), pcsd, flags, h, args_aux, nb_aux); }

            // Same as create#, but all args. are cast to non-const when passed into constructor.
          template<class A1> static cref_t<I> create1nc(bool no_exc, const A1& x1, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1>(x1), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2> static cref_t<I> create2nc(bool no_exc, const A1& x1, const A2& x2, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2>(x1, x2), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3> static cref_t<I> create3nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3>(x1, x2, x3), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3, class A4> static cref_t<I> create4nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3, A4>(x1, x2, x3, x4), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3, class A4, class A5> static cref_t<I> create5nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3, A4, A5>(x1, x2, x3, x4, x5), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3, class A4, class A5, class A6> static cref_t<I> create6nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3, A4, A5, A6>(x1, x2, x3, x4, x5, x6), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> static cref_t<I> create7nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3, A4, A5, A6, A7>(x1, x2, x3, x4, x5, x6, x7), pcsd, flags, h, args_aux, nb_aux); }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static cref_t<I> create8nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8, typename critsec_t<LockSelector>::csdata* pcsd = 0, t_cnt flags = 0x14, F_ev_handler_iref2 h = handler_dflt, const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(), _s_ll nb_aux = -1) { return create_any(no_exc, iref2_args_t<T>::template argsnc<A1, A2, A3, A4, A5, A6, A7, A8>(x1, x2, x3, x4, x5, x6, x7, x8), pcsd, flags, h, args_aux, nb_aux); }

      private: struct _x_assigner;
      public:

        // Convenience function.
        //  Arguments: see create_any.
        //    If use_hst is set (dflt.): x is copied, and the copy is completely managed by cref_t.
        //      NOTE assign() with use_hst is equivalent to create1() with use_hst,
        //          just making an x copy into preallocated memory.
        //        In contrary, with use_delete, create1() creates a copy "new T(x)",
        //        but assign() only takes ownership on the pointer &x.
        //    If use_delete is set: x is assumed to be dynamically created by client (by def. using operator new).
        //      cref_t gains ownership on the object. (On assign() failure, ownership remains on the client side.)
        //      When the last cref_t is destroyed, ev_delete occurs, which by default calls "delete p".
        //      NOTE If real type of x (some T2) is not T, the destructor of T or any of its bases must be declared virtual,
        //      because object destruction occurs on the given original type (~T()).
        //    If use_release is set: x is assumed to be created statically or in some other uncontrollable way.
        //      When the last cref_t is destroyed, ev_release occurs, which by default does nothing to the object.
        //  In other respecs, assign() is the same as create_any().
      static cref_t<I> assign(
        bool no_exc,
        const T& x,
        typename critsec_t<LockSelector>::csdata* pcsd = 0,
        t_cnt flags = 0x14, // use_hst (copy x) | use_critsec
        F_ev_handler_iref2 h = handler_dflt,
        const typename iref2_args_t<Aux>::i_new& args_aux = iref2_args_t<Aux>::args(),
        _s_ll nb_aux = -1
      ) {
        struct f : iref2_flags {};
        const bool b_hst = !!(flags & f::use_hst);
        const bool b_del = !!(flags & f::use_delete);
        const bool b_rel = !!(flags & f::use_release);
        if (int(b_hst) + b_del + b_rel != 1) { if (!no_exc) { throw exc_iref2_assign(); } return cref_t<I>(); }
        return create_any(no_exc, _x_assigner(x, b_hst), pcsd, flags, h, args_aux, nb_aux);
      }


        // Family of pointer casting/replacement functions.
        //
        // Each creates a new reference to the same object, but with another pointer value.
        //  In all _refcast*, T is the type of original reference, i.e. src.ref() is const T&.
        //  The real type of the contained object is unknown to the client.
        //
        // NOTE Reference casting/replacement works only with the following kind of cref_t:
        //   1) weak references (flags(): type -1),
        //   2) multi-functional references (flags(): type 2).
        //   For empty src (ptr() ==0, flags(): type -2), an empty result is always returned.
        //
        //  A. _refcast_s_u:
        //      Does static_cast<I*>(src.ptr()).
        //      (But if src.ptr() == 0, the result will be anyway 0.)
        //      NOTE Casting is unsafe if I is subclass of T, but the real contained object is not an I.
        //    no_exc: specifies the behavior in case
        //      when src.ptr() != 0, but src is of bad kind (not a weak reference or multi-functional reference).
        //      true: will generate an exception.
        //      false: will return an empty cref_t<I>().
        //
        //  B. _refcast_d_u:
        //      Does dynamic_cast<I*>(src.ptr()).
        //      (But if src.ptr() == 0, the result will be anyway 0.)
        //      NOTE On few systems, this may cause system-level exception if the original object
        //        is from another binary module.
        //        Do not use dynamic cast in this way or check it in all target systems.
        //    no_exc: specifies the behavior in cases
        //      1) the result of dynamic cast on the non-0 pointer is 0,
        //        i.e. if the real contained object is not an I,
        //      2) src.ptr() != 0, but src is of bad kind (not a weak reference or multi-functional reference).
        //      true: will generate an exception.
        //      false: will return an empty cref_t<I>().
        //
        //  C. _refcast_replace_u:
        //      Directly sets p_replace to be .ptr() of the returned cref_t.
        //      If src.ptr() != 0, it is completely ignored.
        //      (But if any src.ptr() == 0 || p_replace == 0 || bad kind of src, the result will be anyway 0.)
        //      In case of non-empty cref_t returned, only the client is responsible
        //      for its addressing valid object.
        //      (cref_t itself never uses its own .ptr(); in part. in multi-functional cref_t
        //      all necessary operations are done on the original object pointer
        //      that is stored internally.)
        //    Primary purpose: several cref_t objects may efficiently share single memory area,
        //      addressing different objects, placed in this area in application-specific way.
        //      Lifetime of the shared area and the placed objects may be controlled
        //      via cref_t aux. object (because in multi-functional cref_t,
        //      it is stored and shared together with reference counter),
        //      and/or in application-specific way.
        //
        // Returns:
        //
        //  a) on success - the result of pointer casting or replacement.
        //    Internally, the real object is the same,
        //    but the returned cref_t .ptr() is different from src.ptr(),
        //    appropriate to type of casting.
        //    Also, in case of multi-functional reference,
        //    both src and the returned cref_t share the same reference counter and auxiliary object.
        //
        //  b) (_refcast_s_u, _refcast_d_u only)
        //    on casting error or bad kind of src, and no_exc == true:
        //    an empty cref_t<I>() is returned.
        //
        //  c) (_refcast_s_u, _refcast_d_u only)
        //    on casting error or bad kind of src, and no_exc == false:
        //    an exception is generated.
        //
        //  d) if src.ptr() == 0, an empty cref_t<I>() is returned.
        //    NOTE _refcast_replace_u:
        //      Case (d) occurs if any:
        //      src.ptr() == 0 || p_replace == 0 || bad kind of src (not a weak reference or multi-functional reference)
        //      Even if bad-kind src is valid and contains ref. counter,
        //      the returned empty cref_t does not share it,
        //      and does not internally reference src's real object or auxiliary object.
        //
        //  NOTE In (a, b, d), the returned cref_t refers
        //    to the same critical section data as src,
        //    even if the returned cref_t is empty.
        //
      static cref_t<I> _refcast_s_u(cref_t src, bool no_exc)
      {
        cref_t<I> rv; ((void**)&rv)[2] = src._ps;
        if (!src._p) { return rv; } // src is empty, same will be ret. value
        struct f : iref2_flags {}; const bool b_weak = !src._ph; const bool b_multifun = !b_weak && src._ph->b_v2();
        const I* p2 = 0; if (b_weak || b_multifun)
          { p2 = static_cast<const I*>(src._p); } // I and T must be statically related
        if (!p2) { if (!no_exc) { throw exc_refcast_s_u(); } return rv; }
        const _s_ll delta = (char*)p2 - (char*)src._p;
        ((void**)&rv)[0] = src._ph; ((void**)&rv)[1] = (void*)p2; src._p = 0; src._ph = 0;
        if (b_multifun && (((cref_t&)rv)._ph->flags & f::gen_cast)) { _cref_handle2* ph = _s_ph2(((cref_t&)rv)._ph); try { F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->h.f_handler;
          typename cref_t::t_lock __lock(ph->pcsd); if (ph->h.flags & f::use_critsec) { __lock.try_lock(true); }
          peh(f::ev_cast_static, ph->h.flags, ph->h.cnt & _m, delta, ph->pcsd, ph->pobj, (void*)p2, !!(ph->h.flags & f::use_aux) ? _s_paux(ph) : 0);
        } catch (...) {} }
        return rv;
      }
      static cref_t<I> _refcast_d_u(cref_t src, bool no_exc)
      {
        cref_t<I> rv; ((void**)&rv)[2] = src._ps;
        if (!src._p) { return rv; } // src is empty, same will be ret. value
        struct f : iref2_flags {}; const bool b_weak = !src._ph; const bool b_multifun = !b_weak && src._ph->b_v2();
        const I* p2 = 0; if (b_weak || b_multifun) { p2 = dynamic_cast<const I*>(src._p); }
        if (!p2) { if (!no_exc) { throw exc_refcast_d_u(); } return rv; }
        const _s_ll delta = (char*)p2 - (char*)src._p;
        ((void**)&rv)[0] = src._ph; ((void**)&rv)[1] = (void*)p2; src._p = 0; src._ph = 0;
        if (b_multifun && (((cref_t&)rv)._ph->flags & f::gen_cast)) { _cref_handle2* ph = _s_ph2(((cref_t&)rv)._ph); try { F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->h.f_handler;
          typename cref_t::t_lock __lock(ph->pcsd); if (ph->h.flags & f::use_critsec) { __lock.try_lock(true); }
          peh(f::ev_cast_dynamic, ph->h.flags, ph->h.cnt & _m, delta, ph->pcsd, ph->pobj, (void*)p2, !!(ph->h.flags & f::use_aux) ? _s_paux(ph) : 0);
        } catch (...) {} }
        return rv;
      }
      static cref_t<I> _refcast_replace_u(cref_t src, void* p_replace)
      {
        cref_t<I> rv; ((void**)&rv)[2] = src._ps;
        if (!src._p) { return rv; } // src is empty, same will be ret. value
        struct f : iref2_flags {}; const bool b_weak = !src._ph; const bool b_multifun = !b_weak && src._ph->b_v2();
        const I* p2 = 0; if (b_weak || b_multifun) { p2 = (const I*)p_replace; }
        if (!p2) { return rv; }
        const _s_ll delta = (char*)p2 - (char*)src._p;
        ((void**)&rv)[0] = src._ph; ((void**)&rv)[1] = (void*)p2; src._p = 0; src._ph = 0;
        if (b_multifun && (((cref_t&)rv)._ph->flags & f::gen_cast)) { _cref_handle2* ph = _s_ph2(((cref_t&)rv)._ph); try { F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->h.f_handler;
          typename cref_t::t_lock __lock(ph->pcsd); if (ph->h.flags & f::use_critsec) { __lock.try_lock(true); }
          peh(f::ev_cast_replace, ph->h.flags, ph->h.cnt & _m, delta, ph->pcsd, ph->pobj, (void*)p2, !!(ph->h.flags & f::use_aux) ? _s_paux(ph) : 0);
        } catch (...) {} }
        return rv;
      }

    private:
      struct _x_assigner : iref2_args_t<T>::i_new
      {
        _x_assigner(const T& x, bool b_copy_) { px = &x; b_copy = b_copy_; }
        virtual T* operator()(T* p) const
        {
          if (p) { if (b_copy) { new (p) T(*px); return p; } throw exc_iref2_assign(); }
            else { if (b_copy) { return new T(*px); } return (T*)px; }
        }
        private: const T* px; bool b_copy;
      };

    }; // end of struct iref2



  private:
    _cref_handle* _ph; // _p == 0 => _ph == 0, but not vice versa
    mutable const T* volatile _p;
    _critsec_data0_t<LockSelector>* _ps;

    static const t_cnt _f3 = t_cnt(1) << (sizeof(t_cnt) * 8 - 3);
    static const t_cnt _f2 = _f3 << 1;
    static const t_cnt _f23 = _f2 | _f3; // ==0 - in-module ref. (type 0), ==_f2 - cross-module ref. (type 1), ==_f3 - multi-func. ref. (type 2), ==_f23 - reserved
    static const t_cnt _f1 = _f3 << 2; // "detached (== destructor disabled)"
    static const t_cnt _m = ~(_f1 | _f2 | _f3); // mask for reference counting bits in _ph->cnt

    typedef typename iref2_args_t<T>::F_alloc F_alloc;
    typedef typename iref2_args_t<T>::F_free F_free;
    typedef typename iref2_args_t<T>::F_des F_des;
    typedef typename iref2_args_t<T>::F_del F_del;
    struct _cref_handle
    {
      volatile t_cnt cnt;
      t_cnt flags;
      F_free f_free;
      void* f_handler;

      void _init(t_cnt n0, _s_long flags_) { cnt = (n0 & _m) | _f2; flags = flags_; f_free = _a::_sf_free; f_handler = (void*)_a::_sf_delete1; }

      bool b_v0() const __bmdx_noex { return (cnt & _f23) == 0; } // *this is just a t_cnt
      bool b_v1() const __bmdx_noex { return (cnt & _f23) == _f2; } // *this is a _cref_handle
      bool b_v2() const __bmdx_noex { return (cnt & _f23) == _f3; } // *this is variable size _cref_handle2
      bool b_detached() const __bmdx_noex { return !!(cnt & _f1); }
    };
    struct _cref_handle2
    {
      _cref_handle h;
      _critsec_data0_t<LockSelector>* pcsd;
      void* pobj;
    };
    static _cref_handle2* _s_ph2(_cref_handle* ph) __bmdx_noex { return (_cref_handle2*)((char*)ph - offsetof(_cref_handle2, h)); }

    static const int _nbalign = 8; // must be power of two and >= sizeof(double)
    static const int _nbd = sizeof(double);
    static _s_ll _s_roundup_size(_s_ll nb) { return (nb + (_nbalign - 1)) & ~_s_ll(_nbalign - 1); }
    static void* _s_next_aligned(void* p, _s_ll nbsize) { return __bmdx_null_pchar + _s_roundup_size(_s_ll((char*)p - __bmdx_null_pchar) + nbsize); }
    static void* _s_paux(_cref_handle2* ph) __bmdx_noex { return _s_next_aligned(ph, sizeof(_cref_handle2)); }
    static void* _s_pobj(_cref_handle2* ph, bool b_aux, _s_ll nb_aux) __bmdx_noex { return b_aux ? _s_next_aligned(_s_paux(ph), nb_aux) : _s_paux(ph); }
    static _s_ll _s_nb_h_iref2(bool b_aux, bool b_hst, _s_ll nb_aux, _s_ll nb_hst) __bmdx_noex
    {
      if (b_aux || b_hst)
      {
        _s_ll n = _s_roundup_size(_s_ll((_nbalign > _nbd ?  _nbalign - _nbd - 1 : 0) + sizeof(_cref_handle2)));
        if (b_aux) { n += _s_roundup_size(nb_aux); }
        if (b_hst) { n += _s_roundup_size(nb_hst); }
        return n;
      }
      return _s_ll(sizeof(_cref_handle2));
    }

    void _reset(__bmdx_noarg1) __bmdx_noex
    {
      if (_ph)
      {
        --_ph->cnt;
        try { struct f : iref2_flags {}; if (_ph->b_v2() && (_ph->flags & f::gen_nrefs)) { _cref_handle2* ph = _s_ph2(_ph); F_ev_handler_iref2 peh = (F_ev_handler_iref2)_ph->f_handler; peh(f::ev_nrefs, _ph->flags, _ph->cnt & _m, -1, ph->pcsd, ph->pobj, (void*)_p, !!(_ph->flags & f::use_aux) ? _s_paux(ph) : 0); } } catch (...) {}
        _del_onrc0(_ph, const_cast<T*>(_p));
        if ((_ph->cnt & _m) == 0) { _del_ph(_ph); }
        _ph = 0;
      }
      if (!_ps) { _ps = typename critsec_t<LockSelector>::ff_mc().pdefcsd(); }
      _p = 0;
    }
    static t_cnt* _new_pcnt(t_cnt n0 __bmdx_noarg) __bmdx_noex { t_cnt* p = (t_cnt*)_a::_sf_calloc(sizeof(t_cnt)); if (p) { *p = n0; }  return p; }
    static _cref_handle* _new_ph(t_cnt n0, _s_long flags __bmdx_noarg) __bmdx_noex
    {
      _cref_handle* ph = (_cref_handle*)_a::_sf_calloc(sizeof(_cref_handle));
      if (ph) { new (ph) _cref_handle(); ph->_init(n0, (flags & 0x2) | 0x4); } // use_critsec is forced in this type of handler, the flag only indicates that it's enabled
      return ph;
    }
    template<class Aux> static _cref_handle2*  _new_ph2(
      t_cnt n0,
      typename critsec_t<LockSelector>::csdata* pcsd,
      t_cnt flags,
      F_ev_handler_iref2 peh,
      const typename iref2_args_t<Aux>::i_new& args_aux,
      _s_ll nb_aux // if < 0, use sizeof(Aux)
      __bmdx_noarg
    ) __bmdx_noex
    {
      struct f : iref2_flags {};
      const bool b_aux = !!(flags & f::use_aux);
      const bool b_hst = !!(flags & f::use_hst);
      _s_ll nbh = _s_nb_h_iref2(b_aux, b_hst, nb_aux >= 0 ? nb_aux : _s_ll(sizeof(Aux)), sizeof(T));
      _cref_handle2* ph = (_cref_handle2*)_a::_sf_calloc(nbh);
        if (!ph) { return 0; }
      if (b_aux) { try { args_aux((Aux*)_s_paux(ph)); } catch (...) { _a::_sf_free(ph); return 0; } }
      ph->h.cnt = (n0 & _m) | _f3;
      ph->h.flags = flags;
      ph->h.f_free = _a::_sf_free;
      ph->h.f_handler = (void*)peh;
      ph->pobj = 0;
      ph->pcsd = pcsd;
      if (b_aux) { try { peh(f::ev_aux_con, ph->h.flags, 0, 0, pcsd, 0, 0,  _s_paux(ph)); } catch (...) {} }
      return ph;
    }
    static void _del_onrc0(_cref_handle* ph, T* p __bmdx_noarg) __bmdx_noex
    {
      if (ph->b_detached() || !!(ph->cnt & _m)) { return; }
      if (ph->b_v0()) { try { delete p; } catch (...) {} return; }
      struct f : iref2_flags {};
      const bool b_des = !(ph->flags & f::disable_des);
      if (ph->b_v1()) { if (b_des) { ((F_del)ph->f_handler)(p); } return; }
      if (ph->b_v2())
      {
        const bool b_hst = !!(ph->flags & f::use_hst); const bool b_del = !!(ph->flags & f::use_delete); const bool b_rel = !!(ph->flags & f::use_release); const bool b_aux = !!(ph->flags & f::use_aux);
        _cref_handle2* ph2 = _s_ph2(ph); F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->f_handler;
        try {
          if (b_del) { peh(f::ev_delete, ph->flags, 0, 0, ph2->pcsd, ph2->pobj, p, b_aux ? _s_paux(ph2) : 0); return; }
          if (b_hst && b_des) { peh(f::ev_des, ph->flags, 0, 0,  ph2->pcsd, ph2->pobj, p, b_aux ? _s_paux(ph2) : 0); return; }
          if (b_rel) { peh(f::ev_release, ph->flags, 0, 0,  ph2->pcsd, ph2->pobj, p, b_aux ? _s_paux(ph2) : 0); return; }
        } catch (...) {}
        return;
      }
      ((F_des)0)(0);
    }
    static void _del_pcnt(t_cnt* pcnt __bmdx_noarg) __bmdx_noex { if (!pcnt) { return; } _a::_sf_free(pcnt); }
    static void _del_ph(_cref_handle* ph __bmdx_noarg) __bmdx_noex
    {
      struct f : iref2_flags {};
      if (!ph) { return; }
      if (ph->b_v0()) { _a::_sf_free(ph); return; }
      if (ph->b_v1()) { ph->f_free(ph); return; }
      if (ph->b_v2())
      {
        if ((ph->flags & f::use_aux) && !(ph->flags & f::disable_aux_des))
        {
          _cref_handle2* ph2 = _s_ph2(ph); F_ev_handler_iref2 peh = (F_ev_handler_iref2)ph->f_handler;
          try { peh(f::ev_aux_des, ph->flags, 0, 0, ph2->pcsd, 0, 0, _s_paux(ph2)); } catch (...) {}
        }
        ph->f_free(_s_ph2(ph));
        return;
      }
      ((F_des)0)(0);
    }
  }; // end of struct cref_t




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
      cbarg(__bmdx_noarg1) __bmdx_noex : b_ok(true) {}
      cbarg(const cbarg& x_ __bmdx_noarg) __bmdx_noex : rx(x_.rx), b_ok(x_.b_ok) {}
      template<class Cb> cbarg(const Cb& x_ __bmdx_noarg) __bmdx_noex : b_ok(false) { try { icb* pbase = new Cb(x_); if (!rx.cm_assign(*pbase, 0, 0, true)) { delete pbase; } } catch (...) {} b_ok = !!rx; }
    };


    multithread(__bmdx_noarg1) : _ps(0) { _pdestr = _destr; }

      // NOTE On destruction, the multithread object automatically sets b_stop() flag to all its running threads
      //  (same as signal_stop()) and then detaches from them.
      //  If this has is not desired, the client should call detach() before multithread object is destroyed.
    ~multithread() { _pdestr(this); }

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
      { _pdestr = _destr; this->start(nth, mode, c, cb, priority); }

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
    operator bool() const __bmdx_exs(__bmdx_noargt1) { return n_run() > 0; }

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


      ictx(__bmdx_noarg1) __bmdx_noex : _pm(0), _cb(), _nth(1), _ith(0), _pr(4), _rb_run(), _rth() {}

        // Parent multithread object.
        //  This pointer is valid only if the client did not detach from the running threads.
      const multithread* pm(__bmdx_noarg1) const __bmdx_noex { return _pm; }

        // Total number of running threads.
      _s_long nth(__bmdx_noarg1) const __bmdx_noex { return _nth; }

        // The current thread number [0..nth()).
      _s_long ith(__bmdx_noarg1) const __bmdx_noex { return _ith; }

        // Controls for all the running threads.
        //  NOTE The array may be empty if the client called mt_proc directly (strong call without multithread).
      cref_t<cpparray_t<threadctl> > rth(__bmdx_noarg1) const __bmdx_noex { return _rth; }

        // User callback, may be set or replaced at any time (before/during thread run or on direct mt_proc call).
      cref_t<icb> rcb(__bmdx_noarg1) const __bmdx_noex { return _cb; }
      void set_cb(const cref_t<icb>& x __bmdx_noarg) __bmdx_noex { _cb = x; }

        // Helper function. Divides n into nth() ranges.
        //  Calculates and sets i0 and i2 to half-open range [i0..i2),
        //  corresponding to the current thread index (ith()).
        //  If such i0 or i2 is out of [0..n), they are adjusted to fit into [0..n),
        //  so the resulting range may be empty or shorter than in other threads.
        // Returns:
        //    true - the resulting [i0..i2) is not empty.
        //    false - in any other case. Range [i0..i2) is empty.
      template<class N> inline bool range(_s_ll n, N& i0, N& i2) const
      {
        i0 = 0; i2 = 0;
        if (n <= 0) { return false; }
        _s_ll dn = (n + _nth - 1) / _nth;
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
      virtual void cb_proc(t_ictx* pctx, _s_long ith, _s_long msgtype, _s_ll msg) = 0;


      virtual ~icb() {}
    };

  private:
    void (*_pdestr)(multithread* p); static void _destr(multithread* p) { p->_clear(); }
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
    static bool is_ex_file(const char* ppath __bmdx_noarg) __bmdx_noex    { if ( ppath && *ppath != '\0' && 0 == __bmdx_std_access(ppath, __F_OK) ) { struct stat st; return 0 == stat(ppath, &st) && !!(st.st_mode & S_IFREG); } else { return false; } }
    static inline bool is_ex_file(const std::string& path __bmdx_noarg) __bmdx_noex    { return is_ex_file(path.c_str()); }

      // Returns true if the specified path points to the existing directory, false in all other cases.
    static bool is_ex_dir(const char* ppath __bmdx_noarg) __bmdx_noex    { if ( ppath && *ppath != '\0' && 0 == __bmdx_std_access(ppath, __F_OK) ) { struct stat st; return 0 == stat(ppath, &st) && !!(st.st_mode & S_IFDIR); } else { return false; } }
    static inline bool is_ex_dir(const std::string& path __bmdx_noarg) __bmdx_noex    { return is_ex_dir(path.c_str()); }

      // Creates the specified directory (NOTE POSIX: with rwx access),
      //    or does nothing if the directory already exists.
      //  Returns:
      //    a) true if the directory exists or just created.
      //    b) false on failure.
    static bool mkdir_rwx(const char* ppath __bmdx_noarg) { if (is_ex_dir(ppath)) { return true; } return 0 == __bmdx_std_mkdir(ppath __bmdx_std_mkdir_arg2); }
    static bool mkdir_rwx(const std::string& path __bmdx_noarg) { return mkdir_rwx(path.c_str()); }




    //  is_open, result.
    //  open, close, seek, seek_end, tell, read, write, flush.
    //  static: is_ex_file, is_ex_dir, load_bytes, save_bytes

    file_io() __bmdx_noex : _desc(0), _nwr(0), _mode(0), _res(0) { __pad4 = 0; _last_op_wr = 0; }
    ~file_io() __bmdx_noex { if (is_open()) { close(); } }

      // NOTE Copy constructor does not actually copy.
      //    Defined for compatibility only.
    file_io(const file_io&) __bmdx_noex { new (this) file_io(); }


    inline bool is_open() const __bmdx_noex { return !!_desc; }
      // mode 0: file is not open, mode 1: file is open read-only, mode 2: file is open read-write.
    inline _s_long mode() const __bmdx_noex { return _desc ? _mode : 0; }
    inline _s_long result() const __bmdx_noex { return _res; }

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
    inline void open(const char* pfilename, bool can_wrcr, bool wr_trunc = false) __bmdx_noex
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
    inline void close() __bmdx_noex
    {
      if (!is_open()) { _res = 0; return; }
      _res = fclose(_desc) == 0 ? 1 : -1;
      _desc = 0;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - seek error.
    inline void seek(_s_ll pos) __bmdx_noex
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
    inline void seek_end() __bmdx_noex
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
    inline _s_ll tell() const __bmdx_noex
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
    inline size_t read(void* dest, size_t size) __bmdx_noex
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
    inline size_t write(const void* src, size_t size) __bmdx_noex
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
    inline void flush() __bmdx_noex
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

    inline _s_ll _tell_u() const __bmdx_noex
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
    inline int _seek_u(_s_ll pos) __bmdx_noex
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
      static inline int load_bytes(const std::string& fnp, std::string& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp.c_str(), _stra_str(), &dest); }
      static inline int load_bytes(const char* fnp, std::string& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp, _stra_str(), &dest); }

      static inline int load_bytes(const std::string& fnp, _carray_base_t<char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp, _stra_ca(), &dest); }
      static inline int load_bytes(const std::string& fnp, _carray_base_t<unsigned char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<unsigned char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp, _stra_ca(), &dest); }
      static inline int load_bytes(const std::string& fnp, _carray_base_t<signed char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, _carray_base_t<signed char>& dest __bmdx_noarg) __bmdx_noex { return _load_bytes(fnp, _stra_ca(), &dest); }
      static inline int load_bytes(const char* fnp, cref_t<arrayref_t<char> >& dest __bmdx_noarg) __bmdx_noex;

          // Saves bytes from src to the given file.
          //    b_append == false truncates the file before writing, if it exists.
          //    if n == 0, pdata may be 0.
          // 1 - success.
          // 0 - failed to create file (or open the existing file for writing).
          // -1 - data size too large, or memory alloc. error, or wrong arguments.
          // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
      static inline int save_bytes(const char* fnp, const std::string& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp, src.c_str(), src.length(), b_append); }
      static inline int save_bytes(const std::string& fnp, const std::string& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp.c_str(), src.c_str(), _s_ll(src.length()), b_append); }

      static inline int save_bytes(const char* fnp, const char* pdata, _s_ll n0, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp, pdata, n0, b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<char>& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp, src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<char>& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp.c_str(), src.pd(), src.n(), b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<unsigned char>& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<unsigned char>& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp.c_str(), (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const char* fnp, const arrayref_t<signed char>& src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
      static inline int save_bytes(const std::string& fnp, const arrayref_t<signed char>&  src, bool b_append __bmdx_noarg) __bmdx_noex { return _save_bytes(fnp.c_str(), (const char*)src.pd(), src.n(), b_append); }



    private:
      struct _stra_base { typedef std::string::size_type _t_sz; virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const __bmdx_noex = 0; virtual char* _pd(void* ps __bmdx_noarg) const __bmdx_noex = 0; };
      struct _stra_str : _stra_base { virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const __bmdx_noex { if (n0 < 0) { n0 = 0; } typedef std::string T; _t_sz n = _t_sz(n0); if (_s_ll(n) != n0) { return false; } try { if (n) { ((T*)ps)->resize(n); } else { ((T*)ps)->clear(); } return true; } catch (...) { return false; } } virtual char* _pd(void* ps __bmdx_noarg) const __bmdx_noex { typedef std::string T; return &(*(T*)ps)[0]; } };
      struct _stra_ca : _stra_base { virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const __bmdx_noex { if (n0 < 0) { n0 = 0; } typedef _carray_base_t<char> T; return ((T*)ps)->realloc(n0, 0, 0, 0); } virtual char* _pd(void* ps __bmdx_noarg) const __bmdx_noex { typedef _carray_base_t<char> T; return ((T*)ps)->pd(); } };
      struct _stra_rba;

      static int _load_bytes(const char* fnp, const _stra_base& a, void* ps __bmdx_noarg) __bmdx_noex
      {
        if (!fnp) { return -1; }
        file_io f; f.open(fnp, false); if (!f.is_open()) { return f.result() == -3 ? 0 : -2; }
        f.seek_end(); if (f.result() < 1) { return -2; }
        _s_ll n0 = f.tell(); if (n0 < 0) { return -2; }
        f.seek(0); if (f.result() < 1) { return -2; }
        if (_s_ll(size_t(n0)) != n0) { return -1; }
        if (!a._resize(ps, n0)) { return -1; }
        if (!n0) { return 1; }
        f.read(a._pd(ps), size_t(n0)); if (f.result() < 1) { a._resize(ps, -1); return -2; }
        return 1;
      }
      static int _save_bytes(const char* fnp, const char* pdata, _s_ll n0, bool b_append __bmdx_noarg) __bmdx_noex
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

} // end namespace bmdx



#include <map>

#ifdef _bmdxpl_Psx
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>

  #if 0
  #elif __APPLE__ && __MACH__
  #elif defined(__FreeBSD__)
  #elif defined(__SUNPRO_CC) || defined(__sun)
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



  // Main namespace for shared memory API .
namespace bmdx_shm
{
namespace _bmdx_shm { namespace _api {} } using namespace _bmdx_shm::_api;


#undef __bmdx_shm_name_length
#define __bmdx_shm_name_length 300
typedef bmdx_str::flstr_t<__bmdx_shm_name_length> t_name_shm;
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
    #elif defined(__SUNPRO_CC) || defined(__sun)
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
using namespace bmdx_str::words;


#if defined(__ANDROID__)

    // Data structure for transferring a single file descriptor via socket.
  struct _socket_msg_1fd
  {
      // fd specifies the initial value of the descriptor field in the message.
    _socket_msg_1fd(int fd = -1)    { _ndesc[0] = 1; _v1.iov_base = _ndesc; _v1.iov_len = sizeof(_ndesc); _m.msg_name = 0; _m.msg_namelen = 0; _m.msg_iov = &_v1; _m.msg_iovlen = 1; _m.msg_flags = MSG_DONTWAIT; _m.msg_control = _cmsg.buf; _m.msg_controllen = sizeof(_cmsg.buf);  struct cmsghdr* pcm1 = CMSG_FIRSTHDR(&_m);  pcm1->cmsg_level = SOL_SOCKET; pcm1->cmsg_type = SCM_RIGHTS; pcm1->cmsg_len = CMSG_LEN(sizeof(int) * 1);  int* pdata = (int*)CMSG_DATA(pcm1); pdata[0] = fd; }

    int fd() const        { int* pdata = (int*)CMSG_DATA(CMSG_FIRSTHDR(&_m)); return pdata[0]; }
    int& fd()    { int* pdata = (int*)CMSG_DATA(CMSG_FIRSTHDR(&_m)); return pdata[0]; }

    struct msghdr& m()    { return _m; }
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
    _s_long res() const { return _res; }

      // Factual socket name, used in socket creation. Starts with char '\0'. See key() for original name processing scheme.
    const t_name& name() const        { return _name; }
      // -1..99999. See key() for original socket name processing scheme.
    _s_long n_role() const { return _n_role; }

      // Socket name: limit_length('\0' + name_prefix + str(n_role >= 0 ? n_role % 100000 : "") + name, sizeof(sockaddr_un::sun_path)).
    static t_name key(const t_name& name_orig, _s_long n_role = -1, const char* name_prefix = "")    { t_name _name; _name += '\0'; _name += name_prefix; if (n_role >= 0) { _name += n_role % 100000; } _name += name_orig; if (_name.n() > _s_long(sizeof(_a.sun_path))) { _name.resize(_s_long(sizeof(_a.sun_path))); } return _name; }

      // b_srv true: server side, false: client side. See key() for name processing scheme.
    _asocket_io(bool b_srv, const t_name& name_orig, _s_long n_role = -1, const char* name_prefix = "")    { _name = key(name_orig, n_role, name_prefix); _n_role = n_role < 0 ? -1 : n_role % 100000; _b_srv = b_srv; _h1 = _h2 = -1; _state = 0;  _state = 0; _res = -4; _a.sun_family = PF_LOCAL; memcpy(&_a.sun_path, _name.c_str(), size_t(_name.n())); }


      // Automatically connects or accepts if necessary, then sends message m.
      //  b_close == true: close connection after message being sent successfully.
      //  Returns:
      //    1 - completed.
      //    0 - partially completed. Sleep and retry.
      //    -1 - Server: no peer connected yet. Client: server is not responding (does not exist?). Maybe sleep and retry.
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
      //    -1 - Server: no peer connected yet. Client: server is not responding (does not exist?). Maybe sleep and retry.
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
    static bool fcntl_set_nonblock(int fd)    { int f = fcntl(fd, F_GETFL); if (f != -1) { f = fcntl(fd, F_SETFL, f | O_NONBLOCK); } return f == 0; }

      // Try to automatically create a socket, bind/listen/accept or connect.
      // 1 - done. try_send, try_receive ready to transfer data.
      // -1 - Server: no peer connected yet (accept returned "wait" state). Client: server is not responding (does not exist?), or connection pending. Maybe sleep and retry.
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

  private: bool _b_srv; int _h1, _h2; t_name _name; sockaddr_un _a; mutable _s_long _state; mutable _s_long _res; _s_long _n_role;
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
      //    -4 - search in progress, file descriptor is still unknown. On t_retry_mcs == 0, this means that currently no fd exposed by any process.
      //    -5 - file descriptor exists, but too large delay during transfer occurred. The client must assume that the descriptor exists and call fd_search again later.
    int fd_search(const t_name& name_orig, _s_long t_retry_mcs, _s_long n_role_expose) __bmdx_noex    { return ((p_fd_search)pff[0])(*this, name_orig, t_retry_mcs, n_role_expose); }

      // Try to share a duplicate of the given *io_fd with other processes under the given name.
      //  fd:
      //    >= 0. Associate a duplicate of this fd with the name.
      //    -1. -1 Associate an "invalid" (-1) descriptor value with the name.
      //    -2. Do not process fd, n_role, b_overwrite, instead, if name exists, set only b_expose for this name.
      //  n_role: [-1..n_role_max]. A number to prepend to original name. -1 - do not prepend anything.
      //  b_expose:
      //    true - if an association exists or just created, enable exposing the associated fd to other processes.
      //    false - disable exposing is it was enabled (but do not remove name-fd association as such).
      // Returns:
      //  1 - success.
      //  0 (only on b_overwrite == false) - the name-descriptor association already exists, use fd_search to get it.
      //  -1 - wrong fd (<= -3).
      //  -2 - failure (multiple cases).
      //  -3 (only on fd == -2) - failed to enable exposing - name not found.
    int fd_expose(const t_name& name_orig, int fd, _s_long n_role, bool b_overwrite, bool b_expose) __bmdx_noex    { return ((p_fd_expose)pff[1])(*this, name_orig, fd, n_role, b_overwrite, b_expose); }

      // Remove the name and all associated tasks, close file descriptor if it exists.
      //  1 - removed.
      //  0 - object with the given name does not exist.
      //  -2 - failure.
    int fd_remove(const t_name& name_orig) __bmdx_noex    { return ((p_fd_remove)pff[2])(*this, name_orig); }

      // Remove all names and search tasks, stop (join with) service thread.
    void reset(bool b_wait) __bmdx_noex    { ((p_reset)pff[3])(*this, b_wait); }

    _fd_sharing() { pff[0] = (void*)&_fd_search; pff[1] = (void*)&_fd_expose; pff[2] = (void*)&_fd_remove; pff[3] = (void*)&_reset; }
private:
    typedef int (*p_fd_search)(_fd_sharing& par, const t_name& name_orig, _s_long t_retry_mcs, _s_long n_role_expose);
    typedef int (*p_fd_expose)(_fd_sharing& par, const t_name& name_orig, int fd, _s_long n_role, bool b_overwrite, bool b_expose);
    typedef int (*p_fd_remove)(_fd_sharing& par, const t_name& name_orig);
    typedef void (*p_reset)(_fd_sharing& par, bool b_wait);
    void* pff[4];

    struct job
    {
      _s_long state; // 0 - fd is unknown and search disabled, 1 - fd is unknown, but searching, 2 - fd is known (+ sending fd to other processes on b_mayexpose == true)
      volatile int fd; // may be read w/o locking; -1 - "no file", -2 - "fd still unknown", >=0 - known fd
      bool b_mayexpose;
      bool b_conn1;
      _s_long nrexp;
      _s_long nrsrch;
      critsec_t<job>::csdata csd; // individual lock for this job instance
      t_name name; // original name, associated with file descriptor (not formatted/prefixed to serve as socket name)
      _asocket_io s;
      _socket_msg_1fd m1;
      job(const t_name& name_, _s_long n_role_expose_) : name(name_), s(false, name_, -1, "F\t") { state = 0; fd = -2; b_mayexpose = true; nrexp = n_role_expose_; b_conn1 = true; nrsrch = -1; if (nrexp < 0) { nrexp = -1; } else if (nrexp > n_role_max) { nrexp = n_role_max; } } // cm_create0 may gen. exc.
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
    static int _fd_search(_fd_sharing& par, const t_name& name_orig, _s_long t_retry_mcs, _s_long n_role_expose)
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
        _s_long nrprev;
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
    static int _fd_expose(_fd_sharing& par, const t_name& name_orig, int fd, _s_long n_role, bool b_overwrite, bool b_expose)
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
              if (!jj.resize(_s_ll(m.size()))) { break; }
              _s_ll i = 0;
              for (t_map1::const_iterator q = m.begin(); q != m.end(); ++q, ++i) { jj[i] = q->second; }
            }
            for (_s_ll i = 0; i < jj.n(); ++i) { if (jj[i]) { nj += jj[i]._pnonc_u()->periodic(); } }
          } catch (...) {} } while (false);
          if (nj == 0) { sleep_mcs(10000); }
        }
      }
    };
  };

#endif



namespace _api // public API, merged into namespace bmdx_shm
{

    // Non-blocking, non-locking queue for bytes, for two threads (sender, receiver).
    //  Designed to work as shared object in IPC (may also be used by threads inside one process).
    //  Details: see rfifo_nbl11::init_ref().
  struct rfifo_nbl11
  {
      // NOTE Buffer object, created with constructor, will have minimal size == n0 bytes.
      //  Buffer object, referenced by pointer cast, will have size >= n0 bytes.
    enum { n0 = 8 };

      // NOTE Copying and assignment do not copy anything from source object.
      //   This is not the main intended way of using rfifo_nbl11. See init().
    rfifo_nbl11() : _ipush(0), _ipop(0), _n(n0) {}
    rfifo_nbl11(const rfifo_nbl11&) : _ipush(0), _ipop(0), _n(n0) {}
    void operator=(const rfifo_nbl11&) {}



    // Initializer side.

      // Initialize the buffer object.
      // n_ - place (number of bytes) for queued data.
      // rfifo_nbl11 is kept in memory together with queue data, in single linear area.
      //  Queue data array starts inside rfifo_nbl11 object (offset == +24).
      //  Fixed area for data, included into rfifo_nbl11 object, is n0 == 8 bytes.
      // If n_ > n0, additional place for data is taken adjacent to the end of rfifo_nbl11 object.
    void init_ref(_s_ll n_, _s_ll ipush_ = 0, _s_ll ipop_ = 0) __bmdx_noex { if (sizeof(void*) < 8) { n_ = bmdx_minmax::myllmin(n_, 1ll << 31); } _n = n_ < 1 ? 1 : n_; _ipush = ipush_; _ipop = ipop_; }

      // Data buffer size.
    _s_ll n() const __bmdx_noex { return _n; }



    // Supplier side.

      // The current push position, and the number of bytes (volatile) that may be pushed now.
    _s_ll ipush() const __bmdx_noex { return _ipush; }
    _s_ll nfree() const __bmdx_noex { return _n - navl(); }
    void _sndr_set_ipush(_s_ll ipush) __bmdx_noex { _ipush = ipush; } // for "manual" updating push position if needed

      // If nmax > 0, pushes min(nmax, npush()) bytes from p into the queue.
      // Returns the number of bytes actually pushed (>=0).
      // If the buffer is valid (indexes are not corrupted), push() is faultless.
      // Pushing is transactional: ipush() is increased only once, at the end of successful operation.
    _s_ll push(const char* p, _s_ll nmax) __bmdx_noex
    {
      if (!p) { return 0; }
      if (nmax <= 0) { return 0; }
      _s_ll i2 = _ipush, i1 = _ipop;
      _s_ll n = _n - (i2 - i1);
      _s_ll __np = nmax >= n ? n : _s_ll(nmax);
      _s_ll n1 = bmdx_minmax::myllmin(__np, n, _n - _s_ll(i2 % _n));
      if (n1 > 0)
      {
        bmdx_str::words::memmove_t<char>::sf_memcpy(&d[0] + _ipush % _n, p, size_t(n1));
        p += n1;
        _s_ll n2 = __np - n1;
        if (n2 > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(&d[0] + (_ipush + n1) % _n, p, size_t(n2)); }
        _ipush += __np; // the last action "commits" pushing (important if the object is in shared memory)
       }
      return __np;
    }

      // Same as push(), only pushes byte b up to nmax times.
      // If the buffer is valid (indexes are not corrupted), push_bytes() is faultless.
      // Pushing is transactional: ipush() is increased only once, at the end of successful operation.
    _s_ll push_bytes(char b, _s_ll nmax) __bmdx_noex
    {
      if (nmax <= 0) { return 0; }
      _s_ll i2 = _ipush, i1 = _ipop;
      _s_ll n = _n - (i2 - i1);
      _s_ll __np = nmax >= n ? n : _s_ll(nmax);
      _s_ll n1 = bmdx_minmax::myllmin(__np, n, _n - i2 % _n);
      if (n1 > 0)
      {
        std::memset(&d[0] + _ipush % _n, b, size_t(n1));
        _s_ll n2 = __np - n1;
        if (n2 > 0) { std::memset(&d[0] + (_ipush + n1) % _n, b, size_t(n2)); }
        _ipush += __np; // the last action "commits" pushing (important if the object is in shared memory)
      }
      return __np;
    }


    // Receiver side.

      // The current pop position, and the number of bytes (volatile) that may be popped now.
    _s_ll ipop() const __bmdx_noex { return _ipop; }
    _s_ll navl() const __bmdx_noex { return _ipush - _ipop; }
    void _rcv_set_ipop(_s_ll ipop) __bmdx_noex { _ipop = ipop; } // for skipping data if needed

      // How many bytes, which may be popped, are located contiguously. [0..npop()].
    _s_ll navl_contig() const __bmdx_noex { _s_ll i2 = _ipush, i1 = _ipop; return bmdx_minmax::myllmin(i2 - i1, _n - i1 % _n); }

      // Pointer to the next byte that would be popped.
    const char* peek1() const __bmdx_noex { return &d[0] + _ipop % _n; }

      // If nmax > 0, pops min(nmax, npop()) bytes from the queue into pdest.
      // If b_do_pop == false, ipop() is not increased, so the client just gets a copy of buffer data.
      // pchs: signed 32-bit checksum accumulator (each char unsigned value is added to *pchs).
      // Returns the number of bytes actually popped (>=0).
      // NOTE popping is transactional (ipop() is increased only once, at the end of successful operation).
    _s_ll pop(char* pdest, _s_ll nmax, bool b_do_pop = true, _s_long* pchs = 0) __bmdx_noex
    {
      if (!pdest) { return 0; }
      if (nmax <= 0) { return 0; }
      _s_ll i1 = _ipop, i2 = _ipush;
      _s_ll n = i2 - i1;
      _s_ll __np = nmax >= n ? n : _s_ll(nmax);
      _s_ll n1 = bmdx_minmax::myllmin(__np, n, _n - i1 % _n);
      if (n1 > 0)
      {
        bmdx_str::words::memmove_t<char>::sf_memcpy(pdest, &d[0] + _ipop % _n, size_t(n1), pchs);
        pdest += n1;
        _s_ll n2 = __np - n1;
        if (n2 > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(pdest, &d[0] + (_ipop + n1) % _n, size_t(n2), pchs); }
        if (b_do_pop) { _ipop += __np; } // the last action "commits" popping (important if the object is in shared memory)
      }
      return __np;
    }


      // If nmax > 0, skip min(nmax, npop()) bytes.
      // Returns the number of bytes actually skipped (>=0).
      // NOTE discarding is transactional (ipop() is increased only once, at the end of successful operation).
    _s_ll discard(_s_ll nmax) __bmdx_noex
    {
      if (nmax <= 0) { return 0; }
      _s_ll i2 = _ipush, i1 = _ipop;
      _s_ll n = i2 - i1;
      _s_ll __np = nmax >= n ? n : nmax;
      _ipop += __np; // the last action "commits" discarding (important if the object is in shared memory)
      return __np;
    }

  private:
    volatile _s_ll _ipush;
    volatile _s_ll _ipop;
    _s_ll _n;
    char d[n0]; // first bytes of the buffer
  };





    // Global (interprocess) named lock (mutex) control.
    //    The lock works between processes, threads, and inside a thread.
    //    Only one critsec_gn object with particular name may have its lock set at any moment.
    //    At the same time, locking any other objects with same name will fail.
    //    All operations are non-blocking.
    // NOTE critsec_gn object is not associated with the thread that created it or successfully locked it.
    //  It can be shared between threads, e.g. with smart pointer,
    //    and deleted by thread other than one that had set successful lock.
    //  Limitations:
    //    1. critsec_gn itself is not protected from concurrent access.
    //      This is responsibility of the client.
    //    2. critsec_gn lock() and unlock() are not counted.
    //      This should be done by the client if it needs.
    //      lock() tries to lock if the object is not locked, and does nothing if it's already locked.
    //      unlock() does unlocking if the object is locked, otherwise it does nothing.
    //    3. If a thread contains or references locked critsec_gn object,
    //      and that thread has been terminated such that the object is not deleted,
    //      the lock is not released until the process exits or terminates, or anyone else, referring
    //      to the same critsec_gn object, unlocks it.
  struct critsec_gn
  {
    typedef t_name_shm t_name;

      // critsec_gn(name_): copies object name with possible formatting, suitable for system,
      //  and does nothing more.
      //  See also name().
    critsec_gn(const char* name_, f_shm_name_prefix prefix_cs = &f_prefix_critsec_gn __bmdx_noarg) __bmdx_noex
      { _pdestr = _destr; __pad1 = 0; _b_locked = false; _setname(name_, prefix_cs ? prefix_cs() : f_prefix_critsec_gn()); _init0(); }

      // Factual name, used for creating the global object in the system.
      //  1. name() is different from name_ arg. of critsec_gn constructor.
      //  2. name() usually includes system-dependent prefix and maybe few additional characters.
      //  3. name().n() > 0, and may be limited as system requires (e.g. MAX_PATH).
      //  4. name() may contain null characters.
    const t_name& name() const __bmdx_noex { return _name; }

      // true: the named object already exists, is opened and is locked.
      // false: in any other case.
    bool b_opened(__bmdx_noarg1) const __bmdx_noex { return _b_handle(); }

      // true: the named object already exists, is opened and is locked.
      // false: in any other case.
    bool b_locked(__bmdx_noarg1) const __bmdx_noex { return _b_locked; }

      // 1. If the named object already exists, return true.
      // 2. Otherwise, open or create the named object. Return true (success) or false (failure).
    bool cr_op(__bmdx_noarg1) __bmdx_noex { _init1(); return b_opened(); }

      // 1. If the named object already exists and is locked, return true.
      // 2. Otherwise, open or create the named object if this is not done yet. Return false if failed.
      // 3. Lock the named object. Return true (success) or false (failure).
    bool lock(__bmdx_noarg1) __bmdx_noex { _lock(); return _b_locked; }

      // Unlock the named object (but do not release it).
    void unlock(__bmdx_noarg1) __bmdx_noex { _unlock(); }

      // Unlock and release the named object.
      //  The next call to lock() will re-open the object + lock it.
    void close(__bmdx_noarg1) __bmdx_noex { _reset(); }

      // The destructor automatically unlocks and releases the named object.
    ~critsec_gn() { _pdestr(this); }

  private:
    critsec_gn(const critsec_gn&); void operator=(const critsec_gn&);
    void (*_pdestr)(critsec_gn* p); static void _destr(critsec_gn* p) { p->_reset(); }
    t_name _name;
    union { bool _b_locked; _s_long __pad1; };

    #ifdef _bmdxpl_Wnds

      HANDLE hs;
      bool _b_handle(__bmdx_noarg1) const { return !!hs; } // true: named object created or opened
      void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '\\') { c = '_'; } _name += c; } }
      void _init0(__bmdx_noarg1) { hs = 0; } // platf.-dependent constructor
      void _init1(__bmdx_noarg1) { if (!hs) { hs = CreateSemaphoreA(0, 1, 1, _name.c_str()); } }
      void _lock(__bmdx_noarg1) { _init1(); if (_b_locked || !_b_handle()) { return; } DWORD res_s = WaitForSingleObjectEx(hs, 0, FALSE); _b_locked = res_s == WAIT_OBJECT_0; }
      void _unlock(__bmdx_noarg1) { if (_b_locked) { BOOL b = ReleaseSemaphore(hs, 1, 0); (void)b; _b_locked = false; } }
      void _reset(__bmdx_noarg1) { if (hs) { _unlock(); CloseHandle(hs); hs = 0; } }

    #endif
    #ifdef _bmdxpl_Psx
      #if 0
      #elif defined(__SUNPRO_CC) || defined(__sun)

        int h; int _perm; mutex_t* pm; _s_long uid; _s_long uid_fsh;
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
          if (!(pm2 && pm2 != MAP_FAILED)) { ::close(h2); return; }

          _s_long* puidgen = (_s_long*)(pm2+1); _s_long* pflag = puidgen + 1;
          _s_long pid1 = pflag[1];
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
          int h2 = h; mutex_t* pm2 = pm; _s_long* puidgen = (_s_long*)(pm2+1); _s_long* pflag = puidgen + 1;

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
          int h2 = h; mutex_t* pm2 = pm; _s_long* puidgen = (_s_long*)(pm2+1); _s_long* pflag = puidgen + 1;

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
        void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '/') { c = '_'; } _name += c; } _s_long nmax = sizeof(sockaddr_un) - sizeof(sa_family_t); if (_name.n() > nmax - 1) { _name.resize(nmax - 1); } }
        void _init0(__bmdx_noarg1) { h = -1; _bsock = false; } // platf.-dependent constructor
        void _init1(__bmdx_noarg1) { if (h != -1) { return; } int h2 = ::socket(PF_LOCAL, SOCK_STREAM, 0); if (h2 != -1) { h = h2; _bsock = true; } } // named object creation
        void _lock(__bmdx_noarg1) { _init1(); if (_b_locked || !_b_handle()) { return; } sockaddr_un a; a.sun_family = PF_LOCAL; bmdx_str::words::memmove_t<char>::sf_memcpy(&a.sun_path, _name.c_str(), size_t(_name.n())); _b_locked = ::bind(h, (sockaddr*)&a, sizeof(sa_family_t) + size_t(_name.n())) != -1; } // check b_locked() after return
        void _unlock(__bmdx_noarg1) { if (_b_locked) { ::close(h); h = -1; _b_locked = false; } }
        void _reset(__bmdx_noarg1) { if (h != -1) { ::close(h); h = -1; _b_locked = false; _bsock = false; } } // unlock mutex, release (close) the named object

      #elif __APPLE__ && __MACH__

        // NOTE critsec_gn uses flock to implement global, thread-independent locks on temporary files.
        //    There's potential issue: files in /tmp may survive system reboot,
        //    and the simple open/create/lock approach would leave uncontrolled number of files in /tmp
        //    for undefined term.
        //  The below implementation overcomes the problem by deleting the referred file on critsec_gn object reset.
        //  See _reset() and _lock(): they are implemented differently from dflt. for *nix systems.

        static int cmp_fd(int h1, const char* name2 __bmdx_noarg) { struct stat s[2]; std::memset(&s[0], 0, 2 * sizeof(s[0])); if (::fstat(h1, &s[0]) != 0 || ::stat(name2, &s[1]) != 0) { return -1; } return (s[0].st_dev == s[1].st_dev) && (s[0].st_ino == s[1].st_ino); }
        int h; int _perm;
        bool _b_handle(__bmdx_noarg1) const { return !!h; } // true: named object created or opened
        void _setname(const char* name, const t_name& prefix __bmdx_noarg) { _name = prefix; if (!name) { return; } while(*name) { char c = *name++; if (c == '/') { c = '_'; } _name += c; } }
        void _init0(__bmdx_noarg1) { h = 0; _perm = 0666; } // platf.-dependent constructor
        void _init1(__bmdx_noarg1) { if (h) { return; } int h2 = ::open(_name.c_str(), O_CREAT, _perm); if (h2 != -1) { h = h2; } } // named object creation

          // Since _reset() in any other critsec_gn may delete the file during owning the lock,
          //  the valid handle this->h may point to "stale" file without name in the file system.
          // To overcome that:
          //  _lock() makes an attempt of locking this->h,
          //    and after that checks if this->_name exists and points to the same file as this->h.
          //  If yes (this->h <=> this->_name), _lock() assumes the obtained locking result
          //    (winning/losing the concurrency) as actual, and returns.
          //  If not (i.e. this->h is "stale"):
          //    1. unlock (if was locked) and close this->h.
          //      (The file will disappear automatically, because it has no name already.)
          //    2. repeat open/create/lock/check_for_stale sequence,
          //      until the gained locking result will refer to the actual, named file.
          // See also _reset().
        void _lock(__bmdx_noarg1)
        {
          if (_b_locked) { return; }
          while (1)
          {
            _init1();
            if (!_b_handle()) { return; }
            _b_locked = flock(h, LOCK_NB | LOCK_EX) == 0;
            if (cmp_fd(h, _name.c_str()) == 1) { return; }
            _unlock(); ::close(h); h = 0;
          }
        }

        void _unlock(__bmdx_noarg1) { if (_b_locked) { flock(h, LOCK_NB | LOCK_UN); _b_locked = false; } }

          // On resetting critsec_gn, if it owns a lock or succeeds in setting it,
          //  the file that is locked is deleted.
          // This not only signals that the lock is unset,
          //  but keeps /tmp directory as clean as possible.
          // See also _lock().
        void _reset(__bmdx_noarg1) { if (h) { _lock(); if (_b_locked) { ::unlink(_name.c_str()); } _unlock(); ::close(h); h = 0; } }

      #else

        // The default implementation of global lock: based on flock() on temporary file.

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


  struct _shmobj2s_t_exceptions { struct exc_shmobj2s : std::exception { const char* what() const __bmdx_noex { return "exc_shmobj2s::operator->: p==0"; } }; };


    // Wrapper for an object in shared memory, with two-sided access (2 separate locks).
  template<class T>
  struct shmobj2s_t
  {
    private: struct _shm_sems;
    public:
    struct exc_shmobj2s : _shmobj2s_t_exceptions::exc_shmobj2s {};

      // The pointer to nb bytes of shared memory, if shared memory is opened and compatible,
      //   or created and successfully initialized.
      // Otherwise 0.
    T* p() const { return _p; }
    T* operator->() const __bmdx_exs(exc_shmobj2s)    { if (!_p) { throw exc_shmobj2s(); } return _p; }


      // (Constructor-defined parameter.)
      // Role (assigned on shmobj2s_t construction).
    bool b_side1(__bmdx_noarg1) const        { return _b_side1; }

      // Returns:
      //    >= 0 - the amount of memory for placement of T, allocated on shared memory area creation.
      //      This value may be != ctor. arg. nb, if object already exists. Should be checked to satisfy client needs.
      //      For creating side - if the object already exists with wrong size, it can be destroyed and recreated as necessary.
      //      For non-creating side - if the object already exists with wrong size, it should be closed
      //        and reopened after the creating side making some corrections (if it can do it at all).
      //    0 - a) the shared memory cannot be accessed now, b) not initialized yet.
      // NOTE Positive nb() >= nb arg., passed to shmobj2s_t constructor.
    _s_ll nb(__bmdx_noarg1) const        { return _pdesc() ? _pdesc()->nbt : 0; }

      // Name of the shared object, as passed to shmobj2s_t constructor.
      //  The name, really used as argument to shm_open or CreateFileMapping,
      //    may differ from name():
      //      a) cut to MAX_PATH or PATH_MAX characters,
      //      b) converted to shorter form, in the style of hash function, to match more severe name length limitation.
      //        See e.g. __bmdx_shm_name_max.
      // Returns:
      //  non-0 pointer to the name.
    const t_name_shm& name() const        { return _name; }


      // Determines if this shmobj2s_t holds the specified lock on the referenced object.
      //    (This does not include any locks created on the client side.)
      // Returns:
      // a) True if the object lock is set:
      //    a)) side == 1 - the lock of side 1 is checked.
      //    b)) side == 2 - the lock of side 2 is checked.
      //    c)) side == 4 on b_side1() == true - the lock of side 1 is checked.
      //    d)) side == 4 on b_side1() == false - the lock of side 2 is checked.
      // b) False is returned if the lock is not valid or side arg. wrong.
    bool b_locked(_s_long side = 4 __bmdx_noarg) const        { if (side == 4) { side = b_side1() ? 1 : 2; } if (side == 1) { return _lkmyside.b_lk1(); } if (side == 2) { return _lkmyside.b_lk2(); } return false; }

      // The contained object state:
      //  0 - not constructed yet, 1 - constructed (valid), 2 - destroyed. p() != 0.
      //  -1 - unknown (failure). p() == 0.
    _s_long f_constructed(__bmdx_noarg1) const { if (!_pdesc()) { return -1; } if (_pdesc()->b_destroyed) { return 2; } if (_pdesc()->b_constructed) { return 1; } return 0; }



      // (Informational) Shared memory overhead to keep shmobj2s_t's _shmdesc structure.
    static _s_long nbover(__bmdx_noarg1)    { return sizeof(_shmdesc); }



      // Hints on shared memory area creation and handling.
    struct hints
    {
      typedef t_name_shm t_name;

        // htype (!=0): unique identifier for type of T to be recognized in any other process.
        //        4-byte signed integer, stored in the beginning of allocated shared memory area.
        //        Default is sizeof(T).
        //        Customizing: supply a negative value, uniquely specifying custom category of the referenced object,
        //          known to all processes that are using it.
      _s_long htype;

        // prefix_cs(): returns system and application-dependent semaphore name prefix.
        //        Default: == f_prefix_critsec_gn.
        //        Customizing: may be set to another value if particular system settings are different from its standard.
      t_name (*prefix_cs)();

        // prefix_m(): returns system and application-dependent shared memory name prefix.
        //        Default: == f_prefix_shmobj2s.
        //        Customizing: may be set to another value if particular system settings are different from its standard.
      t_name (*prefix_m)();


      hints() : htype(_s_long(sizeof(T))), prefix_cs(&f_prefix_critsec_gn), prefix_m(&f_prefix_shmobj2s) {}
      hints(_s_long htype_) : htype(htype_), prefix_cs(&f_prefix_critsec_gn), prefix_m(&f_prefix_shmobj2s) {}
    };




      // Remembers the name, role, memory size, parameters. Does nothing else.
      //    name: unique object name (C-string).
      //      May be 0 or "", but this is not recommended.
      //      The corresponding real name for global system object may be a modified form of name arg, suitable for particular platform.
      //      Guidelines for name arg., working well across platforms:
      //        1. Do not use slashes (/, \) in the name.
      //        2. Preferably, do not use non-alphanumeric characters except (_, -, !, space).
      //        3. Keep name length <250 characters.
      //    Role (b_side1):
      //      a) true: on prepare(), side 1 will automatically create/initialize shared memory area.
      //      b) false: on prepare(), side 2 only tries to open the memory and check if it's already initialized with compatible
      //          shmobj2s_t from this or other project.
      //    nb (autolimited to >= 0): hint for number of bytes to allocate for object of type T.
      //        May be larger than sizeof(T) for objects of variable size.
      //        nb is used
      //            1) by creating side (side 1), when the shared memory object header is initialized first time.
      //            2) by any side, at the first attempt to open the existing shared memory.
      //              If the existing memory is larger, it is at once reopened at the size, specified in the object header.
      //            When the memory is open (prepare() returned 1), this->nb() specifies the factual size.
      //              The client should check this->nb(), to ensure that existing area size satisfies application-specific constraints.
      //            NOTE Gross size of allocated shared memory is max(0, nb) + nbover().
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
    shmobj2s_t(const char* name, bool b_side1, _s_ll nb = sizeof(T), const hints& hints_ = hints() __bmdx_noarg) __bmdx_noex    { _pdestr = _destr; _p = 0; _b_side1 = b_side1; _hh = hints_; _hh.htype = hints_.htype != 0 ? hints_.htype : -1; _name = hints_.prefix_m ? hints_.prefix_m() : f_prefix_shmobj2s(); _name += name; _ctor_nbt = bmdx_minmax::myllmax(nb, 0); _rsems.cm_create2(0, 0, 1, _name.c_str(), hints_.prefix_cs); _shm_init0(); }


      // Open/create/initialize/lock the shared memory in role as specified by b_side1 on shmobj2s_t construction.
      //
      //  b_keeplk:
      //    1) Lock the memory at least for the current side (b_side1()).
      //    2) Hold all global locks set after prepare() returns. (Additional locks may be set due to sides_cr != 0.)
      //    true:
      //      a) for the side that may initialize the object:
      //          try to set that side's lock, allocate shared memory if not yet, initialize state variables if not yet, return results.
      //      b) for the side that may NOT initialize the object:
      //          try to set that side's lock, try to get shared memory pointer, check initialization, return results.
      //    false:
      //      a) for the side that may initialize the object:
      //          try to set that side's lock, allocate shared memory if not yet, initialize state variables if not yet, release the lock, return results.
      //      b) for the side that may NOT initialize the object:
      //          try to get shared memory pointer, check initialization, return results.
      //
      //  sides_cr: OR-ed flags, showing which sides may create and initialize shared memory area if it does not exist.
      //    0x1 - side 1 (by default).
      //    0x2 - side 2.
      //    NOTE If the current side (b_side1()) cannot create the shared object (because of the given sides_cr),
      //      prepare() does not lock the opposite side even if it's mentioned in sides_cr.
      //    Namely, complete set of locks, requested by prepare(), is defined as:
      //      ( (b_side1() && (sides_cr & 1)) || (!b_side1() && (sides_cr & 2)) ? sides_cr : 0 ) | ( b_keeplk ? (b_side1() ? 1 : 2) : 0 )
      //
      // Returns:
      //    1 - the memory is available (created and initialized, or already opened). b_opened() == true.
      //    -1 - the shared memory exists, but its structure is not compatible (see htype arg. in shmobj2s_t()).
      //        The memory handle (in the current shmobj2s_t) has been closed.
      //    -2 - failure during operation (it's generally unknown if the shared object is available).
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
    _s_long prepare(bool b_keeplk, _s_long sides_cr = 1 __bmdx_noarg)    { return _prep(b_keeplk, sides_cr); }



      // Construct T at this->p if it can be done (!b_bad() && f_constructed() == 0 or 2).
      //    This may require a) synchronization, and/or b) agreement of what side creates the object.
      //    (This is responsibility of the client, depending on T and client implementation.)
      // Returns:
      //    true: shared memory is open and this->p now points to valid object (constructed or already existed).
      //    false: otherwise.
      //    exception: only if no_exc == false, and T() generated an exception.
    bool obj_create0(bool no_exc  __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1> bool obj_create1(bool no_exc, const A1& x1 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2> bool obj_create2(bool no_exc, const A1& x1, const A2& x2 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3> bool obj_create3(bool no_exc, const A1& x1, const A2& x2, const A3& x3 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3, class A4> bool obj_create4(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3, class A4, class A5> bool obj_create5(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6> bool obj_create6(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5, x6); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool obj_create7(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5, x6, x7); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
    template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool obj_create8(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T(x1, x2, x3, x4, x5, x6, x7, x8); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }

          // Same as obj_create#(), but args. are cast to non-const when passed to constructor.
        template<class A1> bool obj_create1nc(bool no_exc, const A1& x1 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2> bool obj_create2nc(bool no_exc, const A1& x1, const A2& x2 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3> bool obj_create3nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3, class A4> bool obj_create4nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3, class A4, class A5> bool obj_create5nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6> bool obj_create6nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool obj_create7nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }
        template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool obj_create8nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8 __bmdx_noarg) { _s_long q = this->f_constructed(); if (q == 1) { return true; } if (q == 0 || q == 2) { try { new (_p) T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); set_f_constructed(1); return true; } catch (...) { if (!no_exc) { throw; } } } return false; }

      // Explicitly call the destructor, if it can be done (!b_bad() && f_constructed() == 1).
      // If T is accessible and currently constructed, obj_destroy calls ~T(), and after, marks the object it destroyed.
      //    This may require a) synchronization, and/or b) agreement of what side destroys the object.
      //    (This is responsibility of the client, depending on T and client implementation.)
      // The object state may be checked after with f_constructed().
      // NOTE 1) Calling ~T most probably is not needed, because the object T disappears once shared memory is released,
      //      and, by design, does not keep any resource handles.
      //      (Such keeping would lead to inconsistent state in case of failure of the process responsible for destructor calling.)
      //    2) From other side, creating/destroying an object may be used as simple 1-element queue between processes,
      //      because of clear tracking via f_constructed().
    void obj_destroy(__bmdx_noarg1) { if (!(!b_bad() && f_constructed() == 1)) { return; } try { _p->~T(); } catch (...) {} set_f_constructed(2); }

      // Sets f_constructed to new value (0, 1, 2) if possible. Check f_constructed() after that.
      //  NOTE set_f_constructed is not synchronized (this is responsibility of the client, depending on its implementation).
    void set_f_constructed(_s_long state) const        { if (!_pdesc()) { return; } if (state == 0) { _pdesc()->b_constructed = 0; _pdesc()->b_destroyed = 0; } if (state == 1) { _pdesc()->b_constructed = 1; _pdesc()->b_destroyed = 0; } if (state == 2) { _pdesc()->b_destroyed = 1; _pdesc()->b_constructed = 1; } }



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
    ~shmobj2s_t() { _pdestr(this); }


    struct shared_lock
    {
        // true if side 1 lock is set.
      bool b_lk1() const __bmdx_noex    { return vlk1 >= 0; }
        // true if side 2 lock is set.
      bool b_lk2() const __bmdx_noex    { return vlk2 >= 0; }
        // true if any of he locks is set.
      bool b_lk_any() const __bmdx_noex    { return b_lk1() || b_lk2(); }

        // Create an empty object (no any locks).
      shared_lock(__bmdx_noarg1) : vlk1(-1), vlk2(-1) { _pdestr = _destr; }

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
      shared_lock(shmobj2s_t& parent, _s_long sides = 4, _s_long timeout_ms = 0 __bmdx_noarg) __bmdx_noex;

      ~shared_lock() { _pdestr(this); }

        // After clear(), the client may safely execute new (p) shared_lock(...).
      void clear(__bmdx_noarg1) __bmdx_noex    { if (r) { r._pnonc_u()->unlock_normal(*this); } vlk1 = -1; vlk2 = -1; r.clear(); }

        // May be used if a new object is created with another set of locks than some other existing one.
      void swap(shared_lock& x __bmdx_noarg) __bmdx_noex    { bmdx_str::words::swap_bytes(*this, x); }

    private:
      friend struct _shm_sems;
      void (*_pdestr)(shared_lock* p); static void _destr(shared_lock* p) { p->clear(); }
      cref_t<_shm_sems> r; _s_ll vlk1, vlk2; shared_lock(const shared_lock&); void operator=(const shared_lock&);
    };


      // This flag indicates that both sides are marked the object as corrupted.
      //  The flag resides in shared memory.
      // Recovery sequence (optional):
      // When b_bad() == true (means "both sides marked the object as corrupted, by setting f_state1 and f_state2 to -1"):
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
      //  Special value: -1, meaning "invariant lost".
      //    Must be set by side having found a problem that cannot be solved without object reinitialization.
      //    When both flags become -1, b_bad() returns true, side 1 may notice that and reinitialize shared area.
      //  Other values meaning: user-defined.
      //    Sides may use state flags for keeping object states and simple communication.
      // NOTE Each signed char* here points to 1 char only.
    signed char* pf_state1() const        { return _pdesc() ? &_pdesc()->f_state1 : 0; }
    signed char* pf_state2() const        { return _pdesc() ? &_pdesc()->f_state2 : 0; }

  private:
    shmobj2s_t(const shmobj2s_t&); void operator=(const shmobj2s_t&);
    void (*_pdestr)(shmobj2s_t* p); static void _destr(shmobj2s_t* p) { p->_close(true); }

    struct _shmdesc_base
    {
      _s_long handler_type; // in the just opened shared memory, this is initialized last
      _s_long n_shmdesc; // == sizeof(_shmdesc)
      _s_ll nbt; // number of bytes allocated in shared memory for T
    };
    struct _shmdesc : _shmdesc_base
    {
      unsigned char b_constructed; // (NOTE must be 1st member) 0 - not constructed, 1 - constructed (and does not change when b_destroyed becomes 1)
      unsigned char b_destroyed; // 0 - not constructed or still constructed, 1 - destroyed
      signed char f_state1; // 0 - normal, char(-1) - side 1 found bad data, any else - user-defined
      signed char f_state2; // 0 - normal, char(-1) - side 2 found bad data, any else - user-defined
      // the below fields are version-dependent
      _s_long __pad1;
      _s_ll __pad2;
    };
    _shmdesc* _pdesc(__bmdx_noarg1) const { return _p ? (_shmdesc*)_p - 1 : 0; }
    void _reset_mem(__bmdx_noarg1) { if (!_pdesc()) { return; } if (_pdesc()->nbt > 0) { std::memset(_p, 0, size_t(_pdesc()->nbt)); } *(_s_long*)&_pdesc()->b_constructed = 0; }


    struct _shm_sems
    {
      typename critsec_t<_shm_sems>::csdata lkd;
      critsec_gn lkside1;
      critsec_gn lkside2;
      _s_long nlk1, nlk2;
      _s_ll vlk1, vlk2;
      _shm_sems(const char* name, f_shm_name_prefix prefix_cs __bmdx_noarg) __bmdx_noex : lkside1((critsec_gn::t_name("M1\t") += name).c_str(), prefix_cs), lkside2((critsec_gn::t_name("M2\t") += name).c_str(), prefix_cs), nlk1(0), nlk2(0), vlk1(0), vlk2(0) {}

        // If sides (ORed 0x1, 0x2) specify at least one lock,
        //  lock_normal removes all previous locks, then tries to set the new locks as specified.
      static void lock_normal(shared_lock& target, _s_long sides __bmdx_noarg) __bmdx_noex
      {
        if (!(target.r && !!(sides & 3))) { return; }
        unlock_normal(target);
        _shm_sems* ps = target.r._pnonc_u();
        critsec_t<_shm_sems> __lock(10, -1, &ps->lkd); if (sizeof(__lock)) {}
        if (!!(sides & 1)) { _s_long n = ps->nlk1; if (ps->lkside1.lock()) { n += 1; target.vlk1 = ps->vlk1; ps->nlk1 = n; } }
        if (!!(sides & 2)) { _s_long n = ps->nlk2; if (ps->lkside2.lock()) { n += 1; target.vlk2 = ps->vlk2; ps->nlk2 = n; } }
      }

        // unlock_normal removes any locks held by target, then clears target's locks states.
        //  Parent object reference (target.r) is not cleared.
        //  NOTE _shm_sems works as counted shared lock,
        //    so only the last unlock does release the global mutex.
      static void unlock_normal(shared_lock& target __bmdx_noarg) __bmdx_noex
      {
        _shm_sems* ps = target.r._pnonc_u();
        if (!(ps && (target.vlk1 == ps->vlk1 || target.vlk2 == ps->vlk2))) { target.vlk1 = -1; target.vlk2 = -1; return; }
        critsec_t<_shm_sems> __lock(10, -1, &ps->lkd); if (sizeof(__lock)) {}
        if (target.vlk1 == ps->vlk1) { _s_long n = ps->nlk1; if (n == 1) { ps->lkside1.unlock(); } if (n > 0) { ps->nlk1 = n - 1; } }
        if (target.vlk2 == ps->vlk2) { _s_long n = ps->nlk2; if (n == 1) { ps->lkside2.unlock(); } if (n > 0) { ps->nlk2 = n - 1; } }
        target.vlk1 = -1; target.vlk2 = -1;
      }

        // Unconditional unlocking, only for ~shmobj2s_t / _close(true).
      void unlock_forced(_s_long sides __bmdx_noarg) __bmdx_noex
      {
        if (!(sides & 3)) { return; }
        critsec_t<_shm_sems> __lock(10, -1, &lkd); if (sizeof(__lock)) {}
        if (!!(sides & 1)) { ++vlk1; nlk1 = 0; lkside1.unlock(); }
        if (!!(sides & 2)) { ++vlk2; nlk2 = 0; lkside2.unlock(); }
      }
    };


    T* _p;
    union { bool _b_side1; _s_long __pad1; };
    hints _hh;
    t_name_shm _name;
    _s_ll _ctor_nbt;
    cref_t<_shm_sems> _rsems;
    shared_lock _lkmyside;



    _s_long _prep(bool b_keeplk, _s_long sides_cr __bmdx_noarg)
    {
      bool b_may_cr = !!((_s_long(_b_side1) | 2 * _s_long(!_b_side1)) & sides_cr);
      shared_lock __slk(*this, (b_keeplk ? 4 : 0) | (b_may_cr ? sides_cr : 0));
      if ((b_keeplk || b_may_cr) && !__slk.b_lk_any()) { return _rsems ? -3 : -2; }
      if (_p) { if (b_keeplk) { _lkmyside.swap(__slk); } return 1; }
      _shmdesc* h2;
      const _s_ll nbt_open1 = _ctor_nbt;
      h2 = (_shmdesc*)_shm_open(b_may_cr, nbt_open1 + nbover());
      if (!h2) { return -2; }
      if (h2 == (void*)1) { return -4; } // memory handle transfer pending
      if (h2->handler_type == 0) // the memory area is accessible, but not initialized yet
      {
        if (!b_may_cr) { _shm_close(); return -4; }
        std::memset(&h2->b_constructed, 0, (char*)(h2 + 1) - (char*)&h2->b_constructed);
        h2->n_shmdesc = _s_long(sizeof(_shmdesc));
        h2->nbt = nbt_open1;
        h2->handler_type = _hh.htype;
      }
      else
      {
        if (!(h2->handler_type == _hh.htype && h2->n_shmdesc == _s_long(sizeof(_shmdesc)))) { _shm_close(); return -1; }
        // nbt_open1: h2 currently addresses nbt_open1 + nbover() bytes (assigned by system)
        // nbobji: h2->nbt is the number of bytes to which the contained object was initialized
        if (h2->nbt > nbt_open1) // actual object is larger than mapped memory window
        {
          const _s_ll nbt_open2 = h2->nbt;
          _shm_close();
          h2 = (_shmdesc*)_shm_open(b_may_cr, nbt_open2 + nbover());
          if (!(h2->handler_type == _hh.htype && h2->n_shmdesc == _s_long(sizeof(_shmdesc)))) { _shm_close(); return -1; }
          if (h2->nbt != nbt_open2) { _shm_close(); return -2; }
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


    static t_name_shm _shmname_hashed_1_ff(const t_name_shm& src, _s_long nmax __bmdx_noarg)
    {
      const _s_long nsrc = src.n();
      if (nsrc <= nmax)
      {
        t_name_shm n2(src);
        for (_s_long i = 0; i < nsrc; ++i) { if (n2[i] == 0) { n2[i] = 1; } }
        return n2;
      }
      t_name_shm n2(src.pd(), nmax); if (nmax <= 0) { return n2; }
      for (_s_long i = nmax; i < nsrc; ++i) { char& c = n2[i % nmax]; c += src[i]; c -= 55; }
      _s_ll h = 0;
      for (_s_long i = 0; i < nsrc; ++i) { h *= 19; h -= src[i]; } h ^= nsrc; h ^= 53970;
      for (_s_long i = 0; i < 8; ++i) { n2[i % nmax] ^= (unsigned char)h; h >>= 8; }
      for (_s_long i = 0; i < nmax; ++i) { if (n2[i] == 0) { n2[i] = src[i]; } if (n2[i] == 0) { n2[i] = 1; } }
      return n2;
    }
    static t_name_shm _shmname_limited_nb(const t_name_shm& src, _s_long nmax __bmdx_noarg)
    {
      _s_long nlim = bmdx_minmax::mylrange_lb(src.n(), 0, nmax);
      return t_name_shm(src.pd(), nlim);
    }
    #if __APPLE__ && __MACH__
      #define __bmdx_shm_name_max (29) // de-facto value of undeclared SHM_NAME_MAX constant
    #elif defined(_bmdxpl_Wnds)
      #define __bmdx_shm_name_max (MAX_PATH)
    #else
      #define __bmdx_shm_name_max (PATH_MAX)
    #endif

    #if 0
        // Clear platform-specific handles (not including this->p).
      void _shm_init0() __bmdx_noex;

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
      void* _shm_open(bool b_maycreate, _s_ll nbtotal) __bmdx_noex;

        // Close the shared memory area, if it's open. Clear platform-specific handles.
        // NOTE on ~shmobj2s_t, _shm_close is called automatically.
      void _shm_close() __bmdx_noex;
    #endif

    #ifdef _bmdxpl_Wnds
      HANDLE __h; void* __p;
      void _shm_init0(__bmdx_noarg1) __bmdx_noex { __h = 0; __p = 0; }
      void* _shm_open(bool b_maycreate, _s_ll nbtotal __bmdx_noarg) __bmdx_noex
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
            h2 = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, DWORD(nbtotal >> 32), DWORD(nbtotal & 0xffffffffll), _shmname_limited_nb(_name, __bmdx_shm_name_max ).c_str());
            b_reset = h2 && GetLastError() != ERROR_ALREADY_EXISTS;
          }
          else
          {
            h2 = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, _shmname_limited_nb(_name, __bmdx_shm_name_max ).c_str());
          }
          if (!h2) { return 0; }
        }
        _s_long* p2 = (_s_long*)MapViewOfFile(h2, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_T(nbtotal));
        if (!p2) { CloseHandle(h2); __h = 0; return 0; }
        if (b_reset) { p2[0] = 0; }
        __h = h2; __p = p2;
        return __p;
      }
      void _shm_close(__bmdx_noarg1) __bmdx_noex { if (__p) { UnmapViewOfFile(__p); __p = 0; } if (__h) { CloseHandle(__h); __h = 0; } }
    #endif
    #ifdef _bmdxpl_Psx
      #if 0
      #elif defined(__ANDROID__)
        int __fd; size_t __nb; void* __p; _fd_sharing __sh;
        int __shmfd_getnb(int fd) __bmdx_noex { int res = ioctl(fd, ASHMEM_GET_SIZE, 0); if (res < 0) { return -1; } return res; }
        bool __shmfd_resize(int fd, _s_ll nb) __bmdx_noex { int res = ioctl(fd, ASHMEM_SET_SIZE, size_t(nb)); return res >= 0; }
        int __shmfd_create(_s_ll nb) __bmdx_noex
        {
          int fda = ::open("/dev/ashmem", O_RDWR);
            if (fda < 0) { return -1; }
          if (!__shmfd_resize(fda, nb)) { ::close(fda); return -1; }
          if (ioctl(fda, ASHMEM_SET_PROT_MASK, (unsigned long)(PROT_READ | PROT_WRITE)) < 0) { ::close(fda); return -1; }
          return fda;
        }
        void __shmfd_close(int fd __bmdx_noarg) __bmdx_noex { if (fd != -1) { ::close(fd); } }
        void _shm_init0(__bmdx_noarg1) __bmdx_noex { __fd = -1; __p = 0; __nb = 0; }
        void* _shm_open(bool b_maycreate, _s_ll nbtotal __bmdx_noarg) __bmdx_noex
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
              _s_long res = __sh.fd_expose(_name, __fd, _b_side1 ? 1 : 2, false, true);
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
            if (!(__p && __p != MAP_FAILED)) { _shm_close(); return 0; }
          if (b_reset) { ((_s_long*)__p)[0] = 0; }
          __nb = nbtotal;
          return __p;
        }
        void _shm_close(__bmdx_noarg1) __bmdx_noex
        {
          if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
          if (__fd != -1) { __shmfd_close(__fd); __fd = -1; }
        }
      #else
        int __fd; int __perm; size_t __nb; void* __p;
        int __shmfile_open(int flags __bmdx_noarg) __bmdx_noex
        {
          #if 0
            return -1;
          #elif __APPLE__ && __MACH__
            return shm_open(_shmname_hashed_1_ff(_name, __bmdx_shm_name_max).c_str(), flags, __perm);
          #elif defined(__FreeBSD__)
            return shm_open(_shmname_limited_nb(_name, __bmdx_shm_name_max ).c_str(), flags, __perm);
          #elif defined(__SUNPRO_CC) || defined(__sun)
            return ::open(_shmname_limited_nb(_name, __bmdx_shm_name_max ).c_str(), flags, __perm);
          #else
            return ::open(_shmname_limited_nb(_name, __bmdx_shm_name_max ).c_str(), flags, __perm);
          #endif
        }
        void __shmfd_close(int fd __bmdx_noarg) __bmdx_noex { if (fd != -1) { ::close(fd); } }
        void _shm_init0(__bmdx_noarg1) __bmdx_noex { __fd = -1; __p = 0; __nb = 0; __perm = 0666; }
        void* _shm_open(bool b_maycreate, _s_ll nbtotal __bmdx_noarg) __bmdx_noex
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
          int f = MAP_SHARED;
          #ifdef MAP_POPULATE
            f |= MAP_POPULATE;
          #endif
          _s_long* p2 = (_s_long*)mmap(0, n2, PROT_READ|PROT_WRITE, f, fd2, 0);
          if (!(p2 && p2 != MAP_FAILED)) { __shmfd_close(fd2); __fd = -1; return 0; }
          if (b_reset) { p2[0] = 0; }
          __fd = fd2; __p = p2; __nb = n2;
          return __p;
        }
        void _shm_close(__bmdx_noarg1) __bmdx_noex
        {
          if (__p) { munmap(__p, __nb); __p = 0; __nb = 0; }
          if (__fd != -1) { __shmfd_close(__fd); __fd = -1; }
        }
      #endif
    #endif
  };

  template<class T> shmobj2s_t<T>::shared_lock::shared_lock(shmobj2s_t& parent, _s_long sides, _s_long timeout_ms __bmdx_noargt) __bmdx_noex
    : vlk1(-1), vlk2(-1)
  {
    _pdestr = _destr;
    double t0 = clock_ms();
    if (!parent._rsems) { parent._rsems.cm_create2(0, 0, 1, parent._name.c_str(), parent._hh.prefix_cs); }
    if (!parent._rsems) { return; }
    if (!!(sides & 4)) { sides |= parent.b_side1() ? 1 : 2; }
    r = parent._rsems;
    const double dt = timeout_ms;
    do { _shm_sems::lock_normal(*this, sides); if ((!(sides & 1) || b_lk1()) && (!(sides & 2) || b_lk2())) { return; } clear(); if (dt != 0) { sleep_mcs(100); } }
      while (dt < 0 || (dt != 0 && clock_ms() - t0 < dt));
  }



    // Interface for customized memory allocation and object dispatch control.
    // See allocctl_pre::create for example of implementation.
    //
  struct i_allocctl
  {
    typedef arrayref_t<char> t_stringref;

      // On client's behalf, allocates nb bytes of memory + maybe small overhead for internal info,
      //    in implementation-specific way.
      //    The allocated memory needs not to be zero-initialized.
      // On allocation failure, make_rba should return an empty cref_t.
      //
    virtual cref_t<t_stringref> make_rba(_s_ll nb) = 0;

      // Shows to i_allocctl the above allocated object, after it has been filled with data,
      //  but before returning it to the final client.
      //  (Normally, this occurs in the internal IPC message delivery thread.)
      //  The implementation may ignore (default), modify, replace, or clear the object.
      //  If the object is cleared, it's considered either
      //      a) removed,
      //      b) dispatched in other way than the calling routine does.
      //    The calling routine forgets that cref_t, and goes to processing the next message.
      //    E.g. for shmqueue_s this means that objects that are removed in this way
      //    by notify_filled, will not get (be pushed) into the local queue of input messages,
      //    and will not be available via mget().
      //
      //  NOTE There's no guarantee that all objects, made with make_rba, will be shown to notify_filled.
      //      The internal client may e.g. exit between the events, or set another allocator
      //      on application client's request, during the queue being disabled, etc.
      //    notify_filled only receives an object (rba) when all the following is true:
      //      1) rba is created with the same allocator instance, and it was not changed
      //        or re-set by the client from that time,
      //      2) after notify_filled, rba will be pushed into the local queue, servicing mget side.
      //        This may occur immediately or after some delay, if the queue is full and its capacity is limited.
      //
    virtual void notify_filled(cref_t<t_stringref>& rba) { (void)rba; }

    virtual ~i_allocctl() {}
  };

}




typedef arrayref_t<char> _t_stringref;
typedef char _t_rstr_ct; // make_rba's cref_t Aux object of context-dependent size; pointer to the object is _t_rstr_ct*, which is char*

  // Creates a zero-initialized byte array of length nb.
static cref_t<_t_stringref> make_rba_z(_s_ll nb, _s_ll nbadd) __bmdx_noex
{
  typedef cref_t<_t_stringref>::iref2<_t_stringref, _t_rstr_ct> t_iref2;
  if (nb < 0) { return cref_t<_t_stringref>(); }
  cref_t<_t_stringref> rmsg = t_iref2::create0(1, 0, 0x14 | iref2_flags::use_aux, t_iref2::handler_dflt, iref2_args_t<_t_rstr_ct>::args(), bmdx_minmax::myllmax(sizeof(_t_rstr_ct), nb + bmdx_minmax::myllmax(nbadd, 0)));
  _t_rstr_ct* ps =  (_t_rstr_ct*)rmsg.paux();
  if (!ps) { rmsg.clear(); }
    else { rmsg->_link_u(ps, nb); }
  return rmsg;
}
  // Creates a byte array from msg, by value (self-contained), or by reference (msg->pd() must be valid until all referring cref_t's are destroyed).
  //  nbadd > 0 (used only on b_byval == true): adds hidden (i.e. not accounted in t_stringref::n()) zero bytes after the copy of msg data. E.g. nbadd == 1 makes conventional C string.
static cref_t<_t_stringref> make_rba(_t_stringref msg, bool b_byval, _s_ll nbadd = 0) __bmdx_noex
{
  typedef cref_t<_t_stringref>::iref2<_t_stringref, _t_rstr_ct> t_iref2;
  if (!msg.is_valid()) { return cref_t<_t_stringref>(); }
  cref_t<_t_stringref> rmsg = t_iref2::create0(1, 0, 0x14 | iref2_flags::use_aux, t_iref2::handler_dflt, iref2_args_t<_t_rstr_ct>::args(), bmdx_minmax::myllmax(sizeof(_t_rstr_ct), b_byval ? msg.n() + bmdx_minmax::myllmax(nbadd, 0) : 0));
  _t_rstr_ct* ps =  (_t_rstr_ct*)rmsg.paux();
  if (!ps) { rmsg.clear(); }
    else if (b_byval) { if (msg.n() > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(ps, msg.pd(), msg.n()); } rmsg->_link_u(ps, msg.n()); }
    else { rmsg->_link_u(msg.pd(), msg.n()); }
  return rmsg;
}
  // Creates a byte array as concatenated copies of the given parts.
  //  nbadd > 0: adds hidden (i.e. not accounted in t_stringref::n()) zero bytes after the copy of parts data. E.g. nbadd == 1 makes conventional C string.
static cref_t<_t_stringref> make_rba_mp(_t_stringref part1, _t_stringref part2, _t_stringref part3 = _t_stringref(), _s_ll nbadd = 0) __bmdx_noex
{
  typedef cref_t<_t_stringref>::iref2<_t_stringref, _t_rstr_ct> t_iref2;
  if (!(part1.is_valid() && part2.is_valid() && part3.is_valid())) { return cref_t<_t_stringref>(); }
  const _s_ll nbvisible = part1.n() + part2.n() + part3.n();
  cref_t<_t_stringref> rmsg = t_iref2::create0(1, 0, 0x14 | iref2_flags::use_aux, t_iref2::handler_dflt, iref2_args_t<_t_rstr_ct>::args(), bmdx_minmax::myllmax(sizeof(_t_rstr_ct), nbvisible + bmdx_minmax::myllmax(nbadd, 0)));
  _t_rstr_ct* ps =  (_t_rstr_ct*)rmsg.paux();
  if (!ps) { rmsg.clear(); }
    else { rmsg->_link_u(ps, nbvisible); if (part1.n() > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(ps, part1.pd(), part1.n()); ps += part1.n(); } if (part2.n() > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(ps, part2.pd(), part2.n()); ps += part2.n(); } if (part3.n() > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(ps, part3.pd(), part3.n()); ps += part3.n(); } }
  return rmsg;
}

  // NOTE This works if
  //    1) T may be copied and assigned concurrently,
  //    2) T may be moved as plain bytes,
  //    3) this->x_copy is initialized by the getter as necessary.
  //  Roles: setter/updater (s_) and getter (g_) are supposed to be in different threads.
template <class T> struct volatile_cvar_t
{
  volatile T x;
  T x_copy;
  volatile_cvar_t() { s_ver1 = s_ver2 = 0; g_ver_pre = g_ver = 0; }

    // Updates x.
  void s_begin_update(const T& x2, _s_ll& ver_upd) { const _s_ll v = s_ver2 + 0x100; s_ver1 = v; if (&x2 != &x) { (T&)x = x2; } s_ver2 = v; ver_upd = v; }
    // For waiting until g_update_complete() on the other side.
    // 1 - done, 0 - not done yet, -1 (only if pver_upd != 0) - already overwritten by someone else.
  _s_long s_is_done(_s_ll* pver_upd, unsigned char& retcode) const
  {
    _s_ll vg = g_ver; _s_ll vs = s_ver2;
    retcode = (unsigned char)(vg & 0xff);
    if ((vg >> 8) == (vs >> 8))
    {
      if (pver_upd && vs != *pver_upd) { return -1; }
      return 1;
    }
    return 0;
  }

    // Updates x_copy from x.
  void g_update_pre()
  {
    if (s_ver2 == g_ver_pre) { return; }
    double t0 = 0;
    _s_ll cnt = 0;
    while (1)
    {
      _s_ll v1 = s_ver1; T x2((const T&)x); _s_ll v2 = s_ver2;
      if (v2 == v1) { bmdx_str::words::swap_bytes(x_copy, x2); g_ver_pre = v2; return; }
      ++cnt;
      if (cnt == 1000) { t0 = clock_ms(); }
      if (cnt > 1000 && clock_ms() - t0 > 200) { return; }
    }
  }
    // If x_copy has been changed as the result of g_update_pre(),
    //  g_is_new_pre() will return true, until g_update_complete() is called.
  bool g_is_new_pre() const { return (g_ver_pre >> 8) != (g_ver >> 8); }
  void g_update_complete(unsigned char retcode) { g_ver = (g_ver_pre & ~_s_ll(0xff)) | _s_ll(retcode); }

private:
  volatile _s_ll s_ver1, s_ver2;
public:
  _s_ll g_ver_pre;
  volatile _s_ll g_ver;
};



#define __bmdx_shmfifo_nbytes_min (rfifo_nbl11::n0) // minimal buffer space (8 bytes)
#define __bmdx_shmfifo_msg_nbytes_max (((1ll << (5 * 8 - 1)) - 1)) // must be ( (1 << (_shmqueue_ctxx_impl::nb_mlen * 8 - 1)) - 1 )

  // nbytes_dflt: dflt. shared queue size in bytes. (It may be specified by client individually for each queue, see shmqueue_s().)
  // quot: data is pushed in parts (1/quot) of the buffer capacity, to allow earlier (and shorter in time) popping
  // idle_t_short: short sleep if no data now for I/O.
  //    NOTE Mem. copy speed is of order 3000 B/mcs; idle_t_short should be <=> __bmdx_shmfifo_nbytes_dflt/__bmdx_shmfifo_push1_quot.
#ifdef _bmdxpl_Wnds
  #define __bmdx_shmfifo_nbytes_dflt (150 * 1024ll)
  #define __bmdx_shmfifo_push1_quot 5
  #define __bmdx_shmfifo_idle_t_short_mcs 8
#endif
#ifdef _bmdxpl_Psx
  #if __APPLE__ && __MACH__
    #define __bmdx_shmfifo_nbytes_dflt (350 * 1024ll)
    #define __bmdx_shmfifo_push1_quot 5
  #else
    #define __bmdx_shmfifo_nbytes_dflt (150 * 1024ll)
    #define __bmdx_shmfifo_push1_quot 4
  #endif
  #if defined(__sun)
    #define __bmdx_shmfifo_idle_t_short_mcs 0
  #else
    #define __bmdx_shmfifo_idle_t_short_mcs 8
  #endif
#endif

#define __bmdx_shmfifo_idle_t_long_mcs 10000
#define __bmdx_shmfifo_idle_t_enable_time_ms 2100
#define __bmdx_shmfifo_push1_nbmin 3000
#define __bmdx_shmfifo_rcv_deact_timeout_ms 1500
#define __bmdx_shmfifo_rcv_recover_timeout_ms 400
#define __bmdx_shmfifo_pop_nbmin_mlock 2048

struct shmqueue_ctx
{
  typedef arrayref_t<char> t_stringref;

    // shmqueue_ctx_rso version.
  #define __bmdx_shmfifo_ver (-1615777792) // In the current version, shmobj2s_t type index == 1 + yk_c::bytes::cmti_base_t<bmdx_shm::shmqueue_s, 2019, 12, 29, 20, 1>::ind()
  struct shmqueue_ctx_rso;

  shmqueue_ctx(const t_name_shm& name, bool b_receiver, _s_ll nbhint)
  :
    b_rcv_mlk(0), pmsend1(0), pmsend2(0), nmsend(0), b_just_started(true), b_enrq(true),
    bufstate(0), _mprg_ver(0), _mprg_ver_proc(0), _mprg_iend_done(0), iver_al(0),

    buf(name.c_str(), b_receiver, bmdx_minmax::myllmax(nbhint, __bmdx_shmfifo_nbytes_min, rfifo_nbl11::n0) + sizeof(shmqueue_ctx_rso) - rfifo_nbl11::n0, __bmdx_shmfifo_ver),
    sndr_rcv_act(0), b_sndr_waitrcvinit(false), sndr_t0_rcvst(clock_ms()), sndr_ircvst(0)

  { _mprg_cmd.tms = bmdx::clock_ms(); }

  ~shmqueue_ctx() { _mrcv_clear(); }


  // A. Queue client-related part.

    critsec_t<shmqueue_ctx>::csdata csd;
    vnnqueue_t<cref_t<t_stringref> > msgs;
    cref_t<t_stringref> msg_rcv;
    bool b_rcv_mlk;
    t_stringref* pmsend1; // prefix to be sent
    t_stringref* pmsend2; // main message to be sent
    _s_ll nmsend;
    bool b_just_started; // initially true, used to check if the process was restarted, while buf was already existing in some state
    volatile bool b_enrq; // client's requirement to enable or disable the queue (by dflt. b_enrq == true)
      //
      // Shared queue state copy for client non-blocking checks.
      //  bufstate & 0xff:
      //    0 - not initialized yet,
      //    1 - during initialization,
      //    2 - the last initialization attempt has failed ( (bufstate & 0xffffff00)>>8 contains negative error code ),
      //    3 - initialized successfully,
      //    4 - disabled.
    volatile _s_long bufstate;
      //
      // Variables to control client request on discarding messages before they are sent.
    volatile _s_ll _mprg_ver;
    _s_ll _mprg_ver_proc; // the last processed _mprg_ver
    struct mprg_info { _s_ll iend; double tms; mprg_info() { iend = 0; tms = 0; } };
    volatile mprg_info mprg_client; // if iend is set to >= 0, and the queue is sender, when clock reaches tms, purges all messages in range [msgs.ipop() .. ismprg_end); the currently unsent message is also terminated
    mprg_info _mprg_cmd; // (required iend of sender's lq cleanup, time moment when it should be done)
    _s_ll _mprg_iend_done;
    volatile_cvar_t<cref_t<i_allocctl> > v_al;
      _s_ll iver_al; // iver_al == 0 serves as "null"
    struct lqconf_cap
    {
      _s_ll ncapmin, ncapmax, nrsv; lqconf_cap() { ncapmin = -3; ncapmax = -3; nrsv = -3; }
      bool is_valid_cap() const { return ncapmin >= -1 && ncapmax >= -2; }
      bool is_valid_rsv() const { return nrsv >= 0; }
      bool operator==(const lqconf_cap& x) { return x.ncapmin == ncapmin && x.ncapmax == ncapmax && x.nrsv == nrsv; }
      void cp_nondflt(const lqconf_cap& c) { if (c.is_valid_cap()) { ncapmin = c.ncapmin; ncapmax = c.ncapmax; } if (c.is_valid_rsv()) { nrsv = c.nrsv; } }
    };
    volatile_cvar_t<lqconf_cap> v_inq_cap_back; // a copy of the last non-default v_inq_cap.x fields, maybe set by different calls
    volatile_cvar_t<lqconf_cap> v_inq_cap;



      // Size of the currently sent parts of the message.
    _s_ll msend1_n() { if (!pmsend1) { return 0; } return bmdx_minmax::myllmin(pmsend1->n(), __bmdx_shmfifo_msg_nbytes_max); }
    _s_ll msend2_n() { if (!pmsend2) { return 0; } return bmdx_minmax::myllmin(pmsend2->n(), __bmdx_shmfifo_msg_nbytes_max - msend1_n()); }
    void _msend_clear() __bmdx_noex { if (pmsend1) { msgs.pop_n(3); } else if (pmsend2) { msgs.pop_1(); } pmsend1 = pmsend2 = 0; nmsend = 0; }
      //
    bool _lqsend_mprg_pending() const __bmdx_noex { return _mprg_ver != _mprg_ver_proc || _mprg_cmd.iend < _mprg_iend_done; }
      //
      // From sender queue, removes all message message refs. and pointers between [msgs.ipop() .. iend).
      //  If anything has been removed, returns true.
    bool _lqsend_mprg_update() __bmdx_noex
    {
      if (buf.b_side1()) { return false; } // not a sender
      if (_mprg_ver != _mprg_ver_proc)
      {
        mprg_info cmd; while (1) { _s_ll ver1 = _mprg_ver; cmd.iend = mprg_client.iend; cmd.tms = mprg_client.tms; _s_ll ver2 = _mprg_ver; if (ver1 == ver2) { _mprg_ver_proc = ver1; break; } }
        _mprg_cmd = cmd;
      }
      if ((_mprg_cmd.iend - _mprg_iend_done) > 0 && bmdx::clock_ms() >= _mprg_cmd.tms)
      {
        const _s_ll ipop0 = msgs.ipop();
        if (ipop0 < _mprg_cmd.iend)
        {
          _msend_clear();
          while (msgs.ipop() < _mprg_cmd.iend && msgs.navl() > 0)
          {
            if (msgs.front()) { msgs.pop_1(); }
              else { msgs.pop_n(3); }
          }
        }
        _mprg_iend_done = msgs.ipop();
        return _mprg_iend_done != ipop0;
      }
      return false;
    }


    #ifdef _bmdxpl_Wnds
        // NOTE Here, locking is not implemented due to inefficiency.
      void _mrcv_lock() __bmdx_noex {}
      void _mrcv_unlock() __bmdx_noex {}
    #endif
    #ifdef _bmdxpl_Psx
      void _mrcv_lock() __bmdx_noex { b_rcv_mlk = false; if (msg_rcv && !!msg_rcv->pd() && msg_rcv->n() >= __bmdx_shmfifo_pop_nbmin_mlock) { b_rcv_mlk = 0 == mlock(msg_rcv->pd(), (size_t)msg_rcv->n()); } }
      void _mrcv_unlock() __bmdx_noex { if (b_rcv_mlk && msg_rcv && !!msg_rcv->pd() && msg_rcv->n() >= __bmdx_shmfifo_pop_nbmin_mlock) { munlock(msg_rcv->pd(), (size_t)msg_rcv->n()); } b_rcv_mlk = false; }
    #endif
      //
    void _mrcv_clear() __bmdx_noex { if (!msg_rcv) { return; } _mrcv_unlock(); msg_rcv.clear(); }



  // B. Shared queue implementation-related part.

    struct shmqueue_ctx_rso
    {
      volatile _s_ll ipush_plan; // planned end pos. of the currently pushed message (used for recovery if the sending process is restarted)
      volatile _s_ll ipop_plan; // planned end pos. of the currently popped message (used for recovery if the receiving process is restarted)
      volatile _s_ll __rsv[6]; // __rsv[0] is live counter, incremented at each receiver iteration

        // NOTE Let total shared memory size == nb().
        //  Then, data buffer size == nb() - sizeof(_shmdesc) - (sizeof(shmqueue_ctx_rso) - rfifo_nbl11::n0)
      rfifo_nbl11 ringbuf;

    private: shmqueue_ctx_rso(); shmqueue_ctx_rso(const shmqueue_ctx_rso&); void operator=(const shmqueue_ctx_rso&); // this object may be accessed only by pointer or reference
    };

    shmobj2s_t<shmqueue_ctx_rso> buf;
    volatile _s_long sndr_rcv_act; // sender side flag: 0: no receiver activity detected yet, 1 or 2: receiver activity detected at least once, -1: detected that receiver is not active after being active
    bool b_sndr_waitrcvinit; // true: set together with tr_end = -1: the sender waits for the receiver to a) initialize the shared buffer, b) reconnect to the existing buffer
    double sndr_t0_rcvst; _s_ll sndr_ircvst;

      // NOTE Must be protected by critsec_t<shmqueue_ctx> with this->csd.
      // Returns:
      //    1 - success.
      //    0 - incomplete initialization or bad state:
      //      a) (sender only) could not initialize the buffer or fix errors, because the current side is sender (b_side1() == false).
      //          The sender should wait until the receiver makes necessary updates from its side.
      //      b) (receiver only) - the buffer exists and is initialized, but is in specific bad state.
      //          The receiver must wait for *pf_state2() == -1 (i.e. until the bad state is recognized by the sender),
      //      and then reinitialize the buffer.
      //    Codes for errors, returned by shmobj2s_t::prepare():
      //      -1 - the shared memory exists, but its structure is not compatible (see htype arg. in shmobj2s_t()).
      //      -2 - failure during operation (it's generally unknown if the shared object is available).
      //      -3 - lock of side 1 (for b_side1()==true) or side 2 (for b_side1()==false) is set by someone else.
      //      -4 - the memory exists, but not accessible yet: a) pending initialization by other side, b) pending memory handle transfer from other side.
    int _reset() __bmdx_noex
    { try {
      _s_long res = buf.prepare(true);
      if (res < 0) { return res; }
      if (res == 0) { return -2; } // not expected to occur

      const _s_long fc = buf.f_constructed(); // here: fc is 0..2, buf.p() is valid
      if (!buf.b_side1()) // sender
      {
        if (fc == 1)
        {
          const char f1 = *buf.pf_state1();
          const char f2 = *buf.pf_state2();
          if (f1 == -1 && f2 == -1) { return 0; }
          else if (f1 == -1) { *buf.pf_state2() = -1; return 0; }
          else if (f2 == -1) { return 0; }
          else
          {
            if (f1 >= 0) { this->sndr_rcv_act = 2; }
            return 1;
          }
        }
        return 0;
      }
      else // receiver
      {
        if (fc == 1)
        {
          const char f1 = *buf.pf_state1();
          const char f2 = *buf.pf_state2();
          if (f2 == -1) {} // fall-through
            else if (f1 == -1) { return 0; }
            else { return 1; }
        }
        *buf.pf_state1() = -1;
        buf.set_f_constructed(0);
        const _s_ll nbuf = buf.nb() - _s_ll(sizeof(shmqueue_ctx_rso) - rfifo_nbl11::n0);
          if (nbuf < 0) { return -1; }
        shmqueue_ctx_rso* p = buf.p();
          if (!p) { return -2; }
        p->ringbuf.init_ref(nbuf);
        std::memset((void*)&p->__rsv[0], 0, sizeof(p->__rsv));
        p->ipop_plan = 0;
        p->ipush_plan = 0;
        *buf.pf_state1() = 0;
        *buf.pf_state2() = 0;
        buf.set_f_constructed(1);
        return 1;
      }
    } catch (...) {} return -2; }


private:
  shmqueue_ctx(const shmqueue_ctx&); void operator=(const shmqueue_ctx&); // this object cannot be copied
};

struct i_shmqueue_ctxx
{
    // autocreate_mode:
    //    0 - return the sender-side queue object only if it already exists (do not create automatically),
    //    1 - return the receiver-side queue object only if it already exists (do not create automatically),
    //    2 - if the sender-side queue object does not exist, autocreate it,
    //    3 - if the receiver-side queue object does not exist, autocreate it.
    // nbhint:
    //    >= 0 specifies the number of bytes in IPC buffer for the queue. Automatically adjusted to be not less than __bmdx_shmfifo_nbytes_min.
    //    < 0 makes queue with buffer size == __bmdx_shmfifo_nbytes_dflt.
  virtual cref_t<shmqueue_ctx> rqueue(const t_name_shm& name, _s_long autocreate_mode, _s_ll nbhint) = 0;
  virtual ~i_shmqueue_ctxx() {}
};

struct _shmqueue_ctxx_impl : i_shmqueue_ctxx
{
  typedef arrayref_t<char> t_stringref;
  typedef char t_rstr_ct; // make_rba's cref_t Aux object of context-dependent size; pointer to the object is t_rstr_ct*, which is char*
  // typedef carray_t<char> t_rstr_ct;
  typedef _shmqueue_ctxx_impl t_lks_qq;

  static const _s_ll nb_mlen = 5;

  struct qmap_key
  {
    _s_long type; // 0 - sender, 1 - receiver
    t_name_shm name;
    qmap_key(_s_long type_, t_name_shm name_) : type(type_), name(name_) {}
    struct less { bool operator() (const qmap_key& k1, const qmap_key& k2) const { return k1.type < k2.type || (k1.type == k2.type && k1.name < k2.name); } };
  };
  std::map<qmap_key, cref_t<shmqueue_ctx>, qmap_key::less> m;
  volatile _s_ll iver_m;


  _shmqueue_ctxx_impl() { iver_m = 0; }
  ~_shmqueue_ctxx_impl() {}

  virtual cref_t<shmqueue_ctx> rqueue(const t_name_shm& name, _s_long autocreate_mode, _s_ll nbhint)
  {
    cref_t<shmqueue_ctx> rv;
    if (autocreate_mode >= 0 && autocreate_mode <= 3)
    {
      critsec_t<t_lks_qq> __lock(10, -1); if (sizeof(__lock)) {}
      size_t n1 = m.size();
      qmap_key k(autocreate_mode & 1, name);
      try { if (autocreate_mode >= 2 || m.count(k) > 0) {
        cref_t<shmqueue_ctx>& rv2 = m[k];
        if (!rv2)  { rv2.cm_create3(0, 0, 0, name, !!(autocreate_mode & 1), nbhint >= 0 ? nbhint : __bmdx_shmfifo_nbytes_dflt); }
        rv = rv2;
      } } catch (...) {}
      if (m.size() != n1) { iver_m += 1; }
    }
    if (rv)
    {
      if (autocreate_mode >= 2 && (rv->bufstate & 0xff) < 3)
      {
        shmqueue_ctx& q = rv._rnonc();
        critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
        const _s_long bsm = (q.bufstate & 0xff);
        if (bsm < 3 || (bsm != 4 && q.buf.f_constructed() != 1))
        {
          _s_long res = q._reset();
          if (res >= 1) { q.bufstate = 3; }
            else if (res == 0) { q.bufstate = 1; }
            else { q.bufstate = (res << 8) | 2; }
        }
      }
    }
    return rv;
  }

  struct th_handler { threadctl tc; ~th_handler() { if (tc) { tc.signal_stop(); while (tc) { sleep_mcs(5000); } } } };
  struct _ipc_delivery_thread : threadctl::ctx_base
  {
    typedef std::map<qmap_key, cref_t<shmqueue_ctx>, qmap_key::less> t_ctxmap;

    inline static bool _part_update_ctxmap(t_ctxmap& m2, _s_ll& iver_m2) __bmdx_noex
    {
      cref_t<i_shmqueue_ctxx> mqq1;
      _shmqueue_ctxx_impl* pqq1 = 0;
      if (!mqq1) { mqq1 = mqq(); }
      if (!pqq1) { pqq1 = static_cast<_shmqueue_ctxx_impl*>(mqq1._pnonc_u()); }
      if (!pqq1) { return false; }
      bool b_succ = 1;
      if (iver_m2 < pqq1->iver_m)
      {
        critsec_t<t_lks_qq> __lock(10, -1); if (sizeof(__lock)) {}
        try { m2 = pqq1->m; iver_m2 = pqq1->iver_m; } catch (...) { b_succ = 0; }
      }
      if (!b_succ) { return false; }
      return true;
    }

    inline static bool _part_validate_ctx1(shmqueue_ctx& q, bool& b_changed) __bmdx_noex
    {
      // 0. Pre-update volatile settings.

      q.v_al.g_update_pre();
      q.v_inq_cap.g_update_pre();

      // 1. Process: enrq flag.

      if (q.bufstate == 4) // the queue is currently disabled
      {
        if (!q.b_enrq) // continue to be disabled
        {
          if (!q.buf.b_side1() && q._lqsend_mprg_pending())
            { const bool b_purged = q._lqsend_mprg_update(); b_changed =  b_changed || b_purged; }
          return false;
        }
        critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
        q.bufstate = 0; q.b_just_started = true; // enable the queue as if initialized first time
        b_changed = true;
      }
      else if (!q.b_enrq) // the queue is currently enabled, try to disable as requested
      {
        bool b_may_disable = false;
        critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
        const _s_long bsm = (q.bufstate & 0xff);
        if (q.b_sndr_waitrcvinit) { b_may_disable = true; }
        else if (bsm == 3 && q.buf.f_constructed() == 1)
        {
          if (q.buf.b_side1())
          {
            signed char& tr_rcv = *q.buf.pf_state1();
            b_may_disable = tr_rcv == 0 || tr_rcv == 1 || tr_rcv == -1;
          }
          else
          {
            signed char& tr_send = *q.buf.pf_state2();
            b_may_disable = tr_send == 0 || tr_send == 1 || tr_send == -1;
          }
        }
        else { b_may_disable = true; }
        if (b_may_disable)
        {
          q.buf.close();
          q._msend_clear(); q.b_sndr_waitrcvinit = false;
          q._mrcv_clear();
          q.bufstate = 4;
          b_changed = true;
          return false;
        }
      }

      // 2. Process: buffer construction, queue purge rq., working state setting.

      bool b_q_constructed = true; // true if queue is constructed (i.e. in normal working state)
      do { // once
        if (q.b_sndr_waitrcvinit)
        {
          critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
          if (q.buf.f_constructed() != 1) { q.bufstate = 1; b_q_constructed = false; break; }
          const signed char _tr_rcv = *q.buf.pf_state1();
          signed char& tr_send = *q.buf.pf_state2();
          if (_tr_rcv == -1) { q.bufstate = 1; tr_send = -1; b_q_constructed = false; break; }
          q.bufstate = 3;
        }
        else if ((q.bufstate & 0xff) < 3)
        {
          critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
          const _s_long bsm = (q.bufstate & 0xff);
          if (bsm < 3 || (bsm != 4 && q.buf.f_constructed() != 1))
          {
            _s_long res = q._reset(); // NOTE this may return only 0 or 1, if the current side is sender
            if (res >= 1) { q.bufstate = 3; }
              else if (res == 0) { q.bufstate = 1; }
              else { q.bufstate = (res << 8) | 2; }
            if (res < 1) { b_q_constructed = false; break; }
            b_changed = true;
          }
        }
        if (!q.buf.p()) { b_q_constructed = false; break; } // should not occur
      } while (false);

      // here: if b_q_constructed == true: q.bufstate == 3 && q.buf.f_constructed() == 1 && q.buf.p() != 0

      if (!q.buf.b_side1() && q._lqsend_mprg_pending())
      {
        const bool b_purged = q._lqsend_mprg_update(); b_changed =  b_changed || b_purged;
        const bool b_msg_pending = !!q.pmsend2;
        if (b_q_constructed && b_purged && b_msg_pending) { signed char& tr_send = *q.buf.pf_state2(); tr_send = -2; } // the message that's currently being sent, will be canceled
      }
      if (!b_q_constructed) { return false; }

      // 3. Receiver live state update / check.

      if (1)
      {
        volatile _s_ll& ircvst = q.buf->__rsv[0];
        _s_ll x = ircvst;
        const double t = clock_ms();
        if (q.buf.b_side1())
        {
          x += 1;
          ircvst = x;
        }
        else
        {
          const signed char c_rcv = *q.buf.pf_state1();
          if (x != q.sndr_ircvst)
          {
            if (q.sndr_ircvst != 0)
            {
              if (q.sndr_rcv_act == 0 && c_rcv == -2) { q.sndr_rcv_act = 1; } // receiver has just started, it's expected to quickly recover to normal state
                else { q.sndr_rcv_act =  2; }
            }
            q.sndr_t0_rcvst = t;
            q.sndr_ircvst = x;
          }
          if (q.sndr_rcv_act == 2) { if (c_rcv == -2 || t - q.sndr_t0_rcvst >= __bmdx_shmfifo_rcv_deact_timeout_ms) { q.sndr_rcv_act = -1; } }
            else if (q.sndr_rcv_act == 1) { if (t - q.sndr_t0_rcvst >= __bmdx_shmfifo_rcv_recover_timeout_ms) { q.sndr_rcv_act = -1; } }
        }
      }

      return true;
    }

    inline static void _part_update_conf_rcv(shmqueue_ctx& q, bool& b_changed, bool b_validated) __bmdx_noex
    {
      (void)b_validated;
      if (!q.buf.b_side1()) { return; }
      const bool b_al = q.v_al.g_is_new_pre();
      const bool b_cap = q.v_inq_cap.g_is_new_pre();
      if (!(b_al || b_cap)) { return; }

      if (b_al) { b_changed = true; q.v_al.g_update_complete(2); }
      if (b_cap)
      {
        unsigned char retcode = 0;
        _x_update_lqcap(q, retcode);
        if (retcode == 2) { b_changed = true; _x_update_conf_lqcap_back(q); }
        q.v_inq_cap.g_update_complete(retcode);
      }
    }

    inline static void _part_update_conf_send(shmqueue_ctx& q, bool& b_changed, bool b_validated) __bmdx_noex
    {
      (void)b_validated;
      if (q.buf.b_side1()) { return; }
      const bool b_cap = q.v_inq_cap.g_is_new_pre();
      if (!b_cap) { return; }
      if (b_cap)
      {
        unsigned char retcode = 0;
        _x_update_lqcap(q, retcode);
        if (retcode == 2) { b_changed = true; _x_update_conf_lqcap_back(q); }
        q.v_inq_cap.g_update_complete(retcode);
      }
    }

    inline static void _x_update_lqcap(shmqueue_ctx& q, unsigned char& retcode) __bmdx_noex
    {
      retcode = 0;
      // The client-side lock is necessary to avoid clash with mget(), bufstate() etc.
      critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
      _s_ll nmin_prv = q.msgs.ncapmin(), nmax_prv = q.msgs.ncapmax();
      bool b_cap_changed = false, b_rsv_changed = false;
      if (q.v_inq_cap.x_copy.is_valid_cap())
      {
        b_cap_changed = 1 == q.msgs.set_cap_hints(q.v_inq_cap.x_copy.ncapmin, q.v_inq_cap.x_copy.ncapmax);
        if (!b_cap_changed) { retcode = 1; }
      }
      if (retcode != 1 && q.v_inq_cap.x_copy.is_valid_rsv())
      {
        b_rsv_changed = 1 == q.msgs.set_rsv(q.v_inq_cap.x_copy.nrsv);
        if (!b_rsv_changed) { retcode = 1; }
      }
      if (retcode == 1) { if (b_cap_changed) { q.msgs.set_cap_hints(nmin_prv, nmax_prv); } }
        else if (b_cap_changed || b_rsv_changed) { retcode = 2; }
    }
    inline static void _x_update_conf_lqcap_back(shmqueue_ctx& q) __bmdx_noex
    {
      typedef shmqueue_ctx::lqconf_cap lqconf_cap;
      lqconf_cap c = (lqconf_cap&)q.v_inq_cap_back.x;
      c.cp_nondflt(q.v_inq_cap.x_copy);
      _s_ll _ver = 0;
      q.v_inq_cap_back.s_begin_update(c, _ver);
    }

    inline static void _part_do_rcv(shmqueue_ctx& q, bool b_exit, bool& b_changed)
    {
        // Recevier.
        // tr_rcv
        //    0 - initialize, goto 1
        //    1 - wait until message length (fields of nb_mlen bytes) appears in the buffer, peek into it, prep. string for input, goto tr_rcv = 2
        //    2 - discard msg. length, read msg. bytes, read end byte, [if end succeeded: push string into the queue], clear string, goto tr_rcv = 1
        //    -1 - data buffer is broken and currently cannot be used; should wait for the sender setting tr_send = -1, and then re-initialize the buffer
        //    -2 - data sending sequence is broken (probably receiver process is restarted); try to recover:
        //      pop the current message bytes until message end, ignore this data; then goto tr_rcv = 1 (normal state)

      shmqueue_ctx::shmqueue_ctx_rso& shm = *q.buf.p();
      rfifo_nbl11& buf = shm.ringbuf;
      signed char& tr_rcv = *q.buf.pf_state1();
      const signed char _tr_send = *q.buf.pf_state2();

      signed char c_rcv = tr_rcv;
      if (_tr_send == -1) { tr_rcv = c_rcv = -1; }
      if (q.b_just_started)
      {
        q.b_just_started = false;
        switch (c_rcv)
        {
          case -2: break;
          case -1: break;
          case 0: break;
          case 1: tr_rcv = c_rcv = 1; b_changed = true; break;
          case 2: tr_rcv = c_rcv = -2; b_changed = true; break;
          default: tr_rcv = c_rcv = -1; b_changed = true; break;
        }
      }
      if (c_rcv == -1) // wait for sender setting its tr_send = -1, then re-initialize the buffer
      {
        q._mrcv_clear();
        if (_tr_send != -1) { return; }
        critsec_t<shmqueue_ctx> __lock(10, -1, &q.csd); if (sizeof(__lock)) {}
        q.buf.set_f_constructed(0);
        q.bufstate = 1;
        b_changed = true;
        return;
      }


      if (c_rcv == 0 || c_rcv == 1)
      {
        if (b_exit) { q._mrcv_clear(); tr_rcv = -2; return; }
        if (c_rcv == 0) { tr_rcv = 1; b_changed = true; }

        if (buf.navl() < nb_mlen + 1) { return; } // minimal total message length is (nb_mlen (length field) + 0 (empty message) + 1 (end byte))
        char z[nb_mlen];
        buf.pop(z, nb_mlen, false); // peek into msg. length
        const bool b_extmsg = !!((unsigned char)z[0] & 0x80);
        const _s_ll ndatr = ((_s_ll(z[0]) & 127) << 32) + be4(z, 1); // NOTE client data length bit 39 is ignored (it serves as b_extmsg flag)
        const _s_ll iend = buf.ipop() + nb_mlen + ndatr + 1;
        shm.ipop_plan = iend;
        b_changed = true;

        if (b_extmsg)
        {
          if (ndatr != 0) { tr_rcv = -2; return; } // read and discard unsupported messages
          buf._rcv_set_ipop(iend); // skip the empty (non-informational) ext. message
          return;
        }
        else if (ndatr > __bmdx_shmfifo_msg_nbytes_max) { tr_rcv = -2; return; } // read and discard messages that are too long

        cref_t<i_allocctl>& al = q.v_al.x_copy;
        if (al)
        {
          q.iver_al = q.v_al.g_ver;
          q._mrcv_clear();
          try { q.msg_rcv = al->make_rba(ndatr); } catch (...) {}  // creates a string for input message, using custom allocator
            if (!q.msg_rcv) { return; } // will retry herein on the next iteration
        }
        else
        {
          q.iver_al = 0;
          q.msg_rcv = make_rba_z(ndatr, 1); // creates a string for input message, adds a zero byte to make the returned array more like to C string
            if (!q.msg_rcv) { return; } // will retry herein on the next iteration
          q._mrcv_lock();
        }
        tr_rcv = 2;
        return;
      }
      else if (c_rcv == 2)
      {
        if (b_exit) { q._mrcv_clear(); tr_rcv = -2; return; }
        if (!q.msg_rcv)  { tr_rcv = -1; b_changed = true; return; } // invariant check

        const _s_ll ndatr = q.msg_rcv->n();
        const _s_ll i1 = shm.ipop_plan - 1 - ndatr;
        const _s_ll i0 = i1 - nb_mlen;

        if (buf.ipop() < i0 || (buf.ipop() < i1 && buf.ipop() > i0) || buf.ipop() > shm.ipop_plan) // invariant check
          { tr_rcv = -1; b_changed = true; return; }

        if (buf.ipop() < i1) // pop length
          { if (buf.navl() < nb_mlen) { return; } buf.discard(nb_mlen); b_changed = true; }

        if (buf.ipop() < shm.ipop_plan - 1) // pop data
        {
          _s_ll j = buf.ipop() - i1;
          if (j >= ndatr) { tr_rcv = -1; b_changed = true; return; }
          _s_ll npop2 = buf.pop(q.msg_rcv._pnonc_u()->pd() + j, ndatr - j);
          if (npop2 != 0) { b_changed = true; }
        }

        if (buf.ipop() == shm.ipop_plan - 1) // pop end byte, check if it's 1 (if the message is correct), or otherwise
        {
          if (buf.navl() < 1) { return; }
          char c_end(0); buf.pop(&c_end, 1); b_changed = true;
          if (c_end != 1) { q._mrcv_clear(); tr_rcv = 1; return; }
        }

        if (buf.ipop() == shm.ipop_plan)
        {
          cref_t<i_allocctl>& al = q.v_al.x_copy;
          if (al)
          {
            if (q.v_al.g_ver == q.iver_al) { try { al->notify_filled(q.msg_rcv); } catch (...) {} }
            q.iver_al = 0;
          }
          if (1 != q.msgs.push_1(q.msg_rcv)) { return; }
          q._mrcv_clear(); tr_rcv = 1; b_changed = true;
        }

        return;
      }
      else // anything except [-1..2]; attempt to recover
      {
        if (c_rcv != -2) { tr_rcv = -2; }
        q._mrcv_clear();
        if (b_exit) { return; }
        if (shm.ipush_plan < shm.ipop_plan) { tr_rcv = -1; b_changed = true; }
          else if (buf.ipop() < shm.ipop_plan) { const _s_ll nd = buf.discard(shm.ipop_plan - buf.ipop()); if (nd > 0) { b_changed = true; } }
          else if (buf.ipop() == shm.ipop_plan) { tr_rcv = 1; b_changed = true; }
          else { tr_rcv = -1; b_changed = true; }
        return;
      }
    }

    inline static void _part_do_send(shmqueue_ctx& q, bool b_exit, bool& b_changed)
    {
        // Sender.
        // tr_send
        //    0 - just started; set goto tr_send = 1
        //    1 - wait while a message appears on the queue, pop, goto tr_send = 2
        //    2 - push message length, bytes, end byte into the buffer, goto 1
        //    -1 - data buffer is broken and currently cannot be used; should wait for the receiver re-initializing the buffer
        //    -2 - data sending sequence is broken (probably sender process is restarted); try to recover:
        //      push zero bytes up to the end of incomplete message, then push the end byte indicating an error

      shmqueue_ctx::shmqueue_ctx_rso& shm = *q.buf.p();
      rfifo_nbl11& buf = shm.ringbuf;
      const signed char _tr_rcv = *q.buf.pf_state1();
      signed char& tr_send = *q.buf.pf_state2();


      signed char c_send = tr_send;
      if (q.b_sndr_waitrcvinit)
      {
        q._msend_clear();
          // NOTE _part_do_send may be called only on q.bufstate == 3,
          //  meaning that tr_send == 0 occurs right after successful buffer reinit.
          //  by the receiver side, and b_sender_waitrcv should be reset.
        if (!(c_send == 0 || c_send == 1)) { return; }
        q.b_sndr_waitrcvinit = false; b_changed = true;
      }
      if (q.b_just_started)
      {
        q.b_just_started = false;
        switch (c_send)
        {
          case -2: break;
          case -1: break;
          case 0: break;
          case 1: tr_send = c_send = 1; b_changed = true; break;
          case 2: tr_send = c_send = -2; b_changed = true; break;
          default: tr_send = c_send = -1; b_changed = true; break;
        }
      }
      if (c_send == -1 || _tr_rcv == -1) { q._msend_clear(); if (c_send != -1) { tr_send = c_send = -1; } q.b_sndr_waitrcvinit = true; b_changed = true; return; }


      if (c_send == 0 || c_send == 1)
      {
        if (_tr_rcv == -2) { q._msend_clear(); tr_send = -2; b_changed = true; return; }
        if (b_exit) { q._msend_clear(); tr_send = -2; return; }

        if (c_send == 0) { tr_send = 1; b_changed = true; }
        if (!q.pmsend2) // needs to check for a message in local queue
        {
          if (q.msgs.navl() <= 0) { return; }
          const cref_t<t_stringref>& r0 = q.msgs.front();
          if (r0)
          {
            if (!r0._pnonc_u()->is_valid()) { q.msgs.pop_1(); return; }
            q.pmsend2 = r0._pnonc_u();
            q.nmsend = q.msend2_n();
            shm.ipush_plan = (buf.ipush() + nb_mlen + q.nmsend + 1);
          }
          else
          {
            if (q.msgs.navl() < 3) { q.msgs.pop_1(); return; }
            const cref_t<t_stringref>& r1 = q.msgs[1];
            const cref_t<t_stringref>& r2 = q.msgs[2];
            if (!(r1 && r2 && r1._pnonc_u()->is_valid() && r2._pnonc_u()->is_valid())) { q.msgs.pop_n(3); return; }
            q.pmsend1 = r1._pnonc_u();
            q.pmsend2 = r2._pnonc_u();
            q.nmsend = q.msend1_n() + q.msend2_n();
            shm.ipush_plan = (buf.ipush() + nb_mlen + q.nmsend + 1);
          }
          b_changed = true;
        }
        if (buf.nfree() < nb_mlen) { return; } // the shm. buffer has not enough free place yet, to accept message length
        tr_send = 2; b_changed = true;
        return;
      }
      else if (c_send == 2)
      {
        if (_tr_rcv == -1) { tr_send = -1; q.b_sndr_waitrcvinit = true; b_changed = true; return; }
        if (_tr_rcv == -2) { q._msend_clear(); tr_send = -2; b_changed = true; return; }
        if (b_exit) { q._msend_clear(); tr_send = -2; return; }

        const _s_ll i0 = shm.ipush_plan - 1 - q.nmsend - nb_mlen;

        if (!(q.pmsend2 && buf.ipush() >= i0 && buf.ipush() <= shm.ipush_plan)) // invariant check
          { tr_send = -1; q.b_sndr_waitrcvinit = true; b_changed = true; return; }

        const _s_ll i1 = i0 + nb_mlen; // abs. index of the beginning of message prefix
        if (buf.ipush() < i1) // push user message string length
          { if (buf.nfree() < nb_mlen) { return; } char z[nb_mlen]; z[0] = char((q.nmsend >> 32) & 127); set_be4(z, 1, _s_long(q.nmsend)); buf.push(z, nb_mlen); b_changed = true; } // NOTE client data length bit 39 == 0 (reserved for possible protocol updates)

        const _s_ll ndat1 = q.msend1_n();
        const _s_ll i2 = i1 + ndat1; // abs. index of the beginning of main message(after prefix, if it exits)
        if (buf.ipush() < i2) // push prefix
        {
          const _s_ll j = buf.ipush() - i1; const char* pd = q.pmsend1->pd();
          if (j >= ndat1) { tr_send = -1; q.b_sndr_waitrcvinit = true; b_changed = true; return; }
          const _s_ll n = bmdx_minmax::myllrange_ub(buf.n() / __bmdx_shmfifo_push1_quot, __bmdx_shmfifo_push1_nbmin, ndat1 - j);
          if (0 != buf.push(pd + j, n)) { b_changed = true; }
          if (buf.ipush() < i2) { return; }
        }

        const _s_ll i3 = shm.ipush_plan - 1;
        if (buf.ipush() < i3) // push main message
        {
          const _s_ll j = buf.ipush() - i2; const _s_ll ndat2 = q.msend2_n(); const char* pd = q.pmsend2->pd();
          if (j >= ndat2) { tr_send = -1; q.b_sndr_waitrcvinit = true; b_changed = true; return; }
          const _s_ll n = bmdx_minmax::myllrange_ub(buf.n() / __bmdx_shmfifo_push1_quot, __bmdx_shmfifo_push1_nbmin, ndat2 - j);
          if (0 != buf.push(pd + j, n)) { b_changed = true; }
          if (buf.ipush() < i3) { return; }
        }

        if (buf.ipush() < shm.ipush_plan) // push end byte == 1, indicating successful msg. completion
          { if (0 != buf.push_bytes(1, 1)) { b_changed = true; } }

        if (buf.ipush() == shm.ipush_plan)
          { q._msend_clear(); tr_send = 1; b_changed = true; }

        return;
      }
      else // anything except [-1..2]; attempt to recover
      {
        if (c_send != -2) { tr_send = -2; }
        q._msend_clear();
        if (b_exit) { return; }

        if (buf.ipush() < shm.ipush_plan) { const bool bpushed = buf.nfree() > 0 && 0 != buf.push_bytes(0, shm.ipush_plan - buf.ipush()); if (bpushed) { b_changed = true; } }
          else if (buf.ipush() == shm.ipush_plan) { if (q.sndr_rcv_act >= 1) { tr_send = 1; b_changed = true; } }
          else { tr_send = -1; q.b_sndr_waitrcvinit = true; b_changed = true; }
        return;
      }
    }

    void _thread_proc()
    {
      const _s_ll idle_t_long_mcs = __bmdx_shmfifo_idle_t_long_mcs;
      const _s_ll idle_t_short_mcs = __bmdx_shmfifo_idle_t_short_mcs;
      const _s_ll idle_t_enable_time_ms = __bmdx_shmfifo_idle_t_enable_time_ms;

      t_ctxmap m2; _s_ll iver_m2 = -1;
      double t0_idle_en = clock_ms();
      while (1)
      {
        const bool b_exit = !!b_stop();
        if (!b_exit && !_part_update_ctxmap(m2, iver_m2)) { sleep_mcs(idle_t_long_mcs); continue; }
        bool b_changed = false;
        for (std::map<qmap_key, cref_t<shmqueue_ctx>, qmap_key::less>::iterator i = m2.begin(); i != m2.end(); ++i)
        {
          if (!i->second) { continue; }
          shmqueue_ctx& q = *i->second._pnonc_u();
          const bool b_validated = _part_validate_ctx1(q, b_changed);
          if (q.buf.b_side1()) { _part_update_conf_rcv(q, b_changed, b_validated); }
            else { _part_update_conf_send(q, b_changed, b_validated); }
          if (!b_validated) { continue; }
          try {
            if (q.buf.b_side1()) { _part_do_rcv(q, b_exit, b_changed); }
              else { _part_do_send(q, b_exit, b_changed); }
          } catch (...) { t0_idle_en = clock_ms(); b_changed = false; }
        }
        if (b_exit) { break; }
        if (b_changed) { t0_idle_en = clock_ms(); }
          else { sleep_mcs(clock_ms() - t0_idle_en < idle_t_enable_time_ms ? idle_t_short_mcs : idle_t_long_mcs); }
      }
    }
  };

  struct deinit_handler { _s_long& s; deinit_handler(_s_long& s_) : s(s_) {} ~deinit_handler() { critsec_t<t_lks_qq> __lock(10, -1); if (sizeof(__lock)) {} s = 1; } };

  static bool _th_enable()
  {
    static _s_long fl_deinit(0); static deinit_handler __local_dh(fl_deinit); if (sizeof(__local_dh)) {}
    static cref_t<th_handler> x;
    static volatile _s_long objstate(0);
    if (objstate < 2 && !fl_deinit)
    {
      critsec_t<t_lks_qq> __lock(10, -1); if (sizeof(__lock)) {}
      if (!fl_deinit)
      {
        if (objstate < 1) { x.cm_create0(0, 0, 1); if (x) { objstate = 1; } }
        if (objstate == 1)
        {
          bool b = x->tc.start_auto<_ipc_delivery_thread>(int());
          if (b)
          {
            objstate = 2;
            #ifdef _bmdxpl_Psx
              x->tc.set_priority(5);
            #endif
          }
        }
      }
    }
    return objstate == 2 && x;
  }

  static cref_t<i_shmqueue_ctxx> mqq()
  {
    static _s_long fl_deinit(0); static deinit_handler __local_dh(fl_deinit); if (sizeof(__local_dh)) {}
    static cref_t<i_shmqueue_ctxx> x;
    static volatile _s_long objstate(0);
    if (objstate < 1 && !fl_deinit)
    {
      critsec_t<t_lks_qq> __lock(10, -1); if (sizeof(__lock)) {}
      if (!fl_deinit)
      {
        if (objstate < 1) { x = cref_t<_shmqueue_ctxx_impl>::iref2<i_shmqueue_ctxx>::create0(1); if (x) { objstate = 1; } }
      }
    }
    return x;
  }
};










namespace _api // public API, merged into namespace bmdx_shm
{

    // Specialized memory allocator, allowing shmqueue_s to avoid dynamic allocations
    //  on all stages of interprocess message transfer.
    //  The memory is reserved at once on allocator creation.
    //  The allocator is then attached by client to particular queue with shmqueue_s::conf_set_al_in.
    //  Large message transfer time is nearing to 2*memcpy of message data.
    //  For short (< 100 B) messages, the additional overhead may be noticeable
    //    (due to infrequent sleeping on a lock, and allocation logic execution).
    //    Anyway, pre-allocated memory is faster than conventional dynamic allocation.
    //
    // Details: see allocctl_pre::create.
    //
  struct allocctl_pre : i_allocctl
  {
      // Allocator creation. See impl. for descr.
    static inline cref_t<i_allocctl> create(_s_long flags_, _s_ll nbdata_, _s_ll hint_nobj_max_ = -1) __bmdx_noex;

      // Returns the exact amount of memory, pre-allocated by allocctl_pre::create,
      //  given the same arguments.
      // See create() impl. for description of how this memory will be used.
    static inline _s_ll nbmem(_s_ll nbdata_, _s_ll nobj_max_) __bmdx_noex;

      // Convenience function, allocating single area (zero-filled and zero-terminated)
      //  in a way, specified by flags.
      //  The allocator is created for that purpose only temporarily.
      // flags, nbdata: see impl. of create().
    static inline cref_t<t_stringref> make_rba_special(_s_long flags, _s_ll nbdata) __bmdx_noex
      { cref_t<i_allocctl> al = create(flags, nbdata, 1); return al ? al->make_rba(nbdata) : cref_t<t_stringref>(); }

      // See create() impl. for flags descr.
    enum Eflags
    {
      fl_allow_fallback_calloc = 0x1,
      fl_use_mapping = 0x2,
      fl_use_pagelock = 0x4,
      fl_force_fetch = 0x8,
      fl_pagelock_is_critical = 0x10
    };

  private:
    enum _Ex_flags { _fl_client_mask = 0xff, _fl_revert_ws = 0x100 };
    cref_t<t_stringref> a; // handles whole mem. area
    struct _header_whole; struct _header_block;
    static inline void release_hw(_header_whole& hw) __bmdx_noex;
    static inline _s_ll nbblkovr();
    static inline _s_long cref_ev_handler(_s_long optype, _s_ll flags, _s_ll nrefs, _s_ll delta, _critsec_data0_t<t_stringref>* pcsd, void* pobj, void* pinterface, void* paux);
    inline cref_t<t_stringref> make_rba(_s_ll nb);
  };




  struct allocctl_pre::_header_whole { _t_stringref mem; union { _s_long flags; _s_ll __pad1; }; _s_ll ipop, nbbusy; };
  struct allocctl_pre::_header_block { _s_ll nbblock, nrefsblock; enum { nboff_msg = 2 * 8 }; _t_stringref msg; enum { nbblock_min = 16 }; }; // followed by user data + 1 zero byte + alignment up to 7 bytes




    // The function creates specialized memory allocator,
    //  and initially allocates for it long contiguous area of memory.
    //  The memory is initially zero-initialized, but on reuse may contain anything.
    //
    // The allocator, at each call (make_rba), returns smart pointers
    //  to sequentially allocated regions in this memory.
    //  The allocator is oriented for use with shmqueue_s::mget,
    //  for the client being able to receive series of IPC messages
    //  (large messages, or multiple small messages)
    //  without any dynamic allocations during this operation.
    //
    // For fully pre-allocated message receival, make several preparations on the receiving side:
    //
    //    1. Configure the input (receiver side) local queue to have fixed capacity:
    //        q.conf_set_lqcap(true, -1, ncap, ncap);
    //      See also vnnqueue_t:: set_cap_hints, set_rsv.
    //    2. Create an allocator, e.g.
    //        al = allocctl_pre::create(0xff, nb, nobj);
    //      This at once reserves and prepares the necessary amount of memory,
    //        == nbmem(nbdata_, hint_nobj_max_).
    //      (This includes small additional reserves, per-allocator and per-object.)
    //    3. Assign the allocator to the queue:
    //        q.conf_set_al_in(&al);
    //
    //  After that, any incoming IPC message will be written by internal thread into memory,
    //      obtained from allocator's make_rba. The resulting smart pointer to memory
    //      will be pushed into local input queue, and thus becomes available via mget.
    //    The client is expected to pop IPC messages (shmqueue_s::mget), use them as necessary,
    //      and release (forget smart pointer), approximately in the same order
    //      as they have been allocated, i.e. in the order of incoming messages.
    //    The allocator wraps around whole reserved area of memory,
    //      in progress of client releasing the earliest allocated regions.
    //  NOTE The allocator from allocctl_pre::create is thread-safe,
    //      and, if wanted, one instance may be used in several input queues plus on the client side.
    //
    // Arguments:
    //  nbdata_, hint_nobj_max_ -
    //    control how much memory should be pre-allocated by allocctl_pre::create
    //    for incoming IPC messages data.
    //    The scheme of allocator operation, described above, gives a hint on this value:
    //      if the receiving client expects  max. N objects to exist
    //      simultaneously on its side,
    //      nbdata_ should define a space, enough for N + 1 such objects
    //    hint_nobj_max_ = N + 1.
    //      NOTE If all objects have exactly same size, N+1 may be relaxed to N.
    //  flags_ -
    //    ORed flags for fine-tuning allocator's behavior.
    //      0x1 (fl_allow_fallback_calloc) - if the allocator cannot give the requested amount of memory,
    //        it falls back to default shmqueue_s behavior (make_rba_z(n, 1), which is based on calloc).
    //        If the flag is not set, in this situation, the allocator returns an empty object,
    //        which factually results in shared memory queue being paused, until the client releases
    //        the oldest allocated region(s), and the allocator may reuse them.
    //      0x2 (fl_use_mapping) - if set, use mmap (*nix) / CreateFileMapping (Windows)
    //        to initially allocate the memory.
    //        If the flag is not set, allocctl_pre uses make_rba_z().
    //      0x4 (fl_use_pagelock) - if set, apply mlock (*nix) / VirtualLock (Windows)
    //        to the initially allocated memory. If this succeeds, the memory becomes not swappable.
    //      0x8 (fl_force_fetch) - if set, write small amount of null bytes (sparsely)
    //        into the initially allocated memory. This forces all pages to be available
    //        later for read/write without any commit-on-page-fault overhead.
    //      0x10 (fl_pagelock_is_critical) - fail if fl_use_pagelock is set, but page locking failed.
    //        allocctl_pre::create will return an empty object.
    //        (If fl_use_pagelock is not set, fl_pagelock_is_critical is ignored.)
    //
  cref_t<i_allocctl> allocctl_pre::create(_s_long flags_, _s_ll nbdata_, _s_ll hint_nobj_max_) __bmdx_noex
  {
    #define __bmdx_allocctl_pre_csname_base "bmdx_shm::allocctl_pre::create_"
    flags_ &= _fl_client_mask;
    cref_t<i_allocctl> rv = cref_t<allocctl_pre>::iref2<i_allocctl>::create0(1);
    if (!rv) { return rv; }
    const _s_ll nb_alloc = nbmem(nbdata_, hint_nobj_max_);
    allocctl_pre* pa = static_cast<allocctl_pre*>(rv._pnonc_u());
    _header_whole* phw = 0;

      // 1. Allocate.
    if (flags_ & fl_use_mapping)
    {
      #ifdef _bmdxpl_Psx
        int f = MAP_PRIVATE | MAP_ANONYMOUS;
        #ifdef MAP_POPULATE
          f |= MAP_POPULATE;
        #endif
        void* p = mmap(0, size_t(nb_alloc), PROT_READ|PROT_WRITE, f, -1, 0);
        if (p && p != MAP_FAILED) { phw = (_header_whole*)p; }
      #elif defined(_bmdxpl_Wnds)
        HANDLE hfm = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, DWORD(nb_alloc >> 32), DWORD(nb_alloc & 0xffffffffll), NULL);
        if (hfm)
        {
          void* p = MapViewOfFile(hfm, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_T(nb_alloc));
          CloseHandle(hfm);
          if (p) { phw = (_header_whole*)p; }
        }
      #endif
    }
    else
    {
      phw = (_header_whole*)iref2_args_t<t_stringref>::sf_alloc(nb_alloc);
    }
    if (!phw) { rv.clear(); return rv; }

      // 2. Initialize.
    phw->mem = t_stringref((char*)phw, nb_alloc);
    phw->__pad1 = 0;
    phw->flags = flags_;
    phw->ipop = 0;
    phw->nbbusy = 0;

      // 3. Lock.
    if (phw->flags & fl_use_pagelock)
    {
      #ifdef _bmdxpl_Psx
        bool b = 0 == mlock(phw->mem.pd(), (size_t)phw->mem.n());
      #elif defined(_bmdxpl_Wnds)
        SIZE_T nbx = (SIZE_T)phw->mem.n();
        bool b = !!VirtualLock(phw->mem.pd(), nbx);
        if (!b)
        {
          HANDLE hp = GetCurrentProcess(); SIZE_T nbmin = 0, nbmax = 0;
          bool bto = false;
          t_name_shm csname(__bmdx_allocctl_pre_csname_base); csname += GetCurrentProcessId();
          critsec_gn cs(csname.c_str()); if (1) { double t0 = clock_ms(); while (!cs.lock()) { sleep_mcs(10000); if (clock_ms() - t0 > 3000) { bto = true; break; } } }
          if (!bto && !!GetProcessWorkingSetSize(hp, &nbmin, &nbmax))
          {
            nbmin += nbx; nbmax += nbx;
            if (!!SetProcessWorkingSetSize(hp, nbmin, nbmax)) { b = true; phw->flags |= _fl_revert_ws; }
          }
        }
      #endif
      if (!b)
      {
        phw->flags &= ~_s_long(fl_use_pagelock);
        if (phw->flags & fl_pagelock_is_critical)
        {
          release_hw(*phw); rv.clear();
          return rv;
        }
      }
    }

      // 4. Acquire all pages.
    if (phw->flags & fl_force_fetch) { for (_s_ll i = sizeof(_header_whole); i < nb_alloc; i += 511) { ((char*)phw)[i] = 0; } }

      // 5. Embed into the allocator object.
    struct f : iref2_flags {};
    pa->a = cref_t<t_stringref>::iref2<t_stringref>::assign(1, phw->mem, 0, int(f::use_release) | f::gen_nrefs | f::use_critsec, cref_ev_handler);
    if (!pa->a) { release_hw(*phw); rv.clear(); return rv; }

    return rv;
  }

  _s_ll allocctl_pre::nbmem(_s_ll nbdata_, _s_ll nobj_max_) __bmdx_noex
  {
    _s_ll n = nbdata_ > 0 ? nbdata_ : 0;
    if (nobj_max_ < 1) { nobj_max_ = 1; }
    n += sizeof(_header_whole) + nobj_max_ * nbblkovr();
    return n;
  }

  void allocctl_pre::release_hw(_header_whole& hw) __bmdx_noex
  {
    if (!hw.mem.pd()) { return; }
    #ifdef _bmdxpl_Psx
      if (hw.flags & fl_use_pagelock) { munlock(hw.mem.pd(), (size_t)hw.mem.n()); }
      if (hw.flags & fl_use_mapping) { munmap(hw.mem.pd(), (size_t)hw.mem.n()); }
        else { iref2_args_t<t_stringref>::sf_free(hw.mem.pd()); }
    #elif defined(_bmdxpl_Wnds)
      SIZE_T nbx = (SIZE_T)hw.mem.n();
      if (hw.flags & fl_use_pagelock) { VirtualUnlock(hw.mem.pd(), nbx); }
      if (hw.flags & _fl_revert_ws)
      {
        HANDLE hp = GetCurrentProcess(); SIZE_T nbmin = 0, nbmax = 0;
        bool bto = false;
        t_name_shm csname(__bmdx_allocctl_pre_csname_base); csname += GetCurrentProcessId();
        critsec_gn cs(csname.c_str()); if (1) { double t0 = clock_ms(); while (!cs.lock()) { sleep_mcs(10000); if (clock_ms() - t0 > 3000) { bto = true; break; } } }
        if (!bto && !!GetProcessWorkingSetSize(hp, &nbmin, &nbmax) && nbmin >= nbx && nbmax >= nbx)
        {
          nbmin -= nbx; nbmax -= nbx;
          SetProcessWorkingSetSize(hp, nbmin, nbmax);
        }
      }
      if (hw.flags & fl_use_mapping) { UnmapViewOfFile(hw.mem.pd()); }
        else { iref2_args_t<t_stringref>::sf_free(hw.mem.pd()); }
    #endif
  }

  _s_ll allocctl_pre::nbblkovr() { return sizeof(_header_block) + 8; }

  cref_t<_t_stringref> allocctl_pre::make_rba(_s_ll nb)
  {
    if (nb < 0 || !a) { return cref_t<_t_stringref>(); }
    cref_t<_t_stringref>::t_lock __lock(a); if (sizeof(__lock)) {}
    _header_whole& hw = (_header_whole&)a._rnonc();
    _t_stringref bmem(hw.mem.pd() + sizeof(_header_whole), hw.mem.n() - sizeof(_header_whole));
    const _s_ll nb2 = (nb & ~_s_ll(7)) + nbblkovr();
    _s_ll ifree = hw.ipop + hw.nbbusy;
    _s_ll nbcontig;
    if (ifree < bmem.n())
    {
      nbcontig = bmem.n() - ifree;
      if (nbcontig < nb2) // not enough place without wraparound
      {
        if (hw.nbbusy > 0)
        {
          if (nbcontig >= _header_block::nbblock_min)
          {
            _header_block& hb = (_header_block&)bmem[ifree];
            hb.nbblock = nbcontig;
            hb.nrefsblock = 0;
          }
          hw.nbbusy += nbcontig;
          ifree = 0;
          nbcontig = hw.ipop;
        }
        else
        {
          hw.ipop = 0;
          hw.nbbusy = 0;
          ifree = 0;
          nbcontig = bmem.n();
        }
      }
    }
    else
    {
      ifree %= bmem.n();
      nbcontig = hw.ipop - ifree;
    }
    if (nbcontig < nb2) { return !!(hw.flags & fl_allow_fallback_calloc) ? make_rba_z(nb, 1) : cref_t<_t_stringref>(); }
    _header_block& hb = (_header_block&)bmem[ifree];
    hb.nbblock = nb2;
    hb.nrefsblock = 1;
    hb.msg._link_u((char*)(&hb + 1), nb);
    hb.msg[nb] = 0; // force new array to be zero-terminated; additional byte after msg is accounted in nbblkovr()
    hw.nbbusy += nb2;
    return cref_t<t_stringref>::iref2<t_stringref>::_refcast_replace_u(a, &hb.msg);
  }

  _s_long allocctl_pre::cref_ev_handler(_s_long optype, _s_ll flags, _s_ll nrefs, _s_ll delta, _critsec_data0_t<t_stringref>* pcsd, void* pobj, void* pinterface, void* paux)
  {
    (void)flags; (void)nrefs; (void)delta; (void)pinterface; (void)pcsd; (void)paux;
    struct f : iref2_flags {};
    if (optype == f::ev_release)
    {
      t_stringref& rmem = *(t_stringref*)pobj;
      if (!(rmem.pd() && rmem.n() > 0)) { return 1; }
      _header_whole& hw = *(_header_whole*)rmem.pd();
      release_hw(hw);
      return 1;
    }
    if (optype == f::ev_nrefs)
    {
      t_stringref& rmem = *(t_stringref*)pobj;
      if (!(rmem.pd() && rmem.n() > 0)) { return 1; }
      if (pinterface != rmem.pd()) // one of blocks, made by allocator
      {
        _header_whole& hw = *(_header_whole*)rmem.pd();
        _t_stringref bmem(hw.mem.pd() + sizeof(_header_whole), hw.mem.n() - sizeof(_header_whole));
        _header_block& hbcur = *(_header_block*)((char*)pinterface - _header_block::nboff_msg);
        hbcur.nrefsblock += delta;
        if (hbcur.nrefsblock <= 0 && (void*)&hbcur == (void*)&bmem[hw.ipop]) // the block is freed and it was the first in sequence
        {
          while (hw.nbbusy > 0)
          {
            if (hw.nbbusy < _header_block::nbblock_min)
            {
              hw.ipop = (hw.ipop + hw.nbbusy) % bmem.n();
              hw.nbbusy = 0;
              break;
            }
            _header_block& hb2 = (_header_block&)bmem[hw.ipop];
            if (hb2.nrefsblock > 0) { break; } // the current block is not free yet
              // The current block is free.
            const _s_ll nb2 = hb2.nbblock;
            if (nb2 > hw.nbbusy) { break; } // not expected
            hw.ipop = (hw.ipop + nb2) % bmem.n();
            hw.nbbusy -= nb2;
              // Check the next block size.
            const _s_ll nbtail = bmdx_minmax::myllmin(bmem.n() - hw.ipop, hw.nbbusy);
            if (nbtail < _header_block::nbblock_min) // skip the block if it's too small for header
            {
              hw.ipop = (hw.ipop + nbtail) % bmem.n();
              hw.nbbusy -= nbtail;
            }
          }
        }
      }
      return 1;
    }
    return 0;
  }




    // IPC queue of strings, based on shared memory (FIFO buffer).
    //  The shmqueue_s object is designed to work with both sides of the queue, as separate objects.
  struct shmqueue_s
  {
    typedef t_name_shm t_name;
    typedef arrayref_t<char> t_stringref;

      // Global (interprocess) name of the queue.
    const t_name name; // bmdx_str::flstr_t<300>

      // Cached result of the last operation.
    mutable _s_long res;

      // Cached data reference of the last pop or peek operation (see also mget()).
      // NOTE If the client uses mget_str to retrieve messages, d is kept empty.
    cref_t<t_stringref> d;


  private:
    const _s_ll _nbdflt; const _s_long _flags; const _s_long _idle_t_mcs;
    mutable cref_t<shmqueue_ctx> _rq; // cached reference to the queue with the specified name
    struct exc_new_pack3 : std::exception { const char* what() const __bmdx_noex { return "new_pack3::operator()(t*)"; } };
    struct new_pack3 { typedef cref_t<t_stringref> t; const t &prefix, &msg; mutable _s_long ind; new_pack3(const t& prefix_, const t& msg_) : prefix(prefix_), msg(msg_), ind(-1) {} void operator()(t* p) const { ++ind; if (ind == 0) { new (p) t(); } else if (ind == 1) { new (p) t(prefix); } else if (ind == 2) { new (p) t(msg); } else { throw exc_new_pack3(); } } };
  public:


      // name_: global name of the queue.
      // nbdflt_: IPC queue buffer size hint. Dflt. == -1, which means use __bmdx_shmfifo_nbytes_dflt.
      // flags_ (ORed): see flags(). Default value == 1 (use client-side locks).
      // idle_t_mcs_: sleep time at each iteration inside client-side blocking mget or msend (0..1e6 mcs). Dflt. 5000 mcs.
    shmqueue_s(const t_name& name_, const _s_ll nbdflt_ = -1, _s_long flags_ = 1, _s_ll idle_t_mcs_ = -1) __bmdx_noex
      : name(name_.n() ? name_ : "_"), res(0), _nbdflt(nbdflt_), _flags(flags_), _idle_t_mcs(idle_t_mcs_ >= 0 ? _s_long(bmdx_minmax::myllmin(1000000, idle_t_mcs_)) : 5000)
    { if (_flags & 4) { bufstate(2); } if (_flags & 8) { bufstate(3); } }

      // Flags, set by constructor.
      //    0x1 - use client-side locks when sending and receiving messages.
      //      Dflt. value: the flag is set.
      //      Needed only in case if on client side there that more than one thread
      //        in the current process is sending or receiving messages through the same queue
      //        (multiple shmqueue_s with same name).
      //      NOTE shmqueue_s object itself is not protected from concurrency.
      //        Unsetting this flag disables only locks on local queue push/pop
      //        (queues as such are non-blocking by design).
      //    0x2 - optimistic push on receiver not ready.
      //        Dflt. value: the flag is not set.
      //        Responsibility for initializing the queue object in shared memory is on receiver's side.
      //        If this flag is set, msend() will put the message into the local queue:
      //            a) (normal) if shared memory queue is completely initialized,
      //            b) if timeout_ms == 0, and shared memory queue currently does not exist,
      //            c) if timeout_ms == 0, and shared memory queue is not yet completely initialized,
      //            d) if shared memory exists, but the receiver is not active.
      //            NOTE This does NOT include cases when the shared memory object is locked, or the queue is disabled by the client.
      //        If this flag not set, only the above case (a) works.
      //    0x4 - constructor's flag (already done): automatically connect to sender side, and create local queue for msend (i.e. call bufstate(2)).
      //    0x8 - constructor's flag (already done): automatically connect to receiver side, and create local queue for mget (i.e. call bufstate(3)).
      // NOTE flags() does not modify this->res.
    _s_long flags() const __bmdx_noex { return _flags; }



      // Sets custom allocator to the input (mget) queue.
      //
      //  p_al:
      //    a) p_al != 0, p_al.ptr() != 0: the new allocator object.
      //      NOTE *p_al may be any kind of reference.
      //         The client is responsible for object's validity during whole period of its use by shmfifo_s impl.
      //    b) p_al != 0, p_al.ptr() == 0: signals that any existing custom allocator must be removed.
      //    c) p_al == 0: do not change the current allocator, do wait and prev. setting result check only.
      //
      //  timeout_ms: timeout in milliseconds, to wait for the setting being applied on the side of internal thread.
      //    The timeout parameter is similar to that in lqwait_in (see).
      //
      // Returns:
      //  1 - the setting has succeeded.
      //  0 - a) started setting, but not waited (timeout_ms == 0).
      //    b) the setting is not yet done.
      //  -1 - timeout (on timeout_ms > 0).
      //  -2 - the setting has failed to start, or other kind of failure.
      //  -3 (not expected) - the setting has failed inside the internal thread. The current settings not changed.
      //  -4 - the client wants check only for setting result, but no setting changes previously requested by the client.
      //  -5 - configuration setting is unsafe, because client-side locks are disabled: (flags() & 1) == 0.
      //  In addition, this->res holds the result of the last bufstate() call.
      //
    _s_long conf_set_al_in(const cref_t<i_allocctl>* p_al = 0, double timeout_ms = -1) const __bmdx_noex
    {
      if (!(_flags & 1)) { return -5; }
      _s_long res2 = bufstate(3); if (!(res2 >= 3 && res2 <= 5)) { return -2; }
      if (!_rq) { return -2; }
      if (_rq->buf.b_side1() != true) { return -2; } // not expected to occur
      _s_ll vs = 0; bool b_vs = false;
      if (p_al)
      {
        b_vs = true;
        critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
        unsigned char _temp = 0;
        if (_rq->v_al.s_is_done(0, _temp) == 1 && !*p_al && !_rq->v_al.x_copy) { return 1; } // do not reset the allocator if it's already done
        cref_t<i_allocctl> empty;
        _rq->v_al.s_begin_update(*p_al ? *p_al : empty, vs);
      }
      if (timeout_ms == 0) { return 0; }
      const double t0 = clock_ms();
      while (1)
      {
        if (1)
        {
          critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
          unsigned char res3 = 0;
          _s_long res4 = _rq->v_al.s_is_done(b_vs ? &vs : 0, res3);
          if (res4 != 0)
          {
            if (res3 == 2) { return 1; }
            if (res3 == 0) { return -4; }
            return -3;
          }
        }
        if (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms) { return -1; }
        sleep_mcs(_idle_t_mcs);
        if (!_shmqueue_ctxx_impl::_th_enable()) { return -2; } // the application may be exiting during waiting
      }
    }
      //
      // Gets custom allocator of the input (mget) queue, previously set by conf_set_al_in.
      //
      // Returns:
      //  1 - r_al received the last set custom allocator object.
      //    May be empty (.ptr() == 0):
      //      a) the last set allocator was empty (for clearing the previous one set).
      //      b) no conf_set_al_in had been called on this queue before.
      //      In both cases, instead of custom allocator, the internal thread
      //      currently uses default allocation procedure (bmdx_shm::_bmdx_shm::make_rba_z).
      //  0 - same as 1, but the last set object is not yet accepted by internal thread.
      //  -2 - failure, r_al is cleared.
      //  In addition, this->res holds the result of the last bufstate() call.
      //
    _s_long conf_get_al_in(cref_t<i_allocctl>& r_al) const __bmdx_noex
    {
      r_al.clear();
      _s_long res2 = bufstate(3); if (!(res2 >= 3 && res2 <= 5)) { return -2; }
      if (!_rq) { return -2; }
      if (_rq->buf.b_side1() != true) { return -2; } // not expected to occur
      critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
      r_al = (const cref_t<i_allocctl>&)_rq->v_al.x;
      res2 = 1; unsigned char res3 = 0;
      if (_rq->v_al.s_is_done(0, res3) != 1) { res2 = 0; }
      return res2;
    }
      //
      // Optionally sets queue capacity and/or place reserve (if any n* is non-default).
      // Optionally (timeout_ms != 0) waits until the last given settings are applied.
      //
      //  b_receiver:
      //    true - input queue (mget).
      //      For input queue, conf_set_lqcap succeeds only if bufstate(3) returns 3..5 (the shared queue is valid).
      //    false - output queue (msend).
      //      For output queue, conf_set_lqcap succeeds if bufstate(2) >= 0.
      //      (i.e. the shared queue may be invalid, but local queue already created).
      //  ncapmin, ncapmax - queue capacity hints.
      //    The default values are "do not limit queue shrinking or expansion".
      //    Details: see vnnqueue_t :: set_cap_hints.
      //    To skip capacity modification, specify <= -3 to any of ncap*.
      //  nrsv >= 0 - reserves at least nrsv positions in the queue.
      //    Details: see vnnqueue_t :: set_nrsv.
      //    To skip the reserve modification, specify <= -1 to nrsv.
      //
      //  timeout_ms: timeout in milliseconds, to wait for the setting being applied on the side of internal thread.
      //    The timeout parameter is similar to that in lqwait_in/_out (see).
      //
      // Returns:
      //  1 - the setting has succeeded.
      //  0 - a) started setting, but not waited (timeout_ms == 0).
      //    b) the setting is not yet done.
      //  -1 - timeout (on timeout_ms > 0).
      //  -2 - the setting has failed to start, or other kind of failure.
      //      NOTE For sender queue (b_receiver == false), failure may occur if the receiving end is not yet connected
      //        (i.e. bufstate() < 3).
      //  -3 - the setting has failed inside the internal thread. The current settings not changed.
      //  -4 - the client wants check only for setting result, but no setting changes previously requested by the client.
      //  -5 - configuration setting is unsafe, because client-side locks are disabled: (flags() & 1) == 0.
      //  In addition, this->res holds the result of the last bufstate() call.
      //
    _s_long conf_set_lqcap(bool b_receiver, _s_ll ncapmin = -3, _s_ll ncapmax = -3, _s_ll nrsv = -3, double timeout_ms = -1) const __bmdx_noex
    {
      if (!(_flags & 1)) { return -5; }
      _s_long res2 = bufstate(b_receiver ? 3 : 2);
        if (b_receiver && !(res2 >= 3 && res2 <= 5)) { return -2; }
        if (!b_receiver && !(res2 >= 0)) { return -2; }
      if (!_rq) { return -2; }
      if (_rq->buf.b_side1() != b_receiver) { return -2; } // not expected to occur
      shmqueue_ctx::lqconf_cap c;
        c.ncapmin = ncapmin;
        c.ncapmax = ncapmax;
        c.nrsv = nrsv;
      _s_ll vs = 0; bool b_vs = false;
      if (c.is_valid_cap() || c.is_valid_rsv())
      {
        b_vs = true;
        critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
        unsigned char _temp = 0;
        if (_rq->v_inq_cap.s_is_done(0, _temp) == 1 && c == _rq->v_inq_cap.x_copy) { return 1; } // do not modify settings if they are not changed
        _rq->v_inq_cap.s_begin_update(c, vs);
      }
      if (timeout_ms == 0) { return 0; }
      const double t0 = clock_ms();
      while (1)
      {
        if (1)
        {
          critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
          unsigned char res3 = 0;
          _s_long res4 = _rq->v_inq_cap.s_is_done(b_vs ? &vs : 0, res3);
          if (res4 != 0)
          {
            if (res3 == 2) { return 1; }
            if (res3 == 0) { return -4; }
            return -3;
          }
        }
        if (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms) { return -1; }
        sleep_mcs(_idle_t_mcs);
        if (!_shmqueue_ctxx_impl::_th_enable()) { return -2; } // the application may be exiting during waiting
      }
    }
      //
      // Gets actual local queue capacity parameters, previously successfully set by conf_set_lqcap call(s).
      //
      // Returns:
      //  1 - non-null p* have received the last set queue parameters.
      //    NOTE If no conf_set_lqcap had been called on this queue before, all *pn* will be == -3.
      //      In this case, the local queue uses default capacity settings, originally set by vnnqueue_t constructor.
      //  0 - same as 1, but the most recent parameters are not accepted by internal thread (not yet, or setting failed).
      //    In this case, *pn* reflect the previously set values.
      //  -2 - failure, all *pn* are set to -3.
      //  In addition, this->res holds the result of the last bufstate() call.
      //
    _s_long conf_get_lqcap(bool b_receiver, _s_ll* pncapmin = 0, _s_ll* pncapmax = 0, _s_ll* pnrsv = 0) const __bmdx_noex
    {
      if (pncapmin) { *pncapmin = -3; } if (pncapmax) { *pncapmax = -3; } if (pnrsv) { *pnrsv = -3; }
      _s_long res2 = bufstate(b_receiver ? 3 : 2);
        if (b_receiver && !(res2 >= 3 && res2 <= 5)) { return -2; }
        if (!b_receiver && !(res2 >= 0)) { return -2; }
      if (!_rq) { return -2; }
      if (_rq->buf.b_side1() != b_receiver) { return -2; } // not expected to occur
      critsec_t<shmqueue_ctx> __lock(10, -1, &_rq->csd); if (sizeof(__lock)) {}
      res2 = 1; unsigned char res3 = 0;
      if (_rq->v_inq_cap.s_is_done(0, res3) != 1) { res2 = 0; }
      volatile_cvar_t<shmqueue_ctx::lqconf_cap>&  v_ = _rq->v_inq_cap_back;
      v_.g_update_pre(); v_.g_update_complete(2);
      if (pncapmin) { *pncapmin = v_.x_copy.ncapmin; } if (pncapmax) { *pncapmax = v_.x_copy.ncapmax; } if (pnrsv) { *pnrsv = v_.x_copy.nrsv; }
      return res2;
    }



      // navl(bool): returns the number of available messages (>= 0),
      //    depending on direction - ones that are not sent yet,
      //    or ones just received and not popped yet.
      //  b_receiver:
      //    specifies, for what end of the queue the function is called.
      // navl_out: shortcut to navl(false).
      // navl_in: shortcut to navl(true).
      //
      // NOTE navl*() value is volatile. The local queue may grow or shrink
      //    due to internal thread working.
      // NOTE navl*() do not make automatic initialization of queues (as msend, mget do).
      // NOTE navl*() do not modify this->res.
      //
    _s_ll navl(bool b_receiver) const __bmdx_noex
    {
      if (!_shmqueue_ctxx_impl::_th_enable()) { return 0; }
      if (!_rq) { try { _rq = _shmqueue_ctxx_impl::mqq()->rqueue(name, b_receiver ? 1 : 0, _nbdflt); } catch (...) {} }
      if (!_rq) { return 0; }
      return _rq->msgs.navl();
    }
    _s_ll navl_out() const __bmdx_noex { return navl(false); }
    _s_ll navl_in() const __bmdx_noex { return navl(true); }

      // Convenience functions.
      //  A. For sender (lqwait_out): waits until a) navl_out() <= n, or b) timeout occurs.
      //    NOTE This is particularly useful if, after sending some messages,
      //      the program has to exit.
      //      It may call lqwait_out(timeout) to give internal thread time
      //      to complete messages sending.
      //  B. For recevier (lqwait_in): waits until a) navl_in() >= n, or b) timeout occurs.
      // timeout_ms:
      //    <0 - no timeout, wait for (a) infinitely.
      //    0 - check local queue state only once and exit immediately.
      //    >0 - returns on reaching (a) or (b) condition.
      // Returns:
      //  lqwait_out: the last value of navl_out(),
      //  lqwait_in: the last value of navl_in().
      //
      // NOTE lqwait_*() do not make automatic initialization of queues (as msend, mget do).
      // NOTE lqwait_*() do not modify this->res.
      //
    _s_ll lqwait_out(double timeout_ms, _s_ll n = 0) const __bmdx_noex        { const double t0 = clock_ms(); while (1) { const _s_ll na = navl_out(); if (na <= n || timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { return na; } sleep_mcs(_idle_t_mcs); } }
    _s_ll lqwait_in(double timeout_ms, _s_ll n = 1) const __bmdx_noex       { const double t0 = clock_ms(); while (1) { const _s_ll na = navl_in(); if (na >= n || timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { return na; } sleep_mcs(_idle_t_mcs); } }




      // 1. Returns the current local queue state, and ipush, ipop values (message count).
      // 2. Optionally (if the queue is sender && iend_sender >= -1),
      //    sets a signal to release, from local queue,
      //    all messages that are currently unsent or partially sent (i.e. all messages with numbers in range [ipop .. ipush).
      //    Messages are released by internal thread, by popping without any processing,
      //      in short time after lqstate() returns.
      //      Under normal conditions, the client may rely on or wait for release completion
      //      (navl() == 0, lqstate() == 0) in 50..100 ms. See also lqwait_out.
      // b_receiver:
      //    specifies, for what end of the queue the function is called.
      // iend_sender (together with dtms_end):
      //    a) -2 (normal mode) - do not modify local queue, send messages as normal. Ignore dtms_end.
      //    b) -1 - for all msgs. that are available currently:
      //        after dtms_end ms from now, pop (lose) ones that are not yet sent.
      //        If pipush != 0, *pipush receives the index, until which the messages will be popped.
      //        Messages, pushed later (larger index), if any, are not popped.
      //    c) >= 0: after dtms_end ms from now, pop (lose) messages with index in local queue < iend_sender
      //        (i.e. pushed earlier than certain one).
      //    d) invalid args.: dtms_end < 0, or iend_sender <= -3.
      //    NOTE Even with dtms_end > 0, lqstate returns immediately. Local queue cleanup
      //      after the timeout is done by internal thread.
      //    NOTE Cases (b) and (c) offer reliable way to release all client message objects (cref_t<t_stringref>)
      //      during limited amount of time, which is approx.
      //      (dtms_end + __bmdx_shmfifo_idle_t_long_mcs/1000. + 10)
      //      milliseconds.
      //      Some of use cases in applications:
      //        1) all client messages, created in the shared library B, but sent through shmqueue_s in module A (executable or shared library),
      //            must be released before unloading library A (i.e. before its static deinitialization).
      //        2) if immediate sending of messages appears to be impossible (no recipient or it's hanged),
      //            it may be necessary to discard the currently existing messages.
      //            The application may detect that the local queue is "hanged" (e.g. ipop, taken from bufstate or lqstate,
      //            does not grow during 1 s or more).
      // Returns:
      //    ret. code (also in this->res):
      //      -2 - common case failure, no changes,
      //      -1 - the local queue object is not created yet,
      //      0 - the local queue is currently empty (ipush == ipop),
      //      1 - the local queue is currently not empty (ipush > ipop).
      //    *pipush, *pipop - the current (volatile) push/pop message index in the local queue.
      //      Meaningful values are set only if lqstate returns >= 0, i.e. only when the queue exists.
      //        Otherwise, values are set to -1.
    _s_long lqstate(bool b_receiver, _s_ll* pipush = 0, _s_ll* pipop = 0, _s_ll iend_sender = -2, double dtms_end = 0) const __bmdx_noex
    {
      if (pipush) { *pipush = -1; } if (pipop) { *pipop = -1; }
      if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; }
      if (dtms_end < 0 || iend_sender <= -3) { res = -2; return -2; }
      if (!_rq) { try { _rq = _shmqueue_ctxx_impl::mqq()->rqueue(name, b_receiver ? 1 : 0, _nbdflt); } catch (...) {} }
      if (!_rq) { res = -1; return -1; }
      if (_rq->buf.b_side1() != b_receiver) { res = -2; return -2; }
      vnnqueue_t<cref_t<t_stringref> >& lq = _rq->msgs;
      _s_ll ipu = lq.ipush(); const _s_ll ipo = lq.ipop(); if (pipush) { *pipush = ipu; } if (pipop) { *pipop = ipo; }
      if (iend_sender >= -1 && !b_receiver)
      {
        critsec_t<shmqueue_ctx> __lock(_flags & 1 ? 10 : -1, -1, &_rq->csd); if (sizeof(__lock)) {}
        ipu = lq.ipush(); if (pipush) { *pipush = ipu; } // avoiding concurrency with msend
        _rq->mprg_client.iend = iend_sender >= 0 ? bmdx_minmax::myllmin(iend_sender, ipu) : ipu;
        _rq->mprg_client.tms = bmdx::clock_ms() + dtms_end;
        _rq->_mprg_ver += 1;
      }
      res = ipu > ipo ? 1 : 0; return res;
    }



      // Returns IPC buffer state, and optionally ensures the queue creation and enabling/disabling.
      // rqtype:
      //    0 - return the sender-side queue state, but do not autocreate or modify the queue,
      //    1 - return the receiver-side queue state, but do not autocreate or modify the queue,
      //    2 - if the sender-side queue does not exist, autocreate it,
      //    3 - if the receiver-side queue does not exist, autocreate it,
      //    NOTE "Autocreate" means:
      //      if the shared object does not exist, the receiver queue will perform its complete initialization,
      //      while the sender queue will only try to connect to the existing shared object.
      //    4, 5 - if the (sender, receiver)-side queue already exists, request queue enabling.
      //        NOTE By default, when the queue is first created/connected to, it's enabled.
      //        WARNING Queue enabling is done by internal thread and does not effect immediately.
      //    6, 7 - if the (sender, receiver)-side queue already exists, request queue disabling.
      //        The shared object will be closed, and after that, can be opened/used by another process
      //        or another binary module of the current process.
      //        WARNING Queue disabling is done by internal thread and does not effect immediately.
      //    NOTE rqtype's 4..7 are satisfied by internal thread after some delay,
      //      when all necessary conditions are met.
      //      When the queue is factually disabled, its bufstate becomes 4.
      //      After it's reenabled, its bufstate becomes 0, and grows to 3, as in usual initialization sequence.
      // Returns:
      //    ret. code (also in this->res):
      //      0, 1 - during shared object initialization by the receiver side.
      //      2 - for receiver: the last  initialization attempt has failed (maybe retry once more);
      //        for sender: failed to connect to shared memory (probably, the receiver, which is responsible for creation, does not yet exist).
      //      3 - initialized successfully (normal work).
      //      4 - disabled (see enrq below).
      //      5 (sender only) - the buffer is initialized successfully, but the receiver process is not active for too long, i.e. it may currently not exist.
      //      -1 - the local queue object is not created yet (use rqtype 2 or 3 to make it).
      //      -2 - common case failure, no changes.
      //      -3 - shared memory is locked by some other side.
      //    *pipush, *pipop - the current (volatile) push/pop char index in the shared memory queue.
      //      Meaningful values are set only if bufstate returns 3 (i.e. only when the queue is working normally).
      //        Otherwise, values are set to -1.
      //      Hint: in application-specific context, along with regular sending some messages,
      //        *pipop may be used (in sender process) to check if the receiving process is alive and actively consumes messages.
      //    enrq:
    _s_long bufstate(_s_long rqtype, _s_ll* pipush = 0, _s_ll* pipop = 0) const __bmdx_noex
    {
      if (pipush) { *pipush = -1; } if (pipop) { *pipop = -1; }
      if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; }
      const _s_long enrq = rqtype == 4 || rqtype == 5 ? 1 : (rqtype == 6 || rqtype == 7 ? 0 : -1);
      if (enrq >= 0) { rqtype &= 1; }
      if (!_rq) { try { _rq = _shmqueue_ctxx_impl::mqq()->rqueue(name, rqtype, _nbdflt); } catch (...) {} }
      if (!_rq) { res = -1; return -1; }
      if ((rqtype >= 0 && rqtype <= 3) && _rq->buf.b_side1() != !!(rqtype & 1)) { res = -2; return -2; }
      const _s_long st = _rq->bufstate;
      const bool b_rcv_act_all = _rq->buf.b_side1() || _rq->sndr_rcv_act >= 1;
      if (st == 3 && (pipush || pipop)) { try { rfifo_nbl11& b = _rq->buf->ringbuf; if (pipush) { *pipush = b.ipush(); } if (pipop) { *pipop = b.ipop(); } } catch (...) { res = -2; return -2; } }
      if (enrq == 0 || enrq == 1) { _rq->b_enrq = enrq == 1; }
      _s_long stm = st & 0xff;
      if (stm != 2) { if (stm == 3 && !b_rcv_act_all) { stm = 5; } res = stm; return res; }
      stm = st >> 8;
      if (_rq->buf.b_side1())
      {
        if (stm <= -4 || stm >= -1) { stm = -2; }
          else if (stm == -2) { stm = 2; }
      }
      else
      {
        if (stm == -4) { stm = 1; } // the memory exists, but not accessible yet: a) pending initialization by other side, b) pending memory handle transfer from other side
          else if (stm < -4 || stm >= -1) { stm = -2; }
          else if (stm == -2) { stm = 2; }
      }
      res = stm; return res;
    }



      // Send a message (put into local queue, for automatic sending via shared memory buffer by dedicated thread).
      // msg:
      //    any kind of reference to message string.
      //    Efficiency hints:
      //          a) msg and/or prefix may contain strings as hidden aux. objects, and refer to them from their main object
      //            (t_stringref, which is arrayref_t<char>).
      //            By default, creating such object (e.g. see make_rba()) makes a copy of original data, which costs
      //            memory allocation and bytes copying.
      //          b) The client may choose to create and handle data object at once as embedded aux. object of cref_t<t_stringref>.
      //            This is the most efficient way.
      //          c) msg and/or prefix may refer to any data in any storage.
      //            In this case, the storage may not be destroyed at least until the message
      //              has factually been sent via shared memory by internal delivering thread.
      //            The client may ensure that in one the following ways:
      //              1) after calling msend, wait for this->navl() == 0, and then release the data.
      //                This method is not recommended, because it becomes too complex when the client needs to queue
      //                multiple messages without waiting for any of them being factually sent.
      //              2) embed application-specific weak data pointer into msg or prefix.
      //                (Weak pointer reference count will tell when the data is not used anymore by delivering thread.)
      //              3) if data objects are swappable: construct cref_t<t_stringref> with hidden aux. object of the same type as data,
      //                but is empty, and then move (or swap) the data object into aux. object.
      //          d) static constant strings may be sent without any allocations. See make_rrba().
      // timeout_ms:
      //    <0 - block until shared memory buffer creation by the receiving side, or till an error.
      //    0 - try sending once, and return the result immediately.
      //    >0 - same as timeout_ms < 0, but returns 0 when timeout occurred.
      //      or any message is received, or error occurs.
      // prefix: if non-null object is given, its contained string is prepended to msg.
      //    The resulting single string, sent via shared queue, will be exact concatention of (prefix, msg).
      //    Only the client on the receiving side is responsible for recognizing prefixes.
      //    (shmqueue_s does not interpret data in prefix or msg in any special way.)
      //    NOTE prefix should be used to avoid extra allocations and data copying before passing through shared memory,
      //      when main message is very long (e.g. 100 MB), but must be prepended with some metadata,
      //      which helps the recipient to interpret it correctly.
      //      Instead of 3 copies,
      //          1) made by client, joining the prefix and main message before passing to msend,
      //          2) made by delivering thread, copying data into shared memory,
      //          3) made by receiving thread, copying data from shared memory into local memory,
      //      only 2 copies are made (2, 3), because prefix and msg are copied sequentially
      //        into shared memory, directly by the delivering thread.
      //        This costs only one dynamic allocation (3).
      //        If the recipient needs to pass the received data elsewhere, without prefix, it can forcefully set
      //        t_stringref's pd(), n() to main message factual data offset and length in the received string.
      //        (The prefix remains invisible part of the received object, but no extra memory allocations are done,
      //        as it would be in case of copying main message into another local container.)
      //  *pipush_last - push index for the message in the local queue.
      //      Purpose: for later comparison with *pipush from lqstate(), to figure out if the message
      //        has been already put (in whole) into shared memory.
      //      *pipush_last is set to meaningful value only if message is sent (locally pushed) successfully,
      //          i.e. if msend returns 1 or 2.
      //        Otherwise, *pipush_last = -1.
      // Returns (also in this->res):
      //    2 - the message successfully pushed into the output queue.
      //      (The receiving side exists, shared memory buffer is available. The message will be sent by servicing thread.)
      //    1 (only on timeout_ms == 0 && (flags() & 2) != 0, which is non-default) - the message is queued,
      //        but shared memory buffer currently does not exist, or not initialized by the receiving side.
      //        See also flags().
      //    0 (only on timeout_ms >= 0) - failure: timeout during shared memory initialization
      //        by the receiving side (the program is not run, or more time needed?).
      //    -1 - attempt to push message into the input queue.
      //    -2 - failure (memory allocation, incompatibility, message too long, the queue is disabled, etc.).
      //    -3 - shared memory is locked by some other side.
      //      NOTE When locked state is detected, msend does not wait for timeout and returns immediately.
      //    -4 - shared memory exists, but not ready yet (during initialization by the receiver side).
    _s_long msend(cref_t<t_stringref> msg, double timeout_ms = 0, cref_t<t_stringref> prefix = cref_t<t_stringref>(), _s_ll* pipush_last = 0) const __bmdx_noex
    {
      const double t0 = clock_ms();
      if (pipush_last) { *pipush_last = -1; }
      if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; }
      if (!msg) { res = -2; return -2; }
      if (msg->n() > __bmdx_shmfifo_msg_nbytes_max) { res = -2; return -2; }
      if (!_rq) { try { _rq = _shmqueue_ctxx_impl::mqq()->rqueue(name, 2, _nbdflt); } catch (...) {} }
      if (!_rq) { res = -2; return -2; }
      if (_rq->buf.b_side1() != false) { res = -1; return -1; } // not expected to occur
      while (true)
      {
        _s_long bsm = _rq->bufstate;
        if ((bsm & 0xff) == 2)
        {
          _s_long st2 = bsm >> 8;
          if (!(st2 >= -4 && st2 <= -1)) { st2 = -2; }
          if (st2 == -1) { res = -2; return -2; }
          if (st2 == -3) { res = -3; return -3; }
          bsm = 0;
        }
        bsm &= 0xff; if (bsm >= 4) { res = -2; return -2; }
        bool b_optim = !!(_flags & 2);
        if ((bsm == 3 && (_rq->sndr_rcv_act >= 1 || b_optim)) || (bsm <= 1 && timeout_ms == 0 && b_optim))
        {
          critsec_t<shmqueue_ctx> __lock(_flags & 1 ? 10 : -1, -1, &_rq->csd); if (sizeof(__lock)) {}
          vnnqueue_t<cref_t<t_stringref> >& lq = _rq->msgs;
          if (prefix) { res = lq.push_nf(new_pack3(prefix, msg), 3); }
            else { res =  lq.push_1(msg); }
          if (res != 1) { res = -2; return -2; }
          if (pipush_last) { *pipush_last = lq.ipush() - 1; }
          res = bsm == 3 ? 2 : 1; return res;
        }
        if (timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { res = 0; return 0; }
        sleep_mcs(_idle_t_mcs);
        if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; } // the application may be exiting during waiting
      }
    }

      // Convenience function.
      //  Sends a copy of the given string(s), through msend(cref_t...).
    _s_long msend(t_stringref msg, double timeout_ms = 0) const __bmdx_noex    { return msend(_bmdx_shm::make_rba(msg, true), timeout_ms); }
    _s_long msend(t_stringref msg, double timeout_ms, t_stringref prefix) const __bmdx_noex    { return msend(_bmdx_shm::make_rba(msg, true), timeout_ms, _bmdx_shm::make_rba(prefix, true)); }




      // Convenience function for msend.
      //    Creates a byte array from msg.
      //    NOTE Implementation uses calloc.
      //  b_byval:
      //    true (dflt.) - the returned cref_t strongly references a by value copy of msg.
      //    false:
      //      1. The returned array points the given data itself (msg->pd()).
      //      2. The data must exist until the last cref_t, pointing to that data, is destroyed. (Responsibility of the client.)
      //      NOTE Even with b_byval false, the function makes small dynamic allocation, i.e., theoretically may fail.
      //  nbadd > 0 (used only on b_byval == true): adds hidden (not accounted in t_stringref::n()) zero bytes
      //      after the copy of msg data. E.g. nbadd = 1 makes msg copy conventional C string.
      //  Returns:
      //    on success: valid non-empty cref_t<t_stringref>,
      //    on failure: cref_t<t_stringref>().
      //
    static cref_t<t_stringref> make_rba(t_stringref msg, bool b_byval = true, _s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba(msg, b_byval, nbadd); }
      //
      // Convenience function for msend.
      //    Creates a byte array as concatenation of copies of the given parts.
      //    NOTE Implementation uses calloc.
      //  nbadd > 0 (used only on b_byval == true): adds hidden (not accounted in t_stringref::n()) zero bytes
      //      after the copy of msg data. E.g. nbadd = 1 makes msg copy conventional C string.
      //  Returns:
      //    on success: valid non-empty cref_t<t_stringref> with contents equal to part1 + part2 + part3,
      //    on failure: cref_t<t_stringref>().
      //
    static cref_t<t_stringref> make_rba_mp(t_stringref part1, t_stringref part2, t_stringref part3 = t_stringref(), _s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba_mp(part1, part2, part3, nbadd); }
      //
      // Convenience function for msend.
      //    Creates zero-initialized byte array of length nb + max(nbadd, 0).
      //    NOTE Implementation uses calloc.
      //  Returns:
      //    on success: valid non-empty cref_t<t_stringref>,
      //    on failure: cref_t<t_stringref>().
      //
    static cref_t<t_stringref> make_rba_z(_s_ll nb, _s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba_z(nb, nbadd); }
      //
      // Convenience function for msend.
      //  Creates cref_t, pointing to the given arrayref_t object (msg).
      //  No dynamic allocations are made.
      //  This function is the most efficient way of creating references to constant strings.
      // Returns:
      //  valid non-empty cref_t<t_stringref>.
      //    The function does not fail. It does no dynamic allocations.
      // Example.
      //    At global or function scope:
      //      static arrayref_t<char> kw("keyword");
      //    usage:
      //      shmqueue_s q("shared queue name");
      //      q.msend(shmqueue_s::make_rrba(kw));
      //
    static cref_t<t_stringref> make_rrba(t_stringref& msg) __bmdx_noex    { cref_t<t_stringref> rv; rv.assign(msg, false, 1); return rv; }




      // Get next message (one already received by dedicated thread in full from shared memory buffer).
      //    The result is put into this->d. (If this occurs, old contents of this->d are released even if not empty.)
      // timeout_ms:
      //    <0 - block until any message is received or error occurs.
      //    0 - check once (no sleep), and return a message if available.
      //    >0 - while the queue is empty, check with regular intervals, until timeout occurs,
      //      or any message is received, or error occurs.
      // b_do_pop:
      //      true: pop message (remove from the queue).
      //      false: do not pop (i.e. peek only).
      // Returns (also in this->res):
      //    1 - the message is available (popped or peeked as specified in mode).
      //      this->d contains valid object.
      //      NOTE this->d->pd() points to this->d->n() bytes of message, plus 1 zero byte appended (may be used as C string).
      //    0 - no message in the queue (in case if timeout_ms > 0 was specified, then the time has passed already)
      //      this->d is cleared.
      //    -1 - attempt to pop message from the output queue.
      //    -2 - failure (memory allocation, incompatibility, the queue is disabled, etc.).
      //    -3 - shared memory is locked by some other side.
      //      NOTE When locked state is detected, mget does not wait for timeout and returns immediately.
      //  On any result < 0, this->d is not modified.
    _s_long mget(double timeout_ms = 0, bool b_do_pop = true) __bmdx_noex
    {
      const double t0 = clock_ms();
      if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; }
      if (!_rq) { try { _rq = _shmqueue_ctxx_impl::mqq()->rqueue(name, 3, _nbdflt); } catch (...) {} }
      if (!_rq) { res = -2; return -2; }
      if (_rq->buf.b_side1() != true) { res = -1; return -1; } // not expected to occur
      while (true)
      {
        _s_long bsm = _rq->bufstate; if ((bsm & 0xff) == 2) { bsm >>= 8; if (!(bsm >= -3 && bsm <= -1)) { bsm = -2; } res = bsm; return res; }
        bsm &= 0xff; if (bsm >= 4) { res = -2; return -2; }
        if (_rq->msgs.navl() > 0)
        {
          critsec_t<shmqueue_ctx> __lock(_flags & 1 ? 10 : -1, -1, &_rq->csd); if (sizeof(__lock)) {}
          while (!_rq->msgs.front()) { _rq->msgs.pop_1(); }
          this->d = _rq->msgs.front();
          if (b_do_pop) { _rq->msgs.pop_1(); }
          res = 1; return 1;
        }
        if (timeout_ms == 0 || (timeout_ms > 0 && clock_ms() - t0 >= timeout_ms)) { if (this->d) { this->d.clear(); } res = 0; return 0; }
        sleep_mcs(_idle_t_mcs);
        if (!_shmqueue_ctxx_impl::_th_enable()) { res = -2; return -2; } // the application may be exiting during waiting
      }
    }

    struct exc_mget_str : std::exception { const char* what() const __bmdx_noex { return "shmqueue_s::_mget_str"; } };

      // Convenience function.
      //  Gets the next message as std::string.
      //    a) If this->d is not empty, returns its copy as std::string. At the same time, this->d is cleared.
      //    b) If this->d is empty, gets the next message into this->d. If a message has been received: see (a).
      //    c) If this->d is empty, and no next message available, returns std::string().
      //    d) On failure (e.g. allocation), an exception is generated.
      //      In this case, this->d is not cleared. It may still contain a message,
      //      which will be returned on the next call to mget_str (or may be read directly by the client).
      // Returns in this->res:
      //    a) 1 if this->d already contained a message.
      //    b) the result of mget() call (i.e. 1 if a new message has been popped and put into this->d, 0 if no new message, <0 on error).
    std::string mget_str(long timeout_ms = 0)
    {
      struct __local_des { shmqueue_s* p; __local_des(shmqueue_s& x_) : p(&x_) {} ~__local_des() { if (p) { p->d.clear(); } } };
      if (this->d) { res = 1; }
      else
      {
        mget(timeout_ms);
        if (res < 0) { throw exc_mget_str(); }
        if (res == 0) { return std::string(); }
      }
      __local_des des(*this);
      try { return std::string(this->d->pd(), size_t(this->d->n())); } catch (...) { des.p = 0; }
      throw exc_mget_str();
    }
  };




} // end namespace _bmdx_shm::_api
} using namespace _bmdx_shm::_api; } // end namespace bmdx_shm

namespace bmdx
{
  struct file_io::_stra_rba : _stra_base { typedef cref_t<arrayref_t<char> > t_rba; virtual bool _resize(void* ps, _s_ll n0 __bmdx_noarg) const __bmdx_noex { t_rba& x = *(t_rba*)ps; if (n0 < 0) { x.clear(); return true; } x = ::bmdx_shm::_bmdx_shm::make_rba_z(n0, 1); return !!x; } virtual char* _pd(void* ps __bmdx_noarg) const __bmdx_noex { return (*(t_rba*)ps)->pd(); } };
  int file_io::load_bytes(const char* fnp, cref_t<arrayref_t<char> >& dest __bmdx_noargt) __bmdx_noex
  {
    cref_t<arrayref_t<char> > retval;
    int res = _load_bytes(fnp, _stra_rba(), &retval);
    bmdx_str::words::swap_bytes(retval, dest);
    return res;
  }
}

#undef _s_long
#undef _s_ll
#undef _u_ll

#endif // bmdx_cpiomt_H
