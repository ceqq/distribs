// BMDX library 1.5 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
//  Polymorphic container for data and objects, message dispatcher, utilities.
// rev. 2023-07-14
//
// Contacts: bmdx-dev [at] mail [dot] ru, z7d9 [at] yahoo [dot] com
// Project website: hashx.dp.ua
//
// Copyright 2004-2023 Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the Software or the use or other dealings in the Software.
// https://opensource.org/licenses/MIT


// In bmdx_main.h, bmdx_main.cpp:
//
//    Polymorphic container for storing high-level data and objects and passing them between binary modules.
//      (bmdx::  struct unity)
//    Original readable text format for data trees, with encoding/decoding utility.
//      (bmdx::  struct paramline)
//    Asynchronous in- and interprocess message dispatcher.
//      (bmdx::  dispatcher_mt)
//
//    Filename and path utilities, file manipulation utilities.
//      (bmdx::  struct file_utils)
//    Getting executable name, path, command line arguments array, etc.
//      (bmdx::  cmd_myexe, cmd_string, cmd_array)
//    String utilities and character set conversions.
//      (bmdx::  wsToBs, bsToWs, split, trim, replace etc.)
//    Simple date class.
//      (bmdx::  struct _unitydate)
//

#ifndef bmdx_main_H
#define bmdx_main_H
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
  #pragma clang diagnostic ignored "-Wunused-function"
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wdeprecated"
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  #pragma GCC diagnostic ignored "-Wdeprecated-copy"
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
  #pragma GCC diagnostic ignored "-Wstrict-overflow"
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
  #pragma GCC diagnostic ignored "-Wclass-memaccess"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wunused-function"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
  #pragma GCC diagnostic ignored "-Wnonnull-compare"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  #pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4290)
  #pragma warning(disable:4100)
  #pragma warning(disable:4616)
  #pragma warning(disable:4355)
#endif

#include "bmdx_config.h"

#undef __BMDX_DLLEXPORT
#ifdef _bmdxpl_Wnds
  #define __BMDX_DLLEXPORT __declspec(dllexport)
#else
  #define __BMDX_DLLEXPORT
#endif

#include <vector>
#include <exception>
#include <typeinfo>

namespace bmdx
{
    // Implementation part.
  using yk_c::__vecm_tu_selector;
  using yk_c::meta;
  using yk_c::s_long;
  typedef yk_c::meta::s_ll s_ll;
  using yk_c::bytes;
  using yk_c::typer;
  using yk_c::vecm;
  using yk_c::hashx_common;
  using yk_c::hashx;
  using yk_c::vec2_t;
  using yk_c::ordhs_t;
  using yk_c::iterator_t;

  struct unity;
  struct unity_common
  {
    typedef meta::assert<meta::same_t<meta::s_long, bmdx_meta::s_long>::result>::t_true __check1;
    typedef meta::assert<meta::same_t<meta::s_ll, bmdx_meta::s_ll>::result>::t_true __check2;


    typedef void* (*__Psm)(s_long ind_method); // the function returning static function (class or interface method) pointer by given index
    typedef __Psm (*__Pipsm)(const char* interface_name); // the function returning static getter for getter of the given interface static methods

    static void _ls_psm_set(const char* piname, __Psm p) __bmdx_noex;

    enum Edef_itf_method
    {
      _defsm_pi = 0,
      sizeof_defsm = 10
    };


    template<class T, class TList> struct find_type { typedef typename meta::same_t<typename TList::t_cur, T, TList, find_type<T, typename TList::tt_next> >::t __t_res; enum { ind = __t_res::ind }; };
      template<class T> struct find_type<T, meta::nothing> { enum { ind = -1 }; };
    template< class F1=int, class F2=int, class F3=int, class F4=int, class F5=int, class F6=int, class F7=int, class F8=int, class F9=int, class F10=int, class F11=int, class F12=int, class F13=int, class F14=int, class F15=int, class F16=int, class F17=int, class F18=int, class F19=int, class F20=int, class F21=int, class F22=int, class F23=int, class F24=int, class F25=int, class F26=int, class F27=int, class F28=int, class F29=int, class F30=int, class F31=int, class F32=int, class F33=int, class F34=int, class F35=int, class F36=int, class F37=int, class F38=int, class F39=int, class F40=int, class F41=int, class F42=int, class F43=int, class F44=int, class F45=int, class F46=int, class F47=int, class F48=int, class F49=int,
        class F50=int, class F51=int, class F52=int, class F53=int, class F54=int, class F55=int, class F56=int, class F57=int, class F58=int, class F59=int, class F60=int, class F61=int, class F62=int, class F63=int, class F64=int, class F65=int, class F66=int, class F67=int, class F68=int, class F69=int, class F70=int, class F71=int, class F72=int, class F73=int, class F74=int, class F75=int, class F76=int, class F77=int, class F78=int, class F79=int, class F80=int, class F81=int, class F82=int, class F83=int, class F84=int, class F85=int, class F86=int, class F87=int, class F88=int, class F89=int, class F90=int, class F91=int, class F92=int, class F93=int, class F94=int, class F95=int, class F96=int, class F97=int, class F98=int, class F99=int,
        class F100=int, int __ind = sizeof_defsm, class _ = __vecm_tu_selector >
    struct fn_list
      : public fn_list < F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
          F100, int, __ind + 1 >
    {
      typedef fn_list < F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
          F100, int, __ind + 1 >
          tt_next;
      typedef F1 t_cur; enum { ind = __ind };
      static void Freg(void** psmt) { psmt[ind] = (void*)t_cur::F; tt_next::Freg(psmt); }
    };
    template< class F2, class F3, class F4, class F5, class F6, class F7, class F8, class F9, class F10, class F11, class F12, class F13, class F14, class F15, class F16, class F17, class F18, class F19, class F20, class F21, class F22, class F23, class F24, class F25, class F26, class F27, class F28, class F29, class F30, class F31, class F32, class F33, class F34, class F35, class F36, class F37, class F38, class F39, class F40, class F41, class F42, class F43, class F44, class F45, class F46, class F47, class F48, class F49,
        class F50, class F51, class F52, class F53, class F54, class F55, class F56, class F57, class F58, class F59, class F60, class F61, class F62, class F63, class F64, class F65, class F66, class F67, class F68, class F69, class F70, class F71, class F72, class F73, class F74, class F75, class F76, class F77, class F78, class F79, class F80, class F81, class F82, class F83, class F84, class F85, class F86, class F87, class F88, class F89, class F90, class F91, class F92, class F93, class F94, class F95, class F96, class F97, class F98, class F99,
        class F100, int __ind, class _ >
    struct fn_list
    <
      int, F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
      F100, __ind, _
    > { typedef meta::nothing tt_next; typedef meta::nothing t_cur; enum { ind = -1, fn_cnt = __ind }; static void Freg(void** psmt) { (void)psmt; } };


      // Static methods of the module.
    static void* ls_modsm(s_long ind);
    typedef unity_common::__Psm f_ls_modsm;
    static f_ls_modsm pls_modsm(); // returns real address of ls_modsm
    enum Emod_method
    {
        // unity_common::pls_modsm()(msm_obj_ipsm) --> __Pipsm --> call with (interface name) -->
        //    --> __Psm --> call with (method index) --> method helper pointer --> call with (interface pointer, method arguments).
        //  1. ls_modsm -- static methods table, associated with binary module.
        //  2. __Pipsm -- a pointer to _get_sm -- getter of getters of static methods of interface, by interface name.
        //  3. __Psm -- a pointer to __sm -- getter for interface methods call helpers.
        //  4. void* -- __sm(ind_method) -- a) interface method call helper b) internal implementation function, associated with interface.
        //      Method call helpers are enumerated statically in o_proxy struct specialization.
        //  5. Interface method call helper invokes the associated interface method,
        //    passing interface pointer + same arguments as the original interface function.
      msm_obj_ipsm, // _ls_psm_find
      _msm_obj_pi, // _ls_obj_pi
      _msm_obj_cpvoid, // _ls_obj_cpvoid
      _msm_obj_punity, // _ls_obj_punity
      _msm_cv_reg,
      msm_cv_clear,
      msm_cv_create,
      msm_cv_destroy,
      msm_cv_unity_new,
      msm_cv_delete,
      msm_cv_convert,
      msm_cv_assign,
      msm_wstring_new,
      msm_wstring_n,
      msm_wstring_p0,
      msm_wstring_wcs,
      msm_wstring_ti,
      msm_arr_el_get,
      msm_arr_el_set,
      msm_arr_el_append,
      msm_arr_el_insrem,
      msm_arr_sz_set,
      msm_arr_lb_u,
      msm_arr_ub_u,
      msm_arr_sz_u,
      msm_u_set_name,
      msm_u_clear,
      msm_ua_fill,
      msm_ua_ensure_ind,
      _msm_obj_prefo, // _ls_obj_prefo
      msm_obj2_itfslist_cr,
      msm_obj2_itfslist_del,
      msm_cv_regstg,
      msm_ver_map,
      msm_ver_hl,
      msm_rootldr,
      msm_rl_init_handle,
      msm_rl_des_handle,
      msm_rl_cp_handle,
      msm_rl_sym,
      msm_rl_rq,
      msm_pcos_cons,
      msm_pcos_cons_rl,
      msm_pcos_oppar,
      msm_pcos_setref,
      msm_pcos_setmv,
      msm_pcos_setcp,
      msm_pcos_clearstg,
      msm_pcos_makemv,
      msm_pcos_makecp,
      msm_pcos_makecref,
      msm_pcos_makecrefcp,
      msm_cv_recreate,
      msm_rl_init_by_pmsm,
      msm_pcos_checkref,
      msm_pl_encode,
      msm_mget_set_retvals,
      sizeof_msm
    };
    enum Eobject_method
    {
      osm_modsm,
      osm_pvoid,
      osm_prefo,
      osm_type_info, // obj_type_info
      osm_ref_info, // obj_ref_info
      osm_lock_create,
      osm_lock_destroy,
      osm_rc_create,
      osm_rc_incdec,
      osm_lock_state,
      sizeof_osm
    };
    enum Eitfs_method
    {
      itfssm_modsm,
      itfssm_new, // itfs_new
      itfssm_new_copy, // itfs_new_copy
      itfssm_delete, // itfs_delete
      itfssm_type_info, // itfs_type_info
      itfssm_xx_after_construct, // __xx_after_construct
      itfssm_xx_before_destroy, // __xx_before_destroy
      sizeof_itfssm
    };
  };




    //==  Supported types of values wrapped into unity objects.
  enum EUnityType
  {
    // Scalar types.
    // (utype() & utMaskScalar) makes one of these values.
    utEmpty,
    utInt,      // integer number, represented by long
    utFloat,    // f.p. number, represented by double
    utDate,     // date value, represented by double
    utChar,     // char
    utString,   // std::wstring
    utObject,   // object + descriptor
    utUnity,     // only in combination with utArray
    utMap, // ordered map { k, v }
    utHash, // hashlist { k, v }
    _ut_end,

      // Array flag.
    utArray = 0x20,

    _ut_0 = utEmpty | utArray, // no arrays of Empty
    utIntArray,
    utFloatArray,
    utDateArray,
    utCharArray,
    utStringArray,
    _ut_1, // no arrays of Object
    utUnityArray,


    utMaskNoArray=0x1f, // (utype() & utMaskScalar) yields the scalar type, without utArray flag

      // "return std vector" flag for unity::val.
    fvalVector = 0x100
  };




    //==  Exceptions, generated by the library functions.
  typedef bmdx_str::flstr_t<75> _fls75;
  struct _XUBase : std::exception
  {
    _fls75 s;
    const char* what() const __bmdx_noex { return s.c_str(); }
    _XUBase(const _fls75& exc_type, const _fls75& src, const _fls75& x1, const _fls75& x2, const _fls75& x3, const _fls75& x4, const _fls75& x5)
    {
      s = exc_type; s += '/'; s += src; s_long n = 0;
      if (x1.length()) { s += (n ? ", " : ": "); s += x1; ++n; }
      if (x2.length()) { s += (n ? ", " : ": "); s += x2; ++n; }
      if (x3.length()) { s += (n ? ", " : ": "); s += x3; ++n; }
      if (x4.length()) { s += (n ? ", " : ": "); s += x4; ++n; }
      if (x5.length()) { s += (n ? ", " : ": "); s += x5; ++n; }
    }
  };
  struct XUTypeMismatch : _XUBase { XUTypeMismatch(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "", const _fls75& x5 = "") : _XUBase("XUTM", src, x1.length() ? "T1=" + x1 : x1, x2.length() ? "T2=" + x2 : x2, x3, x4, x5) {} };
  struct XUConvFailed: _XUBase { XUConvFailed(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "", const _fls75& x5 = "") : _XUBase("XUCF", src, x1.length() ? "T1=" + x1 : x1, x2.length() ? "T2=" + x2 : x2, x3, x4, x5) {} };
    // NOTE XUExec may be used by client code to generate custom exceptions.
  struct XUExec: _XUBase { XUExec(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "", const _fls75& x5 = "") : _XUBase("XUE", src, x1, x2, x3, x4, x5) {} };




    //==  Value conversion modes.
  enum EConvStrength
  {
    csLazy,
    csNormal,
    csStrict,
    cs_mask = 7
  };




    //==  Char alias.
  typedef char _unitychar;




    //==  Date class.
  struct _unitydate
  {
    inline double f() const { return _jds; } // Julian date in seconds
    meta::s_ll f_sec() const; // Julian date in whole seconds (rounding takes into account floating point calc. precision)
    s_long jdn() const; // Julian day number

    inline void set(double x) { const double max = (meta::s_long_mp - 1) * 86400.; _jds = x < -max ? -max : (x > max ? max : x); }
    inline _unitydate() : _jds(0.) {}
    inline explicit _unitydate(double x) { set(x); }
      // y is not limited, d is limited to 1..31, others - to their standard range.
    _unitydate(s_long y, s_long m, s_long d, s_long h = 0, s_long min = 0, s_long s = 0);

    _unitydate& add_datetime(s_long dy, s_long dm, s_long dd, s_long dh = 0, s_long dmin = 0, s_long ds = 0);
    inline _unitydate& add_time(s_long dh, s_long dmin, s_long ds)        { return add_datetime(0, 0, 0, dh, dmin, ds); }
    inline _unitydate& add_seconds(double ss)        { set(_jds + ss); return *this; }
    inline _unitydate& round_s()        { set(double(f_sec())); return *this; } // rounds the date to whole number of seconds

    s_long d_year() const;
    s_long d_month() const;
    s_long d_day() const;
    s_long d_hour() const;
    s_long d_minute() const;
    s_long d_second() const;
    double d_ms() const; // milliseconds NOTE precision is about 0.05
    s_long d_day_of_week() const; // 1..7 (Mon..Sun)
    void d_get(s_long* py, s_long* pm, s_long* pd, s_long* ph, s_long* pmin, s_long* ps) const; // any ptr. may be 0

    bool operator==(const _unitydate& x) const { return _jds == x._jds; }
    bool operator!=(const _unitydate& x) const { return _jds != x._jds; }
    bool operator>=(const _unitydate& x) const { return _jds >= x._jds; }
    bool operator<=(const _unitydate& x) const { return _jds <= x._jds; }
    bool operator>(const _unitydate& x) const { return _jds > x._jds; }
    bool operator<(const _unitydate& x) const { return _jds < x._jds; }

    private: double _jds;
  };




    //==  Wrapper for static variables.
    //    Ensures exact 1:1 correspondence of a variable to its binary module instance,
    //    independent of binding ruleset used by compiler.
    // Important differences from C++ language static variables:
    //    1) Variable's storage initialization is not done or attempted automatically
    //      during static initialization of the module.
    //      The client must explicitly initialize the storage with help of init*() or rxai*().
    //      To find out if the storage needs initialization, the client should check b_noinit().
    //    2) On static deinitialization, by default, for all types except basic fundamental
    //      (like int, double, T*, see bytes::config0_t isKnown1 in vecm_hashx.h)
    //      their destructor is called. The storage keeps track of the variable being destroyed,
    //      and may automatically limit access to the object, as the client specifies with flags (see px(), rx()).
    //    3) Non-default destructor behavior can be statically set via vecm::spec<TA>::config::dmode.
    //      If 0, the destructor is not executed, and the variable, if initialized at all,
    //      remains accessible during whole static deinitialization of the module.
    //      In this case, the client is responsible for releasing any resources, associated with non-destroyed variable.
    //    4) The variable may be accessed, without additional declarations,
    //      from any place or multiple places in the program.
    //      Its identity is determined by 4 items:
    //          C (associated class),
    //          Fi (field index),
    //          Fs (field selector),
    //          binary module instance of the client function.
    //      NOTE function binding is subject to compiler used.
    //        With default compiler settings, on most systems,
    //          1) non-inline C functions,
    //          2) non-inline non-template class functions,
    //          3) functions in anonymous classes and namespaces
    //          are bound to their own binary module.
    //    5) Variable storage access may fail (end with C++ exception or returning error code)
    //      under number of conditions, for any type, though this is not normal or frequent situation.
  template<class TA, class C = void, int Fi = 0, class Fs = void, meta::s_ll Lcp_mcs = 50, s_long Lto_ms = -1, class _ = __vecm_tu_selector>
  struct svf_m_t
  {
    typedef typename vecm::specf<TA>::t_value t_value;
    typedef svf_m_t<t_value, C, Fi, Fs, Lcp_mcs, Lto_ms, void> t_svf;
    static const meta::s_ll cp_mcs = Lcp_mcs;
    static const s_long to_ms = Lto_ms;
    struct L : critsec_t<t_svf>
    {
      L() : critsec_t<t_svf>(cp_mcs, to_ms) {}
      bool b_valid() const { return cp_mcs < 0 || this->is_locked(); }
    };

  private:
    enum { nst = (sizeof(t_value) + sizeof(meta::s_ll) - 1) / sizeof(meta::s_ll) }; typedef meta::s_ll t_stg[nst];
    struct t_storage { t_stg stg; char init; };
      static t_storage _storage;
    struct t_deinit { ~t_deinit() { if (vecm::specf<TA>::dmode == 0) { return; } L __lock; if (__lock.b_valid()  && _storage.init == 1) { vecm::specf<TA>::Fdestroy_1(&_storage.stg[0]); _storage.init = 2; } } };
      static t_deinit _deinit;
      static t_storage* _pst_m;
    void _reg_stg() const __bmdx_noex { if (!_pst_m) { typedef void* (*PFregstg)(const char* tname, void* pstg_offer); PFregstg f = (PFregstg)unity_common::pls_modsm()(unity_common::msm_cv_regstg); if (f) { _pst_m = (t_storage*)f(typeid(t_svf).name(), &_storage); } } }
  public:

      // NOTE Set lock L if a series of checks and accesses must be made safely in multithreaded context.
    inline bool b_noinit() const { _reg_stg(); return !_pst_m || _pst_m->init == 0; } // still not initialized
    inline bool b_valid() const { _reg_stg(); return _pst_m && _pst_m->init == 1; } // initialized
    inline bool b_destroyed() const { _reg_stg(); return _pst_m && _pst_m->init == 2; } // already destroyed (dflt. during static deinitialization)

      // Return pointer to static value storage, common to all translation units in the same binary module instance.
      // flags:
      //    1 - return storage address even if the value is not yet initialized.
      //    2 - return storage address if the value is valid.
      //    4 - return storage address even if the value is already destroyed (during static deinit.).
      // Returned value:
      //    != 0 - if the value has been registered, and its state corresponds to flags.
      //    0 - failure, or storage state does not satisfy flags.
    inline t_value* px(s_long flags = 2) {  if (!_pst_m) { _reg_stg(); if (!_pst_m) { return 0; } } int qq = _pst_m->init; (void)qq; if (flags & (1 << _pst_m->init)) { return (t_value*)&_pst_m->stg[0]; } return 0; }
      // Returned value: same as px(), but throws exception if the pointer is 0.
    inline t_value& rx(s_long flags = 2)        { t_value* p = px(flags); if (p) { return *p; } if (!_pst_m) { throw XUExec("rx.1", typeid(t_svf).name()); } throw XUExec("rx.2", typeid(t_svf).name()); }

      // Initialization.
      //    init* may be called many times, it makes all checks and does initialization only once.
      // Returns:
      //    true if the variable is currently valid, false if not (already destroyed or failed to initialize).
    inline bool init0(bool no_exc)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init0.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init0.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1> inline bool init1(bool no_exc, const A1& x1)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init1.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init1.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1, class A2> inline bool init2(bool no_exc, const A1& x1, const A2& x2)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init2.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init2.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1, x2); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1, class A2, class A3> inline bool init3(bool no_exc, const A1& x1, const A2& x2, const A3& x3)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init3.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init3.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1, x2, x3); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1, class A2, class A3, class A4> inline bool init4(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init4.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init4.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1, x2, x3, x4); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1, class A2, class A3, class A4, class A5> inline bool init5(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init5.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init5.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1, x2, x3, x4, x5); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
    template<class A1, class A2, class A3, class A4, class A5, class A6> inline bool init6(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init6.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init5.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value(x1, x2, x3, x4, x5, x6); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }

          // Same as init#(), but args. are cast to non-const when passed to constructor.
        template<class A1> inline bool init1nc(bool no_exc, const A1& x1)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init1.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init1.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
        template<class A1, class A2> inline bool init2nc(bool no_exc, const A1& x1, const A2& x2)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init2.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init2.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1, (A2&)x2); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
        template<class A1, class A2, class A3> inline bool init3nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init3.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init3.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1, (A2&)x2, (A3&)x3); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
        template<class A1, class A2, class A3, class A4> inline bool init4nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init4.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init4.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
        template<class A1, class A2, class A3, class A4, class A5> inline bool init5nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init5.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init5.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }
        template<class A1, class A2, class A3, class A4, class A5, class A6> inline bool init6nc(bool no_exc, const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { if (b_valid()) { return true; } L __lock; if (!_pst_m || !__lock.b_valid()) { if (no_exc) { return false; } throw XUExec("init6.1", typeid(t_svf).name()); } if (b_valid()) { return true; } if (b_destroyed()) { if (no_exc) { return false; } throw XUExec("init5.2", typeid(t_svf).name()); } try { new (&_pst_m->stg[0]) t_value((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); } catch (...) { if (!no_exc) { throw; } return false; } _pst_m->init = 1; return true; }

      // Returning ref. to valid value with automatic initialization if necessary.
      //    If the value is not valid or storage can't be accessed, functions throw exceptions.
      //    This may occur even for trivial types like int.
      // NOTE Set lock L if the value must be accessed in multithreaded context during static deinitialization.
    inline t_value& rxai0()        { if (b_noinit()) { init0(0); } return rx(); }
    template<class A1> inline t_value& rxai1(const A1& x1)        { if (b_noinit()) { init1(0, x1); } return rx(); }
    template<class A1, class A2> inline t_value& rxai2(const A1& x1, const A2& x2)        { if (b_noinit()) { init2(0, x1, x2); } return rx(); }
    template<class A1, class A2, class A3> inline t_value& rxai3(const A1& x1, const A2& x2, const A3& x3)        { if (b_noinit()) { init3(0, x1, x2, x3); } return rx(); }
    template<class A1, class A2, class A3, class A4> inline t_value& rxai4(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { if (b_noinit()) { init4(0, x1, x2, x3, x4); } return rx(); }
    template<class A1, class A2, class A3, class A4, class A5> inline t_value& rxai5(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { if (b_noinit()) { init5(0, x1, x2, x3, x4, x5); } return rx(); }
    template<class A1, class A2, class A3, class A4, class A5, class A6> inline t_value& rxai6(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { if (b_noinit()) { init6(0, x1, x2, x3, x4, x5, x6); } return rx(); }

          // Same as rxai#(), but args. are cast to non-const when passed to constructor.
        template<class A1> inline t_value& rxai1nc(const A1& x1)        { if (b_noinit()) { init1nc(0, x1); } return rx(); }
        template<class A1, class A2> inline t_value& rxai2nc(const A1& x1, const A2& x2)        { if (b_noinit()) { init2nc(0, x1, x2); } return rx(); }
        template<class A1, class A2, class A3> inline t_value& rxai3nc(const A1& x1, const A2& x2, const A3& x3)        { if (b_noinit()) { init3nc(0, x1, x2, x3); } return rx(); }
        template<class A1, class A2, class A3, class A4> inline t_value& rxai4nc(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { if (b_noinit()) { init4nc(0, x1, x2, x3, x4); } return rx(); }
        template<class A1, class A2, class A3, class A4, class A5> inline t_value& rxai5nc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { if (b_noinit()) { init5nc(0, x1, x2, x3, x4, x5); } return rx(); }
        template<class A1, class A2, class A3, class A4, class A5, class A6> inline t_value& rxai6nc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { if (b_noinit()) { init6nc(0, x1, x2, x3, x4, x5, x6); } return rx(); }
  };
  template<class T, class C, int Fi, class Fs, meta::s_ll Lcp_mcs, s_long Lto_ms, class _> typename svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::t_storage svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::_storage = { { 0 }, 0 };
  template<class T, class C, int Fi, class Fs, meta::s_ll Lcp_mcs, s_long Lto_ms, class _> typename svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::t_deinit svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::_deinit;
  template<class T, class C, int Fi, class Fs, meta::s_ll Lcp_mcs, s_long Lto_ms, class _> typename svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::t_storage* svf_m_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::_pst_m = 0;




  //==  String conversions, related to character sets.

    // Converting wide character string to 1-byte character string, based on system-default locale (equiv. to std::setlocale(..., "")).
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  std::string wsToBs(arrayref_t<wchar_t> x);
  std::string wsToBs(const wchar_t* x, meta::s_ll n = -1);
    //
    // Converting wide character string to OEM char string - Windows-specific (can be used for text output to console)
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  std::string wsToBsOem(arrayref_t<wchar_t> x);
  std::string wsToBsOem(const wchar_t* x, meta::s_ll n = -1);
    //
    // Converting wide character string to UTF-8 string.
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  inline std::string wsToBsUtf8(arrayref_t<wchar_t> x __bmdx_noarg) { return bmdx_str::conv::wsbs_utf8(x); }
  inline std::string wsToBsUtf8(const wchar_t* x, meta::s_ll n = -1 __bmdx_noarg) { return bmdx_str::conv::wsbs_utf8(x, n); }
    //
    // Direct converting low byte (8 bits) of each wide char value to char.
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  std::string wsToBsLsb(arrayref_t<wchar_t> x);
  std::string wsToBsLsb(const wchar_t* x, meta::s_ll n = -1);
    //
    //
    //
    // Converting a string to wide character string, based on system-default locale (equiv. to std::setlocale(..., "")).
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  std::wstring bsToWs(arrayref_t<char> x);
  std::wstring bsToWs(const char* x, meta::s_ll n = -1);
    //
    // Converting UTF-8 string to UTF-16 string.
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  inline std::wstring bsUtf8ToWs(arrayref_t<char> x __bmdx_noarg) { return bmdx_str::conv::bsws_utf8(x); }
  inline std::wstring bsUtf8ToWs(const char* x, meta::s_ll n = -1 __bmdx_noarg) { return bmdx_str::conv::bsws_utf8(x, n); }
    //
    // Direct converting input char values, treated as unsigned char in range 0..255, to numerically equal wide char values.
    //    n >= 0: specifies length of the string x.
    //    n < 0: autodetect x length based on null char.
  std::wstring bsLsbToWs(arrayref_t<char> x);
  std::wstring bsLsbToWs(const char* x, meta::s_ll n = -1);




  //==  Custom interfacing to a class (o_*).

  // Purpose:
  //    a) accessing functions, declared in the class, from any binary module in the process,
  //    b) replacing their implementation without modifying the class,
  //    c) adding interfaces that are not supported by the class at all,
  //    d) adding interfaces to non-class objects (e.g. char*).

  // Basic usage:
  //    (1) Declare a o_iimpl specialization for one or more interfaces (I1, I2...) as shown below.
  //    (2) Create unity(object), and use its pinterface() method to get the specific interface reference.
  //    Optional (3) Declare specialization of o_interfaces_top<T> : o_interfaces<T, I1, I2...>.
  //      After this, unity::set_obj_atop will attach the interface set to object at once with setting
  //      object reference.
  //    Optional (4) To make implemented interface I accessible in another binary module,
  //      declare o_proxy specialization able to call static versions of all I functions.
  //      After that, unity::pinterface automatically creates proxy object if necessary.
  //      Example: search "struct o_proxy<i_dispatcher_mt>" in the current file.

  // All steps are illustrated below.



  // (1) o_iimpl specialization: a custom interface implementation for a class.

    // template<>
    // struct o_iimpl<T, I1> : o_ibind<T, I1>, I1
    // {
    //   virtual I1* __pMyI() { return this; }
    //   virtual void __itf_after_construct(s_long flags, o_itfset_base* src) {}
    //   virtual void __itf_before_destroy(s_long flags) {}
    //
    //   ... (custom implementation of I1)
    // };
    //
    // template<>
    // struct o_iimpl<T, I2> : o_ibind<T, I2>, I2
    // ...

    // NOTE Default o_iimpl assumes that the target object itself inherits from I1, and is its own implementation.



  // (2) Attaching interfaces to the object.

    // unity x; x.objt<T>(0)(); // create T(), wrapped into x.
    // x.objItfsAttach<o_interfaces<T, I1, I2> >();
    //
    // // Now, any following code does not have to know about type T to operate.
    //
    // x.pinterface<I1>()->...; // invokes a method, implemented in o_iimpl<T, I1>
    // x.pinterface<I2>()->...; // invokes a method, implemented in o_iimpl<T, I2>



  // Optional (3) o_interfaces_top specialization: a structure comprising default set interfaces supported for a class.

    // template<> struct o_interfaces_top<T> : public o_interfaces<T, I1, I2... up to 30 interfaces> {};
    //
    // // Anywhere else (does not have to know about o_interfaces_top<T>):
    //
    // unity x; T* p = new T();
    //  try { x.set_obj_atop<T>(*p, 1); } catch(...) { delete p; throw; } // wrap *p into x.
    //
    // // Anywhere else (does not have to know about type T):
    //
    // x.pinterface<I1>()->...; // invokes a method, implemented in o_iimpl<T, I1>
    // x.pinterface<I2>()->...; // invokes a method, implemented in o_iimpl<T, I2>



  // (4) Example for interface proxy for calling I1 attached in binary module A, from binary module B.

    // I1 and its proxy declaration, in header, included by both A and B.

      //  struct I1
      //  {
      //    virtual double test_sum(double x, double y) = 0;
      //    virtual double test_product(double x, double y) = 0;
      //  };
      //
      //  namespace bmdx
      //  {
      //    template<> struct o_proxy<I1> : o_proxy_base<I1>
      //    {
      //        // __iname() matching is exact, spaces, letter case etc. are taken into account.
      //        // Recommended format:
      //        //   <developer>/<type library name>/<interface name>/<version>/<additional parts>
      //        //  Version, additional parts - optional. Separator (for decomposition): '/'.
      //      static inline const char* __iname() { return "Developer/Example/I1/1.0"; }
      //
      //      struct __test_sum
      //      {
      //        typedef double (*PF)(__Interface* __pi, double x, double y);
      //        static double F(__Interface* __pi, double x, double y) { return __pi->test_sum(x, y); }
      //      };
      //      virtual double test_sum(double x, double y) { return __call<__test_sum>()(__psi(), x, y); }
      //
      //      struct __test_product
      //      {
      //        typedef double (*PF)(__Interface* __pi, double x, double y);
      //        static double F(__Interface* __pi, double x, double y) { return __pi->test_product(x, y); }
      //      };
      //      virtual double test_product(double x, double y) { return __call<__test_product>()(__psi(), x, y); }
      //
      //      typedef unity_common::fn_list<__test_sum, __test_product> __Methods;
      //
      //    };
      //    namespace { o_proxy<I1> __o_proxy_I1_inst; }
      //  }


    // I1 implementation by TestClass itself, in binary module A.

      //  struct TestClass: public I1
      //  {
      //    double s0, p0;
      //    TestClass() : s0(0), p0(1) {}
      //    virtual double test_sum(double x, double y) { return x + y + s0; }
      //    virtual double test_product(double x, double y) { return x * y * p0; }
      //  };


    // Creating an object of TestClass + attaching I1, in binary module A.

      //  unity v;
      //    v.objt<TestClass>(0)();
      //    v.objItfsAttach<o_interfaces<TestClass, I1> >();


    // Calling I1 on any side A or B (assuming the container object is passed by pointer):


      //  void f(unity* pv)
      //  {
      //    o_iptr_t<I1> ptr = pv->pinterface<I1>();
      //    if (ptr)
      //    {
      //      cout << ptr->test_sum(1.1, 2) << "\n";
      //      cout << ptr->test_product(1.1, 2) << "\n";
      //    }
      //  }

      // In module A, ptr will reference directly I1 of TestClass instance.
      // In module B, ptr will contain a proxy object, calling I1 of TestClass instance on the side of module A.




  // (5) Additional info.

    //  1. o_iimpl accesses its target object with __pobj(), and any of attached interfaces
    //    with __pinterface(), both defined in base o_ibind.
    //
    //  2. In the list of interfaces (o_interfaces<>), any argument can be set to int, to quickly disable entry.
    //
    //  3. It is possible to attach interfaces without knowing the real object class.
    //    o_iimpl class may get valid object pointer of certain type from __pobj() with specific flags.
    //    Working with object may be via __pobj() and/or __pinterface() as needed by application.
    //
      // class Nothing {};
      // unity x; x.objt<MyClass1>(0)();
      //
      // x.objItfsAttach<o_interfaces<MyClass1, I0> >();
      // x.objItfsAttach<o_interfaces<Nothing, I1, int, I3, I4> >();
      //
      // x.pinterface<I1>()->...; // invokes o_iimpl<Nothing, I1>, which can still access I0, I3, I4 via __pinterface().
      // x.pinterface<I3>()->...; // analogous with with o_iimpl<Nothing, I3>








    // Implementation part.
  struct _o_refcnt
  {
    unity_common::__Psm rc_posm; // getter for _o_sm_t<T> static methods
    void* rc_pobj;
    void* rc_pcsdx;
    s_long rc_strong;
    s_long rc_weak;
    s_long rc_flags; // (f&1)!=0: rc_pcsdx is dynamically created _rccsd; (f&1)==0: rc_pcsdx is a pointer to static critsec_t<Obj>::csdata

    template<class Obj, class _ = __vecm_tu_selector> struct _rccsd { typedef typename critsec_t<Obj>::csdata t_csd; s_long rcnt; t_csd csd; _rccsd() : rcnt(0), csd() {} };

    _o_refcnt();
  private: _o_refcnt(const _o_refcnt&); void operator=(const _o_refcnt&);
  };
  struct o_itfset_base;
  struct _o_itfslist
  {
    struct handle
    {
      o_itfset_base* p_itfs;
      unity_common::__Psm p_itfssm;
      handle();
      bool b_valid() const;
    };

    unity_common::__Psm pmsm; // that of _o_itfslist and its containing unity object
    _o_refcnt* prc;
    cpparray_t<handle> itfslist;

    _o_itfslist();
  private: _o_itfslist(const _o_itfslist&); void operator=(const _o_itfslist&);
  };
  template<class I, class _ = __vecm_tu_selector> struct o_proxy
  {
    static inline const char* __iname() { return typeid(I).name(); }
    unity_common::__Psm __psm; I* __pi; // not used in default impl.
    o_proxy() : __psm(0), __pi(0) {}
    I* __pci() const { return 0; }
    I* __psi() const { return 0; }
    void __set_pp(unity_common::__Psm psm, I* pi) __bmdx_noex { __psm = psm; __pi = pi; }
  };

  #if __bmdx_use_arg_tu
    namespace { template<class I, class _ = __vecm_tu_selector> struct _o_iptr_des_t; }
  #endif

    // NOTE o_iptr_t is only for in-module use. Its structure for different I may be different.
    //  Pass the original unity object between binary modules if necessary.
  template<class I> struct o_iptr_t
  {
    typedef I __I; typedef o_proxy<__I, __vecm_tu_selector> __Proxy;
    struct exc_null_ptr : std::exception { _fls75 msg; exc_null_ptr() { msg = msg + "o_iptr_t<"+ typeid(I).name() + ">::exc_null_ptr"; } const char* what() const __bmdx_noex { return msg.c_str(); } };



    __I* operator->() const        __bmdx_exs(exc_null_ptr) { return _p_checked(); }
    __I& operator*() const        __bmdx_exs(exc_null_ptr) { return *_p_checked(); }

      // true if o_iptr_t object is non-null, and its referenced object is local.
    bool b_local(__bmdx_noarg1) const { return !!_p; }

      // true if o_iptr_t object is non-null, and its referenced object is from other binary module.
    bool b_nonlocal(__bmdx_noarg1) const { return _f && prx().__pci(); }

      // Conversion to client-side pointer.
    operator const void*() const        { return _p_u(); }

      // Client-side pointer for calling interface functions.
      //  Checked version: ptr().
      //  Unchecked version: ptr_u().
    __I* ptr(__bmdx_noarg1)        __bmdx_exs(exc_null_ptr) { return _p_checked(); }
    const __I* ptr(__bmdx_noarg1) const        __bmdx_exs(exc_null_ptr) { return _p_checked(); }
    __I* ptr_u(__bmdx_noarg1)        __bmdx_noex { return _p_u(); }
    const __I* ptr_u(__bmdx_noarg1) const        __bmdx_noex { return _p_u(); }

      // In case of local interface, returns local pointer.
      // In case of proxy, returns server-side interface pointer (same as source unity::objPtr()).
      //  NOTE server-side interface pointer may be unsafe to use directly (due to binary incompatibility).
    __I* psi_u(__bmdx_noarg1)        __bmdx_noex { return _psi_u(); }
    const __I* psi_u(__bmdx_noarg1) const        __bmdx_noex { return _psi_u(); }



      // NOTE In case of proxy, client-side interface pointers are compared.
    bool operator == (const o_iptr_t& x) const        { return _p_u() == x._p_u(); }
    bool operator != (const o_iptr_t& x) const        { return _p_u() != x._p_u(); }
    bool operator == (const __I* x) const        { return _p_u() == x; }
    bool operator != (const __I* x) const        { return _p_u() != x; }



    void clear(__bmdx_noarg1)        __bmdx_noex { *this = o_iptr_t(); }

    o_iptr_t(__bmdx_noarg1)        __bmdx_noex : _p(0), _f(0)
    {
      #if __bmdx_use_arg_tu
        _p_des = &_o_iptr_des_t<I>::f_des;
      #endif
    }

      // NOTE Construction does not generate exceptions.
      //    If no proxy defined by the client, or the client-defined __Proxy() fails (this is normally not expected),
      //    o_iptr_t will be null (== false).
    o_iptr_t(__I* pi, unity_common::__Psm pmsm __bmdx_noarg)        __bmdx_noex : _p(0), _f(0)
    {
      #if __bmdx_use_arg_tu
        _p_des = &_o_iptr_des_t<I>::f_des;
      #endif
      if (pmsm == unity_common::pls_modsm()) { _p = pi; return; }
        if (!(pi && pmsm)) { return; }
      unity_common::__Pipsm f_get_ism = (unity_common::__Pipsm)pmsm(unity_common::msm_obj_ipsm);
        if (!f_get_ism) { return; }
      unity_common::__Psm psm = f_get_ism(__Proxy::__iname());
        if (!psm) { return; }
      try { new (&prx()) __Proxy(); prx().__set_pp(psm, pi); _f = 1; } catch (...) {}
    }

      // NOTE Copying does not generate exceptions.
      //    If copying fails, ptr_u() == 0.
      //    If copying succeeds, bool(ptr_u()) == bool(x.ptr_u()).
      //    If the client-defined __Proxy copy constructor does not fail anyway, copying always succeeds as well.
    o_iptr_t(const o_iptr_t& x __bmdx_noarg)         __bmdx_noex : _p(x._p), _f(0)
    {
      #if __bmdx_use_arg_tu
        _p_des = &_o_iptr_des_t<I>::f_des;
      #endif
      if (x.b_nonlocal()) { try { new (&prx()) __Proxy(x.prx()); _f = 1; } catch (...) {} }
    }

      // NOTE Assignment does not generate exceptions.
      //    If assignment fails, ptr_u() == 0.
      //    If assignment succeeds, bool(ptr_u()) == bool(x.ptr_u()).
      //    If the client-defined __Proxy copy constructor does not fail anyway, assignment always succeeds as well.

    ~o_iptr_t()        __bmdx_exs(__bmdx_noargt1)
    {
      #if __bmdx_use_arg_tu
        _p_des(*this);
      #else
        _des();
      #endif
    }

  #if __bmdx_use_arg_tu
    o_iptr_t& operator=(const yk_c::meta::arg_tu_t<o_iptr_t>& x2)        __bmdx_exs(__bmdx_noargt1)
      { const o_iptr_t& x = x2.x; _p_des(*this); new (this) o_iptr_t(x); return *this; }
  #else
    o_iptr_t& operator=(const o_iptr_t& x)        __bmdx_exs(__bmdx_noargt1)
      { _des(); new (this) o_iptr_t(x); return *this; }
  #endif



  private:
    __I* _p; char _sprx[sizeof(__Proxy)]; s_long _f;
    #if __bmdx_use_arg_tu
      typedef void (*f_des)(o_iptr_t&);
      f_des _p_des;
      friend struct _o_iptr_des_t<I>;
    #endif
    inline void _des(__bmdx_noarg1)        __bmdx_exs(__bmdx_noargt1) { _p = 0; if (_f) { try { prx().~__Proxy(); } catch (...) {} _f = 0; } }
    inline __I* _p_checked(__bmdx_noarg1) const { if (_p) { return _p; } else if (_f && prx().__pci()) { return prx().__pci(); } throw exc_null_ptr(); }
    inline __I* _p_u(__bmdx_noarg1) const { if (_p) { return _p; } else if (_f) { return prx().__pci(); } return 0; }
    inline __I* _psi_u(__bmdx_noarg1) const { if (_p) { return _p; } else if (_f) { return prx().__psi(); } return 0; }
    __Proxy& prx(__bmdx_noarg1) const { return *(__Proxy*)&_sprx[0]; }
  };

  #if __bmdx_use_arg_tu
    namespace { template<class I, class _> struct _o_iptr_des_t { typedef o_iptr_t<I> t; static void f_des(t& x) { x._des(); } }; }
  #endif

  struct o_itfset_base
  {
    _o_itfslist* __pidyn;

      // NOTE o_iptr_t is not thread-safe or ref.-counted.
      //    The referenced interface object must not be deleted in any way during client use of o_iptr_t.
      // flags OR-ed (dflt. 0x1f):
      //    1 - search includes interfaces from the current binary module (by typeid name matching).
      //    2 - search includes interfaces from all other binary modules (by interface name defined in o_proxy<I> specialization).
      //    4 - search includes interfaces in sets attached before this one.
      //    8 - search includes interfaces in this set.
      //    0x10 - search includes interfaces in sets attached after this one.
      // arg 2: do not specify.
    #if defined(__GNUC__) || defined(__clang__)
      template<class I> o_iptr_t<I> __pinterface(s_long flags = 0x1f, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const;
    #else
      template<class I> o_iptr_t<I> __pinterface(s_long flags, meta::noarg_tu_t<I>) const;
    #endif

    o_itfset_base(meta::noarg_tu_t<o_itfset_base> = meta::noarg_tu_t<o_itfset_base>()) : __pidyn(0) {}
    o_itfset_base(const o_itfset_base& x, meta::noarg_tu_t<o_itfset_base> = meta::noarg_tu_t<o_itfset_base>()) : __pidyn(x.__pidyn) {}
    virtual void* __ifind(const char* iname) = 0; // NOTE the fn. takes an interface name (typeid(I).name()), and returns I* if successful.
    virtual void* __itfs_cast(const char* itfsname) = 0; // NOTE the fn. takes an interface set name (typeid(Itfs).name()), but returns _o_itfs_topmost<Itfs>* if successful
    virtual ~o_itfset_base() {}
  };
  template<class Itfs, class _ = __vecm_tu_selector>
  struct _o_itfs_topmost : Itfs
  {
    virtual void* __itfs_cast(const char* itfsname)
      { if (!itfsname) { return 0; }  if (std::strcmp(typeid(Itfs).name(), itfsname) == 0) { return this; } return 0; }
  };



    //==  Information on reference count of an object, wrapped into unity object.
  struct o_ref_info
  {
    static const s_long rc_deleted = -(s_long(1) << 30);

    s_long _s, _w, _f;

    o_ref_info(s_long s_, s_long w_, s_long f_) : _s(s_), _w(w_), _f(f_) {}

    bool b_valid() const { return _s || _w || _f; }

    s_long rc_strong() const { return _s >= 0 ? _s : 0; }
    s_long rc_weak() const { return _w >= 0 ? _w : 0; }

    bool b_strong() const { return (_f & 3) == 3 && !b_deleted(); }
    bool b_weak() const { return (_f & 3) == 2 && !b_deleted(); }
    bool b_deleted() const { return _s == rc_deleted; }
  };




    //==  Information on type of an object, wrapped into unity object.
  struct o_type_info
  {
    typedef unity_common::__Psm Psm;

    const char* _tstat; // may be 0
    const char* _tdyn; // may be 0
    union { s_long _t_ind; void* __p1; }; // NOTE _t_ind 0 means "not set"
    Psm _pmsm; // 0 if o_type_info is not valid
    Psm _posm; // object or interface set static methods getter, may be 0
    void* _pobj; // object or interface set, may be 0
    union { s_long _t_size; void* __p2; }; // NOTE _t_size -1 means "not set"

    o_type_info(const char* s_, const char* d_, s_long t_ind_, Psm msm_, Psm osm_, void* pobj_, s_long t_size_);

    bool b_valid() const;

    const char* ptstat() const; // non-0 ptr.
    const char* ptdyn() const; // non-0 ptr.

    bool b_local() const; // the object is created in the current bin. module
    bool b_tstat() const; // static type name ptr != 0
    bool b_tdyn() const; // dynamic type name ptr != 0
    bool b_t_ind() const; // vecm type index != 0
    bool b_t_size() const; // _t_size is set to sizeof(T)

      // *_cm regards equal type name strings as equal types even if the object belongs to other binary module.
    bool b_same_tstat(const std::type_info& ti) const;
    bool b_same_tdyn(const std::type_info& ti) const;
    bool b_same_tstat_cm(const std::type_info& ti) const;
    bool b_same_tdyn_cm(const std::type_info& ti) const;

    template<class Obj>
      bool b_same_t_ind(meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const
            { if (_t_ind) { return (b_local() || _t_ind < 0) && bytes::type_index_t<Obj>::ind() == _t_ind; } return false; }

    template<class Obj>
      bool b_same_t_size(meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const
            { if (b_t_size()) { return _t_size == sizeof(Obj); } return false; }

      // match() checks possible variants of object compatibility.
      // match_cm() is same as match(), but regards equal type name strings as equal types
      //    even if the object belongs to other binary module.
      // Returns OR-ed:
      //    1 - static type match, 2 - dynamic type match, 4 - type index match (see type_index_t in vecm_hashx.h), 8 - object size match.
      // NOTE If dynamic type matches with Obj, the original object pointer (whose type info is checked)
      //    may be cast to Obj with dynamic_cast.
      //    For static type or t_ind match, C-style or reinterpret cast is required.
      //  NOTE dynamic_cast may crash or return invalid value on certain POSIX systems (even if called in its own module),
      //    when binary modules made with different compilers are loaded into one program.
    template<class Obj>
      int match(meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const
            { return 1 * b_same_tstat(typeid(Obj)) + 2 * b_same_tdyn(typeid(Obj)) + 4 * b_same_t_ind<Obj>() + 8 * b_same_t_size<Obj>(); }
    template<class Obj>
      int  match_cm(meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const
            { return 1 * b_same_tstat_cm(typeid(Obj)) + 2 * b_same_tdyn_cm(typeid(Obj)) + 4 * b_same_t_ind<Obj>() + 8 * b_same_t_size<Obj>(); }

    bool operator==(const std::type_info& ti) const; // b_same_tdyn(ti) || b_same_tstat(ti)
    bool operator!=(const std::type_info& ti) const;
  };




    //==  A set of interfaces for attaching to object of class T.
  template
  <
    class T,
    class I1=int, class I2=int, class I3=int, class I4=int, class I5=int, class I6=int, class I7=int, class I8=int, class I9=int,
    class I10=int, class I11=int, class I12=int, class I13=int, class I14=int, class I15=int, class I16=int, class I17=int, class I18=int, class I19=int,
    class I20=int, class I21=int, class I22=int, class I23=int, class I24=int, class I25=int, class I26=int, class I27=int, class I28=int, class I29=int,
    class I30=int
  >
  struct o_interfaces;


    //==  This class must be inherited by class, implementing interface I for class T.
  template<class T, class I>
  struct o_ibind
  {
    typedef I __t_i;
    o_ibind(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) : __p_itfs(0) {}
    virtual ~o_ibind() __bmdx_exany {}

      // Should return ptr. to object of class, implementing the interface, normally o_iimpl<T, I>.
    virtual I* __pMyI() = 0;

      // Occurs when the set of interfaces has been fully constructed by default (src == 0) or as copy (src != 0).
      // At this point:
      //  1. All interface sets of the object are already constructed and inserted into interface sets list.
      //  2. (flags & 1):
      //      == 0: interface sets, attached before the current set, already processed their __itf_after_construct() call.
      //      == 1: the current set has been constructed individually and inserted into the existing list of sets.
      //  3. Interfaces in the current set, listed before this one (in o_interfaces decl.), already processed their __itf_after_construct() call.
      //  4. Interfaces in the current set, listed after this one, still did not get __itf_after_construct().
      //  5. (flags & 1):
      //      == 0: interface sets, attached after this one, still did not get __itf_after_construct().
      //      == 1: the current set has been constructed individually and inserted into the existing list of sets.
      // NOTE By default, this method impl. may be empty.
    virtual void __itf_after_construct(s_long flags, o_itfset_base* src) = 0;

      // Occurs right before interfaces set destructor invocation.
      // At this point:
      //  1. All interface sets of the object are still existing.
      //  2. (flags & 1):
      //      == 0: interface sets, attached after this one, already processed their __itf_before_destroy() call.
      //      == 1: the current set is being removed individually from the existing list of sets.
      //  3. Interfaces in the current set, listed after this one (in o_interfaces decl.), already processed their __itf_before_destroy() call.
      //  4. Interfaces in the current set, listed before this one, still did not get __itf_before_destroy().
      //  5. (flags & 1):
      //      == 0: interface sets, attached before this one, still did not get __itf_before_destroy().
      //      == 1: the current set is being removed individually from the existing list of sets.
      // NOTE By default, this method impl. may be empty.
    virtual void __itf_before_destroy(s_long flags) = 0;


      // Parent set of interfaces (weak ref., lives longer than o_ibind and its subobject o_iimpl).
    o_interfaces<T>* __p_itfs;
      // A shortcut to unity::o_api::pobj.
      // flags OR-ed:
      //  1 - return obj. pointer if its static type name matches T.
      //  4 - return obj. pointer if its vecm type index matches that of T (see type_index_t in vecm_hashx.h).
      //  8 - return obj. pointer without type check (flags 1, 4 will be ignored).
      //  0x10 - return obj. pointer even if object is deleted.
      //  0x20 - return obj. pointer if object belongs to the current binary module.
      //  0x40 - return obj. pointer if object belongs to any other binary module.
    T* __pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const { if (!__p_itfs) { return 0; } return __p_itfs->__pobj(flags); }
    template<class I2> o_iptr_t<I2> __pinterface(s_long flags, meta::noarg_tu_t<I2> = meta::noarg_tu_t<I2>()) const { if (!__p_itfs) { return o_iptr_t<I2>(); } return __p_itfs->template __pinterface<I2>(flags); }
  };


    //==  Default implementation of interface I for class T.
    //  Should be specialized as necessary.
    //  The simplest form of specialization will inherit from I (in addition to o_ibind),
    //    and implement I for T as necessary. To access object T itself,
    //    in the necessary mode, this->__pobj should be used.
    // NOTE For given I, several variations of o_iimpl are possible.
    //    Declare o_iimpl<T, IAlt1>, o_iimpl<T, IAlt2> etc., each of them inherits
    //    from an o_ibind<T, I> and implements I differently.
    //    IAlt* are formal arguments, to be listed in the set of interfaces (o_interfaces),
    //    attached to target object.
  template<class T, class I, class _ = __vecm_tu_selector>
  struct o_iimpl : o_ibind<T, I>
  {
      // NOTE The default (non-specialized o_iimpl) assumes that the target object itself inherits from I,
      //    and is its own implementation.
      //    In the customized o_iimpl, if it's designed to inherit from I (e.g. instead of aggregation),
      //    uncomment the below line + remove the default __pMyI impl.
    //virtual I* __pMyI() { return this; }
    virtual I* __pMyI()
    {
      I* p = this->__pobj(0x25); if (p) { return p; } // return object itself if it's from the current binary module and a) has static type name == that of T, or b) has the same vecm type index (see type_index_t in vecm_hashx.h)
      p = this->__pobj(0x44); return p; // return object itself if it's from other binary module and has the correct vecm type index
    }
    virtual void __itf_after_construct(s_long flags, o_itfset_base* src) {}
    virtual void __itf_before_destroy(s_long flags) {}
  };



    // Implementation part.
  template<class T, class Iformal, class Iimpl, class _ = __vecm_tu_selector>
  struct _o_iimpl_f_tu_t
  {
    static o_ibind<T, Iimpl>* Fnew() __bmdx_noex { o_ibind<T, Iimpl>* p = 0; try { p = new o_iimpl<T, Iformal, _>(); } catch (...) {} return p;  }
    static o_ibind<T, Iimpl>* Fnewcp(o_ibind<T, Iimpl>* p0) __bmdx_noex { o_ibind<T, Iimpl>* p = 0; try { if (p0) { p = new o_iimpl<T, Iformal, _>(static_cast<o_iimpl<T, Iformal, _>&>(*p0)); } } catch (...) {} return p;  }
  };

  template
  <
    class T,
    class I1, class I2, class I3, class I4, class I5, class I6, class I7, class I8, class I9,
    class I10, class I11, class I12, class I13, class I14, class I15, class I16, class I17, class I18, class I19,
    class I20, class I21, class I22, class I23, class I24, class I25, class I26, class I27, class I28, class I29,
    class I30
  >
  struct o_interfaces
  :
    public o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int>
  {
    typedef typename o_iimpl<T, I1>::__t_i t_i1;
    typedef o_interfaces<T, I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30> __ThisIImpl;
    typedef o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int> __NextIImpl;
    enum { __xx_icnt = __NextIImpl::__xx_icnt + 1 };

    o_ibind<T, t_i1>* __p_iimpl;

    o_interfaces(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())
      : __NextIImpl()
    {
      __p_iimpl = _o_iimpl_f_tu_t<T, I1, t_i1>::Fnew(); if (!__p_iimpl) { throw XUExec("Fnew 0", typeid(o_iimpl<T, I1>).name(), typeid(this).name()); }
      __p_iimpl->__p_itfs = this;
    }
    o_interfaces(const o_interfaces& src, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())
      : __NextIImpl(src)
    {
      __p_iimpl = _o_iimpl_f_tu_t<T, I1, t_i1>::Fnewcp(src.__ThisIImpl::__p_iimpl); if (!__p_iimpl) { throw XUExec("Fnewcp 0", typeid(o_iimpl<T, I1>).name(), typeid(this).name()); }
      __p_iimpl->__p_itfs = this;
    }
    ~o_interfaces() { if (__p_iimpl) { try { delete __p_iimpl; } catch (...) {} } }
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex { try { __p_iimpl->__itf_after_construct(flags, src); ++nsucc; } catch (...) { return false; } return __NextIImpl::__xx_after_construct(flags, src, nsucc); }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex { ++nsucc; bool b = nmax < 0 || nsucc <= nmax; if (nmax < 0 || nsucc < nmax) { __NextIImpl::__xx_before_destroy(flags, nsucc, nmax); } if (!b) { return; } try { __p_iimpl->__itf_before_destroy(flags); } catch (...) {} }
    virtual void* __ifind(const char* iname) { if (!iname) { return 0; } if (__p_iimpl && 0 == std::strcmp(typeid(t_i1).name(), iname)) { try { return __p_iimpl->__pMyI(); } catch (...) {} } return __NextIImpl::__ifind(iname); }
  };

  template
  <
    class T,
    class I2, class I3, class I4, class I5, class I6, class I7, class I8, class I9,
    class I10, class I11, class I12, class I13, class I14, class I15, class I16, class I17, class I18, class I19,
    class I20, class I21, class I22, class I23, class I24, class I25, class I26, class I27, class I28, class I29,
    class I30
  >
  struct o_interfaces<T, int,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30>
  :
    public o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int>
  {
    typedef o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int> __NextIImpl;
    enum { __xx_icnt = __NextIImpl::__xx_icnt };
    o_interfaces(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) {}
    o_interfaces(const o_interfaces& src, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) {}
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex { ++nsucc; return __NextIImpl::__xx_after_copy(flags, src, nsucc); }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex { ++nsucc; if (nmax < 0 || nsucc < nmax) { __NextIImpl::__xx_before_destroy(flags, nsucc, nmax); } }
    virtual void* __ifind(const char* iname) { if (!iname) { return 0; } return __NextIImpl::find(iname); }
  };

  template<class T>
  struct o_interfaces<T>
  :
    public o_itfset_base
  {
    enum { __xx_icnt = 0 };
    o_interfaces(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) {}
    o_interfaces(const o_interfaces& src, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) : o_itfset_base(src) {}
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex { return true; }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex {}

      // A shortcut to unity::o_api::pobj.
      // flags OR-ed:
      //  1 - return obj. pointer if its static type name matches T.
      //  4 - return obj. pointer if its vecm type index matches that of T (see type_index_t in vecm_hashx.h).
      //  8 - return obj. pointer without type check (flags 1, 4 will be ignored).
      //  0x10 - return obj. pointer even if object is deleted.
      //  0x20 - return obj. pointer if object belongs to the current binary module.
      //  0x40 - return obj. pointer if object belongs to any other binary module.
    T* __pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const;

      // Find interface I by its typeid(I).name().
      //  Search direction inside the interface set: from beginning (leftmost listed in o_interfaces<...>) to end (rightmost).
    virtual void* __ifind(const char*) { return 0; }
  };



    // The default (empty) template for the set of interfaces for attaching to an object T, wrapped into struct unity (as subtype utObject).
    //    When specialized for particular T, must inherit from user-defined set of interfaces (e.g. o_interfaces<T, I1, I2...>).
    //    This specialization influences only 1) unity::set_obj_atop and 2) unity::objt with atop == true (non-default arg.).
    //    All objects, created with set_obj_atop or objt with atop true, will be automatically added support for the specified set of interfaces.
    //    (In the above example: I1, I2 etc. The default implementation relies on T inheriting directly from I1, I2..., but the client code
    //    may define custom implementation for any of them, see struct o_iimpl.)
  template<class T, class _ = __vecm_tu_selector>
  struct o_interfaces_top
  : public o_interfaces<T>{};

}

namespace yk_c
{
  template<> struct vecm::spec<bmdx::unity> { struct aux : vecm::config_aux<bmdx::unity> { }; typedef config_t<bmdx::unity, 1, 4, 1, aux> config; };
}




namespace bmdx
{




  //==  struct unity CONSTRUCTION (brief)

    // unity() - constructs an empty object (utype() == utEmpty)
    // unity(<scalar value>) - constructs a scalar value; input value may be converted to one of the supported types
    // unity(std::wstring) - constructs a wrapped string that may be passed between incompatible binary modules
    // unity(std::string) - converts string to wide string using the system-default locale (equiv. to std::setlocale(..., ""))
    // unity(std::vector<scalar value>) - constructs an array of values
    // unity(<object type>&, bool b_strong) - constructs an object reference; see also set_obj and objt

    // clear() - sets utEmpty, all internally handled objects are freed, storage cleared


  //==  struct unity FUNCTION FAMILIES (brief)

    // pval - pointer to value or array elem.
    // ref - ref. to value or array elem.
    // rx - pointer to value or array elem. with autoconversion to the specified type
    // val - value copy to the specified type
    // cref - ref. to value if it is of the specified type, otherwise to temp. dflt. value
    // conv - in-place type conv.
    // arr_* - array info and modification
    // obj*, set_obj, cpvoid*, pinterface, o_api - referenced objects and interfaces
    //
    // u_*; sc*, ua*, map*, hash*; [ ], path, u_name*
    //    -- general functions; manipulating scalar, array, hashed ordered map, hashlist; accessing values by key, index, path,
    //      additional object name field support.
    //
    // +<unity object>/<path>/<dflt. value and ret. type specifier> -- ternary operator to extract value
    //    from associative tree (consisting of hashlists, maps, arrays) by the given path.


  //==  Indexation summary.

    //  Historically, the library uses, in various functions referring to arrays of values,
    //    different values of base indexes.
    //
    //  1-based indexation:
    //    choose, mapi*, hashi*.
    //  1-based indexation on automatic array creation:
    //    rx, conv, ua*, array, decoded paramline arrays.
    //  0-based indexation:
    //    array0, hashl* (unordered), cmd_array, split.
    //  Setting base index dependent on arguments:
    //    arr_init, arrlb_, i_dispatcher_mt::request.
    //  Indexation, dependent on individual array base index:
    //    [], pval, ref, rx, vstr, vcstr, vint*, vfp, ua*, map*, hashi*.


  struct unity //==
  {
  private:
    struct _stg_u { void* p1; void* p2; };
    template<class T, class _ = meta::nothing, class __ = __vecm_tu_selector> struct _reference_base { static inline T* _drf_c(const unity* p) { return 0; } };
    template<class T> struct _reference_base<T, short> { static const s_long utt = -1; };
    template<class T> struct _reference_base<T, int>
    {
      enum { rb_tag = 1 }; typedef T t;
      static inline t* deref(_stg_u& x, bool byptr) { return reinterpret_cast<t*>(byptr ? x.p1 : &x); }
      static inline t* _drf_c(const unity* p) { return deref(p->_data, p->isByPtr()); }
    };
    typedef s_long (*__Pws_ti)();
  public:
    template<class T> struct checked_ptr
    {
      T* p;

      inline checked_ptr(const T* p_)        __bmdx_noex : p((T*)p_) {}

      inline T& operator*() const        __bmdx_exs(XUExec) { check(); return *p; }
      inline operator T*() const        __bmdx_noex { return p; }

      inline operator bool() const        __bmdx_noex { return !!p; }
      inline bool operator!() const        __bmdx_noex { return !p; }
      inline bool is_null() const        __bmdx_noex { return !p; }

      inline T* operator->() const        __bmdx_exs(XUExec) { check(); return p; }
      inline bool operator==(const checked_ptr& p2) const        __bmdx_noex { return p == p2.p; }
      inline bool operator!=(const checked_ptr& p2) const        __bmdx_noex { return p == p2.p; }

        // Same as in o_iptr_t.
      inline const T* ptr() const        __bmdx_exs(XUExec) { check(); return p; }
      inline T* ptr()        __bmdx_exs(XUExec) { check(); return p; }
      inline const T* ptr_u() const        __bmdx_noex { return p; }
      inline T* ptr_u()        __bmdx_noex { return p; }

      inline void check() const        __bmdx_exs(XUExec) { if (!p) {        throw XUExec("checked_ptr.p == 0: ", typeid(T*).name()); } }
    };

      // Hashlist and map key functions.
    struct kf_unity
    {
      kf_unity(s_long flags_ = 0); // flags: see static const s_long fkcmp*
      s_long flags() const;
      void _set_flags(s_long x) const;
      s_long hash(const unity& x) const;
      bool is_eq(const unity& x1, const unity& x2) const;
      bool less12(const unity& x1, const unity& x2) const; // x1.k_less(x2, _flags)
      bool less21(const unity& x1, const unity& x2) const; // again x1.k_less(x2, _flags)

      template<class K2> inline void cnew(unity* p, const K2& x, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { new (p) unity(x); if ((_flags & fkcmpScalarize) && !p->isScalar()) { try { *p = p->u_key(); } catch (...) { p->clear(); throw; } } }
      template<class K2> inline s_long hash(const K2& x, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return this->hash(unity(x)); }
      template<class K2> inline bool is_eq(const unity& x1, const K2& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return x1.k_eq(unity(x2), _flags); }
      template<class K2> inline bool less12(const unity& x1, const K2& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return x1.k_less(unity(x2), _flags); } // x1 < x2
      template<class K2> inline bool less21(const K2& x1, const unity& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return unity(x1).k_less(x2, _flags); } // again x1 < x2

      private: mutable s_long _flags;
    };

    struct _hl_i;
    typedef yk_c::ordhs_t<unity, unity, kf_unity, kf_unity> t_map;
    typedef yk_c::ordhs_t<unity, unity, kf_unity, kf_unity, __vecm_tu_selector> t_map_tu;
    typedef _hl_i t_hash;

    template<int utt, class _ = meta::nothing, class __ = __vecm_tu_selector> struct valtype_t {};

      template<class _> struct valtype_t<utInt, _> { typedef meta::s_ll t; };
      template<class _> struct valtype_t<utFloat, _> { typedef double t; };
      template<class _> struct valtype_t<utDate, _> { typedef _unitydate t; };
      template<class _> struct valtype_t<utChar, _> { typedef _unitychar t; };
      template<class _> struct valtype_t<utString, _> { typedef std::wstring t; };
      template<class _> struct valtype_t<utUnity, _> { typedef unity t; };
      template<class _> struct valtype_t<utIntArray, _> { typedef vec2_t<meta::s_ll> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<utFloatArray, _> { typedef vec2_t<double> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<utDateArray, _> { typedef vec2_t<_unitydate> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<utCharArray, _> { typedef vec2_t<_unitychar> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<utStringArray, _> { typedef vec2_t<std::wstring> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<utUnityArray, _> { typedef vec2_t<unity> t; typedef typename t::t_value t_elem; typedef vec2_t<t_elem, __vecm_tu_selector> t_tu; };
      template<class _> struct valtype_t<-1, _> { typedef void t; };

    template<class T, class _ = meta::nothing, class __ = __vecm_tu_selector> struct reference_t : _reference_base<T, _> {};

      template<class _> struct reference_t<meta::s_ll, _>  : _reference_base<meta::s_ll, int> { static const s_long utt = utInt; };
      template<class _> struct reference_t<double, _>  : _reference_base<double, int> { static const s_long utt = utFloat; };
      template<class _> struct reference_t<_unitydate, _>  : _reference_base<_unitydate, int> { static const s_long utt = utDate; };
      template<class _> struct reference_t<_unitychar, _>  : _reference_base<_unitychar, int> { static const s_long utt = utChar; };
        // NOTE reference_t::_drf_c for std::wstring and vec2_t<std::wstring>
        //    include additional check for string compatibility.
        //    Returns 0 when string is incompatible.
        //    To get a valid copy of the value, use val and cref.
      template<class _> struct reference_t<std::wstring, _>  : _reference_base<std::wstring, int>
      {
        static const s_long utt = utString;
        static inline typename reference_t::t* _drf_c(const unity* p)
        {
          if (p->pmsm == unity_common::pls_modsm()) { return reference_t::deref(p->_data, true); }
          s_long ti1 = yk_c::typer<std::wstring, __vecm_tu_selector>().t_ind;
          s_long ti2 = ((__Pws_ti)p->pmsm(unity_common::msm_wstring_ti))();
          return  ti1 < 0 && ti1 == ti2 ? reference_t::deref(p->_data, true) : 0;
        }
      };
      template<class _> struct reference_t<unity, _>  { static const s_long utt = utUnity; typedef unity t; };

      template<class T, class __ = __vecm_tu_selector> struct _reference_base_vec2 : _reference_base<T, int, __>
      {
        typedef typename T::t_value t_elem;
        typedef typename T::ta_value ta_elem;
        typedef vec2_t<ta_elem> t;
        typedef vec2_t<ta_elem, __vecm_tu_selector> t_tu;
        typedef typename meta::assert<meta::same_t<T, t>::result>::t_true __check;
        static inline t* _drf_c(const unity* p)
          { t* pvec = _reference_base<T, int, __>::deref(p->_data, true); if (p->pmsm == unity_common::pls_modsm()) { return pvec; } return  p->_compat_chk() ? pvec : 0; }
      };
      template<class _, class __> struct reference_t<vec2_t<meta::s_ll>, _, __>  : _reference_base_vec2<vec2_t<meta::s_ll>, __> { static const s_long utt = utIntArray; };
      template<class _, class __> struct reference_t<vec2_t<double>, _, __>  : _reference_base_vec2<vec2_t<double>, __> { static const s_long utt = utFloatArray; };
      template<class _, class __> struct reference_t<vec2_t<_unitydate>, _, __>  : _reference_base_vec2<vec2_t<_unitydate>, __> { static const s_long utt = utDateArray; };
      template<class _, class __> struct reference_t<vec2_t<_unitychar>, _, __>  : _reference_base_vec2<vec2_t<_unitychar>, __> { static const s_long utt = utCharArray; };
      template<class _, class __> struct reference_t<vec2_t<std::wstring>, _, __>  : _reference_base_vec2<vec2_t<std::wstring>, __> { static const s_long utt = utStringArray; };
      template<class _, class __> struct reference_t<vec2_t<unity>, _, __>  : _reference_base_vec2<vec2_t<unity>, __> { static const s_long utt = utUnityArray; };

      template<class _> struct reference_t<void, _>  : _reference_base<void, int> {};

    template<class T, class _ = meta::nothing, class __ = __vecm_tu_selector> struct trivconv_t { typedef meta::s_ll t_target; static inline t_target F(const T& x) { return x; } static inline T Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<double, _> { typedef double t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<_unitydate, _> { typedef _unitydate t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<_unitychar, _> { typedef _unitychar t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<std::wstring, _> { typedef std::wstring t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unity, _> { typedef unity t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<bool, _> { typedef _unitychar t_target; static inline t_target F(bool x) { return x; } static inline bool Fback(const t_target& x) { return !!x; } };
      template<class _> struct trivconv_t<signed char, _> { typedef _unitychar t_target; static inline t_target F(signed char x) { return x; } static inline signed char Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned char, _> { typedef _unitychar t_target; static inline t_target F(unsigned char x) { return x; } static inline unsigned char Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<wchar_t, _> { typedef meta::s_ll t_target; static inline t_target F(wchar_t x) { return x; } static inline wchar_t Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<signed short int, _> { typedef meta::s_ll t_target; static inline t_target F(signed short int x) { return x; } static inline signed short int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned short int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned short int x) { return x; } static inline unsigned short int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<signed int, _> { typedef meta::s_ll t_target; static inline t_target F(signed int x) { return x; } static inline signed int Fback(const t_target& x) { return (signed int)x; } };
      template<class _> struct trivconv_t<unsigned int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned int x) { return x; } static inline unsigned int Fback(const t_target& x) { return (unsigned int)x; } };
      template<class _> struct trivconv_t<signed long int, _> { typedef meta::s_ll t_target; static inline t_target F(signed long int x) { return x; } static inline signed long int Fback(const t_target& x) { return (signed long int)(x); } };
      template<class _> struct trivconv_t<unsigned long int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned long int x) { return x; } static inline unsigned long int Fback(const t_target& x) { return (unsigned long int)x; } };
      template<class _> struct trivconv_t<signed long long int, _> { typedef meta::s_ll t_target; static inline t_target F(signed long long int x) { return x; } static inline signed long long int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned long long int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned long long int x) { return x; } static inline unsigned long long int Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<float, _> { typedef double t_target; static inline t_target F(float x) { return x; } static inline float Fback(const t_target& x) { return float(x); } };
      template<class _> struct trivconv_t<long double, _> { typedef double t_target; static inline t_target F(long double x) { return x; } static inline long double Fback(const t_target& x) { return x; } };

      struct _wr_cstring :  std::string  {  typedef std::string t_base;  _wr_cstring(); _wr_cstring(const std::string& x); const t_base& cstr() const; std::wstring wstr() const; };
      struct _wr_wstring :  std::wstring  {  typedef std::wstring t_base;  _wr_wstring(); _wr_wstring(const std::wstring& x); std::string cstr() const; const t_base& wstr() const; };

      template<class _> struct trivconv_t<std::string, _> { typedef std::wstring t_target; static inline t_target F(const std::string& x) { return bsToWs(x); } static inline std::string Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<char*, _> { typedef std::wstring t_target; static inline t_target F(const char* x) { return bsToWs(x); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<char[], _> { typedef std::wstring t_target; static inline t_target F(const char x[]) { return bsToWs(x); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<carray_r_t<char>, _> { typedef std::wstring t_target; static inline t_target F(const carray_r_t<char>& x) { if (x.pd()) { return bsToWs(x.pd(), x.n()); } return t_target(); } static inline carray_r_t<char> Fback(const t_target& x) { return carray_r_t<char>(wsToBs(x)); } };
      template<class _> struct trivconv_t<arrayref_t<char>, _> { typedef std::wstring t_target; static inline t_target F(const arrayref_t<char>& x) { return bsToWs(x); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };

      template<class _> struct trivconv_t<wchar_t*, _> { typedef std::wstring t_target; static inline t_target F(const wchar_t* x) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<wchar_t[], _> { typedef std::wstring t_target; static inline t_target F(const wchar_t x[]) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<arrayref_t<wchar_t>, _> { typedef std::wstring t_target; static inline t_target F(const arrayref_t<wchar_t>& x) { if (x.is_empty()) { return t_target(); } return t_target(x.pd(), size_t(x.n())); } static inline _wr_wstring Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<_wr_cstring, _> { typedef std::wstring t_target; static inline t_target F(const _wr_cstring& x) { return x.wstr(); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<_wr_wstring, _> { typedef std::wstring t_target; static inline const t_target& F(const _wr_wstring& x) { return x.wstr(); } static inline _wr_wstring Fback(const t_target& x) { return x; } };

      template<class _, s_long n> struct trivconv_t<bmdx_str::flstr_t<n>, _> { typedef std::string t_target; static inline t_target F(const bmdx_str::flstr_t<n> x) { return x; } static inline bmdx_str::flstr_t<n> Fback(const t_target& x) { return x.c_str(); } };

  private:
    template<int utt, bool __bv = (utt & fvalVector), class __ = __vecm_tu_selector>
      struct _valtype2_t : valtype_t<utt> {};
              template<int utt> struct _valtype2_t<utt, true> { typedef typename valtype_t<((utt & ~fvalVector) | utArray)>::t_elem t_elem; typedef std::vector<t_elem> t; };
    template<int utt, class _  = meta::nothing, class __ = __vecm_tu_selector>
      struct _rx_el { static inline typename valtype_t<utt>::t& F(unity& x) { return x.rx<utt>(); } };
              template<class _> struct _rx_el<utUnity, _> { static inline typename valtype_t<utUnity>::t& F(unity& x) { return x; } };
    template<int utt, class _  = meta::nothing, class __ = __vecm_tu_selector>
      struct _refc_el { static inline const typename valtype_t<utt>::t& F(const unity& x) { return x.ref<utt>(); } };
              template<class _> struct _refc_el<utUnity, _> { static inline const typename valtype_t<utUnity>::t& F(const unity& x) { return x; } };
    template<int utt, class _ = meta::nothing, class __ = __vecm_tu_selector> struct _arr_el_get
    {
      static inline typename _valtype2_t<utt>::t F(const unity* pv, s_long ind, EConvStrength cs)
      {
        typedef typename _valtype2_t<utt>::t T;
        try {
          const T* p = pv->pval<utt>(ind); if (p) { return *p; }
          unity temp; s_long res = pv->_Lel_get(ind, &temp);
          if (res == 1) { return temp.val<utt>(cs); }
          if (res == -2) { throw XUTypeMismatch("_arr_el_get.1", _tname0(pv->utype_noarray()), _tname0(utt)); }
          throw XUExec("_arr_el_get.2", _tname0(pv->utype()), ind);
        } catch (...) { if (cs == csLazy) { return T(); } throw; }
      }
    };
    template<class _> struct _arr_el_get<utUnity, _>
    {
      static inline unity F(const unity* pv, s_long ind, EConvStrength cs)
      {
        typedef unity T;
        try {
          const T* p = pv->pval<utUnity>(ind); if (p) { return *p; }
          unity temp; s_long res = pv->_Lel_get(ind, &temp);
          if (res == 1) { return temp; }
          if (res == -2) { throw XUTypeMismatch("_arr_el_get.3", _tname0(pv->utype_noarray()), _tname0(utUnity)); }
          throw XUExec("_arr_el_get.4", _tname0(pv->utype()), ind);
        } catch (...) { if (cs == csLazy) { return T(); } throw; }
      }
    };
    template<class Td, class Ts, class _ = meta::nothing, class __ = __vecm_tu_selector>
      struct _x_vecadap {};
          template<class Td, class Ts, class _> struct _x_vecadap<vec2_t<Td>, std::vector<Ts>, _> { static bool copy(vec2_t<Td>& dest, const std::vector<Ts>& src) { if (dest.locality() != 1 && dest.compatibility() < 1) { return 0; } dest.clear(); dest.vec2_set_nbase(0); typename std::vector<Ts>::const_iterator e = src.begin(); while(e != src.end()) { if (!dest.el_append(*e++)) { return 0; } } return 1; } };
          template<class Td, class Ts, class _> struct _x_vecadap<vec2_t<Td>, vec2_t<Ts>, _> { static bool copy(vec2_t<Td>& dest, const vec2_t<Ts>& src) { if (src.locality() != 1 && src.compatibility() < 1) { return 0; } if (dest.locality() != 1 && dest.compatibility() < 1) { return 0; } dest.clear(); dest.vec2_set_nbase(src.nbase()); vecm::link1_t<typename vecm::specf<Ts>::t_value, true> e = src.link1_cbegin(); while(!e.is_aend()) { if (!dest.el_append(*e.pval())) { return 0; } e.incr(); } return 1; } };
          template<class _> struct _x_vecadap<vec2_t<std::wstring>, std::vector<std::string>, _> { typedef std::wstring Td; typedef std::string Ts; static bool copy(vec2_t<Td>& dest, const std::vector<Ts>& src) { if (!(dest.locality() == 1 || dest.compatibility() > 0)) { return false; } try { dest.clear(); dest.vec2_set_nbase(0); for (size_t i = 0; i < src.size(); ++i) { dest.push_back(bsToWs(src[i])); } return true; } catch (...) {} return false; } };
          template<class _> struct _x_vecadap<vec2_t<std::wstring>, vec2_t<std::string>, _> { typedef std::wstring Td; typedef std::string Ts; static bool copy(vec2_t<Td>& dest, const vec2_t<Ts>& src) { if (!(dest.locality() == 1 || dest.compatibility() > 0) || !(src.locality() == 1 || src.compatibility() > 0)) { return false; } try { dest.clear(); dest.vec2_set_nbase(src.nbase()); for (s_long i = 0; i < src.n(); ++i) { dest.push_back(bsToWs(src[i])); } return true; } catch (...) {} return false; } };
    template<class Ts> void _x_asgvec(const std::vector<Ts>& src, bool keep_name, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>())        { typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { utt = reference_t<Td>::utt | utArray }; _stg_u st; if (_Lcreate(&st, utt | xfPtr, 0) > 0) { vec2_t<Td>* pv = reference_t<vec2_t<Td> >::deref(st, true); if (_x_vecadap<vec2_t<Td>, std::vector<Ts> >::copy(*pv, src)) { void* _pn(0); if (keep_name && isNamed()) { _pn = _data.p2; _data.p2 = 0; } clear(); _data.p1 = st.p1; _data.p2 = _pn; ut = utt | xfPtr; return; } _Ldestroy(&st, utt | xfPtr, 0); } throw XUExec("_x_asgvec", "src: " + _tname0(utt)); }
    template<class Ts> void _x_asgvec(const vec2_t<Ts>& src, bool keep_name, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>())        { typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { utt = reference_t<Td>::utt | utArray }; _stg_u st; if (_Lcreate(&st, utt | xfPtr, 0) > 0) { vec2_t<Td>* pv = reference_t<vec2_t<Td> >::deref(st, true); if (_x_vecadap<vec2_t<Td>, vec2_t<Ts> >::copy(*pv, src)) { void* _pn(0); if (keep_name && isNamed()) { _pn = _data.p2; _data.p2 = 0; } clear(); _data.p1 = st.p1; _data.p2 = _pn; ut = utt | xfPtr; return; } _Ldestroy(&st, utt | xfPtr, 0); } throw XUExec("_x_asgvec", "src: " + _tname0(utt)); }

    template<class T, class _ = __vecm_tu_selector> struct _deref1_t { typedef T t_obj; enum { is_cref_t = 0 }; static void* ptr(void* pobj) { return reinterpret_cast<typename meta::noncv_t<T>::t*>(pobj); } };
    template<class T, class L, class _> struct _deref1_t<cref_t<T, L>, _> { typedef T t_obj; enum { is_cref_t = 1 }; static void* ptr(void* pobj) { return const_cast<typename meta::noncv_t<T>::t*>(reinterpret_cast<cref_t<T, L>* >(pobj)->_pnonc_u()); } };

    template<class T> T& _rnonc(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const
    {
      enum { utt = reference_t<T, short>::utt };
      if (isObject())
      {
        typedef _deref1_t<T> D;
        typedef typename D::t_obj R;
        if (!D::is_cref_t) { return *checked_ptr<T>(reinterpret_cast<T*>(o_api(this).prefo(typeid(T).name(), bytes::type_index_t<T>::ind(), -1))); }
        std::string t1("|cref_t|"); t1 += typeid(R).name();
        return *checked_ptr<T>(reinterpret_cast<T*>(o_api(this).prefo(t1.c_str(), bytes::type_index_t<R>::ind(), -1)));
      }
      if (utt == utype()) { return *checked_ptr<T>(reference_t<T>::_drf_c(this)); }
      throw XUTypeMismatch("_rnonc", _tname0(utype()), _tname0(utt));
    }
  public:




    // Informational functions.

      // EUnityType
    inline s_long utype() const __bmdx_noex { return ut & xfmUtype; }

      // EUnityType without utArray flag
    inline s_long utype_noarray() const __bmdx_noex { return ut & utMaskNoArray; }

      // Returns type name of the contained object or one of predefined types,
      //  like
      // .Empty, .Int, .String, .Map,
      // X (typeid.name() of the contained object),
      // .Float[], .Unity[].
      //
      // If allowArrDim == true, arrays are with bound. indices, e. g.:
      // .Int[L..U], .String[L..U], .Unity[L..U].
    std::wstring tname(bool allowArrDim = false) const;

      // The set
      //    scalar, array, object, assoc. array (map or hashlist)
      //  covers all available types in a valid object.
      //  Empty is a scalar. Bool is represented by 0/non-0 char.
      // NOTE Tests are able to return false on this == 0.
    inline bool isScalar() const __bmdx_noex { return this && utype() <= utString; }
    inline bool isArray() const __bmdx_noex { return this && !!(ut & utArray); }
    inline bool isUnityArray() const __bmdx_noex { return this && utype() == utUnityArray; }
    inline bool isObject() const __bmdx_noex { return this && utype() == utObject; }
    bool isAssoc() const __bmdx_noex; // true if unity is utHash or utMap

      // Tests for particular types and values.
      // NOTE Tests are able to return false on this == 0.
    inline bool isEmpty() const __bmdx_noex { return this && utype() == utEmpty; }
    inline bool isNonempty() const __bmdx_noex { return !isEmpty(); }
    inline bool isInt() const __bmdx_noex { return this && utype() == utInt; }
    inline bool isFloat() const __bmdx_noex { return this && utype() == utFloat; }
    inline bool isDate() const __bmdx_noex { return this && utype() == utDate; }
    inline bool isString() const __bmdx_noex { return this && utype() == utString; }
    inline bool isChar() const __bmdx_noex { return this && utype() == utChar; }
    inline bool isMap() const __bmdx_noex { return this && utype() == utMap; }
    inline bool isHash() const __bmdx_noex { return this && utype() == utHash; }
      //
      // More complex type tests.
      //
      // Char may be treated as boolean.
    inline bool isBool() const __bmdx_noex { return this && utype() == utChar; }
    bool isBoolTrue() const __bmdx_noex; // true if *this contains char != 0
    bool isBoolFalse() const __bmdx_noex; // true if *this contains char == 0
      //
      // IntOrFloat -- "wide-range numeric value, not in string form"
      // IntOrChar -- "boolean-like value, not in string form"
    inline bool isIntOrFloat() const __bmdx_noex { return this && (utype() == utInt || utype() == utFloat); }
    inline bool isIntOrChar() const __bmdx_noex { return this && (utype() == utInt || utype() == utChar); }
      //
      // *OrEmpty - "optional value of particular type"
    inline bool isIntOrEmpty() const __bmdx_noex { return this && (utype() == utInt || utype() == utEmpty); }
    inline bool isFloatOrEmpty() const __bmdx_noex { return this && (utype() == utFloat || utype() == utEmpty); }
    inline bool isDateOrEmpty() const __bmdx_noex { return this && (utype() == utDate || utype() == utEmpty); }
    inline bool isStringOrEmpty() const __bmdx_noex { return this && (utype() == utString || utype() == utEmpty); }
    inline bool isCharOrEmpty() const __bmdx_noex { return this && (utype() == utChar || utype() == utEmpty); }
    inline bool isMapOrEmpty() const __bmdx_noex { return this && (utype() == utMap || utype() == utEmpty); }
    inline bool isHashOrEmpty() const __bmdx_noex { return this && (utype() == utHash || utype() == utEmpty); }
    inline bool isBoolOrEmpty() const __bmdx_noex { return this && (utype() == utChar || utype() == utEmpty); }

    // Other tests.

      // true if the value is kept by pointer (dynamically allocated).
      // false if the value is kept as bytes within the object storage.
    inline bool isByPtr() const { return !!(ut & xfPtr); }

      // true if the object has a name assigned.
    inline bool isNamed() const { return (ut & xfPtr) && _data.p2; }

      // true if the object is created in the current binary module.
    bool isLocal() const;

      // Exact equality (values, arrays, strings, assoc. arrays, object pointers)
    bool operator==(const unity& x) const;
    bool operator!=(const unity& x) const;

      // Flags for uniqueness and relation of map and hashlist keys
    static const s_long fkcmpNFloat = 0x2; // convert integer keys (utInt, utChar) to double
    static const s_long fkcmpSNocase = 0x1; // strings comparison and hashing are case insensitive (0 == binary comp.)
    static const s_long fkcmpRevOrder = 0x4; // (map only) reverse order in "<" comparison
    static const s_long fkcmpScalarize = 0x8; // do not use non-scalar keys, convert a copy to scalar and use it as key
    static const s_long _fkcmp_n = 16;
    static const s_long _fkcmp_mask = _fkcmp_n - 1;

      // Hash value.
      //  flags:
      //    fkcmpNFloat - for utChar, utInt, calc. hash value from val<utFloat>
      //    fkcmpSNocase - for utString calc. hash value from string in lowercase
    s_long k_hashf(s_long flags) const;

      // Equality and op. <.
      //  flags:
      //    fkcmpNFloat - for utChar, utInt, convert values to utFloat, then compare.
      //    fkcmpSNocase - for utString, case-insensitive comparison.
    bool k_eq(const unity& x, s_long flags) const;
    bool k_less(const unity& x, s_long flags) const;

      // Formatting + stream i/o.

    enum Eiofmt { iofmt_default = 0 };
    struct iofmt
    {
        // px (always != 0) represents the linked object during formatted output.
      const Eiofmt fmt; const unity* const px;
      iofmt(Eiofmt fmt_, const unity& x);
    private:
      typedef hashx<const void*, s_long> t_h;
      mutable cref_t<t_h> _ho;
      t_h& _rho() const;
      friend struct unity;
    };

      // Create specific format type, linked with the current object.
    iofmt format(Eiofmt fmt = iofmt_default) const;

      // Write text representation of x to stream. See also operator<<(ostream...)
    static void write_fmt(std::ostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp = 2);
    static void write_fmt(std::wostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp = 2);

      // Underlying map, hashlist access. Objects are implementation-dependent!
    inline t_map* _pmap() const { typedef t_map& R; return isMap() ? &R(*_m()) : 0; }
    t_hash* _phash() const { return isHash() ? _h() : 0; }

      // Pointer to function, returning pointers to private static methods (of BMDX) in the binary module
      //    that has created the current object. Value same as _pmsm() is returned by  unity_common::pls_modsm()
      //    of that binary module (main executable or shared library instance).
    unity_common::f_ls_modsm _pmsm() const __bmdx_noex { return pmsm; }


      // Construction.

    unity();
    unity(const unity& x);
    unity(const std::wstring& x);
    unity(const arrayref_t<wchar_t>& x);
    unity(const meta::s_ll& x);
    unity(const double& x);
    unity(const _unitychar& x);
    unity(const _unitydate& x);
      // lkt: ref. counting lock type:
      //    0 - do not use critical sections for object ref. counting,
      //    1 (default) - lock ref. counting one lock per type (Obj or T), actual in all translation units of the current binary module,
      //    2 - lock ref. counting with individual lock associated with the target object only (automatically kept together with the wrapped object).
      // See also set_obj.
    template<class Obj>
      unity(Obj& x, bool isStrongRef, s_long lkt = 1, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())        { ut = utEmpty; pmsm = unity_common::pls_modsm(); _data.p1 = 0; _data.p2 = 0; set_obj_atop(x, isStrongRef, lkt); }
    template<class T, class L>
      unity(const cref_t<T, L>& x, s_long lkt = 1, meta::noarg_tu_t<L> = meta::noarg_tu_t<L>())        { typedef cref_t<T, L> Obj; ut = utEmpty; pmsm = unity_common::pls_modsm(); _data.p1 = 0; _data.p2 = 0; Obj* p = new Obj(x); try { set_obj_atop(*p, true, lkt); } catch (...) { try { delete p; } catch (...) {} throw; } }
    template<class T>
      unity(const std::vector<T>& x, s_long arrlb, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())        { ut = utEmpty; pmsm = unity_common::pls_modsm(); _data.p1 = 0; _data.p2 = 0; *this = x; arrlb_(arrlb); }
    template<class T>
      unity(const T& x, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())        { ut = utEmpty; pmsm = unity_common::pls_modsm(); _data.p1 = 0; _data.p2 = 0; *this = x; }


      // NOTE The object may be destroyed in any binary module.
      //   If the object contains simple scalar value (not a string, and having no name),
      //   it may be safely destroyed even after the module where it's created is unloaded.
    ~unity() __bmdx_noex;



      // Clear / assign.
      //  NOTE Ops. in this group do not preserve the current object name.
      //    Assigning dest = src will copy both the name and value of src.
      //    Others just remove the current name.
      //    See also u_assign, u_clear.

      // Clears *this and sets it to utEmpty.
    void clear() __bmdx_noex;

      // Swaps *this and x on the binary level.
      // NOTE If *this and/or x are dynamically constructed, they must belong to the same binary module,
      //    otherwise causes undefined behavior on later object deletion.
      //    For other types of storage allocation, result of swap() is always stable.
    void swap(unity& x) __bmdx_noex;

      // Correctly deleting the current object from within binary module where it was dynamically constructed.
      //    true - the object is deleted successfully.
      //    false - the object deleted, but generated an exception during op. delete.
      // NOTE unity_delete() is equivalent to operator delete, executed in the original module on *this.
      //    1. Do not use it if the object was created in other way than dynamic construction (new unity(...)).
      //    2. Do not use it after the object was recreated (see recreate(), recreate_as()).
    bool unity_delete() __bmdx_noex;

      // Destroys previous contents and initializes the storage
      //    as an empty value created in the current binary module.
      // NOTE If unity is dynamically constructed (new unity(...)) in binary module A, and "recreated" in module B,
      //    it still must be deleted in module A in usual way ("delete p"). Do not use unity_delete().
    unity& recreate() __bmdx_noex;

      // Destroys previous contents and initializes the storage
      //    as an empty value created in the binary module of modsrc.
      // See also recreate().
    unity& recreate_as(const unity& modsrc) __bmdx_noex;

      // Returns non-constant reference to self.
      //  (Useful for modifying rvalues.)
    unity& _rnonc_self() const __bmdx_noex;

      // NOTE all assignments are transactional (on failure, the target object is not modified).
      // NOTE if src is utObject, operator=(const unity& src) copies a reference to that object, not its value.
      // NOTE operator=(const unity& src) copies src object name.
      //    operator=(any other type) clears target object name.
      //  Use combinations of u_clear, u_assign, u_rx, u_conv, ua_assign to keep the name.
      // NOTE Assignment never changes the target's native module association (see _pmsm()).
    unity& operator=(const unity& src);
    unity& operator=(const std::wstring&);
    unity& operator=(const arrayref_t<wchar_t>& src);
    unity& operator=(const meta::s_ll& src);
    unity& operator=(const double& src);
    unity& operator=(const _unitydate& src);
    unity& operator=(const _unitychar& src);

    template<class T, class _ = __vecm_tu_selector> struct _retcr_tu_t { const T& x; _retcr_tu_t(const T& x_) : x(x_) {} };

    template<class Ts> _retcr_tu_t<unity, meta::noarg_tu_t<Ts> > operator=(const std::vector<Ts>& src) { _x_asgvec(src, false); return *this; }
    template<class Ts> _retcr_tu_t<unity, meta::noarg_tu_t<Ts> > operator=(const vec2_t<Ts>& src) { _x_asgvec(src, false); return *this; }
    template<class T, class L> _retcr_tu_t<unity, meta::noarg_tu_t<T> > operator=(const cref_t<T, L>& src)        { typedef cref_t<T, L> Obj; Obj* p = new Obj(src); try { this->set_obj_atop(*p, true, 1); } catch (...) { try { delete p; } catch (...) {} throw; } return *this; }
    template<class T> _retcr_tu_t<unity, meta::noarg_tu_t<T> > operator=(const T& src)        { return (*this = trivconv_t<typename meta::nonc_t<T>::t>::F(src)); }




      // Get pointer to scalar, array, or array element.
      //  MODE This function does not generate exceptions or modify the object.
      //    To get non-0 pointer,
      //      1) utype() must exactly match the given type.
      //      2) In pval(ind), ind must be valid arrlb-based array index.
      //      3) If unity is utString, created in another binary module, it must be compatible with the current module.
      //    In all other cases, pval returns 0.
      //  For pval<utt>(ind), the given utt may be either array type or its element type.
      //    ind is arrlb-based.
      //  NOTE The returned pointer may be either checked or safely dereferenced (throws XUExec on 0).
    template<int utt> inline checked_ptr<typename valtype_t<utt>::t>
      pval(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { return utt == utype() ? _drf_c_i<utt>() : 0; }
    template<class T> inline checked_ptr<T>
      pval(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; return utt == utype() ? _drf_c_i<utt>() : 0; }
    template<int utt> inline checked_ptr<typename valtype_t<utt & utMaskNoArray>::t>
      pval(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template<class T> inline checked_ptr<T>
      pval(s_long ind, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template<int utt> inline checked_ptr<const typename valtype_t<utt>::t>
      pval(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                { return utt == utype() ? _drf_c_i<utt>() : 0; }
    template<class T> inline checked_ptr<const T>
      pval(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const                { enum { utt = reference_t<T>::utt }; return utt == utype() ? _drf_c_i<utt>() : 0; }
    template<int utt> inline checked_ptr<const typename valtype_t<utt & utMaskNoArray>::t>
      pval(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                { const typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template<class T> inline checked_ptr<const T>
      pval(s_long ind, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const                { enum { utt = reference_t<T>::utt }; const typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }



      // Get reference to scalar, array, or array element.
      //  MODE This function does not modify the object.
      //    Arguments are same as in pval.
      //    To get valid reference,
      //      1) utype() must exactly match the given type.
      //      2) In ref(ind), ind must be valid arrlb-based array index.
      //      3) If unity is utString, created in another binary module, it must be compatible with the current module.
      //
      //      4) Feature of ref<T>():
      //        if unity is utObject, and the contained object is
      //          a) valid object of type T,
      //          b) valid cref_t<T> to such object,
      //        a reference to object T is returned.
      //
      //        Object type matching:
      //          a) Both T and the contained object have same static type name.
      //              In this case, the returned reference is unconditionally valid only if the contained object
      //              has been created in the current binary module. If it is from other binary module,
      //              the returned reference may be compatible or incompatible, in part., sizeof(T) is not checked.
      //              The client is responsible for such checks (see also o_api::prefo).
      //
      //          b) Both T and the contained object have same cross-module type index
      //              NOTE Several types have cross-module indexes pre-declared:
      //                  char, double, s_long, meta::s_ll -- search for "cmti_base_t<char" in vecm_hashx.h to see.
      //                  unity, _unitydate, pcos etc. -- search for "cmti_base_t<bmdx::unity" in the current file to see.
      //                  threadctl, threadctl::tid -- in bmdx_config.h and bmdx_cpiomt.h.
      //              See also type_index_t in vecm_hashx.h.
      //
      //  If not (1..4), an exception is generated:
      //    XUTypeMismatch if utype() is not the same as given type.
      //    XUExec a) ind is out of range, b) failed to return reference to contained object.
    template<class T> inline T&        ref(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { return _rnonc<T>(); }
    template<class T> inline const T&        ref(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const                { return _rnonc<T>(); }
    template<int utt> inline typename valtype_t<utt>::t&        ref(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { typedef typename valtype_t<utt>::t T; if (utt != utype()) { throw XUTypeMismatch("ref", _tname0(utype()), _tname0(utt)); } return *checked_ptr<T>(_drf_c_i<utt>()); }
    template<int utt> inline const typename valtype_t<utt>::t&        ref(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                { typedef typename valtype_t<utt>::t T; if (utt != utype()) { throw XUTypeMismatch("ref", _tname0(utype()), _tname0(utt)); } return *checked_ptr<const T>(_drf_c_i<utt>()); }

    template<class T> inline T&        ref(s_long ind, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; return *ref<utt | utArray>().pc(ind); }
    template<class T> inline const T&        ref(s_long ind, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const                { enum { utt = reference_t<T>::utt }; return *ref<utt | utArray>().pc(ind); }
    template<int utt> inline typename valtype_t<utt & utMaskNoArray>::t&        ref(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { return *ref<utt | utArray>().pc(ind); }
    template<int utt> inline const typename valtype_t<utt & utMaskNoArray>::t&        ref(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                { return *ref<utt | utArray>().pc(ind); }



      // Get const reference to scalar, array, array element, or the default value.
      //    On type mismatch or array range error,
      //    a default value is created:
      //      a) T() - no conversion,
      //      b) T(x_dflt) - no conversion,
      //      c) crefx<utUnity>(ind) only: for any scalar type - dynamically created unity(ref(ind)) .
      //  MODE This function does not modify the object.
      //    cref() family is analogous to val(), but it does not make any copies when the contained object
      //    may be accessed directly by reference.
      //  NOTE If the object is from other binary module, and it contains incompatible string,
      //    the returned cref_t will reference a copy of that string, dynamically created in the current module.
      //  Possible exceptions: (rare) XUExec on alloc. error for the default value creation / copy.
    template<int utt> inline cref_t<typename valtype_t<utt>::t>
      cref(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (isString()) { try { cref_t<T> r; r.create1(false, val<utt>()); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create0(false); return r; } catch (...) {}
        throw XUExec("cref"); }
    template<int utt> inline cref_t<typename valtype_t<utt>::t>
      cref(const typename valtype_t<utt>::t& x_dflt, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (isString()) { try { cref_t<T> r; r.create1(false, val<utt>()); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create1(false, x_dflt); return r; } catch (...) {}
        throw XUExec("cref(x_dflt)"); }
    template<int utt> inline cref_t<typename valtype_t<utt>::t>
      cref(s_long ind, const typename valtype_t<utt>::t& x_dflt, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(ind); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (utype() == utStringArray) { try { cref_t<T> r; r.create1(false, val<utt>(ind)); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create1(false, x_dflt); return r; } catch (...) {}
        throw XUExec("cref(ind, x_dflt)"); }
    template<int utt> inline cref_t<typename valtype_t<utt>::t>
      crefx(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const        {
          typedef typename valtype_t<utt>::t T;
          const T* p = pval<utt>(ind); if (p) { return cref_t<T>(*p, false, false); }
          if (utt == utString) { if (utype() == utStringArray) { try { cref_t<T> r; r.create1(false, val<utt>(ind)); return r; } catch (...) {} } }
          if (utt == utUnity && utype() != utUnityArray) { try { cref_t<T> r; reinterpret_cast<cref_t<unity>*>(&r)->create1(false, _arr_el_get<utUnity>::F(this, ind, csLazy)); return r; } catch (...) {} }
          try { cref_t<T> r; r.create0(false); return r; } catch (...) {}
          throw XUExec("crefx"); }



      // Get reference to scalar or array with automatic type conversion.
      //  MODE This function may 1) generate exceptions, 2) modify the object. Modification is transactional.
      //    a) if the internal value matches the given type, returns a reference to the value.
      //      If the internal value is of another type, tries to convert it.
      //      On accessing an array element, the array must already exist,
      //      and be of either target type or utUnity. In case of utUnity,
      //      the contained value type converted to the required type automatically.
      //    b) on successful conversion, returns a reference to the value.
      //    c) if conversion has failed, XUConvFailed is generated. *this is not changed.
      //    d) if ind is out of range, XUExec is generated.
      // NOTE rx(), conv(), val() internally use common conversion function, always yielding same results for same original value.
      // NOTE Conversion removes the object name.
      // NOTE Array, created to replace non-array,  is 1-based by default.
    template<int utt> inline typename valtype_t<utt>::t&
      rx(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, false); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template<class T> inline T&
      rx(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, false); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template<int utt> inline typename valtype_t<utt & utMaskNoArray>::t&
      rx(s_long ind, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())
        {
          enum { utta = utt | utArray, utte = utt & utMaskNoArray }; typedef typename valtype_t<utte>::t T; enum { __z1 = sizeof(T) };
          const s_long uttx = utype(); if (uttx == utta) { return ref<utta>(ind); } else if (uttx == utUnityArray) { return _rx_el<utte>::F(ref<utUnity>(ind)); }
          throw XUConvFailed("rx(ind)", _tname0(utype_noarray()), _tname0(utt));
        }
    template<class T> inline T&
      rx(s_long ind, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())        { return rx<reference_t<T>::utt>(ind); }



      // Ensure that the value in *this is of the given type.
      //  MODES
      //    1) csStrict -- does not modify the object.
      //      If the given type != current object type, XUTypeMismatch is generated.
      //    2) csNormal -- transactional.
      //      a) The object is/converted to the given type with all data.
      //      b) The object is not changed, XUConvFailed is generated.
      //    3) csLazy -- non-transactional. No exceptions. The object
      //      a) is converted to the given type with all data,
      //      b) on allocation error, initialized by default for the given type (the data is lost),
      //      c) cleared (utEmpty).
      // NOTE rx(), conv(), val() internally use common conversion function, always yielding same results for same original value.
      // NOTE Conversion removes the object name.
      // NOTE Array, created to replace non-array,  is 1-based by default.
    template<int utt> inline bool
      conv(EConvStrength cs = csNormal, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { if (utt == utype()) { return true; } _x_cnv_this(cs, utt, false); return utt == utype(); }
    template<class T> inline bool
      conv(EConvStrength cs = csNormal, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return true; } _x_cnv_this(cs, utt, false); return utt == utype(); }


      // Copy the internal value, possibly with conversion to the target type.
      //    On accessing an array element, the array must already exist.
      //    utt may be
      //      a) scalar type -- approp. scalar is returned.
      //      b) array type -- vec2_t is returned.
      //      c) (array type | fvalVector) -- std vector is returned.
      //    b and c are useful when the current object is utUnityArray, whose elements
      //    are also arrays.
      //  MODES
      //    1) csStrict
      //      a) returns a copy of the internal value.
      //      b) if the given type != current object type, XUTypeMismatch is generated.
      //    2) csNormal
      //      a) returns a copy of the internal value.
      //      b) if the given type != current object type, performs copy + conversion.
      //      c) if copying or conversion fails, generates XUConvFailed.
      //    3) csLazy
      //      a) returns a copy of the internal value.
      //      b) on allocation error, returns the default value of the given type,
      //      c) on alloc. error even on the default value, generates XUConvFailed.
      // NOTE rx(), conv(), val() internally use common conversion function, always yielding same results for same original value.
    template<int utt> inline typename _valtype2_t<utt>::t
      val(EConvStrength cs = csNormal, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const
        {
          typedef typename _valtype2_t<utt>::t T;
          if (utt == utype())
          {
            if (utt < utString) { return _refc_el<utt>::F(*this); }
              // compatibility may be 0, because the array will be copied.
            if (isArray()) { if (!(_data.p1 && (_drf_q()->compatibility() >= 0))) { goto lCnvF; } }
            if (!(utt == utString && pmsm != unity_common::pls_modsm())) { try { return _refc_el<utt>::F(*this); } catch (...) {} goto lCnvF; }
          }
          if (true) { T retval; if (_x_cnv_this_val(cs, utt, (utt & fvalVector) ? 0x1 : 0, &retval)) { return retval; } }
        lCnvF:
          if (cs == csLazy) { try { return T(); } catch (...) {} }
          throw XUConvFailed("val", _tname0(utype()), _tname0(utt));
        }
    template<class T> inline T
      val(EConvStrength cs = csNormal, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const                { return val<reference_t<T>::utt>(cs); }
    template<int utt> inline typename _valtype2_t<utt>::t
      val(s_long ind, EConvStrength cs = csNormal, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const        { return _arr_el_get<utt>::F(this, ind, cs); }
    template<class T> inline T
      val(s_long ind, EConvStrength cs = csNormal, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const        { return _arr_el_get<reference_t<T>::utt>::F(this, ind, cs); }


    // NOTE The below v* functions are shorter version of val() + certain additional operations,
    //    dependent on type of the contained value.

      // >= 0 -- length of the contained string.
      //  -1 -- the contained value is empty (utEmpty).
      //  -2 -- the value is neither a string nor an empty.
      //  -3 -- (only if) this == 0 or the object is broken.
    meta::s_ll lenstr() const __bmdx_noex;

      // Wide string representation of a value.
      //    (If the value is itself a wide-character string, vstr returns a copy of that string.)
      //    NOTE vstr() values returned may be formatted differently than val<utString>().
      //  See also val() with utString.
    std::wstring vstr() const;
      std::wstring vstr(s_long ind) const;

      // Convenience functions.
      //  Return reference to the contained std::wstring, if possible. (ref<utString>(), ref<utString>(ind), ref<utUnity>(ind).rstr()).
    const std::wstring& rstr() const;
      const std::wstring& rstr(s_long ind) const;
    std::wstring& rstr();
      std::wstring& rstr(s_long ind);

      // 1-byte character string representation of a value.
      // If the value is wide-character string, the conversion is based on system-default locale (equiv. to std::setlocale(..., "")).
      //    See also wsToBs().
    std::string vcstr() const;
    std::string vcstr(s_long ind) const;

      // Integer representation of a value.
      //  See also val() with utInt.
    meta::s_ll vint() const;
    meta::s_ll vint(s_long ind) const;

      // Integer representation of a value - high doubleword only.
      //  See also val() with utInt.
    s_long vint_h() const;
    s_long vint_h(s_long ind) const;

      // Integer representation of a value - low doubleword only.
      //  See also val() with utInt.
    s_long vint_l() const;
    s_long vint_l(s_long ind) const;

      // Floating-point representation of a value - low doubleword only.
      //  See also val() with utFloat.
    double vfp() const;
    double vfp(s_long ind) const;

      // Short string representation of a value.
    _fls75 vflstr() const __bmdx_noex;





      // Array initialization. T is the type of element. utt is the type of element or array.
      //  If the array is already of the required type, it is simply cleared.
      //  MODES
      //    1) csStrict -- crears the current array, or generates XUTypeMismatch.
      //    2) csNormal -- transactional. Clears or initializes / the object to the target type. Otherwise generates an XUConvFailed.
      //    3) csLazy -- non-transactional. Clears or initializes / the object to the target type. On alloc. error, sets utEmpty and generates XUConvFailed.
      // arr_init<utt>:
      //  Both scalar and array type is accepted. Array element type is resolved automatically.
    template<int utt> vec2_t<typename valtype_t<utt & utMaskNoArray>::t>&
      arr_init(s_long arrlb, EConvStrength cs = csNormal, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())        { return arr_init<typename valtype_t<utt & utMaskNoArray>::t>(arrlb, cs); }
    template<class T> vec2_t<T>&
      arr_init(s_long arrlb, EConvStrength cs = csNormal, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())
      {
        enum { utt = reference_t<T>::utt | utArray };
        if (utt == utype())
        {
          vec2_t<T>* pv = _drf_c_i<utt>();
          if ((pv->compatibility() >= 0)) { pv->clear(); pv->vec2_set_nbase(arrlb); return *pv; }
          throw XUConvFailed("arr_init.1", "-", _tname0(utt), "incompat.");
        }
        else
        {
          if (cs == csStrict) { throw XUTypeMismatch("arr_init.2", _tname0(utype()), _tname0(utt)); }
          _stg_u st;
          if (_Lcreate(&st, utt | xfPtr, 0) > 0)
          {
            vec2_t<T>* pv = reference_t<vec2_t<T> >::deref(st, true);
            if ((pv->compatibility() >= 0)) { pv->vec2_set_nbase(arrlb); clear(); _data = st; ut = utt | xfPtr; return *pv; }
            _Ldestroy(&st, utt | xfPtr, 0);
          }
          throw XUConvFailed("arr_init.2", _tname0(utype()), _tname0(utt));
        }
      }


      // Array size and boundaries.
      //  If *this does not contain an array, XUTypeMismatch is generated.
    s_long arrlb() const; // lower bound (first existing element index).
    s_long arrub() const; // upper bound (largest existing element index).
    s_long arrsz() const; // the number of elements
    unity& arrlb_(s_long); // sets the new lower boundary (upper boundary shifts as well, because the array is not resized)
    unity& arrsz_(s_long); // sets the new (>=0) array size; lower bnd. is preserved
    unity& arrub_(s_long); // setting the new upper bnd., which may resize the array (if ==arrlb(), the array is cleared)

      // Objects, representing missing argument in various functions. Their value is ignored.
    static const unity _0;
    static unity _0nc;
    typedef const unity& _rcu;

      // Clears *this, then creates 1- or 0-based utUnityArray (resp. array, array0),
      //  then appends copies of all non-default arguments.
      //  Returns *this.
      // NOTE Setting an object as element of itself is ignored. The new value is an array of all passed values except the original value.
    unity& array(_rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0,
      _rcu x30=_0 );
    unity& array0(_rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0,
      _rcu x30=_0 );

      // Assigns ind-th elem. of the internal array = x, with conversion as necessary.
      //  NOTE Trivial conversions are allowed even on cs == csStrict.
      //  Returns: true on success, exception on failure, false on failure in csLazy.
    template<class Ts> bool arr_set(s_long ind, const Ts& x, EConvStrength cs = csNormal, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>())
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      try
      {
        if (pmsm == unity_common::pls_modsm() && uttd == utype_noarray()) { *pval<uttd>(ind) = trivconv_t<typename meta::nonc_t<Ts>::t>::F(x); return true; }
        if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_set.4", _tname0(uttd), _tname0(utype_noarray())); } }
        s_long res = _Lel_set(ind, x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_set.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_set.2", _tname0(utype()), ind);
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_set.3", _tname0(utype()), ind); }
    }

      // Appends x to the internal array, with conversion as necessary.
      //  NOTE Trivial conversions are allowed even on cs == csStrict.
      //  Returns: true on success, exception on failure, false on failure in csLazy.
    template<class Ts> bool arr_append(const Ts& x, EConvStrength cs = csNormal, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>())
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      try
      {
        if (pmsm == unity_common::pls_modsm() && uttd == utype_noarray()) { if (pval<uttd | utArray>()->el_append(trivconv_t<typename meta::nonc_t<Ts>::t>::F(x))) { return true; } throw XUExec("arr_append.5", _tname0(utype())); }
        if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_append.4", _tname0(uttd), _tname0(utype_noarray())); } }
        s_long res = _Lel_append(x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_append.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_append.2", _tname0(utype()));
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_append.3", _tname0(utype())); }
    }

      // Inserts m or removes -m (when m is negative) elements.
      //    On insertion, ind is the place of insertion, [lb..ub+1]. If x is not specified, a default value will be inserted.
      //    On removal, elements to be removed are [ind..ind-m-1]. All in this range must be valid element indices.
      //  Returns: true on success, exception on failure, false on failure in csLazy.
    bool arr_insrem(s_long ind, s_long m, EConvStrength cs = csNormal);
    template<class Ts> bool arr_insrem(s_long ind, s_long m, const Ts& x, EConvStrength cs = csNormal, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>())
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_insrem.4", _tname0(utype()), _tname0(utype_noarray())); } }
      try
      {
        if (pmsm == unity_common::pls_modsm() && uttd == utype_noarray())
        {
          s_long res;
          _yk_reg typename unity::valtype_t<uttd | utArray>::t& rv = ref<uttd | utArray>();
          if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, trivconv_t<typename meta::nonc_t<Ts>::t>::F(x)); } else { res = rv.el_insert_ml(ind, m, trivconv_t<typename meta::nonc_t<Ts>::t>::F(x)); } }
            else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } }
          if (res >= 0) { return true; }
          throw XUExec("arr_insrem.5", _tname0(utype()));
        }
        s_long res = _Lel_insrem(ind, m, x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_insrem.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_insrem.2", _tname0(utype()));
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_insrem.3", _tname0(utype())); }
    }



    //==  SCALARS, ARRAYS, NAME FIELD
    // u_*, sc*, ua*, map*, hash*, [ ]

    //-- Common

      // Returns a scalar value, copied from the current object.
      //    utype of the copy is <= utString. Special cases:
      //    a) array - "?array/" + array type is returned.
      //    b) object - "?object/" + typeid name is returned.
      //    c) object of type unity - "?unity/" + its u_name is returned.
      //    d) any other value "?value/" + its type name is returned.
      //  NOTE On cs == csLazy, the function does not throw exceptions
      //    even on error (just returns an empty value).
      //  NOTE result / destination object has no name.
    unity u_key(EConvStrength cs = csNormal) const;
    const unity& u_key_get(unity& dest, EConvStrength cs = csNormal) const; // returns *this

      // If this object has a name (scalar), its copy is returned.
      //  Otherwise, an empty object is returned.
      //  On copy error, XUExec is generated.
    unity u_name() const;

      // Sets the new object name. Before assignment, the name is scalarized by u_key.
    unity& u_name_set(const unity& name);

      // Does (save object name + clear() + conv<utt>(cs) + restore object name).
      //    a) If utt == -1, the object type does not change, except utype() == utObject.
      //    b) utt or utype() utObject removes the object reference, and sets *this to utEmpty.
      //    c) any other utt (not utObject, -1, utype()) produces new,
      //        valid default value of type utt.
      // flags:
      //    0x1 - clear array/map/hashlist in full (array base index = 1, assoc. array key fn. flags = 0, etc.).
      //      If flag is not set, only elements are removed, no container parameters changed.
      //      For assoc. array flags meaning, see static const s_long fkcmp*.
      // Returns:
      //    *this.
      // NOTE Unlike clear(), u_clear() may generate exceptions.
    unity& u_clear(s_long utt = -1, s_long flags = 0x1, EConvStrength cs = csNormal);

      // Same as copy constructor, but keeps object name.
    unity u_copy() const;

      // Same as op.=, but keeps object name.
    unity& u_assign(const unity& x);
    template<class T> unity&
      u_assign(const T& x, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())
      {
        typedef typename meta::assert<meta::same_t<T, unity>::result>::t_false __check; enum { __z1 = sizeof(__check) };
        if (isNamed())
        {
          if (pmsm != unity_common::pls_modsm()) { _x_asg(x, 0x1); return *this; }
          unity x2(x); if (!x2.isByPtr()) { _x_asg(x2, 0x1); return *this; }
            void* _pn = (ut & xfPtr) ? _data.p2 : 0; if (_pn) { _data.p2 = 0; }
          clear(); _data.p1 = x2._data.p1; ut = x2.utype() | xfPtr; x2._data.p1 = 0; x2.ut = utEmpty;
            if (_pn) { _data.p2 = _pn; }
        }
        else { return (*this = x); }
        return *this;
      }

      // Same as rx(), but keeps object name.
    template<int utt> inline typename valtype_t<utt>::t&
      u_rx(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, true); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template<class T> inline T&
      u_rx(meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, true); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }

      // Same as conv(), but keeps object name.
    template<int utt> inline bool
      u_conv(EConvStrength cs = csNormal, meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>())                { if (utt == utype()) { return true; } _x_cnv_this(cs, utt, true); return utype() == utt; }
    template<class T> inline bool
      u_conv(EConvStrength cs = csNormal, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>())                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return true; } _x_cnv_this(cs, utt, true); return utt == utype(); }


    //-- Scalar

    bool sc_empty() const;
    unity& sc_ensure();
    unity& sc_clear();


    //-- Array

      // By default, all ua_*, except const, automatically create 1-based utUnityArray instead of non-array.
      //    ua, ua_val, ua_set automatically add dflt.-constructed elements to front or back of the array
      //    to reach the specified index.

    unity& ua_ensure();
    unity& ua_clear(bool full);
    template<class Ts> unity& ua_assign(const std::vector<Ts>& src, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>()) { _x_asgvec(src, true); return *this; }
    template<class Ts> unity& ua_assign(const vec2_t<Ts>& src, meta::noarg_tu_t<Ts> = meta::noarg_tu_t<Ts>()) { _x_asgvec(src, true); return *this; }

    bool ua_has(s_long ind) const;
    bool ua_canref(s_long ind) const;
    bool ua_empty(s_long ind) const;

    unity& ua(s_long ind);
    unity& ua_first();
    unity& ua_last();
    unity ua_val(s_long ind);
    unity ua_val_first();
    unity ua_val_last();
    unity& ua_set(s_long ind, const unity& v); // returns *this
    unity& ua_append(const unity& v); // returns *this
    s_long uaS();
    s_long uaLb();
    s_long uaUb();
    unity& uaS_set(s_long new_S);
    unity& uaS_set(s_long new_S, const unity& v);
    unity& ua_resize(s_long ind, s_long m); // see arr_insrem for ind, m
    unity& ua_resize(s_long ind, s_long m, const unity& v); // see arr_insrem for ind, m
    unity& uaLb_set(s_long new_L);
    unity& ua_fill(s_long utt, const unity& v = unity()); // -"-; creates new array with same bounds as existing, + fills with v; on err. gen. exc., the object is not changed


    //-- Hashed ordered map

    unity& map_ensure(); // returns *this
    unity& map_clear(bool full); // true resets flags and removes elements (object name is kept), false only removes elements; returns *this
    unity map_keys(s_long lb);
    unity& map_keys_get(unity& dest, s_long lb);  // returns *this
    unity map_values(s_long lb);
    unity& map_values_get(unity& dest, s_long lb);  // returns *this
      // NOTE like all non-const map*, autoconverts to utMap, see also assocFlags_c.
    s_long mapFlags();
    void mapFlags_set(s_long fk_reset, s_long fk_set);

    bool map_empty(const unity& k) const;
    bool map_locate(const unity& k, bool insert = false);
    s_long map_noel() const { return hashx_common::no_elem; }
    bool map_locate(const unity& k, bool insert, s_long& ind); // ind == map_noel() if not found on insert == false
    unity& map(const unity& k); // insert { k, utEmpty } if necessary, return ref. to value with key k
      // Returns: true if k was inserted, false if it existed.
      //  NOTE The name map_append means only that inserting pre-sorted keys into the map is optimized.
      //    map_append always inserts any k in the set of existing keys at the place, defined by comparison kf_unity::less12.
      // See also paramline::list_m.
    bool map_append(const unity& k, const unity& v, bool keep_first = false);
    bool map_del(const unity& k); // true - removed, false - not existed

    s_long mapS();
    unity& mapS_set(s_long new_S); // returns *this

      // In mapi*, ind is 1-based. Valid inds are [1..mapS()].
      //  NOTE Key, value functions generate an exception if ind is out of range.
    bool mapi_has(s_long ind) const; // true if ind is correct
    bool mapi_empty(s_long ind) const; // true if ind is correct and the value is empty
    const unity& mapi_key(s_long ind); // returns key at 1-based ind
    unity& mapi(s_long ind); // returns value at 1-based ind
    unity& mapi_del(s_long ind); // returns *this




    //-- Hashlist

    unity& hash_ensure(); // returns *this
    unity& hash_clear(bool full); // true resets flags and removes elements (object name is kept), false only removes elements; returns *this
    unity hash_keys(s_long lb);
    unity& hash_keys_get(unity& dest, s_long lb);  // returns *this
    unity hash_values(s_long lb);
    unity& hash_values_get(unity& dest, s_long lb);  // returns *this
      // NOTE fkcmpRevOrder is ignored by hashlist object.
      // NOTE like all non-const hash*, autoconverts to utHash, see also assocFlags_c.
    s_long hashFlags();
    void hashFlags_set(s_long fk_reset, s_long fk_set);

    bool hash_empty(const unity& k) const;
    bool hash_locate(const unity& k, bool insert = false);
    unity& hash(const unity& k);
      // keep_first: true keeps existing value if k already exists in the hashlist.
      // pos_before: list element, before which to make insertion of (k, v) pair if k did not exist.
      //    May be
      //      a) hashl_noel() - dflt., end of list.
      //      b) in range [0..hashS()-1] - pos. as returned by hashl_pos_c().
      // Returns: true if k was inserted, false if it existed.
    bool hash_set(const unity& k, const unity& v, bool keep_first = false, s_long pos_before = hashx_common::no_elem);
    bool hash_del(const unity& k);

    s_long hashS();
    s_long hashS_c() const;

      // In hashi*, ind is 1-based. Valid inds are [1..hashS()].
      //  NOTE Key, value functions generate an exception if ind is out of range.
      //  NOTE Access by index implicitly creates entries order vector.
      //    If access is mixed with hashlist modification, use hashl family.
    bool hashi_has(s_long ind) const; // true if ind is correct
    bool hashi_empty(s_long ind) const; // true if ind is correct and the value is empty
    const unity& hashi_key(s_long ind); // returns key at 1-based ind
    const unity& hashi_key_c(s_long ind) const; // returns key at 1-based ind
    unity& hashi(s_long ind); // returns value at 1-based ind
    const unity& hashi_c(s_long ind) const; // returns value at 1-based ind

      // hashl*: access to hashlist pairs (k, v) in the list order. By default, the order that of keys insertion.
      // In hashl*, position is 0-based.
      //    Next, prev.: valid pos is one of (hashl_noel(),  [0..hashS()-1]). If invalid pos. given, hashl_noel() is returned.
      //    Key, value: valid pos. is [0..hashS()-1]. If invalid pos. given, an exception is generated.
      //  Special value hashl_noel() (negative number) serves as single artificial position for at once "before-beginning" and "after-end".
      // NOTE When an element (any except the last one) is deleted from the hashlist,
      //    position of the last element in hashlist is changed:
      //    the new position will be that of the deleted element.
      //    List order does not change anyway.
      //    The client should use the following:
      //      1. If the current position pos is being deleted, store pos0 = prev(pos).
      //      2. Delete key(pos).
      //      3. The next list element to be processed is next(pos0).
      //    (When new (k, v) pair is inserted, all numeric positions of the existing elements are kept.)
    s_long hashl_noel() const { return hashx_common::no_elem; }
    s_long hashl_first();
    s_long hashl_last();
    s_long hashl_next(s_long pos);
    s_long hashl_prev(s_long pos);
    const unity& hashl_key(s_long pos);
    unity& hashl(s_long pos);
    s_long hashl_pos_c(const unity& k) const; // returns pos. of k, or hashl_noel() if k not found



    //-- map, hashlist, array elem. access, [ ], path

      // u_has returns true if the *this is of one of the types, specified by tt,
      //  AND it contains key or index ki.
      //  tt should contain ORed flags, meaning:
      //      1: utUnityArray,
      //      2: utMap,
      //      4: utHash,
      //      8: allow forwarding to contained object in case of utObject / unity.
      //  ki is expected to be
      //      a) an array index: utInt or value of other type, convertible to integer.
      //      b) an associative array key: any kind of value.
      //  If (*this)[ki] won't return valid (const unity&) for any reason,
      //    u_has returns false, and does not generate exceptions.
    bool u_has(const unity& ki, s_long tt) const;

      // operator[] returns, if found type/key/array index match, one of the following:
      //    a) ref. to ki-th array element,
      //    b) map(ki),
      //    c) hash(ki),
      //    d) contained unity object's [ki].
      //  Otherwise, XUExec is generated.
      // NOTE 1. operator[] does not change the type of the internal container.
      //  2. operator[] does not automatically insert associative array keys or normal array elements.
    unity& operator[] (const unity& ki);
    const unity& operator[] (const unity& ki) const;

      // Return a pointer to the element in a branch of the tree, consisting of maps, hashlists, arrays.
      //  keylist: path to the element,
      //    a) as string in paramline array format (with "|" char. before each element; leading "|" or "=|" is optional),
      //    b) (path(unity&) only) as array of path elements.
      // On error, null pointer is returned: a) path not found, b) array indexing with non-number, c) mem. alloc. error.
    checked_ptr<const unity> path(const unity& keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const arrayref_t<wchar_t>& keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const arrayref_t<char>& keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const std::wstring& keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const std::string& keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const wchar_t* keylist) const __bmdx_noex;
    checked_ptr<const unity> path(const char* keylist) const __bmdx_noex;

    checked_ptr<const unity> path(const unity& keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const arrayref_t<wchar_t>& keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const arrayref_t<char>& keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const std::wstring& keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const std::string& keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const wchar_t* keylist, const unity& x_dflt) const __bmdx_noex;
    checked_ptr<const unity> path(const char* keylist, const unity& x_dflt) const __bmdx_noex;

      // Return a pointer to the element in a branch of the tree, consisting of maps, hashlists, arrays,
      //  with autocreation of the given path.
      //  keylist: path to the element,
      //    a) as string in paramline array format (with "|" char. before each element; leading "|" or "=|" is optional).
      //    b) (path_w(unity&) only) as array of path elements.
      //  path_w automatically creates the given path, if it does not exist.
      //  If branch element is empty (e.g. just created), and is indexed with the key which is utInt,
      //      the branch element is automatically converted to array, and resized to make the index valid.
      //      With other key types, such branch element is converted to hashlist,
      //      and only one element is inserted (the key with and empty value).
      // On error, null pointer is returned: a) existing array indexing with non-number, b) mem. alloc. error.
    checked_ptr<unity> path_w(const unity& keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const arrayref_t<wchar_t>& keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const arrayref_t<char>& keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const std::wstring& keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const std::string& keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const wchar_t* keylist) __bmdx_noex;
    checked_ptr<unity> path_w(const char* keylist) __bmdx_noex;



    //-- Common associative array functions (for map and hashlist)

      // assoc* functions work both with maps and with hashlists,
      //    without changing the container type. The container must be map or hashlist.
      //    For maps, list of key/value pairs is iterated in natural sort order,
      //    numeric positions in assocl_* and index values in mapi_* are the same.
    bool assoc_set(const unity& k, const unity& v, bool keep_first = false);
    bool assoc_del(const unity& k);

    s_long assocS_c() const;
      // Returns associative array (utHash or utMap) flags.
      //  If *this is of other type, an exception is generated.
    s_long assocFlags_c() const;

      // NOTE assocl_noel() is returned by assocl_ first, last, next, prev if *this is not map or hashlist.
    s_long assocl_noel() const { return hashx_common::no_elem; }
    s_long assocl_first() const;
    s_long assocl_last() const;
    s_long assocl_next(s_long pos) const;
    s_long assocl_prev(s_long pos) const;

    const unity& assocl_key(s_long pos) const;
    unity& assocl(s_long pos);
    const unity& assocl_c(s_long pos) const;

    #if bmdx_part_unity_oppar_assoc

        // Convenience function for building associative arrays.
        //  Returns map(k) if the current object is utMap, or hash(k) in all other cases.
      unity& operator()(const unity& k);

        // Convenience function for building associative arrays.
        //  Performs map_append(k, v) if the current object is utMap, or hash_set(k, v) in all other cases.
        // Returns *this.
        // See also paramline :: list_m, list_h.
      unity& operator()(const unity& k, const unity& v);

    #endif


      // Convenience functions for paramline :: decode, decode_tree, decode1v.
      //  Return *this.
    unity& pl_dech(arrayref_t<wchar_t> ssrc); // decodes into utHash
    unity& pl_decm(arrayref_t<wchar_t> ssrc); // decodes into utMap
    unity& pl_dech_tree(arrayref_t<wchar_t> ssrc, s_long flags = 0); // decodes into utHash (0x1 flag is ignored)
    unity& pl_decm_tree(arrayref_t<wchar_t> ssrc, s_long flags = 0); // decodes into utMap (0x1 flag is ignored)
    unity& pl_dec1v(arrayref_t<wchar_t> ssrc); // decodes a value, which is scalar or 1-dim. 1-based utUnityArray

    unity& pl_dech(arrayref_t<char> ssrc);
    unity& pl_decm(arrayref_t<char> ssrc);
    unity& pl_dech_tree(arrayref_t<char> ssrc, s_long flags = 0);
    unity& pl_decm_tree(arrayref_t<char> ssrc, s_long flags = 0);
    unity& pl_dec1v(arrayref_t<char> ssrc);

      // Convenience functions for paramline :: encode, encode_tree, encode1v.
    std::wstring pl_enc() const;
    std::wstring pl_enc_tree() const;
    std::wstring pl_enc1v() const;



    //==  REFERENCED OBJECTS (utObject) AND INTERFACES


    struct o_api;
    template<class Obj, class _ = __vecm_tu_selector> struct _o_sm_t;
    template<class Itfs, class _ = __vecm_tu_selector> struct _o_itfssm_t;


      // A) set_obj with x that is different from the current contents of the unity object.
      //    a) On b_strong == true, if the function succeeds, it takes ownership on x.
      //    b) On b_strong == false, the client owns x.
      //    c) If the function fails, it generates an exception, and the client continues to own x.
      //        In this case, the previous contents  of the unity object are not modified.
      // lkt: ref. counting lock type:
      //    0 - do not use critical sections for object ref. counting,
      //    1 (default) - lock ref. counting one lock per type (Obj), actual in all translation units of the current binary module,
      //    2 - lock ref. counting with individual lock associated with the target object only (automatically kept together with the wrapped object).
      // NOTE Binary safety:
      //    The object x must have been created in the current binary module.
      //    For objects created elsewhere, assign their wrapping unity objects instead of direct calling set_obj.
      // NOTE Multithreading for lkt 1 and 2:
      //    1. Only reference counting on the wrapping unity object copying and modification
      //      is thread-safe.
      //    2. The wrapping unity object itself should may be modified by more than one thread at a time.
      //    3. The wrapping unity object may not be accessed and at once  cleared or overwritten
      //      under the target object's  lock (o_api::critsec_rc(o_api(&wrapping object).prc)).
      //      Use critsec_t to additionally protect in such case.
      //    4. On crossing binary boundary, the wrapping unity object
      //      must be
      //      a) protected by some process-global  lock if used by reference,
      //      or b) copied by value, so that each involved binary module
      //      would have its own copy of the wrapping object, referring to the same target (wrapped) object.
      //      In case (b), modifying the wrapping objects in different modules is safe,
      //      and o_api::critsec_rc is itself sufficient process-global lock for protecting the wrapped object.
      //    5. For any shared library, the client is responsible for preventing that library unloading,
      //      until all objects created there and passed into other binary modules (directly or in wrapped form) are released.
      // B) set_obj with the same x as contained in the current unity object.
      //    This just updates reference type.
      //    The new ref. type will be as specified by b_strong.
      //    lkt is ignored, the present type of lock persists.
      //    a) If the new type of reference is same as current, the function does nothing.
      //    b) If the type of reference has changed from strong to weak, and the last strong ref. disappeared, the object will be deleted.
      //    c) If the type of reference has changed from weak to strong, and this is the first strong ref., unity object takes ownership on x.
      //    d) If the object x has been deleted before set_obj, the function works as if x is a valid object.
      //      Specify b_strong = false in this case, to prevent any access except storing the invalid pointer.
    template<class Obj> unity&
      set_obj(Obj& x, bool b_strong, s_long lkt = 1, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())
          {
            o_api a(this);
            o_api::critsec_rc __lock(a.prc); if (sizeof(__lock)) {}
            if (isObject() && a.pvoid(true) == &x)
            {
              bool b0 = !!(ut & xfObjStrongRef); if (b_strong == b0) { return *this; }
              s_long res = a.rc_incdec(b_strong ? (1+8) : (2+4)); if (!res) { throw XUExec("set_obj[1]"); }
              a.upd_ut_ref_type(b_strong);
              return *this;
            }
            _o_refcnt* prc = _o_sm_t<Obj>::rc_create(&x, b_strong ? 1 : 2, lkt); if (!prc) { throw XUExec("set_obj[2]"); }
            clear(); _data.p1 = prc; _data.p2 = 0; ut = utObject | xfPtr; if (b_strong) { ut |= xfObjStrongRef; }
            return *this;
          }

      // NOTE Itfs should have a dependence from the current translation unit anon. namespace,
      //    to avoid incorrect aliasing of virtual and static functions.
      //    See detailed note near objItfsAttach.
      // NOTE The object x must have been created in the current binary module.
      //    Please copy/assign references by means of assigning the wrapping unity objects.
    template<class Obj, class Itfs> unity&
      set_obj(Obj& x, bool b_strong, s_long lkt = 1, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())
          {
            o_api a(this);
            o_api::critsec_rc __lock(a.prc); if (sizeof(__lock)) {}
            if (isObject() && a.pvoid(true) == &x)
            {
              bool b0 = !!(ut & xfObjStrongRef); if (b_strong == b0) { return *this; }
              s_long res = a.rc_incdec(b_strong ? (1+8) : (2+4)); if (!res) { throw XUExec("set_obj[3]"); }
              a.upd_ut_ref_type(b_strong);
              return *this;
            }

            unity u2; u2.pmsm = pmsm;
            _o_refcnt* prc = _o_sm_t<Obj>::rc_create(&x, 2, lkt); if (!prc) { throw XUExec("set_obj[4]"); }
            u2.clear(); u2._data.p1 = prc; u2._data.p2 = 0; u2.ut = utObject | xfPtr;
            o_api a2(&u2);

            s_long res = a2.itfslist_create(); if (res < 0) { throw XUExec("set_obj[5]"); }
            res = a2.itfs_attach<Itfs>(); if (res < 0) { throw XUExec("set_obj[6]"); }
            if (b_strong) { res = a2.rc_incdec(1+8); if (res < 0) { throw XUExec("set_obj[7]"); } }
            a2.upd_ut_ref_type(b_strong);
            this->swap(u2);
            return *this;
          }

      // set_obj_atop automatically attaches o_interfaces_top<Obj> if it's not empty.
      //  Otherwise, set_obj<Obj> is called, no interfaces attached (faster, less memory used).
    template<class Obj> unity&
      set_obj_atop(Obj& x, bool b_strong, s_long lkt = 1, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())
      {
        if (o_interfaces_top<Obj>::__xx_icnt) { return set_obj<Obj, o_interfaces_top<Obj> >(x, b_strong, lkt); }
        return set_obj<Obj>(x, b_strong, lkt);
      }

    template<class T, class _ = __vecm_tu_selector> struct objt_binder_t
    {
      unity& x; bool ne; bool atop; s_long lkt;
      objt_binder_t(unity& x_, bool ne_, bool b_atop_, s_long lkt_) : x(x_), ne(ne_), atop(b_atop_), lkt(lkt_) {}
      bool operator()() { T* p(0); try { p = new T(); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(0)"); } return false; }
      template<class A1> bool operator()(const A1& x1)        { T* p(0); try { p = new T(x1); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(1)"); } return false; }
      template<class A1, class A2> bool operator()(const A1& x1, const A2& x2)        { T* p(0); try { p = new T(x1, x2); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(2)"); } return false; }
      template<class A1, class A2, class A3> bool operator()(const A1& x1, const A2& x2, const A3& x3)        { T* p(0); try { p = new T(x1, x2, x3); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(3)"); } return false; }
      template<class A1, class A2, class A3, class A4> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { T* p(0); try { p = new T(x1, x2, x3, x4); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(4)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(5)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(6)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6, x7); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(7)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6, x7, x8); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(8)"); } return false; }

            // Same as operator(), but args. are cast to non-const when passed to constructor.
          template<class A1> bool createnc(const A1& x1)        { T* p(0); try { p = new T((A1&)x1); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(1)"); } return false; }
          template<class A1, class A2> bool createnc(const A1& x1, const A2& x2)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(2)"); } return false; }
          template<class A1, class A2, class A3> bool createnc(const A1& x1, const A2& x2, const A3& x3)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(3)"); } return false; }
          template<class A1, class A2, class A3, class A4> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(4)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(5)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(6)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(7)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); if (atop) { x.set_obj_atop(*p, true, lkt); } else { x.set_obj(*p, true, lkt); } return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(8)"); } return false; }
    };

      // Object creation with 0..6 arguments. The object will be strong-referenced.
      //    Example:         unity z; z.objt<std::string>(0)(' ', 25);
      //    On success, the previous unity contents are correctly removed, and the new object is set on their place.
      //    On failure, the previous contents are not changed.
      // no_exc: objt_binder_t operator() behavior in case if object construction or interfaces attachment fails:
      //    true: return false.
      //    false: generate an exception.
      // lkt: ref. counting lock type:
      //    0 - do not use critical sections for object ref. counting,
      //    1 (default) - lock ref. counting one lock per type (Obj), actual in all translation units of the current binary module,
      //    2 - lock ref. counting with individual lock associated with the target object only (automatically kept together with the wrapped object).
      //  See set_obj for detailed info on how locks type 1 and 2 work.
      // b_atop:
      //    true - on object creation, attach the default set of interfaces for T to the new object.
      //      See also struct o_interfaces_top.
      //    false (default) - create wrapped object T only, do not create any structures related to interfaces.
    template<class T> objt_binder_t<T>
      objt(bool no_exc, s_long lkt = 1, bool b_atop = false, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) __bmdx_noex
          { return objt_binder_t<T>(*this, no_exc, b_atop, lkt); }

      // Returns object pointer !=0 if unity references a valid (not deleted) object.
    void* cpvoid() const;

      // Returns object pointer !=0 if unity references an object or still contains a pointer to the deleted object.
    void* cpvoidkey() const;

      // Returns a pointer to the object, wrapped by this unity object.
      //  b_checked:
      //    a) true: return non-0 if unity references a valid (not deleted) object of the same static type as Obj.
      //    b) false: return non-0, if unity references any object (utype == utObject) in any state, even if deleted.
      //      This is the same as (Obj*)cpvoidkey().
      //  b_allow_cm:
      //    a) true: only if b_checked true, allow returning non-0 pointer to an object from another binary module, if its vecm type index is the same as for Obj.
      //      (See also type_index_t in vecm_hashx.h)
      //    b) false: do not modify behavior of b_checked flag.
    template<class Obj> Obj* objPtr(bool b_checked = true, bool b_allow_cm = false, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())        { o_api a(this); Obj* p = a.pobj<Obj>(b_checked ? 0x21 : 0x78); if (!p && b_checked && b_allow_cm) { p = a.pobj<Obj>(0x44); } return p; }
    template<class Obj> const Obj* objPtr_c(bool b_checked = true, bool b_allow_cm = false, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const        { o_api a(this); const Obj* p = a.pobj<Obj>(b_checked ? 0x21 : 0x78); if (!p && b_checked && b_allow_cm) { p = a.pobj<Obj>(0x44); } return p; }

      // Analogous to objPtr,
      //    a) returns valid ref. to Obj,
      //    b) generates XUExec if *this contains no object or the object is of wrong type or in wrong state.
      // NOTE Same as "T& ref = x.objRef<T>();", one may use "T& ref = +x;".
      //    See also: unity operator+ and unitypc operator T&.
    template<class Obj> Obj& objRef(bool b_checked = true, bool b_allow_cm = false, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())        { return *checked_ptr<Obj>(this->objPtr<Obj>(b_checked, b_allow_cm)); }
    template<class Obj> const Obj& objRef_c(bool b_checked = true, bool b_allow_cm = false, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const        { return *checked_ptr<Obj>(const_cast<unity*>(this)->objPtr<Obj>(b_checked, b_allow_cm)); }

      // Search direction in the list of interface sets: from end (larger inds, latest attached sets) to the beginning (smaller inds).
      //  Search direction inside an interface set: from beginning (leftmost listed in o_interfaces<...>) to end (rightmost).
      // NOTE o_iptr_t is not thread-safe or ref.-counted.
      //    The referenced interface object must not be deleted in any way during client use of o_iptr_t.
      // See also o_api::pinterface.
    template<class I> o_iptr_t<I>
      pinterface(meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const
          { return o_api(this).pinterface<I>(0x1f); }

      // flags OR-ed:
      //  1 - get static type info,
      //  2 - get dynamic type info,
      //  4 - get vecm type index (see type_index_t in vecm_hashx.h),
      //  8 - return _pobj even if the object is deleted.
      //  16 - get sizeof for object static type.
    o_type_info objTypeInfo(s_long flags) const __bmdx_noex;

    o_ref_info objRefInfo() const __bmdx_noex;

      // Returns true on success (ref. type set, or not changed if new == old), false on failure.
    bool objSetStrongRef(bool b_strong);

      // Creates a set of interfaces (new _o_itfs_topmost<Itfs>()) and attaches it to object (adds to interface sets list of the wrapping unity object).
      //  All interfaces get __itf_after_construct notification.
      // Returns:
      //    1 - the set is added to the existing list of sets.
      //    0 - the set of such static type already exists.
      //    -1 - pidyn == 0 (list of interf. sets is not created - call itfslist_create() first).
      //    -2 - failure.
      // NOTE If Itfs is a template with one of arguments belonging to anonymous namespace,
      //    (like in o_interfaces_top template), or a class in an anon. namespace,
      //    o_iimpl of all interfaces in the set
      //    is created exactly belonging to the current translation unit.
      //  In contrary, if Itfs is common template or a class in named or global namespace,
      //    its constructor may execute in another binary module (depending on of compiler aliasing rules).
      //    Thus o_iimpl objects will execute different versions of their virtual functions
      //    than may be expected by the current translation unit code.
      //    It's recommended to avoid this situation,
      //    and always use o_interfaces_top<Obj> as Itfs,
      //    or another class or template with explicit dependence from current translation unit anon. namespace.
    template<class Itfs> s_long
      objItfsAttach(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>())
          {
            o_api a(this);
            if (!a.prc) { return -2; }
            o_api::critsec_rc(a.prc);
            if (!a.pidyn) { s_long res = a.itfslist_create(); if (res < 0) { return -2; } }
            return a.itfs_attach<Itfs>();
          }

      // 1 - success, -1 - not found, -2 - failure.
    template<class Itfs> s_long
      objItfsRemove(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>())
          {
            o_api a(this);
            o_api::critsec_rc(a.prc);
            s_long ind = a.find_local_by_itfs<Itfs>(1); if (ind < 0) { return ind; }
            return a.itfs_remove_ind(ind);
          }

      // 1 - success, -1 - wrong ind, -2 - failure.
    s_long objItfsRemoveInd(s_long ind);

      // 1 - success, -1 - not found, -2 - failure.
    template<class I> s_long
      objItfRemoveLast(meta::noarg_tu_t<I> = meta::noarg_tu_t<I>())
          {
            o_api a(this);
            o_api::critsec_rc(a.prc);
            s_long ind = a.find_by_itf<I>(0x1f); if (ind < 0) { return ind; }
            return a.itfs_remove_ind(ind);
          }

      // >= 0 - index of the found set. -1 - not found. -2 - failure.
    template<class Itfs> s_long
      objItfsFind(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const
          { return o_api(this).find_local_by_itfs<Itfs>(1); }

      // >= 0 - index of the found set. -1 - not found. -2 - failure.
    s_long objItfsFindPtr(o_itfset_base* p_itfs) const;

      // >= 0 - index of the found set. -1 - not found. -2 - failure.
    template<class I> s_long
      objItfFindLast(meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const
          { return o_api(this).find_by_itf<I>(0x1f); }

      // >= 0 - index of the first matching set (last attached set that can be cast into I). -1 - not found. -2 - failure.
      //  If pret != 0, indexes of all matching sets are pushed into it.
    template<class I> s_long
      objItfFindAll(std::basic_string<s_long>* pret, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const
          { return o_api(this).find_by_itf<I>(0x1f, 0, pret); }

      // ind = [0 .. objItfsN()-1]
    o_itfset_base* objItfsByInd(s_long ind) const;

      //  a) the number of currently attached interface sets (>=0).
      //  b) -2 on failure (unity does not contain an object).
    s_long objItfsN() const;


      // Full-scale interface for manipulating referenced objects.
    struct o_api
    {
        // a) pu, 0, 0 // pu is not utObject
        // b) pu, prc, 0 // no dynamically attached interfaces
        // c) pu, prc, pdyn
        // d) 0, prc, pdyn // partial access, o_api created by one of attached interfaces based on its _o_itfslist backreference
        // e) 0, 0, 0
      unity* pu;
      _o_refcnt* prc;
      _o_itfslist* pidyn;

      friend struct unity; friend struct cv_ff; friend struct _static_conv;
      typedef void* (*__Posm_pvoid)(_o_refcnt* prc, s_long b_checked);
      typedef void* (*__Posm_prefo)(_o_refcnt* prc, const char* tname, s_long t_ind, s_long t_size);
      typedef void (*__Posm_type_info)(_o_refcnt* prc, o_type_info*, s_long flags);
      typedef void (*__Posm_ref_info)(_o_refcnt* prc, o_ref_info*, s_long flags);
      typedef void (*__Posm_lock_create)(meta::s_ll* pstg, _o_refcnt* prc, s_long flags);
      typedef void (*__Posm_lock_destroy)(meta::s_ll* pstg);
      typedef s_long (*__Posm_lock_state)(const meta::s_ll* pstg);
      typedef o_itfset_base* (*__Pitfs_new_copy)(o_itfset_base* p_itfs);
      typedef void (*__Pitfs_delete)(o_itfset_base* p_itfs);
      typedef s_long (*__Pitfs_xx_after_construct)(o_itfset_base* p_itfs, o_itfset_base* p_itfs_src, s_long flags);
      typedef void (*__Pitfs_xx_before_destroy)(o_itfset_base* p_itfs, s_long flags);
      typedef void (*__Pitfs_type_info)(o_itfset_base* p_itfs, o_type_info* pret, s_long flags);
      typedef s_long (*__Prc_incdec)(_o_refcnt* prc, s_long flags);

        // The lock that is used for all operations on unity/utObject,
        //    mainly to protect object ref. counting and interfaces add/rem./search.
        //    If the object was wrapped into unity object without locking,
        //    critsec_rc on such object will do nothing (see also set_obj and objt).
        //  NOTE The lock critsec_rc may be created by the client (from o_api::prc)
        //    to ensure sequence of >=2 operations on unity/utObject in one thread
        //    being not mixed with calls on the same unity/utObject from other threads.
        //    (!) Even under critsec_rc, the wrapping unity
        //    may NOT be cleared, deleted or assigned another value
        //    by any other thread than one that has set this lock.
      struct critsec_rc
      {
        enum { n = 4 }; // based on sizeof(critsec_t) + reserve.
        union { unity_common::__Psm posm; meta::s_ll __pad1; };
        meta::s_ll stg[n];

          // Returns lock state/type:
          //  2 - lock object is valid, lock type - per-object.
          //  1 - lock object is valid, lock type - per-type.
          //  0 - lock object is valid, lock type == no lock.
          //  -1 - lock object is not valid (e.g. after critsec_rc(0)).
          // NOTE Ret. vals. 0..2 are same as unity::objt lkt parameter.
        s_long state() const;

          // If the client has unity x, containing an object, the lock is initialized this way:
          //  o_api __a(&x); critsec_rc __lock(__a.prc);
        critsec_rc(_o_refcnt* prc);
        ~critsec_rc();
      private: critsec_rc(const critsec_rc&); void operator=(const critsec_rc&);
      };

      o_api() __bmdx_noex;
      o_api(const unity* pu_) __bmdx_noex; // NOTE "const" is ignored
      o_api(_o_itfslist& list) __bmdx_noex;
      o_api(void* p, bool b_pidyn) __bmdx_noex;

      void* p_head() const __bmdx_noex;

      o_type_info type_info(s_long flags) const __bmdx_noex;
      o_ref_info ref_info() const __bmdx_noex;
      void* pvoid(bool b_checked) const __bmdx_noex;
      void* prefo(const char* tname, s_long t_ind, s_long t_size) const __bmdx_noex;
      s_long rc_incdec(s_long flags) const __bmdx_noex;
      s_long upd_ut_ref_type(bool b_strong) const __bmdx_noex;
      s_long itfslist_create() __bmdx_noex;
      s_long itfslist_n() const __bmdx_noex;
      s_long itfs_append_cp(const o_api& src) const __bmdx_noex;
      s_long itfs_remove_ind(s_long ind) const __bmdx_noex;
      s_long itfs_remove_all() const __bmdx_noex;
      s_long find_by_pitfs(o_itfset_base* p) const __bmdx_noex;
      #if defined(__GNUC__) || defined(__clang__)
        template<class T> T* pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const __bmdx_noex;
        template<class I> o_iptr_t<I> pinterface(s_long flags, const o_itfset_base* ps0 = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex;
        template<class I> o_iptr_t<I> pinterface_at(s_long ind, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex;
        template<class Itfs> s_long itfs_attach(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const __bmdx_noex;
        template<class Itfs> s_long find_local_by_itfs(s_long flags, s_long ind0 = 0, s_long ind2 = -1, meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const __bmdx_noex;
        template<class I> s_long find_by_itf(s_long flags, o_itfset_base* ps0 = 0, std::basic_string<s_long>* pret = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex;
      #else
        template<class T> T* pobj(s_long flags, meta::noarg_tu_t<T>) const __bmdx_noex;
        template<class I> o_iptr_t<I> pinterface(s_long flags, const o_itfset_base* ps0, meta::noarg_tu_t<I>) const __bmdx_noex;
        template<class I> o_iptr_t<I> pinterface_at(s_long ind, meta::noarg_tu_t<I>) const __bmdx_noex;
        template<class Itfs> s_long itfs_attach(meta::noarg_tu_t<Itfs>) const __bmdx_noex;
        template<class Itfs> s_long find_local_by_itfs(s_long flags, s_long ind0, s_long ind2, meta::noarg_tu_t<Itfs>) const __bmdx_noex;
        template<class I> s_long find_by_itf(s_long flags, o_itfset_base* ps0, std::basic_string<s_long>* pret, meta::noarg_tu_t<I>) const __bmdx_noex;
      #endif
      template<class T> T& robj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const __bmdx_exs(XUExec) { return *unity::checked_ptr<T>(this->pobj<T>(flags)); }

    private:
      o_api _ref_newcp(unity_common::__Psm pmsm_itfslist, s_long flags) const __bmdx_noex;
      s_long _ref_del(s_long flags) __bmdx_noex;
      meta::s_ll _itfslist_nrsv(meta::s_ll n) const __bmdx_noex;
      struct _itfslist_temp1;
    };


    template<class Obj, class _> struct _o_sm_t
    {
      struct _critsec_rc
      {
        typedef critsec_t<Obj> t_cs;
        static const s_long m = s_long(0x7fffffffl); // ~m == sign bit only
        meta::s_ll __stg[(sizeof(critsec_t<Obj>) + 7) / 8]; _o_refcnt::_rccsd<Obj>* prccsd; s_long flags; // 1 - enabled
        _critsec_rc(_o_refcnt& x, bool b_enable = true);
        ~_critsec_rc();
        void enable_autodel();
      };

      static void* pvoid(_o_refcnt* prc, s_long b_checked);
      static void* prefo(_o_refcnt* prc, const char* tname, s_long t_ind, s_long t_size);
      static void obj_type_info(_o_refcnt* prc, o_type_info* pret, s_long flags);
      static void obj_ref_info(_o_refcnt* prc, o_ref_info* pret, s_long flags);
      static void lock_create(meta::s_ll* pstg, _o_refcnt* prc, s_long flags);
      static void lock_destroy(meta::s_ll* pstg);
      static _o_refcnt* rc_create(Obj* pobj, s_long flags, s_long lkt);
      static s_long rc_incdec(_o_refcnt* prc, s_long flags);
      static s_long lock_state(const meta::s_ll* pstg);
      static void* sm(s_long ind) __bmdx_noex;
    };

    template<class Itfs, class _> struct _o_itfssm_t
    {
      static o_itfset_base* itfs_new() __bmdx_noex;
      static o_itfset_base* itfs_new_copy(o_itfset_base* x) __bmdx_noex;
      static void itfs_delete(o_itfset_base* x) __bmdx_noex;
      static void itfs_type_info(o_itfset_base* x, o_type_info* pret, s_long flags) __bmdx_noex;
      static s_long __xx_after_construct(o_itfset_base* x, o_itfset_base* src, s_long flags) __bmdx_noex;
      static void __xx_before_destroy(o_itfset_base* x, s_long flags) __bmdx_noex;
      static void* sm(s_long ind) __bmdx_noex;
    };




    //==  ACCESS TO SHARED LIBRARIES
    struct _mod_exhandle;
      // NOTE Concurrent access:
      //  1. Non-const methods of one mod_handle instance (operator=, swap, clear)
      //  are NOT thread-safe.
      //  2. mod_handle::request arguments must not be modified by client during method call.
      //  3. Simultaneous calls to different mod_handle objects are safe.
    struct mod_handle
    {
      mod_handle() __bmdx_noex;
      ~mod_handle() __bmdx_noex;
      mod_handle(const mod_handle&) __bmdx_noex; // normally, produces a copy of the original handle; does not fail (no allocations)
      void operator=(const mod_handle&) __bmdx_noex; // normally, produces a copy of the original handle; does not fail (no allocations)
      void swap(mod_handle&) __bmdx_noex;
      void clear() __bmdx_noex;

      #ifdef _bmdxpl_Wnds
        typedef HINSTANCE t_nativehandle;
      #endif
      #ifdef _bmdxpl_Psx
        typedef void* t_nativehandle;
      #endif

      bool is_valid() const __bmdx_noex; // true if this handle is valid
      operator bool() const __bmdx_noex; // == is_valid()
      bool is_mainexe() const __bmdx_noex; // true if this handle is valid and references the main executable
      t_nativehandle native() const __bmdx_noex; // native shared library or main executable handle (platform-dependent)
      bool operator==(const mod_handle& x) const __bmdx_noex;
      bool operator!=(const mod_handle& x) const __bmdx_noex;

        // Pointer to procedure or variable by name (GetProcAddress, dlsym).
      void* sym(const char* name) const __bmdx_noex;

        // Call into shared module (.dll, .so) predefined function:
        //    s_long bmdx_mod_request(s_long sig_struct, s_long action, const unity* ppara, unity* pretval);
        //    with action == 1,
        //    sig_struct == unity::sig_struct() of the current binary module,
        //    ppara == &para,
        //    pretval == &retval.
        // para: input parameters of the request.
        // retval: value, returned by the request.
        //    Input and output contents are determined by the client and requested module relationships.
        //  flags: 0x1 - ignore native handle, search for name in the main executable.
        // The request is done under global lock, which may block up to 60 s,
        //  after which the blocked thread cancels the request.
        //  (In multithreaded program, requests should take small time, <= milliseconds.)
        // NOTE Defining bmdx_mod_request in the shared library is optional.
        //    The client may use any other functions and means on communication between executable
        //    and library, or between libraries. For example, via BMDX:
        //    a) global memory (shmobj2s_t), b) in-process common objects storage (unity::pcos).
        // Returns:
        //  1 - success.
        //  -1 - the requested module is not compatible.
        //  -2 - client-side failure or lock timeout.
        //  -3 - failed to get address of bmdx_mod_request function (see below).
        //  -4 - the handle is not valid.
        //  -5 - module-side exception.
        //  <= -10 - module-side error.
      s_long request(const unity& para, unity& retval) const __bmdx_noex;

        // Pointer to function, returning pointers to private static methods (of BMDX) in the binary module
        //    that is held by the current handle. Value same as _pmsm() is returned by  unity_common::pls_modsm()
        //    of that binary module (main executable or shared library instance).
        // NOTE _pmsm() may return 0:
        //    a) if the handle holds a library that is compiled without BMDX at all or maybe with bmdx_part_dllmgmt 0.
        //    b) same as (a) if the handle addresses the main executable - this may occur in shared library,
        //      when it uses bmdx_part_dllmgmt 1, but main executable does not.
        //    c) if the handle is null.
      unity_common::f_ls_modsm _pmsm() const __bmdx_noex;

        // Same as _pmsm, but for root loader of the shared library (the topmost loaded module with BMDX,
        //    that has loaded directly or indirectly all other such modules).
      unity_common::f_ls_modsm _pmsm_rl() const __bmdx_noex;

        // Returns a handle to the current module.
        //  b_autounload:
        //    true - allows to unload the library when this handle is destroyed, if it's the last one,
        //      associated with the library.
        //    false - unconditionally disables library unloading regardless of other handles settings.
        //  NOTE The returned handle is valid only a) for executable, b) for shared library, loaded with unity::mod().
        //    The result depends also on compile-time constants
        //    (any of bmdx_part_dllmgmt, bmdx_expose_ls_modsm should be 1).
        //  NOTE In multithreaded shared library, the client must avoid situation
        //    when autounloadable "handle to itself" remains the last one holding the library.
        //    If this occurs, the library may be unloaded before exiting from its own code, which causes system exception.
        //    It does not matter if the handle to itself is got with hself() or passed from other module.
        //    The correct cleanup sequence may be:
        //      1. The library is notified, by strong call, in application-specific way, right before being unloaded.
        //      2. On that notification, the library destroys all handles to itself that are kept inside the library.
        //      3. On the same notification, the library stops all its threads, or at least threads
        //          that may call hself() as part of their working.
      static mod_handle hself(bool b_autounload) __bmdx_noex;

      struct _stg_mh { enum { n = 8 }; void* _d[n]; _stg_mh(); ~_stg_mh(); _mod_exhandle& operator()() const; };
      private: friend struct unity; friend struct cv_ff; cref_t<_stg_mh> _rex; meta::s_ll __rsv1;
    };

      // Specification for bmdx_mod_request -- the function,
      //    called by unity::mod_handle::request in the target module (shared library or main executable).
      //
      //  sig:
      //      check if equal to unity::sig_struct(), return -1 if not (compatibility error).
      //  action:
      //      1 - default request (called via unity::mod_handle::request, under lock).
      //        The called module may fill pretval
      //          a) in response to ppara,
      //          b) with readable info if ppara is not recognized.
      //      0 and >=2 - reserved., the function must return -2.
      //      < 0 - client-specified action.
      //  ppara, pretval:
      //      nonzero pointers. Must be checked for compatibility before use.
      //      If not compatible, the function must return -1.
      //
      //  To simplify checks, use unity::rqcheck.
      //
      //  Returns:
      //      1 - success.
      //      -1 - compatibility error.
      //      -2 - unrecognized action code or lock failure.
      //      >=2, 0, -3..-9 - reserved.
      //      <= -10 - custom module-side error.
      //
      //extern "C" __BMDX_DLLEXPORT bmdx::s_long bmdx_mod_request(bmdx::s_long sig, bmdx::s_long action, const bmdx::unity* ppara, bmdx::unity* pretval)
      //{
      //  bmdx::unity::rqcheck __rc(sig, action, ppara, pretval); if (__rc <= 0) { return __rc; }
      //
      //  try {
      //    ...
      //    return 1;
      //  } catch (...) { return -10; }
      //}
    struct rqcheck
    {
      s_long res;
      critsec_t<mod_handle> lock;

        // timeout_ms >= 0: try to set multithread lock during rqcheck lifetime.
        //  The lock should only be set if the shared library may be loaded concurrently
        //  in the way other way than unity::mod_handle::request.
        // timeout_ms < 0 (dflt.): do not set the lock, only check compatibility.
      rqcheck(s_long sig, s_long action, const unity* ppara, const unity* pretval, s_long timeout_ms = -1 __bmdx_noarg) __bmdx_noex
        : res(0), lock(timeout_ms >= 0 ? 1000 : -1, timeout_ms)
      {
        if (timeout_ms >= 0 && !lock) { res = -2; return; }
        if (!(action == 1 || action < 0)) { res = -2; return; }
        if (action == 1 && !(sig == bmdx::unity::sig_struct() && ppara->compatibility() > 0 && pretval->compatibility() > 0))
          { res = -1; return; }
        res = 1;
      }

      operator s_long() const { return res; }
    };

        // Returns binary module (.dll, .so) handle representation.
        //    If module with given name is not loaded yet, an attempt to load is done.
        // name:
        //    a) shared library name and optional path.
        //    b) empty string -- create handle for main executable, not for shared library.
        //      In this case, b_autounload is ignored.
        //    c) if name is 0-ptr. or invalid, or module loading has failed, the returned handle is empty (is_valid() == false).
        // b_autounload:
        //    if true, the module may be unloaded if no other handle remains.
        //    If any handle was created with b_autounload == false,
        //    the module won't be unloaded until the program exits.
        // flags:
        //      controlling platform-dependent options to ensure maximum compatibility
        //      when loading shared library (.dll, .so). Default value = 0 (all automatic).
        //    flags & 3:
        //      1 - set RTLD_DEEPBIND if available on this system.
        //          Binds all symbols to their own module, regardless of build type.
        //          This mode is useful if -Wl,-Bsymbolic switch is not used and any
        //              a) the compiled Linux binary is built with GCC using static C and C++ libs.
        //              b) the compiled .so is built with GCC using static C and C++ libs.
        //      0 - do not use RTLD_DEEPBIND even if available.
        //          Symbols binding occurs according to factual compiler options set.
        //      2 - automatic choice (Linux/g++ - enable, others - disable).
        //    flag 0x4 (appeared first in BMDX 1.5 2022-11-15):
        //      a) Windows: assume module name (path) being encoded in UTF-8 instead of system encoding.
        //          In this case, use LoadLibraryW to load module.
        //          (If the flag is not set, LoadLibraryA is used.)
        //      b) POSIX systems: ignored (using dlopen(name...)).
        // NOTE mod(), mod_handle::request(), and other functions
        //    may be safely called from multiple threads.
        // NOTE The client must ensure that no high-level cross-module objects
        //    left in memory when the module is unloaded.
    #if bmdx_part_dllmgmt
      static mod_handle mod(const char* name, bool b_autounload, s_long flags = bmdx_mod_load_def_flags) __bmdx_noex;
    #else
        // With bmdx_part_dllmgmt 0, mod() returns a null handle.
      static mod_handle mod(const char* name, bool b_autounload, s_long flags = bmdx_mod_load_def_flags) __bmdx_noex { return mod_handle(); }
    #endif // bmdx_part_dllmgmt


        // unity object structure signature for binary compatibility check.
    static s_long sig_struct() __bmdx_noex;

      // Assume U <=> *this.
      //
      // U.compatibility() returns:
      //
      //  2 - U is created in the current binary module.
      //    For compatibility 2, U's contents are not additionally checked.
      //    This means that the contained value is usable in all ways, with two warnings:
      //      1. U may wrap a user object, which itself is not compatible.
      //      2. If U is an array or associative array, compatibility() does not check
      //        each of that container's values.
      //        Such container may be, in principle, heterogeneous, i.e. contain elements,
      //        that are created (or assigned) by different binary modules.
      //    NOTE Compatibility 2 is the only possible value if the application does not exchange
      //      BMDX objects with any shared libraries, by design.
      //
      //  1 - U is created not in the current binary module,
      //    but it is compatible, exactly as with compatibility 2.
      //    In part., the contained value may be accessed by reference.
      //
      //  0 - U is created not in the current binary module, and its contained value or object
      //    may not be accessed by reference without additional checks.
      //    A. For strings, compatibility 0 means that direct reference to string
      //      may not be got by the client with ref<utString>() or rstr().
      //      Still, U may be safely copied, assigned, output to stream etc.,
      //      and also the string value is accessible as copy,
      //      with val<utString>(), vstr() etc.
      //    B. For wrapped user objects, compatibility 0
      //      means that both U and the wrapped object have been created
      //      in another binary module.
      //      Still, objPtr<Obj>(true, true) will return non-0 pointer, if the wrapped object
      //      is compatible with the current module.
      //      (This is determined by value of statically declared type index.
      //        See also: 1. unity :: objPtr(). 2. type_index_t in vecm_hashx.h.)
      //
      //  <0 - U itself is not compatible with the current module.
      //    Copying U, and most of access functions are unsafe.
      //    Only clear(), recreate() and destructor may be called safely.
      //    (This case concerns mostly with utMap and utHash types,
      //      and in practice it is not expected to occur across known compilers,
      //      independently on version, including future versions.)
      //
    s_long compatibility() const __bmdx_noex;


    //==  IN-PROCESS COMMON OBJECTS STORAGE (PCOS)
    private: struct _pcos_d_base { mod_handle hhost; virtual ~_pcos_d_base() {} }; friend struct cv_ff_pcos;
    public:

    //  PCOS associates one hash map { unity, cref_t<unity> } with each binary module of the process
    //    (by dflt. main executable, and optionally, any shared library, compiled together with bmdx_main.cpp).
    //  Objects in the map may be set and read concurrently in any binary module of the process.
    //  With each object, its own module handle is kept, until the object is destroyed, to protect object code
    //    from untimely unloading from the process.
    //  Together with messaging system, PCOS ensures transparent access to high-level user objects
    //    anywhere in multi-module application.

    // NOTE Cross-moduleness. Build options.
    //    pcos object may be freely passed (by value or by reference) through binary modules threshold.
    //      If pcos references the storage in a shared library, the library is not unloaded at least until pcos is destroyed,
    //      because pcos internally contains a handle to that library.
    //    For common storage, it's recommended to compile main executable
    //      including bmdx_main.cpp with bmdx_part_dllmgmt 1 (see in bmdx_config.h).
    //    Any pcos object,
    //      a) created in binary module that is compiled with bmdx_part_dllmgmt 1, or
    //      b) given handle to such module on creation, regardless of place of creation,
    //      is fully functional in any module.
    //    In contrary, pcos object, being created by default in a module that is compiled
    //      with bmdx_part_dllmgmt 0, is *non-functional*,
    //      because starts with null handle instead of executable's handle.

    // NOTE Concurrent access: pcos.
    //    All methods of any pcos instance are thread-safe (equiv. to cref_t).
    //    This implies that arguments of any method are not modified or overwritten during pcos method call.
    //    pcos object itself may be safely overwritten at any time, even during method call.

    // NOTE Concurrent access: the wrapped user value or object.
    //      In case of concurrent (modifying) access to particular value, where non-blocking access cannot be ensured,
    //        the client is responsible for synchronization. This should be done by client-defined lock.
    //      Certain specific locks are created automatically, but they are not suitable to protect custom object:
    //        1. cref_t<unity>::t_lock(cref_t from makecp, makemv, operator=) - locks the reference counting of the object.
    //          This lock automatically protects reference counting and object deletion on ref. count == 0.
    //        2. unity::o_api::critsec_rc - locks operations on unity/utObject, in case of wrapping the custom object.
    //          When custom object is stored in pcos, pcos may temporarily lock the object this way for checking.
    //          During unity/utObject lifetime in pcos, the client must not overwrite or clear it directly.
    //          Instead, the client should use unity::o_api normally, i.e. add/remove interfaces, get pointer to the wrapped object,
    //          call the wrapped object, copy the wrapping unity object etc.,
    //          and also manipulate the outer cref_t (get with pcos::operator(), copy, store as necessary, pass to setref etc.).

    // NOTE Ownership on objects.
    //      PCOS ensures safe handling of objects from any binary modules.
    //      Unless special actions taken by the client, the following constraints always apply:
    //      1. A user value (scalar, string, array or user object) is wrapped into struct unity,
    //          which is in turn wrapped into the outer cref_t object.
    //      2. Wrapping into unity has the only purpose of keeping polymorphic values from multiple modules
    //          in the storage hash map.
    //      3. Wrapping into the outer cref_t solves three problems:
    //          1) Together with user value, a handle to the binary module (main executable or shared library)
    //              is kept in aux. object of cref_t.
    //            By design of cref_t, this handle is destroyed anyway later than the user value,
    //              and the library, containing the code of the user value, cannot be occasionally
    //              unloaded until the user value is destroyed.
    //          2) cref_t itself belongs to the root loader of the library (the topmost loaded module with BMDX,
    //              that has loaded directly or indirectly all other such modules).
    //            Most often, main executable is the root loader, unless a shared library that uses BMDX
    //              has been loaded in some way different from unity::mod() call.
    //              Thus, code and critical section data of cref_t itself, are released long after the user value it held,
    //              namely when root loader library is unloaded (or process exits).
    //            In rarer case, when the root loader and the user value code are in the same shared library,
    //              the library is not unloaded until the last pcos object is destroyed, which normally occurs
    //              in code that is outside that library.
    //          3) cref_t<unity> is lightweight concurrency-safe reference to unity object,
    //              so its copying costs approx. 1 lock/unlock,
    //              and copying unity object by value (which is costly for strings, arrays and sometimes user objects)
    //              is not required.
    //      4. PCOS checks the modules to which cref_t, contained unity, and its contained user object (if any)
    //            belong.
    //          General rule is:
    //              if a binary module wants to put into a storage (represented by pcos)
    //              a value that is from a module different from that of the storage itself,
    //              the module must a) supply value's module handle,
    //              or b) pass the value in the ready-made form of cref_t<unity>,
    //              created with makemv, makecp, or taken from any storage with operator().
    //          Example 1 (copying).
    //              If a shared library does not own the storage (e.g. it uses, by default, main executable's storage),
    //                but wants to put its own non-user-object value into the storage, it can use setcp or makecp+setref.
    //              The value will be copied and belong to executable's storage.
    //                Even if the library is unloaded, the value will safely remain in the storage.
    //          Example 2 (moving).
    //              If the value or user object (stored as unity/utObject) must be moved into the storage (setmv or makemv+setref),
    //                the library must supply mod_handle to itself (see mod_handle::hself()), otherwise the call fails -
    //                ownership on object cannot be changed to other binary module
    //                because object's binary code is part of the library.
    //                When the handle is supplied, and the value has been moved successfully (+ handle copy created implicitly),
    //                the library won't be unloaded until all objects, placed in such way, are removed from the storage.
    //          Example 3 (main executable).
    //              Main executable never needs to supply a handle
    //                to put its value into its own (i.e. default) storage, because pcos itself already contains
    //                main executable's handle.

    struct pcos
    {
    private:
      typedef cref_t<_pcos_d_base> t_d; typedef unity_common em; typedef unity_common::f_ls_modsm t_msm; friend struct cv_ff_pcos;
      t_d _d;
      typedef s_long (*f_pcos_cons)(cref_t<_pcos_d_base>* pd, const unity::mod_handle* ph);
      typedef s_long (*f_pcos_oppar)(cref_t<_pcos_d_base>* pd, const unity* pk, cref_t<unity>* prv);
      typedef s_long (*f_pcos_setref)(const cref_t<_pcos_d_base>* pd, const unity* pk, const cref_t<unity>* pxr);
      typedef s_long (*f_pcos_checkref)(const cref_t<_pcos_d_base>* pd, const cref_t<unity>* pxr);
      typedef s_long (*f_pcos_setmv)(const cref_t<_pcos_d_base>* pd, const unity* pk, unity* pxm, unity::mod_handle* phx);
      typedef s_long (*f_pcos_setcp)(const cref_t<_pcos_d_base>* pd, const unity* pk, const unity* pxc, unity::mod_handle* phx);
      typedef s_long (*f_pcos_clearstg)(const cref_t<_pcos_d_base>* pd);
      typedef s_long (*f_pcos_makemv)(const cref_t<_pcos_d_base>* pd, unity* pxm, unity::mod_handle* phx, cref_t<unity>* prv);
      typedef s_long (*f_pcos_makecp)(const cref_t<_pcos_d_base>* pd, const unity* pxc, unity::mod_handle* phx, cref_t<unity>* prv);
    public:

        // Creates a pcos object, referencing certain in-process common objects storage.
        //    By default, main executable is the module, servicing the common storage.
        //    Usually it has longer lifetime than any shared library instance.
        //    The client may pas a handle (h) to any loaded shared library,
        //    to address the storage of that library, or null handle to create non-functional pcos,
        //    which is not associated with any storage.
      pcos(unity::mod_handle h = unity::mod("", 1)) __bmdx_noex        { if (!h) { return; } unity_common::f_ls_modsm pmsm = h._pmsm(); if (!pmsm) { return; } typedef f_pcos_cons tf; tf f = (tf)pmsm(em::msm_pcos_cons); if (!f) { return; } try { f(&_d, &h); } catch (...) {} }

        // Binary module handle of the current storage.
        // May be:
        //  a) by default, the main executable.
        //  b) a shared library, loaded into the process.
        //  c) null handle (if pcos constructor was given a null handle).
      unity::mod_handle hhost() const __bmdx_noex        { t_d d = _d; return d ? d->hhost : unity::mod_handle(); }

        // True if this pcos addresses a normally functioning storage (in the main executable, or in a shared library).
      operator bool() const __bmdx_noex        { return _d; }

        // True if this pcos addresses a normally functioning storage in the main executable.
        //  NOTE The common storage of the main executable is regarded the single storage
        //    that should be used by default by all clients.
        //    Usually it has longer lifetime than any shared library instance.
        //    Storages of shared library instances should be used for private purposes or if main executable
        //    does not support common storage.
      bool b_mainexe() const __bmdx_noex        { t_d d = _d; return d && d->hhost.is_mainexe(); }


        // Get a reference to the value with key k, from the current storage.
        //    (The reference is not removed from the storage, so the value may be accessed again by any client.)
        // The result code and returned value:
        //    1 - the value exists. A ref. to valid object is returned.
        //  An empty cref_t is returned with the following result codes:
        //    0 - the value does not exist.
        //    -2 - op. failed.
        //    -3 - pcos object is non-functional.
      cref_t<unity> operator()(const unity& k, s_long* pres = 0) __bmdx_noex        {  cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; }  typedef f_pcos_oppar tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_oppar); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &k, &rv); } catch (...) {}  if (pres) { *pres = res; } return rv; }

        // Set the new value into the current storage, or remove the existing one (setref only).
        //    xc, xm: the value is moved (xm) or copied (xc) into the storage.
        //      The value is associated (by handle) with binary module in which it's factually created.
        //      By default, this is binary module of the current storage (pcos::hhost()).
        //      Otherwise, it's hx (if specified).
        //      See the above note "Ownership on objects" for explanation when and which hx is necessary to specify.
        //      The correctness of the association is checked in setmv, setcp.
        //      Relaxation for xc: unity object itself may be associated with any module,
        //        because it's automatically copied by setcp on the side, represented by handle.
        //      Anyway, if xm or xc contains user object (unity/utObject),
        //        the user object must have been created by the client on the side, represented by handle.
        //    xr:
        //      a) valid object reference, must have been created with makemv, makecp, or got from any pcos with operator().
        //        Validity is checked by setref.
        //      b) default empty "cref_t<unity>()": removes existing value with key k from the storage.
        // The resut code and actions done:
        //  a) For setmv, setcp, and setref with non-empty cref_t:
        //      2 - successfully added new value. The prev. value with k did not exist.
        //      1 - successfully replaced the prev. existing value with k.
        //      NOTE setmv on success (1 or 2): xm contents are moved into the storage, xm itself is cleared.
        //  b) Only for setref with empty cref_t (for removing the value with k):
        //      1 - successfully removed the existing value with k.
        //      0 - the prev. value with k did not exist, nothing done.
        //  -1 - bad argument.
        //      Possible cause:
        //        a) xr must have been created with makemv, makecp, or got from any pcos with operator().
        //        b) xm or the wrapped object (unity/utObject in xm, xc) is from other binary module than hx or pcos (if hx is not specified).
        //  -2 - op. failed.
        //  -3 - pcos object is non-functional.
      s_long setref(const unity& k, cref_t<unity> xr) __bmdx_noex        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setref tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setref); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xr); } catch (...) {} return res; }
      s_long setmv(const unity& k, unity& xm, unity::mod_handle hx = unity::mod_handle()) __bmdx_noex        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setmv tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setmv); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xm, &hx); } catch (...) {} return res; }
      s_long setcp(const unity& k, const unity& xc, unity::mod_handle hx = unity::mod_handle()) __bmdx_noex        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setcp tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setcp); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xc, &hx); } catch (...) {} return res; }

        // Check if the given reference is correct for passing into setref,
        //    i.e. made with makemv, makecp, or returned by pcos operator().
        //    The check is identical to one that setref() does before modifying the storage.
        //  1 - xr is non-empty and fit for passing into setref().
        //  0 - xr is empty.
        //  -1 - bad xr structure, module or version: xr won't be accepted by setref().
        //  -2 - op. failed.
        //  -3 - pcos object is non-functional.
      s_long checkref(cref_t<unity> xr) __bmdx_noex        { if (!xr) { return 0; } t_d d = _d; if (!d) { return -3; } typedef f_pcos_checkref tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_checkref); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &xr); } catch (...) {} return res; }

        // Clear the referenced storage.
        //    Removes all objects, so that all references and shared library handles, associated with values,
        //    are released. This may produce cascaded library unloads in some cases.
        //  Timely clearing the storage is responsibility of the client module (e.g. main executable).
        //    This is necessary before quitting the process, to allow for unloading all libraries
        //    involved into application-specific objects interchange.
        // The result code:
        //    1 - success. All objects are removed from the storage.
        //    0 - the storage is not initialized, no objects, nothing done.
        //    -2 - op. failed.
        //    -3 - pcos object is non-functional.
      s_long clearstg() __bmdx_noex        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_clearstg tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_clearstg); if (!f) { return -2; } s_long res = -2; try { res = f(&d); } catch (...) {} return res; }

        // Create a strong multi-functional reference to the given value.
        //    The value is moved (xm) or copied (xc) into the reference.
        //    The returned cref_t object may passed into setref of any pcos, or used directly, as strong reference.
        //    xc, xm: the value is moved (xm) or copied (xc) into the new reference.
        //      The value is associated (by handle) with binary module in which it's factually created.
        //      By default, this is binary module of the current storage (pcos::hhost()).
        //      Otherwise, it's hx (if specified).
        //      The correctness of the association is checked by makemv, makecp.
        //      Relaxation for xc: unity object itself may be associated with any module,
        //      because it's automatically copied by makecp on the side, represented by handle.
        //      Anyway, if xm or xc contains user object (unity/utObject),
        //      the user object must have been created by the client on the side, represented by handle.
        // The result code and returned value:
        //  1 - success. A ref. to valid new object is returned.
        //    NOTE makemv on success: xm contents are moved into the returned object, xm itself is cleared.
        //  An empty cref_t is returned with the following result codes:
        //    -1 - bad argument.
        //    Possible cause: xm or the wrapped object (if unity/utObject in xm, xc)
        //      is from other binary module than hx or pcos (if hx is not specified).
        //    -2 - op. failed.
        //    -3 - pcos object is non-functional.
      cref_t<unity> makemv(unity& xm, unity::mod_handle hx = unity::mod_handle(), s_long* pres = 0) __bmdx_noex        { cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; } typedef f_pcos_makemv tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_makemv); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &xm, &hx, &rv); } catch (...) {} if (pres) { *pres = res; } return rv; }
      cref_t<unity> makecp(const unity& xc, unity::mod_handle hx = unity::mod_handle(), s_long* pres = 0) __bmdx_noex        { cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; } typedef f_pcos_makecp tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_makecp); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &xc, &hx, &rv); } catch (...) {} if (pres) { *pres = res; } return rv; }
    };

  private:
    unity_common::__Psm pmsm;
    union { mutable s_long ut; void* __3; }; // ut bits 8..31 contain either 0 or cached compatibility id
    union { mutable _stg_u _data; meta::s_ll __1; double __2; };

    enum {
      _xCompatMask = 0xfffff, _xCompatShift = 12,
      xfmUtype = (utArray << 1) - 1, xfmUtypeAndFlags = (1 << _xCompatShift) - 1, xfPtr = utArray << 1, xfObjStrongRef = utArray << 2, xfObjItfsList = utArray << 3
        //-- 0x3f, 0xfff, 0x40, 0x80, 0x100
    };

    friend struct cv_ff; friend struct _static_conv; friend struct paramline;

    inline t_map_tu* _m() const __bmdx_noex { return reinterpret_cast<t_map_tu*>(_data.p1); }
    inline t_hash* _h() const __bmdx_noex { return reinterpret_cast<t_hash*>(_data.p1); }
    inline s_long _uttf() const __bmdx_noex { return ut & xfmUtypeAndFlags; }
    static _fls75 _tname0(s_long utt) __bmdx_noex;
    static s_long _def_size(s_long utt) __bmdx_noex;
    template<int utt> inline typename valtype_t<utt>::t* _drf_c_i(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const __bmdx_noex { return reference_t<typename valtype_t<utt>::t>::_drf_c(this); }
    template<int utt> inline typename valtype_t<utt>::t_tu* _drf_c_ai(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const __bmdx_noex { return (typename valtype_t<utt>::t_tu*)reference_t<typename valtype_t<utt>::t>::_drf_c(this); }

    s_long _compat_id(unity_common::__Psm pmsm, s_long utype) const __bmdx_noex;
    bool _compat_chk() const __bmdx_noex;
    s_long _compatibility() const __bmdx_noex;
    inline vec2_t<meta::s_ll>* _drf_q() const { return reinterpret_cast<vec2_t<meta::s_ll>*>(_data.p1); }
    inline unity*& _drf_name() const { return *reinterpret_cast<unity**>(&_data.p2); }

      // _x_* may generate exceptions.
    bool _x_cnv_this_val(EConvStrength cs, s_long utt_dest, s_long flags, void* pdest) const;
    void _x_cnv_this(EConvStrength cs, s_long utt_dest, bool keep_name);
    void _x_asg(const unity& x, s_long flags);
    bool _xu_move_p1(unity& src);

      // _L* return result codes (no exceptions).
    s_long _Lcreate(_stg_u* pdest, s_long uttf, s_long flags);
    s_long _Ldestroy(_stg_u* pdest, s_long uttf, s_long flags);
    s_long _Lel_get(s_long ind, unity* pst) const;
    s_long _Lel_set(s_long ind, const unity& x);
    s_long _Lel_append(const unity& x);
    s_long _Lel_insrem(s_long ind, s_long m);
    s_long _Lel_insrem(s_long ind, s_long m, const unity& x);
    s_long _Lu_set_name(const unity* pn);
    s_long _Lu_clear(s_long flags);
    s_long _Ldelete_this();
    _o_itfslist* _Litfslist_cr();
    s_long _Litfslist_del(void* p);
    static _o_itfslist* _Litfslist_cr(unity_common::__Psm pmsm);
    static s_long _Litfslist_del(unity_common::__Psm pmsm, void* p);

    void _ensure_h();
    void _ensure_m();
    void _ensure_sc();

    unity* _path_u(const arrayref_t<wchar_t>& keylist, bool forced) __bmdx_noex;
    unity* _path_w(const arrayref_t<wchar_t>& keylist) __bmdx_noex;
    unity* _path_w(const wchar_t* keylist) __bmdx_noex;
    unity* _path_w(const arrayref_t<char>& keylist) __bmdx_noex;
    unity* _path_w(const char* keylist) __bmdx_noex;
    unity* _path_w(const unity& keylist) __bmdx_noex;
  };




  // Implementation part + comments.

    // flags OR-ed:
    //  1 - return obj. pointer if its static type name matches T.
    //  4 - return obj. pointer if its vecm type index matches that of T (see type_index_t in vecm_hashx.h).
    //  8 - return obj. pointer without type check (flags 1, 4 will be ignored).
    //  0x10 - return obj. pointer even if object is deleted.
    //  0x20 - return obj. pointer if object belongs to the current binary module.
    //  0x40 - return obj. pointer if object belongs to any other binary module.
  #if defined(__GNUC__) || defined(__clang__)
    template<class T> T* unity::o_api::pobj(s_long flags, meta::noarg_tu_t<T>) const __bmdx_noex
  #else
    template<class T> T* unity::o_api::pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const __bmdx_noex
  #endif
  {
    if (!((flags & 0x60) && (flags & 0xd))) { return 0; }
    if (!prc) { return 0; }
    s_long f2 = (flags & 8) ? 0 : (flags & 5); if (flags & 0x10) { f2 |= 8; }
    o_type_info ti(0,0,0,0,0,0,-1); ((__Posm_type_info)prc->rc_posm(unity_common::osm_type_info))(prc, &ti, f2);
    if (!ti._pobj) { return 0; }
    if (ti.b_local()) { if (!(flags & 0x20)) { return 0; } } else { if (!(flags & 0x40)) { return 0; } }
    if (!((flags & 8)||((flags & 1) && ti.b_same_tstat_cm(typeid(T)))||((flags & 4) && ti.b_same_t_ind<T>()))) { return 0; }
    return reinterpret_cast<T*>(ti._pobj);
  }
    // Search direction in the list of interface sets: from end (larger inds, latest attached sets) to the beginning (smaller inds).
    //  Search direction inside an interface set: from beginning (leftmost listed in o_interfaces<...>) to end (rightmost).
    // NOTE o_iptr_t is not thread-safe or ref.-counted.
    //    The referenced interface object must not be deleted in any way during client use of o_iptr_t.
    // flags OR-ed:
    //  1 - search includes interfaces from the current binary module (by interface typeid name).
    //  2 - search includes interfaces from all other binary modules (by interface name defined in o_proxy<I> specialization).
    //  4 - search includes interfaces in sets attached before ps0.
    //  8 - search includes interfaces in ps0.
    //  0x10 - search includes interfaces in sets attached after ps0.
    //  If ps0 == 0, any of flags 4, 8, 0x10 includes all interfaces in sequence.
  #if defined(__GNUC__) || defined(__clang__)
    template<class I> o_iptr_t<I> unity::o_api::pinterface(s_long flags, const o_itfset_base* ps0, meta::noarg_tu_t<I>) const __bmdx_noex
  #else
    template<class I> o_iptr_t<I> unity::o_api::pinterface(s_long flags, const o_itfset_base* ps0 = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex
  #endif
  {
    if (!(prc && pidyn && (flags & 0x1c) && (flags & 3))) { return o_iptr_t<I>(); }
    int pos = 0x10; if (!ps0) { flags |= 0x1c; }
    critsec_rc __lock(prc); if (sizeof(__lock)) {}
    for (meta::s_ll i = pidyn->itfslist.n() - 1; i >= 0; --i)
    {
      _o_itfslist::handle& h = pidyn->itfslist[i]; if (!h.b_valid()) { continue; }
      unity_common::__Psm pmsm_h = (unity_common::__Psm)h.p_itfssm(0);
      if (pos == 0x10) { if (h.p_itfs == ps0 && ps0) { pos = 8; } }
        else if (pos == 8) { pos = 4; }
      if (!(flags & pos)) { continue; }
      if (pmsm_h == unity_common::pls_modsm())
      {
        if (!(flags & 1)) { continue; }
        I* p = 0; try { p = (I*)h.p_itfs->__ifind(typeid(I).name()); } catch (...) {} if (!p) { continue; }
        return o_iptr_t<I>(p, unity_common::pls_modsm());
      }
      else
      {
        if (!(flags & 2)) { continue; }
        if (!pmsm_h) { continue; }
        unity_common::__Pipsm f_get_ism = (unity_common::__Pipsm)pmsm_h(unity_common::msm_obj_ipsm); if (!f_get_ism) { continue; }
        unity_common::__Psm psm = f_get_ism(o_proxy<I>::__iname()); if (!psm) { continue; }
        typedef I* (*__Pdyncast)(o_itfset_base* p_itfs);
          __Pdyncast f_dyncast = (__Pdyncast)psm(unity_common::_defsm_pi); if (!f_dyncast) { continue; }
        I* p = f_dyncast(h.p_itfs);
        if (!p) { continue; }
        return o_iptr_t<I>(p, pmsm_h);
      }
    }
    return o_iptr_t<I>();
  }
    // Creates interface pointer from the attached set at ind.
    // NOTE o_iptr_t is not thread-safe or ref.-counted.
    //    The referenced interface object must not be deleted in any way during client use of o_iptr_t.
  #if defined(__GNUC__) || defined(__clang__)
    template<class I> inline o_iptr_t<I> unity::o_api::pinterface_at(s_long ind, meta::noarg_tu_t<I>) const __bmdx_noex
  #else
    template<class I> inline o_iptr_t<I> unity::o_api::pinterface_at(s_long ind, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex
  #endif
  {
    while (1) // once
    {
      if (!(prc && pidyn)) { break; }
      critsec_rc __lock(prc); if (sizeof(__lock)) {}
      if (!(ind >= 0 && ind < pidyn->itfslist.n())) { break; }
      _o_itfslist::handle& h = pidyn->itfslist[ind]; if (!h.b_valid()) { break; }
      unity_common::__Psm pmsm_h = (unity_common::__Psm)h.p_itfssm(0);
      if (pmsm_h == unity_common::pls_modsm())
      {
        I* p = 0; try { p = (I*)h.p_itfs->__ifind(typeid(I).name()); } catch (...) {} if (!p) { break; }
        return o_iptr_t<I>(p, unity_common::pls_modsm());
      }
      else
      {
        if (!pmsm_h) { break; }
        unity_common::__Pipsm f_get_ism = (unity_common::__Pipsm)pmsm_h(unity_common::msm_obj_ipsm); if (!f_get_ism) { break; }
        unity_common::__Psm psm = f_get_ism(o_proxy<I>::__iname()); if (!psm) { break; }
        typedef I* (*__Pdyncast)(o_itfset_base* p_itfs);
          __Pdyncast f_dyncast = (__Pdyncast)psm(unity_common::_defsm_pi); if (!f_dyncast) { break; }
        I* p = f_dyncast(h.p_itfs);
        if (!p) { break; }
        return o_iptr_t<I>(p, pmsm_h);
      }
    }
    return o_iptr_t<I>();
  }
    // Creates a set of interfaces (new _o_itfs_topmost<Itfs>()) and attaches it to object (adds to interface sets list of the wrapping unity object).
    //  All interfaces get __itf_after_construct notification.
    // Returns:
    //    1 - the set is added to the existing list of sets.
    //    0 - the set of such static type already exists.
    //    -1 - pidyn == 0 (list of interf. sets is not created - call itfslist_create() first).
    //    -2 - failure.
    // NOTE Itfs should have a dependence from the current translation unit anon. namespace,
    //    to avoid incorrect aliasing of virtual and static functions.
    //    See detailed note near unity::objItfsAttach.
  #if defined(__GNUC__) || defined(__clang__)
    template<class Itfs> s_long unity::o_api::itfs_attach(meta::noarg_tu_t<Itfs>) const __bmdx_noex
  #else
    template<class Itfs> s_long unity::o_api::itfs_attach(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const __bmdx_noex
  #endif
  {
    if (!prc) { return -2; }
    if (!pidyn) { return -1; }
    critsec_rc __lock(prc); if (sizeof(__lock)) {}
    s_long res = find_local_by_itfs<Itfs>(1);
    if (res >= 0) { return 0; } if (res <= -2) { return -2; }
    meta::s_ll n0 = pidyn->itfslist.n();
    meta::s_ll nr0 = _itfslist_nrsv(n0);
    meta::s_ll nr2 = _itfslist_nrsv(n0 + 1);
    if (nr2 != nr0) { if (!pidyn->itfslist.resize(nr2)) { return -2; } pidyn->itfslist._set_n_u(n0); }
    _o_itfs_topmost<Itfs>* p = 0;
    try { p = new _o_itfs_topmost<Itfs>(); } catch (...) { return -2; }
    pidyn->itfslist._set_n_u(n0 + 1);
    p->__pidyn = pidyn;
    pidyn->itfslist[n0].p_itfs = p;
    pidyn->itfslist[n0].p_itfssm = _o_itfssm_t<Itfs>::sm;
    if (!_o_itfssm_t<Itfs>::__xx_after_construct(p, 0, 1)) { try { delete p; } catch (...) {} pidyn->itfslist._set_n_u(n0); return -2; }
    return 1;
  }
    // Search for set of interfaces created/attached within the current binary module.
    //    (flags & 1): search includes a set if its static type == Itfs.
    //    (flags & 2): search includes a set if its dynamic type == Itfs.
    //    (flags & 4): search includes a set if it may be cast by dynamic_cast into Itfs.
    //    NOTE When using dynamic_cast, if Itfs is o_interfaces_top specialization, matching is per translation unit.
    //        If Itfs is o_interfaces template instance, matching is per binary module instance.
    //    NOTE dynamic_cast may crash on certain POSIX systems (even if called in its own module)
    //        when binary modules made with different compilers are mixed in one program.
    // Search direction: from end (larger inds, latest attached sets) to the beginning (smaller inds).
    //    ind0: least index to include.
    //    ind2: least index to not include (-1 == end).
    // Returns: >= 0 - index of the found set. -1 - not found. -2 - failure.
  #if defined(__GNUC__) || defined(__clang__)
    template<class Itfs> s_long unity::o_api::find_local_by_itfs(s_long flags, s_long ind0, s_long ind2, meta::noarg_tu_t<Itfs>) const __bmdx_noex
  #else
    template<class Itfs> s_long unity::o_api::find_local_by_itfs(s_long flags, s_long ind0 = 0, s_long ind2 = -1, meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const __bmdx_noex
  #endif
  {
    if (!prc) { return -2; } if (!pidyn) { return -1; } if (!(flags & 7)) { return -1; }
    if (ind0 < 0 || ind2 < -1) { return -2; }
    critsec_rc __lock(prc); if (sizeof(__lock)) {}
    cpparray_t<_o_itfslist::handle>& ls = pidyn->itfslist;
    if (ind2 > ls.n()) { ind2 = s_long(ls.n()); }
    if (ind2 == -1) { ind2 = s_long(ls.n()); }
    for (meta::s_ll i = ind2 - 1; i >= ind0; --i)
    {
      _o_itfslist::handle& hs = ls[i]; if (!hs.b_valid()) { continue; }
      unity_common::__Psm pmsm_h = (unity_common::__Psm)hs.p_itfssm(unity_common::itfssm_modsm);
      if (pmsm_h != unity_common::pls_modsm()) { continue; }
      if (flags & 1)
      {
        __Pitfs_type_info f_ti = (__Pitfs_type_info)hs.p_itfssm(unity_common::itfssm_type_info);
          if (!f_ti) { continue; }
        o_type_info iti(0,0,0,0,0,0,-1);
        if (flags & 3) { f_ti(hs.p_itfs, &iti, flags & 3); }
        if ((flags & 1) && iti.b_same_tstat(typeid(Itfs))) { return s_long(i); }
        if ((flags & 2) && iti.b_same_tdyn(typeid(Itfs))) { return s_long(i); }
        try { if ((flags & 4) && dynamic_cast<Itfs*>(hs.p_itfs)) { return s_long(i); } } catch (...) {}
      }
    }
    return -1;
  }
    // Search for set (or several sets) of interfaces, from which a valid interface pointer may be created by pinterface().
    // Search direction: from end (larger inds, latest attached sets) to the beginning (smaller inds).
    //  Arguments and algorithm == that of pinterface (see).
    //  If pret != 0, indexes of all matching sets are pushed into it.
    // Returns: >= 0 - index of the first matching set (last attached set that can be cast into I). -1 - not found. -2 - failure.
  #if defined(__GNUC__) || defined(__clang__)
    template<class I> s_long unity::o_api::find_by_itf(s_long flags, o_itfset_base* ps0, std::basic_string<s_long>* pret, meta::noarg_tu_t<I>) const __bmdx_noex
  #else
    template<class I> s_long unity::o_api::find_by_itf(s_long flags, o_itfset_base* ps0 = 0, std::basic_string<s_long>* pret = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const __bmdx_noex
  #endif
  {
    if (!(prc && pidyn)) { return -2; }
    if (!((flags & 0x1c) && (flags & 3))) { return -1; }
    if (pret) { try { pret->clear(); } catch (...) { return -2; } }
    int pos = 0x10; if (!ps0) { flags |= 0x1c; }
    critsec_rc __lock(prc); if (sizeof(__lock)) {}
    for (meta::s_ll i = pidyn->itfslist.n() - 1; i >= 0; --i)
    {
      _o_itfslist::handle& h = pidyn->itfslist[i]; if (!h.b_valid()) { continue; }
      unity_common::__Psm pmsm_h = (unity_common::__Psm)h.p_itfssm(0);
      if (pos == 0x10) { if (h.p_itfs == ps0 && ps0) { pos = 8; } }
        else if (pos == 8) { pos = 4; }
      if (!(flags & pos)) { continue; }
      if (pmsm_h == unity_common::pls_modsm())
      {
        if (!(flags & 1)) { continue; }
        I* p = 0; try { p = (I*)h.p_itfs->__ifind(typeid(I).name()); } catch (...) {} if (!p) { continue; }
        if (!pret) { return s_long(i); }
        try { pret->push_back(s_long(i)); } catch (...) { return -2; }
      }
      else
      {
        if (!(flags & 2)) { continue; }
        if (!pmsm_h) { continue; }
        unity_common::__Pipsm f_get_ism = (unity_common::__Pipsm)pmsm_h(unity_common::msm_obj_ipsm); if (!f_get_ism) { continue; }
        unity_common::__Psm psm = f_get_ism(o_proxy<I>::__iname()); if (!psm) { continue; }
        typedef I* (*__Pdyncast)(o_itfset_base* p_itfs);
          __Pdyncast f_dyncast = (__Pdyncast)psm(unity_common::_defsm_pi); if (!f_dyncast) { continue; }
        I* p = f_dyncast(h.p_itfs);
        if (!p) { continue; }
        if (!pret) { return s_long(i); }
        try { pret->push_back(s_long(i)); } catch (...) { return -2; }
      }
    }
    if (pret && pret->size()) { return (*pret)[0]; }
    return -1;
  }






  template<class Obj, class _> unity::_o_sm_t<Obj, _>::_critsec_rc::_critsec_rc(_o_refcnt& x, bool b_enable)
  {
    prccsd = 0; flags = 0;
    if (b_enable && x.rc_pcsdx)
    {
      if (x.rc_flags & 1) { prccsd = (_o_refcnt::_rccsd<Obj>*)x.rc_pcsdx; }
      new (&__stg[0]) t_cs(10, -1, prccsd ? &prccsd->csd :  (typename t_cs::csdata*)x.rc_pcsdx);
      flags |= 1;
      if (prccsd) { ++prccsd->rcnt; }
    }
  }
  template<class Obj, class _> unity::_o_sm_t<Obj, _>::_critsec_rc::~_critsec_rc()
  { if (flags & 1) {
      if (prccsd) { s_long q = prccsd->rcnt; prccsd->rcnt = (q & ~m) | ((q - 1) & m); }
      try { ((t_cs*)&__stg[0])->~t_cs(); } catch (...) {}
      if (prccsd && prccsd->rcnt == ~m) { try { delete prccsd; } catch (...) {} }
  } }
    // After enable_autodel(), the last destroyed lock deletes the referenced crit. sec. data object.
  template<class Obj, class _> void unity::_o_sm_t<Obj, _>::_critsec_rc::enable_autodel()
    { if ((flags & 1) && prccsd) { prccsd->rcnt |= ~m; } }

  template<class Obj, class _> void* unity::_o_sm_t<Obj, _>::pvoid(_o_refcnt* prc, s_long b_checked)
  {
    if (!prc) { return 0; }
    if (!b_checked) { return prc->rc_pobj; }
    if (1) { _critsec_rc __lock(*prc); if (sizeof(__lock)) {} if (prc->rc_strong != o_ref_info::rc_deleted) { return prc->rc_pobj; } }
    return 0;
  }
  template<class Obj, class _> void* unity::_o_sm_t<Obj, _>::prefo(_o_refcnt* prc, const char* tname, s_long t_ind, s_long t_size)
  {
    if (!prc || !tname) { return 0; }
    if (1)
    {
      bool b1 = 0 == std::strcmp(typeid(Obj).name(), tname) && (t_size < 0 || s_long(sizeof(Obj)) == t_size);
      bool b2 = b1 || (t_ind < 0 && t_ind == bytes::type_index_t<Obj>::ind());
      if (b2) { _critsec_rc __lock(*prc); if (sizeof(__lock)) {} if (prc->rc_strong != o_ref_info::rc_deleted) { return prc->rc_pobj; } }
    }
    typedef unity::_deref1_t<Obj> D;
    typedef typename D::t_obj R;
    const char* t1 = "|cref_t|"; (void)t1; const size_t nt1 = 8;
    if (!!D::is_cref_t && 0 == std::strncmp(tname, t1, nt1))
    {
      bool b1 = 0 == std::strcmp(typeid(R).name(), tname + nt1) && (t_size < 0 || s_long(sizeof(R)) == t_size);
      bool b2 = b1 || (t_ind < 0 && t_ind == bytes::type_index_t<R>::ind());
      if (b2) { _critsec_rc __lock(*prc); if (sizeof(__lock)) {} if (prc->rc_strong != o_ref_info::rc_deleted) { return prc->rc_pobj; } }
    }
    if (1)
    {
      bool b1 = 0 == std::strcmp(typeid(R).name(), tname)  && (t_size < 0 || s_long(sizeof(R)) == t_size);
      bool b2 = b1 || (t_ind < 0 && t_ind == bytes::type_index_t<R>::ind());
      if (b2) { _critsec_rc __lock(*prc); if (sizeof(__lock)) {} if (prc->rc_strong != o_ref_info::rc_deleted) { return unity::_deref1_t<Obj>::ptr(prc->rc_pobj); } }
    }
    return 0;
  }
    // flags OR-ed: 1 - get static type info, 2 - get dynamic type info, 4 - get vecm type index (see type_index_t in vecm_hashx.h),
    //  8 - return _pobj even if the object is deleted, 16 - return sizeof(Obj).
  template<class Obj, class _> void unity::_o_sm_t<Obj, _>::obj_type_info(_o_refcnt* prc, o_type_info* pret, s_long flags)
  {
    if (!pret) { return; }
    if (!prc) { new (pret) o_type_info(0, 0, 0, unity_common::pls_modsm(), 0, 0, -1); return; }
    bool b_incl_dyn = !!(flags & 2); bool b_checked = !(flags & 8); bool b_lock = b_incl_dyn || b_checked;
    _critsec_rc __lock(*prc, b_lock); if (sizeof(__lock)) {}
    new (pret) o_type_info(
      (flags & 1) ? typeid(Obj).name() : 0,
      (b_incl_dyn && prc->rc_strong != o_ref_info::rc_deleted) ? typeid(*(Obj*)prc->rc_pobj).name() : 0,
      (flags & 4) ? bytes::type_index_t<Obj>::ind() : 0,
      unity_common::pls_modsm(),
      _o_sm_t::sm,
      (!b_checked || prc->rc_strong != o_ref_info::rc_deleted) ? prc->rc_pobj : 0,
      (flags & 16) ? sizeof(Obj) : 0
    );
  }
    // (flags & 3) -- informs obj_ref_info on the kind of reference (for complete return value):
    //    == 3: prc->rc_pobj is strong-referenced.
    //    == 2: prc->rc_pobj is weak-referenced.
  template<class Obj, class _> void unity::_o_sm_t<Obj, _>::obj_ref_info(_o_refcnt* prc, o_ref_info* pret, s_long flags)
  {
    if (!pret) { return; }
    if (!prc) { new (pret) o_ref_info(0, 0, 0); return; }
    _critsec_rc __lock(*prc); if (sizeof(__lock)) {}
    new (pret) o_ref_info(prc->rc_strong, prc->rc_weak, (flags & 2) ? ((flags & 1) ? 3 : 2) : 0);
  }
    // pstg must point to 32 bytes storage space, aligned to 8 bytes.
    //    flags: 1 - enable lock (0 will create a non-locking object). Other flags == 0.
    //  At crit. sec. end, the client must call lock_destroy(pstg).
  template<class Obj, class _> void unity::_o_sm_t<Obj, _>::lock_create(meta::s_ll* pstg, _o_refcnt* prc, s_long flags)
    { if (!(prc && pstg)) { return; } new (pstg) _critsec_rc(*prc, (flags & 1)); }
    // Returns lock state/type:
    //  2 - pstg is valid, lock type - per-object.
    //  1 - pstg is valid, lock type - per-type.
    //  0 - pstg is valid, lock type == no lock.
    //  -1 - pstg is not valid.
    // NOTE Ret. vals. 0..2 are same as unity::objt lkt parameter.
  template<class Obj, class _> s_long unity::_o_sm_t<Obj, _>::lock_state(const meta::s_ll* pstg)
    { if (!pstg) { return -1; } _critsec_rc& cs = *(_critsec_rc*)pstg; if (cs.flags & 1) { return cs.prccsd ? 2 : 1; } return 0; }
  template<class Obj, class _> void unity::_o_sm_t<Obj, _>::lock_destroy(meta::s_ll* pstg)
    { if (!pstg) { return; } ((_critsec_rc*)pstg)->~_critsec_rc(); }
    // pobj must be != 0.
    // (flags & 1): create _o_refcnt + set pobj as 1st strong reference.
    // (flags & 2): create _o_refcnt + set pobj as 1st weak reference.
    // lkt: lock type (0 - none, 1 - critsec_t<Obj>, 2 - critsec_t<Obj> with individual csdata), see set_obj
  template<class Obj, class _> _o_refcnt* unity::_o_sm_t<Obj, _>::rc_create(Obj* pobj, s_long flags, s_long lkt)
  {
    if (!pobj || (flags & 3) == 3 || (flags & 3) == 0) { return 0; }
    bool b_strong = !!(flags & 1);

    cref_t<_o_refcnt, cref_nonlock> _rrc; _rrc.create0(1); if (!_rrc) { return 0; }
    cref_t<_o_refcnt::_rccsd<Obj>, cref_nonlock> _rcsd2; if (lkt == 2) { _rcsd2.create0(1); if (!_rcsd2) { return 0; } }

    _o_refcnt* z_rc = _rrc._pnonc_u(); _rrc._detach_u();
    z_rc->rc_flags = 0;
    z_rc->rc_pobj = const_cast<typename meta::noncv_t<Obj>::t*>(pobj);
    z_rc->rc_posm = &_o_sm_t<Obj>::sm;
    if (b_strong) { z_rc->rc_strong = 1; }
      else { z_rc->rc_weak = 1; }
    if (lkt == 1) { z_rc->rc_pcsdx = typename critsec_t<Obj>::ff_mc().pdefcsd(); }
      else if (lkt == 2) { z_rc->rc_pcsdx = _rcsd2._pnonc_u(); _rcsd2._detach_u(); z_rc->rc_flags |= 1; }
      else { z_rc->rc_pcsdx = 0; } // lkt == 0

    return z_rc;
  }
    // Allowed: (+strong), (-strong), (+strong -weak), (-strong +weak), (+weak), (-weak).
    //  (flags & 1): add 1 strong reference.
    //  (flags & 2): release 1 strong reference.
    //  (flags & 4): add 1 weak reference.
    //  (flags & 8): release 1 weak reference.
    // Returns:
    //  (retval & 1) != 0: 1 strong reference added.
    //  (retval & 2) != 0: 1 strong reference released.
    //  (retval & 4) != 0: 1 weak reference added.
    //  (retval & 8) != 0: 1 weak reference released.
    //  (retval & 0x10) != 0 (only on strong reference release): prc->rc_pobj is deleted (the last strong reference released).
    //  (retval & 0x20) != 0 (only on any reference release): *prc is deleted (all references are removed).
    //  retval == 0 - nothing done (prc == 0, or failed to add strong reference, or invalid flags).
  template<class Obj, class _> s_long unity::_o_sm_t<Obj, _>::rc_incdec(_o_refcnt* prc, s_long flags)
  {
    if (!prc) { return 0; }
    s_long retval = 0;
    if (flags & 1) // +strong...
    {
      if (flags & 6) { return 0; }
      typename _o_sm_t<Obj>::_critsec_rc __lock(*prc); if (sizeof(__lock)) {}
      if (prc->rc_strong < 0) { return 0; }
      if (flags & 8) { if (prc->rc_weak <= 0) { return 0; } --prc->rc_weak; retval |= 8; }
      ++prc->rc_strong; retval |= 1;
      return retval;
    }
    else if (flags & 2) // -strong...
    {
      if (flags & 8) { return 0; }
      typename _o_sm_t<Obj>::_critsec_rc __lock(*prc); if (sizeof(__lock)) {}
      if (prc->rc_strong <= 0) { return 0; }
      if (flags & 4) { if (prc->rc_weak < 0) { return 0; } ++prc->rc_weak; retval |= 4; }
      --prc->rc_strong; retval |= 2;
      if (prc->rc_strong == 0) { try { delete (Obj*)prc->rc_pobj; } catch (...) {} prc->rc_strong = o_ref_info::rc_deleted; retval |= 0x10; }
      if (prc->rc_strong <= 0 &&  prc->rc_weak <= 0) { __lock.enable_autodel(); try { delete prc; } catch (...) {} retval |= 0x20; }
      return retval;
    }
    else if (flags & 4) // +weak
    {
      if (flags & 8) { return 0; }
      typename _o_sm_t<Obj>::_critsec_rc __lock(*prc); if (sizeof(__lock)) {}
      if (prc->rc_weak < 0) { return 0; }
      ++prc->rc_weak; retval |= 4;
      return retval;
    }
    else if (flags & 8) // -weak only
    {
      typename _o_sm_t<Obj>::_critsec_rc __lock(*prc); if (sizeof(__lock)) {}
      if (prc->rc_weak <= 0) { return 0; }
      --prc->rc_weak; retval |= 8;
      if (prc->rc_strong <= 0 &&  prc->rc_weak <= 0) { __lock.enable_autodel(); try { delete prc; } catch (...) {} retval |= 0x20; }
      return retval;
    }
    return 0;
  }
  template<class Obj, class _> void* unity::_o_sm_t<Obj, _>::sm(s_long ind) __bmdx_noex
  {
    enum { size = unity_common::sizeof_osm };
    static void* smt[size] =
    {
      (void*)unity_common::pls_modsm(),
      (void*)pvoid,
      (void*)prefo,
      (void*)obj_type_info,
      (void*)obj_ref_info,
      (void*)lock_create,
      (void*)lock_destroy,
      (void*)rc_create,
      (void*)rc_incdec,
      (void*)lock_state,
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }


  template<class Itfs, class _> o_itfset_base* unity::_o_itfssm_t<Itfs, _>::itfs_new() __bmdx_noex
    { try { o_itfset_base* p = new _o_itfs_topmost<Itfs>(); p->__pidyn = 0; return p; } catch (...) {} return 0; }
    // NOTE __pidyn of the new copy is set to 0. Should be initialized to point to the new set's container.
  template<class Itfs, class _> o_itfset_base* unity::_o_itfssm_t<Itfs, _>::itfs_new_copy(o_itfset_base* x0) __bmdx_noex
    { try { if (!x0) { return 0; } _o_itfs_topmost<Itfs>* x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); if (!x) { return 0; } o_itfset_base* p = new _o_itfs_topmost<Itfs>(*x); p->__pidyn = 0; return p; } catch (...) {} return 0; }
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::itfs_delete(o_itfset_base* x) __bmdx_noex
    { if (!x) { return; } try { delete x; } catch (...) {} }
    // flags OR-ed: 1 - get static type info, 2 - get dynamic type info, 4 - get vecm type index (see type_index_t in vecm_hashx.h),
    //  8 - return _pobj even if the object is deleted,
    //  16 - return sizeof(Obj).
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::itfs_type_info(o_itfset_base* x, o_type_info* pret, s_long flags) __bmdx_noex
  {
    if (!pret) { return; }
    bool b_incl_dyn = !!(flags & 2);
    new (pret) o_type_info(
      (flags & 1) ? typeid(Itfs).name() : 0,
      b_incl_dyn && x ? typeid(*x).name() : 0,
      (flags & 4) ? bytes::type_index_t<Itfs>::ind() : 0,
      unity_common::pls_modsm(),
      _o_itfssm_t::sm,
      x,
      (flags & 16) ? sizeof(Itfs) : 0
    );
  }
    // __xx_after_construct, __xx_before_destroy
    //  (flags & 1):
    //      == 0: the set is constructed / will be destroyed as part of full sets list construction / destruction.
    //      == 1: the set is constructed / will be destroyed individually in the existing list of sets.
    // Returns: 1 - success, 0 - failure.
  template<class Itfs, class _> s_long unity::_o_itfssm_t<Itfs, _>::__xx_after_construct(o_itfset_base* x0, o_itfset_base* src0, s_long flags) __bmdx_noex
    { _o_itfs_topmost<Itfs>* x = 0; Itfs* src = 0; try { if (x0) { x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); } if (src0) { src = (_o_itfs_topmost<Itfs>*)src0->__itfs_cast(typeid(Itfs).name()); } } catch (...) {} if (!x) { return false; } if (src0 && !src) { return false; } bool b = false; s_long n = 0; try { b = x->__xx_after_construct(flags, src0, n); } catch (...) {} if (!b) { try { s_long n2 = 0; x->__xx_before_destroy(flags, n2, n); } catch (...) {} } return b; }
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::__xx_before_destroy(o_itfset_base* x0, s_long flags) __bmdx_noex
    { try { _o_itfs_topmost<Itfs>* x = 0; if (x0) { x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); } if (!x) { return; } s_long n = 0; x->__xx_before_destroy(flags, n, -1); } catch (...) {} }
  template<class Itfs, class _> void* unity::_o_itfssm_t<Itfs, _>::sm(s_long ind) __bmdx_noex
  {
    enum { size = unity_common::sizeof_itfssm };
    static void* smt[size] =
    {
      (void*)unity_common::pls_modsm(),
      (void*)itfs_new,
      (void*)itfs_new_copy,
      (void*)itfs_delete,
      (void*)itfs_type_info,
      (void*)__xx_after_construct,
      (void*)__xx_before_destroy,
    };
    return ind >= 0 && ind < size ? smt[ind] : 0;
  }

    // When b_noinherit == false:
    //    the o_proxy<I> specialization should define:
    //      1) static const char* __iname() { return "developer / program_or_library_name / interface_name / version"; }
    //      2) Override I's virtual functions, to return __call<__method_name>()(__psi(), method args ... ).
    //      3) For each each method,
    //        struct __method_name {
    //          typedef method_ret_type (*PF)(__Interface* __pi,  method args ... );
    //          method_ret_type F(__Interface* __pi,  method args...) { return __pi->method_name(method args ... ); }
    //        };
    //      4) typedef unity_common::fn_list<__method1_name, __method2_name ... > __Methods;
    //
    // When b_noinherit == true and Prx is a custom class:
    //    For other binary modules, remaps some interface X with name given by Prx::__iname()
    //    to I in custom way, defined in Prx. X is not needed to be declared in the current module at all.
    //    Neither X, nor I are inherited by o_proxy_base.
    //    Prx contains only non-trivial static methods (__method_name::F, __Methods typedef),
    //    implementing X in the current module. X should be correctly emulated by means of I.
    //    The number, type of args., and ret. val. of methods, listed in __Methods, must be exactly same as declared
    //    by usual o_proxy<X> in other modules.
    //  NOTE Such remapping may solve two tasks:
    //    1. Interface versioning. E.g. let I is the newer improved version of X, and still may somehow function as X,
    //      but the developer doesn't want to attach both I and X to all objects for backward compatibility.
    //      It is enough to at once redefine o_proxy<I>::__iname() to return smth. different
    //      from the former value (version "2.0" or such),
    //      + define custom Prx for X with original X iname, as described above.
    //    2. Interface emulation as such. Same as (1), but for case when interface pointer I
    //      and its object are quite different from interface X, used in other binary module's context.
  template<class I, class Prx = o_proxy<I>, bool b_noinherit = false, class _ = __vecm_tu_selector> struct o_proxy_base : meta::if_t<b_noinherit, meta::nothing, I>::result
  {
    typedef I __Interface;
    typedef Prx __Proxy;

    o_proxy_base() : __psm(0), __pi(0) { __smt(); }
    virtual ~o_proxy_base() {}
    static void* __sm(s_long ind) { return ind >= 0 && ind < __Proxy::__Methods::fn_cnt ? __smt()[ind] : 0; }
    I* __pci() const { return const_cast<o_proxy_base*>(this); } // client-side interface ptr.
    I* __psi() const { return __pi; } // server-side interface ptr.
    void __set_pp(unity_common::__Psm psm, I* pi) __bmdx_noex { __psm = psm; __pi = pi; }

    template<class T> typename T::PF __call() const
    {
      struct exc_call : std::exception {  _fls75 msg; exc_call() { msg = msg + "o_proxy_base __call " + typeid(I).name() + ' ' + typeid(T).name(); } const char* what() const __bmdx_noex { return msg.c_str(); } };
      typedef typename T::PF t_f;
      t_f f = (t_f)__psm(unity_common::find_type<T, typename __Proxy::__Methods>::ind);
      if (!f) { throw exc_call(); }
      return f;
    }

  private:
    unity_common::__Psm __psm; __Interface* __pi; // not used by o_proxy object when it's role is server
    static void** __smt()
    {
      static void* x[__Proxy::__Methods::fn_cnt] = { 0 };
        // The following may be safely called several times in MT env.
      if (x[0] == 0)
      {
        for (s_long i = 1; i < unity_common::sizeof_defsm; ++i) { x[i] = 0; }
        __Proxy::__Methods::Freg(x);
        x[0] = (void*)_ls_dyncast;
        unity_common::_ls_psm_set(__Proxy::__iname(), &__sm);
      }
      return x;
    }
    static void* _ls_dyncast(o_itfset_base* pibase) { I* p = 0; try { p = (I*)pibase->__ifind(typeid(I).name()); } catch (...) {} return p; }
  };

  template<class T> T* o_interfaces<T>::__pobj(s_long flags, meta::noarg_tu_t<T>) const
    { return unity::o_api(*this->__pidyn).pobj<T>(flags); }

  #if defined(__GNUC__) || defined(__clang__)
    template<class I> o_iptr_t<I> o_itfset_base::__pinterface(s_long flags, meta::noarg_tu_t<I>) const
  #else
    template<class I> o_iptr_t<I> o_itfset_base::__pinterface(s_long flags = 0x1f, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const
  #endif
      { return unity::o_api(*this->__pidyn).pinterface<I>(flags, this); }




    // Implementation-dependent internal hashlist interface.
  struct unity::_hl_i
  {
    struct exc_consistency {}; struct exc_subscript {};
    typedef yk_c::hashx<unity, unity, kf_unity> t_hash;
    typedef t_hash::entry entry;


    s_long n() const __bmdx_noex;
    unity& opsub(const unity& k __bmdx_noarg) __bmdx_exs(exc_subscript);
    const entry* operator() (s_long ind) const __bmdx_noex;
    const entry* find(const unity& k, s_long* ret_pind = 0) const __bmdx_noex;
    s_long insert(const unity& k, const entry** ret_pentry = 0, s_long* ret_pind = 0, s_long ind_before = hashx_common::no_elem) __bmdx_noex;
    s_long remove(const unity& k) __bmdx_noex;
    s_long remove_i(s_long ind) __bmdx_noex;
    const entry* h(s_long ind_h) const __bmdx_noex;
    s_long hl_clear(bool full) __bmdx_noex;
    const t_hash::f_kf* pkf() const __bmdx_noex;
    bool hl_set_kf(const t_hash::f_kf& kf) __bmdx_noex;

    s_long qi_noel() const __bmdx_noex;
    s_long qi_next(s_long pos) const __bmdx_noex;
    s_long qi_prev(s_long pos) const __bmdx_noex;
    s_long qi_indk(s_long ind) const __bmdx_noex;

    s_long compatibility() const __bmdx_noex;
    void assign_impl(const _hl_i& x);

  protected: unity_common::__Psm pmsm; // == pmsm of the containing unity object (factual _hl_i object may not be moved between wrappers)
    _hl_i(); _hl_i(const _hl_i&); void operator=(const _hl_i&);
  };




    //==  Write string representation of unity object to std ostream.
  std::ostream& operator<<(std::ostream& s, const unity::iofmt& f);
  std::ostream& operator<<(std::ostream& s, const unity& x);
  std::wostream& operator<<(std::wostream& s, const unity::iofmt& f);
  std::wostream& operator<<(std::wostream& s, const unity& x);

  std::ostream& operator<<(std::ostream& s, const unity::_wr_cstring& x);
  std::ostream& operator<<(std::ostream& s, const unity::_wr_wstring& x);
  std::wostream& operator<<(std::wostream& s, const unity::_wr_cstring& x);
  std::wostream& operator<<(std::wostream& s, const unity::_wr_wstring& x);




  // Implementation part.
namespace
{
  struct _unitypc2;
  struct unitypc
  {
  private: const unity* _p;
  public:
    unitypc(const unity* p_)         __bmdx_noex : _p(p_) {}
    unitypc(const unity& x)         __bmdx_noex : _p(&x) {}
    unity* _pnonc() const         __bmdx_noex { return const_cast<unity*>(_p); }

    bool b_valid() const { return !!_p; }

    unity& operator[] (const unity& ki) { check(); return (*_pnonc())[ki]; }
    const unity& operator[] (const unity& ki) const { check(); return (*_pnonc())[ki]; }

    unity* operator->() { check(); return _pnonc(); }
    const unity* operator->() const { check(); return _pnonc(); }

    unity& operator*() { check(); return *_pnonc(); }
    const unity& operator*() const { check(); return *_pnonc(); }

    bool operator==(const unitypc& p2) const         __bmdx_noex { return _p == p2._p; }
    bool operator==(const unity* p2) const         __bmdx_noex { return _p == p2; }
    bool operator!=(const unitypc& p2) const         __bmdx_noex { return _p == p2._p; }
    bool operator!=(const unity* p2) const         __bmdx_noex { return _p != p2; }

    operator bool() const         __bmdx_noex { return !!_p; }

    operator unity*()         __bmdx_noex { return _pnonc(); }
    operator const unity*() const         __bmdx_noex { return _pnonc(); }

    operator unity&() { return **this; }
    operator const unity&() const { return **this; }

      // Using objPtr, retrieves T* (without const) from the referenced object.
      //  Returns T* on success, 0 on failure.
    template<class T> operator T*()         __bmdx_noex { typedef typename meta::nonc_t<T>::t t; t* px = 0; if (_p) { px = _pnonc()->objPtr<t>(); if (!px) { px = (t*)_pnonc()->objPtr_c<const T>(); } } return px; }
    template<class T> operator const T*() const         __bmdx_noex { typedef typename meta::nonc_t<T>::t t; const T* px = 0; if (_p) { px = _pnonc()->objPtr_c<t>(); if (!px) { px = _pnonc()->objPtr_c<const T>(); } } return px; }

      // Using objPtr, retrieves T* (without const) from the referenced object.
      //  Returns T& on success, or generates an exception if T* is 0.
    template<class T> operator T&()        { typedef typename meta::nonc_t<T>::t t; t* px = 0; if (_p) { px = _pnonc()->objPtr<t>(); if (!px) { px = (t*)_pnonc()->objPtr_c<const T>(); } } return *unity::checked_ptr<T>(px); }
    template<class T> operator const T&() const        { typedef typename meta::nonc_t<T>::t t; const T* px = 0; if (_p) { px = _pnonc()->objPtr_c<t>(); if (!px) { px = _pnonc()->objPtr_c<const T>(); } } return *unity::checked_ptr<const T>(px); }

    void check() const __bmdx_exs(XUExec) { if (!_p) { throw XUExec("unitypc.p == 0"); } }
    template<class T> inline _unitypc2 operator/ (const T& path) const;
  };

  struct _unitypc2 : unitypc
  {
    _unitypc2(const unity* p_) : unitypc(p_) {}
    _unitypc2(const unity& x) : unitypc(&x) {}
    _unitypc2(const unitypc& x_) : unitypc(x_) {}

    template<class T> inline T operator/ (T dflt) const;
    inline unity::_wr_cstring operator/ (const char* dflt) const;
    inline unity::_wr_wstring operator/ (const wchar_t* dflt) const;

    template<class T> inline T operator% (T dflt) const;
    inline unity::_wr_cstring operator% (const char* dflt) const;
    inline unity::_wr_wstring operator% (const wchar_t* dflt) const;
  };

    // Smart (weak) pointer for advanced functionality
    //    (automatic objPtr, 0-ptr checks, "+x / key / dflt" operator).
  inline unitypc operator+(const unity& x) __bmdx_noex { return x; }

    // Check if the given object contains certain path.
    //  Usage example:
    //    unity h; ... if (+h * "root|branch|instance") { ... }
    // Returns this if it is an associative array containing the given path.
    //  Otherwise returns 0.
  template<class T>
  unitypc operator* (const unitypc& x, const T& path)
    { if (!x) { return 0; } return x->path(path) ? x : unitypc(0); }

    // Returns a pointer to the object, referenced by the given path, if it exists.
    //  Otherwise returns 0.
  template<class T>
  _unitypc2 unitypc::operator/ (const T& path) const
    { if (!*this) { return 0; } return (**this).path(path).p; }

    // Attempts to convert the referenced value to T.
    //    If the value does not exist, dflt is returned.
    //    If the value cannot be converted (rules are that of unity::val()), throws an exception.
    //  Usage examples:
    //    unity h; ... if (+h / "root|branch|instance|string key" / "N/A") { ... } // returns string
    //    unity h; ... if (+h / "root|branch|instance|string key" / L"N/A") { ... } // returns wstring
    //    unity h; ... if (+h / "root|branch|instance|string key" / string("N/A")) { ... }
    //    unity h; ... if (+h / "root|branch|instance|string key" / wstring("N/A")) { ... }
    //    unity h; ... if (+h / "root|branch|instance|integer key" / -1ll) { ... }
    //    unity h; ... if (+h / "root|branch|instance|float key" / -1.) { ... }
    //    To get non-default value, h should be any kind of tree - hashlist, map, array, containing other objects recursively.
  template<class T>
  T _unitypc2::operator/ (T dflt) const
    { if (!*this) { return dflt; } typedef unity::trivconv_t<typename meta::nonc_t<T>::t> t_conv; return t_conv::Fback((**this).template val<typename t_conv::t_target>()); }

    // Similar to operator/ above.
    //  Attempts to convert the referenced value to T.
    //    If the value does not exist, dflt is returned.
    //    If the value cannot be converted (rules are that of unity::val()), dflt is returned as well.
    //    Exceptions in most cases do not occur (only theoretically, on constructing copy of dflt).
    //  Usage examples:
    //    unity h; ... if (+h / "root|branch|instance|integer key" % -1ll) { ... }
    //    unity h; ... if (+h / "root|branch|instance|float key" % -1.) { ... }
    //    To get non-default value, h should be any kind of tree - hashlist, map, array, containing other objects recursively.
    // NOTE For strings % behaves exactly like /.
  template<class T>
  T _unitypc2::operator% (T dflt) const
    { if (!*this) { return dflt; } typedef unity::trivconv_t<typename meta::nonc_t<T>::t> t_conv; try { return t_conv::Fback((**this).template val<typename t_conv::t_target>()); } catch (...) {} return dflt; }

  unity::_wr_cstring _unitypc2::operator/ (const char* dflt) const { if (!*this) { return unity::_wr_cstring(dflt); } return unity::_wr_cstring((**this).vcstr()); }
  unity::_wr_wstring _unitypc2::operator/ (const wchar_t* dflt) const { if (!*this) { return unity::_wr_wstring(dflt); } return unity::_wr_wstring((**this).vstr()); }
  unity::_wr_cstring _unitypc2::operator% (const char* dflt) const { if (!*this) { return unity::_wr_cstring(dflt); } return unity::_wr_cstring((**this).vcstr()); }
  unity::_wr_wstring _unitypc2::operator% (const wchar_t* dflt) const { if (!*this) { return unity::_wr_wstring(dflt); } return unity::_wr_wstring((**this).vstr()); }
}




  //==  Utilities.

    // Full path and filename of this exe (calculated once).
    // NOTE POSIX: it is assumed that original program path was encoded as UTF-8.
  std::wstring cmd_myexe();
    // Command line, without executable path/filename.
    //  b_quoting:
    //        a) true: get command line as array of arguments, quote each argument, then concatenate all using space character.
    //        b) false / command line is available only as array of arguments: concatenate arguments as they are, using space character.
    //        c) false / command line is available as whole: return arguments string exactly as supplied on program start.
    // NOTE POSIX: it is assumed that original program arguments were encoded as UTF-8.
  std::wstring cmd_string(bool b_quoting = true);
    // argv (0-based string array of command line arguments, size >= 1;
    //    0 - program name (not exactly executable name/path),
    //    1..* - dequoted arguments).
    // NOTE POSIX: it is assumed that original program arguments were encoded as UTF-8.
  unity cmd_array();

    // Prepare single command line argument with properly escaped and replaced characters.
    //  See original processctl::ff_mc::arg1() for details.
  std::string cmd_arg1(const arrayref_t<char>& s, bool b_shell);
  std::string cmd_arg1(const arrayref_t<wchar_t>& s, bool b_shell);

  const std::string cCR = "\r";
  const std::string cLF = "\n";
  const std::string cCRLF = "\r\n";
  const std::wstring wCR = L"\r";
  const std::wstring wLF = L"\n";
  const std::wstring wCRLF = L"\r\n";

  std::string cpathsep(); // single path separator character
  std::wstring wpathsep(); // single path separator character
  std::string cpathsep2(); // two path separator characters
  std::wstring wpathsep2(); // two path separator characters


  //== Wide character string helper functions.

    // (locale-aware)
    // Wide character string comparison (s1>s2: 1, s1==s2: 0, s1<s2: -1)
    // loc_type (only for b_case_insensitive == true):
    //    0: use the currently set locale (std::setlocale(..., 0)) to compare strings.
    //    1: use system-default locale (equiv. to std::setlocale(..., "")) to compare strings.
    //    2: use C locale (equiv. to std::setlocale(..., "C")) to compare strings.
    //    any other value: return an empty string.
  s_long wscompare(arrayref_t<wchar_t> s1, arrayref_t<wchar_t> s2, bool ignore_case, s_long loc_type = 1);
    //
    // (locale-aware)
    // Wide character string replacement.
    // Sequentially replace 'from' string occurrences in 's' by 'to',
    //    returns the result.
    // If nmax >= 0, no more than nmax leftmost replacements are done.
    // loc_type (only for b_case_insensitive == true):
    //    0: use the currently set locale (to std::setlocale(..., 0)) to search for substrings.
    //    1: use system-default locale (equiv. to std::setlocale(..., "")) to search for substrings.
    //    2: use C locale (equiv. to std::setlocale(..., "C")) to search for substrings.
    //    any other value: return an empty string.
  std::wstring replace(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& from, const arrayref_t<wchar_t>& to, bool b_case_insensitive = false, s_ll nmax = -1, s_long loc_type = 1);
    //
    // (locale-aware)
    // Convert s to lowercase (lcase_la) or uppercase (ucase_la) representation.
    // loc_type:
    //    0: use the currently set locale (std::setlocale(..., 0)).
    //    1: use system-default locale (equiv. to std::setlocale(..., "")).
    //    2: use C locale (equiv. to std::setlocale(..., "C")).
    //    any other value: return an empty string.
  std::wstring lcase_la(const arrayref_t<wchar_t>& s, s_long loc_type = 1);
  std::wstring ucase_la(const arrayref_t<wchar_t>& s, s_long loc_type = 1);
    //
    // (locale-independent)
    // true if str matches the given pattern.
    //  See arrayref_t :: string_like.
    //  Recognizes constructs similar to: [a-zA-Z], *, ?, # (i.e. digit)
  inline bool wstring_like(const arrayref_t<wchar_t>& str, const arrayref_t<wchar_t>& pattern __bmdx_noarg)
    { return str.string_like(pattern); }
    //
    // (locale-independent)
    // Trim all 'swat' string occurrences in the beginning and end of the string 's'
  std::wstring trim(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& swhat = L" ", bool b_left = true, bool b_right = true);
    //
    // (locale-independent)
    // Sequentially split s into 0-based array of strings, based on delimiter (delim).
    // nmax: max number of string parts in the output array. nmax < 0 means "no limit".
  unity split(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& delim, meta::s_ll nmax = -1); // returns 0-based array of utString
  std::vector<std::wstring> splitToVector(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& delim, meta::s_ll nmax = -1);
    //
    // (locale-independent)
    //  If asrc is an array, join string representations of its elements (asrc.vstr(i)), inserting delim between them.
    //  If asrc is not an array, join() returns asrc.vstr().
  std::wstring join(const unity& asrc, const arrayref_t<wchar_t>& delim);
  std::wstring joinVector(const std::vector<std::wstring>& asrc, const arrayref_t<wchar_t>& delim);


  //== 1-byte character string helper functions.

    // (using C locale)
    // Sequentially replace 'from' string occurrences in 's' by 'to',
    //    returns the result.
    // If b_case_insensitive == true, the search of 'from' in s is based on C locale.
    // If b_case_insensitive == false, replace_c is locale-independent.
  std::string replace_c(const arrayref_t<char>& s, const arrayref_t<char>& from, const arrayref_t<char>& to, bool b_case_insensitive = false, s_ll nmax = -1);
    //
    // (using C locale)
    // Convert s to lowercase (lcase_c) or uppercase (ucase_c) representation.
  std::string lcase_c(const arrayref_t<char>& s);
  std::string ucase_c(const arrayref_t<char>& s);
    //
    // (locale-independent)
    // Trim all 'swat' string occurrences in the beginning and end of the string 's'
  std::string trim(const arrayref_t<char>& s, const arrayref_t<char>& swhat = " ", bool b_left = true, bool b_right = true);
    //
    // (locale-independent)
    // Sequentially split s into 0-based array of strings, based on delimiter (delim).
    // nmax: max number of string parts in the output array. nmax < 0 means "no limit".
  unity split(const arrayref_t<char>& s, const arrayref_t<char>& delim, meta::s_ll nmax = -1); // returns 0-based array of utString
  std::vector<std::string> splitToVector(const arrayref_t<char>& s, const arrayref_t<char>& delim, meta::s_ll nmax = -1);
    //
    // (using system-default locale)
    //  If asrc is an array, join string representations of its elements (asrc.vcstr(i)), inserting delim between them.
    //  If asrc is not an array, join() returns asrc.vcstr().
    //  See also unity :: vcstr().
  std::string join(const unity& asrc, const arrayref_t<char>& delim);
  std::string joinVector(const std::vector<std::string>& asrc, const arrayref_t<char>& delim);



    // Date/time generators.
  _unitydate d_datetime(s_long y, s_long m, s_long d, s_long h = 0, s_long min = 0, s_long s = 0);
  _unitydate d_time(s_long h, s_long m, s_long s);
  _unitydate d_now(bool allow_fracsec = false); // returns the current local time
  _unitydate d_nowutc(bool allow_fracsec = false); // returns the current UTC time

    // Returns a copy of selected argument value. ind is 1-based.
    //  If ind is out of range or selects a default argument, utEmpty is returned.
  unity choose(s_long ind, const unity& x1=unity::_0, const unity& x2=unity::_0, const unity& x3=unity::_0, const unity& x4=unity::_0, const unity& x5=unity::_0, const unity& x6=unity::_0, const unity& x7=unity::_0, const unity& x8=unity::_0, const unity& x9=unity::_0, const unity& x10=unity::_0, const unity& x11=unity::_0, const unity& x12=unity::_0, const unity& x13=unity::_0, const unity& x14=unity::_0, const unity& x15=unity::_0, const unity& x16=unity::_0, const unity& x17=unity::_0, const unity& x18=unity::_0, const unity& x19=unity::_0, const unity& x20=unity::_0, const unity& x21=unity::_0, const unity& x22=unity::_0, const unity& x23=unity::_0, const unity& x24=unity::_0, const unity& x25=unity::_0, const unity& x26=unity::_0, const unity& x27=unity::_0, const unity& x28=unity::_0, const unity& x29=unity::_0,
    const unity& x30=unity::_0);




  //==  Utility for iterating through utMap, utHash, utUnityArray, and scalar.

    // Iterates through the container, referencing its particular element with pair
    //    (const unity& first, const unity& second).
    //  a) for associative array, (first, second) are (key, value).
    //  b) for utUnityArray, (first, second) both point to the same array element itself.
    //  c) for scalar, the only one position is valid, like an array with one element.
    //    (first, second) point to the same scalar value.
    //  d) for arrays of type other than utUnityArray, (first, second) cannot be used,
    //    but the array can still be iterated, and indexed with uiterate::pos,
    //    namely, by x.val<T>(pos), x.ref<T>(pos), or any other like function.
    //    T may be elem. type, or elem. or array type index ut*.
    //    Details: see unity::val, unity::ref etc.
    // Usage:
    //    for (uiterate x(my_object); x; ++x) { ... x.key()  x->first ... x.value()  x->second ...  }
    //    for (auto x : uiterate(my_object)) { ... x.first ... x.second ...  }
  struct uiterate
  {
    const unity& x;

      // _t: -2 - non-unity array, -1 - utUnityArray, 0 - scalar or object, 1 - assoc. array
    s_long pos; signed char _t, _dir, _pad1, _pad2;
    private: s_ll& _rdata() const { return *(s_ll*)&pos; }
    public:

      // "a"        - forward iterator at before-begin pos.
      // "b" or ""  - forward iterator at begin pos.
      // "d"        - forward iterator at last pos.
      // "e"        - forward iterator at end (last + 1) pos.
      //
      // "-a"       - reverse iterator at the end of x_.
      // "-b" or "-" - reverse iterator at the last element of x_.
      // "-d"       - reverse iterator at the first element of x_.
      // "-e"       - reverse iterator at before-begin pos. in x_.
      // Reverse iterator position and movement work as like the container was mirrored
      //  (swap([0],[n-1]), swap([1],[n-2]) ...),
      //  and the result iterated normally, with forward iterator.
    uiterate(const unity& x_, const char* spec = "b")
      : x(x_)
      {
        _rdata() = 0;
        _set_utype();
        if (!spec) { spec = ""; }
        bool b_fwd = spec[0] != '-';
        char tpos = spec[b_fwd ? 0 : 1]; if (!(tpos == 'a' || tpos == 'd' || tpos == 'e')) { tpos = 'b'; }
        _dir = b_fwd ? 1 : -1;
        if (b_fwd)
        {
          if (_t == 1) { if (tpos == 'b') { pos = x_.assocl_first(); } else if (tpos == 'd') { pos = x_.assocl_last(); } else { pos = x_.assocl_noel(); } }
            else if (_t < 0) { if (tpos == 'b') { pos = x_.arrlb(); } else if (tpos == 'e') { pos = x_.arrub() + 1; } else if (tpos == 'a') { pos = x_.arrlb() - 1; } else { pos = x_.arrub(); } }
            else { if (tpos == 'b' || tpos == 'd') { pos = 0; } else { pos = 1; } }
        }
        else
        {
          if (_t == 1) { if (tpos == 'b') { pos = x_.assocl_last(); } else if (tpos == 'd') { pos = x_.assocl_first(); } else { pos = x_.assocl_noel(); } }
            else if (_t < 0) { if (tpos == 'b') { pos = x_.arrub(); } else if (tpos == 'e') { pos = x_.arrlb() - 1; } else if (tpos == 'a') { pos = x_.arrub() + 1; } else { pos = x_.arrlb(); } }
            else { if (tpos == 'b' || tpos == 'd') { pos = 0; } else { pos = 1; } }
        }
      }
    uiterate(const unity& x_, s_long pos_, bool b_forward)        : x(x_) { _rdata() = 0; pos = pos_; _dir = b_forward ? 1 : -1; _set_utype(); }
    uiterate(const uiterate& x_)                                  : x(x_.x) { _rdata() = x_._rdata(); }

    uiterate& operator=(const uiterate& x_)       { new (this) uiterate(x_); return *this; }

    uiterate& operator++()                        { if (_t == 1) { pos = _dir == 1 ? x.assocl_next(pos) : x.assocl_prev(pos); } else if (_t < 0) { pos += _dir; } else { pos ^= 1; } return *this; }
    uiterate& operator--()                        { if (_t == 1) { pos = _dir == 1 ? x.assocl_prev(pos) : x.assocl_next(pos); } else if (_t < 0) { pos -= _dir; } else { pos ^= 1; } return *this; }

      // Loop through all elements with operator++: from begin() to and not including (while !=) end()
    uiterate begin() const                        { return uiterate(x, _dir == 1 ? "b" : "-b"); }
    uiterate end() const                          { return uiterate(x, _dir == 1 ? "e" : "-e"); }
      // Loop through all elements with operator--: from d() to and not including (while !=) a()
    uiterate a() const                        { return uiterate(x, _dir == 1 ? "a" : "-a"); }
    uiterate d() const                        { return uiterate(x, _dir == 1 ? "d" : "-d"); }

    bool operator==(const uiterate& y) const      { return &x == &y.x && _rdata() == y._rdata(); }
    bool operator!=(const uiterate& y) const      { return !(*this == y); }

    operator bool() const               { if (_t == 1) { return pos != x.assocl_noel(); } if (_t < 0) { return _dir == 1 ? pos != x.arrub() + 1 : pos != x.arrlb() - 1; } return pos != 1; }

    const unity& key() const            { if (_t == 1) { return x.assocl_key(pos); } else if (_t == 0 && pos == 0) { return x; } return x[pos]; }
    const unity& value() const          { return _vnc(); }

    struct ref1
    {
      const unity& first; const unity& second;
      ref1(const unity& k, const unity& v) : first(k), second(v) {}
      ref1* operator->() { return this; }
    };
    struct ref2
    {
      const unity& first; unity& second;
      ref2(const unity& k, unity& v) : first(k), second(v) {}
      ref2* operator->() { return this; }
    };

    ref1 operator*() const              { return ref1(key(), value()); }
    ref1 operator()() const             { return **this; }
    ref1 operator->() const             { return **this; }

  protected:
    unity& _vnc() const          { unity& z = *(unity*)&x; if (_t == 1) { return z.assocl(pos); } else if (_t == 0 && pos == 0) { return z; } return z.ref<unity>(pos); }
    void _set_utype() { if (x.isAssoc()) { _t = 1; } else if (x.isArray()) { _t = x.utype() == utUnityArray ? -1 : -2; } else { _t = 0; } }
  };

    // Same as uiterate, but gives non-constant reference to value.
    //  (assignable iter.value(), (*iter).second, iter().second, iter->second ...)
    // Usage:
    //    for (uiterate_nc x(my_object); x; ++x) { ... x.key() ... x.value()= ...  }
    //    for (auto x : uiterate_nc(my_object)) { ... x.first ... x.second= ...  }
  struct uiterate_nc : uiterate
  {
    uiterate_nc(const unity& x_, s_long pos_, bool b_forward)       : uiterate(x_, pos_, b_forward) {}
    uiterate_nc(const unity& x_, const char* spec = "b")            : uiterate(x_, spec) {}
    uiterate_nc(const uiterate& x_)                                 : uiterate(x_) {}

    uiterate_nc& operator=(const uiterate& x_)       { uiterate::operator=(x_); return *this; }

    uiterate_nc& operator++()                        { uiterate::operator++(); return *this; }
    uiterate_nc& operator--()                        { uiterate::operator--(); return *this; }

    uiterate_nc begin() const                        { return uiterate(x, _dir == 1 ? "b" : "-b"); }
    uiterate_nc end() const                          { return uiterate(x, _dir == 1 ? "e" : "-e"); }
    uiterate_nc a() const                        { return uiterate(x, _dir == 1 ? "a" : "-a"); }
    uiterate_nc d() const                        { return uiterate(x, _dir == 1 ? "d" : "-d"); }

    unity& value() const          { return _vnc(); }

    ref2 operator*() const              { return ref2(key(), value()); }
    ref2 operator()() const             { return **this; }
    ref2 operator->() const             { return **this; }
  };

    // Same as uiterate, but dereferencing gives constant reference to key, instead of pair (first, second).
    //  (iter.key() <=> (*iter), iter(), iter->...)
    // Usage:
    //    for (uiterate_k x(my_object); x; ++x) { ... x.key()  *x  x()  x-> ... x.value() ...  }
    //    for (auto& k : uiterate_k(my_object)) { ... k ... value N/A ...  }
  struct uiterate_k : uiterate
  {
    uiterate_k(const unity& x_, s_long pos_, bool b_forward)       : uiterate(x_, pos_, b_forward) {}
    uiterate_k(const unity& x_, const char* spec = "b")            : uiterate(x_, spec) {}
    uiterate_k(const uiterate& x_)                                 : uiterate(x_) {}

    uiterate_k& operator=(const uiterate& x_)       { uiterate::operator=(x_); return *this; }

    uiterate_k& operator++()                        { uiterate::operator++(); return *this; }
    uiterate_k& operator--()                        { uiterate::operator--(); return *this; }

    uiterate_k begin() const                        { return uiterate(x, _dir == 1 ? "b" : "-b"); }
    uiterate_k end() const                          { return uiterate(x, _dir == 1 ? "e" : "-e"); }
    uiterate_k a() const                        { return uiterate(x, _dir == 1 ? "a" : "-a"); }
    uiterate_k d() const                        { return uiterate(x, _dir == 1 ? "d" : "-d"); }

    const unity& operator*() const              { return key(); }
    const unity& operator()() const             { return key(); }
    const unity* operator->() const              { return &key(); }
  };

    // Same as uiterate, but dereferencing gives constant reference to value, instead of pair (first, second).
    //  (iter.value() <=> (*iter), iter(), iter->...)
    // Usage:
    //    for (uiterate_v x(my_object); x; ++x) { ... x.key() ... x.value()  *x  x()  x-> ...  }
    //    for (auto& v : uiterate_v(my_object)) { ... key N/A ... v ...  }
  struct uiterate_v : uiterate
  {
    uiterate_v(const unity& x_, s_long pos_, bool b_forward)       : uiterate(x_, pos_, b_forward) {}
    uiterate_v(const unity& x_, const char* spec = "b")            : uiterate(x_, spec) {}
    uiterate_v(const uiterate& x_)                                 : uiterate(x_) {}

    uiterate_v& operator=(const uiterate& x_)       { uiterate::operator=(x_); return *this; }

    uiterate_v& operator++()                        { uiterate::operator++(); return *this; }
    uiterate_v& operator--()                        { uiterate::operator--(); return *this; }

    uiterate_v begin() const                        { return uiterate(x, _dir == 1 ? "b" : "-b"); }
    uiterate_v end() const                          { return uiterate(x, _dir == 1 ? "e" : "-e"); }
    uiterate_v a() const                        { return uiterate(x, _dir == 1 ? "a" : "-a"); }
    uiterate_v d() const                        { return uiterate(x, _dir == 1 ? "d" : "-d"); }

    const unity& operator*() const              { return value(); }
    const unity& operator()() const             { return value(); }
    const unity* operator->() const              { return &value(); }
  };

    // Same as uiterate, but dereferencing gives non-constant reference to value, instead of pair (first, second).
    //  (assignable iter.value(), (*iter), iter(), iter->...)
    // Usage:
    //    for (uiterate_vnc x(my_object); x; ++x) { ... x.key() ... x.value()=  *x=  x()=  x-> ...  }
    //    for (auto& v : uiterate_vnc(my_object)) { ... key N/A ... v= ...  }
  struct uiterate_vnc : uiterate
  {
    uiterate_vnc(const unity& x_, s_long pos_, bool b_forward)       : uiterate(x_, pos_, b_forward) {}
    uiterate_vnc(const unity& x_, const char* spec = "b")            : uiterate(x_, spec) {}
    uiterate_vnc(const uiterate& x_)                                 : uiterate(x_) {}

    uiterate_vnc& operator=(const uiterate& x_)       { uiterate::operator=(x_); return *this; }

    uiterate_vnc& operator++()                        { uiterate::operator++(); return *this; }
    uiterate_vnc& operator--()                        { uiterate::operator--(); return *this; }

    uiterate_vnc begin() const                        { return uiterate(x, _dir == 1 ? "b" : "-b"); }
    uiterate_vnc end() const                          { return uiterate(x, _dir == 1 ? "e" : "-e"); }
    uiterate_vnc a() const                        { return uiterate(x, _dir == 1 ? "a" : "-a"); }
    uiterate_vnc d() const                        { return uiterate(x, _dir == 1 ? "d" : "-d"); }

    unity& value() const          { return _vnc(); }

    unity& operator*() const              { return value(); }
    unity& operator()() const             { return value(); }
    unity* operator->() const              { return &value(); }
  };




    //==  Data trees decoding/encoding.
    //  struct paramline provides original text format, representing
    //    acyclic data trees, consisting of pairs key = value,
    //    where key, in general, may be string or scalar value of any type,
    //    and value may be scalar, string, array or associative array.
    //    The text format is designed so that it could be easily written manually,
    //    and easily read in forward direction, line by line (without jumps between sections),
    //    even if data structure is quite complex and encoded by computer, not human.
    //  Paramline functions are organized into 2 levels:
    //    1. Encoding and decoding single set of "key = value" pairs.
    //      See decode(), encode().
    //      At this level, there's one limitation: the encoded values cannot represent nested associative arrays,
    //      so that multilevel data tree may be formed from arrays only.
    //    2. Encoding and decoding full data tree.
    //      See decode_tree(), encode_tree().
    //
  struct paramline
  {
    static const std::string    cpterm;       // "; "
      static const std::string  cpterm_short; // ";"
    static const std::string    ceterm;       // " = "
      static const std::string  ceterm_short; // "="
    static const std::string    cvterm;       // "|"
    static const std::string    cao_appseq;   // array op. "++"
    static const std::string    cao_app1;     // array op. "+"
    static const std::string    cao_setseq;   // array op. ".."
    static const std::string    cao_set1;     // array op. "."

    static const std::wstring   wpterm;         // L"; "
      static const std::wstring wpterm_short;   // L";"
      static const unity        uwpterm;        // == wpterm
      static const unity        uwpterm_short;  // == wpterm_short
    static const std::wstring   weterm;         // L" = "
      static const std::wstring weterm_short;   // L"="
    static const std::wstring   wvterm;         // L"|"
    static const std::wstring   wao_appseq;     // array op. L"++"
    static const std::wstring   wao_app1;       // array op. L"+"
    static const std::wstring   wao_setseq;     // array op. L".."
    static const std::wstring   wao_set1;       // array op. L"."

      // Decode a string, specifying single set of pairs "key = value".
      // Recognizes ";" and pterm2 as separator of the pairs.
      // Puts keys and values into mh.
      //
      // flags:
      //
      //  0x1 - set map (utMap) as type of mh or the returned container.
      //    By default (flag not set): set hashlist (utHash) as type of mh or the returned container.
      //    NOTE (decode(mh) only) When the existing map or hashlist is cleared, its flags are kept.
      //      This influences keys comparison behavior when inserting decoded keys and values from the string.
      //
      //  0x2 - overwrite value if met duplicate key.
      //    By default (flag not set): use keep-first rule with one exception.
      //	  1. Keep-first rule: the first occurred value with particular key is not overwritten
      //      by later values with the same key.
      //    2. Exception:
      //        k = \z<spec.>
      //      a) If k already exists in the decoded container, AND its value is an array,
      //        the new spec. modifies the existing array:
      //        1) if spec. is empty (\z only is specified), k is assigned an array with size 0 and base index 1.
      //        2) if spec. contains instructions (base index, size, operation),
      //          they are sequentially applied to the existing array under k.
      //      b) If k occurs first time, k is assigned an array with size 0 and base index 1,
      //        and then modified according to spec.
      //      For array spec. (\z), see detailed description (4, 5) below.
      //
      //  0x100 - decode keys in the same way as values (i.e. decode1v()).
      //      This restores key AND its data type
      //      (string, date, integer number, floating-point number, boolean, empty,
      //      also \z<spec.> can make a key which is itself an array),
      //      if ssrc has been specifically written,
      //      or obtained from assoc. array with encode() with 0x100 flag set.
      //    By default (flag not set): keys are decoded as strings, which means only unescaping certain sequences
      //      (\;, \\, \<space>, \=, \|, \~)
      //
      // Returns:
      //    decode(ssrc, mh, flags, pterm2): mh.
      //    decode(ssrc, flags, pterm2): new associative array.
      //
      // Detailed description.
      //
      // 1. 10 basic examples of decoding. (flags = 0)
      //
      //    Original string:
      //      key1 = value1; key2 = value2; key3 = | array elem. 1 | array elem. 2 | 1234 | 3.14159 | array elem. 5;
      //      key4 = \0; key4a = false; key5 = \1; key5a = true;
      //      key6 = 2014-01-15 23:59:00; key6a = \d2014-01-15 23:59:00;
      //      key6b = 2014-01-15; key6c = \d2014-01-15;
      //      key6d = \s2014-01-15 23:59:00;
      //
      //      key7 = \e;
      //      key8 = 1.1; key8a = 1.; key8b = \f1;
      //      key9 = 2; key9a = \i2.0;
      //      key10 = \z
      //      key11 = \z += |1|2|3; key11 = \z += |a|b|c
      //
      //    The decoded associative array:
      //      "key1", "key2" have literal "value1", "value2" values,
      //      "key3" is 1-based utUnityArray of 5 elements of various types
      //        (2 strings, integer number, floating point number, string),
      //      keys 4 to 5a have boolean values,
      //      keys 6 to 6c have utDate values,
      //      "key6d" has the literal string value,
      //
      //      "key7" has the utEmpty value,
      //      all key8* have the floating point values,
      //      both key9* have integer values,
      //      "key10" is empty 1-based utUnityArray.
      //      "key11" is utUnityArray, containing 2 second-level arrays:
      //          ["key11"][1] = (1, 2, 3), ["key11"][2] = ("a", "b", "c")
      //
      //    NOTE By default (if 0x100 flag is not set), the key as such is treated as string value,
      //      even if it resembles a number or other type.
      //
      // 2. Escape sequences.
      //
      //  Literal occurrences of special characters must be escaped
      //    in places or cases where the characters could be treated incorrectly.
      //  In any place, special characters that are escaped, are treated literally.
      //  Escaping rules, as well as whole paramline format, are independent on the platform.
      //  More detailed:
      //    2.1. Literal semicolon (;) must be escaped in all cases (\;).
      //    2.2. When literal value of the given pterm2 must occur in a string,
      //      it is specified with sequence \~ .
      //      By default (pterm2 == wCRLF), \~ represents 2-character sequence (CR, LF).
      //    2.3. Literal spaces must be escaped if occurring leftmost/rightmost of keys, values,
      //      and array values.
      //      Unescaped leading and trailing spaces are always stripped off the key or value text before decoding.
      //    2.4. Literal equality signs (=) must be escaped in keys (\=),
      //      because in a "<key>=<value>" construct
      //      the first unescaped "=" is treated as separating the key and the value.
      //      Literal "=" in values can, but not required to be escaped, because they carry no special meaning.
      //    2.5. Literal "|" must be escaped in values only (\|), in the following cases:
      //        a) if it is the first character of a string value. Alternatively, such string may be prefixed with \s.
      //        b) if it is a character in a text, representing an array element.
      //      These rules are due to leading "|" character in key's value means that an array is encoded,
      //        and also it separates successive array elements.
      //    2.6. Literal backslash must be escaped (\\):
      //        a) when it's necessary to avoid any special backslash use, like described above, and also
      //        b) if any of 2-character sequences
      //            \0, \1, \e, \d, \i, \f, \s, \z
      //          should occur literally at the beginning of any string value.
      //      Any other occurrences of the backslash are not treated as escape sequences,
      //        and both the backslash and the following character are normal part of a key or string value.
      //
      // 3. An error during decoding causes an exception,
      //    in part. on failure to interpret explicitly specified type, e.g. integer (\i<number>) etc.
      //
      //    At this point, mh can be partially modified.
      //    It contains keys and values decoded up to the place in ssrc that caused the exception.
      //
      // 4. General format for decode() input string (ssrc):
      //
      //		<key> <rvalue> [; <etc.>]
      //
      //	rvalue: [ = [<scalar value, sequence, or array spec.>] ]
      //	scalar value: string, date, integer number, floating-point number, boolean, empty
      //	sequence: | <scalar value or array spec.> [| <etc.>]
      //	array spec.: \z [<base index>] [<size>] [<array simple op.> <rvalue>]
      //		base index: b<[+-]integer_base>
      //		size: n<[+]unsigned_integer_size>
      //		array simple op.: <[+-]integer_index|+|++>
      //	NOTE All spaces and number signs are optional, except for separating two numbers.
      //
      // 5. Ways of array construction.
      //
      // 	5.1. Basic array initialization (if not created yet).
      //		k = |a|123|c\|d
      //		k = \z++=|a|123|c\|d
      //		k = \z+=a; k = \z+=123; k = \z+=c|d
      //		k = \z1=a; k = \z2=123; k = \z3=c|d
      //		All of the above       means        set k = array of ("a"; 123; "c|d").
      //
      //		NOTE += and ++= operations may be used many times with the same key, for appending elements and sequences.
      //    NOTE <integer_index>= operation automatically expands the resulting array to the specified index,
      //      and then makes an assignment.
      // 		NOTE Inside string value, both "|" and "\|" mean literal "|".
      //			k=\z+=c|d
      //			k=\z+=c\|d
      //			k=\z+=\sc|d
      //			k=\z+=\sc\|d
      //		All of the above       means        set k = array with single element "c|d".
      //
      // 	5.2. Basic second-level arrays.
      // 		In a sequence, "|" immediately separates elements, while "\|" means literal "|".
      //			After separation, each element is processed individually.
      //
      //		k=|a|b|\zn10|d         means        set k = array of ("a"; "b"; 1-based array with 10 empty elements; "d")
      //		k=|a|b|\zn10 5=x|d     means        set k = array of ("a"; "b";
      //                                            1-based array with 10 elements: [1..4, 6..10] are empty, [5] = "x";
      //                                            "d")
      //		k=|a|b|\zb0n10|d       means        set k = array of ("a"; "b"; 0-based array with 10 empty elements; "d")
      //		k=|a|b|\z+=|c|d        means        set k = array of ("a"; "b"; array with 1 empty element; "c"; "d")
      //		k=|a|b|\z+=c\|d        means        set k = array of ("a"; "b"; array with 1 string "c|d")
      //		k=|a|b|\z+=\|c|d       means        set k = array of ("a"; "b"; array with 1 string "|c"; "d")
      //		k=|a|b|\z+=\|c\|d      means        set k = array of ("a"; "b"; array with 1 string "|c|d")
      //    NOTE When key is assigned inside a sequence, \z as element of sequence cannot be assigned
      //      second-level sequence, because "|" already separates elements of the top-level sequence.
      //      In this case, only single elements can be appended or assigned, also base index and size
      //      (i.e. array shape) can be set.
      //      For full multilevel arrays, see below.
      //
      //	5.3. Matrices and multilevel arrays.
      //		In the examples, "|" characters, used for listing array elements, fill the the most nested array.
      //
      // 		Matrix 3*3 with strings as elements ("ax1", "ax2" etc.).
      //			k = \z1=|ax1|ax2|ax3; k = \z2=|bx1|bx2|bx3; k = \z3=|cx1|cx2|cx3
      //
      // 		Matrix 3*5 with empty elements.
      //			k = \z1=\zn5; k = \z2=\zn5; k = \z3=\zn5
      //
      // 		Matrix 3*5 with empty elements and 0-based indexation.
      //			k = \zb0 0=\zb0n5; k = \z1=\zb0n5; k = \z2=\zb0n5
      //
      // 		Array of arrays of arrays (dimension 2*2*3). All arrays are 1-based.
      //			k = \z1=\z1=|x111|x112|x113; k = \z1=\z2=|x121|x122|x123; k = \z2=\z1=|x211|x212|x213; k = \z2=\z2=|x221|x222|x223
      //
    unity& decode(arrayref_t<wchar_t> ssrc, unity& mh, s_long flags = 0, arrayref_t<wchar_t> pterm2 = wCRLF);
    unity decode(arrayref_t<wchar_t> ssrc, s_long flags = 0, arrayref_t<wchar_t> pterm2 = wCRLF);

      // Functional part of decode().
      // Decodes ssrc as one value (scalar, sequence, or array spec.), specified without key and equality sign.
      // See also decode().
      //
    unity& decode1v(arrayref_t<wchar_t> ssrc, unity& dest);
    unity decode1v(arrayref_t<wchar_t> ssrc);


      // Decode multiline text, representing a tree of values.
      //  Default behavior:
      //    1. Clear mha and set its type to map/hashlist (if flags has 0x1 set/unset resp.).
      //    2. Split ssrc, using pterm2.
      //    3. Decode each resulting substring in the same way as decode() does,
      //      then merge the result into mha, under the current, or new specified (in the current line) branch path.
      //      On duplicate key, the existing (i.e. first set) value is kept.
      //      For lines with equal branch paths, contents are merged into the same branch of mha.
      //  Branch path format:
      //    1. The path in mha is specified by list of elements after an empty key,
      //      in the same line with normal keys and values, e.g.
      //        "=|pathelem1|pathelem2; k1 = v1; k2 = v2"
      //      means
      //        mha("pathelem1")("pathelem2")("k1") = "v1";
      //        mha("pathelem1")("pathelem2")("k2") = "v2";
      //    2. If no path specified (or empty key's value is not an array),
      //      k[i], v[i] pairs are merged into the root (mha itself).
      //    3. In general, types and number of path elements, keys and values may be anything that decode() supports:
      //      string, number, empty, or any other type, normally supported by paramline::.
      //
      // flags:
      //  NOTE For good default in parsing manually written text, use
      //    flags = 0x5a.
      //
      //  0x1 - for mha and branches, use map as target container type instead of hashlist.
      //
      //  0x2 - overwrite value in a branch if met duplicate path or key.
      //      1. When the flag is set,
      //        1) (+) any new value or branch (map, hashlist or array)
      //          will overwrite the existing value,
      //          and also the new single value may replace the existing branch.
      //        2) (+) Branches as such are always merged.
      //          When same branch path is explicitly specified in more than one text line,
      //          all keys/values from that line are merged into the existing branch.
      //      2. When the flag is not set, the following actions are not done (spec. is ignored):
      //        1) (-) assigning new value instead of existing value or branch
      //          with the same key is not done.
      //        2) (-) branch path, assigning new branch over the existing single value
      //          (except for utEmpty), is not applied.
      //          (Such assignment would be equivalent to converting a value into map, hash or array -
      //          in order to follow the specified path.)
      //          The previous path remains effective.
      //          NOTE This can lead to strange effects when braced blocks are enabled (see flag 0x40).
      //            Such block, after path that hasn't been applied, will be instead merged
      //            into the previous effective path.
      //        3) (-) in array area: assigning a value to already existing array element,
      //          except for
      //            1) the existing element is utEmpty (might be created automatically on array shape setting)
      //            2) the existing element is subarray, and the new value is \z<spec.>,
      //              i.e. creates a subarray or modifies the existing subarray shape and values.
      //      3. Regardless of 0x2 flag:
      //        1) (!) branch path, whose particular element points into an existing array,
      //          but is not itself an array index of type utInt or utEmpty (same as integer 0),
      //          decode_tree generates C++ exception.
      //        2) (+) if the specified path/key exists, and its value is empty (utEmpty) -
      //          any specified action (either value assignment, or branch creation/modification)
      //          will be done unconditionally.
      //        3) (+) if the specified path leads to existing branch (map, hashlist or array),
      //          and converts it into an array with \z spec. (or modifies shape modification of existing array),
      //          such modification is done unconditionally.
      //        4) (-) if branch path spec. is not an array (e.g. "=key" instead of "=|key"),
      //          it is ignored.
      //          The previous path remains effective.
      //          See also note in 2.2 above.
      //
      //  0x4 (decode_tree(mha) only) - do not clear mha if it's already associative, merge ssrc tree into mha.
      //    This has the following effects:
      //      1) flag 0x1 is ignored, the input mha type (map or hashlist (dflt.)) is used for all new nodes.
      //      2) mha is not cleared, all decoded branches are merged into it.
      //      3) all new nodes inherit mapFlags (or hashFlags) from mha.
      //
      //  0x8 - ignore substring like regexp. "^[\t ]*//.*$".
      //    This allows for writing single-line C-style comments.
      //
      //  0x10 - convert distinct CR, LF and LFCR sequences to CRLF before decoding.
      //
      //  0x20 - include branch paths, i.e. pairs ("", branch path as utUnityArray of keys),
      //      into all branches of the decoded tree.
      //      If flag is not set, no empty string key ("") occurs in any branch of the decoded tree.
      //      NOTE 0x100 flag (see) disables 0x20, but empty string key ("") may occur in the tree,
      //        because its encoded form "\s" is different from empty string.
      //      NOTE By default, root path (empty array) is not assigned to mha[""],
      //        unless explicitly specified: "=\z".
      //
      //  0x40 - treat lines containing only braces and optional spaces (matching regexp. "[ {}]+")
      //      as start/end of multiline key/value block.
      //
      //    Example. The following text, in its basic form,
      //
      //          =|playback; autostart = true; stop1 = true
      //          =|playback|lists; /home/list1; /home/list2
      //
      //      is decoded into the following data structure
      //      regardless of 0x40 flag:
      //
      //          // pseudocode
      //          hashlist(
      //            "autostart" => true,
      //            "stop1" => true,
      //            "playback" => hashlist("/home/list1" => <empty>, "/home/list2" => <empty>)
      //          )
      //
      //      Same data structure can be decoded from the alternate form of text (only with 0x40 flag):
      //
      //          =|playback
      //          {
      //            autostart = true; stop1 = true
      //            =|lists
      //            {
      //              /home/list1
      //              /home/list2
      //            }
      //          }
      //
      //      NOTE Braces are recognized only if the line does not contain any other characters except spaces,
      //        otherwise brace will be treated as part of a key.
      //      NOTE Line with the opening brace is associated with path,
      //        specified in the line above,
      //        only if
      //          a) no additional lines between them (the line with path is the nearest to the line with opening brace),
      //          b) additional lines are empty, or (on flag 0x8 set) contain comments only.
      //        Otherwise, the path assumed for in-braces area, is =|, relative to the current branch.
      //        This case works, even if not very useful.
      //            =|a|b|c
      //
      //            // comment
      //
      //            {
      //            z = 5
      //            {
      //            z = 7
      //            =|d; z = 10
      //            }
      //            }
      //          means the same as
      //            =|a|b|c; z = 5
      //            =|a|b|c|; z = 7
      //            =|a|b|c||d; z = 10
      //
      //  0x100 - decode keys in the same way as values (i.e. as if with decode1v()).
      //      This restores key AND its data type
      //      (string, date, integer number, floating-point number, boolean, empty,
      //      also \z<spec.> can make a key which is itself an array).
      //      Should be used if the original tree has been encoded with 0x100 flag (on the flag: see encode()),
      //      or manually written with keys that are not only of string type (see also decode()).
      //    By default (flag not set): keys are decoded as strings, which means only unescaping certain sequences
      //      (\;, \\, \<space>, \=, \|, \~).
      //    NOTE 0x100 mode distinguishes between two string representations (in ssrc)
      //      of the empty key (equivalent to empty unity()):
      //        a) as empty string, b) as \e.
      //      (a) is still treated as key for an array, specifying branch path.
      //        This empty key and its value are not included into mha
      //        even if 0x20 flag is specified.
      //      (b) is treated as normal pair (empty key, user-defined value), which should appear
      //        in mha when the function completes.
      //      Keys in the form (a) and (b) do not overwrite each other, while inside each of forms
      //        "overwrite" rule is controlled by 0x2 flag (see above).
      //
      //
      // Returns: mha.
      //    mha may be
      //      a) utHash (default root container type),
      //      b) utMap (default root type on flag 0x1 set),
      //      c) utUnityArray (when dflt. root type has been changed by path with array spec., e.g. =|\z ).
      // NOTE Literal CRLF pairs in ssrc must be specified as special sequence \~ .
      //    (More detailed: after decoding, occurrences of \~ in strings are replaced with value of pterm2.)
      //    See also decode().
      //
      // Detailed information.
      //
      // 1. Formal description of the encoded tree format.
      //
      //   a) In array area:
      //    <path> <array area op.> <rvalue> [; <etc.>]
      //   b) In associative array area:
      //    <path> <key> <rvalue> [; <etc.>]
      //
      //  path:
      //    a) Specifies a path to branch, in the form of sequence of keys.
      //      The branch itself, if already exists, may be an array or associative array.
      //      If the branch does not exist, it is created as assoc. array.
      //        =|<path element>[|etc.]
      //    b) Specifies a path to branch, ensuring it being an array (even if it exists as e.g. assoc. array).
      //      NOTE Branch type change clears its previous contents.
      //        =|<path element>[|etc.]|<path element>|\z<array shape spec.>
      //    c) Specifies the tree root, which can be assoc. array (default), or an array
      //      (if previously converted with =|\z<array shape spec.>).
      //        =\z
      //  array area op.: <[+-]integer_index|+|++|.|..>
      //  rvalue: [ = [<scalar value, sequence, or array spec.>] ]
      //  scalar value: string, date, boolean value, integer number, floating-point number, empty
      //  sequence: | <scalar value or array spec.> [| <etc.>]
      //  array spec.: \z [<base index>] [<size>] [<array simple op.> <rvalue>]
      //    base index: b<[+-]integer_base>
      //    size: n<[+]unsigned_integer_size>
      //    array simple op.: <[+-]integer_index|+|++>
      //  array shape spec.: \z [<base index>] [<size>]
      //
      //  NOTE All spaces and number signs are optional, except for separating two numbers.
      //
      //
      // 2. Formal description of the encoded tree format with braced areas enabled (flag 0x40).
      //  This is multiline extension of the default format, described above.
      //
      //	=<path to assoc. array> [; <key> <rvalue>] [; etc...]
      //	{
      //			// Array elements at the current level.
      //		[<key> <rvalue>] [; etc...]
      //		[etc...]
      //			// Nested branches, with or without braces.
      //		[<nested branch>]
      //		[etc...]
      //	}
      //
      //	=<path to array> [; <array area op.> <rvalue>] [; etc...]
      //	{
      //			// Array elements at the current level.
      //		[<array area op.> <rvalue>] [; etc...]
      //		[etc...]
      //			// Nested branches, with or without braces.
      //      //  Here, the first element in the nested branch path is expected to be an integer index
      //      //  in the current array. If the array does not contain such index,
      //      //  it's automatically expanded as necessary.
      //		[<nested branch>]
      //		[etc...]
      //	}
      //
      //  NOTE The nested branch path is specified relative to path of the braced area,
      //    containing that branch.
      //
    unity& decode_tree(arrayref_t<wchar_t> ssrc, unity& mha, s_long flags = 0, arrayref_t<wchar_t> pterm2 = wCRLF);
    unity decode_tree(arrayref_t<wchar_t> ssrc, s_long flags = 0, arrayref_t<wchar_t> pterm2 = wCRLF);

      // Encodes the given map or hashlist (uses pterm as element separator),
      //    in the form of one-level list "key1 = value1; key2 = value2 ...".
      //  The resulting string is recognizable by decode() and decode_tree().
      //  Details on the encoded string format: see decode().
      // flags:
      //    0x100 - encode keys in the same way as values (i.e. encode1v()).
      //        This allows keys to keep original data type
      //        (string, date, integer number, floating-point number, boolean, empty),
      //        and later recover it by passing the same flag (0x100) into decode() or decode_tree().
      //      NOTE For complex key (itself array or assoc. array),
      //        only short string representation (e.g. array dimensions) is encoded.
      //        Later, it can be decoded as plain string only.
      //      By default (flag not set):
      //        1. Non-string keys are converted into strings first.
      //        2. If string key contains certain characters, they are escaped as necessary for unambiguous decoding.
      //          Details: see decode().
      //    0x200 - if met an empty key (utEmpty), force encoding it as \e.
      //      By default (flag not set): encode empty key as empty string.
      // pterm:
      //    key-value pairs are joined with pterm (if it's empty, wpterm == "; " is used).
      // Returns:
      //    The encoded string representation of mhsrc.
      //    If mhsrc is not an assoc. array, encode() returns "".
      //    If an exception occurs during encoding, sdest may contain partially encoded data.
      // NOTE encode() supports multilevel arrays,
      //    e.g.
      //      encode1v(unity("k", unity().array(unity().array(1, 2, 3), unity().array(), unity().array("a", "b", "c"))))
      //    yields
      //      k = |||; k = \z1=|1|2|3; k = \z2=\z; k = \z3=|a|b|c
      //    for which decode() will reconstruct exactly the original array into value of the key "k".
      //
    std::wstring& encode(const unity& mhsrc, std::wstring& sdest, s_long flags = 0, arrayref_t<wchar_t> pterm = wpterm);
    std::wstring encode(const unity& mhsrc, s_long flags = 0, arrayref_t<wchar_t> pterm = wpterm);

      // Functional parts of encode().
      // Details on the format of encoded names and values: see decode().
      //
      // encode1n(): converts the given value into string with proper escaping of certain characters,
      //      the result could be used as left part of string like "key = value", recognizable by decode().
      //  flags: see encode() flags 0x100, 0x200.
      //
      // encode1v(): encodes single value (scalar, string or one-dimensional array),
      //    so that it can be passed to decode1v()
      //    or used as right part of string like "key = value", recognizable by decode().
      //  NOTE One limitation in comparison with encode() is that encode1v()
      //    does not encode multilevel arrays.
      //    If the given value is an array with nested arrays (the second level),
      //    the returned string will define only shape of the 2nd-level arrays,
      //    e.g.
      //      encode1v(unity().array(unity().array(1, 2, 3), unity().array(), unity().array("a", "b", "c")))
      //    yields
      //      |\zb1n3|\z|\zb1n3
      //    For 3rd and deeper level of arrays nesting, any values are ignored by encode1v()).
      //
    std::wstring encode1n(const unity& name, s_long flags = 0); // encode single name
    std::wstring& encode1v(const unity& value, std::wstring& sdest); // encode single value (scalar or array)
    std::wstring encode1v(const unity& value);

      // Encodes the given data tree into multiline text, producing separate text line for each branch.
      //    Here, "branch" means collection of (key, value) pairs of an associative array,
      //      where value is anything except an associative array.
      //      Such value is considered separate branch
      //      and is encoded into one of lines, following the line of the parent branch.
      //      (Additionally, if the value is an array whose some element (recursively)
      //      is an assoc. array, that assoc. array is also encoded into separate line,
      //      while in the original line the corresponding array element is empty.)
      //    At the beginning of each line, full path of the branch is inserted in the form:
      //        =|<path element>[|etc.]
      //        =|<path element>[|etc.]|<path element>|\z<array shape spec.>
      //      See also decode_tree() / detailed information / 1.
      //
      //  mhasrc: the root map, hashlist or array.
      //  flags: see encode().
      //  pterm: used to separate parameters in a line, representing a branch.
      //  pterm2: used to separate lines (branches).
      //
      // Result:
      //  a) multiline text.
      //  b) "", if mhasrc is not an array or associative array, or utObject that can be dereferenced to it.
      //  c) if an exception occurs during encoding, sdest may contain partially encoded data.
      //
    std::wstring& encode_tree(const unity& mhasrc, std::wstring& sdest, s_long flags = 0, arrayref_t<wchar_t> pterm = wpterm, arrayref_t<wchar_t> pterm2 = wCRLF);
    std::wstring encode_tree(const unity& mhasrc, s_long flags = 0, arrayref_t<wchar_t> pterm = wpterm, arrayref_t<wchar_t> pterm2 = wCRLF);




    //== Helper functions for assoc. arrays.

    typedef const unity& _rcu;
    static _rcu _0; // empty value, for representing default argument

      // Makes set1 += set2,
      //    where sets can be utMap or utHash (otherwise XUTypeMismatch is generated).
      // keep_first:
      //    true will prevent overwriting the existing values in set1 by values from set2.
      // Returns:
      //    set1.
    unity& merge(unity& set1, const unity& set2, bool keep_first = false);
    unity& merge(unity& set1, const arrayref_t<wchar_t>& set2_pl, bool keep_first = false);

      // Returns utMap, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output map, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
      // See also unity :: operator().
    unity list_m
      ( _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

      // Returns utMap, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output map, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_mx
      ( s_long fk, // unity::fkcmp* flags
        _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0);

      // Returns utHash, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output hashlist, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
      // See also unity :: operator().
    unity list_h
      ( _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

      // Returns utHash, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output hashlist, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_hx
      ( s_long fk, // unity::fkcmp* flags
        _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

      // _list_*_set_u are same as list_*, only assign the result directly to dest. arg.
      //  NOTE The functions do not check for assigning a value to its own container.
      //    This case must be avoided by the client.
    void _list_mx_set_u
      ( unity& mdest,
        s_long fk, // unity::fkcmp* flags, 0 for default
        _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );
    void _list_hx_set_u
      ( unity& hdest,
        s_long fk, // unity::fkcmp* flags, 0 for default
        _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

  private:
    enum { fl_v = 1, fl_bs = 2, fl_crlf = 4, fl_sc = 8, fl_sp = 16, fl_eq = 32, fl_ba = 64 };
    friend struct _paramline_branch;
    friend struct _paramline_array_spec;
    friend struct _paramline_test;
    static arrayref_t<wchar_t> x_pterm2_dflt();
    static void x_encode1n(const unity& x, std::wstring& retval, s_long flags_encdec, const arrayref_t<wchar_t>& pterm2);
    static void x_encode1v(const unity& x, std::wstring& retval, s_long flags_encdec, vec2_t<std::wstring, __vecm_tu_selector>* ret_psubarrs, hashx<const unity*, s_long>* phstop, const arrayref_t<wchar_t>& pterm2);
    static void x_encode_branch(s_long flags_encdec, const unity& mh, const arrayref_t<wchar_t>& path, std::wstring& sdest, hashx<const unity*, s_long>& hstopk, hashx<const unity*, s_long>& hstopv, arrayref_t<wchar_t> pterm, arrayref_t<wchar_t> pterm2, arrayref_t<wchar_t> pathpfx);
    static void x_repl_e1(const arrayref_t<wchar_t>& s1, std::wstring& s2, bool s_name, bool s_ar_elem, bool b_esc_eqsign, const arrayref_t<wchar_t>& pterm2);
    static void x_replace2a(std::wstring& s, s_long flags, const arrayref_t<wchar_t>& pterm2);
    static void x_replace4(arrayref_t<wchar_t> s1, std::wstring& s2, s_long& replflags);
    struct _opt_unity;
    static void x_decode(arrayref_t<wchar_t> ssrc, unity& mha, s_long flags_encdec, arrayref_t<wchar_t> pterm2, _opt_unity* pret_path, s_long* pret_replflags);
    static void x_decode1v(const arrayref_t<wchar_t>& ssv, bool v_ar_elem, s_long replflags, unity& vdest, const arrayref_t<wchar_t>& pterm2);
    static bool x_decode1v_auto_date(const arrayref_t<wchar_t>& s, unity& retval) __bmdx_noex;
    static bool x_incorrect_numeric_value_str(const arrayref_t<wchar_t>& s, bool b_nans);
    static bool x_incorrect_integer_value_str(const arrayref_t<wchar_t>& s, bool allow_float_str);
    static int _match_spaces(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend);
    static int _match_character(arrayref_t<wchar_t> s, s_ll pos, arrayref_t<wchar_t> cc);
    static int _match_int32(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_x, arrayref_t<wchar_t> terms);
    static int _match_uint64(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, bmdx_meta::u_ll& ret_x, arrayref_t<wchar_t> terms);
    static int _msp_op_index(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_i);
    static int _msp_op_pluseq(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend);
    static int _msp_op_pluspluseq(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend);
    static int _msp_b(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_b);
    static int _msp_n(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_n);
  };




    //==  FILE UTILITIES.
  enum EFileUtilsPredefinedDir
  {
      pdCurDir, // the current directory
      pdThisAppDir, // path to the current application
      pdTempDir, // Windows: %systemroot%\temp, Linux: %TMPDIR%
      pdUserDefinedDir,
          //- user-defined directory, passed as second argument; if none, the path to the current application is used
      pdDoNotChange // do not complete path
  };

  struct file_utils
  {
      // Returns true is pathstr specifies the full path.
      // Returns false for relative paths.
      // The function analyzes both correct and incorrect paths.
    bool is_full_path(const std::wstring& pathstr) const;
    bool is_full_path(const std::string& pathstr) const;

      // Get path without filename (just remove the last path element).
    std::wstring strip_path(const std::wstring& pathstr) const;
    std::string strip_path(const std::string& pathstr) const;

      // Get filename (the last path element).
    std::wstring strip_filename(const std::wstring& pathstr) const;
    std::string strip_filename(const std::string& pathstr) const;

      // Returns pathstr without the last extsep and the characters after,
      //  if this extsep occurs after the last path separator.
    std::wstring remove_ext(const std::wstring& pathstr, const std::wstring& extsep = L".") const;
    std::string remove_ext(const std::string& pathstr, const std::string& extsep = ".") const;

      // Inserts s_add into copy of pathstr before the last occurrence of extsep,
      //  if this extsep occurs after the last path separator.
      //  Otherwise, returns pathstr + s_add.
    std::wstring add_to_name(const std::wstring& pathstr, const std::wstring& s_add, const std::wstring& extsep = L".") const;
    std::string add_to_name(const std::string& pathstr, const std::string& s_add, const std::string& extsep = ".") const;

      // Replace filename part in fnp with another filename fn.
    std::wstring replace_filename(const std::wstring& fnp, const std::wstring& fn) const;
    std::string replace_filename(const std::string& fnp, const std::string& fn) const;

      // Joins two path parts (or copies pathstr, if pathstr2 is omitted).
      // Then, deletes all duplicate slashes.
      // Finally, deletes the last slash, if the path has no characters after it.
    std::wstring join_path(const std::wstring& pathstr, const std::wstring& pathstr2 = L"") const;
    std::wstring join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3) const;
    std::wstring join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3, const std::wstring& ps4) const;

    std::string join_path(const std::string& pathstr, const std::string& pathstr2 = "") const;
    std::string join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3) const;
    std::string join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3, const std::string& ps4) const;

      // Returns true if path contains slash at the end.
    bool has_rightmost_patshep(const std::wstring& pathstr) const;
    bool has_rightmost_patshep(const std::string& pathstr) const;

      // Returns false if path contains invalid symbols
      //   or incorrectly represents path in some other way.
      // Otherwise returns true.
    bool is_valid_path(const std::wstring& pathstr) const;
    bool is_valid_path(const std::string& pathstr) const;

      // If pathstr is already a full path, it is returned as is.
      // If pathstr starts with ".", the point is removed, and the result is prepended with the current directory, then the path is regarded as complete.
      // In other cases, a predefined path, selected by pd argument, is prepended to pathstr.
      //  For pd = pdUserDefinedDir, the function uses sUserDefDir. If sUserDefDir is not specified, it is assumed to be this application executable file path.
    std::wstring complete_path(const std::wstring& pathstr, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir = L"") const;
    std::string complete_path(const std::string& pathstr, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir = "") const;

      // Returns true if file exists (and is not a directory).
      //  If pathstr contains rightmost path separator character,
      //  the function always returns false.
      // The function is similar to file_io::is_ex_file.
    bool is_ex_file(const std::wstring& pathstr) const;
    bool is_ex_file(const std::string& pathstr) const;

      // Returns true if directory exists (and is not a file).
      //  pathstr may or may not contain rightmost path separator character,
      //  this does not influence the behavior.
      // The function is similar to file_io::is_ex_dir.
    bool is_ex_dir(const std::wstring& pathstr) const;
    bool is_ex_dir(const std::string& pathstr) const;

      // Non-recursive sequential expansion of environment variables in s.
      //  1. Converts tokens, limited by '%', into corresponding values of env. variables (std::getenv).
      //  2. If variable is not found in the environment, its name is replaced with an empty string.
      //  3. Sequence "%%" in s is treated specially:
      //    a) if met beyond variable name, it is immediately converted to single literal percent sign, unrelated to env. variables.
      //    b) if met inside variable name, ends the name and immediately marks the beginning of the next variable name.
      // NOTE std::wstring version of expand_env_nr:
      //    1. Converts tokens, limited by '%', into 1-byte character string.
      //    2. Gets env. variable value.
      //    3. Converts the value back to std::wstring.
      //    Conversions are based on system-default locale (equiv. to std::setlocale(..., "")).
      //    See also wsToBs(), bsToWs().
    std::wstring expand_env_nr(const std::wstring& s) const;
    std::string expand_env_nr(const std::string& s) const;

      // Recursively create directories to ensure that pathstr exists.
      //  On success (all was created or already existed) returns true.
      // NOTE POSIX: mk_subdir creates each non-existing directory in pathstr
      //  with permissions 0777.
      //  For already existing directories, their current permissions are not modified.
    bool mk_subdir(const std::wstring& pathstr) const;
    bool mk_subdir(const std::string& pathstr) const;

      // Deletes single file or empty directory.
      //  The function is similar to file_io::remove_one.
      // flags:
      //  0x2: allow removal if path specifies a file.
      //  0x4: allow removal if path specifies a directory.
      // Returns:
      //  1 - success.
      //  0 - path does not exist.
      //  -1 - bad argument (path is null or empty string, also (flags & 6) == 0).
      //  -2 - common case failure.
      //  -3 - removal failed (file/directory exists).
      //  -4 - removal of existing file/directory is not allowed by flags.
    int remove_one(const std::wstring& pathstr, s_long flags = 0x6);
    int remove_one(const std::string& pathstr, s_long flags = 0x6);

      // Loads whole file (pathstr) into wide-character string.
      //  format_string should contain
      //    1) one of "binary" or "text".
      //      For "binary", the input data is treated as plain array of bytes,
      //        which should be converted into string according to the specified or guessed encoding.
      //      "text" is different in:
      //        1.1) auto-converts input's single \r and \n into \r\n,
      //        1.2) recognizes UTF encoding by BOM at the beginning of file, and does not insert BOM into ret_s:
      //          UTF-8 - EF BB BF
      //          UTF-16LE - FE FF
      //          UTF-16BE - FF FE
      //    2) one or more encodings to try:
      //      "local8bit", "lsb8bit", "utf8", "utf16le", "utf16be".
      //      local8bit: uses system-default locale to convert file read as byte string, into wide string (see also bsToWs()).
      //        If it is specified in format_string, it is used the default encoding in case if others do not match.
      //      lsb8bit: read raw bytes and sets each as lower byte of wchar_t.
      //        If it is specified in format_string, it suppresses local8bit.
      //      Encodings are checked in the order of format_string,
      //        and the first matching encoding wins.
      //      Any unknown parts of format_string are ignored.
      //  ret_s: if non-default value is passed into ret_s, it is assigned the loaded string,
      //    and load_text itself returns boolean value (success/failure).
      //  pd: see complete_path.
      // Returns:
      //    On success: string with the loaded characters (but see also ret_s).
      //    On failure or wrong args.: utEmpty.
    unity load_text(const std::string& format_string, const std::wstring& pathstr, EFileUtilsPredefinedDir pd = pdCurDir, unity& ret_s = unity::_0nc) const __bmdx_noex;
    unity load_text(const std::string& format_string, const std::string& pathstr, EFileUtilsPredefinedDir pd = pdCurDir, unity& ret_s = unity::_0nc) const __bmdx_noex;

      // Saves characters of wide-character string str to the specified file (pathstr_target).
      //  format_string should contain
      //    1) one of "binary" or "text".
      //      For "binary", characters from str are 1:1 converted to output, according to selected encoding,
      //        without additional operations.
      //      "text" is different in:
      //        1.1) auto-converts input's single \r and \n into \r\n,
      //        1.2) auto-prepends BOM when writing from the beginning of file (file is empty or truncated):
      //          UTF-8 - EF BB BF
      //          UTF-16LE - FE FF
      //          UTF-16BE - FF FE
      //    2) target encoding:
      //      "local8bit", "lsb8bit", "utf8", "utf16le", "utf16be".
      //    3) mode:
      //      "truncate" or "append".
      //    For example: "text append utf16le".
      //    NOTE Any unknown parts of format_string are ignored.
      //  pd: see complete_path.
      // Returns:
      //    On success: true.
      //    On failure or wrong args.: false.
    bool save_text(const std::string& format_string, const std::wstring& str, const std::wstring& pathstr_target, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const __bmdx_noex;
    bool save_text(const std::string& format_string, const std::wstring& str, const std::string& pathstr_target, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const __bmdx_noex;


      // Loads bytes from the given file into dest.
      //    (Same as file_io::load_bytes.)
      //  flags:
      //    0x2 - load file sequentially, until EOF (instead of getting file length and loading length bytes as whole).
      //      This can be used for dynamically updated files or special file systems,
      //      where file seeking to end of file works actually incorrectly.
      // Returns:
      //  1 - success.
      //  0 - file does not exist.
      //  -1 - memory alloc. error, or wrong arguments.
      //  -2 - file i/o error. NOTE On i/o error, dest may be left modified.
    int load_bytes(arrayref_t<wchar_t> fnp, std::string& dest, s_long flags = 0) __bmdx_noex;
    int load_bytes(arrayref_t<char> fnp, std::string& dest, s_long flags = 0) __bmdx_noex;

      // Saves bytes from src to the given file.
      //    (Same as file_io::save_bytes.)
      // b_append: false truncates the file before writing, if it exists.
      // Returns:
      //  1 - success.
      //  0 - failed to create file (or open the existing file for writing).
      //  -1 - data size too large, or memory alloc. error, or wrong arguments.
      //  -2 - file i/o error. NOTE On i/o error, the file may be left modified.
    int save_bytes(arrayref_t<wchar_t> fnp, arrayref_t<char> src, bool b_append) __bmdx_noex;
    int save_bytes(arrayref_t<char> fnp, arrayref_t<char> src, bool b_append) __bmdx_noex;

  private:
    bool xHasCurDirShortCut(const std::wstring& pathstr) const;
    bool xHasCurDirShortCut(const std::string& pathstr) const;
    bool xmk_subdir(const std::string& pathstr, int level) const;
    #ifdef _bmdxpl_Wnds
      struct __utils_l2;
      bool xmk_subdir(const std::wstring& pathstr, int level) const;
    #endif
  };









  //==  MULTITHREADED MESSAGE DISPATCHER
#if bmdx_part_dispatcher_mt



// === Definition ===
//
//  BMDX message dispatcher is the specialized communication subsystem,
//    allowing multiple clients (like programmatic objects, CPU threads, processes)
//    to exchange high-level structured information, with optional binary attachments.
//
//  The message dispatcher is represented by
//    - dispatcher_mt class,
//    - i_dispatcher_mt interface,
//    - underlying implementation (using several dedicated CPU threads).
//
// === Notation ===
//
//  { key; value }    - associative array; key, value may be literary values, parameter names, or descriptions of such
//  (element, element ... )    - list or array as programmatic object; elements may be literary values, parameter names, or descriptions of such
//  |element|element ...    - list or array as string in the program; each "|" starts new element; see also struct paramline
//  [ ... ]    - optional element or part
//  <identifier>    - some value or parameter; the identifier is used to describe the value in text
//  * ?    - standard wildcards (any characters, any single character)
//
// === Additional information ===
//
//  See arch_notes.txt / MESSAGE DISPATCHER for
//      State of development,
//      Terminology,
//      Slot types and address structure,
//      Examples of configuration,
//      Configuration parameters.


    // Informational structure with all values for "flags" arguments of all dispatcher_mt, i_dispatcher_mt functions.
  struct dispatcher_mt_flags
  { enum e {
      // msend, mget
    get_hashlist = 1,
    discard_msg = 2,
    discard_att = 4,
    peek_only = 8,
    anlo_msg = 0x10,
    anlo_att = 0x20,
    ignore_hanged = 0x40,
    use_chsbin = 0x80,

      // dispatcher_mt
    disp_off_th_mask = 0xfff00,
    disp_off_th_lqsd = 0x100,
    disp_off_th_lmsc = 0x200,

      // request, slots_create, dispatcher_mt
    ensure_local_subs = 0x1,

      // slots_remove
    array_of_slotnames = 0x1
  }; };

    // Request types (1st arg. of i_dispatcher_mt::request).
  struct dispatcher_mt_rt
  { enum e {
    get_threads_names = 1,
    get_th_slots_names = 2,
    get_threads_n = 11,
    get_th_slots_n = 12,

    set_qs_priority = 3,
    get_qs_priority = 4,
    deliver_qs_now = 8,
    get_qs_nvsubs = 13,

    create_proxy = 5,
    create_disp_thread = 6,
    remove_disp_thread = 7,

    reset_cmdslot_phase = 9,

    set_shmqueue_conf = 10
  }; };

    // Access to i_dispatcher_mt: see dispatcher_mt :: new_proxy.
  struct i_dispatcher_mt
  {
    typedef arrayref_t<char> t_stringref;
    struct tracking_info
    {
      s_ll id, state;
      bool is_empty() const { return id == 0 && state == 0; }
      tracking_info() : id(0), state(0) {}
      tracking_info(s_ll id_, s_ll state_) : id(id_), state(state_) {}
    };




      // Send a message
      //    (push the message into the internal queue or other storage, associated with destination address).
      //    Depending on the source and destination slot types, configuration and location, the message may be
      //      a) delivered immediately (during the current call),
      //      b) delivered after a delay (by internal threads in the current CPU process, and also that in the receiving CPU process if it's different),
      //      c) rejected (immediately or after a delay).
      //
      //  msg:
      //      a) hashlist or map (hashlist is the most efficient),
      //      b) paramline-encoded string (manually or with encode()) representation of the above.
      //    Minimum required contents (in string form):
      //
      //      src = <NAME>; trg = <ADDR>; text = <T>
      //
      //    For example:
      //      o_iptr_t<i_dispatcher_mt> pdisp;
      //        // ... init. pdisp for accessing the chosen dispatcher thread.
      //      pdisp->write("src = po_1; trg = |LP|receiver|pi_1; text = abcde");
      //      (All spaces are optional.)
      //      This sends message "abcde" from output pin-type slot po_1 of the current dispatcher thread (referred by pdisp)
      //        into the same dispatcher process (LP = "local process"), thread "receiver", input pin-type slot pi_1.
      //
      //      NAME of the slot (here: sender) may be any of two kinds:
      //        <slot type>_<name root>
      //          - one-string name of the slot.
      //        |<slot type>_<name root>|<name part 2>[|part 3[|part 4 ...]]
      //          - multipart (array) name of the slot (showed in string form).
      //        Detailed description: arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure.
      //
      //      ADDR - destination slot address, consisting of concatenated transfer type, process, thread, slot name + optional parts.
      //        Detailed description: arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure.
      //        Formats of ADDR for different transfer types:
      //          a)    |LP|<thread name>|<slot name>
      //          b)    |LPA|<qs slot name>
      //          c)    |LM|<process name>|<thread name>|<slot name>
      //        (a) is for sending the message between two dispatcher threads inside the same one dispatcher process.
      //        (b) same as (a), only the name of dispatcher thread, owning the specified qs (subscription) slot, is not known to sender.
      //        (c) sends the message from one dispatcher process to another, using IPC (shared memory).
      //          NOTE "LM" only specifies technical way of message exchange.
      //            It works regardless of the sender and the recipient being a) in the same dispatcher process,
      //            b) in different dispatcher processes inside one CPU process, c) in different dispatcher processes,
      //            each inside its own CPU process.
      //        NOTE "Dispatcher process": distinct dispatcher_mt object + all associated objects and CPU threads, inside one CPU process.
      //            See arch_notes.txt / MESSAGE DISPATCHER / Terminology.
      //
      //      T is the client message.
      //        Usually, T is a string, numeric value, or one-dimensional 1-based array.
      //        Also, T may be empty, but "text" key must be present.
      //        If ADDR is local, and anlo_msg flag is set, T may contain any object of set of objects.
      //
      //      NOTE The client may put additional entries into the message.
      //        Entry name (hashlist key) must be string >= 2 chars., starting from '_'. The second character must be != '_'.
      //        (Mainly, the client should not use keys, starting from letter or "__".)
      //
      //    Rules of correspondence between sender and receiver slots.
      //      1. Slot types matching (including transfer direction):
      //        po_* --> pi_*, qi_*, qs_*    - send a message with any kind of information.
      //        pbo_*, hbo_* --> pbi_*, qbi_*    - send command message. After the recipient receives the command, it must respond.
      //            Until that, the sender cannot issue a new command.
      //        pbi_*, qbi_* --> pbo_*, hbo_*    - respond to a command.
      //        (automatic) qs_* --> pi_*, qi_*    - messages, sent to qs slot directly or using LPA-type address,
      //            are delivered to subscribers by internal procedure.
      //      2. Slot with one-string name
      //        may send to any slot with 1) corresponding type, 2) exactly same name root, 3) with or without any additional name parts.
      //        NOTE Bidirectional output slots (pbo, hbo) with particular name,
      //          may issue commands to different target threads with same name root.
      //          In case of hbo, command messages may be sent at once to several targets with same name root;
      //          each command target is expected to reply once; this response is stored individually for the target's address,
      //          on the side of command sender, until the command sender pops it.
      //      3. Slot with multipart name may send only to slot, whose name parts are exactly equal,
      //          except for type prefix in the first part (po-pi, pbo-pbi etc.).
      //
      //    NOTE If msg is hashlist or map, all its non-scalar (.isScalar() == false) keys, and their values,
      //      are ignored, i.e. will not be passed to recipient.
      //      Under scalar keys, values may be of any type. Inside the dispatcher process, clients may even send each other user objects.
      //        See also flags.
      //
      //  att:
      //      optional binary attachment (BLOB), for sending along with the message. See also make_rba* helpers.
      //
      //  flags (ORed, default = 0):
      //    16 "anlo_msg" - allow user objects in msg to be sent by dispatcher
      //        inside the dispatcher process. msg must be given in the form of hashlist or map.
      //        If the flag is not set (default), all values of type utObject, in the internal copy of msg, are cleared (made utEmpty).
      //        To completely allow user objects passing, several conditions must be satisfied:
      //          1) per-dispatcher_mt flags __msend_anlo, __mget_anlo must not be set to false or 0.
      //          2) the sender must set its anlo_msg flag, on msend() call.
      //          3) the recipient must set anlo_msg flag on mget() call.
      //          4) user objects' (sender's) binary module must not be unloaded, until all objects are released (deleted) on recipient's side.
      //            (This may be determined querying by wrapping unity objects ref. count.)
      //        NOTE If the user objects belong to the binary module, different from that of the recipient,
      //          the recipient should take care of binary compatibility and lifetime of the objects.
      //          The original binary module may be unloaded only after all its objects are released.
      //          This situation is common in plugin-based program, where plugins are 3rd party shared libraries.
      //    32 "anlo_att" - pass att as such, without copying into intermediate object inside the dispatcher.
      //        This is the way to pass BLOBs in the current dispatcher process efficiently, without any data copying,
      //        especially to multiple recipients.
      //        (If the target address is non-local, the flag allows to make less data copies, but cannot totally avoid copying.)
      //        If the flag is not set (default), data, associated with att, is copied by value.
      //        To allow in-process BLOBs passing without copying, the following conditions must be satisfied:
      //          1) per-dispatcher_mt flags __msend_anlo, __mget_anlo are not set to false or 0. (This is the default.)
      //          2) the sender must set its own anlo_att flag, when sending a message with binary attachment (see also msend()).
      //          3) the recipient must set anlo_att flag on mget() call.
      //        NOTE If the att object belongs to the binary module, different from that of the recipient,
      //          the recipient should take care of lifetime of the object.
      //          The original binary module may be unloaded only after att is released on recipient's side.
      //          In case if att may have multiple recipients (e.g. delivered to many addresses):
      //            1) the recipients they must not modify the data reference (arrayref_t), contained in att, namely its pd() and n().
      //            2) The original binary module may be unloaded only after att is released by all recipients.
      //                E.g. the sender may keep a copy of att, and prevent shared library unloading until att.n_refs() == 1.
      //    64 "ignore_hanged" - (only) for non-local recipient, e.g. peer process:
      //          push message into local queue even if peer process appears incommunicable:
      //            a) hanged or terminated (tracked activity stopped > 3 s ago),
      //            b) normally exited,
      //            c) not exited yet, but already ended its session (this is irreversible, until peer's dispatcher_mt recreation).
      //          On ignore_hanged flag set, the message will stored locally, and sent automatically in normal way,
      //            when peer's activity is resumed. Note that the new peer may be another entity than the previous.
      //          If the flag is not set, the function will return error code -14 for cases (a, b, c).
      //          NOTE The flag does not act when the peer is communicated first time.
      //            If the peer is not accessible, the message is not pushed, and msend returns -2.
      //    128 "use_chsbin" - calculate and send checksum for att, when it's sent non-locally.
      //          The checksum is verified by recipient. On no binary attachment, this flag is ignored.
      //          NOTE use_chsbin is logically ORed with per-dispatcher_mt flag __lmsc_chsbin; att's checksum is inserted into the message
      //            if the result is true.
      //
      //  tracking:
      //      optional client-side object, receiving volatile information about message, until it's put into recipient's slot.
      //      If the object is supplied, the reference type may be the following:
      //        a) cross-module reference, b) multifunctional reference.
      //        I.e. tracking.is_cm() == true. See also cref_t is_cm.
      //      NOTE tracking is treated as non-const volatile object.
      //    tracking->id: unique message id. msend at once generates unique id and writes it to tracking->id.
      //    tracking->state: the current state of the message.
      //        a) for local target address (LP, LPA):
      //          When msend returns, both its ret. code and tracking->state are set
      //          to the same final value (1, 0, or <= -1).
      //        b) for non-local target address (LM), when msend returns:
      //          1 (transient state) - the message is cached locally and is being / will be soon transferred to another process.
      //              Later, may be changed to any final state (success or failure, see below).
      //          3 (final state of tracking) - the message has been accepted by dispatcher_mt on the receiving side,
      //              and successfully put into the recipient's slot.
      //              It still may be unread (mget) by final recipient.
      //              NOTE Tracking system does track and send notification on final recipient peeking into or popping the message.
      //          <= -1 (final state of message transfer) - failure, same as msend() error code.
      //    NOTE The tracking object is referenced by the dispatcher until
      //      a) final state of tracking is reached,
      //      b) i_dispatcher_mt proxy, to which tracking object was passed, has been released (tracking->state == -15).
      //
      // Returns:
      //    1 - success, the message has been
      //        a) written into the receiving slot (for local target address),
      //        b) queued for sending to non-local target address.
      //    0 (only for pi_* slots) - success, prev. message has been overwritten.
      //    -1 - invalid message format (address, slot name or type, sender-recipient matching) or any invalid argument.
      //    -2 - common-case failure.
      //    -3 - no session.
      //    -5 (only for command messages and responses) - rejected, out of order between the same pair of command source and target.
      //        To fix this, the client may choose to reset the slot (see request() with rt 9).
      //    -20 - target queue overflow (qs, qi, qbi) or prev. command no processed yet (pbi). The client may try again later.
      //    -6 - source slot does not exist or could not be accessed (locking timeout).
      //    -7 - source thread does not exist (see also request() rt 7).
      //    -8 - target slot does not exist or could not be accessed (locking timeout).
      //    -9 - target thread does not exist.
      //    -10 - send messages to the specified slot from this source thread is not permitted (see also arch_notes.txt: "input all", "input lpa" params. of input slots).
      //    -11 - non-local communication is disabled or not available.
      //    -12 - non-local message transfer failed on peer side.
      //    -13 (in tracking->state only) - message decoding failed on recipient side: a) unsupported msg. format, b) continuous failure of msg. decoding.
      //    -14 - (with ignore_hanged flag unset, on LM targets): failed to send because peer process appears to be hanged or terminated.
      //    -15 (in tracking->state only) - tracking is stopped (i_dispatcher_mt proxy, to which tracking object was passed, has been released).
      //        NOTE When configured by default, capacities of all types of queues are unlimited.
      //
    virtual s_long msend(const unity& msg, cref_t<t_stringref> att = cref_t<t_stringref>(), s_long flags = 0, cref_t<tracking_info> tracking = cref_t<tracking_info>()) __bmdx_noex = 0;
      //
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
    static cref_t<t_stringref> make_rba(t_stringref msg, bool b_byval = true, s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba(msg, b_byval, nbadd); }
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
    static cref_t<t_stringref> make_rba_mp(t_stringref part1, t_stringref part2, t_stringref part3 = t_stringref(), s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba_mp(part1, part2, part3, nbadd); }
      //
      // Convenience function for msend.
      //    Creates zero-initialized byte array of length nb + max(nbadd, 0).
      //    NOTE Implementation uses calloc.
      //  Returns:
      //    on success: valid non-empty cref_t<t_stringref>,
      //    on failure: cref_t<t_stringref>().
      //
    static cref_t<t_stringref> make_rba_z(s_ll nb, s_ll nbadd = 0) __bmdx_noex    { return ::bmdx_shm::_bmdx_shm::make_rba_z(nb, nbadd); }



      // Pop a message from the given slot (or peek: get a reference without popping),
      //    if available.
      //
      //  slotname:
      //      Recipient's slot in the current dispatcher thread.
      //      The name may be one-string or multipart.
      //      Multipart name may be passed directly as array, or as paramline-encoded string (manually or with encode1v()) representation of it.
      //      Slot name structure (two kinds):
      //        <slot type>_<name root>
      //          - one-string name of the slot.
      //        |<slot type>_<name root>|<name part 2>[|part 3[|part 4 ...]]
      //          - multipart (array) name of the slot (showed in string form).
      //        Detailed description: arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure.
      //    Recipient's slot types:
      //      pi, qi - receive-only slot - mget returns a message sent by someone,
      //      pbi, qbi - command recipient - mget returns a command, to which the client will have to respond (msend from pbi, qbi to original sender's address),
      //      pbo, hbo - command sender - mget returns the response to previously sent command.
      //
      //  retmsg:
      //    a) on success, if a message is available and discard_msg flag is not set, receives the message.
      //    b) in any other cases, retmsg is cleared (utEmpty).
      //    By default, the returned message is represented as hashlist (see also get_hashlist flag),
      //    containing (as minimum) the following entries:
      //      { "src"; <ADDR1> }
      //      { "trg"; <ADDR2> }
      //      {  "text"; <T> }
      //    T is the sender's message itself.
      //        Usually, T is a string, numeric value, one-dimensional 1-based array, or empty.
      //        If ADDR1 is local address, and anlo_msg flag was set both in mget and in the original msend,
      //          T may contain any object of set of objects.
      //      NOTE The sender is allowed to put additional entries into the message.
      //        Entry name (hashlist key) is a string >= 2 chars., starting from '_'. The second character != '_'.
      //        All keys, starting from letter or "__", should be ignored by the recipient.
      //    ADDR1 is the complete sender's address, suitable for responding to the message (if it's necessary).
      //      The address may be of any type (LP, LPA, LM), allowed for the current slot type.
      //      NOTE Except for command inputs (pbi, qbi), if the recipient needs to respond,
      //        it cannot send the response through the same slot from which the original message has been popped.
      //        See also msend comment "Rules of correspondence between sender and receiver slots".
      //    ADDR2 is the original recipient's address, as specified on the sender's side.
      //      The address may be used to determine if the sender is local, and what names it uses.
      //
      //  retatt (optional): points to the receiving variable for binary attachment (BLOB),
      //    associated with the message.
      //    1. If retatt == 0, but the message contains the attachment,
      //      attachment's bytes are trivially recoded into wide string (wide chars. with values 0..255)
      //      and put into "bin" key of the returned message.
      //    2. If retatt != 0, but *retatt does not receive an attachment (on no attachment or on error), it's anyway cleared.
      //    See also (below): anlo_att flag use for efficient BLOBs passing along with messages.
      //
      //  flags (ORed, default = 1|4):
      //    1 "get_hashlist" - return hashlist in retmsg. If not set, return string in retmsg.
      //      By dflt., the flag is set.
      //    2 "discard_msg" - do not write the message into retmsg.
      //      a) if 8 ("peek_only") flag is not set, the message is lost.
      //      b) if 8 ("peek_only") flag is set, discard_msg flag is ignored. The client gets a copy of the existing message.
      //      NOTE For command input: even if the message is discarded,
      //          the client must respond anyway:
      //        a) if both command sender's address and the command are simple, fixed and known statically (aka signal):
      //          1. Discard the message (with flag 2).
      //          2. React and respond to the command as necessary.
      //        b) for multiple sources and variable commands:
      //          1. Peek the message first (with flags (1|8|4)).
      //          2. Extract sender's address ("src" key) and/or the command ("text" key).
      //          3. Discard the message and the attachment, if exists (with flags (2|4)).
      //          4. React and respond to the command as necessary.
      //    4 "discard_att" - ignore (do not return in any way) message binary attachment, even if it exists.
      //      By dflt., the flag is set.
      //      a) if 8 ("peek_only") flag is not set, the binary attachment, if exists, is lost.
      //      b) if 8 ("peek_only") flag is set, the binary attachment continues to exist,
      //        until the message is actually popped or discarded.
      //    8 "peek_only" - return everything as specified by other flags, only do not pop the message.
      //        May be used find out if a message is available, without actual copying and returning any data to the client.
      //        E.g. call mget with flags (8|2|4).
      //    16 "anlo_msg" - allow user objects to be returned in retmsg, when it's in the form of hashlist.
      //          This is the way to pass user objects inside the current process.
      //        If the flag is not set (default), all values of type utObject, contained in retmsg, are cleared (made utEmpty).
      //        See also msend anlo_msg flag for more detailed description.
      //    32 "anlo_att" - if the message contains binary attachment, and retatt != 0 is given,
      //          set the attachment directly into *retatt, even if it is non-local object.
      //          This is efficient way to pass BLOBs inside the current process without any data copying.
      //        If the flag is not set (default), *retatt will be assigned a by value copy of the attachment,
      //          created in the binary module of retmsg (normally, it's the same as mget client's).
      //        See also msend anlo_att flag for more detailed description.
      //
      // Returns:
      //    2 - success, got two-part message (msg. itself and the binary attachment).
      //        Parts are returned through retmsg, *retatt, as specified by flags.
      //    1 - success, got single-part message (no binary part). The message is returned .
      //        The message is returned through retmsg, as specified by flags. *retatt is cleared.
      //    On any of the below results, retmsg, *retatt are cleared.
      //      0 - no message, or (pbo, hbo) no response because of no command sent.
      //      -20 - (pbo, hbo only) waiting for command completion, still command target sent no response.
      //          See also ret. code -21 below.
      //      -21 - (pbi, qbi only) waiting for the client (command target) responding
      //          to the current (recently popped) command. The client must first reply to the current command,
      //          and after should try to pop the next command.
      //          This may occur if, in the past, the client had occasionally lost the control right after popping the command,
      //          and sent no reply to command sender. Now the control is regained, and the command sender still waits for response.
      //          If the client does not know how to respond correctly, there are 2 options:
      //            a) if the client (command target) remembers or knows command sender address, send there a message,
      //              containing, in addition to addresses ("src", "trg" keys), and empty "text" and "_e_retry" keys.
      //              In this way, the command sender should always check the responses for "_e_retry" key, and resend the commands when necessary.
      //            b) the client (command target) may reset the slot phase (see request() with rt 9), to be able to receive new commands.
      //              This way is recommended for non-local command/response exchange.
      //              The command sender should
      //                1) use tracking to see if the command message has been put into command target's slot.
      //                2) after that, if the command target does not respond for too long (command sender's mget returns -20),
      //                  the command sender should reset its slot (pbo, hbo) as well, to be able to re-send the command (or send another).
      //      -1 - invalid slot name or type.
      //      -2 - common-case failure.
      //      -3 - no session.
      //      -6 - the specified slot does not exist or could not be accessed (locking timeout).
      //      -7 - thread does not exist (see also request() with rt 7).
      //
    virtual s_long mget(const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt = 0, s_long flags = 1|4) __bmdx_noex = 0;



      // Subscribe, unsubscribe for messages, or check subscription state.
      //
      //  addr_qs - address of the queue, used for delivering the messages to all subscribed clients.
      //    The address may be of any type:
      //      LP - local queue in the explicitly specified dispatcher thread,
      //      LPA - local queue, residing in the dispatcher thread, whose name is not known to the subscribing client,
      //      LM - queue in the explicitly specified dispatcher thread in another dispatcher process.
      //    Detailed structure of the address: arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure.
      //
      //  recv_sl - name of the recipient's slot (type qi or pi), in the current thread.
      //    For subscribe() success, the recipient's slot must have the same name root as target qs.
      //
      //  rt - request type:
      //      1 - subscribe,
      //      2 - unsubscribe,
      //      3 - check if subscribed (but do not change anything).
      //    The request is completed (i.e. with final result) immediately only if addr_qs is local (LP, LPA).
      //    For out-of-process targets, the client should supply tracking object, which will asynchronously
      //    receive the final result code when it's available.
      //
      //  tracking:
      //    If the object is supplied, the reference type may be the following:
      //      a) cross-module reference, b) multifunctional reference. I.e. tracking.is_cm() == true. See also cref_t is_cm.
      //    tracking->id: unique request id. subscribe() at once generates unique id and writes it to tracking->id.
      //    tracking->state: the current state of the request. Any of subscribe() ret. codes, see below.
      //
      // NOTE When a subscription is created dynamically (subscribe()),
      //      on dispatcher restart (initialization) from any side (qs slot, receiving slot),
      //      the subscription is not recovered automatically.
      //    In contrary, pre-configured subscriptions are created automatically on dispatcher initialization:
      //      a) from the side of qs slot: see arch_notes.txt "output" param. of qs slot,
      //      b) from the side of subscribed slot: see arch_notes.txt "input qsa" param. of pi, qi slots.
      //
      // Returns (directly, + sets the same into tracking->state, treated as volatile const.):
      //    0 (transient state, for non-local addr_qs only) - the request has been sent successfully.
      //    1 (final state) - unsubscribed successfully, or was not subscribed.
      //    2 (final state) - subscribed successfully, or was subscribed.
      //  Error codes (final state):
      //    -1 - invalid args (address, slot name or type, request type).
      //    -2 - common-case failure.
      //    -3 - no session.
      //    -5 - recv_sl does not exist or could not be accessed (locking timeout).
      //    -6 - addr_qs does not exist on the destination side or could not be accessed (locking timeout).
      //    -10 - addr_qs exists, but does not allow (un-)subscribing recv_sl. See also arch_notes.txt: "output fixed" param. of qs slot.
      //    -11 - non-local communication is disabled or not available.
      //    -12 (non-local request only) subscription request failure on peer side.
      //    -15 (non-local request only) subscription tracking is stopped (e.g. end of session during tracking).
      //
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt, cref_t<tracking_info> tracking = cref_t<tracking_info>()) __bmdx_noex = 0;



      // Request information or perform an extended operation.
      //
      //  Common return values:
      //    1 - success.
      //    -1 - invalid request type (rt).
      //    -2 - operation failed.
      //    -3 - no session.
      //    -4 - operation not permitted.
      //    -5 - invalid argument (args and/or retval as required by request type).
      //  Common rule for retval:
      //    retval is cleared a) if request has failed, b) if retval is not used in this type of request.
      //    Otherwise, retval contains the result of the request, in the format according to its type.
      //
      //  rt (request type):
      //
      //    rt 1 (get_threads_names) - get names of all dispatcher threads.
      //      args: not used.
      //      On success, retval: 0-based utStringArray.
      //
      //      rt 11 (get_threads_n) - get number of dispatcher threads.
      //        args: not used.
      //        On success, retval: utInt.
      //
      //    rt 2 (get_th_slots_names) - get names of all slots of a thread.
      //      args: thread name (string).
      //      On success, retval:
      //          0-based utStringArray.
      //          Slot names are is string form. If necessary to convert them into array form, use paramline decode().
      //      Specific return value: -7 - thread does not exist.
      //
      //      rt 12 (get_th_slots_n) - get number of slots in a thread.
      //        args: thread name (string).
      //        On success, retval: utInt.
      //        Specific return value: -7 - thread does not exist.
      //
      //    rt 3 (set_qs_priority) - set priority and timing for internal thread, delivering subscription messages from all qs slots.
      //      args: array(<priority>, <delivery loop sleep time>).
      //        priority: 1..7 in units of threadctl. (Dflt. priority (normal) is 4.)
      //        sleep time: in mcs, >= 0
      //      retval: not used.
      //      Specific return value: -7 - the internal delivery thread does not exist (configured so).
      //
      //    rt 4 (get_qs_priority) - get priority and timing for internal thread, delivering subscription messages from all qs slots.
      //      retval: 1-based utIntArray with 2 elements same as args in rt == 3, see above.
      //      Specific return value: -7 - the internal delivery thread does not exist (configured so).
      //
      //    rt 5 (create_proxy) - create a new proxy object and assign it to retval.
      //      args: thread name (string) for the new proxy.
      //      See also: dispatcher_mt :: new_proxy.
      //      NOTE retval is re-initialized to belong to the binary module of the current dispatcher process (i.e. dispatcher_mt object).
      //      Specific return value: -7 - thread does not exist.
      //
      //    rt 6 (create_disp_thread) - create a new dispatcher thread.
      //      args: configuration of a thread, as described in arch_notes.txt / MESSAGE DISPATCHER / Configuration parameters.
      //        Only one thread configuration ({ thread name; slots and thread params. }) may be specified.
      //      flags:
      //        0x1 "ensure_local_subs" - if set, the request checks if all pre-defined local subscriptions of all slots of the new thread
      //          (local addresses in qs slots "output", and pi, qi "input qsa" keys),
      //          have been created successfully. If not so, the request cancels thread creation and returns -2.
      //          NOTE This request updates, but does not verify non-local subscriptions.
      //            To explicitly verify any subscriptions, the client should use subscribe().
      //      retval: not used.
      //      NOTE If the thread contains pre-configured subscriptions, all of them are automatically updated,
      //        but if any of them fails, the thread creation is not canceled, and the request returns 1 (success).
      //        Additional options:
      //          1) Local subscription update with verification is done by slots_create (with flag 0x1 set).
      //          2) To explicitly verify any subscriptions, the client should use subscribe().
      //      Specific return value: -6 - thread already exists.
      //
      //    rt 7 (remove_disp_thread) - remove the existing thread. May be applied to any thread, incl. one associated with the current proxy.
      //      args: thread name (string).
      //      retval: not used.
      //      Specific return value: -7 - thread does not exist.
      //
      //    rt 8 (deliver_qs_now) - deliver all pending messages from all qs slots, configured in the current dispatcher process, right now.
      //      retval: not used.
      //      args: must contain integer value with ORed flags:
      //          0x1 - deliver messages from the calling dispatcher thread's qs slots, configured with "delivery = thread".
      //          0x2 - deliver messages from all threads' qs slots, configured with "delivery = disp" .
      //            This action is performed only if the current thread is configured with disp = true, otherwise it's no-op.
      //      NOTE This request, with args = 3, replaces former periodic() function.
      //      NOTE Making this request is necessary for client only if any:
      //          a) the current or another dispatcher threads contain qs slots, but automatic delivery is disabled (__thm_lqsd = 0).
      //          b) the current thread contains qs slots, configured with "delivery = thread".
      //        If no qs slots are covered by the specified flags, the request is no-op.
      //      NOTE If automatic delivery is enabled (__thm_lqsd = 1 or 2), the request may speed up delivery (depends on internal thread timing).
      //
      //    rt 9 (reset_cmdslot_phase) - reset phase in the given command slot (pbo, hbo, pbi, qbi).
      //      Result: if the request is satisfied, the command slot is ready to issue/receive new command to/from any address.
      //      This request may be used
      //        a) by command sender, when it detects that its command has been lost or timed out due to exceptional situation.
      //          E.g. mget returning -20 for too long, which may mean terminated command recipient thread or peer process.
      //        b) by command sender, if it attempts to send a command in normal way, but its msend returns -5 (out of order).
      //        c) by command recipient, if it attempts to respond to a command, but its msend returns -5 (out of order).
      //        d) by command recipient, if it polls for new command, but its mget returns -21 (out of order).
      //      Common usage hint:
      //        For any particular pair of command sender - command target slots, if any side has been reset (or restarted, recreated),
      //          another side must be reset (or restarted, recreated) as well.
      //          Otherwise, on attempt to send new command, msend will anyway fail with code -5 (out of order).
      //
      //      args: hashlist, map or string: src = <slot name>; trg = <target address>
      //        a) for pbo, pbi, qbi: only src is required (target address is not applicable).
      //          NOTE qbi: the currently queued input commands are not deleted.
      //        b) for hbo: both src and trg are required.
      //          trg selects the command target, for which the client has the out of order situation.
      //          Slot phase will be reset in association with the command target (trg) only.
      //        Hint: user message (msg of msend) itself may be passed as request argument.
      //      retval: not used.
      //
      //    rt 10 (set_shmqueue_conf) - customize the internal shared queue for communicating with the given peer process.
      //        This request calls any or both of shmqueue_s :: conf_set_al_in, conf_set_lqcap,
      //          and returns the results.
      //      args: hashlist of arguments.
      //          args("peer_name", <peer process name>): required setting.
      //          args(<function name>)(<arg. name>, <arg. value>)(...)...: optional settings.
      //          Each particular function is called only if its name is specified in args.
      //          See also the below example.
      //      retval:
      //          a) hashlist of shmqueue_s function calls results:
      //            retval[<function name>]
      //            If the function was called: the value is integer ret. code.
      //            If the function was not called: the value is empty.
      //          b) empty: may occur (on request ret. code == 0, -1..-6) if the error occurred before doing the request.
      //          shmqueue_s functions ret. codes are in retval[<function name>].
      //          See also the below example.
      //      Specific return value of request():
      //        0 - nothing to do (no functions specified).
      //        -6 - nothing is done, because LMSC (shared memory controller) is disabled by dispatcher configuration.
      //        -7 - some of shmqueue_s functions failed.
      //      flags:
      //        0x1 - if set, resets all queue parameters to default on the current dispatcher proxy being destroyed.
      //          If not set (dflt.), the queue parameters will be reset on dispatcher session being destroyed.
      //        NOTE To reset the allocator to default value at any time, call request() with the following setting:
      //          args("conf_set_al_in")("p_al", unity());
      //          If "p_al" key is omitted, the function does not change the allocator
      //          (equiv. to p_al = 0 in shmqueue_s::conf_set_al_in).
      //
      //      EXAMPLE
      //          The following code makes binary attachments for some max. number of messages
      //          from dispatcher process "prc2" getting into pre-allocated memory,
      //          which is the fastest way of queue-based data transfer.
      //
      //          using namespace bmdx_shm;
      //          const s_ll nmsgs = 100;
      //          const s_ll nbmsg = (1 * 1024 * 1024) + 2000; // 1 MB for binary attachment + e.g. 2000 bytes for encoded dispatcher message itself
      //          cref_t<i_allocctl> al = allocctl_pre::create(0xff, nbmsg * nmsgs, nmsgs); // use pre-allocated memory for messages
      //            if (!al) { <handle pre-allocation failure> }
      //
      //          unity args, retval;
      //          args("peer_name", "prc2");
      //          args("conf_set_al_in")("p_al", unity(al, false))("timeout_ms", 500);
      //          args("conf_set_lqcap")("b_receiver", true)("ncapmin", -1)("ncapmax", -2)("nrsv", nmsgs)("timeout_ms", 500);
      //
      //          s_long res = disp->request(dispatcher_mt_rt::set_shmqueue_conf, retval, args);
      //            if (res == 0)
      //            {
      //              if (retval["conf_set_al_in"].vint() < 0) { <handle the error> }
      //              if (retval["conf_set_lqcap"].vint() < 0) { <handle the error> }
      //            }
      //            if (res != 1) { <handle request error> }
      //
      //      NOTE In the dispatcher, setting custom allocator for particular shared queue
      //          does not totally disable dynamic memory allocations, associated with that queue.
      //          It influences only the corresponding shmqueue_s, to help receiving
      //          long series of shmqueue_s messages, containing dispatcher messages + large attachments,
      //          without excessive dynamic allocations.
      //          a) The main dispatcher message (mget retmsg) is always decoded from the received plain bytearray
      //            in normal way, which involves dynamic memory allocations according to message contents.
      //          b) The attachment remains inside the memory, provided by custom allocator.
      //            It will be returned to client as such, without excessive copying,
      //            if mget is called with anlo_att flag set.
      //
      //    rt 13 (get_qs_nvsubs):
      //        a) get number of slots, subscribed to the given local qs slot.
      //        b) if (flags & 0x1) != 0, get complete list of subscribed slot addresses (strings).
      //      args: complete local (LP or  LPA) qs slot address, analogous to addr_qs arg. of subscribe() (see).
      //      retval:
      //        a) on success, the number of slots: integer >= 0.
      //        b) on success, the list of slot addresses: 0-based utStringArray.
      //        c) on failure: empty.
      //      Specific return value:
      //        -6 - the specified qs slot does not exist, or could not be accessed (locking timeout).
      //
    virtual s_long request(s_long rt, unity& retval, const unity& args = unity(), s_long flags = 0) __bmdx_noex = 0;



      // Dynamically creates one or more slots in the current dispatcher thread.
      //
      //  slotscfg:
      //    contains configuration tree for the new slots, as associative array (hashlist, map),
      //      or as string representation of that array, paramline-encoded (manually or with encode_tree(), for decode_tree() with flags 0x3a).
      //    Structure of such configuration is subset of full dispatcher configuration.
      //    This subset describes the list of new slots and their parameters, as follows:
      //      { "slots"; { <slot name>; <slot configuration, or empty> } }
      //    Details on slot name format and configuration are in
      //      arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure, Configuration parameters, Examples of configuration.
      //    Brief descr. of slot types:
      //      po - output-only pin, message goes directly to the recipient.
      //      pi - input-only pin, messages may be overwritten if not popped in time.
      //      qi - non-blocking input queue for values; any number of messages from one or more sources
      //          are queued and do not overwrite each other.
      //        NOTE When configured by default, capacities of all types of queues are unlimited.
      //      pbo, hbo - command sender pin. Ensures the sequence
      //          1) command sender sent a command,
      //          2) command target popped and executed the command,
      //          3) command target pushed a response, and now is ready for new command,
      //          4) command sender popped and analyzed the response, and now is ready to issue new command.
      //          pbo can send one command at a time, to any target.
      //          hbo can send commands to multiple targets, one at a time per target.
      //      pbi, qbi  - command target pins.
      //            pbi can hold only one command at a time, so it's convenient only for sequential exchange
      //              with single command sender only.
      //            qbi is a queue, to which multiple senders can push their commands independently, without collisions.
      //              The commands are queued, and the client (command target) pops/executes/responds to them one by one.
      //      qs - the queue for delivering the messages to multiple subscribed clients (from multiple senders).
      //    Primitive example of configuration (in string form):
      //      for thread A:
      //        =|slots; po_strings; pi_hash_values
      //      for thread B:
      //        =|slots; pi_strings; po_hash_values
      //      Thread A sends some strings to thread B. Thread B calculates hash values of that strings, and sends them back to thread A.
      //      Threads are designed so that thread A won't send another string until thread B "returns" hash value for the previous string.
      //    Example 2 (same task, but using command slots):
      //      for thread A:
      //        =|slots; pbo_hash_calc
      //      for thread B:
      //        =|slots; pbi_hash_calc
      //      Here, the dispatcher as such will ensure the sequential data exchange.
      //      Thread B has to respond (with hash value) to each string, sent by thread A.
      //      Thread A cannot send another string until thread B responds with the hash value.
      //
      //  flags:
      //      0x1 "ensure_local_subs" - if set, the function checks if all pre-defined local subscriptions
      //        (local addresses in qs slots "output", and pi, qi "input qsa" keys)
      //        have been created successfully. If not so, the function cancels slots creation and returns -2.
      //        NOTE slots_create updates, but does not verify non-local subscriptions.
      //          To explicitly verify any subscriptions, the client should use subscribe().
      //
      // Returns:
      //    1 - success.
      //    -1 - invalid or incompatible slotscfg.
      //    -2 - common-case failure.
      //    -3 - no session.
      //    -4 - one of new slot names is not unique.
      //    -7 - thread does not exist.
      //
    virtual s_long slots_create(const unity& slotscfg, s_long flags = 0) __bmdx_noex = 0;



      // Removes slots from the current dispatcher thread.
      //  slotnames: may be
      //    a) Name of single slot, in the form of array (utStringArray or utUnityArray).
      //      The array must be non-empty, 1-based. The first member must be string (po_..., qi_...  etc.).
      //    b) Name of single slot, as paramline-encoded (with encode1v(), for decode1v()) string.
      //      (NOTE For identifier-like strings, paramline-encoded form is exactly same as original string.)
      //    c) List of slot names, as an array (utUnityArray).
      //      This case works, instead of (a), only if (flags & 1) != 0.
      //      Each element in the list should be like (a) or (b) above.
      //    d) Associative array, whose keys are slot names in string form, like in (b) above.
      //      NOTE Case (d) is useful for removing slots, for which the client remembers original configuration variable.
      //        (Only associative array keys are used, values are ignored.)
      //
      //        E.g. 1: the client may extract slots configuration from associative form of slotscfg arg. of slots_create(), used to add slots in the past:
      //              slots_remove(slotscfg["slots"]);
      //          This removes all slots, created by slots_create(slotscfg). added See also slots_create().
      //
      //        E.g. 2: the client may extract slots configuration from associative form of cfg arg. of dispatcher_mt().
      //              o_iptr_t<i_dispatcher_mt> pdisp;
      //                // ... init. pdisp for accessing the chosen dispatcher thread.
      //              unity name_th; pdisp->thread_name(name_th);
      //              slots_remove(cfg[name_th]["slots"]);
      //          This removes all slots of the thread name_th, originally created on dispatcher_mt initialization. See also dispatcher_mt().
      //
      //    NOTE Details on slot name format are in
      //      arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure.
      //
      //  flags:
      //    0x1 "array_of_slotnames" not set (dflt.): if slotnames is an array, it specifies single slot name in the form of array - case (a).
      //    0x1 is set: if slotnames is an array, it specifies an array of slot names - case (c).
      //    other bits: reserved, should be 0.
      //
      // Returns:
      //    2 - success.
      //    1 - removed only part of the given slots, because that some of slots do not exist (or bad names given).
      //    0 - removed only part of the given slots, because of common-case failure.
      //    -1 - invalid argument (bad value of slotnames arg.).
      //    -2 - common-case failure.
      //    -3 - no session.
      //    -7 - thread does not exist.
      //
    virtual s_long slots_remove(const unity& slotnames, s_long flags = 0) __bmdx_noex = 0;



      // Returns the current dispatcher thread (that of the current proxy) name.
      //    Thread names are unique within the process.
      //    Thread name cannot be empty string.
      //  retname: on success, assigned the current thread name.
      //
      // Returns:
      //    1 - success. retname: assigned the current thread name.
      //    -2 - common-case failure. retname is not changed.
      //
    virtual s_long thread_name(unity& retname) __bmdx_noex = 0;



      // Get the dispatcher process name.
      //    Process names are unique within the local computer.
      //    Process name cannot be empty string.
      //  retname: on success, assigned the dispatcher process name.
      // Returns:
      //    1 - success. retname: assigned the dispatcher process name.
      //    -2 - common-case failure. retname is not changed.
      //
    virtual s_long process_name(unity& retname) __bmdx_noex = 0;



    virtual ~i_dispatcher_mt() __bmdx_exany {}
  };



  template<> struct o_proxy<i_dispatcher_mt> : o_proxy_base<i_dispatcher_mt>
  {
    static const char* __iname() { return "Ye.K./bmdx/i_dispatcher_mt/v2"; }
    typedef i_dispatcher_mt::tracking_info tracking_info;
    struct __msend { typedef s_long (*PF)(__Interface* __pi, const unity* msg, cref_t<t_stringref>* att, s_long flags, cref_t<tracking_info>* tracking); static s_long F(__Interface* __pi, const unity* msg, cref_t<t_stringref>* att, s_long flags, cref_t<tracking_info>* tracking) { return __pi->msend(*msg, *att, flags, *tracking); } };
    struct __mget { typedef s_long (*PF)(__Interface* __pi, const unity* slotname, unity* retmsg, cref_t<t_stringref>* retatt, s_long flags); static s_long F(__Interface* __pi, const unity* slotname, unity* retmsg, cref_t<t_stringref>* retatt, s_long flags) { return __pi->mget(*slotname, *retmsg, retatt, flags); } };
    struct __subscribe { typedef s_long (*PF)(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt, cref_t<tracking_info>* tracking); static s_long F(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt, cref_t<tracking_info>* tracking) { return __pi->subscribe(*addr_qs, *recv_sl, rt, *tracking); } };
    struct __request { typedef s_long (*PF)(__Interface* __pi, s_long rt, unity* retval, const unity* args, s_long flags); static s_long F(__Interface* __pi, s_long rt, unity* retval, const unity* args, s_long flags) { return __pi->request(rt, *retval, *args, flags); } };
    struct __slots_create { typedef s_long (*PF)(__Interface* __pi, const unity* slotscfg, s_long flags); static s_long F(__Interface* __pi, const unity* slotscfg, s_long flags) { return __pi->slots_create(*slotscfg, flags); } };
    struct __slots_remove { typedef s_long (*PF)(__Interface* __pi, const unity* slotnames, s_long flags); static s_long F(__Interface* __pi, const unity* slotnames, s_long flags) { return __pi->slots_remove(*slotnames, flags); } };
    struct __thread_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->thread_name(*retname); } };
    struct __process_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->process_name(*retname); } };
    virtual s_long msend(const unity& msg, cref_t<t_stringref> att, s_long flags, cref_t<tracking_info> tracking) __bmdx_noex { return __call<__msend>()(__psi(), &msg, &att, flags, &tracking); }
    virtual s_long mget(const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt, s_long flags) __bmdx_noex { return __call<__mget>()(__psi(), &slotname, &retmsg, retatt, flags); }
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt, cref_t<tracking_info> tracking) __bmdx_noex { return __call<__subscribe>()(__psi(), &addr_qs, &recv_sl, rt, &tracking); }
    virtual s_long request(s_long rt, unity& retval, const unity& args, s_long flags) __bmdx_noex { return __call<__request>()(__psi(), rt, &retval, &args, flags); }
    virtual s_long slots_create(const unity& slotscfg, s_long flags) __bmdx_noex { return __call<__slots_create>()(__psi(), &slotscfg, flags); }
    virtual s_long slots_remove(const unity& slotnames, s_long flags) __bmdx_noex { return __call<__slots_remove>()(__psi(), &slotnames, flags); }
    virtual s_long thread_name(unity& retname) __bmdx_noex { return __call<__thread_name>()(__psi(), &retname); }
    virtual s_long process_name(unity& retname) __bmdx_noex { return __call<__process_name>()(__psi(), &retname); }
    typedef unity_common::fn_list<__msend, __mget, __subscribe, __request, __slots_create, __slots_remove, __thread_name, __process_name> __Methods;
  };
  namespace { o_proxy<i_dispatcher_mt> __o_proxy_i_dispatcher_mt_inst; }



  struct dispatcher_mt
  {
      // Creates the "dispatcher process", and at once starts communication session.
      //    (Terminology: see arch_notes.txt / MESSAGE DISPATCHER / Terminology)
      //
      //  process_name:
      //    name of the dispatcher process, unique for the local computer.
      //    Name uniqueness is checked automatically by dispatcher_mt(), by means of IPC.
      //    Name may anything except the following: empty string, "LP", "LM", "R", "LPA", "LMA", "RMA".
      //
      //  cfg: application-specific communication scheme and parameters.
      //    This structure is tree-like. Normally, specified in any of forms:
      //        a) multilevel associative array (hashlist or map),
      //        b) string form of the above, paramline-encoded (manually or with encode_tree(), for decode_tree() with flags 0x3a).
      //        The upper level of cfg tree contains all dispatcher threads, with their configurations,
      //          plus all global (i.e. per-dispatcher_mt) parameters.
      //    Also,
      //        c) cfg may be empty, or contain only dispatcher process parameters, without threads.
      //          In this case, threads and their slots should be added later dynamically,
      //          using i_dispatcher_mt request() with rt 6, and/or slots_create.
      //    Formal description:
      //      cfg: {
      //        <thread name>; {
      //          "slots"; { <slot name>; <slot configuration, or empty> }
      //          <thread parameter name>; <value>
      //        }
      //        <per-dispatcher_mt parameter name>; <value>
      //      }
      //      Slot configuration: { <slot parameter name>; <value> }
      //      Value of per-dispatcher_mt, thread, and slot parameter:
      //        scalar, array or associative array, as described in arch_notes.txt / MESSAGE DISPATCHER / Configuration parameters.
      //      All elements are optional.
      //    See also arch_notes.txt / MESSAGE DISPATCHER / Examples of configuration.
      //
      //  flags:
      //      0x1 "ensure_local_subs" - if set, dispatcher_mt() at once checks if all pre-defined local subscriptions of all slots of all created threads
      //          (local addresses in qs slots "output", and pi, qi "input qsa" keys),
      //          have been created successfully. If not so, dispatcher_mt() fails: is_valid() will return false.
      //          NOTE dispatcher_mt() requests, but does not verify non-local subscriptions.
      //            To explicitly verify any subscriptions, the client should use subscribe().
      //      0x100 "disp_off_th_lqsd" - disable the thread, responsible for automatic message delivery for local qs slots. Suppresses __thm_lqsd configuration setting (equiv. to __thm_lqsd = 0).
      //      0x200 "disp_off_th_lmsc" - disable the thread, responsible for interprocess message delivery. Suppresses __thm_lmsc configuration setting (equiv. to __thm_lmsc = 0).
      //      0xfff00 "disp_off_th_mask" - a mask for disabling all kinds of additional threads and active subsystems, started by dispatcher_mt. (Includes bits, reserved for that in future versions.)
      //
      // Results:
      //
      //  a) On successful construction, an internal session object is created.
      //    is_valid() returns true.
      //    The client creates proxy objects, with i_dispatcher_mt interface, to access dispatcher threads.
      //    See also new_proxy().
      //    The client passes proxy objects into application-defined CPU threads, so that they could communicate
      //      with each other and with another dispatcher processes inside this and other CPU processes.
      //      NOTE Proxy object may be passed with usual function call, or via common objects storage (see struct pcos).
      //    Application-defined client of the proxy may reside as in the main executable, so in any kind of
      //      dynamically loaded shared library.
      //      By design of BMDX library, all proxy functions and associated objects are compiler-independent.
      //    One CPU process may create one or more dispatcher processes (dispatcher_mt instances), as needed
      //      for application architecture.
      //    Two dispatcher processes communicate by means of IPC (based on shared memory).
      //      In i_dispatcher_mt, LM-type addresses are used for that.
      //      Inside one dispatcher process, threads may use LM-type addresses as well.
      //      NOTE The dispatcher does not re-interpret any non-local address as local.
      //        Address type imperatively selects technical way of message transfer, with all associated limitations.
      //    Communication session may end in several ways:
      //      1. On ~dispatcher_mt().
      //      2. On end_session().
      //      3. When both dispatcher_mt and all client proxies are completely released.
      //
      //  b) On failure, is_valid() == false.
      //
    dispatcher_mt(arrayref_t<wchar_t> process_name, const unity& cfg = unity(), s_long flags = 0) __bmdx_noex;
    dispatcher_mt(arrayref_t<char> process_name, const unity& cfg = unity(), s_long flags = 0) __bmdx_noex;
    ~dispatcher_mt() __bmdx_noex;

      // Returns:
      //    true - if dispatcher_mt constructor has succeeded.
      //    false - a) if dispatcher_mt constructor has failed, b) after ~dispatcher_mt().
      //
      // Concurrency: is_valid() may be called concurrently.
      //
    bool is_valid() const __bmdx_noex;

      // Returns:
      //    true: during the period, starting from successful construction,
      //      and ending with a) ~dispatcher_mt(), or b) end_session().
      //    false: beyond this time period.
      //
      // Concurrency: has_session() may be called concurrently.
      //
    bool has_session() const __bmdx_noex;

      // Resets an internal flag to signal that the session is no longer valid.
      //    Internal threads are stop-signaled.
      //    Any call via any i_dispatcher_mt will return error code -3 ("no session").
      // NOTE Anyway, until ~dispatcher_mt(), dispatcher_mt strongly references its session object.
      //
      // Concurrency: end_session() may be called concurrently.
      //
    void end_session() __bmdx_noex;

      // Returns wrapped proxy object with interface i_dispatcher_mt.
      //
      // Typical sequence of dispatcher_mt and proxy use:
      //    1. Create dispatcher_mt instance (with application-specific communication scheme and parameters in cfg arg.).
      //      Check if dispatcher_mt is_valid(), i.e. construction and initialization succeeded.
      //    2. For each client (communication party), create wrapped proxy object
      //      with new_proxy().
      //      Pass the wrapped proxy objects to clients in application-specific way.
      //    3. In each client, get i_dispatcher_mt pointer from proxy object,
      //      received (see above) from dispatcher_mt creator.
      //      Use the pointer to send and receive messages,
      //        optionally dynamically create/remove dispatcher threads and slots,
      //        optionally create/remove subscriptions,
      //        optionally create new proxies etc.
      //  E.g.
      //      dispatcher_mt disp("unique name", cfg);
      //      unity proxy1, proxy2;
      //      disp.new_proxy(proxy1, "client1");
      //      disp.new_proxy(proxy2, "client2");
      //      ... // pass proxy1 and proxy2 to clients.
      //    // === in client 1 ===
      //      o_iptr_t<i_dispatcher_mt> pdisp = proxy1.pinterface<i_dispatcher_mt>();
      //      ... // use pdisp
      //    // === in client 2 ===
      //      o_iptr_t<i_dispatcher_mt> pdisp = proxy2.pinterface<i_dispatcher_mt>();
      //      ... // use pdisp
      //
      // IMPORTANT One dispatcher thread may have >= 0 proxies associated.
      //  The proxy is completely thread-safe and compiler-independent.
      //  Until being released, each proxy strongly references common dispatcher session object.
      //  Beyond proxy lifetime, the dispatcher session object does not store any client's objects (made in client's binary module),
      //    unless the client intentionally sets anlo_msg, anlo_att flags in the call to msend (see i_dispatcher_mt msend, mget).
      //
      // See also:
      //      dispatcher_mt()
      //      arch_notes.txt / MESSAGE DISPATCHER / Slot types and address structure, Configuration parameters, Examples of configuration.
      //
      // Arguments:
      //    dest: receives the wrapped proxy object, with i_dispatcher_mt interface attached.
      //    thread_name: dispatcher thread name, as needed to identify particular communicating client.
      //      The thread with this name should have been either defined in cfg argument to dispatcher_mt constructor,
      //      or added dynamically before the current call to new_proxy().
      //
      // Concurrency: new_proxy() may be called concurrently.
      //
      // Return value:
      //  1 - success. dest contains the proxy object, associated with the given thread name.
      //    In all the below cases, dest is cleared (empty).
      //    -2 - common-case failure.
      //    -3 - no session.
      //    -5 - invalid thread name.
      //    -7 - thread does not exist.
      //
    s_long new_proxy(unity& dest, arrayref_t<wchar_t>  thread_name) const __bmdx_noex;
    s_long new_proxy(unity& dest, arrayref_t<char>  thread_name) const __bmdx_noex;

      // Similar to i_dispatcher_mt::request. Differences:
      //  1) has all possible permissions.
      //  2) the request is not associated with any particular thread (certain request types may fail).
      //
      // Concurrency: request() may be called concurrently.
      //
      // Return value:
      //    same as i_dispatcher_mt::request.
      //
    s_long request(s_long rt, unity& retval, const unity& args = unity(), s_long flags = 0) __bmdx_noex;

      // Flags, permitting serious operations from the side of any proxy (i_dispatcher_mt request()):
      //    0x1 - create another thread.
      //    0x2 - remove any thread, including proxy's own.
      //    0x4 - new proxy creation for thread with the specified name.
      //    0x8 - get the list of dispatcher threads; get the list of slots for any thread.
      //    0x10 - get/set automatic subscription delivery parameters.
      //    0x20 - per-dispatcher_mt flag __msend_anlo "allow msend to pass by reference user objects and attachments, including non-local"
      //    0x40 - per-dispatcher_mt flag __mget_anlo "allow mget to return by reference user objects and attachments, including non-local"
      //    0x80 - allow request() with rt = shmqueue_in_conf.
      // Logic:
      //    b_get == true reads all the current flags into f.
      //    b_set == true sets all flags at once to values, specified in f.
      //    If both b_get and b_set are true, the internal flags variable and f are swapped.
      // Dflt. value of dispatcher_mt construction: 0x7f (all allowed).
      //
      // Concurrency: frqperm() may be called concurrently.
      //
      // Returns:
      //    true - success,
      //    false - no session.
      //
    bool frqperm(s_long& f, bool b_get = true, bool b_set = false) const __bmdx_noex;

  private:
    dispatcher_mt(const dispatcher_mt&); dispatcher_mt& operator=(const dispatcher_mt&);
    struct cch_session;  struct thread_proxy; struct mst_semaphore; friend struct yk_c::vecm;
    cref_t<cch_session> _r_ths;
  }; // struct dispatcher_mt

#endif // bmdx_part_dispatcher_mt

}

namespace yk_c
{
  template<> struct hashx_common::kf_basic<bmdx::unity> : bmdx::unity::kf_unity {};
  template<class Aux, class _bs> struct meta::construct_f<bmdx::unity, Aux, _bs> { s_long __fill; typedef bmdx::unity t; inline void f(t* p) const { new (p) t(); } };

  template<> struct vecm::spec<bmdx::unity::mod_handle> { typedef config_t<bmdx::unity::mod_handle, 1, 4, 1> config; };

  template<> struct bytes::type_index_t<bmdx::unity> : cmti_base_t<bmdx::unity, 2015, 6, 1, 12, -1> {};
  template<int i> struct bytes::type_index_t<hashx_common::entry<bmdx::unity, bmdx::unity, i> > : cmti_base_t<hashx_common::entry<bmdx::unity, bmdx::unity, i>, 2015, 6, 10, 17 + i, -1> {};
  template<> struct bytes::type_index_t<bmdx::_unitydate> : cmti_base_t<bmdx::_unitydate, 2017, 6, 20, 20, -1> {};
  template<> struct bytes::type_index_t<bmdx::unity::pcos> : cmti_base_t<bmdx::unity::pcos, 2019, 7, 24, 17, -1> {};
  namespace
  {
    bytes::type_index_t<bmdx::unity> __cmti_inst_unity;
    bytes::type_index_t<hashx<bmdx::unity, bmdx::unity>::entry> __cmti_inst_unity_h_entry;
    bytes::type_index_t<bmdx::_unitydate> __cmti_inst__unitydate;
    bytes::type_index_t<bmdx::unity::pcos> __cmti_inst_unity_pcos;
  }
}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif
