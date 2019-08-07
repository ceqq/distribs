// BMDX library 1.1 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2019-07-29
//
// Copyright 2004-2019 Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine)
// Contacts: bmdx-dev [at] mail [dot] ru, z7d9 [at] yahoo [dot] com
// Project website: hashx.dp.ua
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the Software or the use or other dealings in the Software.
// https://opensource.org/licenses/MIT

// bmdx_main.h, bmdx_main.cpp: polytype value, message dispatcher, utilities.

// Features:
//
//    Polytype value, for passing high-level data structures across binary modules.    (bmdx::  struct unity)
//
//    Simple date class.    (bmdx::  struct _unitydate)
//    Configuration file format with support of trees, + encoding/decoding utility.    (bmdx::  struct paramline)
//    Utilities for files and directories.    (bmdx::  struct file_utils)
//    Access to command line arguments array, executable name/path etc.    (bmdx::  cmd_myexe, cmd_string, cmd_array)
//    String character set conversions.    (bmdx::  wsToBs, bsToWs etc.)
//    String manipulation and other utility functions.    (bmdx::  split, trim, replace, array etc.)
//
//    Asynchronous message dispatcher.    (bmdx::  dispatcher_mt)

#ifndef bmdx_main_H
#define bmdx_main_H
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
  #pragma clang diagnostic ignored "-Wunused-function"
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
  #pragma GCC diagnostic ignored "-Wnonnull-compare"
  #pragma GCC diagnostic ignored "-Wunused-function"
  #pragma GCC diagnostic ignored "-Wdeprecated"
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
#endif


#include "bmdx_config.h"
#include "yk_cx.h"

#undef __BMDX_DLLEXPORT
#ifdef _bmdxpl_Wnds
  #define __BMDX_DLLEXPORT __declspec(dllexport)
#else
  #define __BMDX_DLLEXPORT
#endif



//==  struct unity CONSTRUCTION (brief)

  // unity() - constructs an empty object (utype() == utEmpty)
  // unity(<scalar value>) - constructs a scalar value; input value may be converted to one of the supported types
  // unity(std::wstring) - constructs a wrapped string that may be passed between incompatible binary modules
  // unity(std::string) - converts C string to wide string using the current locale
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
  // u_*, sc*, ua*, map*, hash*, [ ], path, u_name*
  //    -- common type, scalar, array, hashed map, hashlist manipulation, accessing values by key, index, path,
  //      additional object name field support.
  //
  // +<unity object>/<path>/<dflt. value and ret. type specifier> -- ternary operator to extract value
  //    from associative tree (consisting of hashes, maps, arrays) by the given path.


//==  Indexation summary.

  //  Historically, the library uses, in various functions referring to arrays of values,
  //    different values of base indexes.
  //
  //  1-based indexation:
  //    choose, mapi*, hashi*.
  //  1-based indexation on automatic array creation:
  //    rx, conv, ua*, array, decoded paramline arrays.
  //  0-based indexation:
  //    array0, hashl* (unoredered), cmd_array, split.
  //  Setting base index dependent on arguments:
  //    arr_init, arrlb_, i_dispatcher_mt::request.
  //  Indexation, dependent on individual array base index:
  //    [], pval, ref, rx, vstr, vcstr, vint*, vfp, ua*, map*, hashi*.


#include <vector>
#include <exception>
#include <typeinfo>

namespace bmdx
{
    // Implementation part.
  using namespace yk_c::experimental;
  using yk_c::meta;
  using yk_c::hashx;
  using yk_c::hashx_common;
  using yk_c::vecm;
  using yk_c::typer;
  using yk_c::bytes;
  using yk_c::s_long;
  typedef yk_c::meta::s_ll s_ll;
  using yk_c::__vecm_tu_selector;

  struct unity;
  struct unity_common
  {
    typedef meta::assert<meta::same_t<meta::s_long, bmdx_meta::s_long>::result>::t_true __check1;
    typedef meta::assert<meta::same_t<meta::s_ll, bmdx_meta::s_ll>::result>::t_true __check2;


    typedef void* (*__Psm)(s_long ind_method); // the function returning static function (class or interface method) pointer by given index
    typedef __Psm (*__Pipsm)(const char* interface_name); // the function returning static getter for getter of the given interface static methods

    static void _ls_psm_set(const char* piname, __Psm p) throw();

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
    const char* what() const throw() { return s.c_str(); }
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




    //==  Wrapper for static varibles.
    //    Ensures exact 1:1 correspondence of a variable to its binary module instance,
    //    independent of binding ruleset used by compiler.
    // Important differences from C++ language static varialbles:
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
    void _reg_stg() const throw() { if (!_pst_m) { typedef void* (*PFregstg)(const char* tname, void* pstg_offer); PFregstg f = (PFregstg)unity_common::pls_modsm()(unity_common::msm_cv_regstg); if (f) { _pst_m = (t_storage*)f(typeid(t_svf).name(), &_storage); } } }
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




    //==  String character set conversions.

    // NOTE When (ws*, bs*, replace)  functions are used, even indirectly, in multithreaded program,
    //  client-side setlocale and locale-dependent standard function calls
    //  must be protected with a lock:
    //    t_critsec_locale __lock(wsbs_lkp(), -1);
    //  (The above object blocks only if it's necessary, depending on the platform.)
  struct wsbs_lks {};
  typedef critsec_t<wsbs_lks> t_critsec_locale;
  s_long wsbs_lkp(); // lock sleep period: >= 1 (mcs) if lock is needed on this platform, -1 if not

  std::string wsToBs(const std::wstring& x); // UTF-16 string to OS local encoding char string
  std::string wsToBs(const wchar_t* x, meta::s_ll n = -1); // n < 0: autodetect length based on null char.
  std::string wsToBsOem(const std::wstring& x); // UTF-16 string to OEM char string - Windows-specific (can be used for text output to console)
  std::string wsToBsOem(const wchar_t* x, meta::s_ll n = -1);
  std::string wsToBsUtf8(const std::wstring& x); // UTF-16 string to UTF-8 string
  std::string wsToBsUtf8(const wchar_t* x, meta::s_ll n = -1);
  std::string wsToBsLsb(const std::wstring& x); // low byte of each wide char value to char
  std::string wsToBsLsb(const wchar_t* x, meta::s_ll n = -1);

  std::wstring bsToWs(const std::string& x); // OS local encoding char string to UTF-16 string
  std::wstring bsToWs(const char* x, meta::s_ll n = -1);
  std::wstring bsUtf8ToWs(const std::string& x); // UTF-8 string to UTF-16 string
  std::wstring bsUtf8ToWs(const char* x, meta::s_ll n = -1);
  std::wstring bsLsbToWs(const std::string& x); // char. values to wide char values 0..255
  std::wstring bsLsbToWs(const char* x, meta::s_ll n = -1);




  //==  Custom interafcing to a class (o_*).

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
  //      decalre o_proxy specialization able to call static versions of all I functions.
  //      After that, unity::pinterface automatically creates proxy object if necessary.

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

    // NOTE Default o_iimpl assumes that the target object itself inherits from MyI, and is its own implementation.



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
    // // Anywere else (does not have to know about o_interfaces_top<T>):
    //
    // unity x; T* p = new T();
    //  try { x.set_obj_atop<T>(*p, 1); } catch(...) { delete p; throw; } // wrap *p into x.
    //
    // // Anywere else (does not have to know about type T):
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
    //  2. In the list of interfaces (o_interfaces<>), any argument can be set to int, to quickly diasable entry.
    //
    //  3. It is poissible to attach interfaces without knowing the real object class.
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
    void __set_pp(unity_common::__Psm psm, I* pi) throw () { __psm = psm; __pi = pi; }
  };
    // NOTE o_iptr_t is only for in-module use. Its structure for different I may be different.
    //  Pass the original unity object between binary modules if necessary.
  template<class I, class _ = __vecm_tu_selector> struct o_iptr_t
  {
    typedef I __I; typedef o_proxy<__I, _> __Proxy;
    struct exc_null_ptr {};



    __I* operator->() const        throw (exc_null_ptr) { return _p_checked(); }
    __I& operator*() const        throw (exc_null_ptr) { return *_p_checked(); }

      // true if o_iptr_t object is non-null, and its referenced object is local.
    bool b_local() const { return !!_p; }

      // true if o_iptr_t object is non-null, and its referenced object is from other binary module.
    bool b_nonlocal() const { return _f && prx().__pci(); }

      // Conversion to client-side pointer.
    operator const void*() const { return _p_u(); }

      // Client-side pointer for calling interface functions.
      //  Checked version: ptr().
      //  Unchecked version: ptr_u().
    __I* ptr()        throw(exc_null_ptr) { return _p_checked(); }
    const __I* ptr() const        throw(exc_null_ptr) { return _p_checked(); }
    __I* ptr_u()        throw() { return _p_u(); }
    const __I* ptr_u() const        throw() { return _p_u(); }

      // In case of local interface, returns local pointer.
      // In case of proxy, returns server-side interface pointer (same as source unity::objPtr()).
      //  NOTE server-side interface pointer may be unsafe to use directly (due to binary incompatibility).
    __I* psi_u()        throw () { return _psi_u(); }
    const __I* psi_u() const        throw () { return _psi_u(); }



      // NOTE In case of proxy, client-side interface pointers are compared.
    bool operator == (const o_iptr_t& x) const        throw () { return _p_u() == x._p_u(); }
    bool operator != (const o_iptr_t& x) const        throw () { return _p_u() != x._p_u(); }
    bool operator == (const __I* x) const        throw () { return _p_u() == x; }
    bool operator != (const __I* x) const        throw () { return _p_u() != x; }



    void clear()        throw () { this->~o_iptr_t(); new (this) o_iptr_t(); }

    o_iptr_t()        throw () : _p(0), _f(0) {}
    ~o_iptr_t()        throw () { _p = 0; if (_f) { try { prx().~__Proxy(); } catch (...) {} _f = 0; } }

      // NOTE Construction does not generate exceptions.
      //    If no proxy defined by the client, or the client-defined __Proxy() fails,
      //    o_iptr_t will be null (== false).
    o_iptr_t(__I* pi, unity_common::__Psm pmsm)        throw () : _p(0), _f(0)
    {
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
    o_iptr_t(const o_iptr_t& x)         throw () : _p(x._p), _f(0) { if (x.b_nonlocal()) { try { new (&prx()) __Proxy(x.prx()); _f = 1; } catch (...) {} } }

      // NOTE Assignment does not generate exceptions.
      //    If assignment fails, ptr_u() == 0.
      //    If assignment succeeds, bool(ptr_u()) == bool(x.ptr_u()).
      //    If the client-defined __Proxy copy constructor does not fail anyway, assignment always succeeds as well.
    o_iptr_t& operator=(const o_iptr_t& x)        throw () { this->~o_iptr_t(); new (this) o_iptr_t(x); return *this; }



  private: __I* _p; char _sprx[sizeof(__Proxy)]; s_long _f;
    inline __I* _p_checked() const { if (_p) { return _p; } else if (_f && prx().__pci()) { return prx().__pci(); } throw exc_null_ptr(); }
    inline __I* _p_u() const { if (_p) { return _p; } else if (_f) { return prx().__pci(); } return 0; }
    inline __I* _psi_u() const { if (_p) { return _p; } else if (_f) { return prx().__psi(); } return 0; }
    __Proxy& prx() const { return *(__Proxy*)&_sprx[0]; }
  };
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
    virtual ~o_ibind() {}

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
    // NOTE For given MyI, several variations of o_iimpl are possible.
    //    Declare o_iimpl<T, IAlt1>, o_iimpl<T, IAlt2> etc., each of them inherits
    //    from an o_ibind<T, MyI> and implments MyI differently.
    //    IAlt* are formal arguments, to be listed in the set of interfaces (o_interfaces),
    //    attached to target object.
  template<class T, class MyI, class _ = __vecm_tu_selector>
  struct o_iimpl : o_ibind<T, MyI> //, MyI
  {
      // Default implementation assumes that the target object itself inherits from MyI, and is its own implementation.
      //    Customized o_iimpl will inherit from MyI and implement it differently.
    //virtual MyI* __pMyI() { return this; }
    virtual MyI* __pMyI()
    {
      MyI* p = this->__pobj(0x25); if (p) { return p; } // return object itself if it's from the current binary module and has the correct static type or vecm type index (see type_index_t in vecm_hashx.h)
      p = this->__pobj(0x44); return p; // return object itself if it's from other binary module and has the correct vecm type index
    }
    virtual void __itf_after_construct(s_long flags, o_itfset_base* src) {}
    virtual void __itf_before_destroy(s_long flags) {}
  };



    // Implementation part.
  template<class T, class Iformal, class Iimpl, class _ = __vecm_tu_selector>
  struct _o_iimpl_f_tu_t
  {
    static o_ibind<T, Iimpl>* Fnew() throw() { o_ibind<T, Iimpl>* p = 0; try { p = new o_iimpl<T, Iformal, _>(); } catch (...) {} return p;  }
    static o_ibind<T, Iimpl>* Fnewcp(o_ibind<T, Iimpl>* p0) throw() { o_ibind<T, Iimpl>* p = 0; try { if (p0) { p = new o_iimpl<T, Iformal, _>(static_cast<o_iimpl<T, Iformal, _>&>(*p0)); } } catch (...) {} return p;  }
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
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() { try { __p_iimpl->__itf_after_construct(flags, src); ++nsucc; } catch (...) { return false; } return __NextIImpl::__xx_after_construct(flags, src, nsucc); }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() { ++nsucc; bool b = nmax < 0 || nsucc <= nmax; if (nmax < 0 || nsucc < nmax) { __NextIImpl::__xx_before_destroy(flags, nsucc, nmax); } if (!b) { return; } try { __p_iimpl->__itf_before_destroy(flags); } catch (...) {} }
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
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() { ++nsucc; return __NextIImpl::__xx_after_copy(flags, src, nsucc); }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() { ++nsucc; if (nmax < 0 || nsucc < nmax) { __NextIImpl::__xx_before_destroy(flags, nsucc, nmax); } }
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
    bool __xx_after_construct(s_long flags, o_itfset_base* src, s_long& nsucc, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() { return true; }
    void __xx_before_destroy(s_long flags, s_long& nsucc, s_long nmax, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw() {}

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



    // This is the default (empty) set of interfaces, attached to object T by unity::set_obj_atop.
    //    If it's specialized for a class (specialization should inherit from particular o_interfaces<T, I1, I2...> class),
    //    any object, assigned with set_obj_atop, supports all interfaces, listed in that specialization.
  template<class T, class _ = __vecm_tu_selector>
  struct o_interfaces_top
  : public o_interfaces<T>{};

}

namespace yk_c
{
  template<> struct vecm::spec<bmdx::unity> { struct aux : vecm::config_aux<bmdx::unity> { }; typedef config_t<bmdx::unity, 1, 4, 1, aux> config; };
  template<class T, class L> struct vecm::spec<bmdx::cref_t<T, L> > { typedef bmdx::cref_t<T, L> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
}




namespace bmdx
{




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
    template<class T, class _ = __vecm_tu_selector> struct checked_ptr
    {
      T* p;

      inline checked_ptr(const T* p_)        throw() : p((T*)p_) {}
      inline T& operator*() const        throw(XUExec) { check(); return *p; }
      inline operator T*() const        throw() { return p; }
      inline operator bool() const        throw() { return !!p; }
      inline T* operator->() const        throw(XUExec) { check(); return p; }
      inline bool operator==(const checked_ptr& p2) const        throw() { return p == p2.p; }
      inline bool operator==(const T* p2) const        throw() { return p == p2; }
      inline bool operator!=(const checked_ptr& p2) const        throw() { return p == p2.p; }
      inline bool operator!=(const T* p2) const        throw() { return p != p2; }

        // Same as in o_iptr_t.
      inline const T* ptr() const        throw(XUExec) { check(); return p; }
      inline T* ptr()        throw(XUExec) { check(); return p; }
      inline const T* ptr_u() const        throw() { return p; }
      inline T* ptr_u()        throw() { return p; }

      inline void check() const        throw(XUExec) { if (!p) {        throw XUExec("checked_ptr.p == 0: ", typeid(T*).name()); } }
    };

      // Hash and map key functions.
    struct kf_unity
    {
      kf_unity(s_long flags_ = 0); // flags: see static const s_long fkcmp*
      s_long flags() const;
      void _set_flags(s_long x) const;
      s_long hash(const unity& x) const;
      bool is_eq(const unity& x1, const unity& x2) const;
      bool less12(const unity& x1, const unity& x2) const; // x1 < x2
      bool less21(const unity& x1, const unity& x2) const; // again x1 < x2

      template<class K2> inline void cnew(unity* p, const K2& x, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { new (p) unity(x); if ((_flags & fkcmpScalarize) && !p->isScalar()) { try { *p = p->u_key(); } catch (...) { p->clear(); throw; } } }
      template<class K2> inline s_long hash(const K2& x, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return this->hash(unity(x)); }
      template<class K2> inline bool is_eq(const unity& x1, const K2& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return x1.k_eq(unity(x2), _flags); }
      template<class K2> inline bool less12(const unity& x1, const K2& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return x1.k_less(unity(x2), _flags); }
      template<class K2> inline bool less21(const K2& x1, const unity& x2, meta::noarg_tu_t<K2> = meta::noarg_tu_t<K2>()) const { return unity(x2).k_less(x1, _flags); }

      private: mutable s_long _flags;
    };

    struct _hl_i;
    typedef yk_c::experimental::ordhs_t<unity, unity, kf_unity, kf_unity> t_map;
    typedef yk_c::experimental::ordhs_t<unity, unity, kf_unity, kf_unity, __vecm_tu_selector> t_map_tu;
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

      template<class _> struct trivconv_t<wchar_t*, _> { typedef std::wstring t_target; static inline t_target F(const wchar_t* x) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<wchar_t[], _> { typedef std::wstring t_target; static inline t_target F(const wchar_t x[]) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };

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
          template<class Td, class Ts, class _> struct _x_vecadap<vec2_t<Td>, std::vector<Ts>, _> { static bool copy(vec2_t<Td>& dest, const std::vector<Ts>& src) { return _utils::copy(dest, src); } };
          template<class Td, class Ts, class _> struct _x_vecadap<vec2_t<Td>, vec2_t<Ts>, _> { static bool copy(vec2_t<Td>& dest, const vec2_t<Ts>& src) { return _utils::copy(dest, src); } };
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
    inline s_long utype() const throw () { return ut & xfmUtype; }

      // EUnityType without utArray flag
    inline s_long utype_noarray() const throw () { return ut & utMaskNoArray; }

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
      //    scalar, array, object, assoc. array (map or hash)
      //  covers all available types in a valid object.
      //  Empty is a scalar. Bool is represented by 0/non-0 char.
      // NOTE Tests are able to return false on this == 0.
    inline bool isScalar() const throw () { return this && utype() <= utString; }
    inline bool isArray() const throw () { return this && !!(ut & utArray); }
    inline bool isObject() const throw () { return this && utype() == utObject; }
    bool isAssoc() const throw (); // true if unity is utHash or utMap

      // Tests for particular types and values.
      // NOTE Tests are able to return false on this == 0.
    inline bool isEmpty() const throw () { return this && utype() == utEmpty; }
    inline bool isInt() const throw () { return this && utype() == utInt; }
    inline bool isFloat() const throw () { return this && utype() == utFloat; }
    inline bool isDate() const throw () { return this && utype() == utDate; }
    inline bool isString() const throw () { return this && utype() == utString; }
    inline bool isChar() const throw () { return this && utype() == utChar; }

    inline bool isMap() const throw () { return this && utype() == utMap; }
    inline bool isHash() const throw () { return this && utype() == utHash; }

    inline bool isBool() const throw () { return this && utype() == utChar; }
    bool isBoolTrue() const throw (); // true if *this contains char != 0
    bool isBoolFalse() const throw (); // true if *this contains char == 0

    inline bool isIntOrFloat() const throw () { return this && utype() == utInt && utype() == utFloat; }

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

      // Flags for uniqueness and relation of map and hash keys
    static const s_long fkcmpNFloat = 0x2; // convert integer keys (utInt, utChar) to double
    static const s_long fkcmpSNocase = 0x1; // strings comparison and hashing are case insensitive (0 == binary comp.)
    static const s_long fkcmpRevOrder = 0x4; // (map only) reverse order in "<" comparison (hash only)
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

      // Underlying map, hash access. Objects are implementation-dependent!
    inline t_map* _pmap() const { typedef t_map& R; return isMap() ? &R(*_m()) : 0; }
    t_hash* _phash() const { return isHash() ? _h() : 0; }

      // Pointer to function, returning pointers to private static methods (of BMDX) in the binary module
      //    that has created the current object. Value same as _pmsm() is returned by  unity_common::pls_modsm()
      //    of that binary module (main executable or shared library instance).
    unity_common::f_ls_modsm _pmsm() const throw() { return pmsm; }


      // Construction.

    unity();
    unity(const unity& x);
    unity(const std::wstring& x);
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
    ~unity() throw();



      // Clear / assign.
      //  NOTE Ops. in this group do not preserve the current object name.
      //    Assigning dest = src will copy both the name and value of src.
      //    Others just remove the current name.
      //    See also u_assign, u_clear.

      // Clears *this and sets it to utEmpty.
    void clear() throw();

      // Swaps *this and x on the binary level.
      // NOTE If *this and/or x are dynamically constructed, they must belong to the same binary module,
      //    otherwise causes undefined behavior on later object deletion.
      //    For other types of storage allocation, result of swap() is always stable.
    void swap(unity& x) throw();

      // Correctly deleting the current object from within binary module where it was dynamically constructed.
      //    true - the object is deleted successfully.
      //    false - the object deleted, but generated an exception during op. delete.
      // NOTE unity_delete() is equivalent to operator delete, executed in the original module on *this.
      //    1. Do not use it if the object was created in other way than dynamic construction (new unity(...)).
      //    2. Do not use it after the object was recreated (see recreate(), recreate_as()).
    bool unity_delete() throw();

      // Destroys previous contents and initializes the storage
      //    as an empty value created in the current binary module.
      // NOTE If unity is dynamically constructed (new unity(...)) in binary module A, and "recreated" in module B,
      //    it still must be deleted in module A in usual way ("delete p"). Do not use unity_delete().
    unity& recreate() throw();

      // Destroys previous contents and initializes the storage
      //    as an empty value created in the binary module of modsrc.
      // See also recreate().
    unity& recreate_as(const unity& modsrc) throw();

      // NOTE all assignments are transactional (on failure, the target object is not modified).
      // NOTE if src is utObject, operator=(const unity& src) copies a reference to that object, not its value.
      // NOTE operator=(const unity& src) copies src object name.
      //    operator=(any other type) clears target object name.
      //  Use combinations of u_clear, u_assign, u_rx, u_conv, ua_assign to keep the name.
      // NOTE Assignment never changes the target's native module association (see _pmsm()).
    unity& operator=(const unity& src);
    unity& operator=(const std::wstring&);
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
      //              (see type_index_t in vecm_hashx.h for details).
      //              NOTE Several types have cross-module indexes pre-declared:
      //                  char, double, s_long, meta::s_ll -- in vecm_hashx.h.
      //                  _unitydate, unity -- below in bmdx_main.h.
      //                  threadctl, threadctl::tid -- in bmdx_config.h and bmdx_cpiomt.h.
      //
      //  If not (1..4), an exception is generated:
      //    XUTypeMismatch if utype() is not the same as given type.
      //    XUExec a) ind is out of range, b) failed to return refrence to contained object.
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
      //      c) crefx<utUnity>(ind)) only: for any scalar type - dynamically created unity(ref(ind)) .
      //  MODE This function does not modify the object.
      //    cref() family is analogous to val(), but it does not make any copies when the contained object
      //    may be accessed directly by reference.
      //  NOTE If the object is from other binary module, and it contains incompatible string,
      //    the returned cref_t will reference a copy of that string, created in the current module.
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


    // The below v* functions are shorter version of val() + certain additional operations,
    //    for each particular type.

      // >= 0 -- length of the contained string.
      //  -1 -- the contained value is empty (utEmpty).
      //  -2 -- the value is neither a string nor an empty.
      //  -3 -- (only if) this == 0 or the object is broken.
    meta::s_ll lenstr() const throw();

      // Wide string representation of a value.
      //  NOTE vstr() values returned may be formatted differently than val<utString>().
    std::wstring vstr() const;
      std::wstring vstr(s_long ind) const;

      // C-string representation of a value.
    std::string vcstr() const;
    std::string vcstr(s_long ind) const;

    meta::s_ll vint() const;
    meta::s_ll vint(s_long ind) const;

      // High doubleword.
    s_long vint_h() const;
    s_long vint_h(s_long ind) const;

      // Low doubleword.
    s_long vint_l() const;
    s_long vint_l(s_long ind) const;

      // Floating-point value.
    double vfp() const;
    double vfp(s_long ind) const;

      // Short string representation of a value.
    _fls75 vflstr() const throw();





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
      //  If *this does not contain an array, XUTypeMismatch is genereated.
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


    //-- Hashed map

    unity& map_ensure(); // returns *this
    unity& map_clear(bool full); // true resets flags and removes elements (object name is kept), false only removes elements; returns *this
    unity map_keys(s_long lb);
    unity& map_keys_get(unity& dest, s_long lb);  // returns *this
    unity map_values(s_long lb);
    unity& map_values_get(unity& dest, s_long lb);  // returns *this
    s_long mapFlags();
    void mapFlags_set(s_long fk_reset, s_long fk_set);

    bool map_empty(const unity& k) const;
    bool map_locate(const unity& k, bool insert = false);
    s_long map_noel() const { return hashx_common::no_elem; }
    bool map_locate(const unity& k, bool insert, s_long& ind); // ind == map_noel() if not found on insert == false
    unity& map(const unity& k); // insert { k, utEmpty } if necessary, return ref. to value with key k
      // Returns: true if k was inserted, false if it existed.
    bool map_append(const unity& k, const unity& v, bool keep_first = false);
    bool map_del(const unity& k); // true - removed, false - not existed

    s_long mapS();
    unity& mapS_set(s_long new_S); // returns *this

      // In mapi*, ind is 1-based. Valid inds are [1..mapS()].
      //  NOTE Key, value functions genereate an exception if ind is out of range.
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
      //  NOTE Key, value functions genereate an exception if ind is out of range.
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



    //-- map, hash, array elem. access, [ ], path

      // u_has returns true if the inner value is the container of one of the specified types
      //    (tt is a combination of flags: 1 <=> utUnityArray, 2 <=> utMap, 4 <=> utHash, 8: allow forwarding to the contained unity object.)
    bool u_has(const unity& ki, s_long tt) const;
      // operator[] returns, if found type/key/index match, one of the following:
      //    a) ref. to ki-th array element,
      //    b) map(ki),
      //    c) hash(ki),
      //    d) contained unity object [ki].
      //  Otherwise, XUExec is generated.
    unity& operator[] (const unity& ki);
    const unity& operator[] (const unity& ki) const;

      // Keys in the list should be in paramline array format (first "|" or "=|" is optional).
      //  mha is arbitrary tree of maps, hashes, arrays.
      //  NOTE forced == true: if key (keylist element) is a non-numeric key, but, in this position
      //  in the tree, array already exists, the function returns 0 (array is not converted to hash or map).
      //  In other cases, for numeric key, an array is created containing that index,
      //  for non-numeric key -  a hash with appropriate entry is autocreated
      //  (if a scalar exists in this place, it will be overwritten).
    const unity* path(const std::wstring& keylist) const throw();
    const unity* path(const wchar_t* keylist) const throw();
    const unity* path(const std::string& keylist) const throw();
    const unity* path(const char* keylist) const throw();
    const unity* path(const unity& keylist) const throw();

    const unity* path(const std::wstring& keylist, const unity& x_dflt) const throw();
    const unity* path(const wchar_t* keylist, const unity& x_dflt) const throw();
    const unity* path(const std::string& keylist, const unity& x_dflt) const throw();
    const unity* path(const char* keylist, const unity& x_dflt) const throw();
    const unity* path(const unity& keylist, const unity& x_dflt) const throw();

      // path_w returns 0 only on failure. If the specified branch or key is missing,
      //  it's inserted.
    checked_ptr<unity> path_w(const std::wstring& keylist)        throw() { return _path_w(keylist); }
    checked_ptr<unity> path_w(const wchar_t* keylist)        throw() { return _path_w(keylist); }
    checked_ptr<unity> path_w(const std::string& keylist)        throw() { return _path_w(keylist); }
    checked_ptr<unity> path_w(const char* keylist)        throw() { return _path_w(keylist); }
    checked_ptr<unity> path_w(const unity& keylist)        throw() { return _path_w(keylist); }



    //-- Common associative array functions (for map and hash)

      // assoc* functions work both with maps and with hashes,
      //    without changing the container type. The container must be map or hash.
      //    For maps, list of key/value pairs is iterated in natural sort order,
      //    numeric positions in assocl_* and index values in mapi_* are the same.
    bool assoc_set(const unity& k, const unity& v, bool keep_first = false);
    bool assoc_del(const unity& k);

    s_long assocS_c() const;

      // NOTE assocl_noel() is returned by assocl_ first, last, next, prev if *this is not map or hash.
    s_long assocl_noel() const { return hashx_common::no_elem; }
    s_long assocl_first() const;
    s_long assocl_last() const;
    s_long assocl_next(s_long pos) const;
    s_long assocl_prev(s_long pos) const;

    const unity& assocl_key(s_long pos) const;
    unity& assocl(s_long pos);
    const unity& assocl_c(s_long pos) const;



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
      unity& x; bool ne; s_long lkt;
      objt_binder_t(unity& x_, bool ne_, s_long lkt_) : x(x_), ne(ne_), lkt(lkt_) {}
      bool operator()() { T* p(0); try { p = new T(); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(0)"); } return false; }
      template<class A1> bool operator()(const A1& x1)        { T* p(0); try { p = new T(x1); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(1)"); } return false; }
      template<class A1, class A2> bool operator()(const A1& x1, const A2& x2)        { T* p(0); try { p = new T(x1, x2); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(2)"); } return false; }
      template<class A1, class A2, class A3> bool operator()(const A1& x1, const A2& x2, const A3& x3)        { T* p(0); try { p = new T(x1, x2, x3); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(3)"); } return false; }
      template<class A1, class A2, class A3, class A4> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { T* p(0); try { p = new T(x1, x2, x3, x4); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(4)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(5)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(6)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6, x7); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(7)"); } return false; }
      template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool operator()(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)        { T* p(0); try { p = new T(x1, x2, x3, x4, x5, x6, x7, x8); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(8)"); } return false; }

            // Same as operator(), but args. are cast to non-const when passed to constructor.
          template<class A1> bool createnc(const A1& x1)        { T* p(0); try { p = new T((A1&)x1); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(1)"); } return false; }
          template<class A1, class A2> bool createnc(const A1& x1, const A2& x2)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(2)"); } return false; }
          template<class A1, class A2, class A3> bool createnc(const A1& x1, const A2& x2, const A3& x3)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(3)"); } return false; }
          template<class A1, class A2, class A3, class A4> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(4)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(5)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(6)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(7)"); } return false; }
          template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> bool createnc(const A1& x1, const A2& x2, const A3& x3, const A4& x4, const A5& x5, const A6& x6, const A7& x7, const A8& x8)        { T* p(0); try { p = new T((A1&)x1, (A2&)x2, (A3&)x3, (A4&)x4, (A5&)x5, (A6&)x6, (A7&)x7, (A8&)x8); x.set_obj(*p, true, lkt); return true; } catch (...) {} if (p) { try { delete p; } catch (...) {} } if (!ne) { throw XUExec("objt_binder_t()(8)"); } return false; }
    };

      // Object creation with 0..6 arguments. The object will be strong-referenced.
      //    Example:         unity z; z.objt<std::string>(0)(' ', 25);
      //    On success, the previous unity contents are correctly removed, and the new object is set on their place.
      //    On failure, the previous contents are not changed.
      // lkt: ref. counting lock type:
      //    0 - do not use critical sections for object ref. counting,
      //    1 (default) - lock ref. counting one lock per type (Obj), actual in all translation units of the current binary module,
      //    2 - lock ref. counting with individual lock associated with the target object only (automatically kept together with the wrapped object).
      // See set_obj for detailed info on how locks type 1 and 2 work.
    template<class T> objt_binder_t<T>
      objt(bool no_exc, s_long lkt = 1, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) throw()
          { return objt_binder_t<T>(*this, no_exc, lkt); }

      // Returns object pointer !=0 if unity references a valid (not deleted) object.
    void* cpvoid() const;

      // Returns object pointer !=0 if unity references an object or still contains a pointer to the deleted object.
    void* cpvoidkey() const;

      // Returns object pointer !=0:
      //    a) b_checked == true: if unity references a valid (not deleted) object of the same static type as Obj.
      //    b) b_checked == false: same as cpvoidkey() cast to Obj* (if unity references any object
      //      or still contains a pointer to the deleted object).
    template<class Obj> Obj*
      objPtr(bool b_checked = true, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>())
          { return o_api(this).pobj<Obj>(b_checked ? 0x21 : 0x78); }
    template<class Obj> const Obj*
      objPtr_c(bool b_checked = true, meta::noarg_tu_t<Obj> = meta::noarg_tu_t<Obj>()) const
          { return o_api(this).pobj<Obj>(b_checked ? 0x21 : 0x78); }

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
    o_type_info objTypeInfo(s_long flags) const throw();

    o_ref_info objRefInfo() const throw();

      // Returns true on success (ref. type set, or not changed if new == old), false on failure.
    bool objSetStrongRef(bool b_strong);

      // Creates a set of interfaces (new _o_itfs_topmost<Itfs>()) and attaches it to object (adds to interface sets list of the wrapping unity object).
      //  All interfaces get __itf_after_construct notification.
      // Returns:
      //    1 - the set is added to the existing list of sets.
      //    0 - the set of such static type already exists.
      //    -1 - pidyn == 0 (list of interf. sets is not created - call itfslist_create() first).
      //    -2 - failure.
      // NOTE If Itfs is a template with one of arguments belonging to anonimous namespace,
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

      o_api() throw();
      o_api(const unity* pu_) throw(); // NOTE "const" is ignored
      o_api(_o_itfslist& list) throw();
      o_api(void* p, bool b_pidyn) throw();

      void* p_head() const throw();

      o_type_info type_info(s_long flags) const throw();
      o_ref_info ref_info() const throw();
      void* pvoid(bool b_checked) const throw();
      void* prefo(const char* tname, s_long t_ind, s_long t_size) const throw();
      s_long rc_incdec(s_long flags) const throw();
      s_long upd_ut_ref_type(bool b_strong) const throw();
      s_long itfslist_create() throw();
      s_long itfslist_n() const throw();
      s_long itfs_append_cp(const o_api& src) const throw();
      s_long itfs_remove_ind(s_long ind) const throw();
      s_long itfs_remove_all() const throw();
      s_long find_by_pitfs(o_itfset_base* p) const throw();
      #if defined(__GNUC__) || defined(__clang__)
        template<class T> T* pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const throw();
        template<class I> o_iptr_t<I> pinterface(s_long flags, const o_itfset_base* ps0 = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw();
        template<class I> o_iptr_t<I> pinterface_at(s_long ind, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw();
        template<class Itfs> s_long itfs_attach(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const throw();
        template<class Itfs> s_long find_local_by_itfs(s_long flags, s_long ind0 = 0, s_long ind2 = -1, meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const throw();
        template<class I> s_long find_by_itf(s_long flags, o_itfset_base* ps0 = 0, std::basic_string<s_long>* pret = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw();
      #else
        template<class T> T* pobj(s_long flags, meta::noarg_tu_t<T>) const throw();
        template<class I> o_iptr_t<I> pinterface(s_long flags, const o_itfset_base* ps0, meta::noarg_tu_t<I>) const throw();
        template<class I> o_iptr_t<I> pinterface_at(s_long ind, meta::noarg_tu_t<I>) const throw();
        template<class Itfs> s_long itfs_attach(meta::noarg_tu_t<Itfs>) const throw();
        template<class Itfs> s_long find_local_by_itfs(s_long flags, s_long ind0, s_long ind2, meta::noarg_tu_t<Itfs>) const throw();
        template<class I> s_long find_by_itf(s_long flags, o_itfset_base* ps0, std::basic_string<s_long>* pret, meta::noarg_tu_t<I>) const throw();
      #endif

    private:
      o_api _ref_newcp(unity_common::__Psm pmsm_itfslist, s_long flags) const throw();
      s_long _ref_del(s_long flags) throw();
      meta::s_ll _itfslist_nrsv(meta::s_ll n) const throw();
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
      static void* sm(s_long ind) throw();
    };

    template<class Itfs, class _> struct _o_itfssm_t
    {
      static o_itfset_base* itfs_new() throw();
      static o_itfset_base* itfs_new_copy(o_itfset_base* x) throw();
      static void itfs_delete(o_itfset_base* x) throw();
      static void itfs_type_info(o_itfset_base* x, o_type_info* pret, s_long flags) throw();
      static s_long __xx_after_construct(o_itfset_base* x, o_itfset_base* src, s_long flags) throw();
      static void __xx_before_destroy(o_itfset_base* x, s_long flags) throw();
      static void* sm(s_long ind) throw();
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
      mod_handle() throw();
      ~mod_handle() throw();
      mod_handle(const mod_handle&) throw(); // normally, produces a copy of the original handle; does not fail (no allocations)
      void operator=(const mod_handle&) throw(); // normally, produces a copy of the original handle; does not fail (no allocations)
      void swap(mod_handle&) throw();
      void clear() throw();

      #ifdef _bmdxpl_Wnds
        typedef HINSTANCE t_nativehandle;
      #endif
      #ifdef _bmdxpl_Psx
        typedef void* t_nativehandle;
      #endif

      bool is_valid() const throw(); // true if this handle is valid
      operator bool() const throw(); // == is_valid()
      bool is_mainexe() const throw(); // true if this handle is valid and references the main executable
      t_nativehandle native() const throw(); // native shared library or main executable handle (platform-dependent)
      bool operator==(const mod_handle& x) const throw();
      bool operator!=(const mod_handle& x) const throw();

        // Pointer to procedure or variable by name (GetProcAddress, dlsym).
      void* sym(const char* name) const throw();

        // Call into shared module (.dll, .so) predefined function:
        //    s_long bmdx_mod_request(s_long sig_struct, s_long action, const unity* ppara, unity* pretval);
        //    with action == 1,
        //    sig_struct == unity::sig_struct() of the current binary module,
        //    ppara == &para,
        //    pretval == &retval.
        // para: input parameters of the request.
        // retval: value, returned by the reuest.
        //    Input and output contents are determined by the client and requested module relationships.
        //  flags: 0x1 - ignore native handle, search for name in the main executable.
        // The request is done under global lock, which may block up to 60 s,
        //  after which the blocked thread cancels the request.
        //  (In multithreaded program, requests should take small time, <= milliseconds.)
        // NOTE Defining bmdx_mod_request in the shared library is optional.
        //    The client may use any other functions and means on communication between executable
        //    and library, or between libraries. For example, via BMDX:
        //    a) global memory (shmobj2s_t), b) process' common objects storage (unity::pcos).
        // Returns:
        //  1 - success.
        //  -1 - the requested module is not compatible.
        //  -2 - client-side failure or lock timeout.
        //  -3 - failed to get address of bmdx_mod_request function (see below).
        //  -4 - the handle is not valid.
        //  -5 - module-side exception.
        //  <= -10 - module-side error.
      s_long request(const unity& para, unity& retval) const throw();

        // Pointer to function, returning pointers to private static methods (of BMDX) in the binary module
        //    that is held by the current handle. Value same as _pmsm() is returned by  unity_common::pls_modsm()
        //    of that binary module (main executable or shared library instance).
        // NOTE _pmsm() may return 0:
        //    a) if the handle holds a library that is compiled without BMDX at all or maybe with bmdx_part_dllmgmt 0.
        //    b) same as (a) if the handle addresses the main executable - this may occur in shared library,
        //      when it uses bmdx_part_dllmgmt 1, but main executable does not.
        //    c) if the handle is null.
      unity_common::f_ls_modsm _pmsm() const throw();

        // Same as _pmsm, but for root loader of the shared library (the topmost loaded module with BMDX,
        //    that has loaded directly or indirectly all other such modules).
      unity_common::f_ls_modsm _pmsm_rl() const throw();

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
      static mod_handle hself(bool b_autounload) throw();

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
      rqcheck(s_long sig, s_long action, const unity* ppara, const unity* pretval, s_long timeout_ms = -1 __bmdx_noarg) throw()
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
        // NOTE mod(), mod_handle::request(), and other functions
        //    may be safely called from multiple threads.
        // NOTE The client must ensure that no high-level cross-module objects
        //    left in memory when the module is unloaded.
    #if bmdx_part_dllmgmt
      static mod_handle mod(const char* name, bool b_autounload, s_long flags = bmdx_mod_load_def_flags) throw();
    #else
        // With bmdx_part_dllmgmt 0, mod() returns a null handle.
      static mod_handle mod(const char* name, bool b_autounload, s_long flags = bmdx_mod_load_def_flags) throw() { return mod_handle(); }
    #endif //bmdx_part_dllmgmt


        // unity object structure signature for binary comatibility check.
    static s_long sig_struct() throw ();

      // Returns:
      //  2 - the current object is created in the current module.
      //  1 - the current object is compatible with the current module.
      //  0 - the current object may be safely copied to an object, created in the current module.
      //    For strings, 0 means that direct reference to string may not be got by the client.
      //      Still, strings may be copied and assigned.
      //    For wrapped objects, 0 means that the object has been created in other binary module.
      //  <0 - the current object is not compatible with the current module.
      //  Only clear() and destructor may be called safely.
    s_long compatibility() const throw ();


    //==  PROCESS' COMMON OBJECTS STORAGE (PCOS)
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
    //      If pcos references the storage in a shared library, the library is not unloded at least until pcos is destroyed,
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
    //      2. Wrapping into unity has the only purpose of keeping polymorhic values from multiple modules
    //          in the storage hash.
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
    //          For example, if a shared library does not own the storage (e.g. executable's storage),
    //              but wants to put its own value into the storage, it can use setcp or makecp+setref.
    //            By default, the value will be copied and belong to executable's storage.
    //              In this case, the library may be unloaded, but the value remains in the storage.
    //            If the value must be moved (setmv or makemv+setref), or if it is custom object (stored as unity/utObject),
    //              the library must supply mod_handle to itself (e.g. see mod_handle::hself()), otherwise the call fails,
    //              because ownership on object cannot be changed to other binary module
    //              because of object's code placement inside the library.
    //              In this case, the library cannot be unloaded until all such objects are removed from the storage.
    //          For 2nd example, main executable never needs to supply a handle to put its value therein,
    //            because default pcos itself already contains main executable's handle.
    //          More generally, if a module that wants to put into a storage (represented by pcos)
    //            a value that is from module other than that storage, value's module handle must be supplied,
    //            or a value must be already in the form cref_t<unity>, created with makemv, makecp,
    //            or taken from a storage with operator().

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

        // Creates a pcos object, referencing certain process' common object storage.
        //    By default, main executable is the module, servicing the common storage.
        //    Usually it has longer lifetime than any shared library instance.
        //    The client may pas a handle (h) to any loaded shared library,
        //    to address the storage of that library, or null handle to create non-functional pcos,
        //    which is not associated with any storage.
      pcos(unity::mod_handle h = unity::mod("", 1)) throw()        { if (!h) { return; } unity_common::f_ls_modsm pmsm = h._pmsm(); if (!pmsm) { return; } typedef f_pcos_cons tf; tf f = (tf)pmsm(em::msm_pcos_cons); if (!f) { return; } try { f(&_d, &h); } catch (...) {} }

        // Binary module handle of the current storage.
        // May be:
        //  a) by default, the main executable.
        //  b) a shared library, loaded into the process.
        //  c) null handle (if pcos constructor was given a null handle).
      unity::mod_handle hhost() const throw()        { t_d d = _d; return d ? d->hhost : unity::mod_handle(); }

        // True if this pcos addresses a normally functioning storage (in the main executable, or in a shared library).
      operator bool() const throw()        { return _d; }

        // True if this pcos addresses a normally functioning storage in the main executable.
        //  NOTE The common storage of the main executable is regarded the single storage
        //    that should be used by default by all clients.
        //    Usually it has longer lifetime than any shared library instance.
        //    Storages of shared library instances should be used for private purposes or if main executable
        //    does not support common storage.
      bool b_mainexe() const throw()        { t_d d = _d; return d && d->hhost.is_mainexe(); }

      // === === === === === === === ===

        // Get a reference to the value with key k, from the current storage.
        //    (The reference is not removed from the storage, so the value may be accessed again by any client.)
        // The result code and returned value:
        //    1 - the value exists. A ref. to valid object is returned.
        //  An empty cref_t is returned with the following result codes:
        //    0 - the value does not exist.
        //    -2 - op. failed.
        //    -3 - pcos object is non-functional.
      cref_t<unity> operator()(const unity& k, s_long* pres = 0) throw()        {  cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; }  typedef f_pcos_oppar tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_oppar); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &k, &rv); } catch (...) {}  if (pres) { *pres = res; } return rv; }

        // Set the new value into the current storage, or remove the existing one (setref only).
        //    xc, xm: the value is moved (xm) or copied (xc) into the storage.
        //      The value is associated (by handle) with binary module in which it's factually created.
        //      By default, this is binary module of the current storage (pcos::hhost()).
        //      Otherwise, it's hx (if specified).
        //      See the above note "Ownership on objects" for explanation when and which hx is neccessary to specify.
        //      The correctness of the association is checked in setmv, setcp.
        //      Relaxation for xc: unity object itself may be associated with any module,
        //        because it's automatically copied by setcp on the side, represented by handle.
        //      Anyway, if xm or xc contains a user object (unity/utObject),
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
      s_long setref(const unity& k, cref_t<unity> xr) throw()        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setref tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setref); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xr); } catch (...) {} return res; }
      s_long setmv(const unity& k, unity& xm, unity::mod_handle hx = unity::mod_handle()) throw()        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setmv tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setmv); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xm, &hx); } catch (...) {} return res; }
      s_long setcp(const unity& k, const unity& xc, unity::mod_handle hx = unity::mod_handle()) throw()        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_setcp tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_setcp); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &k, &xc, &hx); } catch (...) {} return res; }

        // Check if the given reference is correct for passing into setref,
        //    i.e. made with makemv, makecp, or returned by pcos operator().
        //    The check is identical to one that setref() does before modifying the storage.
        //  1 - xr is non-empty and fit for passing into setref().
        //  0 - xr is empty.
        //  -1 - bad xr structure, module or version: xr won't be accepted by setref().
        //  -2 - op. failed.
        //  -3 - pcos object is non-functional.
      s_long checkref(cref_t<unity> xr) throw()        { if (!xr) { return 0; } t_d d = _d; if (!d) { return -3; } typedef f_pcos_checkref tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_checkref); if (!f) { return -2; } s_long res = -2; try { res = f(&d, &xr); } catch (...) {} return res; }

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
      s_long clearstg() throw()        { t_d d = _d; if (!d) { return -3; } typedef f_pcos_clearstg tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { return -2; } tf f = (tf)pmsm(em::msm_pcos_clearstg); if (!f) { return -2; } s_long res = -2; try { res = f(&d); } catch (...) {} return res; }

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
        //      Anyway, if xm or xc contains a user object (unity/utObject),
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
      cref_t<unity> makemv(unity& xm, unity::mod_handle hx = unity::mod_handle(), s_long* pres = 0) throw()        { cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; } typedef f_pcos_makemv tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_makemv); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &xm, &hx, &rv); } catch (...) {} if (pres) { *pres = res; } return rv; }
      cref_t<unity> makecp(const unity& xc, unity::mod_handle hx = unity::mod_handle(), s_long* pres = 0) throw()        { cref_t<unity> rv; t_d d = _d; if (!d) { if (pres) { *pres = -3; } return rv; } typedef f_pcos_makecp tf; t_msm pmsm = d->hhost._pmsm(); if (!pmsm) { if (pres) { *pres = -2; } return rv; } tf f = (tf)pmsm(em::msm_pcos_makecp); if (!f) { if (pres) { *pres = -2; } return rv; } s_long res = -3; try { res = f(&d, &xc, &hx, &rv); } catch (...) {} if (pres) { *pres = res; } return rv; }
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

    inline t_map_tu* _m() const throw () { return reinterpret_cast<t_map_tu*>(_data.p1); }
    inline t_hash* _h() const throw () { return reinterpret_cast<t_hash*>(_data.p1); }
    inline s_long _uttf() const throw () { return ut & xfmUtypeAndFlags; }
    static _fls75 _tname0(s_long utt) throw ();
    static s_long _def_size(s_long utt) throw();
    template<int utt> inline typename valtype_t<utt>::t* _drf_c_i(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const throw () { return reference_t<typename valtype_t<utt>::t>::_drf_c(this); }
    template<int utt> inline typename valtype_t<utt>::t_tu* _drf_c_ai(meta::noargi_tu_t<utt> = meta::noargi_tu_t<utt>()) const throw () { return (typename valtype_t<utt>::t_tu*)reference_t<typename valtype_t<utt>::t>::_drf_c(this); }

    s_long _compat_id(unity_common::__Psm pmsm, s_long utype) const throw();
    bool _compat_chk() const throw();
    s_long _compatibility() const throw();
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

    unity* _path_u(const std::wstring& keylist, bool forced) throw();
    unity* _path_w(const std::wstring& keylist) throw();
    unity* _path_w(const wchar_t* keylist) throw();
    unity* _path_w(const std::string& keylist) throw();
    unity* _path_w(const char* keylist) throw();
    unity* _path_w(const unity& keylist) throw();
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
    template<class T> T* unity::o_api::pobj(s_long flags, meta::noarg_tu_t<T>) const throw()
  #else
    template<class T> T* unity::o_api::pobj(s_long flags, meta::noarg_tu_t<T> = meta::noarg_tu_t<T>()) const throw()
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
    template<class I> o_iptr_t<I> unity::o_api::pinterface(s_long flags, const o_itfset_base* ps0, meta::noarg_tu_t<I>) const throw()
  #else
    template<class I> o_iptr_t<I> unity::o_api::pinterface(s_long flags, const o_itfset_base* ps0 = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw()
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
    template<class I> inline o_iptr_t<I> unity::o_api::pinterface_at(s_long ind, meta::noarg_tu_t<I>) const throw()
  #else
    template<class I> inline o_iptr_t<I> unity::o_api::pinterface_at(s_long ind, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw()
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
    template<class Itfs> s_long unity::o_api::itfs_attach(meta::noarg_tu_t<Itfs>) const throw()
  #else
    template<class Itfs> s_long unity::o_api::itfs_attach(meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const throw()
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
    // Search for set of interaces created/attached within the current binary module.
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
    template<class Itfs> s_long unity::o_api::find_local_by_itfs(s_long flags, s_long ind0, s_long ind2, meta::noarg_tu_t<Itfs>) const throw()
  #else
    template<class Itfs> s_long unity::o_api::find_local_by_itfs(s_long flags, s_long ind0 = 0, s_long ind2 = -1, meta::noarg_tu_t<Itfs> = meta::noarg_tu_t<Itfs>()) const throw()
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
    // Search for set (or several sets) of interaces, from which a valid interface pointer may be created by pinterface().
    // Search direction: from end (larger inds, latest attached sets) to the beginning (smaller inds).
    //  Arguments and algorithm == that of pinterface (see).
    //  If pret != 0, indexes of all matching sets are pushed into it.
    // Returns: >= 0 - index of the first matching set (last attached set that can be cast into I). -1 - not found. -2 - failure.
  #if defined(__GNUC__) || defined(__clang__)
    template<class I> s_long unity::o_api::find_by_itf(s_long flags, o_itfset_base* ps0, std::basic_string<s_long>* pret, meta::noarg_tu_t<I>) const throw()
  #else
    template<class I> s_long unity::o_api::find_by_itf(s_long flags, o_itfset_base* ps0 = 0, std::basic_string<s_long>* pret = 0, meta::noarg_tu_t<I> = meta::noarg_tu_t<I>()) const throw()
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
  template<class Obj, class _> void* unity::_o_sm_t<Obj, _>::sm(s_long ind) throw()
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


  template<class Itfs, class _> o_itfset_base* unity::_o_itfssm_t<Itfs, _>::itfs_new() throw()
    { try { o_itfset_base* p = new _o_itfs_topmost<Itfs>(); p->__pidyn = 0; return p; } catch (...) {} return 0; }
    // NOTE __pidyn of the new copy is set to 0. Should be initialized to point to the new set's container.
  template<class Itfs, class _> o_itfset_base* unity::_o_itfssm_t<Itfs, _>::itfs_new_copy(o_itfset_base* x0) throw()
    { try { if (!x0) { return 0; } _o_itfs_topmost<Itfs>* x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); if (!x) { return 0; } o_itfset_base* p = new _o_itfs_topmost<Itfs>(*x); p->__pidyn = 0; return p; } catch (...) {} return 0; }
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::itfs_delete(o_itfset_base* x) throw()
    { if (!x) { return; } try { delete x; } catch (...) {} }
    // flags OR-ed: 1 - get static type info, 2 - get dynamic type info, 4 - get vecm type index (see type_index_t in vecm_hashx.h),
    //  8 - return _pobj even if the object is deleted,
    //  16 - return sizeof(Obj).
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::itfs_type_info(o_itfset_base* x, o_type_info* pret, s_long flags) throw()
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
  template<class Itfs, class _> s_long unity::_o_itfssm_t<Itfs, _>::__xx_after_construct(o_itfset_base* x0, o_itfset_base* src0, s_long flags) throw()
    { _o_itfs_topmost<Itfs>* x = 0; Itfs* src = 0; try { if (x0) { x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); } if (src0) { src = (_o_itfs_topmost<Itfs>*)src0->__itfs_cast(typeid(Itfs).name()); } } catch (...) {} if (!x) { return false; } if (src0 && !src) { return false; } bool b = false; s_long n = 0; try { b = x->__xx_after_construct(flags, src0, n); } catch (...) {} if (!b) { try { s_long n2 = 0; x->__xx_before_destroy(flags, n2, n); } catch (...) {} } return b; }
  template<class Itfs, class _> void unity::_o_itfssm_t<Itfs, _>::__xx_before_destroy(o_itfset_base* x0, s_long flags) throw()
    { try { _o_itfs_topmost<Itfs>* x = 0; if (x0) { x = (_o_itfs_topmost<Itfs>*)x0->__itfs_cast(typeid(Itfs).name()); } if (!x) { return; } s_long n = 0; x->__xx_before_destroy(flags, n, -1); } catch (...) {} }
  template<class Itfs, class _> void* unity::_o_itfssm_t<Itfs, _>::sm(s_long ind) throw()
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
    void __set_pp(unity_common::__Psm psm, I* pi) throw () { __psm = psm; __pi = pi; }

    template<class T> typename T::PF __call() const { return (typename T::PF)__psm(unity_common::find_type<T, typename __Proxy::__Methods>::ind);  }

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


    s_long n() const throw();
    unity& operator[] (const unity& k) throw (exc_subscript);
    const entry* operator() (s_long ind) const throw();
    const entry* find(const unity& k, s_long* ret_pind = 0) const throw();
    s_long insert(const unity& k, const entry** ret_pentry = 0, s_long* ret_pind = 0, s_long ind_before = hashx_common::no_elem) throw();
    s_long remove(const unity& k) throw();
    s_long remove_i(s_long ind) throw();
    const entry* h(s_long ind_h) const throw();
    s_long hl_clear(bool full) throw();
    const t_hash::f_kf* pkf() const throw();
    bool hl_set_kf(const t_hash::f_kf& kf) throw();

    s_long qi_noel() const throw();
    s_long qi_next(s_long pos) const throw();
    s_long qi_prev(s_long pos) const throw();
    s_long qi_indk(s_long ind) const throw();

    s_long compatibility() const throw();
    void assign_impl(const _hl_i& x);

  protected: unity_common::__Psm pmsm; // == pmsm of the containing unity object (factual _hl_i object may not be moved between wrappers)
    _hl_i(); _hl_i(const _hl_i&); void operator=(const _hl_i&);
  };




    //==  Write string representation of a unity object to std ostream.
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
    unitypc(const unity* p_)         throw() : _p(p_) {}
    unitypc(const unity& x)         throw() : _p(&x) {}
    unity* _pnonc() const         throw() { return const_cast<unity*>(_p); }

    bool b_valid() const { return !!_p; }

    unity& operator[] (const unity& ki) { check(); return (*_pnonc())[ki]; }
    const unity& operator[] (const unity& ki) const { check(); return (*_pnonc())[ki]; }

    unity* operator->() { check(); return _pnonc(); }
    const unity* operator->() const { check(); return _pnonc(); }

    unity& operator*() { check(); return *_pnonc(); }
    const unity& operator*() const { check(); return *_pnonc(); }

    bool operator==(const unitypc& p2) const         throw() { return _p == p2._p; }
    bool operator==(const unity* p2) const         throw() { return _p == p2; }
    bool operator!=(const unitypc& p2) const         throw() { return _p == p2._p; }
    bool operator!=(const unity* p2) const         throw() { return _p != p2; }

    operator bool() const         throw() { return !!_p; }

    operator unity*()         throw() { return _pnonc(); }
    operator const unity*() const         throw() { return _pnonc(); }

    operator unity&() { return **this; }
    operator const unity&() const { return **this; }

      // Using objPtr, retrieves T* (without const) from the referenced object.
      //  Returns T* on success, 0 on failure.
    template<class T> operator T*()         throw() { typedef typename meta::nonc_t<T>::t t; t* px = 0; if (_p) { px = _pnonc()->objPtr<t>(); if (!px) { px = (t*)_pnonc()->objPtr_c<const T>(); } } return px; }
    template<class T> operator const T*() const         throw() { typedef typename meta::nonc_t<T>::t t; const T* px = 0; if (_p) { px = _pnonc()->objPtr_c<t>(); if (!px) { px = _pnonc()->objPtr_c<const T>(); } } return px; }

      // Using objPtr, retrieves T* (without const) from the referenced object.
      //  Returns T& on success, or generates an exception if T* is 0.
    template<class T> operator T&()        { typedef typename meta::nonc_t<T>::t t; t* px = 0; if (_p) { px = _pnonc()->objPtr<t>(); if (!px) { px = (t*)_pnonc()->objPtr_c<const T>(); } } return *unity::checked_ptr<T>(px); }
    template<class T> operator const T&() const        { typedef typename meta::nonc_t<T>::t t; const T* px = 0; if (_p) { px = _pnonc()->objPtr_c<t>(); if (!px) { px = _pnonc()->objPtr_c<const T>(); } } return *unity::checked_ptr<const T>(px); }

    void check() const throw(XUExec) { if (!_p) { throw XUExec("unitypc.p == 0"); } }
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
  inline unitypc operator+(const unity& x) throw() { return x; }

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
    { if (!*this) { return 0; } return (**this).path(path); }

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
    //    To get non-default value, h should be any kind of tree - hash, map, array, containing other objects recursively.
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
    //    To get non-default value, h should be any kind of tree - hash, map, array, containing other objects recursively.
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

  std::wstring cmd_myexe(); // full path and filename of this exe (calculated once)
    // Command line, without executable path/filename.
    //  b_quoting true: quote strings with special characters and white spaces.
    //  b_quoting false: only concatenate all arguments as they are.
  std::wstring cmd_string(bool b_quoting = true);
  unity cmd_array(); // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)

    // Prepare single command line argument with properly escaped and replaced characters.
    //  See original processctl::ff_mc::arg1() for details.
  std::string cmd_arg1(const std::string& s, bool b_shell);
  std::string cmd_arg1(const std::wstring& s, bool b_shell);

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

    // Wide character string comparison (s1>s2: 1, s1==s2: 0, s1<s2: -1)
  s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case);

    // Non-recursive replacement.
    // Replace 'from' string occurences in 's' by 'to', return the result.
    // If nmax >= 0, no more than nmax leftmost replacements are done.
    // NOTE On ignoreCase == true, replace() uses system-default locale to search for substrings.
  std::string replace(const std::string& s, const std::string& from, const std::string& to, bool ignoreCase = false, s_ll nmax = -1);
  std::wstring replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, bool ignoreCase = false, s_ll nmax = -1);

    // Trim all 'swat' string occurences in the beginning and end of the string 's'
  std::wstring trim(const std::wstring& s, const std::wstring& swhat = L" ", bool b_left = true, bool b_right = true);
  std::string trim(const std::string& s, const std::string& swhat = " ", bool b_left = true, bool b_right = true);

    // NOTE l/u-case functions depend on the currently set locale.
  std::string lcase(const std::string& s);
  std::wstring lcase(const std::wstring& s);
  std::string ucase(const std::string& s);
  std::wstring ucase(const std::wstring& s);

    // true if str matches the given pattern.
    //  Recognizes constructs similar to: [a-zA-Z], *, ?, # (i.e. digit)
  bool wstring_like(const std::wstring& str, const std::wstring& ptn0);

  unity split(const std::wstring&, const std::wstring& delim, meta::s_ll nmax = -1); // returns 0-based array of utString
  unity split(const std::string&, const std::string& delim, meta::s_ll nmax = -1); // returns 0-based array of utString
  std::vector<std::string> splitToVector(const std::string&, const std::string& delim, meta::s_ll nmax = -1);
  std::vector<std::wstring> splitToVector(const std::wstring&, const std::wstring& delim, meta::s_ll nmax = -1);
  std::string join(const unity& asrc, const std::string& delim);
  std::wstring join(const unity& asrc, const std::wstring& delim);

    // Returns a short string representation of x.
    // If x is a string, it is just copied.
    // If x is an array, object, or unknown value,
    //    only a short prefix and the object type name
    //    is included into output (but NO CONTENTS).
    // If x is a date, it is converted using fixed format: YYYY-MM-DD HH-MM-SS,
    //    where H, M, S are missing if all are 0.
    // If x is char, its boolean value is returned: "True" on 0 or "False" on non-0.
    // For other values (utEmpty, utInt, utFloat)
    //    x.s_copy() is returned.
  std::wstring CStr3(const unity& x);

    // Date/time generators.
  _unitydate d_datetime(s_long y, s_long m, s_long d, s_long h = 0, s_long min = 0, s_long s = 0);
  _unitydate d_time(s_long h, s_long m, s_long s);
  _unitydate d_now(bool allow_fracsec = false); //returns the current local time
  _unitydate d_nowutc(bool allow_fracsec = false); //returns the current UTC time

    // Retuns a copy of selected argument value. ind is 1-based.
    //  If ind is out of range or selects a default argument, utEmpty is returned.
  unity choose(s_long ind, const unity& x1=unity::_0, const unity& x2=unity::_0, const unity& x3=unity::_0, const unity& x4=unity::_0, const unity& x5=unity::_0, const unity& x6=unity::_0, const unity& x7=unity::_0, const unity& x8=unity::_0, const unity& x9=unity::_0, const unity& x10=unity::_0, const unity& x11=unity::_0, const unity& x12=unity::_0, const unity& x13=unity::_0, const unity& x14=unity::_0, const unity& x15=unity::_0, const unity& x16=unity::_0, const unity& x17=unity::_0, const unity& x18=unity::_0, const unity& x19=unity::_0, const unity& x20=unity::_0, const unity& x21=unity::_0, const unity& x22=unity::_0, const unity& x23=unity::_0, const unity& x24=unity::_0, const unity& x25=unity::_0, const unity& x26=unity::_0, const unity& x27=unity::_0, const unity& x28=unity::_0, const unity& x29=unity::_0,
    const unity& x30=unity::_0);




    //==  Decode/encode a line like param1=val1; param2=val2...
  struct paramline
  {
    static const std::string cpterm; // "; ";
    static const std::string ceterm; // " = ";
    static const std::string cvterm; // "|";

    static const std::wstring wpterm; // L"; ";
    static const std::wstring weterm; // L" = ";
    static const std::wstring wvterm; // L"|";

    static const unity& _0;

      //Decode a parameter line.
      //Recognizes ";" and pterm2 as element separator.
      //Puts keys and values into mh.
      //  mh is initialized as utMap (or existing map cleared) on useMap == true,
      //  or as utHash on useMap (or existing hash cleared) == false.
      // NOTE (decode(mh) only) When the existing map or hash is cleared, its flags (keys comparison) are kept.
      //    The results of decoding depend on it. If default behavior is necessary,
      //    the client should call mh.clear() or mh.u_clear() before decode().
      //Example:
      //  key1 = value1; key2 = value2; key3 = | array elem. 1 | array elem. 2 | array elem. 3 | 1234 | 3.14159 | array elem. 6;
      //  key4 = \0; key4a = false; key5 = \1; key5a = true;
      //  key6 = 2014-01-15 23:59:00; key6a = \d2014-01-15 23:59:00;
      //  key6b = 2014-01-15; key6c = \d2014-01-15;
      //  key6d = \s2014-01-15 23:59:00;
      //  key7 = \e;
      //  key8 = 1.1; key8a = 1.; key8b = \f1;
      //  key9 = 2; key9a = \i2.0;
      //  key10 = \z
      //
      //  Here,
      //    key1, key2 have literal "value1", "value2" values,
      //    key3 is 1-based utArray of 6 utUnity elements,
      //    keys 4 to 5a have boolean values,
      //    keys 6 to 6c have utDate values,
      //    key6d has the literal string value,
      //    key7 has the utEmpty value,
      //    all key8* have the floating point values,
      //    both key9* have integer values,
      //    key10 has the utArray value with no elements.
      //
      //  The key itself is always a string value.
      //
      //Returns mh.
      //
      //Note 1. Literal occurences of special characters must be escaped
      //  in places or cases where they would be treated incorrectly.
      //  In any place, special characters that are escaped, are treated literally.
      //  All rules do not depend on the platform.
      //  More detailed:
      //    1.1. Literal semicolon (;) must be escaped in all cases (\;).
      //    1.2. Literal 2-byte (CR, LF) sequence should be stated as \~ by default (pterm2 == wCRLF),
      //      to avoid splitting the string literal, i.e. if you need to decode multiline strings as single values.
      //    1.3. Literal spaces must be escaped if occuring leftmost/rightmost of keys, values, and array values.
      //      Otherwise they will not go with the corresponding strings to further processing.
      //    1.4. Literal equality signs (=) must be escaped in keys, i.e. in a "<key>=<value>" construct
      //      the first unescaped "=" is treated as separating the key and the value.
      //      Literal "=" in values is treated correctly both with and without preceding "\".
      //    1.5. Literal "|" must be escaped in values only if it is the first character of a string value.
      //      Alternatively, such string may be preceded by \s.
      //      Otherwise, the value will be treated as an array.
      //    1.6. Literal backslash must be escaped (\\) when it's necessary to avoid any of the above cases,
      //      and also if \0, \1, \e, \d, \i, \f, \s or \z should occur literally at the beginning of a string value,
      //      instead of specifyng a type or a special value.
      //      Any other occurences of the backslash are not treated as escape sequnces,
      //      and both the backslash and the following character are normal part of a key or string value.
      //Note 2. If an exception occurs during the decoding,
      //  mh contains keys and values decoded up to the place in ssrc that caused the exception.
    unity& decode(const std::wstring& ssrc, unity& mh, bool useMap, const std::wstring& pterm2 = wCRLF);
    unity decode(const std::wstring& ssrc, bool useMap, const std::wstring& pterm2 = wCRLF);

    unity& decode1v(const std::wstring& ssrc, unity& dest);
    unity decode1v(const std::wstring& ssrc);

      // Decode multiline text, representing a tree of values.
      //  Default behavior:
      //  1. Clear mh and set its type to map or hash (for useMap true or false resp.).
      //  2. Split ssrc, using pterm2.
      //  3. Decode each resulting substring and merge the result into mh,
      //      On duplicate key, the existing (i.e. first set) value is kept.
      //      For lines with the same branch paths, contents are merged into the same branch of mh.
      //  Branch path rules:
      //  1. Normally, in a substring of ssrc, "" (empty string) key specifies a branch path, e.g. "=|key1|key2".
      //      All other keys and values from this substring are put into the branch,
      //      e.g. mh.hash("key1").hash("key2").hash(k[i]) = v[i].
      //  2. If no path specified (or "" key's value is not an array), k[i], v[i] are merged into the root (mh itself).
      //  3. Path element type: string, number, empty, or any other type, normally supported by paramline::decode() for array elements.
      // flags:
      //  0x1 - for mh and branches, use map as target container type instead of hash.
      //  0x2 - overwrite value in a branch if met duplicate path.
      //      (Branch by value, value by branch, value by value. Branches having same path are always merged).
      //  0x4 (decode_tree(mh) only) - do not clear mh if it's already associative, merge ssrc tree into mh.
      //    This has the following effects:
      //      1) flag 0x1 is ignored, the input mh type (map or hash (dflt.)) is used for all new nodes.
      //      2) mh is not cleared, all decoded branches are merged into it.
      //      3) all new nodes inherit mapFlags (or hashFlags) from mh.
      //  0x8 - ignore substring like regexp. "^[\t ]*//.*$". This allows for writing single-line C-style comments.
      //  0x10 - convert distinct CR, LF and LFCR sequences to CRLF before decoding.
      //  0x20 - include branch paths, i.e. pairs ("", branch path as utUnityArray of keys),
      //      into all branches of the decoded tree.
      //      (NOTE By default, root path (empty array) is not assigned to mh[""], unless explicitly specified: "=\z").
      //      When 0x20 flag is not set, "" (empty string) key does not occur in mh.
      //  0x40 - treat lines containing braces and optional spaces (like regexp. "[ {}]+") as start/end of multiline key/value block.
      //
      //    A. decode_tree() without 0x40 flag:
      //      begin: process lines.
      //      (path[, keys/values]): branch path = path; branch[key[i]] = value[i].
      //      keys/values only: branch path = empty path (i.e. root); mh[key[i]] = value[i].
      //      line without path or keys: skip.
      //
      //    B. decode_tree() with 0x40 flag:
      //      begin: stack.front = empty path (i.e. root); last_path = empty path; process lines.
      //      (path[, keys/values]): last_path = stack.back + path; branch path = last_path; branch[key[i]] = value[i].
      //      (keys/values only): last_path = stack.back; branch path = last_path; branch[key[i]] = value[i].
      //      line without path or keys: skip.
      //      {: if (last_path == stack.back) { last_path += empty elem. (utEmpty); } stack.push(last_path).
      //      }: last_path = stack.size > 1 ? stack.pop() : stack.front.
      //
      //      Example. The following two sequences are equivalent, when decoded with 0x40 flag:
      //
      //      =|playback; autostart = true; stop1 = true
      //      =|playback|lists; /home/list1.m3u; /home/list2.m3u; /home/list3.m3u;
      //
      //      =|playback
      //      {
      //        autostart = true; stop1 = true
      //        =|lists
      //        {
      //          /home/list1.m3u;
      //          /home/list2.m3u;
      //          /home/list3.m3u;
      //        }
      //      }
      //
      //      NOTE Braces are recognized only if the line does not contain any other characters except spaces,
      //        otherwise brace will be treated as part of a key.
      //
      // Returns: mh.
      // NOTE With default pterm2, literal CRLF pairs in ssrc must be replaced with special sequence "\~".
      //  See also decode().
    unity& decode_tree(const std::wstring& ssrc, unity& mh, s_long flags = 0, const std::wstring& pterm2 = wCRLF);
    unity decode_tree(const std::wstring& ssrc, s_long flags = 0, const std::wstring& pterm2 = wCRLF);

      //Encodes the given map or hash (uses pterm as element separator).
      //  If mhsrc is of other subtype, returns "".
      //  If an exception occurs during encoding, sdest may contain partially encoded data.
    std::wstring& encode(const unity& mhsrc, std::wstring& sdest, const std::wstring& pterm = wpterm);
    std::wstring encode(const unity& mhsrc, const std::wstring& pterm = wpterm);

      //Encode single part of line.
    std::wstring encode1n(const unity& name); // encode single name
    std::wstring& encode1v(const unity& value, std::wstring& sdest); // encode single value (scalar or array)
    std::wstring encode1v(const unity& value);

      //Encodes the given map or hash into multiline text, producing separate text line
      //  for each branch (all key-value pairs of each associative array except pairs where value is associative array itself).
      //  At the beginning of each line, an empty key with value == full path of the branch is inserted: "=|key1|key2...".
      //  pterm - used to separate parameters in a line, representing a branch.
      //  pterm2 - used to separate lines (branches).
      // Result:
      //  a) multiline text.
      //  b) if mhsrc is not an associative array, returns "".
      //  c) if an exception occurs during encoding, sdest may contain partially encoded data.
    std::wstring& encode_tree(const unity& mhsrc, std::wstring& sdest, const std::wstring& pterm = wpterm, const std::wstring& pterm2 = wCRLF);
    std::wstring encode_tree(const unity& mhsrc, const std::wstring& pterm = wpterm, const std::wstring& pterm2 = wCRLF);

      //set1 += set2, where sets can be utMap or utHash (or else XUTypeMismatch is generated),
      //  keep_first = true prevents overwriting the existing values in set1 by values from set2.
      //Returns set1.
    unity& merge(unity& set1, const unity& set2, bool keep_first = false);
    unity& merge(unity& set1, const std::wstring& set2_pl, bool keep_first = false);

    typedef const unity& _rcu;

      // Returns utMap, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output map, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
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
      //  On inserting keys into the output hash, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_h
      ( _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

      // Returns utHash, containing pairs {x1, x2}, {x3, x4} and so one. NOTE All values after first _0 are ignored.
      //  On inserting keys into the output hash, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_hx
      ( s_long fk, // unity::fkcmp* flags
        _rcu x1=_0, _rcu x2=_0, _rcu x3=_0, _rcu x4=_0, _rcu x5=_0, _rcu x6=_0, _rcu x7=_0, _rcu x8=_0, _rcu x9=_0, _rcu x10=_0, _rcu x11=_0, _rcu x12=_0, _rcu x13=_0, _rcu x14=_0, _rcu x15=_0, _rcu x16=_0, _rcu x17=_0, _rcu x18=_0, _rcu x19=_0, _rcu x20=_0, _rcu x21=_0, _rcu x22=_0, _rcu x23=_0, _rcu x24=_0, _rcu x25=_0, _rcu x26=_0, _rcu x27=_0, _rcu x28=_0, _rcu x29=_0, _rcu x30=_0, _rcu x31=_0, _rcu x32=_0, _rcu x33=_0, _rcu x34=_0, _rcu x35=_0, _rcu x36=_0, _rcu x37=_0, _rcu x38=_0, _rcu x39=_0, _rcu x40=_0, _rcu x41=_0, _rcu x42=_0, _rcu x43=_0, _rcu x44=_0, _rcu x45=_0, _rcu x46=_0, _rcu x47=_0, _rcu x48=_0, _rcu x49=_0, _rcu x50=_0, _rcu x51=_0, _rcu x52=_0, _rcu x53=_0, _rcu x54=_0, _rcu x55=_0, _rcu x56=_0, _rcu x57=_0, _rcu x58=_0,
        _rcu x59=_0, _rcu x60=_0 );

      // _list_*_set_u are same as list_*, only assign the result directly to dest. arg.
      //  NOTE The functions do not check for assigning a value to its own container. This must be avoided.
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
    static void x_encode1(const unity& x, std::wstring& retval, bool x_name, bool x_ar_elem, bool x_nourecur = false);
    static void x_encode_branch(const unity& mh, const std::wstring& path, std::wstring& sdest, hashx<const unity*, int>& hstop, const std::wstring& pterm, const std::wstring& pterm2, const std::wstring& pathpfx);
    static void x_repl_e1(const std::wstring& s1, std::wstring& s2, bool s_name, bool s_ar_elem);
    static void x_replace2a(std::wstring& s, s_long flags);
    static void x_replace4(const std::wstring& s1, std::wstring& s2, s_long& flags);
    static void x_decode1v(unity& v, bool v_ar_elem, s_long flags);
    static bool x_decode1v_auto_date(const std::wstring& s, unity& retval) throw ();
    static bool x_incorrect_numeric_value_str(const std::wstring& s, bool b_nans);
    static bool x_incorrect_integer_value_str(const std::wstring& s, bool allow_float_str);
  };




    //==  File operations utilities.
  enum EFileUtilsPredefinedDir
  {
      pdCurDir, //the current directory
      pdThisAppDir, //path to the current application
      pdTempDir, //Windows: %systemroot%\temp, Linux: %TMPDIR%
      pdUserDefinedDir,
          //- user-defined directory, passed as second argument; if none, the path to the current application is used
      pdDoNotChange // do not complete path
  };
  struct file_utils
  {
        //Returns true is pathstr specifies the full path.
        //Returns false for relative paths.
        //The function analyzes both correct anf incorrect paths.
    bool is_full_path(const std::wstring& pathstr) const;
    bool is_full_path(const std::string& pathstr) const;

        //Get path without filename (just remove the last path element).
    std::wstring strip_path(const std::wstring& pathstr) const;
    std::string strip_path(const std::string& pathstr) const;

        //Get filename (the last path element).
    std::wstring strip_filename(const std::wstring& pathstr) const;
    std::string strip_filename(const std::string& pathstr) const;

        // Returns pathstr without the last extsep and the characters after,
        //  if this extsep occurs after the last path separator.
    std::wstring remove_ext(const std::wstring& pathstr, const std::wstring& extsep = L".") const;
    std::string remove_ext(const std::string& pathstr, const std::string& extsep = ".") const;

        // Inserts s_add into copy of pathstr before the last occurence of extsep,
        //  if this extsep occurs after the last path separator.
        //  Otherwise, returns pathstr + s_add.
    std::wstring add_to_name(const std::wstring& pathstr, const std::wstring& s_add, const std::wstring& extsep = L".") const;
    std::string add_to_name(const std::string& pathstr, const std::string& s_add, const std::string& extsep = ".") const;

        // Replace filename part in fnp with another filename fn.
    std::wstring replace_filename(const std::wstring& fnp, const std::wstring& fn) const;
    std::string replace_filename(const std::string& fnp, const std::string& fn) const;

        //Joins two path parts (or copies pathstr, if pathstr2 is omitted).
        //Then, deletes all duplicate slahses.
        //Finally, deletes the last slash, if the path has no characterss after it.
    std::wstring join_path(const std::wstring& pathstr, const std::wstring& pathstr2 = L"") const;
    std::wstring join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3) const;
    std::wstring join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3, const std::wstring& ps4) const;

    std::string join_path(const std::string& pathstr, const std::string& pathstr2 = "") const;
    std::string join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3) const;
    std::string join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3, const std::string& ps4) const;

        //Returns True if path contains slash at the end.
    bool has_rightmost_patshep(const std::wstring& pathstr) const;
    bool has_rightmost_patshep(const std::string& pathstr) const;

        //Returns false if path contains invalid symbols
        //   or incorrectly represents path in some other way.
        //Otherwise returns true.
    bool is_valid_path(const std::wstring& pathstr) const;
    bool is_valid_path(const std::string& pathstr) const;

        //If sPath is already a full path, it is returned as is.
        //If sPath starts with ".", the point is removed, and the result is prepended with the current directory, then the path is regarded as complete.
        //In other cases, a predefined path, selected by pd argument, is prepended to sPath.
        //  For pd = pdUserDefinedDir, the function uses sUserDefDir. If sUserDefDir is not specified, it is assumed to be this applicaition executable file path.
    std::wstring complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir = L"") const;
    std::string complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir = "") const;

        //Returns true if file exists (and is not a directory).
        //  If sPath contains rightmost path separator character,
        //  the function always returns false.
    bool is_ex_file(const std::wstring& sPath) const;
    bool is_ex_file(const std::string& sPath) const;

        //Returns true if directory exists (and is not a file).
        //  sPath may or may not contain rightmost path separator character,
        //  this does not influence the behavior.
    bool is_ex_dir(const std::wstring& sPath) const;
    bool is_ex_dir(const std::string& sPath) const;

        // Non-recursive expansion of environment variables in s.
        // NOTE std::wstring version of expand_env_nr
        //  converts system variable names between %-signs
        //  into local OS encoding (chars), gets variable value (again chars),
        //  then converts back to UTF-16.
    std::wstring expand_env_nr(const std::wstring& s) const;
    std::string expand_env_nr(const std::string& s) const;

        //Recursively create direcories to ensure that sPath exists.
        //  On success (all was created or already existed) returns true.
        // NOTE POSIX: mk_subdir creates each non-existing directory in sPath
        //  with permissions 0777.
        //  For already existing directories, their current permissions are not modified.
    bool mk_subdir(const std::wstring& sPath) const;
    bool mk_subdir(const std::string& sPath) const;


      //Loads whole file into the string.
      // Returns:
      //    On success: string with the loaded characters (but see also ret_s).
      //    On failure or wrong args.: utEmpty.
      // format_string should contain
      //    1) one of "binary" or "text". "text" is different in:
      //      1.1) auto-converts single \r and \n into \r\n,
      //      1.2) analyzes 0xfeff tag on loading UTF-16, but does not insert it into string.
      //    2) one or more encodings to try: "local8bit", "utf16le", "utf16be", "lsb8bit".
      //    NOTE local8bit, if allowed and all others do not match, is used as the default encoding.
      //    NOTE lsb8bit (read raw bytes as lower byte of wchar_t) suppresses local8bit.
      //    NOTE Any unknown parts of format_string are ignored.
      // ret_s: if a unity object is passed into ret_s, it is assigned the loaded string,
      //    and load_string itself returns boolean value (success/failure).
      // pd: see complete_path.
      // NOTE To load UTF-8 string, use couple: file_io::load_bytes, bsUtf8ToWs.
    unity load_string(const std::string& format_string, const std::wstring& sPath, EFileUtilsPredefinedDir pd = pdCurDir, unity& ret_s = unity::_0nc) const;
    unity load_string(const std::string& format_string, const std::string& sPath, EFileUtilsPredefinedDir pd = pdCurDir, unity& ret_s = unity::_0nc) const;

      //Saves characters of the string str to the specified file.
      // format_string should contain
      //    1) one of "binary" or "text". "text" is different in:
      //      1.1) auto-converts single \r and \n into \r\n,
      //      1.2) auto-inserts 0xfeff tag on saving UTF-16.
      //    2) target encoding: "local8bit", "utf16le", "utf16be", "lsb8bit".
      //    3) mode: "truncate" or "append".
      //    For example: "text append utf16le".
      //    NOTE Any unknown parts of format_string are ignored.
      // pd: see complete_path.
      // Returns:
      //    On success: true.
      //    On failure or wrong args.: false.
      // NOTE To save UTF-8 string, use couple: wsToBsUtf8, file_io::save_bytes.
    bool save_string(const std::string& format_string, const std::wstring& str, const std::wstring& sTargetFilePath, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const throw();
    bool save_string(const std::string& format_string, const std::wstring& str, const std::string& sTargetFilePath, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const throw();


      // Loads bytes from the given file into dest.
      // 1 - success.
      // 0 - file does not exist.
      // -1 - memory alloc. error, or wrong arguments.
      // -2 - file i/o error. NOTE On i/o error, dest may be left modified.
    int load_cstr(const std::wstring& fnp, std::string& dest) throw();
    int load_cstr(const std::string& fnp, std::string& dest) throw();
    int load_cstr(const char* fnp, std::string& dest) throw();

      // Saves bytes from src to the given file.
      //    b_append == false truncates the file before writing, if it exists.
      //    if n == 0, pdata may be 0.
      // 1 - success.
      // 0 - failed to create file (or open the existing file for writing).
      // -1 - data size too large, or memory alloc. error, or wrong arguments.
      // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
    int save_cstr(const std::wstring& fnp, const std::string& src, bool b_append) throw();
    int save_cstr(const std::string& fnp, const std::string& src, bool b_append) throw();
    int save_cstr(const char* fnp, const std::string& src, bool b_append) throw();
    int save_cstr(const char* fnp, const char* pdata, meta::s_ll n, bool b_append) throw();

  private:
    static bool xHasCurDirShortCut(const std::wstring& sPath);
    static bool xHasCurDirShortCut(const std::string& sPath);
    static bool xmk_subdir(const std::string& sPath, int level);
  };




    // Implementation part.
    // Default lock selector for fifo_m11_t. Lock itself may be redefined.
    // For example, to disable queue locking on a user type:
    //  namespace yk_c { template<> struct bytes::lock_t<fifo_m11_lks_t<user type> > : bytes::lock_t<meta::nothing> {}; }
  template<class T>
  struct fifo_m11_lks_t {};

    // Implementation part.
    // Async. queue, where 1 reader and 1 writer may reside in different threads.
    // May work in a) non-blocking mode (normally),
    //   b) semi-blocking (on queue overflow, the last element may be overwritten with fresh value),
    //   c) blocking mode (push() waits on overflow an returns if no place appeared).
  template<class T, class LockSelector = fifo_m11_lks_t<T>, class _bs = meta::nothing>
  struct fifo_m11_t
  {
    typedef bytes::lock_t<LockSelector> t_lock; typedef T t_value; typedef cref_t<t_value, cref_nonlock> t_elem;

      // If n < 3 it is adjusted to 3.
      //  After construction, is_valid() should be checked.
    fifo_m11_t(s_long n) throw () : _isup(0), _irec(0), _elems(yk_c::typer<t_elem, _bs>, 0) { if (n < 3) { n = 3; } _elems.el_append_m(n, meta::construct_f<t_elem, meta::nothing, _bs>()); }

    bool is_valid() const throw() { return _elems.n() >= 3; }

      // Pushing a copy of the given value to queue.
      // m:
      //    -2 - non-blocking push. On queue overflow, does nothing, returns -2.
      //    -1 - semi-blocking push. On queue overflow, attempts to overwrite the last pushed value,
      //        which may cause short blocking (rarely - if the receiver pops out all elements
      //        up to the overwritten one and tries to pop it as well within the same thread time quantum).
      //        NOTE In semi-blocking mode, x is copied before locking.
      //    0..1e7 - time, ms. On queue overflow, push will sleep during that time, and then retry once,
      //        in non-blocking mode.
      // Returns:
      //    1 - pushed successfully.
      //    0 - overflow, but the last pushed element was overwritten with a copy of x.
      //    -1 - incorrect m.
      //    -2 - queue overflow (only on m == -2).
      //    -3 - timeout (only on m >= 0).
      //    -4 - memory allocation error (on any m).
      //    -5 - the queue object is invalid.
    s_long push(const t_value& x, s_long m) throw()
    {
      if (!is_valid()) { return -5; } if (!(m >= -2 && m <= 10000000)) { return -1; }
      s_long s(_isup), s2(s + 1), r(_irec); if (s2 >= _elems.n()) { s2 = 0; }
      if (s2 == r) // near-full (1 elem. free) - considered as overflow
      {
        if (m == -2) { return -2; }
        if (m >= 0) { sleep_mcs(meta::s_ll(m) * 1000); return push(x, -2); }
        t_elem e(x, true); if (!e.has_ref()) { return -4; }
        if (true)
        {
          t_lock __lock; if (sizeof(__lock)) {}
          if (_irec == r) { s -= 1; if (s < 0) { s = _elems.n(); } *_elems.pval_0u<t_elem>(s) = e; return 0; }
        }
      }
        // Enough space
      t_elem* p = _elems.pval_0u<t_elem>(s); p->clear(); p->copy(x, true);
      if (p->has_ref()) { _isup = s2; return 1; }
      return -4;
    }

    bool is_empty() const { return _irec == _isup; }

      // m:
      //    -2 - non-blocking pop. May return an empty ref., leaving 1 element unpopped.
      //      This is suitable for very frequent regular pushs/pops.
      //    -1 - semi-blocking pop. Rarely, may block to get the one available element
      //        (only if push uses m == -1).
      //        Locking is needed because push() (see it also) with m == -1
      //        is allowed to overwrite the last element on queue overflow.
      //    0..1e7 - time, ms. Sleep and then once recheck, in non-blocking mode, if an element is available.
      //      The check may block for short time as necessary.
      // b_pop:
      //    true: pop an element and return it (i.e. strong reference to the object).
      //    false: do not pop an element, return its copy (i.e. copy of strong reference to the object).
      // Returns:
      //    a) t_elem with has_ref() == true -- popped successfully.
      //    b) t_elem with has_ref() == false -- no elements available (m == -1: right now, m == -2: after timeout),
      //      or (only on m == -2) 0 or 1 available, but left unpopped; also on wrong m or invalid queue object.
      //    NOTE Since t_elem owns the dynamically allocated value, no mem. alloc. errors may occur in pop.
    t_elem pop(s_long m, bool b_pop) throw()
    {
      if (!is_valid()) { return t_elem(); } if (!(m >= -2 && m <= 10000000)) { return t_elem(); }
      s_long r(_irec), s(_isup);
      if (r == s) { return t_elem(); }
      s_long r2 = r + 1; if (r2 >= _elems.n()) { r2 = 0; }
      if (r2 == s) // getting seemingly last available element
      {
        if (m == -2) { return t_elem(); }
        if (m >= 0) { sleep_mcs(meta::s_ll(m) * 1000); return pop(-2, b_pop); }
        t_elem x;
        if (true)
        {
          t_lock __lock; if (sizeof(__lock)) {}
          t_elem* p = _elems.pval_0u<t_elem>(r); x = *p;
          if (b_pop) { p->clear(); _irec = r2; }
        }
        return x;
      }
      else  // getting exactly non-last element
      {
        t_elem* p = _elems.pval_0u<t_elem>(r);
        t_elem x = *p;
        if (b_pop) { p->clear(); _irec = r2; }
        return x;
      }
    }

//    template<class _bs2> operator fifo_m11_t<T, LockSelector, _bs2>&() { return *(fifo_m11_t<T, LockSelector, _bs2>*)this; }
//    template<class _bs2> operator const fifo_m11_t<T, LockSelector, _bs2>&() const { return *(const fifo_m11_t<T, LockSelector, _bs2>*)this; }
  protected:  volatile s_long _isup, _irec; vecm _elems;
  };




  //==  MULTITHREADED MESSAGE DISPATCHER
#if bmdx_part_dispatcher_mt

    // All functions are intended to be called from a dispatcher client,
    //    normally residing in separate thread.
    //    All operations are internally synchronized.
  struct i_dispatcher_mt
  {
      // Take a new message out of the slot.
      //  slotname: must be string (utString).
      //  Allowed reciever slots:
      //      pi_*, qi_* - read common message
      //      pbi_*, qbi_* - read command
      //      pbo_* - read response to previously sent command
      //  retmsg: on success, overwritten by message string.
      //  retbuf (optional, may be 0): receiving container for binary data,
      //    associated with the message.
      //    If retbuf is not specified, but the message contains binary part,
      //    characters are recoded into wide string (wide char. values 0..255)
      //    and put into "bin" key of the message.
      //    NOTE recoding large binary data blocks results in performance loss.
      //  flags (OR):
      //    1 - return hash in retmsg. If not set, return string in retmsg.
      //      NOTE If the client resides in another binary module than dispatcher,
      //        setting this flag is not recommended (to pop & decode string message may be more compatible).
      //    2 - discard message if it exists, do not write to retmsg, retbuf.
      //      NOTE (command input only) The client must respond anyway to correct address:
      //        a) for multiple sources: peek only (flags |= 0x8, see below); extract sender address; pop & discard (flags |= 2); write response.
      //        b) for single source: pop & discard (flags |= 2); write response to the known address.
      //    4 - ignore (lose) message binary part if it is received but no retbuf given.
      //    8 - "peek" -- return everything as specified, only do not pop the message.
      //        May be used with flag value 2 to test potential pop result w/o getting any data.
      // Returns:
      //    3 - success, ret. message discarded (only on (flags & 2) != 0).
      //    2 - success, got two-part message (msg. itself and binary part). Parts are returned as flags specify.
      //    1 - success, got single-part message (no binary part).
      //    0 - no message, or no response because of no command sent.
      //    20 - (pbo only) remote or interprocess command failed.
      //      retmsg is set to auto-generated response, containing
      //      trg, src, err_code, err_msg. (It does not contain original command message.)
      //      NOTE (flags & 2) (discard message) suppresses code 20, code 3 is returned instead.
      // NOTE After codes >= 0, the client may issue another command.
      // NOTE In case of codes -2..-19 , the slot may be not functional during undefined period of time.
      // NOTE After codes <= -20, the client should take specific action.
      //    -1 - invalid slot name or type.
      //    -2 - failed (mem. alloc. error, invalid proxy object, binary incompatibility etc.).
      //    -3 - no session.
      //    -6 - slot does not exist or inaccessible.
      //    -7 - thread does not exist.
      //    -20 - (pbo only) waiting for command completion, still command target sent no response.
      //    -21 - (pbi, qbi only, i.e. for commands) waiting for the client responding to the current command
      //      (i.e. the client must call write() first, to reply to prev. command).
      //      This may occur if, in the past, the client occasionally lost the control after popping the command,
      //      now the control is regained, and the other side still waits for response.
      //      If the client cannot handle such situation, the recommended behavior is: send a message, containing "text; _e_retry;".
      //    -22 - input message decoding failed. The client should discard the message (re-pop with (flags & 2) != 0).
    virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw() = 0;

      // Write a message into the slot.
      //    Depending on the slot type and condition,
      //    msg may be put into the queue, rejected,
      //    or overwrite prev. message.
      //  msg: paramline-encoded string, or else hash or map. Min. contents:
      //    src = <NAME>; trg = <ADDR>; text = <T>
      //      NAME - source slot name. Name format variants (simple and specialized) are described near slots_create.
      //      ADDR - destination slot address, consisting of concatenated scope, process, thread, slot names.
      //        For in-process messages:
      //          |LP|<thread name>|<slot name>
      //          |LPA|<qs slot name>
      //        For inter-process messages:
      //          (not implemented) |LM|<process name>|<thread name>|<slot name>
      //          (not implemented) |LMA|<qs slot name>
      //        For network messages:
      //          (not implemented) |R|<host name or address:port>|<process name>|<thread name>|<slot name>
      //          (not implemented) |RMA|<host name or address:port>|<qs slot name>
      //          (not implemented) |RPA|<host name or address:port>|<process name>|<qs slot name>
      //        ADDR examples:
      //          |LP|recevier|pi_log
      //          |LP|recevier|pi_indicator|volume_db|L
      //          |R|192.168.1.1|recevier|pi_indicator|power
      //      See also arch_notes.txt for address and slot name formats.
      //      T is arbitrary client message (may be empty).
      //  Matching of slot names between sender and receiver.
      //    1. Allowed slot types correspondence:
      //      po --> pi, qi, qs
      //      qs --> pi, qi
      //      pbo --> pbi, qbi (send command)
      //      pbi, qbi --> pbo (send response)
      //    2. Slot with type and name root only (no additional name parts)
      //      may send to any slot with corresponding type, exactly same root,
      //      with or without any additional name parts.
      //    3. Slot with type, name root and additional name parts
      //      may send to any slot with corresponding type,
      //      exactly same root and exactly same additional name parts.
      //  buf: optional binary data, associated with the message.
      // Returns:
      //    1 - success, written.
      //    0 - success, prev. message overwritten (only for non-command pins).
      // NOTE After codes >= 0, the client may write another message or command.
      // NOTE In case of codes -2..-19 , the slot may be not functional during undefined period of time.
      // NOTE After codes <= -20, the client should take specific action.
      //    -1 - invalid message format, address, slot name or slot type, sender-reciever slot mismatch,
      //        target address for response does not match with command source address.
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
      //    -3 - no session.
      //    -5 - rejected, out of order (only for command messages and responses).
      //    -6 - local source slot does not exist or currently unusable.
      //    -7 - local source thread does not exist.
      //    -8 - local target slot does not exist or currently unusable.
      //    -9 - local target thread does not exist.
      //    -10 - writing to the specified slot from this source thread is not allowed.
      //    -11 - remote or interprocess sending is disabled.
      //    -12 - remote or interprocess sending failed.
      //    -20 - target queue overflow (only for qs, qi), the client may try later.
    virtual s_long write(const unity& msg, const arrayref_t<char>* buf = 0) throw() = 0;

      // Call for information or extended operation.
      //  Common return values:
      //    1 - success.
      //    -1 - invalid request type (rt).
      //    -2 - operation failed.
      //    -3 - session is closed.
      //    -4 - operation not permitted.
      //    -5 - invalid argument (args and/or retval as required by request type).
      //
      //  rt (request type):
      //    1 - get names of all dispatcher threads.
      //      args: not used.
      //      On success, retval: 0-based utStringArray.
      //
      //    11 - get number of dispatcher threads.
      //      args: not used.
      //      On success, retval: utInt.
      //
      //    2 - get names of all slots of a thread.
      //      args: thread name (scalar).
      //      On success, retval: 0-based utStringArray.
      //      Specific return value: -7 - thread does not exist.
      //
      //    12 - get number of slots in a thread.
      //      args: thread name (scalar).
      //      On success, retval: utInt.
      //      Specific return value: -7 - thread does not exist.
      //
      //    3 - set priority and timing for internal thread, delivering subscription messages.
      //      args: array(<priority>, <delivery loop sleep time>).
      //        priority: 1..7 in units of threadctl.
      //        sleep time: mcs, >= 0
      //      retval: not used.
      //      Specific return value: -7 - the internal delivery thread does not exist (configured so).
      //
      //    4 - get priority and timing for internal thread, delivering subscription messages.
      //      retval: 1-based array with 2 elements same as args in rt == 3, see above.
      //      Specific return value: -7 - the internal delivery thread does not exist (configured so).
      //
      //    5 - create a new proxy object and assign it to retval.
      //      args: thread name (scalar) for the new proxy.
      //      See also: dispatcher_mt new_proxy.
      //      NOTE retval is re-initialized to belong to the binary module of the current proxy and dispatcher_mt objects.
      //      Specific return value: -7 - thread does not exist.
      //
      //    6 - create a new thread.
      //      args: configuration a thread, similar to described near dispatcher_mt::dispatcher_mt().
      //        Only one configuration (path <thread name>|...) may be specified.
      //      retval: not used.
      //      Specific return value: -6 - thread already exists.
      //
      //    7 - remove the existing thread (may be applied to the thread associated with the current proxy).
      //      args: thread name (scalar).
      //      retval: not used.
      //      Specific return value: -7 - thread does not exist.
    virtual s_long request(s_long rt, unity& retval, const unity& args = unity()) throw() = 0;

      // Creates one or more slots during dispatcher session.
      //  slotscfg is a) hash or map with configuration tree as described below,
      //      b, c)  single- or multiline string representon of that tree in paramline::decode_tree format (decoding flags: 0x1a).
      //    a)
      //      { "slots", { <slot name>, <empty or slot configuration tree> } }
      //    b)
      //      =|slots; po_messages1; pi_progress1; pi_progress2; |qi_char|UI; |qi_char|single|R|192.168.1.2:3500|vmpcp|UI
      //    c) Multiline format is convenient for specifying per-slot configuration:
      //      =|slots|po_messages1
      //      =|slots|pi_progress1
      //      =|slots|pi_progress2
      //      =|slots|\|qi_char\|UI
      //      =|slots|\|qi_char\|single\|R\|192.168.1.2:3500\|vmpcp\|UI
      //    Slot name:
      //    a) Simple:
      //        <slot type>_<slot name root>
      //      Slot types:
      //        po -- output-only pin, message goes directly to receiver
      //        pi -- input-only pin, messages may be overwritten if not pushed in time
      //        pbo -- strong-bound call (command) pin, keeps sequence
      //          "sender pushes cmd, receiver pops cmd, receiver pushes response, sender pops response"
      //        pbi -- strong-bound call input pin on the receiver side,
      //          only one command at a time may be processed
      //        qi -- non-blocking input queue for values; on overflow, the new value cannot be pushed
      //          (local sender receives an error code, remote sender is not notified)
      //        qbi -- strong-bound call (command) input queue on the receiver side; on overflow,
      //          a new command will not be pushed, and for both local and remote sender
      //          a response with an error message is automatically generated
      //        qs -- queue for broadcasting (multiple senders, multiple receivers)
      //      Slot name root: non-empty string.
      //    b) Specialized:
      //        |<slot type>_<slot name root>|<postfix>
      //      postfix - additional address parts, separated by "|". Examples:
      //        |qi_char|UI
      //        |qi_char|single|R|192.168.1.2:3500|vmpcp|UI
      //    Slot configuration tree: this part is optional. Specifies additional parameters
      //        influencing slot function.
      //    See arch_notes.txt "dispatcher_mt threads configuration structure" for details.
      // Returns:
      //    1 - success.
      //    -1 - invalid or incompatible slotscfg.
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
      //    -3 - no session.
      //    -4 - one of new slot names is not unique.
      //    -7 - thread does not exist.
    virtual s_long slots_create(const unity& slotscfg) throw() = 0;

      // slotname: must be
      //    a) (string) Scalar slot name in the format, described near slots_create.
      //    b) decoded vector slot name (array).
      // Returns:
      //    1 - success.
      //    0 - slot does not exist.
      //    -1 - invalid slotname.
      //    -2 - failed (mem. alloc. error, invalid object, inaccessible thread or smth. else).
      //    -3 - no session.
      //    -7 - thread does not exist.
    virtual s_long slot_remove(const unity& slotname) throw() = 0;

      // Get this thread name as registered in dispatcher.
      //  Thread names are unique within the process.
      //  Thread name cannot be empty string.
      //  retname: on success, overwritten by the returned string.
      // Returns:
      //    1 - success.
      //      In all the following, retname remains unchanged:
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
    virtual s_long thread_name(unity& retname) throw() = 0;

      // Get this process name as registered in dispatcher.
      //  Process names are unique within the local machine.
      //  Process name cannot be empty string.
      //  retname: on success, overwritten by the returned string.
      // Returns:
      //    1 - success.
      //      In all the following, retname remains unchanged:
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
    virtual s_long process_name(unity& retname) throw() = 0;

      // Subscribe, unsubscribe for messages, or check subscription state.
      //  addr_qs - messages source (any address as string or array,
      //    redirecting addresses like LPA are automatically dereferenced into factual address).
      //  recv_sl - receiving qi or pi slot in the current thread (slot name as string or array).
      //    NOTE qs slot and receiver slot must have same name root for subscribing to succeed.
      //  rt - request type:
      //    1 - begin subscribe,
      //    2 - begin unsubscribe,
      //    3 - begin checking for being subscribed,
      //    4 - return the current state of operation,
      //    5 - discard the current operation (do not wait for result).
      //  Ops. 1, 2, 3 complete immediately if request target (addr_qs) is in-process.
      //  Returns:
      //    2 - op. completed, result: subscribed, or already subscribed.
      //    1 - op. completed, result: not subscribed, or just unsubscribed.
      //    0 - op. discarded or no operation is in progress with this addr_qs, recv_sl pair.
      //    10 - op. completed: slot, defined by addr_qs, does not exist on the destination side or inaccessible.
      //    11 - op. completed: addr_qs is unreachable (network and interprocess only).
      //    12 - op. completed: addr_qs does not allow (un-)subscribing recv_sl.
      //    13 - op. timeout (network and interprocess only).
      //    20 - op. is in progress (network and interprocess only).
      //    -1 - invalid args (address, slot name or type, request type).
      //    -2 - failure (out of memory, out of number of parallel requests, no network access etc.).
      //    -3 - no session.
      //    -4 - another operation with same addr_qs, recv_sl is currently pending.
      //    -5 - recv_sl does not exist or inaccessible.
      // NOTE Subscriptions can be created
      //  a) statically, with qs slot configuration (output list).
      //    This is automatically recreated on qs side's restart.
      //  b) dynamically, with subscribe() call from receiver thread.
      //    If any of sides may have restarted, the receiving side may check and resubscribe as necessary.
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw() = 0;

      // Performs periodic tasks:
      //    - pop/deliver messages from thread's qs slots configured with delivery = thread.
      //    - (for disp thread) pop/deliver messages from qs slots with delivery = disp, in all threads.
      // periodic() should be called in a loop
      //    with some sleep at each iteration, during whole session.
      // periodic() call is not necessary in the following cases:
      //    a) the current thread has no qs slots with delivery = thread,
      //    b) the current thread is disp thread, but configuration contains __qsdt >= 0
      //      (automatic delivery for disp thread's messages by internal thread).
      // flags: reserved, must be 0.
      // Returns:
      //    1 - success.
      //    0 - nothing to do for this thread.
      //    -1 - invalid flags.
      //    -2 - failed (mem. alloc. error or smth. else).
      //    -3 - no session.
    virtual s_long periodic(s_long flags = 0) throw() = 0;

    virtual ~i_dispatcher_mt(){}
  };

  template<> struct o_proxy<i_dispatcher_mt> : o_proxy_base<i_dispatcher_mt>
  {
    static const char* __iname() { return "Ye.K. / bmdx / i_dispatcher_mt"; }

    struct __pop { typedef s_long (*PF)(__Interface* __pi, const unity* slotname, unity* retmsg, _carray_base_t<char>* retbuf, s_long flags); static s_long F(__Interface* __pi, const unity* slotname, unity* retmsg, _carray_base_t<char>* retbuf, s_long flags) { return __pi->pop(*slotname, *retmsg, retbuf, flags); } };
    struct __write { typedef s_long (*PF)(__Interface* __pi, const unity* msg, const arrayref_t<char>* buf); static s_long F(__Interface* __pi, const unity* msg, const arrayref_t<char>* buf) { return __pi->write(*msg, buf); } };
    struct __request { typedef s_long (*PF)(__Interface* __pi, s_long rt, unity* retval, const unity* args); static s_long F(__Interface* __pi, s_long rt, unity* retval, const unity* args) { return __pi->request(rt, *retval, *args); } };
    struct __slots_create { typedef s_long (*PF)(__Interface* __pi, const unity* slotscfg); static s_long F(__Interface* __pi, const unity* slotscfg) { return __pi->slots_create(*slotscfg); } };
    struct __slot_remove { typedef s_long (*PF)(__Interface* __pi, const unity* slotname); static s_long F(__Interface* __pi, const unity* slotname) { return __pi->slot_remove(*slotname); } };
    struct __thread_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->thread_name(*retname); } };
    struct __process_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->process_name(*retname); } };
    struct __subscribe { typedef s_long (*PF)(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt); static s_long F(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt) { return __pi->subscribe(*addr_qs, *recv_sl, rt); } };
    struct __periodic { typedef s_long (*PF)(__Interface* __pi, s_long flags); static s_long F(__Interface* __pi, s_long flags) { return __pi->periodic(flags); } };

    virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw() { return __call<__pop>()(__psi(), &slotname, &retmsg, retbuf, flags); }
    virtual s_long write(const unity& msg, const arrayref_t<char>* buf) throw() { return __call<__write>()(__psi(), &msg, buf); }
    virtual s_long request(s_long rt, unity& retval, const unity& args) throw() { return __call<__request>()(__psi(), rt, &retval, &args); }
    virtual s_long slots_create(const unity& slotscfg) throw() { return __call<__slots_create>()(__psi(), &slotscfg); }
    virtual s_long slot_remove(const unity& slotname) throw() { return __call<__slot_remove>()(__psi(), &slotname); }
    virtual s_long thread_name(unity& retname) throw() { return __call<__thread_name>()(__psi(), &retname); }
    virtual s_long process_name(unity& retname) throw() { return __call<__process_name>()(__psi(), &retname); }
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw() { return __call<__subscribe>()(__psi(), &addr_qs, &recv_sl, rt); }
    virtual s_long periodic(s_long flags) throw() { return __call<__periodic>()(__psi(), flags); }

    typedef unity_common::fn_list<__pop, __write, __request, __slots_create, __slot_remove, __thread_name, __process_name, __subscribe, __periodic> __Methods;
  };
  namespace { o_proxy<i_dispatcher_mt> __o_proxy_i_dispatcher_mt_inst; }

  struct dispatcher_mt
  {
      // process_name - non-empty string, should be unique in the local machine context.
      //    Name can be anything except a) empty, b) one of LP, LM, R, LPA, LMA, RPA, RMA.
      // cfg: is a) hash or map with threads configuration tree as described below,
      //      b, c)  single- or multiline string representon of that tree in paramline::decode_tree format (decoding flags: 0x1a).
      //  associative array { thread name, thread configuration tree } in a) hash or map form b) paramline string form.
      //    Thread name may not start from "__" or be empty (such keys are treated specially).
      //    a)
      //      { <thread name>,
      //        { "slots", { <slot name>, <empty or slot configuration tree> } },
      //        { <thread parameter>, <value> }
      //      }
      //    b)
      //      =|thread1|slots; po_messages1; pi_progress1; pi_progress2; |qi_char|UI; |qi_char|single|R|192.168.1.2:3500|vmpcp|UI
      //      =|thread2|slots; pi_messages1
      //      =|thread2; disp = true
      //      __exitmode = 1
      //    c) Multiline format is convenient for specifying per-slot configuration:
      //      =|thread1|slots|po_messages1
      //      =|thread1|slots|pi_progress1
      //      =|thread1|slots|pi_progress2
      //      =|thread1|slots|\|qi_char\|UI
      //      =|thread1|slots|\|qi_char\|single\|R\|192.168.1.2:3500\|vmpcp\|UI
      //      =|thread2|slots|pi_messages1
      //      =|thread2; disp = true
      //      __exitmode = 1
      //    See arch_notes.txt "dispatcher_mt threads configuration structure" for details.
      //    See also slots_create() for brief descr. of slot types.
      // Results:
      // On successful construction, an internal session object is created.
      //    is_valid() will return true.
      //    The client creates proxy objects (new_proxy()), referring to the session.
      //    The client puts proxy objects into threads, so that threads could write and pop
      //      messages.
      // Session ends according with __exitmode parameter in configuration.
      //  (By default, session is immediately ended by ~dispatcher_mt(),
      //    but internal objects are released only after all proxy objects are released by the client.)
    dispatcher_mt(const std::wstring& process_name, const unity& cfg = unity()) throw();
    dispatcher_mt(const std::string& process_name, const unity& cfg = unity()) throw();
    ~dispatcher_mt() throw();

      // true - on successful construction.
      // false - on construction failure, and after destruction.
      // NOTE is_valid() may be called concurrently.
    bool is_valid() const throw();

      // true from successful construction till destruction or end_session().
      // NOTE has_session() may be called concurrently.
    bool has_session() const throw();

      // Resets an internal flag to signal that the session is no longer valid.
      //  Any call via any proxy interface will return error code "session is closed".
      // NOTE until being destroyed, dispatcher_mt holds the session object
      //  with all associated data.
      // NOTE end_session() may be called concurrently.
    void end_session() throw();

      // Relation between proxy object, dispatcher "threads" and CPU thread:
      //  1. Any proxy object represents one particular dispatcher "thread", named by thread_name.
      //  2. Multiple proxy objects may represent same dispatcher "thread" (have same thread_name),
      //    and may be called concurrently.
      //  3. Several proxy objects with different thread names may be used in single CPU thread and vice versa.
      //  4. A proxy object may not be destroyed (== wrapping unity object clear or overwrite)
      //    concurrently with calling any of its methods.
      // Return value:
      //  1 - success. dest contains the new object.
      //    In all the below cases, dest is cleared (empty).
      //    -2 - failure (mem. alloc. error or smth. else).
      //    -3 - no session.
      //    -5 - invalid thread name.
      //    -7 - thread does not exist.
      // NOTE new_proxy() may be called concurrently.
    s_long new_proxy(unity& dest, const std::wstring& thread_name) const throw();
    s_long new_proxy(unity& dest, const std::string& thread_name) const throw();


      // Similar to i_dispatcher_mt::request. Differences:
      //  1) has all possible permissions.
      //  2) the request is not associated with any particular thread (certain request types may fail).
      // NOTE request() may be called concurrently.
    s_long request(s_long rt, unity& retval, const unity& args = unity()) throw();

      // Flags, permitting serious operations from side of any proxy (i_dispatcher_mt request()):
      //    0x1 - create another thread.
      //    0x2 - remove any thread, including proxy's own.
      //    0x4 - new proxy creation for thread with specified name.
      //    0x8 - get list of dispatcher threads, get list of slots for any thread.
      //    0x10 - get/set automatic subscription delivery parameters.
      // If both b_get and b_set are true, new flags are set from f, and after, the old flags are written to f.
      // Dflt. value of dispatcher_mt construction: 0x1f (all allowed).
      // Returns: true - success, false - no session.
      // NOTE frqperm() may be called concurrently.
    bool frqperm(s_long& f, bool b_get = true, bool b_set = false) const throw();

  private:
    struct cch_session; cref_t<cch_session> _r_ths;
    struct thread_proxy; struct mst_semaphore;
    friend struct yk_c::vecm;
    dispatcher_mt(const dispatcher_mt&); dispatcher_mt& operator=(const dispatcher_mt&);
  };
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

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif
