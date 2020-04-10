// BMDX library 1.3 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2020-04-10
// See bmdx_main.h for details.
//
// Contacts: bmdx-dev [at] mail [dot] ru, z7d9 [at] yahoo [dot] com
// Project website: hashx.dp.ua


// LIBRARY CONFIGURATION HEADER
// FOR MANUAL EDITING

#ifndef bmdx_config_H
#define bmdx_config_H



  // 1: enables DLL/shared libraries loader functionality.
  // 0: disable.
  //  NOTE Even if disabled, only unity::mod() (i.e. loader) becomes non-functional, but
  //    mod_handle objects as such continue to work properly, if they have been created
  //    in some other binary module, having bmdx_part_dllmgmt 1.
  //  NOTE In Linux, compiling may require additional library (-ldl).
  //  NOTE Even if the part is disabled, the library can operate on an existing handle
  //    created in any module where the part is enabled.
#ifndef bmdx_part_dllmgmt
  #define bmdx_part_dllmgmt 1
#endif

  // Default flags for controlling for .dll (.so) load mode.
  //  Currently: Linux only:
  //    flags & 3: controls RTLD_DEEPBIND use in dlopen
  //          (0 - disable, 1 - enable if available, 2 - enable for g++ only).
  //  See also unity::mod() (flags arg.).
#ifndef bmdx_mod_load_def_flags
  #define bmdx_mod_load_def_flags 0
#endif

  // 1: Always expose symbol __bmdx_ls_modsm, referring to unity_common::ls_modsm(),
  //    which returns static methods of bmdx_main.
  // 0: Do not expose __bmdx_ls_modsm unless bmdx_part_dllmgmt is 1.
  // NOTE Do not define bmdx_expose_ls_modsm 0 if common objects storage (PCOS)
  //      is used by this module in any way.
#ifndef bmdx_expose_ls_modsm
  #define bmdx_expose_ls_modsm 1
#endif

  // bmdx_part_dispatcher_mt 1 enables message dispatcher functionality.
  //  0 to disable.
#ifndef bmdx_part_dispatcher_mt
  #define bmdx_part_dispatcher_mt 1
#endif

  // Enables unity::operator(), which is shortcut to map(), map_append(), hash(), hash_set().
#ifndef bmdx_part_unity_oppar_assoc
  #define bmdx_part_unity_oppar_assoc 1
#endif

  // Cross-module compatibility for strings. OR-ed flags:
  //    1 - treat std::string as cross-module (0 - treat as safe to use only in its own binary module),
  //    2 - treat std::wstring as cross-module (0 - treat as safe to use only in its own binary module),
  //    4 - a) if compiler type is unknown, ignore flags 1, 2 and treat strings as safe to use only in their own binary module,
  //        b) for known compilers, treat strings as compatible across the set of binary modules, created with the same compiler type.
  // Hints:
  //  1. (the best choice) If bmdx_cmti_stdstring_mode is not defined,
  //    strings compatibility is set automatically for max. efficiency and safety,
  //    i.e. enabled only for small subset of known compiler types.
  //    This is the best choice when target compilers for the application are generally unknown.
  //  2. If bmdx_cmti_stdstring_mode is defined to 0,
  //    all strings, in all compilers, are treated as incompatible when crossing binary module boundary.
  //    This enables automatic string copying and special access functions use everywhere in BMDX library.
  //  3. Set flags 1, 2 together with flag 4 if only if you are sure that EACH of your target compilers
  //    produces strings, which can be safely interchanged between modules, compiled with the SAME compiler type and version.
  //  4. Set flags 1, 2 and unset flag 4 if only if you are sure that ALL your target compilers are able to interchange
  //    their std string (or wstring) objects safely.
  //    NOTE This mode should be used only for making a set DLLs with single compiler type and exactly same version,
  //      provided that strings compatibility has been manually tested.
#ifndef bmdx_cmti_stdstring_mode
//  #define bmdx_cmti_stdstring_mode 7
#endif





  // Uncomment to disable excessive compiler warnings in your program.
#if defined(__clang__)
//  #pragma clang diagnostic ignored "-Wunknown-pragmas"
//  #pragma clang diagnostic ignored "-Wunused-parameter"
//  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
#endif
#if defined(__GNUC__) && !defined(__clang__)
//  #pragma GCC diagnostic ignored "-Wpragmas"
//  #pragma GCC diagnostic ignored "-Wunused-parameter"
//  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
//  #pragma GCC diagnostic ignored "-Wnonnull-compare"
//  #pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#ifdef _MSC_VER
//  #pragma warning(disable:4100)
//  #pragma warning(disable:4800)
//  #pragma warning(disable:4355)
//  #pragma warning(disable:4345)
//  #pragma warning(disable:4996)
//  #undef _CRT_SECURE_NO_WARNINGS
//  #define _CRT_SECURE_NO_WARNINGS
#endif





  // UTF-8 may be forced in wsToBs, bsToWs if libc does not provide symmetric wctomb, wcrtomb / mbtowc, mbrtowc.
  //  (Also, it's switched on automatically if these fns. fail to convert ASCII alphanum. characters.)
#ifdef __ANDROID__
  #define bmdx_wsbs_force_utf8 1
  #define bmdx_bsws_force_utf8 1
#endif





  // Required
namespace yk_c { struct bytes; }
namespace __bmdx_config
{
  struct nothing {}; template<bool cond, class _ = nothing> struct assert { typedef bool t_false; };  template<class _> struct assert<true, _> { enum { result = true }; typedef bool t_true; };
  template<class L, class _> struct _check_isdef { enum { b = false }; }; template<class L> struct _check_isdef<L, typename L::_t_is_defined> { enum { b = true }; };

  typedef assert<_check_isdef<yk_c::bytes, bool>::b>::t_false __check1; // ERROR Include bmdx_config.h before or instead of any inclusion of vecm_hashx.h.
}
#include "bmdx_cpiomt.h"
namespace yk_c
{
  template<class T, class Aux> struct lock_impl_t : bmdx::critsec_t<T>
  {
    typedef T t_target; typedef bmdx::critsec_t<T> t_impl; struct exc_lock {};
    lock_impl_t() : t_impl(10, -1) { if (!this->t_impl::is_locked()) { throw exc_lock(); } }
  };
}
#undef yk_c_vecm_hashx_H
#include "vecm_hashx.h" // must be included after bmdx_cpiomt.h to use lock_impl_t (see above)
#if !defined(__bmdx_cpiomt_threadctl_spec)
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

    template<s_long nmax> struct hashx_common::kf_basic<bmdx_str::flstr_t<nmax> > : _skf_pchars<char>
    {
      typedef bmdx_str::flstr_t<nmax> t_k;
      inline void _fls_cnew(t_k* p, const t_k& x) const { new (p) t_k(x); }
      inline s_long _fls_hash(const t_k& x) const { return this->Fhash(x.pd(), x.n()); }
      inline bool _fls_is_eq(const t_k& x1, const t_k& x2) const { return x1 == x2; }
      template<class K2> inline void cnew(t_k* p, const K2& x) const { this->_fls_cnew(p, x); }
      template<class K2> inline s_long hash(const K2& x) const { return this->_fls_hash(x); }
      template<class K2> inline bool is_eq(const t_k& x1, const K2& x2) const { return this->_fls_is_eq(x1, x2); }
      inline bool is_eq(const t_k& x1, const char* x2) const { if (!x2) { return false; } for (s_long i = 0; i < x1.n(); ++i) { char c = *x2++; if (!c || c != x1[i]) { return false; } } return !*x2; }
    };
  }
#endif





#ifndef __bmdx_cmti_stdstring_DEFINED
#define __bmdx_cmti_stdstring_DEFINED
  namespace yk_c
  {
    #undef __bmdx_cmti_stdstring_AUTO
    #if defined(bmdx_cmti_stdstring_mode) && !(bmdx_cmti_stdstring_mode & 4)
          // Common index for all compilers.
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 0, t  = 1; };
        #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
    #elif defined(__MINGW32_MAJOR_VERSION)
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 1, t  = (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __MINGW32_MAJOR_VERSION * 37 ^ __MINGW32_MINOR_VERSION * 4); };
        #if bmdx_cmti_stdstring_mode & 4
          #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
        #else
          #if _GLIBCXX_FULLY_DYNAMIC_STRING != 0 // otherwise string resize is not compatible
            #define __bmdx_cmti_stdstring_AUTO 3
          #endif
        #endif
    #elif defined(__GNUC__) && !defined(__MINGW32_MAJOR_VERSION) && !defined(__clang__)
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 2, t  = (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __GNUC__ * 37 ^ __GNUC_MINOR__ * 4); };
        #if bmdx_cmti_stdstring_mode & 4
          #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
        #else
          #if _GLIBCXX_FULLY_DYNAMIC_STRING != 0 // otherwise string resize is not compatible
            #define __bmdx_cmti_stdstring_AUTO 3
          #endif
        #endif
    #elif defined(_MSC_VER)  && !(defined(__ICC) || defined(__INTEL_COMPILER) || defined(__clang__))
          // MSVC can share strings between modules only when CRT is linked dynamically
          // (code generation /MD or /MDd, which automatically defines _DLL).
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 3, t  = 1 + (1023 & (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ (_MSC_VER * 3 / 11 ^ _MSC_VER * 7 % 19))); };
        #if bmdx_cmti_stdstring_mode & 4
          #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
        #else
          #if defined(_DLL)
            #define __bmdx_cmti_stdstring_AUTO 3
          #endif
        #endif
    #elif defined(__clang__)  && !defined(__BORLANDC__)
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 4, t  = (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __clang_major__ * 37 ^ __clang_minor__ * 4); };
        #if bmdx_cmti_stdstring_mode & 4
          #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
        #else
          #define __bmdx_cmti_stdstring_AUTO 3
        #endif
    #elif defined(__SUNPRO_CC)
        struct __bmdx_cmti_stdstring_cmptype { static const int y = 2015, m = 6, h = 5, t  = 1 + (1023 & (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ (__SUNPRO_CC * 3 / 11 ^ __SUNPRO_CC * 7 / 13 ^ __SUNPRO_CC * 7 % 19))); };
        #if bmdx_cmti_stdstring_mode & 4
          #define __bmdx_cmti_stdstring_AUTO (bmdx_cmti_stdstring_mode & 3)
        #else
          #define __bmdx_cmti_stdstring_AUTO 3
        #endif
    #endif

    #if __bmdx_cmti_stdstring_AUTO & 1
      template<> struct bytes::type_index_t<std::string> : cmti_base_t<
          std::string,
          __bmdx_cmti_stdstring_cmptype::y,
          __bmdx_cmti_stdstring_cmptype::m,
          3,
          __bmdx_cmti_stdstring_cmptype::h,
          __bmdx_cmti_stdstring_cmptype::t
      > {};
        namespace { bytes::type_index_t<std::string> __cmti_inst_string; }
    #endif
    #if __bmdx_cmti_stdstring_AUTO & 2
      template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<
          std::wstring,
          __bmdx_cmti_stdstring_cmptype::y,
          __bmdx_cmti_stdstring_cmptype::m,
          4,
          __bmdx_cmti_stdstring_cmptype::h,
          __bmdx_cmti_stdstring_cmptype::t
      > {};
          namespace { bytes::type_index_t<std::wstring> __cmti_inst_wstring; }
    #endif
  }
#endif


#endif
