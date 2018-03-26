// BMDX library 1.1 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2018-03-26
// See bmdx_main.h for description.

// LIBRARY CONFIGURATION HEADER
// FOR MANUAL EDITING

#ifndef bmdx_config_H
#define bmdx_config_H



  // Enables (1) DLL/shared libraries loader functionality. 0 to disable.
  //  NOTE In Linux, compiling may require additional library (-ldl).
  //  NOTE Even if the part is disabled, the library can operate on an existing handle
  //    created in any module where the part is enabled.
#ifndef bmdx_part_dllmgmt
  #define bmdx_part_dllmgmt 1
#endif

  // Enables (1) message dispatcher functionality. 0 to disable.
#ifndef bmdx_part_dispatcher_mt
  #define bmdx_part_dispatcher_mt 1
#endif


  // Cross-module compatibility for strings as vecm element.
  // Define bmdx_cmti_stdstring_mode as OR-ed flags:
  //    1 - make std::string crossmodule (0 - make local to binary module),
  //    2 - make std::wstring crossmodule (0 - make local to binary module),
  //    4 - ignore flags 1, 2 if compiler type is unknown, otherwise make strings crossmodule per compiler type.
  //    not defined - compatibility is autodetected for compiler type (see near __bmdx_cmti_stdstring_AUTO).
#ifndef bmdx_cmti_stdstring_mode
//  #define bmdx_cmti_stdstring_mode 7
#endif


  // Flags for controlling for .dll (.so) load mode.
  //  Currently: Linux only:
  //    flags & 3: controls RTLD_DEEPBIND use in dlopen
  //          (0 - disable, 1 - enable if available, 2 - enable for g++ only).
  //  See also unity::mod().
#ifndef bmdx_mod_load_def_flags
  #define bmdx_mod_load_def_flags 0
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
//  #pragma warning(disable:4800)
//  #pragma warning(disable:4290)
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


namespace yk_c { struct bytes; }
namespace
{
  struct nothing {}; template<bool cond, class _ = nothing> struct assert { typedef bool t_false; };  template<class _> struct assert<true, _> { enum { result = true }; typedef bool t_true; };
  template<class L, class _> struct _check_isdef { enum { b = false }; }; template<class L> struct _check_isdef<L, typename L::_t_is_defined> { enum { b = true }; };

    // ERROR: vecm_hashx.h must not be included before this header.
    //  Include bmdx_config.h before or instead of any inclusion of vecm_hashx.h.
  typedef assert<_check_isdef<yk_c::bytes, bool>::b>::t_false __check1;
}



  // Required
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
#include "vecm_hashx.h" // must be included after bmdx_cpiomt.h to use the above lock

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
