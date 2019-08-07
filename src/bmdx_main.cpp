// BMDX library 1.1 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2019-07-29
// See bmdx_main.h for description.

#ifndef bmdx_main_H


#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
  #pragma GCC diagnostic ignored "-Wnonnull-compare"
  #pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#include "bmdx_main.h"
using namespace bmdx_str::words;
using namespace bmdx_str::conv;

#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <clocale>
#include <cctype>
#include <cwctype>
#include <cerrno>

namespace
{
  enum { pslen = 1, ps2len = 2 };
  typedef std::wstring::size_type _t_wz;
  typedef std::string::size_type _t_sz;
  const _t_wz nposw = std::wstring::npos;
  const _t_sz nposc = std::string::npos;
  struct static_init_lks {};
  bmdx::unity_common::__Psm _rootmodsm(0);
  // const std::wstring& __wBSCRLF() { static const std::wstring x(L"\\\r\n"); return x; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS-dependent code. Part 1. Common library.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bmdx
{
  #ifdef _bmdxpl_Wnds
    s_long wsbs_lkp() { return -1; } // ws/bs functions do not change locale, locking is not needed
  #endif
  #ifdef _bmdxpl_Psx
      s_long wsbs_lkp() { return 10; } // ws/bs functions change locale, locking is needed
  #endif
  #ifdef _bmdxpl_OS_NAME_HERE
      s_long wsbs_lkp() { }
  #endif
//~!!! fix for POSIX systems: avoid irreversible modification of program locale
  _fls75 __locname_du;
  bool __b_locname_du(false);
  struct setlocale_locked
  {
    int cat;
    _fls75 name0;
    t_critsec_locale __lock;
    setlocale_locked(int cat_, const char* locname_)
      : cat(-1), name0(), __lock(wsbs_lkp(), -1)
    {
      if (!locname_) { return; }
      const bool b_du = *locname_ == 0;
      if (b_du && __b_locname_du) { locname_ = __locname_du.c_str(); }
      name0 = std::setlocale(cat_, 0);
      if (_fls75(locname_) != name0)
      {
        #if defined(__BORLANDC__)
          (void)cat_;
          cat = LC_ALL; // this value works well, while others (at least LC_CTYPE) lead to permanent malfunctioning of std. out
        #else
          cat = cat_;
        #endif
        const char* p = std::setlocale(cat, locname_);
        if (p && b_du && !__b_locname_du) { __locname_du = p; __b_locname_du = true; }
      }
    }    
//    ~setlocale_locked() { if (cat >= 0) { std::setlocale(cat, name0.c_str()); } }
  };
}

namespace bmdx
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal utilities.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // NOTE This cannot trim \0 char, but more efficient since does not create additional string argument.
  template<class Char>
  static std::basic_string<Char> _trim(const std::basic_string<Char>& s, const Char* pwhat, bool b_left = true, bool b_right = true)
  {
    if (!pwhat) { return s; }
    meta::s_ll lw = 0; const Char* p = pwhat; while (*p++) { ++lw; }
    meta::s_ll ls = meta::s_ll(s.length());
    if (ls == 0 || ls < lw) { return s; }
    meta::s_ll pos1(0); meta::s_ll pos2(ls);
    if (b_left) { while (pos1 <= ls - lw) { if (0 == std::memcmp(s.c_str() + pos1, pwhat, size_t(lw) * sizeof(Char))) { pos1 += lw; } else { break; } } }
    if (b_right) { while (pos2 > pos1 + lw) { if (0 == std::memcmp(s.c_str() + pos2 - lw, pwhat, size_t(lw) * sizeof(Char))) { pos2 -= lw; } else { break; } } }
    if (pos2 - pos1 == meta::s_ll(s.length())) { return s; }
    if (pos2 <= pos1) { return std::basic_string<Char>(); }
    return s.substr(_t_sz(pos1), _t_sz(pos2 - pos1));
  }
    // NOTE This is able to trim \0 char.
  template<class Char>
  static std::basic_string<Char> _trim_s(const std::basic_string<Char>& s, const std::basic_string<Char>& what, bool b_left = true, bool b_right = true)
  {
    meta::s_ll lw = meta::s_ll(what.length()); if (!lw) { return s; }
    meta::s_ll ls = meta::s_ll(s.length());
    if (ls == 0 || ls < lw) { return s; }
    meta::s_ll pos1(0); meta::s_ll pos2(ls);
    if (b_left) { while (pos1 <= ls - lw) { if (0 == std::memcmp(s.c_str() + pos1, what.c_str(), size_t(lw) * sizeof(Char))) { pos1 += lw; } else { break; } } }
    if (b_right) { while (pos2 > pos1 + lw) { if (0 == std::memcmp(s.c_str() + pos2 - lw, what.c_str(), size_t(lw) * sizeof(Char))) { pos2 -= lw; } else { break; } } }
    if (pos2 - pos1 == meta::s_ll(s.length())) { return s; }
    if (pos2 <= pos1) { return std::basic_string<Char>(); }
    return s.substr(_t_sz(pos1), _t_sz(pos2 - pos1));
  }
  std::wstring trim(const std::wstring& s, const std::wstring& swhat, bool b_left, bool b_right) { return _trim_s(s, swhat, b_left, b_right); }
  std::string trim(const std::string& s, const std::string& swhat, bool b_left, bool b_right) { return _trim_s(s, swhat, b_left, b_right); }

  std::string lcase(const std::string& s) { std::string s2; for(_t_sz pos=0; pos < s.length(); ++pos) { s2 += char(std::tolower(s[pos])); } return s2; }
  std::string ucase(const std::string& s) { std::string s2; for(_t_sz pos=0; pos < s.length(); ++pos) { s2 += char(std::toupper(s[pos])); } return s2; }
  std::wstring lcase(const std::wstring& s) { std::wstring s2; for(_t_wz pos=0; pos < s.length(); ++pos) { s2 += wchar_t(std::towlower(s[pos])); } return s2; }
  std::wstring ucase(const std::wstring& s) { std::wstring s2; for(_t_wz pos=0; pos < s.length(); ++pos) { s2 += wchar_t(std::towupper(s[pos])); } return s2; }

  static bool _wcsileftincl1(const wchar_t* ps1, const wchar_t* ps2) { if (!ps1 || !ps2) { return false; } while (*ps2) { if (!(*ps1 && std::towlower(*ps1) == std::towlower(*ps2))) { return false; } ++ps1; ++ps2; } return true; }

  static meta::s_ll max_scstr() { static meta::s_ll n(0); if (!n) { meta::s_ll n2 = meta::s_ll(std::string().max_size()); if (n2 >> 60) { n2 = (1ll << 60) - 1; } n = n2; } return n; }
  static meta::s_ll max_swstr() { static meta::s_ll n(0); if (!n) { meta::s_ll n2 = meta::s_ll(std::wstring().max_size()); if (n2 >> 59) { n2 = (1ll << 59) - 1; } n = n2; } return n; }

    // Returns <=n chars starting from s[pos], like any of the given chars.
    //    n == -1 means get all matching chars.
  static std::string trim_n(const std::string& s, _t_sz pos, const std::string& chars, s_long n)
  {
    struct __local { static int _0() { return 0; } };
    if (meta::s_ll(pos) < __local::_0()) { pos = 0; } if (pos >= s.size() || n < -1) { return std::string(); }
    _t_sz pos2 = s.find_first_not_of(chars, pos); if (pos2 == nposc) { pos2 = s.size(); }
    if (n != -1) { if (pos2 > pos + n) { pos2 = pos + n; } }
    return s.substr(pos, pos2 - pos);
  }

  static _fls75 dtofls(const _unitydate& x)
  {
    s_long y, m, d, h, min, s; x.d_get(&y, &m, &d, &h, &min, &s); double fs = x.d_ms(); bool is_fs = fs >= 0.001 && fs <= 999.999;

    _fls75 q;
    if (y >= 0 && y < 1000) { q += '+'; } q += y;
    q += '-'; if (m < 10) { q += '0'; } q += m;
    q += '-'; if (d < 10) { q += '0'; } q += d;
    if (h || min || s || is_fs)
    {
      q += ' '; if (h < 10) { q += '0'; } q += h;
      q += ':'; if (min < 10) { q += '0'; } q += min;
      q += ':'; if (s < 10) { q += '0'; } q += s;
      if (is_fs)
      {
        _fls75 xf(int(fs * 1000) + 1000000);
        xf[0] = '.';
        int n = xf.length();
        while (n > 2 && xf[n - 1] == '0') { --n; }
        q += xf.substr(0, n);
      }
    }
    return q;
  }

    // Find (in s) pos. of first character which does not occur in chars.
    //  Search only in range [begin..end). The range is automatically adjusted to fit into s.
    // chars: 0-terminated string. (\0 character cannot be specified as ignored by search.)
    // Returns:
    //  a) if found -- the position in range [begin..end).
    //  b) if not found -- s.length().
  _t_wz _find_char_except(const std::wstring& s, _t_wz begin, _t_wz end, const wchar_t* chars) throw()
  {
    if (begin <= 0) { begin = 0; }
    if (end > s.length()) { end = s.length(); }
    if (!*chars || end <= begin) { return s.length(); }
    for (_t_wz ic = begin; ic < end; ++ic)
    {
      _yk_reg wchar_t q = s[ic];
      _yk_reg wchar_t c = 0;
      _yk_reg const wchar_t* chars2 = chars;
      while (true)
      {
        c = *chars2++;
        if (!c) { break; }
        if (q == c) { break; }
      }
      if (!c) { return ic; }
    }
    return s.length();
  }

namespace
{
  typedef meta::assert<meta::same_t<unity::t_hash::entry, hashx<unity, unity>::entry>::result>::t_true __check1;
  typedef meta::assert<meta::same_t<unity::t_map::entry, hashx<unity, unity>::entry>::result>::t_true __check2;
  typedef meta::assert<(sizeof(unity) == 4 * (sizeof(void*) >= sizeof(s_long) ? sizeof(void*) : sizeof(s_long)))>::t_true __check4;
  typedef meta::assert<(sizeof(unsigned long long) >= 8)>::t_true __check5;

  bool __bmdx_std_getenv(const char* varname, std::string& retval)
  {
    #ifdef _MSC_VER
      char* p = 0; size_t n = 0;
      _dupenv_s(&p, &n, varname);
      if (p) { try { retval = p; } catch (...) { free(p); throw; } free(p); return true; }
      retval.clear();
      return false;
    #else
      const char* p = getenv(varname);
      if (p) { retval = p; return true; }
      retval.clear();
      return false;
    #endif
  }
}

namespace
{
struct _unity_hl_impl : public unity::_hl_i, protected yk_c::hashx<unity, unity, unity::kf_unity, __vecm_tu_selector>
{
  typedef meta::assert<(no_elem == -1)>::t_true __check1;
  typedef yk_c::hashx<unity, unity, unity::kf_unity, __vecm_tu_selector> t_hash;
  typedef t_hash::entry entry;

  _unity_hl_impl();
  _unity_hl_impl(const _unity_hl_impl& x);
  void assign(const _unity_hl_impl& x);

  s_long n() const throw();
  unity& operator[] (const unity& k) throw (exc_subscript);
  const entry* operator() (s_long ind) const throw();
  const entry* find(const unity& k, s_long* ret_pind = 0) const throw();
  s_long insert(const unity& k, const entry** ret_pentry = 0, s_long* ret_pind = 0, s_long ind_before = -1) throw();
  s_long remove(const unity& k) throw();
  s_long remove_i(s_long ind) throw();
  const entry* h(s_long ind_h) const throw();
  s_long hl_clear(bool full) throw();
  const f_kf* pkf() const throw();
  bool hl_set_kf(const f_kf& kf) throw();

  s_long qi_noel() const throw();
  s_long qi_next(s_long pos) const throw();
  s_long qi_prev(s_long pos) const throw();
  s_long qi_indk(s_long ind) const throw();
  s_long compatibility() const throw();
private:
  struct _vecm_x : vecm { friend struct _unity_hl_impl; };

  vecm _list; // n == 1 + hash n
  mutable vecm _indk; // n == hash n

  bool _a_rsv_1() throw();
  bool _a_inserted(s_long ind_before = -1) throw();
  void _a_removed(s_long ind) throw();
  void _a_cleared() throw();
  bool _a_indk() const throw();

  bool _check() const throw();
  const _vecm_x& __crvx(const vecm& v) const throw();
  meta::s_ll _v(s_long prev, s_long next) const throw();
  s_long _iprev(meta::s_ll x) const throw();
  s_long _inext(meta::s_ll x) const throw();
  meta::s_ll _v_of(s_long ind) const throw();
  s_long _prev_of(s_long ind) const throw();
  s_long _next_of(s_long ind) const throw();
  void _setv(s_long ind, meta::s_ll x) const throw();
  void __eject(s_long ind) throw();
  void __insert(s_long ind, s_long after) throw();

  void operator=(const _unity_hl_impl& x); // assign() used instead
};
}

struct cv_ff
{
  template<class T, class _ = __vecm_tu_selector>
  struct _pdyn_t { _pdyn_t(T* p_) : _p(p_) {} ~_pdyn_t() { if (_p) { try { delete _p;  } catch (...) {} _p = 0; } } operator bool() const { return !!_p; } T* ptr() const { return _p; } T* take() { T* p2 = _p; _p = 0; return p2; } private: T* _p; void operator=(const _pdyn_t&); _pdyn_t(const _pdyn_t&); };

    // Returns string representation of the dimensions.
    //  "" on error.
  static std::string vecm_strdim(const vecm& x);
  template<s_long utta, class _ = __vecm_tu_selector> struct vec2_strdim_t { static std::wstring F(const wchar_t* prefix, const unity& u) {
    typedef typename unity::valtype_t<utta>::t V;
    typedef typename unity::valtype_t<utta>::t_tu Vtu;
    Vtu* pvec = (Vtu*)unity::reference_t<V>::deref(u._data, true);
    if (u.isLocal()) { return prefix + bsToWs(vecm_strdim(pvec->rvecm())); }
    if (pvec && pvec->compatibility() >= 0) { return prefix + bsToWs(vecm_strdim(pvec->rvecm())); }
    return prefix + std::wstring(L"[?]");
  } };

  template<class Td, class Ts, bool b = meta::same_t<Td, Ts>::result, class _ = __vecm_tu_selector> struct _kfx_conv
    { static inline Td F(const Ts* x) { Td z[1] = { Td(*x) }; return z[0]; } };
  template<class Td, class _> struct _kfx_conv<Td, Td, true, _> { static inline const Td& F(const Td* x) { return *x; } };
  template<class _> struct _kfx_conv<double, _unitydate, false, _> { static inline double F(const _unitydate* x) { return x->f(); } };

  template<class Td, class Aux = meta::nothing, class _ = __vecm_tu_selector> struct _kfx
  {
    template<class Ts> static inline Td Fconv(const Ts* x) { Td z[1] = { *x }; return z[0]; }
    static inline const Td& Fconv(const Td* x) { return *x; }
    static inline Td Fconv(const _unitydate* x) { Td z[1] = { x->f() }; return z[0]; }

    template<class A, class B> static inline std::string _pmsg(const A* p1, const B* p2) { std::string s = p1 ? "1" : "0"; s += p2 ? "1" : "0"; s += " "; s += typeid(A).name(); s += " "; s += typeid(B).name(); return s; }
    template<class B> static inline std::string _pmsg(const B* p) { std::string s = p ? "1" : "0"; s += " "; s += typeid(B).name(); return s; }



    // In Feq, Fless*, Fhash:
    //  unity::fkcmpSNocase - case insensitive comparison, lowercase chars in hash fn.
    //  unity::fkcmpNFloat - the flag is ignored, A and B (or C1, C2) are always converted to Td, except if any of them is std::wstring (then, must be A=B=Td=std::wstring).



    template<class B> static inline s_long Fhash(const B* p, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      if (!p) { throw XUExec("_kfx::Fhash.1", _pmsg(p)); }
      return hashx_common::kf_basic<Td>().hash(_kfx_conv<Td, B>::F(p));
    }
    static inline s_long Fhash(const std::wstring* p_, s_long flags)
    {
      if (!p_) { throw XUExec("_kfx::Fhash.2", _pmsg(p_)); }
      const std::wstring& key = *p_;
      if (flags & unity::fkcmpSNocase) { _yk_reg s_long h = 0; _yk_reg const wchar_t* p = key.c_str(); _yk_reg _t_wz n = key.size(); while (n > 0) { h *= 31; h += std::towlower(*p); ++p; --n; } return h; }
        else { _yk_reg s_long h = 0; _yk_reg const wchar_t* p = key.c_str(); _yk_reg _t_wz n = key.size(); while (n > 0) { h *= 31; h += *p++; --n; } return h; }
    }
    static inline s_long Fhash(const unity* p_, s_long flags) { return p_->k_hashf(flags); }
    template<class C> static inline s_long Fhash(const vec2_t<C, __vecm_tu_selector>* p, s_long flags)
    {
//      if (!(p && (p->compatibility() > 0))) { throw XUExec("_kfx::Fhash.3"); }
      if (!p) { throw XUExec("_kfx::Fhash.3"); }
      vecm::link1_t<C, true, __vecm_tu_selector> la = p->link1_cbegin();
      s_long n = p->n(); s_long h = _kfx<s_long>::Fhash(&n, 0);
      while (!la.is_aend()) { h ^= Fhash(la.pval(), flags); la.incr(); }
      return h;
    }
    template<class M> static inline s_long _Fhash_assoc(const M* p_, s_long flags)
    {
      if (!p_) { throw XUExec("_kfx::_Fhash_assoc.1",  _pmsg(p_)); }
      s_long n = p_->n(); s_long h = _kfx<s_long>::Fhash(&n, 0);
      for (s_long i = 0; i < n; ++i) { const typename M::entry* e = (*p_)(i); h += _kfx<unity>::Fhash(&e->k, flags); h ^= h * 17; h += _kfx<unity>::Fhash(&e->v, flags); h ^= h / 15; }
      return h;
    }
    static inline s_long Fhash(const unity::t_map_tu* p_, s_long flags) { return _Fhash_assoc(p_, flags); }
    static inline s_long Fhash(const unity::t_hash* p_, s_long flags) { return _Fhash_assoc(p_, flags); }


    template<class A, class B> static inline bool Feq(const A* p1, const B* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<A, std::wstring>::result>::t_false __check1; enum { __z1 = sizeof(__check1) };
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      typedef typename meta::assert<meta::same_t<A, unity>::result>::t_false __check3; enum { __z3 = sizeof(__check3) };
      typedef typename meta::assert<meta::same_t<B, unity>::result>::t_false __check4; enum { __z4 = sizeof(__check4) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.1", _pmsg(p1, p2)); }
      Td a = _kfx_conv<Td, A>::F(p1);
      Td b = _kfx_conv<Td, B>::F(p2);
      if (meta::same_t<Td, double>::result && *reinterpret_cast<meta::s_ll*>(&a) == *reinterpret_cast<meta::s_ll*>(&b)) { return true; }
      return a == b;
    }
    static inline bool Feq(const std::wstring* p1, const std::wstring* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, std::wstring>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.2", _pmsg(p1, p2)); }
      if (flags & unity::fkcmpSNocase)
      {
        if (p1->size() != p2->size()) { return false; }
        _t_wz n = p1->size(); for (_t_wz pos = 0; pos < n; ++pos) { if (std::towlower((*p1)[pos]) != std::towlower((*p2)[pos])) { return false; } }
        return true;
      }
      else { return *p1 == *p2; }
    }
    static inline bool Feq(const unity* p1, const unity* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, unity>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.5", _pmsg(p1, p2)); }
      return p1->k_eq(*p2, flags);
    }
    template<class C1, class C2> static inline bool Feq(const vec2_t<C1, __vecm_tu_selector>* p1, const vec2_t<C2, __vecm_tu_selector>* p2, s_long flags)
    {
//      if (!(p1 && (p1->compatibility() > 0))) { throw XUExec("_kfx::Feq.3"); }
//      if (!(p2 && (p2->compatibility() > 0))) { throw XUExec("_kfx::Feq.4"); }
      if (!p1) { throw XUExec("_kfx::Feq.3"); }
      if (!p2) { throw XUExec("_kfx::Feq.4"); }
      if (p1->nbase() != p2->nbase()) { return false; }
      if (p1->n() != p2->n()) { return false; }
      vecm::link1_t<C1, true, __vecm_tu_selector> la(p1->link1_cbegin()); vecm::link1_t<C2, true, __vecm_tu_selector> lb(p2->link1_cbegin());
      while (!la.is_aend()) { if (!Feq(la.pval(), lb.pval(), flags)) { return false; } la.incr(); lb.incr(); }
      return true;
    }
    template<class M> static inline s_long _Feq_assoc(const M* p1, const M* p2, s_long flags)
    {
      if (!(p1 && p2)) { throw XUExec("_kfx::_Feq_assoc.1", _pmsg(p1, p2)); }
      if (p1->n() != p2->n()) { return false; }
      for (s_long i = 0; i < p1->n(); ++i) { const typename M::entry *e1 = (*p1)(i), *e2 = (*p2)(i); if (!_kfx<unity>::Feq(&e1->k, &e2->k, flags)) { return false; } if (!_kfx<unity>::Feq(&e1->v, &e2->v, flags)) { return false; } }
      return true;
    }
    static inline s_long Feq(const unity::t_map_tu* p1, const unity::t_map_tu* p2, s_long flags) { return _Feq_assoc(p1, p2, flags); }
    static inline s_long Feq(const unity::t_hash* p1, const unity::t_hash* p2, s_long flags) { return _Feq_assoc(p1, p2, flags); }



      // NOTE Fless12 ignores fkcmpRevOrder.
    template<class A, class B> static inline bool Fless12(const B* p1, const A* p2, s_long flags)
    {
      return !!_Fless12(p1, p2, flags);
//      if (flags & unity::fkcmpRevOrder) { return _Fless12(p2, p1, flags); }
//        else { return _Fless12(p1, p2, flags); }
    }

    template<class A, class B> static inline bool _Fless12(const A* p1, const B* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<A, std::wstring>::result>::t_false __check1; enum { __z1 = sizeof(__check1) };
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      typedef typename meta::assert<meta::same_t<A, unity>::result>::t_false __check3; enum { __z3 = sizeof(__check3) };
      typedef typename meta::assert<meta::same_t<B, unity>::result>::t_false __check4; enum { __z4 = sizeof(__check4) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.1", _pmsg(p1, p2)); }
      Td a = _kfx_conv<Td, A>::F(p1);
      Td b = _kfx_conv<Td, B>::F(p2);
      if (meta::same_t<Td, double>::result && *reinterpret_cast<meta::s_ll*>(&a) == *reinterpret_cast<meta::s_ll*>(&b)) { return false; }
      return a < b;
    }
    static inline bool _Fless12(const std::wstring* p1, const std::wstring* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, std::wstring>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.2", _pmsg(p1, p2)); }
      const std::wstring& a = *p1; const std::wstring& b = *p2;
      typedef wchar_t T;
      if (a.size() == 0) { return b.size() > 0; } if (b.size() == 0) { return false; }
      typedef typename std::wstring::const_iterator L;
      L la(a.begin()); L lb(b.begin()); L lae(a.end()); L lbe(b.end());
      do {
        if (la == lae) { return lb != lbe; } if (lb == lbe) { return false; }
        // ~!!! not impl. add flag for locale-dependent comparing c1, c2
        if (flags & unity::fkcmpSNocase) { T c1 = T(std::towlower(*la)); T c2 = T(std::towlower(*lb)); if (c1 != c2) { return c1 < c2; } }
          else { if (*la < *lb) { return true; } if (*lb < *la) { return false; } }
        ++la; ++lb;
      } while (true);
    }
    static inline bool _Fless12(const unity* p1, const unity* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, unity>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.3", _pmsg(p1, p2)); }
      return p1->k_less(*p2, flags);
    }
    template<class C1, class C2> static inline bool _Fless12(const vec2_t<C1, __vecm_tu_selector>* p1, const vec2_t<C2, __vecm_tu_selector>* p2, s_long flags)
    {
//      if (!(p1 && (p1->compatibility() > 0))) { throw XUExec("_kfx::Fless.3"); }
//      if (!(p2 && (p2->compatibility() > 0))) { throw XUExec("_kfx::Fless.4"); }
      if (!p1) { throw XUExec("_kfx::Fless.3"); }
      if (!p2) { throw XUExec("_kfx::Fless.4"); }
      const vec2_t<C1, __vecm_tu_selector>& a = *p1; const vec2_t<C2, __vecm_tu_selector>& b = *p2;
      if (a.n() == 0) { return b.n() > 0; } if (b.n() == 0) { return false; }
      vecm::link1_t<C1, true, __vecm_tu_selector> la(a.link1_cbegin()); vecm::link1_t<C2, true, __vecm_tu_selector> lb(b.link1_cbegin());
      do {
        _yk_reg const C1* p1 = la.pval(); _yk_reg const C2* p2 = lb.pval(); if (!p1) { return !!p2; } if (!p2) { return false; }
        if (_Fless12(p1, p2, flags)) { return true; } if (_Fless12(p2, p1, flags)) { return false; }
        la.incr(); lb.incr();
      } while (true);
    }
    template<class M> static inline s_long _Fless12_assoc(const M* p1, const M* p2, s_long flags)
    {
      if (!(p1 && p2)) { throw XUExec("_kfx::_Fless12_assoc.1", _pmsg(p1, p2)); }
      const M& a = *p1; const M& b = *p2;
      if (a.n() == 0) { return b.n() > 0; } if (b.n() == 0) { return false; }
      s_long m = a.n(); if (m < b.n()) { m = b.n(); }
      for (s_long i = 0; i < m; ++i)
      {
        _yk_reg const typename M::entry *e1 = a(i), *e2 = b(i);
        if (!e1) { return !!e2; } if (!e2) { return false; }
        if (_kfx<unity>::_Fless12(&e1->k, &e2->k, flags)) { return true; } if (_kfx<unity>::_Fless12(&e2->k, &e1->k, flags)) { return false; }
        if (_kfx<unity>::_Fless12(&e1->v, &e2->v, flags)) { return true; } if (_kfx<unity>::_Fless12(&e2->v, &e1->v, flags)) { return false; }
      }
      return true;
    }
    static inline s_long _Fless12(const unity::t_map_tu* p1, const unity::t_map_tu* p2, s_long flags) { return _Fless12_assoc(p1, p2, flags); }
    static inline s_long _Fless12(const unity::t_hash* p1, const unity::t_hash* p2, s_long flags) { return _Fless12_assoc(p1, p2, flags); }
  };

  struct cv_reg
  {
    static void Freg(unity* px, s_long flags);
  };

  struct cv_clear
  {
    typedef void (*PF)(unity* x);
    static void Lclear(unity* x);
    static void Fclear(unity* x);
  };

  struct cv_recreate
  {
    typedef s_long (*PF)(unity* x);
    static s_long Frecreate(unity* x);
  };

  struct cv_array
  {
    typedef s_long (*PFlb_u)(const unity* pv);
    static s_long Llb_u(const unity* pv);
    static s_long Flb_u(const unity* pv);

    typedef s_long (*PFub_u)(const unity* pv);
    static s_long Lub_u(const unity* pv);
    static s_long Fub_u(const unity* pv);

    typedef s_long (*PFsz_u)(const unity* pv);
    static s_long Lsz_u(const unity* pv);
    static s_long Fsz_u(const unity* pv);

      // 1 - success.
      // 0 - ind already exists.
      // < 0 - error.
    typedef s_long (*PFua_ensure_ind)(unity* pv, s_long ind);
    static s_long Lua_ensure_ind(unity* pv, s_long ind);
    static s_long Fua_ensure_ind(unity* pv, s_long ind);

      // 1 - success.
      // -1 - op. failed or ind is out of range.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_get)(const unity* pv, unity* pst, s_long ind);
    static s_long Lel_get(const unity* pv, unity* pst, s_long ind);
    static s_long Fel_get(const unity* pv, unity* pst, s_long ind);

      // 1 - success.
      // -1 - op. failed or ind is out of range.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_set)(unity* pv, const unity* px, s_long ind);
    static s_long Lel_set(unity* pv, const unity* px, s_long ind);
    static s_long Fel_set(unity* pv, const unity* px, s_long ind);

      // 1 - success.
      // -1 - op. failed.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_append)(unity* pv, const unity* px);
    static s_long Lel_append(unity* pv, const unity* px);
    static s_long Fel_append(unity* pv, const unity* px);

      // flags:
      //    0x1 - set lbnd (otherwise lbnd arg. is ignored).
      //    0x2 - set size or ubnd (otherwise sx arg. is ignored).
      //    0x4 (not ignored only on 0x2) - sx is ubnd, otherwise sx is size.
      //  Resizing is done at the end of the array.
      // 1 - success. If pv is an array and only lbnd is set, the function always succeeds.
      // -1 - op. failed.
      // -2 - pv is not an array.
      // -3 - bad args.
    typedef s_long (*PFsz_set)(unity* pv, s_long lbnd, s_long size, s_long flags);
    static s_long Lsz_set(unity* pv, s_long lbnd, s_long size, s_long flags);
    static s_long Fsz_set(unity* pv, s_long lbnd, s_long sx, s_long flags);

      // Returns:
      //  1 - success,
      //  0 - nothing done because the array is empty and its type is not changed,
      //  < 0 - failure (the value indicates the place of error occurence).
    static s_long Lua_fill(unity* pv, const unity* px, s_long utt);
    typedef s_long (*PFua_fill)(unity* pv, const unity* px, s_long utt);
    static s_long Fua_fill(unity* pv, const unity* px, s_long utt);

      // px may be 0 -- a) on removal, b) T() will be used on insertion.
      //  m > 0 -- insert.
      //  m < 0 -- remove.
      //  m == 0 -- do nothing.
      // 1 - success.
      // -1 - op. failed.
      // -2 - pv is not an array, or pv is 0.
    typedef s_long (*PFel_insrem)(unity* pv, const unity* px, s_long ind, s_long m);
    static s_long Lel_insrem(unity* pv, const unity* px, s_long ind, s_long m);
    static s_long Fel_insrem(unity* pv, const unity* px, s_long ind, s_long m);
  };

  struct cv_wstring
  {
      // L / F accepts 1, 2, 4-byte character sequences. Resulting wchar_t is formed by mere assignment, without any conversion.
      // flags are reserved, must be 0.
    typedef std::wstring* (*PFnew)(meta::s_ll, const void* p, s_long, s_long);
    static std::wstring* Lnew(void* (*pmsm)(s_long), meta::s_ll n, const void* p, s_long wcsx, s_long flags);
    static std::wstring* Fnew(meta::s_ll n, const void* p, s_long wcsx, s_long flags);

      // *ps - dest. std::wstring, must exist.
      //  *p - first source character, n - number of characters, wcsx - sizeof(character type) + 0x100 if memory layout is BE
      // flags - reserved, must be 0.
    static bool Fcopy(std::wstring* ps, meta::s_ll n, const void* p, s_long wcsx, s_long flags);

    typedef meta::s_ll (*PFn)(const void* ps);
    typedef const void* (*PFp0)(const void* ps);
    typedef s_long (*PFwcs)();

    static meta::s_ll Ln(void* (*pmsm)(s_long), const void* ps);
    static const void* Lp0(void* (*pmsm)(s_long), const void* ps);
    static s_long Lwcs(void* (*pmsm)(s_long));
    static s_long Lti(void* (*pmsm)(s_long));

    static meta::s_ll Fn(const void* ps);
    static const void* Fp0(const void* ps);
    static s_long Fwcs();
    static s_long Fti();
  };

  struct cv_delete
  {
    typedef s_long (*PF)(void* p, s_long uttf);
    typedef s_long (*PFdel)(void* p, s_long uttf);
    template<int utt_, class _ = __vecm_tu_selector> struct _del_t;

      // uttf: unity type + all flags from unity::ut field (or 0 if storage is detached).
      // Returns:
      //  2 - deleted.
      //  1 - p == 0.
      //  0 - failure during deletion (delete generated an exception, object ref. release failed etc.).
      //  -1 - wrong uttf.
    static s_long Ldelete(void* (*pmsm)(s_long), void* p, s_long uttf);
    static s_long Fdelete(void* p, s_long uttf);
  };

  struct cv_create
  {
    typedef s_long (*PFcreate)(unity::_stg_u* pdest, s_long uttf, s_long flags);

      // pdest must be non-0.
      // flags: reserved, must be 0.
      // NOTE Lcreate/Fcreate does not support arbitrary object creation (utObject).
      // Returns:
      //  1 - success.
      //  0 - dynamic allocation failed. pdest p1 and p2 are set to 0.
      //  -1 - invalid utt/flags. pdest p1 and p2 are set to 0.
    static s_long Lcreate(void* (*pmsm)(s_long), unity::_stg_u* pdest, s_long uttf, s_long flags);
    static s_long Fcreate(unity::_stg_u* pdest, s_long uttf, s_long flags);

      // pdest must be non-0.
      // uttf: unity type + all flags from unity::ut field (or 0 if storage is detached).
      // flags:
      //  0x4 - set p1, p2 to 0 even if utt is invalid.
      // NOTE when pdest->p1 is a pointer, p2 is expected to be unity* or 0. p2 is automatically deleted.
      // Returns flags:
      //  positive value:
      //    0x1 - p1 del. successfully, or scalar in *pdest was trivially erased.
      //      0x2 - p1 destructor failed,
      //    0x4 - p2 del. successfully,
      //      0x8 - p2 destructor failed.
      //  0 - both p1 and p2 are 0.
      //  -1 - invalid arguments, deleted only what's possible.
      //      Both p1 and p2 are set to 0 if 0x4 flag was set, otherwise only correctly deleted pointers are set to 0.
    typedef s_long (*PFdestroy)(unity::_stg_u* pdest, s_long uttf, s_long flags);
    static s_long Ldestroy(void* (*pmsm)(s_long), unity::_stg_u* pdest, s_long uttf, s_long flags);
    static s_long Fdestroy(unity::_stg_u* pdest, s_long uttf, s_long flags);

    typedef unity* (*PFunity_new)();
    static unity* Lunity_new(void* (*pmsm)(s_long));
    static unity* Funity_new();
  };

  struct cv_convert
  {
      //  keep_name
      //    true -  keep object name, if it exists,
      //    false - the name will be reset if utt_dest != utype() and conversion succeeds.
      //  NOTE Fcnv_this on error:
      //    a) on csNormal, csStrict: generates exceptions. The object is left unchanged.
      //    b) on csLazy: sets the object to utEmpty. The object name is kept if keep_name is true.
    static void Lcnv_this(unity* p, EConvStrength cs, s_long utt_dest, s_long keep_name);
    typedef s_long (*PFcnv_this)(unity* p, s_long cs, s_long utt_dest, s_long keep_name);
      // Fcnv_this returns:
      //  1 - success.
      //   0 - utt_dest is same as the current type.
      //  < 0 - failure (the number shows where it occured.)
    static s_long Fcnv_this(unity* p, s_long cs, s_long utt_dest, s_long keep_name) throw();

      // flags:
      //    0x1 - keep object name, if it exists.
      //    0x2 - for any type, create new object dynamically, even if 0x1 is not set or the type is trivial.
      //      (Nonetheless, if px->isEmpty() and the name is not copied from it,
      //        p->_data.p1, p2 will be 0, and p->ut == utEmpty, without xfPtr.)
      //  NOTE Fasg generates exceptions on error (the object is left unchanged).
      // Fasg returns:
      //  1 - success.
      //   0 - p and px are the same.
      //  < 0 - failure (the number shows where it occured.)
    typedef s_long (*PFasg)(unity* p, const unity* px, s_long flags);
    static void Lasg(unity* p, const unity* px, s_long flags);
    static s_long Fasg(unity* p, const unity* px, s_long flags) throw();
  };

  struct u_set_name
  {
      // Returns:
      //    2 - name has been set successfully.
      //    1 (on pn->isEmpty()) - name has been removed successfully or not existed.
      //    0 - failed to set or remove name, nothing changed.
    typedef s_long (*PF)(unity* p, const unity* pn);
    static s_long Lset_name(unity* p, const unity* pn);
    static s_long Fset_name(unity* p, const unity* pn);
  };

  struct u_clear
  {
      // flags:
      //    0x1 (for arrays, map, hashlist) - full clear, set base inds and ky fns to dflt.; otherwise only removes elements.
      //  NOTE The object name is not changed by this function.
      // Returns:
      //    2, 1 - cleared successfully.
      //    0 - failed to create the dflt. value (theoretically for strings), nothing done.
      //    -1 - unknown type, nothing done.
    typedef s_long (*PF)(unity* p, s_long flags);
    static s_long Lu_clear(unity* p, s_long flags);
    static s_long Fu_clear(unity* p, s_long flags);
  };

  struct cv_obj2
  {
    typedef void* (*PFitfslist_cr)();
    static void* Litfslist_cr(unity_common::__Psm pmsm);
    static void* Fitfslist_cr();

      // Returns: 1 - success, 0 - pointer == 0, -1 - deleted but exception occured during op. delete.
    typedef s_long (*PFitfslist_del)(void* p);
    static s_long Litfslist_del(unity_common::__Psm pmsm, void* p);
    static s_long Fitfslist_del(void* p);
  };

  struct cv_hstst
  {
    static void* Fregstg(const char* tname, void* pstg_offer);
  };

  struct cv_ver
  {
    typedef s_long (*PFver)();
    static s_long Fver_map();
    static s_long Fver_hl();
  };

  struct cv_rootldr
  {
    typedef s_long (*PFrootldr)(unity_common::__Psm offer, unity_common::__Psm* ret_rootldr);
    static s_long Frootldr(unity_common::__Psm offer, unity_common::__Psm* ret_rootldr);

    typedef s_long (*PFinit_handle)(unity::mod_handle* h, const char* name, s_long flags0, s_long flags);
    static s_long Finit_handle(unity::mod_handle* h, const char* name, s_long flags0, s_long flags);

    typedef s_long (*PFdestroy_handle)(unity::mod_handle* pmh);
    static s_long Fdestroy_handle(unity::mod_handle* pmh);

    typedef s_long (*PFcopy_handle)(const unity::mod_handle* pmhsrc, unity::mod_handle* pmhdest);
    static s_long Fcopy_handle(const unity::mod_handle* pmhsrc, unity::mod_handle* pmhdest);

    typedef s_long (*PFinit_by_pmsm)(unity_common::f_ls_modsm pmsm, s_long b_au, unity::mod_handle* pmhdest);
    static s_long Finit_by_pmsm(unity_common::f_ls_modsm pmsm, s_long b_au, unity::mod_handle* pmhdest);

    typedef void* (*PFsym)(const unity::mod_handle* pmh, const char* name);
    static void* Fsym(const unity::mod_handle* pmh, const char* name);

    typedef s_long (*PFrequest)(const unity::mod_handle* pmh, const unity* para, unity* retval);
    static s_long Frequest(const unity::mod_handle* pmh, const unity* para, unity* retval);
  };  
};

struct cv_ff_pcos
{
  typedef unity_common t_uc;
  typedef unity_common::f_ls_modsm t_msm;
  static s_long Fcons(cref_t<unity::_pcos_d_base>* pd, const unity::mod_handle* ph);
  static s_long Fcons_rl(cref_t<unity::_pcos_d_base>* pd, const unity::mod_handle* ph);
  static s_long Foppar(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, cref_t<unity>* prv);
  static s_long Fsetref(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, cref_t<unity>* pxr);
  static s_long Fsetmv(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, unity* pxm, const unity::mod_handle* phx);
  static s_long Fsetcp(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, const unity* pxc, const unity::mod_handle* phx);
  static s_long Fclearstg(const cref_t<unity::_pcos_d_base>* pd);
  static s_long Fmakemv(const cref_t<unity::_pcos_d_base>* pd, unity* pxm, const unity::mod_handle* phx, cref_t<unity>* prv);
  static s_long Fmakecp(const cref_t<unity::_pcos_d_base>* pd, const unity* pxc, const unity::mod_handle* phx, cref_t<unity>* prv);
  static s_long Fcheckref(const cref_t<unity::_pcos_d_base>* pd, cref_t<unity>* pxr);

  static s_long Fmakepcoscref(const unity::mod_handle* phx, cref_t<unity>* prv);
  static s_long Fmakepcoscrefcp(const unity::mod_handle* phx, const unity* psrc, cref_t<unity>* prv);

  typedef s_long (*f_makepcoscref)(const unity::mod_handle* phx, cref_t<unity>* prv);
  typedef s_long (*f_makepcoscrefcp)(const unity::mod_handle* phx, const unity* psrc, cref_t<unity>* prv);

  typedef cv_ff::cv_recreate::PF f_recreate;
};

std::string cv_ff::vecm_strdim(const vecm& x) { try { std::string s; s += "["; s += ntocs(x.nbase()); s += ".."; s += ntocs(x.nbase() + x.n() - 1); s += "]"; return s; } catch (...) {} return std::string(); }

template<int utt_, class _> struct cv_ff::cv_delete::_del_t  {    static s_long F(void * p, s_long uttf) {
    if (!p) { return 1; }
    try { delete reinterpret_cast<typename unity::valtype_t<utt_>::t*>(p); return 2; } catch (...) { return 0; }
} };
template<class _> struct cv_ff::cv_delete::_del_t<utEmpty, _> { static s_long F(void * p, s_long uttf) { return 1; } };
template<class _> struct cv_ff::cv_delete::_del_t<utObject, _> { static s_long F(void * p, s_long uttf) {
  if (!p) { return 1; }
  unity::o_api a(p, !!(uttf & unity::xfObjItfsList));
  s_long res =  a._ref_del((uttf & unity::xfObjStrongRef) ? 1 : 2);
    if (res > 0) { return 2; }
  return 0;
} };
template<class _> struct cv_ff::cv_delete::_del_t<utMap, _> { static s_long F(void * p, s_long uttf) { if (!p) { return 1; } try { delete reinterpret_cast<unity::t_map_tu*>(p); return 2; } catch (...) { return 0; } } };
template<class _> struct cv_ff::cv_delete::_del_t<utHash, _> { static s_long F(void * p, s_long uttf) { if (!p) { return 1; } try { delete static_cast<_unity_hl_impl*>((unity::_hl_i*)p); return 2; } catch (...) { return 0; } } };

void cv_ff::cv_clear::Lclear(unity* x) { ((PF)x->pmsm(unity_common::msm_cv_clear))(x); }
s_long cv_ff::cv_array::Llb_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Flb_u(pv); } return ((PFlb_u)pv->pmsm(unity_common::msm_arr_lb_u))(pv); }
s_long cv_ff::cv_array::Lub_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Fub_u(pv); } return ((PFub_u)pv->pmsm(unity_common::msm_arr_ub_u))(pv); }
s_long cv_ff::cv_array::Lsz_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Fsz_u(pv); } return ((PFsz_u)pv->pmsm(unity_common::msm_arr_sz_u))(pv); }
s_long cv_ff::cv_array::Lua_ensure_ind(unity* pv, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fua_ensure_ind(pv, ind); } return ((PFua_ensure_ind)pv->pmsm(unity_common::msm_ua_ensure_ind))(pv, ind); }
s_long cv_ff::cv_array::Lel_get(const unity* pv, unity* pst, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_get(pv, pst, ind); } return ((PFel_get)pv->pmsm(unity_common::msm_arr_el_get))(pv, pst, ind); }
s_long cv_ff::cv_array::Lel_set(unity* pv, const unity* px, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_set(pv, px, ind); } return ((PFel_set)pv->pmsm(unity_common::msm_arr_el_set))(pv, px, ind); }
s_long cv_ff::cv_array::Lel_append(unity* pv, const unity* px) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_append(pv, px); } return ((PFel_append)pv->pmsm(unity_common::msm_arr_el_append))(pv, px); }
s_long cv_ff::cv_array::Lsz_set(unity* pv, s_long lbnd, s_long size, s_long flags) { if (pv->pmsm == unity_common::ls_modsm) { return Fsz_set(pv, lbnd, size, flags); } return ((PFsz_set)pv->pmsm(unity_common::msm_arr_sz_set))(pv, lbnd, size, flags); }
s_long cv_ff::cv_array::Lua_fill(unity* pv, const unity* px, s_long utt) { if (pv->pmsm == unity_common::ls_modsm) { return Fua_fill(pv, px, utt); } return ((PFua_fill)pv->pmsm(unity_common::msm_ua_fill))(pv, px, utt); }
s_long cv_ff::cv_array::Lel_insrem(unity* pv, const unity* px, s_long ind, s_long m) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_insrem(pv, px, ind, m); } return ((PFel_insrem)pv->pmsm(unity_common::msm_arr_el_insrem))(pv, px, ind, m); }
std::wstring* cv_ff::cv_wstring::Lnew(void* (*pmsm)(s_long), meta::s_ll n, const void* p, s_long wcsx, s_long flags) { if (pmsm == unity_common::ls_modsm) { return Fnew(n, p, wcsx, flags); } return ((PFnew)pmsm(unity_common::msm_wstring_new))(n, p, wcsx, flags); }
meta::s_ll cv_ff::cv_wstring::Ln(void* (*pmsm)(s_long), const void* ps) { if (pmsm == unity_common::ls_modsm) { return Fn(ps); } return ((PFn)pmsm(unity_common::msm_wstring_n))(ps); }
const void* cv_ff::cv_wstring::Lp0(void* (*pmsm)(s_long), const void* ps) { if (pmsm == unity_common::ls_modsm) { return Fp0(ps); } return ((PFp0)pmsm(unity_common::msm_wstring_p0))(ps); }
s_long cv_ff::cv_wstring::Lwcs(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Fwcs(); } return ((PFwcs)pmsm(unity_common::msm_wstring_wcs))(); }
s_long cv_ff::cv_wstring::Lti(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Fti(); } return ((unity::__Pws_ti)pmsm(unity_common::msm_wstring_ti))(); }
s_long cv_ff::cv_delete::Ldelete(void* (*pmsm)(s_long), void* p, s_long uttf) { if (pmsm == unity_common::ls_modsm) { return Fdelete(p, uttf); } return ((PF)pmsm(unity_common::msm_cv_delete))(p, uttf); }
s_long cv_ff::cv_create::Lcreate(void* (*pmsm)(s_long), unity::_stg_u* pdest, s_long uttf, s_long flags) { if (pmsm == unity_common::ls_modsm) { return Fcreate(pdest, uttf, flags); } return ((PFcreate)pmsm(unity_common::msm_cv_create))(pdest, uttf, flags); }
s_long cv_ff::cv_create::Ldestroy(void* (*pmsm)(s_long), unity::_stg_u* pdest, s_long uttf, s_long flags) { if (pmsm == unity_common::ls_modsm) { return Fdestroy(pdest, uttf, flags); } return ((PFdestroy)pmsm(unity_common::msm_cv_destroy))(pdest, uttf, flags); }
unity* cv_ff::cv_create::Lunity_new(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Funity_new(); } return ((PFunity_new)pmsm(unity_common::msm_cv_unity_new))(); }
void cv_ff::cv_convert::Lcnv_this(unity* p, EConvStrength cs, s_long utt_dest, s_long keep_name)
{
  s_long res(0);
  if (p->pmsm == unity_common::ls_modsm) { res = Fcnv_this(p, cs, utt_dest, keep_name); } else { res = ((PFcnv_this)p->pmsm(unity_common::msm_cv_convert))(p, cs, utt_dest, keep_name); }
  if (res >= 0 || cs == csLazy) { return; }
  std::string e("Lcnv_this.A");
  e[e.size() - 1] = char(s_long('A') + (-res % 26));
  throw XUExec(e);
}
void cv_ff::cv_convert::Lasg(unity* p, const unity* px, s_long flags)
{
  s_long res(0);
  if (p->pmsm == unity_common::ls_modsm) { res = Fasg(p, px, flags); } else { res = ((PFasg)p->pmsm(unity_common::msm_cv_assign))(p, px, flags); }
  if (res >= 0) { return; }
  _fls75 e("Lasg.A");
  e[e.length() - 1] = char(s_long('A') + (-res % 26));
  throw XUExec(e);
}
s_long cv_ff::u_set_name::Lset_name(unity* p, const unity* pn) { return ((PF)p->pmsm(unity_common::msm_u_set_name))(p, pn); }
s_long cv_ff::u_clear::Lu_clear(unity* p, s_long flags) { return ((PF)p->pmsm(unity_common::msm_u_clear))(p, flags); }
void* cv_ff::cv_obj2::Litfslist_cr(unity_common::__Psm pmsm) { if (pmsm == unity_common::ls_modsm) { return Fitfslist_cr(); } return ((PFitfslist_cr)pmsm(unity_common::msm_obj2_itfslist_cr))(); }
s_long cv_ff::cv_obj2::Litfslist_del(unity_common::__Psm pmsm, void* p) { if (pmsm == unity_common::ls_modsm) { return Fitfslist_del(p); } return ((PFitfslist_del)pmsm(unity_common::msm_obj2_itfslist_del))(p); }

void cv_ff::cv_reg::Freg(unity* px, s_long flags) {} // not impl.
void cv_ff::cv_clear::Fclear(unity* x) { x->clear(); }
s_long cv_ff::cv_recreate::Frecreate(unity* x) { if (!x) { return 0; } x->recreate(); return 1; }

s_long cv_ff::cv_array::Flb_u(const unity* pv) { return pv->_drf_q()->nbase(); }
s_long cv_ff::cv_array::Fub_u(const unity* pv) { return pv->_drf_q()->nbase() + pv->_drf_q()->n() - 1; }
s_long cv_ff::cv_array::Fsz_u(const unity* pv) { return pv->_drf_q()->n(); }
s_long cv_ff::cv_array::Fua_ensure_ind(unity* pv, s_long ind)
{
  s_long n = pv->_drf_q()->n(); s_long nb = pv->_drf_q()->nbase();
  ind -= nb;
  if (ind < 0)
  {
      // Insert diff., nbase -= diff.
    if (cv_ff::cv_array::Fel_insrem(pv, 0, nb, -ind) <= 0) { return -1; }
    cv_ff::cv_array::Fsz_set(pv, nb + ind, 0, 0x1); // succeeds
    return 1;
  }
  else if (ind >= n)
  {
      // Append diff.
    if (cv_ff::cv_array::Lsz_set(pv, 0, ind + 1, 0x2) <= 0) { return -2; }
    return 1;
  }
  return 0;
}
s_long cv_ff::cv_array::Fel_get(const unity* pv, unity* pst, s_long ind)
{
  if (!(pv && pst)) { return -2; }
  try {
    switch (pv->utype())
    {
      case utIntArray: { enum { utt = utInt }; new (pst) unity(pv->ref<utt>(ind)); break; }
      case utFloatArray: { enum { utt = utFloat }; new (pst) unity(pv->ref<utt>(ind)); break; }
      case utDateArray: { enum { utt = utDate }; new (pst) unity(pv->ref<utt>(ind)); break; }
      case utCharArray: { enum { utt = utChar }; new (pst) unity(pv->ref<utt>(ind)); break; }
      case utStringArray: { enum { utt = utString }; new (pst) unity(pv->ref<utt>(ind)); break; }
      case utUnityArray: { enum { utt = utUnity }; new (pst) unity(pv->ref<utt>(ind)); break; }
      default: return -2;
    }
  } catch (...) { return -1; }
  return 1;
}
s_long cv_ff::cv_array::Fel_set(unity* pv, const unity* px, s_long ind)
{
  if (!(pv && px)) { return -2; }
  try {
    switch (pv->utype())
    {
      case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
      case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
      case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
      case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
      case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
      case utUnityArray: { enum { utt = utUnity }; pv->ref<utt>(ind) = *px; break; }
      default: return -2;
    }
  } catch (...) { return -1; }
  return 1;
}
s_long cv_ff::cv_array::Fel_append(unity* pv, const unity* px)
{
  if (!(pv && px)) { return -2; }
  try {
    switch (pv->utype())
    {
      case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
      case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
      case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
      case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
      case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
      case utUnityArray: { enum { utt = utUnity }; typedef unity::valtype_t<utt | utArray>::t_tu Ttu; Ttu& r = (Ttu&)pv->ref<utt | utArray>(); r.push_back(*px); break; }
      default: return -2;
    }
  } catch (...) { return -1; }
  return 1;
}
s_long cv_ff::cv_array::Fsz_set(unity* pv, s_long lbnd, s_long sx, s_long flags)
{
  if (!pv) { return -2; }
  try {
    switch (pv->utype())
    {
      case utIntArray: { enum { utt = utInt }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      case utFloatArray: { enum { utt = utFloat }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      case utDateArray: { enum { utt = utDate }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      case utCharArray: { enum { utt = utChar }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      case utStringArray: { enum { utt = utString }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      case utUnityArray: { enum { utt = utUnity }; typedef unity::valtype_t<utt | utArray>::t_tu Ttu; Ttu& r = (Ttu&)pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
      default: return -2;
    }
  } catch (...) { return -1; }
  return 1;
}
s_long cv_ff::cv_array::Fua_fill(unity* pv, const unity* px, s_long utt)
{
  if (pv->pmsm != unity_common::ls_modsm) { return -1; }
  utt |= utArray;

  if (!pv->isArray()) { try { pv->u_clear(utt); return 1; } catch (...) {} return -2; }

  s_long n = pv->_drf_q()->n(); s_long nb = pv->_drf_q()->nbase();
  if (pv->utype() == utt &&  n == 0) { return 0; }
  unity z0; z0.u_clear(utt);
  if (n && cv_ff::cv_array::Fel_insrem(&z0, px, 1, n) <= 0) {

  return -3; }
  if (nb != 1) { cv_ff::cv_array::Fsz_set(&z0, nb, 0, 0x1); } // succeeds
    void* _pn = pv->_data.p2; if (_pn) { pv->_data.p2 = 0; }
  cv_ff::cv_create::Fdestroy(&pv->_data, pv->_uttf(), 0x4);
  pv->_data.p1 = z0._data.p1; pv->ut = z0.ut;
    if (_pn) { pv->_data.p2 = _pn; }
  z0._data.p1 = 0; z0.ut = utEmpty;
  return 1;
}
s_long cv_ff::cv_array::Fel_insrem(unity* pv, const unity* px, s_long ind, s_long m)
{
  if (!pv) { return -2; }
  if (m == 0) { return 1; }
  s_long res;
  try {
    switch (pv->utype())
    {
      case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      case utUnityArray: { enum { utt = utUnity }; typedef unity::valtype_t<utt | utArray>::t_tu Ttu; Ttu& rv = (Ttu&)pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, px ? *px : unity()); } else { res = rv.el_insert_ml(ind, m, px ? *px : unity()); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
      default: return -2;
    }
  } catch (...) { return -1; }
  return res >= 0 ? 1 : -1;
}

std::wstring* cv_ff::cv_wstring::Fnew(meta::s_ll n, const void* p, s_long wcsx, s_long flags)
{
  if (wcsx == Fwcs()) { return new (std::nothrow) std::wstring(reinterpret_cast<const wchar_t*>(p), _t_wz(n)); }
  std::wstring* ps = new (std::nothrow) std::wstring;
  if (Fcopy(ps, n, p, wcsx, flags)) { return ps; }
  try { delete ps; } catch (...) {} return 0;
}
bool cv_ff::cv_wstring::Fcopy(std::wstring* ps, meta::s_ll n, const void* p, s_long wcsx, s_long flags)
{
  if (!ps) { return false; }
  bool b = false;
  try
  {
    const char* pc = reinterpret_cast<const char*>(p);
    ps->reserve(_t_wz(n));
    if (wcsx == 1) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(*pc); pc += 1; } b = true; }
      else if (wcsx == 2) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 2; } b = true; }
      else if (wcsx == 0x102) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[1]) + (s_long(pc[0]) << 8)); pc += 2; } b = true; }
      else if (wcsx == 4) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 4; } b = true; }
      else if (wcsx == 0x104) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[3]) + (s_long(pc[2]) << 8)); pc += 4; } b = true; }
  } catch (...) {}
  return b;
}
meta::s_ll cv_ff::cv_wstring::Fn(const void* ps) { return reinterpret_cast<const std::wstring*>(ps)->size(); }
const void* cv_ff::cv_wstring::Fp0(const void* ps) { static const wchar_t* pc0 = L""; const std::wstring& r = *reinterpret_cast<const std::wstring*>(ps); return r.size() ? &r[0] : pc0; }
s_long cv_ff::cv_wstring::Fwcs() { s_long x1 = 0x201; s_long x2 = s_long(((char*)&x1)[0]) + (s_long(((char*)&x1)[1]) << 8); return sizeof(wchar_t) | (x1 == x2 || sizeof(wchar_t) == 1 ? 0 : 1) << 8; }
s_long cv_ff::cv_wstring::Fti() { return yk_c::typer<std::wstring, __vecm_tu_selector>().t_ind; }

s_long cv_ff::cv_delete::Fdelete(void* p, s_long uttf)
{
  s_long i1 = (uttf & utMaskNoArray) + (uttf & utArray ? _ut_end : 0) ;
  static PFdel del_table[2 * _ut_end] = {
    _del_t<utEmpty>::F, _del_t<utInt>::F, _del_t<utFloat>::F, _del_t<utDate>::F, _del_t<utChar>::F, _del_t<utString>::F, _del_t<utObject>::F, _del_t<utUnity>::F, _del_t<utMap>::F, _del_t<utHash>::F,
    0, _del_t<utIntArray>::F, _del_t<utFloatArray>::F, _del_t<utDateArray>::F, _del_t<utCharArray>::F, _del_t<utStringArray>::F, 0, _del_t<utUnityArray>::F, 0, 0,
  };
  if (i1 >= 2 * _ut_end) { return -1; }
  PFdel pf = reinterpret_cast<PFdel>(del_table[i1]); if (!pf) { return -1; }
  return pf(p, uttf);
}

s_long cv_ff::cv_create::Fcreate(unity::_stg_u* pdest, s_long uttf, s_long flags)
{
  if (uttf & utArray)
  {
    pdest->p2 = 0;
    if (!(uttf & unity::xfPtr)) { pdest->p1 = 0; return -1; }
    switch (uttf & unity::xfmUtype)
    {
      case utIntArray: pdest->p1 = new (std::nothrow) vec2_t<meta::s_ll, __vecm_tu_selector>(1, 0, iterator_t<meta::s_ll, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      case utFloatArray: pdest->p1 = new (std::nothrow) vec2_t<double, __vecm_tu_selector>(1, 0, iterator_t<double, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      case utDateArray: pdest->p1 = new (std::nothrow) vec2_t<_unitydate, __vecm_tu_selector>(1, 0, iterator_t<_unitydate, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      case utCharArray: pdest->p1 = new (std::nothrow) vec2_t<_unitychar, __vecm_tu_selector>(1, 0, iterator_t<_unitychar, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      case utStringArray: pdest->p1 = new (std::nothrow) vec2_t<std::wstring, __vecm_tu_selector>(1, 0, iterator_t<std::wstring, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      case utUnityArray: pdest->p1 = new (std::nothrow) vec2_t<unity, __vecm_tu_selector>(1, 0, iterator_t<unity, true, __vecm_tu_selector>()); return pdest->p1 != 0;
      default: pdest->p1 = 0; return -1;
    }
  }
  else
  {
    if (uttf & unity::xfPtr)
    {
      pdest->p2 = 0;
      switch (uttf  & unity::xfmUtype)
      {
        case utEmpty: pdest->p1 = 0; return 1;
        case utInt: pdest->p1 = new (std::nothrow) meta::s_ll(); return pdest->p1 != 0;
        case utChar: pdest->p1 = new (std::nothrow) _unitychar(); return pdest->p1 != 0;
        case utFloat: pdest->p1 = new (std::nothrow) double(); return pdest->p1 != 0;
        case utDate: pdest->p1 = new (std::nothrow) _unitydate(); return pdest->p1 != 0;
        case utString: pdest->p1 = new (std::nothrow) std::wstring; return pdest->p1 != 0;
        case utMap: pdest->p1 = new (std::nothrow) unity::t_map_tu; return pdest->p1 != 0;
        case utHash: { unity::_hl_i* p = new (std::nothrow) _unity_hl_impl;  pdest->p1 = p; return pdest->p1 != 0; }
        default: pdest->p1 = 0; return -1;
      }
    }
    else
    {
      switch (uttf & unity::xfmUtype)
      {
        case utEmpty: pdest->p1 = 0; pdest->p2 = 0; return 1;
        case utInt: new ((meta::s_ll*)pdest) meta::s_ll(); return 1;
        case utChar: pdest->p1 = 0; pdest->p2 = 0; new ((char*)pdest) _unitychar(); return 1;
        case utFloat: new ((double*)pdest) double(); return 1;
        case utDate: new ((_unitydate*)pdest) _unitydate(); return 1;
        default: pdest->p1 = 0; pdest->p2 = 0; return -1;
      }
    }
  }
}
s_long cv_ff::cv_create::Fdestroy(unity::_stg_u* pdest, s_long uttf, s_long flags)
{
  if (!pdest) { return -1; }
  if (!(uttf & unity::xfPtr))
  {
    s_long utt = uttf & unity::xfmUtype;
    if (utt >= utEmpty && utt < utString) { pdest->p1 = 0; pdest->p2 = 0; return 1; }
    if ((flags & 0x4)) { pdest->p1 = 0; pdest->p2 = 0; }
    return -1;
  }
  else
  {
    s_long res = 0, res2 = 0;
    if (pdest->p1) { res = cv_delete::Fdelete(pdest->p1, uttf); if (res >= 0) { pdest->p1 = 0; res = res > 0 ? 0x1 : 0x2; } else { res = -1; } }
    if (pdest->p2) { res2 = cv_delete::Fdelete(pdest->p2, utUnity); if (res2 >= 0) { pdest->p2 = 0; res2 = res2 > 0 ? 0x4 : 0x8; } else { res2 = -1; } }
    if (res >= 0 && res2 >= 0) { return res | res2; }
    if ((flags & 0x4)) { pdest->p1 = 0; pdest->p2 = 0; }
    return -1;
  }
}
unity* cv_ff::cv_create::Funity_new() { return new (std::nothrow) unity; }

s_long cv_ff::u_set_name::Fset_name(unity* p, const unity* pn)
{
  if (pn->isEmpty()) // remove the current name
  {
    if (!p->isNamed()) { return 1; }
    unity*& pn2 = p->_drf_name(); try { delete pn2; } catch (...) {} pn2 = 0;
    return 1;
  }
  try
  {
    _pdyn_t<unity> pn2(new unity(pn->isScalar() ? *pn : pn->u_key()));
    if (p->isNamed()) { try { delete p->_drf_name(); } catch (...) {} p->_data.p2 = pn2.take(); return 2; }
    else
    {
      if (p->isByPtr()) { p->_data.p2 = pn2.take(); return 2; }
      unity temp; if (cv_ff::cv_convert::Fasg(&temp, p, 0x2) < 0) { return 0; }
      p->_data.p1 = temp._data.p1;
        temp._data.p1 = 0; temp.ut = utEmpty;
        p->_data.p2 = pn2.take(); p->ut |= unity::xfPtr;
      return 2;
    }
  }
  catch (...) { return 0; }
}

void* cv_ff::cv_obj2::Fitfslist_cr() { try { return new _o_itfslist(); } catch (...) {} return 0; }
s_long cv_ff::cv_obj2::Fitfslist_del(void* p) { if (!p) { return 0; } try { delete (_o_itfslist*)p; return 1; } catch (...) {} return -1; }

template<class T, class C = void, int Fi = 0, class Fs = void, meta::s_ll Lcp_mcs = 50, s_long Lto_ms = -1, class _ = __vecm_tu_selector>
struct _stg_tu_t
{
  enum { nst = (sizeof(T) + sizeof(meta::s_ll) - 1) / sizeof(meta::s_ll) }; typedef meta::s_ll t_stg[nst];
  struct t_storage { t_stg stg; char init; };
    static t_storage _storage;
  struct t_deinit { ~t_deinit() { critsec_t<_stg_tu_t> __lock(Lcp_mcs, Lto_ms); if ((__lock.is_locked() || Lto_ms < 0)  && _storage.init == 1) { try { ((T*)&_storage.stg[0])->~T(); } catch (...) {} _storage.init = 2; } } };
    static t_deinit _deinit;
};
template<class T, class C, int Fi, class Fs, meta::s_ll Lcp_mcs, s_long Lto_ms, class _> typename _stg_tu_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::t_storage _stg_tu_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::_storage = { { 0 }, 0 };
template<class T, class C, int Fi, class Fs, meta::s_ll Lcp_mcs, s_long Lto_ms, class _> typename _stg_tu_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::t_deinit _stg_tu_t<T,C,Fi,Fs,Lcp_mcs,Lto_ms,_>::_deinit;

void* cv_ff::cv_hstst::Fregstg(const char* tname, void* pstg_offer)
{
  if (!tname) { return 0; }
  typedef hashx<const char*, void*, hashx_common::kf_str<const char*, __vecm_tu_selector> > t_hash;
  typedef _stg_tu_t<t_hash, cv_ff::cv_hstst> V;
  typedef critsec_t<V> L;
  L __lock(50, -1);
  if (V::_storage.init == 2 || !__lock.is_locked()) { return 0; }
  t_hash& rh = *(t_hash*)(&V::_storage.stg[0]);
  if (V::_storage.init == 0) { new (&rh) t_hash(); V::_storage.init = 1; }
  if (pstg_offer)
  {
    const t_hash::entry* e = 0;
    s_long res = rh.insert(tname, &e);
    if (res == 1) { e->v = pstg_offer; return e->v; }
    if (res == 0) { return e->v; }
    return 0;
  }
  else
  {
    const t_hash::entry* e = rh.find(tname);
    if (e) { return e->v; }
    return 0;
  }
}

s_long cv_ff::cv_ver::Fver_map() { return 0x101; } // &0xff part may differ for compatible versions (same ordhs_t data structure and functioning)
s_long cv_ff::cv_ver::Fver_hl() { return 0x101; } // &0xff part may differ for compatible versions (same _unity_hl_impl data layout and _hl_i calls work correctly)

  // offer - pointer to ls_modsm of DLL which is root DLL loader for the calling module.
  // ret_rootmodsm - factually assigned root loader for this module.
  //  Root loader module:
  //    a) executable - not unloaded until program exit.
  //    b) a .dll (.so), loaded in other way than via unity::mod.
  //      It won't be unloaded if referenced from any mod_handle, by its destructor.
  //      Though, its unloading in other way should be prevented by the client.
  // The function sets the first given non-0 offer as root loader (pass offer == 0 to query the current state),
  //    so that all mod_handle functions are executed in root loader, not in the current module.
  //    This allows for free using mod_handle objects even if the (non-root) module that created them is already unloaded.
  // Returns:
  //  flags:
  //    0x1 - rootldr exists (if ret_rootmodsm != 0, *ret_rootmodsm will be set to that pointer).
  //    0x2 - rootldr is just assigned with the current call.
  //    0x4 - the current module is rootldr (i.e. *ret_rootmodsm == the current unity_common::ls_modsm,
  //      so the module should not be unloaded until program exits).
  //    NOTE If offer != 0, the returned value anyway != 0.
s_long cv_ff::cv_rootldr::Frootldr(unity_common::__Psm offer, unity_common::__Psm* ret_rootmodsm)
{
  critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
  s_long f = 0;
  if (!_rootmodsm) { if (offer) { _rootmodsm = offer; f |= 2; } }
  if (_rootmodsm) { f |= 1; }
  if (_rootmodsm == unity_common::ls_modsm) { f |= 4; }
  if (ret_rootmodsm) { *ret_rootmodsm = _rootmodsm; }
  return f;
}

typedef hashx<std::string, unity_common::__Psm> t_hipsm;
unity_common::__Psm _ls_psm_find(const char* piname) throw()
{
  if (!piname) { return 0; }
  svf_m_t<t_hipsm, unity_common>::L __lock; if (!__lock.b_valid()) { return 0; }
      svf_m_t<t_hipsm, unity_common> rsth; if (rsth.b_noinit()) { rsth.init0(1); }
      t_hipsm* phi = rsth.px(); if (!phi) { return 0; }
  const hashx<std::string, unity_common::__Psm>::entry* e = phi->find(piname);
  unity_common::__Psm pf = e ? e->v : 0;
  return pf;
}
void unity_common::_ls_psm_set(const char* piname, unity_common::__Psm pf) throw()
{
  if (!piname) { return; }
  svf_m_t<t_hipsm, unity_common>::L __lock; if (!__lock.b_valid()) { return; }
      svf_m_t<t_hipsm, unity_common> rsth; if (rsth.b_noinit()) { rsth.init0(1); }
      t_hipsm* phi = rsth.px(); if (!phi) { return; }
  try { phi->opsub(piname) = pf; } catch (...) {}
}
void* _ls_obj_pi(void*, const char*) throw() { return 0; }
void* _ls_obj_cpvoid(void*, s_long) throw() { return 0; }
unity* _ls_obj_punity(void*) throw() { return 0; }
void* _ls_obj_prefo(void*, const char*) throw() { return 0; }

void* unity_common::ls_modsm(s_long ind)
{
  enum { size = unity_common::sizeof_msm };
  static void* smt[size] =
  {
    (void*)_ls_psm_find,
    (void*)_ls_obj_pi,
    (void*)_ls_obj_cpvoid,
    (void*)_ls_obj_punity,
    (void*)cv_ff::cv_reg::Freg,
    (void*)cv_ff::cv_clear::Fclear,
    (void*)cv_ff::cv_create::Fcreate,
    (void*)cv_ff::cv_create::Fdestroy,
    (void*)cv_ff::cv_create::Funity_new,
    (void*)cv_ff::cv_delete::Fdelete,
    (void*)cv_ff::cv_convert::Fcnv_this,
    (void*)cv_ff::cv_convert::Fasg,
    (void*)cv_ff::cv_wstring::Fnew,
    (void*)cv_ff::cv_wstring::Fn,
    (void*)cv_ff::cv_wstring::Fp0,
    (void*)cv_ff::cv_wstring::Fwcs,
    (void*)cv_ff::cv_wstring::Fti,
    (void*)cv_ff::cv_array::Fel_get,
    (void*)cv_ff::cv_array::Fel_set,
    (void*)cv_ff::cv_array::Fel_append,
    (void*)cv_ff::cv_array::Fel_insrem,
    (void*)cv_ff::cv_array::Fsz_set,
    (void*)cv_ff::cv_array::Flb_u,
    (void*)cv_ff::cv_array::Fub_u,
    (void*)cv_ff::cv_array::Fsz_u,
    (void*)cv_ff::u_set_name::Fset_name,
    (void*)cv_ff::u_clear::Fu_clear,
    (void*)cv_ff::cv_array::Fua_fill,
    (void*)cv_ff::cv_array::Fua_ensure_ind,
    (void*)_ls_obj_prefo,
    (void*)cv_ff::cv_obj2::Fitfslist_cr,
    (void*)cv_ff::cv_obj2::Fitfslist_del,
    (void*)cv_ff::cv_hstst::Fregstg,
    (void*)cv_ff::cv_ver::Fver_map,
    (void*)cv_ff::cv_ver::Fver_hl,
    (void*)cv_ff::cv_rootldr::Frootldr,
    (void*)cv_ff::cv_rootldr::Finit_handle,
    (void*)cv_ff::cv_rootldr::Fdestroy_handle,
    (void*)cv_ff::cv_rootldr::Fcopy_handle,
    (void*)cv_ff::cv_rootldr::Fsym,
    (void*)cv_ff::cv_rootldr::Frequest,
    (void*)cv_ff_pcos::Fcons,
    (void*)cv_ff_pcos::Fcons_rl,
    (void*)cv_ff_pcos::Foppar,
    (void*)cv_ff_pcos::Fsetref,
    (void*)cv_ff_pcos::Fsetmv,
    (void*)cv_ff_pcos::Fsetcp,
    (void*)cv_ff_pcos::Fclearstg,
    (void*)cv_ff_pcos::Fmakemv,
    (void*)cv_ff_pcos::Fmakecp,
    (void*)cv_ff_pcos::Fmakepcoscref,
    (void*)cv_ff_pcos::Fmakepcoscrefcp,
    (void*)cv_ff::cv_recreate::Frecreate,
    (void*)cv_ff::cv_rootldr::Finit_by_pmsm,
    (void*)cv_ff_pcos::Fcheckref,
  };

  return ind >= 0 && ind < size ? smt[ind] : 0;
}
unity_common::f_ls_modsm unity_common::pls_modsm() { return &ls_modsm; }

struct _static_conv
{
  static void cnv_all(EConvStrength cs, s_long uttf_src, s_long utt_dest, s_long flags, const void* psrc, void* pdest);
  static void cnv_incompat(EConvStrength cs, s_long utt_dest, s_long flags, const unity& usrc, void* pdest);
  static void conv_Empty_Int(s_long fc, meta::s_ll& retval);
  static void conv_Empty_Float(s_long fc, double& retval);
  static void conv_Empty_Date(s_long fc, _unitydate& retval);
  static void conv_Empty_String(s_long fc, std::wstring& retval);
  static void conv_Empty_Char(s_long fc, _unitychar& retval);
  static void conv_Empty_Unity(s_long fc, unity& retval);
  static void conv_Empty_Map(s_long fc, unity::t_map_tu& retval);
  static void conv_Empty_Hash(s_long fc, unity::t_hash& retval);
  static void conv_Empty_UnityArray(s_long fc, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Empty_IntArray(s_long fc, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Empty_FloatArray(s_long fc, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Empty_DateArray(s_long fc, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Empty_StringArray(s_long fc, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Empty_CharArray(s_long fc, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Empty_UnityVector(s_long fc, std::vector<unity>& retval);
  static void conv_Empty_IntVector(s_long fc, std::vector<meta::s_ll>& retval);
  static void conv_Empty_FloatVector(s_long fc, std::vector<double>& retval);
  static void conv_Empty_DateVector(s_long fc, std::vector<_unitydate>& retval);
  static void conv_Empty_StringVector(s_long fc, std::vector<std::wstring>& retval);
  static void conv_Empty_CharVector(s_long fc, std::vector<_unitychar>& retval);
  static void conv_Int_Int(s_long fc, const meta::s_ll& x, meta::s_ll& retval);
  static void conv_Int_Float(s_long fc, const meta::s_ll& x, double& retval);
  static void conv_Int_Date(s_long fc, const meta::s_ll& x, _unitydate& retval);
  static void conv_Int_String(s_long fc, const meta::s_ll& x, std::wstring& retval);
  static void conv_Int_Char(s_long fc, const meta::s_ll& x, _unitychar& retval);
  static void conv_Int_Unity(s_long fc, const meta::s_ll& x, unity& retval);
  static void conv_Int_UnityArray(s_long fc, const meta::s_ll& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Int_IntArray(s_long fc, const meta::s_ll& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Int_FloatArray(s_long fc, const meta::s_ll& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Int_DateArray(s_long fc, const meta::s_ll& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Int_StringArray(s_long fc, const meta::s_ll& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Int_CharArray(s_long fc, const meta::s_ll& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Int_UnityVector(s_long fc, const meta::s_ll& x, std::vector<unity>& retval);
  static void conv_Int_IntVector(s_long fc, const meta::s_ll& x, std::vector<meta::s_ll>& retval);
  static void conv_Int_FloatVector(s_long fc, const meta::s_ll& x, std::vector<double>& retval);
  static void conv_Int_DateVector(s_long fc, const meta::s_ll& x, std::vector<_unitydate>& retval);
  static void conv_Int_StringVector(s_long fc, const meta::s_ll& x, std::vector<std::wstring>& retval);
  static void conv_Int_CharVector(s_long fc, const meta::s_ll& x, std::vector<_unitychar>& retval);
  static void conv_Char_Int(s_long fc, const _unitychar& x, meta::s_ll& retval);
  static void conv_Char_Float(s_long fc, const _unitychar& x, double& retval);
  static void conv_Char_Date(s_long fc, const _unitychar& x, _unitydate& retval);
  static void conv_Char_String(s_long fc, const _unitychar& x, std::wstring& retval);
  static void conv_Char_Char(s_long fc, const _unitychar& x, _unitychar& retval);
  static void conv_Char_Unity(s_long fc, const _unitychar& x, unity& retval);
  static void conv_Char_UnityArray(s_long fc, const _unitychar& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Char_IntArray(s_long fc, const _unitychar& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Char_FloatArray(s_long fc, const _unitychar& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Char_DateArray(s_long fc, const _unitychar& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Char_StringArray(s_long fc, const _unitychar& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Char_CharArray(s_long fc, const _unitychar& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Char_UnityVector(s_long fc, const _unitychar& x, std::vector<unity>& retval);
  static void conv_Char_IntVector(s_long fc, const _unitychar& x, std::vector<meta::s_ll>& retval);
  static void conv_Char_FloatVector(s_long fc, const _unitychar& x, std::vector<double>& retval);
  static void conv_Char_DateVector(s_long fc, const _unitychar& x, std::vector<_unitydate>& retval);
  static void conv_Char_StringVector(s_long fc, const _unitychar& x, std::vector<std::wstring>& retval);
  static void conv_Char_CharVector(s_long fc, const _unitychar& x, std::vector<_unitychar>& retval);
  static void conv_Float_Int(s_long fc, const double& x, meta::s_ll& retval);
  static void conv_Float_Float(s_long fc, const double& x, double& retval);
  static void conv_Float_Date(s_long fc, const double& x, _unitydate& retval);
  static void conv_Float_String(s_long fc, const double& x, std::wstring& retval);
  static void conv_Float_Char(s_long fc, const double& x, _unitychar& retval);
  static void conv_Float_Unity(s_long fc, const double& x, unity& retval);
  static void conv_Float_UnityArray(s_long fc, const double& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Float_IntArray(s_long fc, const double& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Float_FloatArray(s_long fc, const double& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Float_DateArray(s_long fc, const double& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Float_StringArray(s_long fc, const double& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Float_CharArray(s_long fc, const double& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Float_UnityVector(s_long fc, const double& x, std::vector<unity>& retval);
  static void conv_Float_IntVector(s_long fc, const double& x, std::vector<meta::s_ll>& retval);
  static void conv_Float_FloatVector(s_long fc, const double& x, std::vector<double>& retval);
  static void conv_Float_DateVector(s_long fc, const double& x, std::vector<_unitydate>& retval);
  static void conv_Float_StringVector(s_long fc, const double& x, std::vector<std::wstring>& retval);
  static void conv_Float_CharVector(s_long fc, const double& x, std::vector<_unitychar>& retval);
  static void conv_Date_Int(s_long fc, const _unitydate& x, meta::s_ll& retval);
  static void conv_Date_Float(s_long fc, const _unitydate& x, double& retval);
  static void conv_Date_Date(s_long fc, const _unitydate& x, _unitydate& retval);
  static void conv_Date_String(s_long fc, const _unitydate& x, std::wstring& retval);
  static void conv_Date_Char(s_long fc, const _unitydate& x, _unitychar& retval);
  static void conv_Date_Unity(s_long fc, const _unitydate& x, unity& retval);
  static void conv_Date_UnityArray(s_long fc, const _unitydate& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Date_IntArray(s_long fc, const _unitydate& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Date_FloatArray(s_long fc, const _unitydate& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Date_DateArray(s_long fc, const _unitydate& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Date_StringArray(s_long fc, const _unitydate& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Date_CharArray(s_long fc, const _unitydate& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Date_UnityVector(s_long fc, const _unitydate& x, std::vector<unity>& retval);
  static void conv_Date_IntVector(s_long fc, const _unitydate& x, std::vector<meta::s_ll>& retval);
  static void conv_Date_FloatVector(s_long fc, const _unitydate& x, std::vector<double>& retval);
  static void conv_Date_DateVector(s_long fc, const _unitydate& x, std::vector<_unitydate>& retval);
  static void conv_Date_StringVector(s_long fc, const _unitydate& x, std::vector<std::wstring>& retval);
  static void conv_Date_CharVector(s_long fc, const _unitydate& x, std::vector<_unitychar>& retval);
  static void conv_String_Int(s_long fc, const std::wstring& x, meta::s_ll& retval);
  static void conv_String_Float(s_long fc, const std::wstring& x, double& retval);
    static bool conv_String_Date0(const std::wstring& x, _unitydate& retval, bool no_exc);
  static void conv_String_Date(s_long fc, const std::wstring& x, _unitydate& retval);
  static void conv_String_String(s_long fc, const std::wstring& x, std::wstring& retval);
  static void conv_String_Char(s_long fc, const std::wstring& x, _unitychar& retval);
  static void conv_String_Unity(s_long fc, const std::wstring& x, unity& retval);
  static void conv_String_UnityArray(s_long fc, const std::wstring& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_String_IntArray(s_long fc, const std::wstring& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_String_FloatArray(s_long fc, const std::wstring& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_String_DateArray(s_long fc, const std::wstring& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_String_StringArray(s_long fc, const std::wstring& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_String_CharArray(s_long fc, const std::wstring& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_String_UnityVector(s_long fc, const std::wstring& x, std::vector<unity>& retval);
  static void conv_String_IntVector(s_long fc, const std::wstring& x, std::vector<meta::s_ll>& retval);
  static void conv_String_FloatVector(s_long fc, const std::wstring& x, std::vector<double>& retval);
  static void conv_String_DateVector(s_long fc, const std::wstring& x, std::vector<_unitydate>& retval);
  static void conv_String_StringVector(s_long fc, const std::wstring& x, std::vector<std::wstring>& retval);
  static void conv_String_CharVector(s_long fc, const std::wstring& x, std::vector<_unitychar>& retval);
  static void conv_Object_Int(s_long fc, char& x0, meta::s_ll& retval);
  static void conv_Object_Float(s_long fc, char& x0, double& retval);
  static void conv_Object_Date(s_long fc, char& x0, _unitydate& retval);
  static void conv_Object_String(s_long fc, char& x0, std::wstring& retval);
  static void conv_Object_Char(s_long fc, char& x0, _unitychar& retval);
  static void conv_Object_Unity(s_long fc, char& x0, unity& retval);
  static void conv_Object_IntArray(s_long fc, char& x0, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Object_FloatArray(s_long fc, char& x0, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Object_DateArray(s_long fc, char& x0, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Object_StringArray(s_long fc, char& x0, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Object_CharArray(s_long fc, char& x0, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Object_IntVector(s_long fc, char& x0, std::vector<meta::s_ll>& retval);
  static void conv_Object_FloatVector(s_long fc, char& x0, std::vector<double>& retval);
  static void conv_Object_DateVector(s_long fc, char& x0, std::vector<_unitydate>& retval);
  static void conv_Object_StringVector(s_long fc, char& x0, std::vector<std::wstring>& retval);
  static void conv_Object_CharVector(s_long fc, char& x0, std::vector<_unitychar>& retval);
  static void conv_Unity_Int(s_long fc, const unity& x, meta::s_ll& retval);
  static void conv_Unity_Float(s_long fc, const unity& x, double& retval);
  static void conv_Unity_Date(s_long fc, const unity& x, _unitydate& retval);
  static void conv_Unity_String(s_long fc, const unity& x, std::wstring& retval);
  static void conv_Unity_Char(s_long fc, const unity& x, _unitychar& retval);
  static void conv_Unity_Unity(s_long fc, const unity& x, unity& retval);
  static void conv_Unity_UnityArray(s_long fc, const unity& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_Unity_IntArray(s_long fc, const unity& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_Unity_FloatArray(s_long fc, const unity& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_Unity_DateArray(s_long fc, const unity& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_Unity_StringArray(s_long fc, const unity& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_Unity_CharArray(s_long fc, const unity& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_Unity_UnityVector(s_long fc, const unity& x, std::vector<unity>& retval);
  static void conv_Unity_IntVector(s_long fc, const unity& x, std::vector<meta::s_ll>& retval);
  static void conv_Unity_FloatVector(s_long fc, const unity& x, std::vector<double>& retval);
  static void conv_Unity_DateVector(s_long fc, const unity& x, std::vector<_unitydate>& retval);
  static void conv_Unity_StringVector(s_long fc, const unity& x, std::vector<std::wstring>& retval);
  static void conv_Unity_CharVector(s_long fc, const unity& x, std::vector<_unitychar>& retval);
  static void conv_Map_Map(s_long fc, const unity::t_map& x, unity::t_map_tu& retval);
  static void conv_Map_Hash(s_long fc, const unity::t_map& x, unity::t_hash& retval);
  static void conv_Map_Unity(s_long fc, const unity::t_map& x, unity& retval);
  static void conv_Hash_Map(s_long fc, const unity::t_hash& x, unity::t_map_tu& retval);
  static void conv_Hash_Hash(s_long fc, const unity::t_hash& x, unity::t_hash& retval);
  static void conv_Hash_Unity(s_long fc, const unity::t_hash& x, unity& retval);
  static void conv_Map_String(s_long fc, const unity::t_map& x, std::wstring& retval);
  static void conv_Hash_String(s_long fc, const unity::t_hash& x, std::wstring& retval);
  static void conv_UnityArray_Int(s_long fc, const vec2_t<unity>& x, meta::s_ll& retval);
  static void conv_UnityArray_Float(s_long fc, const vec2_t<unity>& x, double& retval);
  static void conv_UnityArray_Date(s_long fc, const vec2_t<unity>& x, _unitydate& retval);
  static void conv_UnityArray_String(s_long fc, const vec2_t<unity>& x, std::wstring& retval);
  static void conv_UnityArray_Char(s_long fc, const vec2_t<unity>& x, _unitychar& retval);
  static void conv_UnityArray_Unity(s_long fc, const vec2_t<unity>& x, unity& retval);
  static void conv_UnityArray_UnityArray(s_long fc, const vec2_t<unity>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_UnityArray_IntArray(s_long fc, const vec2_t<unity>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_UnityArray_FloatArray(s_long fc, const vec2_t<unity>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_UnityArray_DateArray(s_long fc, const vec2_t<unity>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_UnityArray_StringArray(s_long fc, const vec2_t<unity>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_UnityArray_CharArray(s_long fc, const vec2_t<unity>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_UnityArray_UnityVector(s_long fc, const vec2_t<unity>& x, std::vector<unity>& retval);
  static void conv_UnityArray_IntVector(s_long fc, const vec2_t<unity>& x, std::vector<meta::s_ll>& retval);
  static void conv_UnityArray_FloatVector(s_long fc, const vec2_t<unity>& x, std::vector<double>& retval);
  static void conv_UnityArray_DateVector(s_long fc, const vec2_t<unity>& x, std::vector<_unitydate>& retval);
  static void conv_UnityArray_StringVector(s_long fc, const vec2_t<unity>& x, std::vector<std::wstring>& retval);
  static void conv_UnityArray_CharVector(s_long fc, const vec2_t<unity>& x, std::vector<_unitychar>& retval);
  static void conv_IntArray_Int(s_long fc, const vec2_t<meta::s_ll>& x, meta::s_ll& retval);
  static void conv_IntArray_Float(s_long fc, const vec2_t<meta::s_ll>& x, double& retval);
  static void conv_IntArray_Date(s_long fc, const vec2_t<meta::s_ll>& x, _unitydate& retval);
  static void conv_IntArray_String(s_long fc, const vec2_t<meta::s_ll>& x, std::wstring& retval);
  static void conv_IntArray_Char(s_long fc, const vec2_t<meta::s_ll>& x, _unitychar& retval);
  static void conv_IntArray_Unity(s_long fc, const vec2_t<meta::s_ll>& x, unity& retval);
  static void conv_IntArray_UnityArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_IntArray_IntArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_IntArray_FloatArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_IntArray_DateArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_IntArray_StringArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_IntArray_CharArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_IntArray_UnityVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<unity>& retval);
  static void conv_IntArray_IntVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<meta::s_ll>& retval);
  static void conv_IntArray_FloatVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<double>& retval);
  static void conv_IntArray_DateVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<_unitydate>& retval);
  static void conv_IntArray_StringVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<std::wstring>& retval);
  static void conv_IntArray_CharVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<_unitychar>& retval);
  static void conv_FloatArray_Int(s_long fc, const vec2_t<double>& x, meta::s_ll& retval);
  static void conv_FloatArray_Float(s_long fc, const vec2_t<double>& x, double& retval);
  static void conv_FloatArray_Date(s_long fc, const vec2_t<double>& x, _unitydate& retval);
  static void conv_FloatArray_String(s_long fc, const vec2_t<double>& x, std::wstring& retval);
  static void conv_FloatArray_Char(s_long fc, const vec2_t<double>& x, _unitychar& retval);
  static void conv_FloatArray_Unity(s_long fc, const vec2_t<double>& x, unity& retval);
  static void conv_FloatArray_UnityArray(s_long fc, const vec2_t<double>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_FloatArray_IntArray(s_long fc, const vec2_t<double>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_FloatArray_FloatArray(s_long fc, const vec2_t<double>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_FloatArray_DateArray(s_long fc, const vec2_t<double>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_FloatArray_StringArray(s_long fc, const vec2_t<double>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_FloatArray_CharArray(s_long fc, const vec2_t<double>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_FloatArray_UnityVector(s_long fc, const vec2_t<double>& x, std::vector<unity>& retval);
  static void conv_FloatArray_IntVector(s_long fc, const vec2_t<double>& x, std::vector<meta::s_ll>& retval);
  static void conv_FloatArray_FloatVector(s_long fc, const vec2_t<double>& x, std::vector<double>& retval);
  static void conv_FloatArray_DateVector(s_long fc, const vec2_t<double>& x, std::vector<_unitydate>& retval);
  static void conv_FloatArray_StringVector(s_long fc, const vec2_t<double>& x, std::vector<std::wstring>& retval);
  static void conv_FloatArray_CharVector(s_long fc, const vec2_t<double>& x, std::vector<_unitychar>& retval);
  static void conv_CharArray_Int(s_long fc, const vec2_t<_unitychar>& x, meta::s_ll& retval);
  static void conv_CharArray_Float(s_long fc, const vec2_t<_unitychar>& x, double& retval);
  static void conv_CharArray_Date(s_long fc, const vec2_t<_unitychar>& x, _unitydate& retval);
  static void conv_CharArray_String(s_long fc, const vec2_t<_unitychar>& x, std::wstring& retval);
  static void conv_CharArray_Char(s_long fc, const vec2_t<_unitychar>& x, _unitychar& retval);
  static void conv_CharArray_Unity(s_long fc, const vec2_t<_unitychar>& x, unity& retval);
  static void conv_CharArray_UnityArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_CharArray_IntArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_CharArray_DateArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_CharArray_FloatArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_CharArray_StringArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_CharArray_CharArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_CharArray_UnityVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<unity>& retval);
  static void conv_CharArray_IntVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<meta::s_ll>& retval);
  static void conv_CharArray_FloatVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<double>& retval);
  static void conv_CharArray_DateVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<_unitydate>& retval);
  static void conv_CharArray_StringVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<std::wstring>& retval);
  static void conv_CharArray_CharVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<_unitychar>& retval);
  static void conv_DateArray_Int(s_long fc, const vec2_t<_unitydate>& x, meta::s_ll& retval);
  static void conv_DateArray_Float(s_long fc, const vec2_t<_unitydate>& x, double& retval);
  static void conv_DateArray_Date(s_long fc, const vec2_t<_unitydate>& x, _unitydate& retval);
  static void conv_DateArray_String(s_long fc, const vec2_t<_unitydate>& x, std::wstring& retval);
  static void conv_DateArray_Char(s_long fc, const vec2_t<_unitydate>& x, _unitychar& retval);
  static void conv_DateArray_Unity(s_long fc, const vec2_t<_unitydate>& x, unity& retval);
  static void conv_DateArray_UnityArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_DateArray_IntArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_DateArray_FloatArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_DateArray_DateArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_DateArray_StringArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_DateArray_CharArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_DateArray_UnityVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<unity>& retval);
  static void conv_DateArray_IntVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<meta::s_ll>& retval);
  static void conv_DateArray_FloatVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<double>& retval);
  static void conv_DateArray_DateVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<_unitydate>& retval);
  static void conv_DateArray_StringVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<std::wstring>& retval);
  static void conv_DateArray_CharVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<_unitychar>& retval);
  static void conv_StringArray_Int(s_long fc, const vec2_t<std::wstring>& x, meta::s_ll& retval);
  static void conv_StringArray_Float(s_long fc, const vec2_t<std::wstring>& x, double& retval);
  static void conv_StringArray_Date(s_long fc, const vec2_t<std::wstring>& x, _unitydate& retval);
  static void conv_StringArray_String(s_long fc, const vec2_t<std::wstring>& x, std::wstring& retval);
  static void conv_StringArray_Char(s_long fc, const vec2_t<std::wstring>& x, _unitychar& retval);
  static void conv_StringArray_Unity(s_long fc, const vec2_t<std::wstring>& x, unity& retval);
  static void conv_StringArray_UnityArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_StringArray_IntArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_StringArray_FloatArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_StringArray_DateArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_StringArray_StringArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_StringArray_CharArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_StringArray_UnityVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<unity>& retval);
  static void conv_StringArray_IntVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<meta::s_ll>& retval);
  static void conv_StringArray_FloatVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<double>& retval);
  static void conv_StringArray_DateVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<_unitydate>& retval);
  static void conv_StringArray_StringVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<std::wstring>& retval);
  static void conv_StringArray_CharVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<_unitychar>& retval);
  static void conv_utStringArray_Int(s_long fc, const unity& x, meta::s_ll& retval);
  static void conv_utStringArray_Float(s_long fc, const unity& x, double& retval);
  static void conv_utStringArray_Date(s_long fc, const unity& x, _unitydate& retval);
  static void conv_utStringArray_String(s_long fc, const unity& x, std::wstring& retval);
  static void conv_utStringArray_Char(s_long fc, const unity& x, _unitychar& retval);
  static void conv_utStringArray_Unity(s_long fc, const unity& x, unity& retval);
  static void conv_utStringArray_UnityArray(s_long fc, const unity& x, vec2_t<unity, __vecm_tu_selector>& retval);
  static void conv_utStringArray_IntArray(s_long fc, const unity& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval);
  static void conv_utStringArray_FloatArray(s_long fc, const unity& x, vec2_t<double, __vecm_tu_selector>& retval);
  static void conv_utStringArray_DateArray(s_long fc, const unity& x, vec2_t<_unitydate, __vecm_tu_selector>& retval);
  static void conv_utStringArray_StringArray(s_long fc, const unity& x, vec2_t<std::wstring, __vecm_tu_selector>& retval);
  static void conv_utStringArray_CharArray(s_long fc, const unity& x, vec2_t<_unitychar, __vecm_tu_selector>& retval);
  static void conv_utStringArray_UnityVector(s_long fc, const unity& x, std::vector<unity>& retval);
  static void conv_utStringArray_IntVector(s_long fc, const unity& x, std::vector<meta::s_ll>& retval);
  static void conv_utStringArray_FloatVector(s_long fc, const unity& x, std::vector<double>& retval);
  static void conv_utStringArray_DateVector(s_long fc, const unity& x, std::vector<_unitydate>& retval);
  static void conv_utStringArray_StringVector(s_long fc, const unity& x, std::vector<std::wstring>& retval);
  static void conv_utStringArray_CharVector(s_long fc, const unity& x, std::vector<_unitychar>& retval);
};

  // psrc: points to the source value.
  //  pdest: points to the destination value.
  //      For T with constructors, the destination value must be already initialized (e.g. by default).
  //  uttf_src: any valid utEmpty..utUnityArray. Must also include unity object flags.
  //    Some of them are ignored (xfPtr). (This may change.)
  //    Others are used (xfObjStrongRef, xfObjItfsList).
  //  utt_dest = any valid utInt..utUnityArray.
  //  flags:
  //    0x1 (not ignored only if utt_dest is an array): pdest points to std::vector, otherwise to vec2_t.
  // NOTE conv_X_X calls assignment (operator=) for any X and cs, including csStrict.
  // NOTE conv_*_Unity calls assignment (operator=) for any input type and cs, including csStrict.
  // NOTE conv_Empty_* creates a default value for any output type and cs, including csStrict.
  //    psrc is ignored in this case.
  // NOTE conv_Object_Unity creates a new object reference.
  // NOTE for map and hash, only 8 conversions and copy fns. are defined:
  //  (Map || Hash) --> (String || Unity || Map || Hash).
  // NOTE no conversion is defined for:
  //    conv_*_Empty (== clear),
  //    conv_*_Object (== set_obj),
  //    conv_Object_UnityArray,
  //    conv_Object_UnityVector.
  // NOTE On error, conversion may generate exceptions even with cs == csLazy.
  //    *pdest is left valid, but with undefined value (valid, but partially modified or cleared).
void _static_conv::cnv_all(EConvStrength cs, s_long uttf_src, s_long utt_dest, s_long flags, const void* psrc, void* pdest)
{
  if (cs == csStrict)
  {
    s_long utt_src = uttf_src & unity::xfmUtype;
    if (utt_src != utt_dest && utt_src != utEmpty && utt_dest != utUnity)
      { throw XUTypeMismatch("cnv_all.6", unity::_tname0(uttf_src), unity::_tname0(utt_dest)); }
  }

  s_long i1 = (uttf_src & utMaskNoArray) + (uttf_src & utArray ? _ut_end : 0);
  s_long i2 = (utt_dest & utMaskNoArray) + (utt_dest & utArray ? _ut_end : 0);

  static void* conv_table[2][2 * _ut_end][2 * _ut_end]
  = {
    {
      { 0, (void*)&conv_Empty_Int, (void*)&conv_Empty_Float, (void*)&conv_Empty_Date, (void*)&conv_Empty_Char, (void*)&conv_Empty_String, 0, (void*)&conv_Empty_Unity, (void*)&conv_Empty_Map, (void*)&conv_Empty_Hash, 0, (void*)&conv_Empty_IntArray, (void*)&conv_Empty_FloatArray, (void*)&conv_Empty_DateArray, (void*)&conv_Empty_CharArray, (void*)&conv_Empty_StringArray, 0, (void*)&conv_Empty_UnityArray, 0, 0 },
      { 0, (void*)&conv_Int_Int, (void*)&conv_Int_Float, (void*)&conv_Int_Date, (void*)&conv_Int_Char, (void*)&conv_Int_String, 0, (void*)&conv_Int_Unity, 0, 0, 0, (void*)&conv_Int_IntArray, (void*)&conv_Int_FloatArray, (void*)&conv_Int_DateArray, (void*)&conv_Int_CharArray, (void*)&conv_Int_StringArray, 0, (void*)&conv_Int_UnityArray, 0, 0 },
      { 0, (void*)&conv_Float_Int, (void*)&conv_Float_Float, (void*)&conv_Float_Date, (void*)&conv_Float_Char, (void*)&conv_Float_String, 0, (void*)&conv_Float_Unity, 0, 0, 0, (void*)&conv_Float_IntArray, (void*)&conv_Float_FloatArray, (void*)&conv_Float_DateArray, (void*)&conv_Float_CharArray, (void*)&conv_Float_StringArray, 0, (void*)&conv_Float_UnityArray, 0, 0 },
      { 0, (void*)&conv_Date_Int, (void*)&conv_Date_Float, (void*)&conv_Date_Date, (void*)&conv_Date_Char, (void*)&conv_Date_String, 0, (void*)&conv_Date_Unity, 0, 0, 0, (void*)&conv_Date_IntArray, (void*)&conv_Date_FloatArray, (void*)&conv_Date_DateArray, (void*)&conv_Date_CharArray, (void*)&conv_Date_StringArray, 0, (void*)&conv_Date_UnityArray, 0, 0 },
      { 0, (void*)&conv_Char_Int, (void*)&conv_Char_Float, (void*)&conv_Char_Date, (void*)&conv_Char_Char, (void*)&conv_Char_String, 0, (void*)&conv_Char_Unity, 0, 0, 0, (void*)&conv_Char_IntArray, (void*)&conv_Char_FloatArray, (void*)&conv_Char_DateArray, (void*)&conv_Char_CharArray, (void*)&conv_Char_StringArray, 0, (void*)&conv_Char_UnityArray, 0, 0 },
      { 0, (void*)&conv_String_Int, (void*)&conv_String_Float, (void*)&conv_String_Date, (void*)&conv_String_Char, (void*)&conv_String_String, 0, (void*)&conv_String_Unity, 0, 0, 0, (void*)&conv_String_IntArray, (void*)&conv_String_FloatArray, (void*)&conv_String_DateArray, (void*)&conv_String_CharArray, (void*)&conv_String_StringArray, 0, (void*)&conv_String_UnityArray, 0, 0 },
      { 0, (void*)&conv_Object_Int, (void*)&conv_Object_Float, (void*)&conv_Object_Date, (void*)&conv_Object_Char, (void*)&conv_Object_String, 0, (void*)&conv_Object_Unity, 0, 0, 0, (void*)&conv_Object_IntArray, (void*)&conv_Object_FloatArray, (void*)&conv_Object_DateArray, (void*)&conv_Object_CharArray, (void*)&conv_Object_StringArray, 0, 0, 0, 0 },
      { 0, (void*)&conv_Unity_Int, (void*)&conv_Unity_Float, (void*)&conv_Unity_Date, (void*)&conv_Unity_Char, (void*)&conv_Unity_String, 0, (void*)&conv_Unity_Unity, 0, 0, 0, (void*)&conv_Unity_IntArray, (void*)&conv_Unity_FloatArray, (void*)&conv_Unity_DateArray, (void*)&conv_Unity_CharArray, (void*)&conv_Unity_StringArray, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, (void*)&conv_Map_String, 0, (void*)&conv_Map_Unity, (void*)&conv_Map_Map, (void*)&conv_Map_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, (void*)&conv_Hash_String, 0, (void*)&conv_Hash_Unity, (void*)&conv_Hash_Map, (void*)&conv_Hash_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_IntArray_Int, (void*)&conv_IntArray_Float, (void*)&conv_IntArray_Date, (void*)&conv_IntArray_Char, (void*)&conv_IntArray_String, 0, (void*)&conv_IntArray_Unity, 0, 0, 0, (void*)&conv_IntArray_IntArray, (void*)&conv_IntArray_FloatArray, (void*)&conv_IntArray_DateArray, (void*)&conv_IntArray_CharArray, (void*)&conv_IntArray_StringArray, 0, (void*)&conv_IntArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_FloatArray_Int, (void*)&conv_FloatArray_Float, (void*)&conv_FloatArray_Date, (void*)&conv_FloatArray_Char, (void*)&conv_FloatArray_String, 0, (void*)&conv_FloatArray_Unity, 0, 0, 0, (void*)&conv_FloatArray_IntArray, (void*)&conv_FloatArray_FloatArray, (void*)&conv_FloatArray_DateArray, (void*)&conv_FloatArray_CharArray, (void*)&conv_FloatArray_StringArray, 0, (void*)&conv_FloatArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_DateArray_Int, (void*)&conv_DateArray_Float, (void*)&conv_DateArray_Date, (void*)&conv_DateArray_Char, (void*)&conv_DateArray_String, 0, (void*)&conv_DateArray_Unity, 0, 0, 0, (void*)&conv_DateArray_IntArray, (void*)&conv_DateArray_FloatArray, (void*)&conv_DateArray_DateArray, (void*)&conv_DateArray_CharArray, (void*)&conv_DateArray_StringArray, 0, (void*)&conv_DateArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_CharArray_Int, (void*)&conv_CharArray_Float, (void*)&conv_CharArray_Date, (void*)&conv_CharArray_Char, (void*)&conv_CharArray_String, 0, (void*)&conv_CharArray_Unity, 0, 0, 0, (void*)&conv_CharArray_IntArray, (void*)&conv_CharArray_FloatArray, (void*)&conv_CharArray_DateArray, (void*)&conv_CharArray_CharArray, (void*)&conv_CharArray_StringArray, 0, (void*)&conv_CharArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_StringArray_Int, (void*)&conv_StringArray_Float, (void*)&conv_StringArray_Date, (void*)&conv_StringArray_Char, (void*)&conv_StringArray_String, 0, (void*)&conv_StringArray_Unity, 0, 0, 0, (void*)&conv_StringArray_IntArray, (void*)&conv_StringArray_FloatArray, (void*)&conv_StringArray_DateArray, (void*)&conv_StringArray_CharArray, (void*)&conv_StringArray_StringArray, 0, (void*)&conv_StringArray_UnityArray, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_UnityArray_Int, (void*)&conv_UnityArray_Float, (void*)&conv_UnityArray_Date, (void*)&conv_UnityArray_Char, (void*)&conv_UnityArray_String, 0, (void*)&conv_UnityArray_Unity, 0, 0, 0, (void*)&conv_UnityArray_IntArray, (void*)&conv_UnityArray_FloatArray, (void*)&conv_UnityArray_DateArray, (void*)&conv_UnityArray_CharArray, (void*)&conv_UnityArray_StringArray, 0, (void*)&conv_UnityArray_UnityArray, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
      { 0, (void*)&conv_Empty_Int, (void*)&conv_Empty_Float, (void*)&conv_Empty_Date, (void*)&conv_Empty_Char, (void*)&conv_Empty_String, 0, (void*)&conv_Empty_Unity, (void*)&conv_Empty_Map, (void*)&conv_Empty_Hash, 0, (void*)&conv_Empty_IntArray, (void*)&conv_Empty_FloatArray, (void*)&conv_Empty_DateArray, (void*)&conv_Empty_CharArray, (void*)&conv_Empty_StringArray, 0, (void*)&conv_Empty_UnityArray, 0, 0 },
      { 0, (void*)&conv_Int_Int, (void*)&conv_Int_Float, (void*)&conv_Int_Date, (void*)&conv_Int_Char, (void*)&conv_Int_String, 0, (void*)&conv_Int_Unity, 0, 0, 0, (void*)&conv_Int_IntArray, (void*)&conv_Int_FloatArray, (void*)&conv_Int_DateArray, (void*)&conv_Int_CharArray, (void*)&conv_Int_StringArray, 0, (void*)&conv_Int_UnityArray, 0, 0 },
      { 0, (void*)&conv_Float_Int, (void*)&conv_Float_Float, (void*)&conv_Float_Date, (void*)&conv_Float_Char, (void*)&conv_Float_String, 0, (void*)&conv_Float_Unity, 0, 0, 0, (void*)&conv_Float_IntArray, (void*)&conv_Float_FloatArray, (void*)&conv_Float_DateArray, (void*)&conv_Float_CharArray, (void*)&conv_Float_StringArray, 0, (void*)&conv_Float_UnityArray, 0, 0 },
      { 0, (void*)&conv_Date_Int, (void*)&conv_Date_Float, (void*)&conv_Date_Date, (void*)&conv_Date_Char, (void*)&conv_Date_String, 0, (void*)&conv_Date_Unity, 0, 0, 0, (void*)&conv_Date_IntArray, (void*)&conv_Date_FloatArray, (void*)&conv_Date_DateArray, (void*)&conv_Date_CharArray, (void*)&conv_Date_StringArray, 0, (void*)&conv_Date_UnityArray, 0, 0 },
      { 0, (void*)&conv_Char_Int, (void*)&conv_Char_Float, (void*)&conv_Char_Date, (void*)&conv_Char_Char, (void*)&conv_Char_String, 0, (void*)&conv_Char_Unity, 0, 0, 0, (void*)&conv_Char_IntArray, (void*)&conv_Char_FloatArray, (void*)&conv_Char_DateArray, (void*)&conv_Char_CharArray, (void*)&conv_Char_StringArray, 0, (void*)&conv_Char_UnityArray, 0, 0 },
      { 0, (void*)&conv_String_Int, (void*)&conv_String_Float, (void*)&conv_String_Date, (void*)&conv_String_Char, (void*)&conv_String_String, 0, (void*)&conv_String_Unity, 0, 0, 0, (void*)&conv_String_IntArray, (void*)&conv_String_FloatArray, (void*)&conv_String_DateArray, (void*)&conv_String_CharArray, (void*)&conv_String_StringArray, 0, (void*)&conv_String_UnityArray, 0, 0 },
      { 0, (void*)&conv_Object_Int, (void*)&conv_Object_Float, (void*)&conv_Object_Date, (void*)&conv_Object_Char, (void*)&conv_Object_String, 0, (void*)&conv_Object_Unity, 0, 0, 0, (void*)&conv_Object_IntArray, (void*)&conv_Object_FloatArray, (void*)&conv_Object_DateArray, (void*)&conv_Object_CharArray, (void*)&conv_Object_StringArray, 0, 0, 0, 0 },
      { 0, (void*)&conv_Unity_Int, (void*)&conv_Unity_Float, (void*)&conv_Unity_Date, (void*)&conv_Unity_Char, (void*)&conv_Unity_String, 0, (void*)&conv_Unity_Unity, 0, 0, 0, (void*)&conv_Unity_IntArray, (void*)&conv_Unity_FloatArray, (void*)&conv_Unity_DateArray, (void*)&conv_Unity_CharArray, (void*)&conv_Unity_StringArray, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, (void*)&conv_Map_String, 0, (void*)&conv_Map_Unity, (void*)&conv_Map_Map, (void*)&conv_Map_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, (void*)&conv_Hash_String, 0, (void*)&conv_Hash_Unity, (void*)&conv_Hash_Map, (void*)&conv_Hash_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_IntArray_Int, (void*)&conv_IntArray_Float, (void*)&conv_IntArray_Date, (void*)&conv_IntArray_Char, (void*)&conv_IntArray_String, 0, (void*)&conv_IntArray_Unity, 0, 0, 0, (void*)&conv_IntArray_IntVector, (void*)&conv_IntArray_FloatVector, (void*)&conv_IntArray_DateVector, (void*)&conv_IntArray_CharVector, (void*)&conv_IntArray_StringVector, 0, (void*)&conv_IntArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_FloatArray_Int, (void*)&conv_FloatArray_Float, (void*)&conv_FloatArray_Date, (void*)&conv_FloatArray_Char, (void*)&conv_FloatArray_String, 0, (void*)&conv_FloatArray_Unity, 0, 0, 0, (void*)&conv_FloatArray_IntVector, (void*)&conv_FloatArray_FloatVector, (void*)&conv_FloatArray_DateVector, (void*)&conv_FloatArray_CharVector, (void*)&conv_FloatArray_StringVector, 0, (void*)&conv_FloatArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_DateArray_Int, (void*)&conv_DateArray_Float, (void*)&conv_DateArray_Date, (void*)&conv_DateArray_Char, (void*)&conv_DateArray_String, 0, (void*)&conv_DateArray_Unity, 0, 0, 0, (void*)&conv_DateArray_IntVector, (void*)&conv_DateArray_FloatVector, (void*)&conv_DateArray_DateVector, (void*)&conv_DateArray_CharVector, (void*)&conv_DateArray_StringVector, 0, (void*)&conv_DateArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_CharArray_Int, (void*)&conv_CharArray_Float, (void*)&conv_CharArray_Date, (void*)&conv_CharArray_Char, (void*)&conv_CharArray_String, 0, (void*)&conv_CharArray_Unity, 0, 0, 0, (void*)&conv_CharArray_IntVector, (void*)&conv_CharArray_FloatVector, (void*)&conv_CharArray_DateVector, (void*)&conv_CharArray_CharVector, (void*)&conv_CharArray_StringVector, 0, (void*)&conv_CharArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_StringArray_Int, (void*)&conv_StringArray_Float, (void*)&conv_StringArray_Date, (void*)&conv_StringArray_Char, (void*)&conv_StringArray_String, 0, (void*)&conv_StringArray_Unity, 0, 0, 0, (void*)&conv_StringArray_IntVector, (void*)&conv_StringArray_FloatVector, (void*)&conv_StringArray_DateVector, (void*)&conv_StringArray_CharVector, (void*)&conv_StringArray_StringVector, 0, (void*)&conv_StringArray_UnityVector, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_UnityArray_Int, (void*)&conv_UnityArray_Float, (void*)&conv_UnityArray_Date, (void*)&conv_UnityArray_Char, (void*)&conv_UnityArray_String, 0, (void*)&conv_UnityArray_Unity, 0, 0, 0, (void*)&conv_UnityArray_IntVector, (void*)&conv_UnityArray_FloatVector, (void*)&conv_UnityArray_DateVector, (void*)&conv_UnityArray_CharVector, (void*)&conv_UnityArray_StringVector, 0, (void*)&conv_UnityArray_UnityVector, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
  };

  typedef void(*Conv1)(s_long flags, char& pdest);
  typedef void(*Conv2)(s_long flags, const char& psrc, char& pdest);

  s_long fconv = cs & cs_mask; fconv |= (uttf_src & (unity::xfObjItfsList | unity::xfObjStrongRef));
  if (i1 >= 2 * _ut_end || i2 >=  2 * _ut_end) { throw XUConvFailed("cnv_all.1", i1, i2, flags, fconv); }
  try
  {
    if (i1 == utEmpty)
    {
      Conv1 f_conv1 = (Conv1)conv_table[flags & 0x1 ? 1 : 0][i1][i2];
      if (f_conv1 && pdest) { f_conv1(fconv, *(char*)pdest); }
        else { throw XUConvFailed("cnv_all.2", i1, i2, flags, fconv); }
    }
    else
    {
      Conv2 f_conv2 = (Conv2)conv_table[flags & 0x1 ? 1 : 0][i1][i2];
      if (f_conv2 && psrc && pdest) { f_conv2(fconv, *(char*)psrc, *(char*)pdest); }
        else { throw XUConvFailed("cnv_all.3", i1, i2, flags, fconv); }
    }
  }
  catch (_XUBase&) { throw; }
  catch (std::exception& e) { throw XUConvFailed("cnv_all.4", e.what(), i1, i2, flags, fconv); }
  catch (...) { throw XUConvFailed("cnv_all.5", i1, i2, flags, fconv); }
}

  // Specialized function for converting incompatible unity value from other binary module
  //  into the value of required type.
  // Implemented for: utStringArray --> multiple types.
  // The function behaves similarly to cnv_all.
  //  usrc: the source value, may be from any binary module.
  //  pdest: points to the destination value.
  //    For T with constructors, the destination value must be already initialized (e.g. by default).
  //  utt_dest = any valid utInt..utUnityArray.
  //  flags:
  //    0x1 (not ignored only if utt_dest is an array): pdest points to std::vector, otherwise to vec2_t.
void _static_conv::cnv_incompat(EConvStrength cs, s_long utt_dest, s_long flags, const unity& usrc, void* pdest)
{
  if (cs == csStrict)
  {
    if (usrc.utype() != utt_dest && !usrc.isEmpty() && utt_dest != utUnity)
      { throw XUTypeMismatch("cnv_incompat.6", unity::_tname0(usrc.utype()), unity::_tname0(utt_dest)); }
  }

  s_long i1 = usrc.utype_noarray() + (usrc.isArray() ? _ut_end : 0);
  s_long i2 = (utt_dest & utMaskNoArray) + (utt_dest & utArray ? _ut_end : 0);

  typedef void* t_conv_table2[2 * _ut_end];
  static void* conv_utStringArray_any[2][2 * _ut_end]
  = {
    { 0, (void*)&conv_utStringArray_Int, (void*)&conv_utStringArray_Float, (void*)&conv_utStringArray_Date, (void*)&conv_utStringArray_Char, (void*)&conv_utStringArray_String, 0, (void*)&conv_utStringArray_Unity, 0, 0, 0, (void*)&conv_utStringArray_IntArray, (void*)&conv_utStringArray_FloatArray, (void*)&conv_utStringArray_DateArray, (void*)&conv_utStringArray_CharArray, (void*)&conv_utStringArray_StringArray, 0, (void*)&conv_utStringArray_UnityArray, 0, 0 },
    { 0, (void*)&conv_utStringArray_Int, (void*)&conv_utStringArray_Float, (void*)&conv_utStringArray_Date, (void*)&conv_utStringArray_Char, (void*)&conv_utStringArray_String, 0, (void*)&conv_utStringArray_Unity, 0, 0, 0, (void*)&conv_utStringArray_IntVector, (void*)&conv_utStringArray_FloatVector, (void*)&conv_utStringArray_DateVector, (void*)&conv_utStringArray_CharVector, (void*)&conv_utStringArray_StringVector, 0, (void*)&conv_utStringArray_UnityVector, 0, 0 },
  };
  static t_conv_table2* conv_table[2 * _ut_end]
  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, conv_utStringArray_any, 0, 0, 0, 0 };

  typedef void(*Conv2)(s_long flags, const unity& rsrc, char& pdest);

  s_long fconv = cs & cs_mask;
  if (i1 >= 2 * _ut_end || i2 >=  2 * _ut_end) { throw XUConvFailed("cnv_incompat.1", i1, i2, flags, fconv); }
  try
  {
    t_conv_table2* p = conv_table[i1];
    Conv2 f_conv2 = p ? (Conv2)p[flags & 0x1 ? 1 : 0][i2] : 0;
    if (f_conv2 && pdest) { f_conv2(fconv, usrc, *(char*)pdest); }
      else { throw XUConvFailed("cnv_incompat.3", i1, i2, flags, fconv); }
  }
  catch (_XUBase&) { throw; }
  catch (std::exception& e) { throw XUConvFailed("cnv_incompat.4", e.what(), i1, i2, flags, fconv); }
  catch (...) { throw XUConvFailed("cnv_incompat.5", i1, i2, flags, fconv); }
}

//========================================================================

void _static_conv::conv_Empty_Int(s_long fc, meta::s_ll& retval) { retval = 0L; }
void _static_conv::conv_Empty_Float(s_long fc, double& retval) { retval = 0.; }
void _static_conv::conv_Empty_Date(s_long fc, _unitydate& retval) { retval = _unitydate(0.); }
void _static_conv::conv_Empty_String(s_long fc, std::wstring& retval) { retval.clear(); }
void _static_conv::conv_Empty_Char(s_long fc, _unitychar& retval) { retval = 0; }
void _static_conv::conv_Empty_Unity(s_long fc, unity& retval) { retval.clear(); }
void _static_conv::conv_Empty_Map(s_long fc, unity::t_map_tu& retval) { retval.ordhs_clear(); }
void _static_conv::conv_Empty_Hash(s_long fc, unity::t_hash& retval) { retval.hl_clear(true); }

void _static_conv::conv_Empty_UnityArray(s_long fc, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); }
void _static_conv::conv_Empty_IntArray(s_long fc, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); }
void _static_conv::conv_Empty_FloatArray(s_long fc, vec2_t<double, __vecm_tu_selector>& retval) { retval.clear(); }
void _static_conv::conv_Empty_DateArray(s_long fc, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); }
void _static_conv::conv_Empty_StringArray(s_long fc, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); }
void _static_conv::conv_Empty_CharArray(s_long fc, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); }

void _static_conv::conv_Empty_UnityVector(s_long fc, std::vector<unity>& retval) { retval.clear(); }
void _static_conv::conv_Empty_IntVector(s_long fc, std::vector<meta::s_ll>& retval) { retval.clear(); }
void _static_conv::conv_Empty_FloatVector(s_long fc, std::vector<double>& retval) { retval.clear(); }
void _static_conv::conv_Empty_DateVector(s_long fc, std::vector<_unitydate>& retval) { retval.clear(); }
void _static_conv::conv_Empty_StringVector(s_long fc, std::vector<std::wstring>& retval) { retval.clear(); }
void _static_conv::conv_Empty_CharVector(s_long fc, std::vector<_unitychar>& retval) { retval.clear(); }

//========================================================================

void _static_conv::conv_Int_Int(s_long fc, const meta::s_ll& x, meta::s_ll& retval) { retval = x; }
void _static_conv::conv_Int_Float(s_long fc, const meta::s_ll& x, double& retval) { retval = double(x); }
void _static_conv::conv_Int_Date(s_long fc, const meta::s_ll& x, _unitydate& retval) { retval.set(double(x)); }
void _static_conv::conv_Int_String(s_long fc, const meta::s_ll& x, std::wstring& retval) { try { retval = ntows(x); return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }
void _static_conv::conv_Int_Char(s_long fc, const meta::s_ll& x, _unitychar& retval) { retval = _unitychar(x); }
void _static_conv::conv_Int_Unity(s_long fc, const meta::s_ll& x, unity& retval) { retval = x; }

void _static_conv::conv_Int_UnityArray(s_long fc, const meta::s_ll& x, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Int_IntArray(s_long fc, const meta::s_ll& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Int_FloatArray(s_long fc, const meta::s_ll& x, vec2_t<double, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(double(x)); }
void _static_conv::conv_Int_DateArray(s_long fc, const meta::s_ll& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(_unitydate(double(x))); }
void _static_conv::conv_Int_StringArray(s_long fc, const meta::s_ll& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); std::wstring s; conv_Int_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Int_CharArray(s_long fc, const meta::s_ll& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); _unitychar b; conv_Int_Char(fc, x, b); retval.push_back(b); }

void _static_conv::conv_Int_UnityVector(s_long fc, const meta::s_ll& x, std::vector<unity>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Int_IntVector(s_long fc, const meta::s_ll& x, std::vector<meta::s_ll>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Int_FloatVector(s_long fc, const meta::s_ll& x, std::vector<double>& retval) { retval.clear(); retval.push_back(double(x)); }
void _static_conv::conv_Int_DateVector(s_long fc, const meta::s_ll& x, std::vector<_unitydate>& retval) { retval.clear(); retval.push_back(_unitydate(double(x))); }
void _static_conv::conv_Int_StringVector(s_long fc, const meta::s_ll& x, std::vector<std::wstring>& retval) { retval.clear(); std::wstring s; conv_Int_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Int_CharVector(s_long fc, const meta::s_ll& x, std::vector<_unitychar>& retval) { retval.clear(); _unitychar b; conv_Int_Char(fc, x, b); retval.push_back(b); }

//========================================================================

void _static_conv::conv_Char_Int(s_long fc, const _unitychar& x, meta::s_ll& retval) { retval = x?1L:0L; }
void _static_conv::conv_Char_Float(s_long fc, const _unitychar& x, double& retval) { retval = x?1.:0.; }
void _static_conv::conv_Char_Date(s_long fc, const _unitychar& x, _unitydate& retval) { retval = d_time(0, (s_long((unsigned char)x)) / 60, (s_long((unsigned char)x)) % 60); }
void _static_conv::conv_Char_String(s_long fc, const _unitychar& x, std::wstring& retval) { try { retval = ntows(s_long(x)); return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }
void _static_conv::conv_Char_Char(s_long fc, const _unitychar& x, _unitychar& retval) { retval = x; }
void _static_conv::conv_Char_Unity(s_long fc, const _unitychar& x, unity& retval) { retval = x; }

void _static_conv::conv_Char_UnityArray(s_long fc, const _unitychar& x, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Char_IntArray(s_long fc, const _unitychar& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); meta::s_ll temp; conv_Char_Int(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_Char_FloatArray(s_long fc, const _unitychar& x, vec2_t<double, __vecm_tu_selector>& retval) { retval.clear(); double temp; conv_Char_Float(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_Char_DateArray(s_long fc, const _unitychar& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); _unitydate temp; conv_Char_Date(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_Char_StringArray(s_long fc, const _unitychar& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); std::wstring s; conv_Char_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Char_CharArray(s_long fc, const _unitychar& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }

void _static_conv::conv_Char_UnityVector(s_long fc, const _unitychar& x, std::vector<unity>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Char_IntVector(s_long fc, const _unitychar& x, std::vector<meta::s_ll>& retval) { retval.clear(); meta::s_ll temp; conv_Char_Int(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_Char_FloatVector(s_long fc, const _unitychar& x, std::vector<double>& retval) { retval.clear(); double x2; conv_Char_Float(fc, x, x2); retval.push_back(x2); }
void _static_conv::conv_Char_DateVector(s_long fc, const _unitychar& x, std::vector<_unitydate>& retval) { retval.clear(); _unitydate x2; conv_Char_Date(fc, x, x2); retval.push_back(x2); }
void _static_conv::conv_Char_StringVector(s_long fc, const _unitychar& x, std::vector<std::wstring>& retval) { retval.clear(); std::wstring s; conv_Char_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Char_CharVector(s_long fc, const _unitychar& x, std::vector<_unitychar>& retval) { retval.clear(); retval.push_back(x); }

//========================================================================

void _static_conv::conv_Float_Int(s_long fc, const double& x, meta::s_ll& retval) { retval = meta::s_ll(std::floor(x)); }
void _static_conv::conv_Float_Float(s_long fc, const double& x, double& retval) { retval = x; }
void _static_conv::conv_Float_Date(s_long fc, const double& x, _unitydate& retval) { retval.set(x); }
void _static_conv::conv_Float_String(s_long fc, const double& x, std::wstring& retval) { try { retval = _fls75(x, 15, 15).wstr(); return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }
void _static_conv::conv_Float_Char(s_long fc, const double& x, _unitychar& retval) { retval = _unitychar(meta::s_ll(std::floor(x))); }
void _static_conv::conv_Float_Unity(s_long fc, const double& x, unity& retval) { retval = x; }

void _static_conv::conv_Float_UnityArray(s_long fc, const double& x, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Float_IntArray(s_long fc, const double& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(meta::s_ll(std::floor(x))); }
void _static_conv::conv_Float_FloatArray(s_long fc, const double& x, vec2_t<double, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Float_DateArray(s_long fc, const double& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(_unitydate(x)); }
void _static_conv::conv_Float_StringArray(s_long fc, const double& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); std::wstring s; conv_Float_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Float_CharArray(s_long fc, const double& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); _unitychar b; conv_Float_Char(fc, x, b); retval.push_back(b); }

void _static_conv::conv_Float_UnityVector(s_long fc, const double& x, std::vector<unity>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Float_IntVector(s_long fc, const double& x, std::vector<meta::s_ll>& retval) { retval.clear(); retval.push_back(meta::s_ll(std::floor(x))); }
void _static_conv::conv_Float_FloatVector(s_long fc, const double& x, std::vector<double>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Float_DateVector(s_long fc, const double& x, std::vector<_unitydate>& retval) { retval.clear(); retval.push_back(_unitydate(x)); }
void _static_conv::conv_Float_StringVector(s_long fc, const double& x, std::vector<std::wstring>& retval) { retval.clear(); std::wstring s; conv_Float_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Float_CharVector(s_long fc, const double& x, std::vector<_unitychar>& retval) { retval.clear(); _unitychar b; conv_Float_Char(fc, x, b); retval.push_back(b); }

//========================================================================

void _static_conv::conv_Date_Int(s_long fc, const _unitydate& x, meta::s_ll& retval) { retval = meta::s_ll(std::floor(x.f())); return; }
void _static_conv::conv_Date_Float(s_long fc, const _unitydate& x, double& retval) { retval = x.f(); }
void _static_conv::conv_Date_Date(s_long fc, const _unitydate& x, _unitydate& retval) { retval = x; }
void _static_conv::conv_Date_String(s_long fc, const _unitydate& x, std::wstring& retval) { try { retval = dtofls(x).wstr(); return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }
void _static_conv::conv_Date_Char(s_long fc, const _unitydate& x, _unitychar& retval) { retval = _unitychar(meta::s_ll(std::floor(x.f()))); }
void _static_conv::conv_Date_Unity(s_long fc, const _unitydate& x, unity& retval) { retval = x; }

void _static_conv::conv_Date_UnityArray(s_long fc, const _unitydate& x, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Date_IntArray(s_long fc, const _unitydate& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(meta::s_ll(std::floor(x.f()))); }
void _static_conv::conv_Date_FloatArray(s_long fc, const _unitydate& x, vec2_t<double, __vecm_tu_selector>& retval) { retval.push_back(x.f()); }
void _static_conv::conv_Date_DateArray(s_long fc, const _unitydate& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.push_back(x); }
void _static_conv::conv_Date_StringArray(s_long fc, const _unitydate& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); std::wstring s; conv_Date_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Date_CharArray(s_long fc, const _unitydate& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); _unitychar b; conv_Date_Char(fc, x, b); retval.push_back(b); }

void _static_conv::conv_Date_UnityVector(s_long fc, const _unitydate& x, std::vector<unity>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_Date_IntVector(s_long fc, const _unitydate& x, std::vector<meta::s_ll>& retval) { retval.clear(); retval.push_back(meta::s_ll(std::floor(x.f()))); }
void _static_conv::conv_Date_FloatVector(s_long fc, const _unitydate& x, std::vector<double>& retval) { retval.push_back(x.f()); }
void _static_conv::conv_Date_DateVector(s_long fc, const _unitydate& x, std::vector<_unitydate>& retval) { retval.push_back(x); }
void _static_conv::conv_Date_StringVector(s_long fc, const _unitydate& x, std::vector<std::wstring>& retval) { retval.clear(); std::wstring s; conv_Date_String(fc, x, s); retval.push_back(s); }
void _static_conv::conv_Date_CharVector(s_long fc, const _unitydate& x, std::vector<_unitychar>& retval) { retval.clear(); _unitychar b; conv_Date_Char(fc, x, b); retval.push_back(b); }

//========================================================================

void _static_conv::conv_String_Int(s_long fc, const std::wstring& x, meta::s_ll& retval)
{
  try { retval = str2i(x, 0, false); return; } catch (...) {}
  if ((fc & cs_mask) == csLazy) { retval = 0; return; }
  throw XUConvFailed("conv_String_Int.1", "", "", x);
}

void _static_conv::conv_String_Float(s_long fc, const std::wstring& x, double& retval)
{
  try { retval = str2f(x, 0, false, true); return; } catch (...) {}
  if ((fc & cs_mask) == csLazy) { retval = 0.; return; }
  throw XUConvFailed("conv_String_Float.1", "", "", x);
}

bool _static_conv::conv_String_Date0(const std::wstring& x, _unitydate& retval, bool no_exc)
{
  try
  {
    std::string s0 = wsToBs(x); _t_sz pos = 0;
    // \s*[+-]?y{>=1 w/sign | >=4 w/o sign}[-/.]mm[-/.]dd(\s+h{1,2}:mm(:ss([.][0-9]+)?)?)?
    const std::string space(" \t");
    const std::string digit("0123456789");
    const std::string pm("+-");
    const std::string sep1("-/.");
    const std::string sep2(":-");
    const std::string sep3(".");
    std::string s;
    bool is_signed(false); bool b(false); s_long sign(1), y(0), m(0), d(0), h(0), mt(0), sec(0); bool is_frac(false); double frac(0.);
    do // once
    {
        // leading spaces
      s = trim_n(s0, pos, space, -1); pos += s.size();

        // year sign, year, month, day
      s = trim_n(s0, pos, pm, 1); pos += s.size();
        if (s.size() > 0) { is_signed = true; sign = s[0] == '+' ? 1 : -1; }
      s = trim_n(s0, pos, digit, 9); pos += s.size();
        if (!(s.size() >= 4 || (is_signed && s.size() >= 1))) { break; }
          y = s_long(atol(s.c_str())) * sign; if (s.size() > 4 && y == 0) { break; }
      s = trim_n(s0, pos, sep1, 1); pos += s.size(); if (s.size() != 1) { break; }
      s = trim_n(s0, pos, digit, 2); pos += s.size();
        if (s.size() != 2) { break; }
          m = s_long(atol(s.c_str())); if (m < 1 || m > 12) { break; }
      s = trim_n(s0, pos, sep1, 1); pos += s.size(); if (s.size() != 1) { break; }
      s = trim_n(s0, pos, digit, 2); pos += s.size();
        if (s.size() != 2) { break; }
          d = s_long(atol(s.c_str())); if (d < 1 || d > 31) { break; }

        // spaces between date and time (or possibly trailing spaces)
      s = trim_n(s0, pos, space, -1); pos += s.size();
        if (pos == s0.size()) { b = true; break; } if (s.size() == 0) { break; }

        // hours, minutes
      s = trim_n(s0, pos, digit, 2); pos += s.size();
        if (!(s.size() == 1 || s.size() == 2)) { break; }
          h = s_long(atol(s.c_str())); if (h > 23) { break; }
      s = trim_n(s0, pos, sep2, 1); pos += s.size(); if (s.size() != 1) { break; }
      s = trim_n(s0, pos, digit, 2); pos += s.size();
        if (s.size() != 2) { break; }
          mt = s_long(atol(s.c_str())); if (mt > 59) { break; }

        // possibly trailing spaces
      s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }

        // seconds
      s = trim_n(s0, pos, sep2, 1); pos += s.size(); if (s.size() != 1) { break; }
      s = trim_n(s0, pos, digit, 2); pos += s.size();
        if (s.size() != 2) { break; }
          sec = s_long(atol(s.c_str())); if (sec > 59) { break; }

        // possibly trailing spaces
      s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }

        // frac. part of seconds
      s = trim_n(s0, pos, sep3, 1); pos += s.size(); if (s.size() != 1) { break; }
      s = trim_n(s0, pos, digit, 9); pos += s.size();
        if (s.size() == 0) { break; }
          is_frac = true; s = "." + s; frac = atof(s.c_str());

        // end of string or trailing spaces
      s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }
    }
    while (false);
    if (b) { retval = _unitydate(y, m, d, h, mt, sec); if (is_frac) { retval.add_seconds(frac); } return true; }
    return false;
  } catch (...) { if (no_exc) { return false; } throw; }
}
void _static_conv::conv_String_Date(s_long fc, const std::wstring& x, _unitydate& retval)
{
  try
  {
    if (conv_String_Date0(x, retval, false)) { return; }
    throw XUConvFailed("conv_String_Date.1", "", "", x);
  }
  catch(_XUBase&) { if ((fc & cs_mask)!=csLazy) throw; }
  catch(std::exception& e) { if ((fc & cs_mask)!=csLazy) { throw XUConvFailed("conv_String_Date.2", "", "", e.what(), x); } }
  catch(...) { if ((fc & cs_mask)!=csLazy) { throw XUConvFailed("conv_String_Date.3", "", "", x); } }
  retval.set(0.);
}
void _static_conv::conv_String_String(s_long fc, const std::wstring& x, std::wstring& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }
void _static_conv::conv_String_Char(s_long fc, const std::wstring& x, _unitychar& retval) { meta::s_ll temp; conv_String_Int(fc, x, temp); retval = _unitychar(temp); }
void _static_conv::conv_String_Unity(s_long fc, const std::wstring& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_String_UnityArray(s_long fc, const std::wstring& x, vec2_t<unity, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_String_IntArray(s_long fc, const std::wstring& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { retval.clear(); meta::s_ll temp; conv_String_Int(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_String_FloatArray(s_long fc, const std::wstring& x, vec2_t<double, __vecm_tu_selector>& retval) { retval.clear(); double y; conv_String_Float(fc, x, y); retval.push_back(y); }
void _static_conv::conv_String_DateArray(s_long fc, const std::wstring& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); _unitydate y; conv_String_Date(fc, x, y); retval.push_back(y); }
void _static_conv::conv_String_StringArray(s_long fc, const std::wstring& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_String_CharArray(s_long fc, const std::wstring& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { retval.clear(); _unitychar b; conv_String_Char(fc, x, b); retval.push_back(b); }

void _static_conv::conv_String_UnityVector(s_long fc, const std::wstring& x, std::vector<unity>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_String_IntVector(s_long fc, const std::wstring& x, std::vector<meta::s_ll>& retval) { retval.clear(); meta::s_ll temp; conv_String_Int(fc, x, temp); retval.push_back(temp); }
void _static_conv::conv_String_FloatVector(s_long fc, const std::wstring& x, std::vector<double>& retval) { retval.clear(); double y; conv_String_Float(fc, x, y); retval.push_back(y); }
void _static_conv::conv_String_DateVector(s_long fc, const std::wstring& x, std::vector<_unitydate>& retval) { retval.clear(); _unitydate y; conv_String_Date(fc, x, y); retval.push_back(y); }
void _static_conv::conv_String_StringVector(s_long fc, const std::wstring& x, std::vector<std::wstring>& retval) { retval.clear(); retval.push_back(x); }
void _static_conv::conv_String_CharVector(s_long fc, const std::wstring& x, std::vector<_unitychar>& retval) { retval.clear(); _unitychar b; conv_String_Char(fc, x, b); retval.push_back(b); }

//========================================================================

void _static_conv::conv_Object_Int(s_long fc, char& x0, meta::s_ll& retval) {  unity::o_api a(&x0, !!(fc & unity::xfObjItfsList)); retval = (char*)a.pvoid(false)-(char*)0; }
void _static_conv::conv_Object_Float(s_long fc, char& x0, double& retval) {  unity::o_api a(&x0, !!(fc & unity::xfObjItfsList)); retval = double(meta::s_ll((char*)a.pvoid(false)-(char*)0)); }
void _static_conv::conv_Object_Date(s_long fc, char& x0, _unitydate& retval) { retval.set(0); }
void _static_conv::conv_Object_String(s_long fc, char& x0, std::wstring& retval)
{
  try {
    unity::o_api a(&x0, !!(fc & unity::xfObjItfsList));
    unity::o_api::critsec_rc __lock(a.prc);
    o_ref_info ri = a.ref_info();
    o_type_info ti = a.type_info(1+8);

    typedef bmdx_meta::u_ll u_ll;
      u_ll q = u_ll((char*)ti._pobj - (char*)0);
    char qsbuf[20] = "";
      for (int pos = 0, i = 15; i >= 0; --i) { int x = int(0xf & (q >> (i << 2))); if (x || pos || i == 0) { qsbuf[pos] = char(x) + (x >= 10 ? 'A' - 10 : '0'); ++pos; qsbuf[pos] = 0; } }

    std::wstring buf;
      buf += bsToWs(ti.ptstat());
      buf += L':';
      buf += ( ri.b_deleted() ? L'D' : (ri.b_strong() ? L'S' : L'W') );
      buf += L"0x";
      buf += _fls75(qsbuf).wstr();

    retval = buf;
    return;
  } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_Object_Char(s_long fc, char& x0, _unitychar& retval) { unity::o_api a(&x0, !!(fc & unity::xfObjItfsList)); retval = !!a.pvoid(true); }
void _static_conv::conv_Object_Unity(s_long fc, char& x0, unity& retval)
{
  unity::o_api a(&x0, !!(fc & unity::xfObjItfsList));
  unity::o_api a2 = a._ref_newcp(retval.pmsm, (fc & unity::xfObjStrongRef) ? 1 : 2);
  if (!a2.prc)
  {
    if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Object_Unity", "", "", meta::s_ll(&x0 - (char*)0), fc & ~cs_mask); }
    retval.clear();
    return;
  }
  retval.clear();
  retval._data.p1 = a2.p_head();
  retval.ut = utObject | unity::xfPtr;
  if (fc & unity::xfObjStrongRef) { retval.ut |= unity::xfObjStrongRef; }
  if (a2.pidyn) { retval.ut |= unity::xfObjItfsList; }
}

void _static_conv::conv_Object_IntArray(s_long fc, char& x0, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { meta::s_ll temp; conv_Object_Int(fc,x0,temp); retval.clear(); retval.push_back(temp); }

void _static_conv::conv_Object_FloatArray(s_long fc, char& x0, vec2_t<double, __vecm_tu_selector>& retval) {  double temp; conv_Object_Float(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_DateArray(s_long fc, char& x0, vec2_t<_unitydate, __vecm_tu_selector>& retval) { retval.clear(); retval.push_back(_unitydate(0.)); }
void _static_conv::conv_Object_StringArray(s_long fc, char& x0, vec2_t<std::wstring, __vecm_tu_selector>& retval) { std::wstring temp; conv_Object_String(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_CharArray(s_long fc, char& x0, vec2_t<_unitychar, __vecm_tu_selector>& retval) { _unitychar temp; conv_Object_Char(fc,x0,temp); retval.clear(); retval.push_back(temp); }

void _static_conv::conv_Object_IntVector(s_long fc, char& x0, std::vector<meta::s_ll>& retval) {  meta::s_ll temp; conv_Object_Int(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_FloatVector(s_long fc, char& x0, std::vector<double>& retval) { double temp; conv_Object_Float(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_DateVector(s_long fc, char& x0, std::vector<_unitydate>& retval) { _unitydate temp; conv_Object_Date(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_StringVector(s_long fc, char& x0, std::vector<std::wstring>& retval) {   std::wstring temp; conv_Object_String(fc,x0,temp); retval.clear(); retval.push_back(temp); }
void _static_conv::conv_Object_CharVector(s_long fc, char& x0, std::vector<_unitychar>& retval) {   _unitychar temp; conv_Object_Char(fc,x0,temp); retval.clear(); retval.push_back(temp); }

void _static_conv::conv_Unity_Int(s_long fc, const unity& x, meta::s_ll& retval) { cnv_all(((EConvStrength)(fc & cs_mask)), x._uttf(), utInt, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_Float(s_long fc, const unity& x, double& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utFloat, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_Date(s_long fc, const unity& x, _unitydate& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utDate, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_String(s_long fc, const unity& x, std::wstring& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utString, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_Char(s_long fc, const unity& x, _unitychar& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utChar, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_Unity(s_long fc, const unity& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_Unity_UnityArray(s_long fc, const unity& x, vec2_t<unity, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utUnityArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_IntArray(s_long fc, const unity& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utIntArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_FloatArray(s_long fc, const unity& x, vec2_t<double, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utFloatArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_DateArray(s_long fc, const unity& x, vec2_t<_unitydate, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utDateArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_StringArray(s_long fc, const unity& x, vec2_t<std::wstring, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utStringArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_CharArray(s_long fc, const unity& x, vec2_t<_unitychar, __vecm_tu_selector>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utCharArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }

void _static_conv::conv_Unity_UnityVector(s_long fc, const unity& x, std::vector<unity>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utUnityArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_IntVector(s_long fc, const unity& x, std::vector<meta::s_ll>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utIntArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_FloatVector(s_long fc, const unity& x, std::vector<double>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utFloatArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_DateVector(s_long fc, const unity& x, std::vector<_unitydate>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utDateArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_StringVector(s_long fc, const unity& x, std::vector<std::wstring>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utStringArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
void _static_conv::conv_Unity_CharVector(s_long fc, const unity& x, std::vector<_unitychar>& retval) { cnv_all((EConvStrength)(fc & cs_mask), x._uttf(), utCharArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }

//========================================================================

void _static_conv::conv_Map_Map(s_long fc, const unity::t_map& x, unity::t_map_tu& retval)
{
  retval.ordhs_set0_nexc();
  typedef const unity::t_map_tu Ttu;
  retval = (Ttu&)x;
  if (retval.nexc() == 0) { return; }
  if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Map_Map"); }
  retval.ordhs_clear();
}
void _static_conv::conv_Map_Hash(s_long fc, const unity::t_map& x, unity::t_hash& retval)
  { retval.hl_clear(true); bool b = retval.hl_set_kf(*x.pkf()); if (b) { try { for (s_long i = 0; i < x.n(); ++i) { const unity::t_map::entry* e = x(i); retval[e->k] = e->v; } } catch (...) { b = false; } } if (b) { return; } if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Map_Hash"); } retval.hl_clear(true); }
void _static_conv::conv_Map_Unity(s_long fc, const unity::t_map& x, unity& retval)
  {
    retval.clear(); unity::_stg_u st;
    if (cv_ff::cv_create::Lcreate(retval.pmsm, &st, utMap | unity::xfPtr, 0) > 0)
      { retval._data = st; retval.ut = utMap | unity::xfPtr; conv_Map_Map(fc, x, *retval._m()); return; }
    if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Map_Unity"); }
    retval.clear();
  }

void _static_conv::conv_Hash_Map(s_long fc, const unity::t_hash& x, unity::t_map_tu& retval)
  { retval.ordhs_clear(); bool b = retval.ordhs_set_kf(*x.pkf()); if (b) { try { for (s_long i = 0; i < x.n(); ++i) { const unity::t_hash::entry* e = x(i); retval[e->k] = e->v; } } catch (...) { b = false; } } if (b) { return; } if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Hash_Map"); } retval.ordhs_clear(); }
void _static_conv::conv_Hash_Hash(s_long fc, const unity::t_hash& x, unity::t_hash& retval)
{
  typedef const unity::t_hash Ttu;
  try { retval.assign_impl((Ttu&)x); return; } catch (...) {}
  if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Hash_Hash"); }
  retval.hl_clear(true);
}
void _static_conv::conv_Hash_Unity(s_long fc, const unity::t_hash& x, unity& retval)
  {
    retval.clear(); unity::_stg_u st;
    if (cv_ff::cv_create::Lcreate(retval.pmsm, &st, utHash | unity::xfPtr, 0) > 0)
      { retval._data = st; retval.ut = utHash | unity::xfPtr; conv_Hash_Hash(fc, x, *retval._h()); return; }
    if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Hash_Unity"); }
    retval.clear();
  }

void _static_conv::conv_Map_String(s_long fc, const unity::t_map& x, std::wstring& retval)
{
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utMap n=" << x.n() << " f=" << (x.pkf() ? x.pkf()->flags() : 0);
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
      buf += wLF;
    for (s_long i = 0; i < x.n(); ++i)
    {
      const unity::t_map::entry* e = x(i);
      std::wstring s2 = e->k.val<utString>(), s3 = e->v.val<utString>();
      buf2.resize(0); buf2 << i + 1 << ' ' << s2.length() << ' ' << s3.length() << ' ';
        buf += buf2.wstr();
      buf += s2; buf += L' '; buf += s3; buf += wLF;
    }
    retval = buf;
    return;
  } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_Hash_String(s_long fc, const unity::t_hash& x, std::wstring& retval)
{
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utHash n=" << x.n() << " f=" << (x.pkf() ? x.pkf()->flags() : 0);
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
      buf += wLF;
    s_long pos = x.qi_next(x.qi_noel());
    for (s_long i = 0; i < x.n(); ++i)
    {
      const unity::t_hash::entry* e = x.h(pos);
      std::wstring s2 = e->k.val<utString>(), s3 = e->v.val<utString>();
      buf2.resize(0); buf2 << i + 1 << ' ' << s2.length() << ' ' << s3.length() << ' ';
        buf += buf2.wstr();
      buf += s2; buf += L' '; buf += s3; buf += wLF;
      pos = x.qi_next(pos);
    }
    retval = buf;
    return;
  } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } }
  retval.clear();
}

//========================================================================

void _static_conv::conv_UnityArray_Int(s_long fc, const vec2_t<unity>& x, meta::s_ll& retval) { EConvStrength cs = (EConvStrength)(fc & cs_mask); if (x.n() > 0) { retval = x.pval_0u(0)->val<utInt>(cs); } else { retval = 0; } }
void _static_conv::conv_UnityArray_Float(s_long fc, const vec2_t<unity>& x, double& retval) { EConvStrength cs = (EConvStrength)(fc & cs_mask); if (x.n() > 0) { retval = x.pval_0u(0)->val<utFloat>(cs); } else { retval = 0; } }
void _static_conv::conv_UnityArray_Date(s_long fc, const vec2_t<unity>& x, _unitydate& retval) { EConvStrength cs = (EConvStrength)(fc & cs_mask); if (x.n() > 0) { retval = x.pval_0u(0)->val<utDate>(cs); } else { retval.set(0.); } }
void _static_conv::conv_UnityArray_String(s_long fc, const vec2_t<unity>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utUnityArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
      buf += wLF;
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      std::wstring s2 = x.pc(ind)->val<utString>(cs);
      buf += ntows(ind);
      buf += L' '; buf += ntows(s2.length());
      buf += L' '; buf += s2;
      buf += wLF;
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_UnityArray_Char(s_long fc, const vec2_t<unity>& x, _unitychar& retval) { EConvStrength cs = (EConvStrength)(fc & cs_mask); if (x.n() > 0) { retval = x.pval_0u(0)->val<utChar>(cs); } else { retval = 0; } }
void _static_conv::conv_UnityArray_Unity(s_long fc, const vec2_t<unity>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_UnityArray_UnityArray(s_long fc, const vec2_t<unity>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_UnityArray_UnityArray.1"); }
}
void _static_conv::conv_UnityArray_IntArray(s_long fc, const vec2_t<unity>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utInt>(cs)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_UnityArray_FloatArray(s_long fc, const vec2_t<unity>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utFloat>(cs)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_UnityArray_DateArray(s_long fc, const vec2_t<unity>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utDate>(cs)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_UnityArray_StringArray(s_long fc, const vec2_t<unity>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utString>(cs)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_UnityArray_CharArray(s_long fc, const vec2_t<unity>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utChar>(cs)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_UnityArray_UnityVector(s_long fc, const vec2_t<unity>& x, std::vector<unity>& retval)
{
  retval.clear();
  retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
void _static_conv::conv_UnityArray_IntVector(s_long fc, const vec2_t<unity>& x, std::vector<meta::s_ll>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utInt>(cs)); }
}
void _static_conv::conv_UnityArray_FloatVector(s_long fc, const vec2_t<unity>& x, std::vector<double>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utFloat>(cs)); }
}
void _static_conv::conv_UnityArray_DateVector(s_long fc, const vec2_t<unity>& x, std::vector<_unitydate>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utDate>(cs)); }
}
void _static_conv::conv_UnityArray_StringVector(s_long fc, const vec2_t<unity>& x, std::vector<std::wstring>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utString>(cs)); }
}
void _static_conv::conv_UnityArray_CharVector(s_long fc, const vec2_t<unity>& x, std::vector<_unitychar>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utChar>(cs)); }
}

//========================================================================

void _static_conv::conv_IntArray_Int(s_long fc, const vec2_t<meta::s_ll>& x, meta::s_ll& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
void _static_conv::conv_IntArray_Float(s_long fc, const vec2_t<meta::s_ll>& x, double& retval) {  if (x.n() > 0) { conv_Int_Float(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_IntArray_Date(s_long fc, const vec2_t<meta::s_ll>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Int_Date(fc, *x.pval_0u(0), retval); } else { retval.set(0.); } }
void _static_conv::conv_IntArray_String(s_long fc, const vec2_t<meta::s_ll>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utIntArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      buf += L' '; buf += ntows(*x.pc(ind));
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_IntArray_Char(s_long fc, const vec2_t<meta::s_ll>& x, _unitychar& retval) {  if (x.n() > 0) { conv_Int_Char(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_IntArray_Unity(s_long fc, const vec2_t<meta::s_ll>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_IntArray_UnityArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_IntArray_IntArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_IntArray_IntArray.1"); }
}
void _static_conv::conv_IntArray_FloatArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  retval.clear();
  double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_IntArray_DateArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  retval.clear();
  _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Date(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_IntArray_StringArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  retval.clear();
  std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_String(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_IntArray_CharArray(s_long fc, const vec2_t<meta::s_ll>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Char(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}

void _static_conv::conv_IntArray_UnityVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<unity>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
void _static_conv::conv_IntArray_IntVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
void _static_conv::conv_IntArray_FloatVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<double>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_IntArray_DateVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<_unitydate>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Date(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_IntArray_StringVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<std::wstring>& retval)
{
  retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_String(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_IntArray_CharVector(s_long fc, const vec2_t<meta::s_ll>& x, std::vector<_unitychar>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

void _static_conv::conv_FloatArray_Int(s_long fc, const vec2_t<double>& x, meta::s_ll& retval) { if (x.n() > 0) { conv_Float_Int(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_FloatArray_Float(s_long fc, const vec2_t<double>& x, double& retval) { if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
void _static_conv::conv_FloatArray_Date(s_long fc, const vec2_t<double>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Float_Date(fc, *x.pval_0u(0),retval); } else { retval.set(0.); } }
void _static_conv::conv_FloatArray_String(s_long fc, const vec2_t<double>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utFloatArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      buf += L' '; buf += ftows(*x.pc(ind), 14, 13);
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_FloatArray_Char(s_long fc, const vec2_t<double>& x, _unitychar& retval) {  if (x.n() > 0) { conv_Float_Char(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_FloatArray_Unity(s_long fc, const vec2_t<double>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_FloatArray_UnityArray(s_long fc, const vec2_t<double>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_FloatArray_IntArray(s_long fc, const vec2_t<double>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  retval.clear();
  meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Int(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_FloatArray_FloatArray(s_long fc, const vec2_t<double>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_FloatArray_FloatArray.1"); }
}
void _static_conv::conv_FloatArray_DateArray(s_long fc, const vec2_t<double>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  retval.clear();
  _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Date(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_FloatArray_StringArray(s_long fc, const vec2_t<double>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  retval.clear();
  std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_String(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_FloatArray_CharArray(s_long fc, const vec2_t<double>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  retval.clear();
  _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Char(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}

void _static_conv::conv_FloatArray_UnityVector(s_long fc, const vec2_t<double>& x, std::vector<unity>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
void _static_conv::conv_FloatArray_IntVector(s_long fc, const vec2_t<double>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear();
  meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Int(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_FloatArray_FloatVector(s_long fc, const vec2_t<double>& x, std::vector<double>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
void _static_conv::conv_FloatArray_DateVector(s_long fc, const vec2_t<double>& x, std::vector<_unitydate>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Date(fc, *x.pc(ind), temp); retval.push_back(temp); }
}
void _static_conv::conv_FloatArray_StringVector(s_long fc, const vec2_t<double>& x, std::vector<std::wstring>& retval)
{
  retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_String(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_FloatArray_CharVector(s_long fc, const vec2_t<double>& x, std::vector<_unitychar>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

void _static_conv::conv_CharArray_Int(s_long fc, const vec2_t<_unitychar>& x, meta::s_ll& retval) {  if (x.n() > 0) { conv_Char_Int(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_CharArray_Float(s_long fc, const vec2_t<_unitychar>& x, double& retval) {  if (x.n() > 0) { conv_Char_Float(fc, *x.pval_0u(0),retval); } else { retval = 0; } }
void _static_conv::conv_CharArray_Date(s_long fc, const vec2_t<_unitychar>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Char_Date(fc, *x.pval_0u(0), retval); } else { retval.set(0.); } }
void _static_conv::conv_CharArray_String(s_long fc, const vec2_t<_unitychar>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utCharArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      buf += L' ';
      buf += ntows(s_long(*x.pc(ind)));
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_CharArray_Char(s_long fc, const vec2_t<_unitychar>& x, _unitychar& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
void _static_conv::conv_CharArray_Unity(s_long fc, const vec2_t<_unitychar>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_CharArray_UnityArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_CharArray_IntArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Int(fc,*x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_CharArray_DateArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Date(fc,*x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_CharArray_FloatArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Float(fc,*x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_CharArray_StringArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_String(fc,*x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_CharArray_CharArray(s_long fc, const vec2_t<_unitychar>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_CharArray_CharArray.1"); }
}

void _static_conv::conv_CharArray_UnityVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<unity>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
void _static_conv::conv_CharArray_IntVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Int(fc,*x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_CharArray_FloatVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<double>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_CharArray_DateVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<_unitydate>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Date(fc,*x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_CharArray_StringVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<std::wstring>& retval)
{
  retval.clear(); retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_String(fc,*x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_CharArray_CharVector(s_long fc, const vec2_t<_unitychar>& x, std::vector<_unitychar>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}

//========================================================================

void _static_conv::conv_DateArray_Int(s_long fc, const vec2_t<_unitydate>& x, meta::s_ll& retval) {  if (x.n() > 0) { conv_Date_Int(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_DateArray_Float(s_long fc, const vec2_t<_unitydate>& x, double& retval) {  if (x.n() > 0) { conv_Date_Float(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_DateArray_Date(s_long fc, const vec2_t<_unitydate>& x, _unitydate& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = _unitydate(0); } }
void _static_conv::conv_DateArray_String(s_long fc, const vec2_t<_unitydate>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utDateArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      std::wstring s2; conv_Date_String(fc, *x.pc(ind), s2);
      buf += L' '; buf += s2;
      if (ind < i9) { buf += L','; }
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_DateArray_Char(s_long fc, const vec2_t<_unitydate>& x, _unitychar& retval) { if (x.n() > 0) { conv_Date_Char(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_DateArray_Unity(s_long fc, const vec2_t<_unitydate>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_DateArray_UnityArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_DateArray_IntArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Int(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_DateArray_FloatArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  retval.clear(); retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Float(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_DateArray_DateArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_DateArray_DateArray.1"); }
}
void _static_conv::conv_DateArray_StringArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_String(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_DateArray_CharArray(s_long fc, const vec2_t<_unitydate>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}

void _static_conv::conv_DateArray_UnityVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<unity>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
void _static_conv::conv_DateArray_IntVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Int(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_DateArray_FloatVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<double>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_DateArray_DateVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<_unitydate>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
void _static_conv::conv_DateArray_StringVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<std::wstring>& retval)
{
  retval.clear(); std::wstring temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_String(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_DateArray_CharVector(s_long fc, const vec2_t<_unitydate>& x, std::vector<_unitychar>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

void _static_conv::conv_StringArray_Int(s_long fc, const vec2_t<std::wstring>& x, meta::s_ll& retval) { if (x.n() > 0) { conv_String_Int(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_StringArray_Float(s_long fc, const vec2_t<std::wstring>& x, double& retval) { if (x.n() > 0) { conv_String_Float(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_StringArray_Date(s_long fc, const vec2_t<std::wstring>& x, _unitydate& retval) { if (x.n() > 0) { conv_String_Date(fc, *x.pval_0u(0), retval); } else { retval.set(0.); } }
void _static_conv::conv_StringArray_String(s_long fc, const vec2_t<std::wstring>& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    std::wstring buf; _fls75 buf2;
    buf2 << "utStringArray n=" << x.n() << " b=" << x.nbase();
      if (x.n() > 0) { buf2 << ":"; }
      buf += buf2.wstr();
      buf += wLF;
    const s_long i9 = x.nbase() + x.n() - 1;
    for (s_long ind = x.nbase(); ind <= i9; ++ind)
    {
      buf += ntows(ind);
      buf += L' '; buf += ntows(x.pc(ind)->length());
      buf += L' '; buf += *x.pc(ind);
      buf += wLF;
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_StringArray_Char(s_long fc, const vec2_t<std::wstring>& x, _unitychar& retval) {  if (x.n() > 0) { conv_String_Char(fc, *x.pval_0u(0), retval); } else { retval = 0; } }
void _static_conv::conv_StringArray_Unity(s_long fc, const vec2_t<std::wstring>& x, unity& retval) { try { retval = x; return; } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } } retval.clear(); }

void _static_conv::conv_StringArray_UnityArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_StringArray_IntArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Int(fc, *x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_StringArray_FloatArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_StringArray_DateArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Date(fc, *x.pc(ind), temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}
void _static_conv::conv_StringArray_StringArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  retval.clear();
  if (!retval.vec2_copy(x, false)) { throw XUConvFailed("conv_StringArray_StringArray.1"); }
}
void _static_conv::conv_StringArray_CharArray(s_long fc, const vec2_t<std::wstring>& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
  retval.vec2_set_nbase(x.nbase());
}

void _static_conv::conv_StringArray_UnityVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<unity>& retval)
{
  retval.clear();
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
void _static_conv::conv_StringArray_IntVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear(); meta::s_ll temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Int(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_StringArray_FloatVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<double>& retval)
{
  retval.clear(); double temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Float(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_StringArray_DateVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<_unitydate>& retval)
{
  retval.clear(); _unitydate temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Date(fc, *x.pc(ind),temp); retval.push_back(temp); }
}
void _static_conv::conv_StringArray_StringVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<std::wstring>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
void _static_conv::conv_StringArray_CharVector(s_long fc, const vec2_t<std::wstring>& x, std::vector<_unitychar>& retval)
{
  retval.clear(); _unitychar temp;
  const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Char(fc, *x.pc(ind),temp); retval.push_back(temp); }
}


//========================================================================

void _static_conv::conv_utStringArray_Int(s_long fc, const unity& x, meta::s_ll& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_Int.1"); }
    if (x.arrsz() > 0) { retval = x.val<utInt>(x.arrlb()); return; }
  } catch (...) { if (cs != csLazy) { throw; } }
  retval = 0;
}
void _static_conv::conv_utStringArray_Float(s_long fc, const unity& x, double& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_Float.1"); }
    if (x.arrsz() > 0) { retval = x.val<utFloat>(x.arrlb()); return; }
  } catch (...) { if (cs != csLazy) { throw; } }
  retval = 0;
}
void _static_conv::conv_utStringArray_Date(s_long fc, const unity& x, _unitydate& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_Date.1"); }
    if (x.arrsz() > 0) { retval = x.val<utDate>(x.arrlb()); return; }
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.set(0.);
}
void _static_conv::conv_utStringArray_String(s_long fc, const unity& x, std::wstring& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_String.1"); }
    s_long n = x.arrsz(), lb = x.arrlb();
    std::wstring buf; _fls75 buf2;
    buf2 << "utStringArray n=" << n << " b=" << lb;
      if (n > 0) { buf2 << ":"; }
      buf += buf2.wstr();
      buf += wLF;
    const s_long ub = lb + n - 1;
    for (s_long ind = lb; ind <= ub; ++ind)
    {
      std::wstring s = x.val<utString>(ind);
      buf += ntows(ind);
      buf += L' '; buf += ntows(s.length());
      buf += L' '; buf += s;
      buf += wLF;
    }
    retval = buf;
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_Char(s_long fc, const unity& x, _unitychar& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_Char.1"); }
    if (x.arrsz() > 0) { retval = x.val<utChar>(x.arrlb()); return; }
  } catch (...) { if (cs != csLazy) { throw; } }
  retval = 0;
}
void _static_conv::conv_utStringArray_Unity(s_long fc, const unity& x, unity& retval)
{
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_Char.1"); }
    retval = x;
    return;
  } catch (...) { if ((fc & cs_mask) != csLazy) { throw; } }
  retval.clear();
}

void _static_conv::conv_utStringArray_UnityArray(s_long fc, const unity& x, vec2_t<unity, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_UnityArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utUnity>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_IntArray(s_long fc, const unity& x, vec2_t<meta::s_ll, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_IntArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utInt>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_FloatArray(s_long fc, const unity& x, vec2_t<double, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_FloatArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utFloat>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_DateArray(s_long fc, const unity& x, vec2_t<_unitydate, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_DateArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utDate>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_StringArray(s_long fc, const unity& x, vec2_t<std::wstring, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_StringArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utString>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_CharArray(s_long fc, const unity& x, vec2_t<_unitychar, __vecm_tu_selector>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_CharArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utChar>(ind)); }
    retval.vec2_set_nbase(lb);
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}

void _static_conv::conv_utStringArray_UnityVector(s_long fc, const unity& x, std::vector<unity>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_UnityArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utUnity>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_IntVector(s_long fc, const unity& x, std::vector<meta::s_ll>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_IntArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utInt>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_FloatVector(s_long fc, const unity& x, std::vector<double>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_FloatArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utFloat>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_DateVector(s_long fc, const unity& x, std::vector<_unitydate>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_DateArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utDate>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_StringVector(s_long fc, const unity& x, std::vector<std::wstring>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_StringArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utString>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}
void _static_conv::conv_utStringArray_CharVector(s_long fc, const unity& x, std::vector<_unitychar>& retval)
{
  EConvStrength cs = (EConvStrength)(fc & cs_mask);
  try {
    if (x.utype() != utStringArray) { throw XUConvFailed("conv_utStringArray_CharArray.1"); }
    retval.clear();
    s_long n = x.arrsz(), lb = x.arrlb();
    const s_long _i2 = lb + n;  for (s_long ind = lb; ind < _i2; ++ind) { retval.push_back(x.val<utChar>(ind)); }
    return;
  } catch (...) { if (cs != csLazy) { throw; } }
  retval.clear();
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// unity impl.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//enum { _trace_cnv_n = 256 };
//int _trace_cnv_ind(0);
//s_long _trace_cnv_data[4 * _trace_cnv_n] = {};
//extern void _trace_cnv_f(s_long line, bool b_enter)
//{
//  int i = _trace_cnv_ind;
//  int i0 = i % _trace_cnv_n;
//  set_be4(_trace_cnv_data, 16 * i0 + 0, 0x0f0f0f0f);
//  set_be4(_trace_cnv_data, 16 * i0 + 4, i);
//  set_be4(_trace_cnv_data, 16 * i0 + 8, line + (b_enter ? 0 : 0x10000000));
//  set_be4(_trace_cnv_data, 16 * i0 + 12, GetCurrentThreadId());
//  i += 1;
//  int i2 = _trace_cnv_ind; i2 += 1;
//  if (i != i2) { i += 1; }
//  _trace_cnv_ind = i;
//}
//struct _trace_cnv_s
//{
//  s_long line;
//  _trace_cnv_s(s_long line_) { line = line_; _trace_cnv_f(line, true); }
//  ~_trace_cnv_s() { _trace_cnv_f(line, false); }
//};

s_long unity::_def_size(s_long utt) throw()
{
  enum { sp = sizeof(void*) };
  static s_long size_table[2 * _ut_end] =
  {
    0, sizeof(meta::s_ll), sizeof(double), sizeof(_unitydate), 1, sp, sp, -1, sp, sp
    -1, sp, sp, sp, sp, sp, -1, sp, -1, -1,
  };
  return size_table[(utt & utMaskNoArray) + (utt & utArray ? _ut_end : 0)];
}

s_long unity::_compat_id(unity_common::__Psm pmsm_, s_long utype) const throw () // class method
  { meta::s_ll n = ((char*)pmsm_ - (char*)0) + utype; n ^= (n >> (32 - _xCompatShift)); return s_long(n & _xCompatMask); }
bool unity::_compat_chk() const throw()
  { if (!this) { return false; } if (pmsm == unity_common::ls_modsm) { return true; } const s_long v1 = (ut >> _xCompatShift) & _xCompatMask; const s_long v2 = _compat_id(&unity_common::ls_modsm, utype()); if (v1 == v2) { return true; } const s_long v3 = _compat_id(&unity_common::ls_modsm, -1); if (v1 == v3) { return false; } if (_compatibility() > 0) { ut = _uttf() | (v2 << _xCompatShift); return true; } ut = _uttf() | (v3 << _xCompatShift); return false; }
s_long unity::_compatibility() const throw()
{
  if (!this) { return -3; }
  if (pmsm == unity_common::ls_modsm) { return 2; }
  if (isMap())
  {
    s_long ver = ((cv_ff::cv_ver::PFver)pmsm(unity_common::msm_ver_map))();
    if ((ver & ~s_long(0xff)) == (cv_ff::cv_ver::Fver_map() & ~s_long(0xff))) { return _m()->compatibility(); }
    return -1;
  }
  if (isHash()) { return _h()->compatibility(); }
  if (isScalar())
  {
    if (utype() == utString)
    {
    std::wstring* p = reference_t<std::wstring>::deref(_data, true);
      return p ? (yk_c::typer<std::wstring, __vecm_tu_selector>().t_ind < 0 ? 1 : 0) : -1;
    }
    return 1;
  }
  if (isArray()) { return _drf_q()->compatibility(); }
  if (isObject()) { return 0; }
  return -1;
}

void unity::_x_cnv_this(EConvStrength cs, s_long utt_dest, bool keep_name)  { if (utt_dest == utype()) { return; } cv_ff::cv_convert::Lcnv_this(this, cs, utt_dest, keep_name);  }
s_long cv_ff::cv_convert::Fcnv_this(unity* p, s_long cs, s_long utt_dest, s_long keep_name) throw()
{
  if (utt_dest == p->utype()) { return 0; }
  void* _pn = keep_name && p->isNamed() ? p->_data.p2 : 0;
  if (utt_dest == utEmpty)
  {
      if (_pn) { p->_data.p2 = 0; }
    p->clear();
      if (_pn) { p->_data.p2 = _pn; p->ut |= unity::xfPtr; }
    return 1;
  }
  if (cs == csStrict) { return -1; }
  unity::_stg_u st;
  s_long uttf_dest = utt_dest; if (_pn || utt_dest >= utString) { uttf_dest |= unity::xfPtr; }
  s_long _step(0);
  try
  {
    if (cv_ff::cv_create::Fcreate(&st, uttf_dest, 0) <= 0) { return -2; }
    _step = 1;
    _static_conv::cnv_all((EConvStrength)cs, p->_uttf(), utt_dest, 0, p->isByPtr() ? p->_data.p1 : &p->_data, (uttf_dest & unity::xfPtr) ? st.p1 : &st);
    _step = 2;
  }
  catch (_XUBase&) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, uttf_dest, 0); } return -3; } }
  catch (std::exception&) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, uttf_dest, 0); } return -4; } }
  catch (...) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, uttf_dest, 0); } return -5; } }

  if (_step == 2)
  {
    if (_pn) { st.p2 = _pn; p->_data.p2 = 0; }
    cv_ff::cv_create::Fdestroy(&p->_data, p->_uttf(), 0x4);
    p->_data = st;
    p->ut = uttf_dest;
    return 1;
  }

  if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, uttf_dest, 0); }
  if (_pn) { p->_data.p2 = 0; }
  cv_ff::cv_create::Fdestroy(&p->_data, p->_uttf(), 0x4);
  p->ut = utEmpty; if (_pn) { p->ut |= unity::xfPtr; p->_data.p2 = _pn; }
  return -6;
}
  // _x_asg: see Fasg decl. for flags.
void unity::_x_asg(const unity& x, s_long flags) { cv_ff::cv_convert::Lasg(this, &x, flags); }
s_long cv_ff::cv_convert::Fasg(unity* p, const unity* px, s_long flags) throw()
{
  if (!(p && px)) { return -11; } if (px == p) { return 0; }
  if ((flags & 0x1) && p->isNamed()) { flags |= 0x100; }
  if (!(flags & 0x1) && px->isNamed()) { flags |= 0x200; }
  if (px->utype() < utString && !(flags & 0x302) && p->isScalar())
  {
    // Assign -- 1
    p->clear(); bytes::memmove_t<char>::F(&p->_data, px->isByPtr() ? px->_data.p1 : &px->_data, unity::_def_size(px->utype())); p->ut = px->utype();
    return 1;
  }
  try
  {
    void* _pn1 ((flags & 0x100) ? p->_data.p2 : 0);
    cv_ff::_pdyn_t<unity> _pn2 ((flags & 0x200) ? new unity(*px->_drf_name()) : 0);
    switch (px->utype())
    {
      case utEmpty:
        {
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->ut = utEmpty;
            if (_pn1) { p->_data.p2 = _pn1; p->ut |= unity::xfPtr; } else if (_pn2) { p->_data.p2 = _pn2.take(); p->ut |= unity::xfPtr; }
          return 1;
        }
      case utString:
        {
            // NOTE pss may be incompatible with current module.
            // It's not called directly, only through cv_wstring::L* wrappers.
          std::wstring* pss = unity::reference_t<std::wstring>::deref(px->_data, true);
          std::wstring* next = cv_ff::cv_wstring::Fnew(cv_ff::cv_wstring::Ln(px->pmsm, pss), cv_ff::cv_wstring::Lp0(px->pmsm, pss), cv_ff::cv_wstring::Lwcs(px->pmsm), 0);
          if (!next) { return -2; }
          // Assign -- 2
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->_data.p1 = next; p->ut = utString | unity::xfPtr;
            if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
          return 1;
        }
      case utObject:
        {
          unity::o_api a(px);
          unity::o_api a2 = a._ref_newcp(p->pmsm, (px->ut & unity::xfObjStrongRef) ? 1 : 2);
          if (!a2.prc) { return -9; }
          // Assign -- 3
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->_data.p1 = a2.p_head();
          p->ut = utObject | unity::xfPtr;
          if (px->ut & unity::xfObjStrongRef) { p->ut |= unity::xfObjStrongRef; }
          if (a2.pidyn) { p->ut |= unity::xfObjItfsList; }
            if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
          return 1;
        }
      default:
        {
              // NOTE Array compatibility == 0 is acceptable as well as > 0, because the array is just copied to the same type.
          bool b_inccnv = false;
          if (px->isArray()) { if (px->pmsm != unity_common::ls_modsm) { if (px->_drf_q()->compatibility() < 0) { b_inccnv = true; } } }
            else { if (!px->_compat_chk()) { return -6; } }
          unity::_stg_u st;
          s_long uttf_dest = px->utype() | unity::xfPtr;
          const volatile bool b_destptr = !!(uttf_dest & unity::xfPtr); // always true
          if (cv_ff::cv_create::Fcreate(&st, uttf_dest, 0) <= 0) { return -7; }
          bool b(false);
          if (b_inccnv) { try { _static_conv::cnv_incompat(csStrict, px->utype(), 0, *px, b_destptr ? st.p1 : &st); b = true; } catch (...) {} }
            else { try { _static_conv::cnv_all(csStrict, px->_uttf(), px->utype(), 0, px->isByPtr() ? px->_data.p1 : &px->_data, b_destptr ? st.p1 : &st); b = true; } catch (...) {} }
          if (!b) { cv_ff::cv_create::Fdestroy(&st, uttf_dest, 0); return -8; }
          // Assign -- 4
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->_data = st; p->ut = uttf_dest;
            if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
          return 1;
        } // end default
    } // end switch
  } catch (...) {}
  return -1;
}
  // _cnv_val:
  // Returns false in csLazy if failed to copy and initialize by default, so that *pdest is left with undefined value.
bool unity::_x_cnv_this_val(EConvStrength cs, s_long utt_dest, s_long flags, void* pdest) const
{
  const s_long utt_src = utype();
  const s_long uttf_src = _uttf();
  if (cs == csStrict && utt_dest != utt_src) { throw XUTypeMismatch("_cnv_val.1", _tname0(utt_src), _tname0(utt_dest)); }
  if (isScalar())
  {
    if (utt_src == utString)
    {
      if (pmsm != unity_common::ls_modsm)
      {
          // NOTE pss may be incompatible with current module.
          // It's not called directly, only through cv_wstring::L* wrappers.
        std::wstring* pss = reference_t<std::wstring>::deref(_data, true);
        if (utt_dest == utString)
        {
          std::wstring* psd = reinterpret_cast<std::wstring*>(pdest);
          if (cv_ff::cv_wstring::Fcopy(psd, cv_ff::cv_wstring::Ln(pmsm, pss), cv_ff::cv_wstring::Lp0(pmsm, pss), cv_ff::cv_wstring::Lwcs(pmsm), 0))
            { return true; }
        }
        else
        {
          std::wstring temp;
          if (cv_ff::cv_wstring::Fcopy(&temp, cv_ff::cv_wstring::Ln(pmsm, pss), cv_ff::cv_wstring::Lp0(pmsm, pss), cv_ff::cv_wstring::Lwcs(pmsm), 0))
            { try { _static_conv::cnv_all(cs, utString | xfPtr, utt_dest, flags, &temp, pdest); return true; } catch (...) {} }
        }
        if (cs == csLazy) { return false; }
        throw XUConvFailed("_cnv_val.2", _tname0(utt_src), _tname0(utt_dest));
      }
    }
  }
  else if (isArray())
  {
    if (pmsm != unity_common::ls_modsm)
    {
        // NOTE compatibility == 0 may be acceptable if the array is just copied to the same type.
        //    In other cases, compatibility > 0 is acceptable only.
      const s_long c = _drf_q()->compatibility();
      if (!(_data.p1 && (c > 0 || (c == 0 && utt_dest == utt_src && !(flags & 0x1)))))
      {
        try { _static_conv::cnv_incompat(cs, utt_dest, flags, *this, pdest); return true; }
          catch (...) { if (cs != csLazy) { throw; } }
        return false;
      }
    }
  }
  else
  {
      // NOTE utObject from another module may be safely converted with cnv_all, since o_api use is safe, and the object contents are not accessed.
    if (!_compat_chk() && !isObject()) { if (cs == csLazy) { return false; } throw XUConvFailed("_cnv_val.4", _tname0(utt_src), _tname0(utt_dest)); }
  }
  try { _static_conv::cnv_all(cs, uttf_src, utt_dest, flags, isByPtr() ? _data.p1 : &_data, pdest); return true; }
    catch (...) { if (cs != csLazy) { throw; } }
  return false;
}

  // src must be != this.
  // Effect: move src value to this, delete old value of this, set src empty.
  // Name of this is kept.
  // Moving is done by moving src p1 if possible.
  // (If objects are from different modules, or src is not by pointer, moving is done via _x_asg + src clear().)
bool unity::_xu_move_p1(unity& src)
{
  if (this == &src) { return false; }

  if (src.pmsm == pmsm && src.isByPtr())
  {
    void* _pn = (ut & xfPtr) ? _data.p2 : 0;
      if (_pn) { _data.p2 = 0; }
    clear();
    s_long uttf0 = src._uttf();
    _data.p1 = src._data.p1; src._data.p1 = 0; src.ut = utEmpty; if (src._data.p2) { src.ut |= xfPtr; }
    _data.p2 = _pn;
    ut = uttf0;
    src.clear();
  }
  else
  {
    _x_asg(src, 0x1);
    src.clear();
  }
  return true;
}

s_long unity::_Lcreate(_stg_u* pdest, s_long uttf, s_long flags) { return cv_ff::cv_create::Lcreate(pmsm, pdest, uttf, flags); }
s_long unity::_Ldestroy(_stg_u* pdest, s_long uttf, s_long flags) { return cv_ff::cv_create::Ldestroy(pmsm, pdest, uttf, flags); }
s_long unity::_Lel_get(s_long ind, unity* pst) const { return cv_ff::cv_array::Lel_get(this, pst, ind); }
s_long unity::_Lel_set(s_long ind, const unity& x) { return cv_ff::cv_array::Lel_set(this, &x, ind); }
s_long unity::_Lel_append(const unity& x) { return cv_ff::cv_array::Lel_append(this, &x); }
s_long unity::_Lel_insrem(s_long ind, s_long m) { return cv_ff::cv_array::Lel_insrem(this, 0, ind, m); }
s_long unity::_Lel_insrem(s_long ind, s_long m, const unity& x) { return cv_ff::cv_array::Lel_insrem(this, &x, ind, m); }
s_long unity::_Lu_set_name(const unity* pn) { return cv_ff::u_set_name::Lset_name(this, pn); }
s_long unity::_Ldelete_this() { return cv_ff::cv_delete::Ldelete(pmsm, this, utUnity); }
_o_itfslist* unity::_Litfslist_cr() { return (_o_itfslist*)cv_ff::cv_obj2::Litfslist_cr(pmsm); }
s_long unity::_Litfslist_del(void* p) { return cv_ff::cv_obj2::Litfslist_del(pmsm, p); }
_o_itfslist* unity::_Litfslist_cr(unity_common::__Psm pmsm) { return (_o_itfslist*)cv_ff::cv_obj2::Litfslist_cr(pmsm); }
s_long unity::_Litfslist_del(unity_common::__Psm pmsm, void* p) { return cv_ff::cv_obj2::Litfslist_del(pmsm, p); }

  // flags: see unity::u_clear() for description.
s_long unity::_Lu_clear(s_long flags) { return cv_ff::u_clear::Lu_clear(this, flags); }
s_long cv_ff::u_clear::Fu_clear(unity* p, s_long flags)
{
  if (p->isEmpty()) { return 2; }
  else if (p->isScalar())
  {
    try { _static_conv::cnv_all(csLazy, utEmpty, p->utype(), 0, 0, p->isByPtr() ? p->_data.p1 : &p->_data); } catch (...) { return 0; }
    return 2;
  }
  else
  {
    switch (p->utype())
    {
      case utObject:
      {
          void* _pn = p->_data.p2; if (_pn) { p->_data.p2 = 0; }
        cv_ff::cv_create::Fdestroy(&p->_data, p->_uttf(), 0x4); p->ut = utEmpty;
          if (_pn) { p->_data.p2 = _pn; p->ut |= unity::xfPtr; }
        return 2;
      }
      case utIntArray: { enum { utt = utInt }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utFloatArray: { enum { utt = utFloat }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utDateArray: { enum { utt = utDate }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utStringArray: { enum { utt = utString }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utCharArray: { enum { utt = utChar }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utUnityArray: { enum { utt = utUnity }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utMap: { if (flags & 0x1) { p->_m()->ordhs_clear(); return 2; } return p->_m()->remove_all() >= 0 ? 1 : -1; }
      case utHash: { return p->_h()->hl_clear(bool(flags & 0x1)) >= 0 ? 1 : -1; }
      default: { return -1; }
    }
  }
}



unity* unity::_path_u(const std::wstring& keylist, bool forced) throw()
{
  try {
    unity k;
    if (1)
    {
      _t_wz possc = keylist.find(L';');
      if (possc == nposw)
      {
        _t_wz pos1 = keylist.find_first_not_of(L' ');
        if (pos1 != nposw)
        {
          if (keylist[pos1] == '=')
          {
            _t_wz posv = keylist.find(L'|', pos1);
            std::wstring s2 = keylist; s2[pos1] = posv == nposw ? L'|' : L' '; paramline().decode1v(s2, k);
          }
          else if (keylist[pos1] == '|') { paramline().decode1v(keylist, k); }
          else
          {
            _t_wz posv = keylist.find(L'|', pos1);
            if (posv == nposw) { paramline().decode1v(keylist, k); }
            else if (pos1 > 0) { std::wstring s2 = keylist; s2[pos1 - 1] = L'|'; paramline().decode1v(s2, k); }
            else { std::wstring s2 = L"|"; s2 += keylist; paramline().decode1v(s2, k); }
          }
        }
        else { paramline().decode1v(keylist, k); }
      }
      else
      {
        std::wstring s2 = _trim(keylist, L" ", true, false); if (s2.length() == 0) { s2 = L"=|"; } else if (s2[0] == L'=') {} else if (s2[0] == L'|') { s2 = L"=" + keylist; } else { s2 = L"=|" + keylist; }
        k = paramline().decode(s2, false).hash(L"");
      }
    }
    if (!k.isArray())
    {
      if (this->isHash())
      {
        if (this->hash_locate(k) || forced) { return &this->hash(k); } else { return 0; }
      }
      else if (this->isMap())
      {
        if (this->map_locate(k) || forced) { return &this->map(k); } else { return 0; }
      }
      else if (this->utype() == utUnityArray)
      {
        meta::s_ll ind(0); try { if (k.utype() == utEmpty || k.utype() == utInt || k.utype() == utFloat) { ind = k.vint(); } else { return 0; } } catch (...) { return 0; }
        if (s_long(ind) != ind) { return 0; }
        if (this->ua_has(s_long(ind)) || forced) { return &this->ua(s_long(ind)); } else { return 0; }
      }
      else if (!forced) { return 0; }
      bool b(false);
      meta::s_ll ind(0); try { b = k.utype() == utEmpty || k.utype() == utInt || k.utype() == utFloat; if (b) { ind = k.vint(); } } catch (...) { return 0; }
      if (b) { return &this->ua(s_long(ind)); }
      return &this->hash(k);
    }
    unity* node = this;
    for (int j = k.arrlb(); j <= k.arrub(); ++j)
    {
      if (node->isHash())
      {
        if (node->hash_locate(k.ref<utUnity>(j)) || forced) { node = &node->hash(k.ref<utUnity>(j)); } else { return 0; }
        continue;
      }
      else if (node->isMap())
      {
        if (node->map_locate(k.ref<utUnity>(j)) || forced) { node = &node->map(k.ref<utUnity>(j)); } else { return 0; }
        continue;
      }
      else if (node->utype() == utUnityArray)
      {
        meta::s_ll ind(0); try { if (k.ua(j).utype() == utEmpty || k.ua(j).utype() == utInt || k.ua(j).utype() == utFloat) { ind = k.ua(j).vint(); } else { return 0; } } catch (...) { return 0; }
        if (node->ua_has(s_long(ind)) || forced) { node = &node->ua(s_long(ind)); }
        continue;
      }
      else if (!forced) { return 0; }
      bool b(false);
      meta::s_ll ind(0); try { b = k.ua(j).utype() == utEmpty || k.ua(j).utype() == utInt || k.ua(j).utype() == utFloat; if (b) { ind = k.ua(j).vint(); } } catch (...) { return 0; }
      if (b) { node = &node->ua(s_long(ind)); } else { node = &node->hash(k.ref<utUnity>(j)); }
      continue;
    }
      return node;
  } catch (...) {}
  return 0;
}

unity::unity()        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; }
unity::unity(const unity& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
unity::unity(const std::wstring& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
unity::unity(const meta::s_ll& x)        { ut = utInt; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf_c_i<utInt>() = x; }
unity::unity(const double& x)        { ut = utFloat; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf_c_i<utFloat>() = x; }
unity::unity(const _unitychar& x)        { ut = utChar; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *_drf_c_i<utChar>() = x; }
unity::unity(const _unitydate& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
unity::~unity() throw() { clear(); }
void unity::clear() throw()
{
  s_long uttf = _uttf();
  if (uttf < utString) { _data.p1 = 0; _data.p2 = 0; ut = utEmpty; return; }
  cv_ff::cv_create::Ldestroy(pmsm, &_data, uttf, 0x4);
  ut = utEmpty;
}

unity& unity::operator=(const unity& src) { _x_asg(src, 0); return *this; }
unity& unity::operator=(const std::wstring& src)
{
  std::wstring* next = cv_ff::cv_wstring::Lnew(pmsm, src.length(), cv_ff::cv_wstring::Fp0(&src), cv_ff::cv_wstring::Fwcs(), 0);
  if (next) { clear(); _data.p1 = next; ut = utString | xfPtr; return *this; }
  throw XUExec("operator=(const std::wstring&)");
}
unity& unity::operator=(const meta::s_ll& src) { clear(); ut = utInt; *_drf_c_i<utInt>() = src; return *this; }
unity& unity::operator=(const double& src) { clear(); ut = utFloat; *_drf_c_i<utFloat>() = src; return *this; }
unity& unity::operator=(const _unitydate& src) { clear(); ut = utDate; *_drf_c_i<utDate>() = src; return *this; }
unity& unity::operator=(const _unitychar& src) { clear(); ut = utChar; *_drf_c_i<utChar>() = src; return *this; }

void unity::swap(unity& x) throw()
{
  if (this == &x) { return; }
  meta::s_ll temp[1 + sizeof(unity) / 8];
  bytes::memmove_t<char>::F(temp, this, sizeof(unity));
  bytes::memmove_t<char>::F(this, &x, sizeof(unity));
  bytes::memmove_t<char>::F(&x, temp, sizeof(unity));
}

bool unity::unity_delete() throw() { return _Ldelete_this() > 0; }
unity& unity::recreate() throw() { this->~unity(); new (this) unity(); return *this; }
unity& unity::recreate_as(const unity& modsrc) throw() { this->~unity(); new (this) unity(); this->pmsm = modsrc.pmsm; return *this; }

s_long unity::arrlb() const { if (isArray()) { return cv_ff::cv_array::Llb_u(this); } throw(XUTypeMismatch("arrlb", _tname0(utype()), "array")); }
s_long unity::arrub() const { if (isArray()) { return cv_ff::cv_array::Lub_u(this); } throw(XUTypeMismatch("arrub", _tname0(utype()), "array")); }
s_long unity::arrsz()const { if (isArray()) { return cv_ff::cv_array::Lsz_u(this); } throw(XUTypeMismatch("arrsz", _tname0(utype()), "array")); }
unity& unity::arrlb_(s_long lbnd)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, lbnd, 0, 0x1);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrlb_.1", _tname0(utype()), "array"); }
  throw XUExec("arrlb_.2", _tname0(utype()));
}
unity& unity::arrsz_(s_long n)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, 0, n, 0x2);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrsz_.1", _tname0(utype()), "array"); }
  throw XUExec("arrsz_.2", _tname0(utype()));
}
unity& unity::arrub_(s_long ubnd)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, 0, ubnd, 0x6);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrub_.1", _tname0(utype()), "array"); }
  throw XUExec("arrub_.2", _tname0(utype()));
}
bool unity::arr_insrem(s_long ind, s_long m, EConvStrength cs)
{
  try
  {
    s_long res = _Lel_insrem(ind, m); if (res == 1) { return true; } if (res == -2) { throw XUExec("arr_insrem.6", _tname0(utype())); }
    throw XUExec("arr_insrem.7", _tname0(utype()));
  }
  catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
  catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_insrem.3", _tname0(utype())); }
}



std::wstring unity::vstr() const
{
  if (isAssoc() || isArray()) { std::wostringstream st; st << *this; return st.str(); }
  return val<utString>(csNormal);
}
std::wstring unity::vstr(s_long ind) const
{
  if (utype() == utUnityArray) { return ref<utUnity>(ind).vstr(); }
  return _arr_el_get<utString>::F(this, ind, csNormal);
}
meta::s_ll unity::lenstr() const throw()
{
  if (!this || !pmsm) { return -3; }
  if (isEmpty()) { return -1; }
  if (!isString()) { return -2; }
  std::wstring* p = reference_t<std::wstring>::deref(this->_data, true);
  return cv_ff::cv_wstring::Ln(pmsm, p);
}
std::string unity::vcstr() const
{
  if (isAssoc() || isArray()) { std::ostringstream st; st << *this; return st.str(); }
  return wsToBs(val<utString>(csNormal));
}
std::string unity::vcstr(s_long ind) const
{
  if (utype() == utUnityArray) { return ref<utUnity>(ind).vcstr(); }
  return wsToBs(_arr_el_get<utString>::F(this, ind, csNormal));
}
meta::s_ll unity::vint() const { return val<utInt>(csNormal); }
meta::s_ll unity::vint(s_long ind) const { return _arr_el_get<utInt>::F(this, ind, csNormal); }
s_long unity::vint_h() const { return s_long(val<utInt>(csNormal) >> 32); }
s_long unity::vint_h(s_long ind) const { return s_long(_arr_el_get<utInt>::F(this, ind, csNormal) >> 32); }
s_long unity::vint_l() const { return s_long(val<utInt>(csNormal)); }
s_long unity::vint_l(s_long ind) const { return s_long(_arr_el_get<utInt>::F(this, ind, csNormal)); }
double unity::vfp() const { return val<utFloat>(csNormal); }
double unity::vfp(s_long ind) const { return _arr_el_get<utFloat>::F(this, ind, csNormal); }
_fls75 unity::vflstr() const throw()
{
  switch (utype())
  {
    case utEmpty: return "";
    case utInt: return *_drf_c_i<utInt>();
    case utFloat: return *_drf_c_i<utFloat>();
    case utDate: { return dtofls(*_drf_c_i<utDate>()); }
    case utChar: { return "+" + _fls75(s_long((unsigned char)(*_drf_c_i<utChar>()))); }
    case utString:
    {
      std::wstring* p = _drf_c_i<utString>(); if (p) { return *p; }
      p = reference_t<std::wstring>::deref(this->_data, true);
      _fls75 s;
      try
      {
        meta::s_ll n = cv_ff::cv_wstring::Ln(pmsm, p);
        const void* p0 = cv_ff::cv_wstring::Lp0(pmsm, p);
        s_long wcsx = cv_ff::cv_wstring::Lwcs(pmsm);
        s.resize(s_long(n)); n = s.length();
        const char* pc = reinterpret_cast<const char*>(p0);
        if (wcsx == 1) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(*pc); pc += 1; } }
          else if (wcsx == 2) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 2; } }
          else if (wcsx == 0x102) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[1]) + (s_long(pc[0]) << 8)); pc += 2; } }
          else if (wcsx == 4) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 4; } }
          else if (wcsx == 0x104) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[3]) + (s_long(pc[2]) << 8)); pc += 4; } }
      } catch (...) {}
      return s;
    }
    case utObject: return _fls75("obj@") + meta::s_ll((char*)cpvoidkey() - (char*)0);
    case utMap: { if (! _compat_chk()) { break; } _fls75 s; t_map_tu& rm = *_m(); s += "map "; s += rm.n(); if (rm.pkf()->flags() != 0) { s += " f"; s += rm.pkf()->flags(); } s += ": "; for (s_long i = 0; i < rm.n(); ++i) { s += rm(i)->k.vflstr(); s += "="; s += rm(i)->v.vflstr(); if (i + 1 < rm.n()) { s += ";"; } if (s.is_full()) { break; } } return s; }
    case utHash: { if (! _compat_chk()) { break; } _fls75 s; t_hash& rh = *_h(); s += "hash "; s += rh.n(); if (rh.pkf()->flags() != 0) { s += " f"; s += rh.pkf()->flags(); } s += ": "; for (s_long i = 0; i < rh.n(); ++i) { s += rh(i)->k.vflstr(); s += "="; s += rh(i)->v.vflstr(); if (i + 1 < rh.n()) { s += ";"; } if (s.is_full()) { break; } } return s; }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utCharArray:
    case utStringArray:
    case utUnityArray:
    {
      if (! _compat_chk()) { break; }
      _fls75 s;
      try
      {
        const char* al = "IFDCS?V";
        s_long n = cv_ff::cv_array::Lsz_u(this); s_long nb = cv_ff::cv_array::Llb_u(this);
        s += "ar "; s += al[utype() - utIntArray]; s += ' '; s += n;
        if (nb != 0) { s += " b"; s += nb; }
        switch (utype())
        {
          case utUnityArray: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += ref<utUnity>(i).vflstr(); if (i + 1 < nb + n) { s += ";"; } if (s.is_full()) { break; } } } break; }
          case utStringArray: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += crefx<utString>(i).ref(); if (i + 1 < nb + n) { s += ";"; } if (s.is_full()) { break; } } } break; }
          default: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += val<utUnity>(i).vflstr(); if (i + 1 < nb + n) { s += ";"; } if (s.is_full()) { break; } } } break; }
        }
      }
      catch (...) {}
      return s;
    }

    default: break;
  }
  return _fls75("?") + _fls75(utype());
}

unity::_wr_cstring::_wr_cstring() {}
unity::_wr_cstring::_wr_cstring(const std::string& x) : t_base(x) {}
const unity::_wr_cstring::t_base& unity::_wr_cstring::cstr() const { return *this; }
std::wstring unity::_wr_cstring::wstr() const { return bsToWs(*this); }

unity::_wr_wstring::_wr_wstring() {}
unity::_wr_wstring::_wr_wstring(const std::wstring& x) : t_base(x) {}
std::string unity::_wr_wstring::cstr() const { return wsToBs(*this); }
const unity::_wr_wstring::t_base& unity::_wr_wstring::wstr() const { return *this; }

bool unity::isAssoc() const throw () { if (!this) { return false; } s_long t = utype(); return t == utHash || t == utMap; } // true if unity is utHash or utMap
bool unity::isBoolTrue() const throw () { return this && utype() == utChar && *_drf_c_i<utChar>() != 0; } // true if *this contains char != 0
bool unity::isBoolFalse() const throw () { return this && utype() == utChar && *_drf_c_i<utChar>() == 0; } // true if *this contains char == 0
bool unity::isLocal() const { return pmsm == unity_common::ls_modsm; }
bool unity::operator==(const unity& x) const { return k_eq(x, 0); }
bool unity::operator!=(const unity& x) const { return !k_eq(x, 0); }


std::wstring unity::tname(bool allowArrDim) const
{
  if (allowArrDim && isArray())
  {
    switch (utype())
    {
      case utIntArray: return cv_ff::vec2_strdim_t<utIntArray>::F(L".Int", *this);
      case utFloatArray: return cv_ff::vec2_strdim_t<utFloatArray>::F(L".Float", *this);
      case utDateArray: return cv_ff::vec2_strdim_t<utDateArray>::F(L".Date", *this);
      case utStringArray: return cv_ff::vec2_strdim_t<utStringArray>::F(L".String", *this);
      case utCharArray: return cv_ff::vec2_strdim_t<utCharArray>::F(L".Char", *this);
      case utUnityArray: return cv_ff::vec2_strdim_t<utUnityArray>::F(L".Unity", *this);
      default:
          return L".T" + ntows(utype());
    }
  }
  else if (utype() == utObject) { return bsToWs(objTypeInfo(1).ptstat()); }
  else { return bsToWs(_tname0(utype())); }
}

_fls75 unity::_tname0(s_long uttf) throw()
{
    s_long utt = uttf & xfmUtype;
    switch (utt)
    {
        case utEmpty: return ".Empty";
        case utInt: return ".Int";
        case utFloat: return ".Float";
        case utDate: return ".Date";
        case utString: return ".String";
        case utObject: return ".Object";
        case utChar: return ".Char";
        case utMap: return ".Map";
        case utHash: return ".Hash";
        case utIntArray: return ".Int[]";
        case utFloatArray: return ".Float[]";
        case utDateArray: return ".Date[]";
        case utStringArray: return ".String[]";
        case utCharArray: return ".Char[]";
        case utUnityArray: return ".Unity[]";
        default: return _fls75(".T") + _fls75(utt);
    }
}

s_long unity::k_hashf(s_long flags) const
{
  if (!isScalar() && (flags & fkcmpScalarize)) { return u_key().k_hashf(flags); }
  switch (utype())
  {
      case utEmpty: return -1;
      case utInt: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf_c_i<utInt>(), flags) : cv_ff::_kfx<meta::s_ll>::Fhash(_drf_c_i<utInt>(), flags); }
      case utFloat: return cv_ff::_kfx<double>::Fhash(_drf_c_i<utFloat>(), flags);
      case utDate: return cv_ff::_kfx<double>::Fhash(_drf_c_i<utDate>(), flags) ^ 0x999;
      case utChar: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf_c_i<utChar>(), flags) : cv_ff::_kfx<_unitychar>::Fhash(_drf_c_i<utChar>(), flags); }
      case utString:
      {
        const std::wstring* ps = _drf_c_i<utString>();
        if (ps) { return cv_ff::_kfx<std::wstring>::Fhash(ps, flags); }
        std::wstring s = this->vstr(); return cv_ff::_kfx<std::wstring>::Fhash(&s, flags);
      }
      case utObject: { const void* p = cpvoidkey(); return cv_ff::_kfx<const void*>::Fhash(&p, flags); }
      case utMap: { return cv_ff::_kfx<t_map_tu>::Fhash(_m(), flags); }
      case utHash: { return cv_ff::_kfx<t_hash>::Fhash(_h(), flags); }

      case utIntArray: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf_c_ai<utIntArray>(), flags) : cv_ff::_kfx<meta::s_ll>::Fhash(_drf_c_ai<utIntArray>(), flags); }
      case utFloatArray: return cv_ff::_kfx<double>::Fhash(_drf_c_ai<utFloatArray>(), flags);
      case utDateArray: return cv_ff::_kfx<double>::Fhash(_drf_c_ai<utDateArray>(), flags);
      case utStringArray:
      {
        vec2_t<std::wstring, __vecm_tu_selector>* pvec = _drf_c_ai<utStringArray>();
        if (isLocal()) { return cv_ff::_kfx<std::wstring>::Fhash(pvec, flags); }
        if (pvec && pvec->compatibility() > 0) { return cv_ff::_kfx<std::wstring>::Fhash(pvec, flags); }
        unity v2 = *this; // attempt to create local copy
        return v2.k_hashf(flags);
      }
      case utCharArray: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf_c_ai<utCharArray>(), flags) : cv_ff::_kfx<_unitychar>::Fhash(_drf_c_ai<utCharArray>(), flags); }
      case utUnityArray: return cv_ff::_kfx<unity>::Fhash(_drf_c_ai<utUnityArray>(), flags);
      default: return -2 - utype();
  }
}

bool unity::k_eq(const unity& x, s_long flags) const
{
    if (this == &x) { return true; }
    if (flags & fkcmpScalarize)
    {
      if (!isScalar()) { return u_key().k_eq(x, flags); }
      if (!x.isScalar()) { return k_eq(x.u_key(), flags); }
    }
    switch (utype())
    {
        case utEmpty:
          { return x.isEmpty(); }
        case utInt:
            switch (x.utype())
            {
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utInt>(), x._drf_c_i<utInt>(), flags); } return cv_ff::_kfx<meta::s_ll>::Feq(_drf_c_i<utInt>(), x._drf_c_i<utInt>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utInt>(), x._drf_c_i<utFloat>(), flags); } return false; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utInt>(), x._drf_c_i<utChar>(), flags); } return false; }
              default: return false;
            }
        case utFloat:
            switch (x.utype())
            {
              case utFloat: { return cv_ff::_kfx<double>::Feq(_drf_c_i<utFloat>(), x._drf_c_i<utFloat>(), flags); }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utFloat>(), x._drf_c_i<utInt>(), flags); } return false;  }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utFloat>(), x._drf_c_i<utChar>(), flags); } return false;  }
              default: return false;
            }
        case utChar:
            switch (x.utype())
            {
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utChar>(), x._drf_c_i<utChar>(), flags); } return cv_ff::_kfx<_unitychar>::Feq(_drf_c_i<utChar>(), x._drf_c_i<utChar>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utChar>(), x._drf_c_i<utFloat>(), flags); } return false; }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utChar>(), x._drf_c_i<utInt>(), flags); } return false; }
              default: return false;
            }
        case utString:
          {
            if (x.utype() != utString) { return false; }
            const std::wstring* ps1 = _drf_c_i<utString>();
            const std::wstring* ps2 = x._drf_c_i<utString>();
            if (ps1)
            {
              if (ps2) { return cv_ff::_kfx<std::wstring>::Feq(ps1, ps2, flags); }
              std::wstring x2 = x.vstr();
              return cv_ff::_kfx<std::wstring>::Feq(ps1, &x2, flags);
            }
            else
            {
              std::wstring x1 = this->vstr();
              if (ps2) { return cv_ff::_kfx<std::wstring>::Feq(&x1, ps2, flags); }
              std::wstring x2 = x.vstr();
              return cv_ff::_kfx<std::wstring>::Feq(&x1, &x2, flags);
            }
          }
        case utDate:
          { if (x.utype() == utDate) { return cv_ff::_kfx<double>::Feq(_drf_c_i<utDate>(), x._drf_c_i<utDate>(), flags); } return false; }

        case utObject:
          { if (x.utype() == utObject) { return cpvoidkey() == x.cpvoidkey(); } return false; }

        case utMap: { if (x.utype() == utMap) { return !!cv_ff::_kfx<t_map_tu>::Feq(_m(), x._m(), flags); } return false; }
        case utHash: { if (x.utype() == utHash) { return !!cv_ff::_kfx<t_hash>::Feq(_h(), x._h(), flags); } return false; }

        case utIntArray:
            switch (x.utype())
            {
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utIntArray>(), x._drf_c_ai<utIntArray>(), flags); } return cv_ff::_kfx<meta::s_ll>::Feq(_drf_c_ai<utIntArray>(), x._drf_c_ai<utIntArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utIntArray>(), x._drf_c_ai<utFloatArray>(), flags); } return false; }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utIntArray>(), x._drf_c_ai<utCharArray>(), flags); } return false; }
              default: return false;
            }
        case utFloatArray:
            switch (x.utype())
            {
              case utFloatArray: { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utFloatArray>(), flags); }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utIntArray>(), flags); } return false;  }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utCharArray>(), flags); } return false;  }
              default: return false;
            }
        case utCharArray:
            switch (x.utype())
            {
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utCharArray>(), x._drf_c_ai<utCharArray>(), flags); } return cv_ff::_kfx<_unitychar>::Feq(_drf_c_ai<utCharArray>(), x._drf_c_ai<utCharArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utCharArray>(), x._drf_c_ai<utFloatArray>(), flags); } return false; }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utCharArray>(), x._drf_c_ai<utIntArray>(), flags); } return false; }
              default: return false;
            }
        case utDateArray:
          { if (x.utype() == utDateArray) { return cv_ff::_kfx<double>::Feq(_drf_c_ai<utDateArray>(), x._drf_c_ai<utDateArray>(), flags); } return false; }
        case utStringArray:
          {
            if (x.utype() != utStringArray) { return false; }
            typedef vec2_t<std::wstring, __vecm_tu_selector> A;
            const A* pvec1 = _drf_c_ai<utStringArray>();
            const A* pvec2 = x._drf_c_ai<utStringArray>();
            if (pvec1)
            {
              if (pvec2) { return cv_ff::_kfx<std::wstring>::Feq(pvec1, pvec2, flags); }
              A x2(x.val<utStringArray>());
              return cv_ff::_kfx<std::wstring>::Feq(pvec1, &x2, flags);
            }
            else
            {
              A x1(this->val<utStringArray>());
              if (pvec2) { return cv_ff::_kfx<std::wstring>::Feq(&x1, pvec2, flags); }
              A x2(x.val<utStringArray>());
              return cv_ff::_kfx<std::wstring>::Feq(&x1, &x2, flags);
            }
          }
        case utUnityArray:
          { if (x.utype() == utUnityArray) { return cv_ff::_kfx<unity>::Feq(_drf_c_ai<utUnityArray>(), x._drf_c_ai<utUnityArray>(), flags); } return false; }
        default: throw XUExec("k_eq", utype());
    }
}

bool unity::k_less(const unity& x, s_long flags) const
{
    if (this == &x) { return false; }
    if (flags & fkcmpScalarize)
    {
      if (!isScalar()) { return u_key().k_less(x, flags); }
      if (!x.isScalar()) { return k_less(x.u_key(), flags); }
    }
    if (flags & fkcmpRevOrder) { return x.k_less(*this, flags & ~fkcmpRevOrder); }
    switch (utype())
    {
        case utEmpty:
          { return !x.isEmpty(); }
        case utInt:
            switch (x.utype())
            {
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utInt>(), x._drf_c_i<utInt>(), flags); } return cv_ff::_kfx<meta::s_ll>::Fless12(_drf_c_i<utInt>(), x._drf_c_i<utInt>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utInt>(), x._drf_c_i<utFloat>(), flags); } return utInt < utFloat; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utInt>(), x._drf_c_i<utChar>(), flags); } return utInt < utChar; }
              default: return utInt < x.utype();
            }
        case utFloat:
            switch (x.utype())
            {
              case utFloat: { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utFloat>(), x._drf_c_i<utFloat>(), flags); }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utFloat>(), x._drf_c_i<utInt>(), flags); } return utFloat < utInt; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utFloat>(), x._drf_c_i<utChar>(), flags); } return utFloat < utChar; }
              default: return utFloat < x.utype();
            }
        case utChar:
            switch (x.utype())
            {
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utChar>(), x._drf_c_i<utChar>(), flags); } return cv_ff::_kfx<_unitychar>::Fless12(_drf_c_i<utChar>(), x._drf_c_i<utChar>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utChar>(), x._drf_c_i<utFloat>(), flags); } return utChar < utFloat; }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utChar>(), x._drf_c_i<utInt>(), flags); } return utChar < utInt; }
              default: return utChar < x.utype();
            }
        case utString:
          {
            if (x.utype() != utString) { return utString < x.utype(); }
            const std::wstring* ps1 = _drf_c_i<utString>();
            const std::wstring* ps2 = x._drf_c_i<utString>();
            if (ps1)
            {
              if (ps2) { return cv_ff::_kfx<std::wstring>::Fless12(ps1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Fless12(ps1, &x2, flags);
            }
            else
            {
              std::wstring x1 = this->vstr();
              if (ps2) { return cv_ff::_kfx<std::wstring>::Fless12(&x1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Fless12(&x1, &x2, flags);
            }
          }
        case utDate:
          { if (x.utype() == utDate) { return cv_ff::_kfx<double>::Fless12(_drf_c_i<utDate>(), x._drf_c_i<utDate>(), flags); } return utDate < x.utype(); }

        case utObject:
          { if (x.utype() == utObject) { return cpvoidkey() < x.cpvoidkey(); } return utObject < x.utype(); }

        case utMap: { if (x.utype() == utMap) { return cv_ff::_kfx<t_map_tu>::Fless12(_m(), x._m(), flags); } return utMap < x.utype(); }
        case utHash: { if (x.utype() == utHash) { return cv_ff::_kfx<t_hash>::Fless12(_h(), x._h(), flags); } return utHash < x.utype(); }

        case utIntArray:
            switch (x.utype())
            {
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utIntArray>(), x._drf_c_ai<utIntArray>(), flags); } return cv_ff::_kfx<meta::s_ll>::Fless12(_drf_c_ai<utIntArray>(), x._drf_c_ai<utIntArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utIntArray>(), x._drf_c_ai<utFloatArray>(), flags); } return utIntArray < utFloatArray; }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utIntArray>(), x._drf_c_ai<utCharArray>(), flags); } return utIntArray < utCharArray; }
              default: return utIntArray < x.utype();
            }
        case utFloatArray:
            switch (x.utype())
            {
              case utFloatArray: { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utFloatArray>(), flags); }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utIntArray>(), flags); } return utFloatArray < utIntArray;  }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utFloatArray>(), x._drf_c_ai<utCharArray>(), flags); } return utFloatArray < utCharArray;  }
              default: return utFloatArray < x.utype();
            }
        case utCharArray:
            switch (x.utype())
            {
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utCharArray>(), x._drf_c_ai<utCharArray>(), flags); } return cv_ff::_kfx<_unitychar>::Fless12(_drf_c_ai<utCharArray>(), x._drf_c_ai<utCharArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utCharArray>(), x._drf_c_ai<utFloatArray>(), flags); } return utCharArray < utFloatArray; }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utCharArray>(), x._drf_c_ai<utIntArray>(), flags); } return utCharArray < utIntArray; }
              default: return utCharArray < x.utype();
            }
        case utDateArray:
          { if (x.utype() == utDateArray) { return cv_ff::_kfx<double>::Fless12(_drf_c_ai<utDateArray>(), x._drf_c_ai<utDateArray>(), flags); } return utDateArray < x.utype(); }
        case utStringArray:
          {
            typedef vec2_t<std::wstring, __vecm_tu_selector> A;
            if (x.utype() == utStringArray)
            {
              const A* pvec1 = _drf_c_ai<utStringArray>();
              const A* pvec2 = x._drf_c_ai<utStringArray>();
              if (pvec1)
              {
                if (pvec2) { return cv_ff::_kfx<std::wstring>::Fless12(pvec1, pvec2, flags); }
                A x2(x.val<utStringArray>());
                return cv_ff::_kfx<std::wstring>::Fless12(pvec1, &x2, flags);
              }
              else
              {
                A x1(this->val<utStringArray>());
                if (pvec2) { return cv_ff::_kfx<std::wstring>::Fless12(&x1, pvec2, flags); }
                A x2(x.val<utStringArray>());
                return cv_ff::_kfx<std::wstring>::Fless12(&x1, &x2, flags);
              }
            }
            return utStringArray < x.utype();
          }
        case utUnityArray:
          { if (x.utype() == utUnityArray) { return cv_ff::_kfx<unity>::Fless12(_drf_c_ai<utUnityArray>(), x._drf_c_ai<utUnityArray>(), flags); } return utUnityArray < x.utype(); }
        default: throw XUExec("k_less", utype());
    }
}

const unity unity::_0;
unity unity::_0nc;

unity& unity::array0( _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30 )
{
  arr_init<utUnity>(0);
  if (&x1!=&_0 && &x1!=this) arr_append(x1); if (&x2!=&_0 && &x2!=this) arr_append(x2); if (&x3!=&_0 && &x3!=this) arr_append(x3); if (&x4!=&_0 && &x4!=this) arr_append(x4); if (&x5!=&_0 && &x5!=this) arr_append(x5); if (&x6!=&_0 && &x6!=this) arr_append(x6); if (&x7!=&_0 && &x7!=this) arr_append(x7); if (&x8!=&_0 && &x8!=this) arr_append(x8); if (&x9!=&_0 && &x9!=this) arr_append(x9); if (&x10!=&_0 && &x10!=this) arr_append(x10); if (&x11!=&_0 && &x11!=this) arr_append(x11); if (&x12!=&_0 && &x12!=this) arr_append(x12); if (&x13!=&_0 && &x13!=this) arr_append(x13); if (&x14!=&_0 && &x14!=this) arr_append(x14); if (&x15!=&_0 && &x15!=this) arr_append(x15); if (&x16!=&_0 && &x16!=this) arr_append(x16); if (&x17!=&_0 && &x17!=this) arr_append(x17); if (&x18!=&_0 && &x18!=this) arr_append(x18); if (&x19!=&_0 && &x19!=this) arr_append(x19); if (&x20!=&_0 && &x20!=this) arr_append(x20); if (&x21!=&_0 && &x21!=this) arr_append(x21); if (&x22!=&_0 && &x22!=this) arr_append(x22); if (&x23!=&_0 && &x23!=this) arr_append(x23); if (&x24!=&_0 && &x24!=this) arr_append(x24); if (&x25!=&_0 && &x25!=this) arr_append(x25); if (&x26!=&_0 && &x26!=this) arr_append(x26); if (&x27!=&_0 && &x27!=this) arr_append(x27); if (&x28!=&_0 && &x28!=this) arr_append(x28); if (&x29!=&_0 && &x29!=this) arr_append(x29); if (&x30!=&_0 && &x30!=this) arr_append(x30);
  return *this;
}
unity& unity::array( _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30 )
{
  arr_init<utUnity>(1);
  if (&x1!=&_0 && &x1!=this) arr_append(x1); if (&x2!=&_0 && &x2!=this) arr_append(x2); if (&x3!=&_0 && &x3!=this) arr_append(x3); if (&x4!=&_0 && &x4!=this) arr_append(x4); if (&x5!=&_0 && &x5!=this) arr_append(x5); if (&x6!=&_0 && &x6!=this) arr_append(x6); if (&x7!=&_0 && &x7!=this) arr_append(x7); if (&x8!=&_0 && &x8!=this) arr_append(x8); if (&x9!=&_0 && &x9!=this) arr_append(x9); if (&x10!=&_0 && &x10!=this) arr_append(x10); if (&x11!=&_0 && &x11!=this) arr_append(x11); if (&x12!=&_0 && &x12!=this) arr_append(x12); if (&x13!=&_0 && &x13!=this) arr_append(x13); if (&x14!=&_0 && &x14!=this) arr_append(x14); if (&x15!=&_0 && &x15!=this) arr_append(x15); if (&x16!=&_0 && &x16!=this) arr_append(x16); if (&x17!=&_0 && &x17!=this) arr_append(x17); if (&x18!=&_0 && &x18!=this) arr_append(x18); if (&x19!=&_0 && &x19!=this) arr_append(x19); if (&x20!=&_0 && &x20!=this) arr_append(x20); if (&x21!=&_0 && &x21!=this) arr_append(x21); if (&x22!=&_0 && &x22!=this) arr_append(x22); if (&x23!=&_0 && &x23!=this) arr_append(x23); if (&x24!=&_0 && &x24!=this) arr_append(x24); if (&x25!=&_0 && &x25!=this) arr_append(x25); if (&x26!=&_0 && &x26!=this) arr_append(x26); if (&x27!=&_0 && &x27!=this) arr_append(x27); if (&x28!=&_0 && &x28!=this) arr_append(x28); if (&x29!=&_0 && &x29!=this) arr_append(x29); if (&x30!=&_0 && &x30!=this) arr_append(x30);
  return *this;
}
unity choose(s_long ind, const unity& x1, const unity& x2, const unity& x3, const unity& x4, const unity& x5, const unity& x6, const unity& x7, const unity& x8, const unity& x9, const unity& x10, const unity& x11, const unity& x12, const unity& x13, const unity& x14, const unity& x15, const unity& x16, const unity& x17, const unity& x18, const unity& x19, const unity& x20, const unity& x21, const unity& x22, const unity& x23, const unity& x24, const unity& x25, const unity& x26, const unity& x27, const unity& x28, const unity& x29, const unity& x30)
{
  const unity& _0 = unity::_0;
  switch (ind)
  {
    case 1: return &x1!=&_0?x1:unity(); case 2: return &x2!=&_0?x2:unity(); case 3: return &x3!=&_0?x3:unity(); case 4: return &x4!=&_0?x4:unity(); case 5: return &x5!=&_0?x5:unity(); case 6: return &x6!=&_0?x6:unity(); case 7: return &x7!=&_0?x7:unity(); case 8: return &x8!=&_0?x8:unity(); case 9: return &x9!=&_0?x9:unity(); case 10: return &x10!=&_0?x10:unity(); case 11: return &x11!=&_0?x11:unity(); case 12: return &x12!=&_0?x12:unity(); case 13: return &x13!=&_0?x13:unity(); case 14: return &x14!=&_0?x14:unity(); case 15: return &x15!=&_0?x15:unity(); case 16: return &x16!=&_0?x16:unity(); case 17: return &x17!=&_0?x17:unity(); case 18: return &x18!=&_0?x18:unity(); case 19: return &x19!=&_0?x19:unity(); case 20: return &x20!=&_0?x20:unity(); case 21: return &x21!=&_0?x21:unity(); case 22: return &x22!=&_0?x22:unity(); case 23: return &x23!=&_0?x23:unity(); case 24: return &x24!=&_0?x24:unity(); case 25: return &x25!=&_0?x25:unity(); case 26: return &x26!=&_0?x26:unity(); case 27: return &x27!=&_0?x27:unity(); case 28: return &x28!=&_0?x28:unity(); case 29: return &x29!=&_0?x29:unity(); case 30: return &x30!=&_0?x30:unity();
    default: return unity();
  }
}





















































unity unity::u_key(EConvStrength cs) const { unity u; u_key_get(u, cs); return u; }
const unity& unity::u_key_get(unity& dest, EConvStrength cs) const
{
  try {
    if (utype() <= utString) { dest._x_asg(*this, 0); return *this; }
    if (isArray()) { dest = L"?array/" + tname(); return *this; }
    if (isObject()) { const unity* p = objPtr_c<unity>(true); dest = p ? L"?unity/" + p->u_name().vstr() : bsToWs(std::string("?object/") + objTypeInfo(1).ptstat()); return *this; }
    dest = L"?value/" + tname();
    return *this;
  }
  catch (_XUBase&) { if (cs == csLazy) { dest.clear(); return *this; } throw; }
  catch (...) { if (cs == csLazy) { dest.clear(); return *this; } throw XUExec("u_key_get.1", dest.vflstr()); }
}
unity unity::u_name() const { if (isNamed()) { return unity(*_drf_name()); } return unity(); }
unity& unity::u_name_set(const unity& name)       { s_long res = _Lu_set_name(&name); if (res > 0) { return *this; } throw XUExec("u_name_set"); }
unity unity::u_copy() const        { unity x; x._x_asg(*this, 0x1); return x; }
unity& unity::u_assign(const unity& x)        { _x_asg(x, 0x1); return *this; }
unity& unity::u_clear(s_long utt, s_long flags, EConvStrength cs)
{
  // utt changed: create st, clear this, assign st.
  // utt changed for named: create ptr st, backup name, clear this, assign st, assign name.
  // utt unchanged: call _Lu_clear.
  if (utt == -1 || utt == utype()) { if (_Lu_clear(flags) > 0) { return *this; } else { throw XUExec("u_clear.1"); } }
  else if (utt >= 0) // create new object
  {
    if (cs == csStrict) { throw XUTypeMismatch("u_clear.4", _tname0(utt), _tname0(utype())); }
    _stg_u st;
    void* _pn = (ut & xfPtr) ? _data.p2 : 0;
    s_long uttf_dest = utt; if (_pn || utt >= utString) { uttf_dest |= xfPtr; }
    if (_Lcreate(&st, uttf_dest, 0) > 0)
    {
        if (_pn) { _data.p2 = 0; }
      clear(); _data = st; ut = uttf_dest;
        if (_pn) { _data.p2 = _pn; }
      return *this;
    }
    else { throw XUExec("u_clear.2"); }
  } else { throw XUExec("u_clear.3"); }
}




bool unity::sc_empty() const        { return isEmpty() || !isScalar(); }
unity& unity::sc_ensure()        { _ensure_sc(); return *this; }
unity& unity::sc_clear()        { if (!isEmpty()) { u_clear(utEmpty); } return *this; }
unity& unity::ua_ensure()        { if (!isArray()) { u_clear(utUnityArray); } return *this; }
unity& unity::ua_clear(bool full)        { return u_clear(isArray() ? -1 : utUnityArray, full ? 0x1 : 0); } // true resets all, false only removes elements
bool unity::ua_has(s_long ind) const { if (!isArray()) { return false; } s_long n = cv_ff::cv_array::Lsz_u(this); s_long nb = cv_ff::cv_array::Llb_u(this); ind -= nb; return ind >= 0 && ind < n; }
bool unity::ua_canref(s_long ind) const { if (!isArray()) { return pmsm == unity_common::ls_modsm; } return utype() == utUnityArray && _compat_chk(); }
bool unity::ua_empty(s_long ind) const { if (!isArray()) { return true; } if (utype() != utUnityArray) { return false; } typedef unity::valtype_t<utUnityArray>::t_tu Ttu; unity* p = checked_ptr<Ttu>(_drf_c_ai<utUnityArray>())->pval(ind); return !p || p->isEmpty(); }
unity& unity::ua(s_long ind) { if (!isArray()) { u_clear(utUnityArray); } if (utype() == utUnityArray && cv_ff::cv_array::Lua_ensure_ind(this, ind) >= 0) { typedef unity::valtype_t<utUnityArray>::t_tu Ttu; return *checked_ptr<Ttu>(_drf_c_ai<utUnityArray>())->pc(ind); } throw XUExec("ua"); }
unity& unity::ua_first() { if (!isArray()) { u_clear(utUnityArray); } unity* px = ref<utUnityArray>().pval_first(); if (px) { return *px; } throw XUExec("ua_first.1"); }
unity& unity::ua_last() { if (!isArray()) { u_clear(utUnityArray); } unity* px = ref<utUnityArray>().pval_last(); if (px) { return *px; } throw XUExec("ua_last.1"); }
unity unity::ua_val(s_long ind) { if (!isArray()) { u_clear(utUnityArray); } cv_ff::cv_array::Lua_ensure_ind(this, ind); return val<utUnity>(ind); }
unity unity::ua_val_first() { if (!isArray()) { u_clear(utUnityArray); } return val<utUnity>(cv_ff::cv_array::Llb_u(this)); }
unity unity::ua_val_last() { if (!isArray()) { u_clear(utUnityArray); } return val<utUnity>(cv_ff::cv_array::Lub_u(this)); }
unity& unity::ua_set(s_long ind, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } cv_ff::cv_array::Lua_ensure_ind(this, ind); arr_set(ind, v); return *this; }
unity& unity::ua_append(const unity& v) { if (!isArray()) { u_clear(utUnityArray); } arr_append(v); return *this; }
s_long unity::uaS() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Lsz_u(this); }
s_long unity::uaLb() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Llb_u(this); }
s_long unity::uaUb() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Lub_u(this); }
unity& unity::uaS_set(s_long new_S) { if (!isArray()) { u_clear(utUnityArray); } if (cv_ff::cv_array::Lsz_set(this, 0, new_S, 0x2) > 0) { return *this; } throw XUExec("uaS_set.1"); }
unity& unity::uaS_set(s_long new_S, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } s_long n = cv_ff::cv_array::Lsz_u(this); s_long delta = new_S - n; if (!delta) { return *this; } s_long nb = cv_ff::cv_array::Llb_u(this); if (cv_ff::cv_array::Lel_insrem(this, &v, nb + n + (delta > 0 ? 0 : delta), delta) > 0) { return *this; } throw XUExec("uaS_set.2"); }
unity& unity::ua_resize(s_long ind, s_long m) { if (!isArray()) { u_clear(utUnityArray); } if (m == 0) { return *this; } s_long q1 = cv_ff::cv_array::Llb_u(this); s_long q2 = q1 + cv_ff::cv_array::Lsz_u(this); if (m > 0) { if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } } else { s_long ind2 = ind - m; if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } if (ind2 < q1) { ind2 = q1; } else if (ind2 > q2) { ind2 = q2; } m = ind - ind2; if (m == 0) { return *this; } } if (cv_ff::cv_array::Lel_insrem(this, 0, ind, m) > 0) { return *this; } throw XUExec("ua_resize.1"); }
unity& unity::ua_resize(s_long ind, s_long m, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } if (m == 0) { return *this; } s_long q1 = cv_ff::cv_array::Llb_u(this); s_long q2 = q1 + cv_ff::cv_array::Lsz_u(this); if (m > 0) { if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } } else { s_long ind2 = ind - m; if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } if (ind2 < q1) { ind2 = q1; } else if (ind2 > q2) { ind2 = q2; } m = ind - ind2; if (m == 0) { return *this; } } if (cv_ff::cv_array::Lel_insrem(this, &v, ind, m) > 0) { return *this; } throw XUExec("ua_resize.2"); }
unity& unity::uaLb_set(s_long new_L) { if (!isArray()) { u_clear(utUnityArray); } if (cv_ff::cv_array::Lsz_set(this, new_L, 0, 0x1) > 0) { return *this; } throw XUExec("uaLb_set.1"); }
unity& unity::ua_fill(s_long utt, const unity& v) { s_long res = cv_ff::cv_array::Lua_fill(this, &v, utt); if (res >= 0) { return *this; } std::string e("ua_fill.A"); e[e.size() - 1] = char(s_long('A') + (-res % 26)); throw XUExec(e); }






namespace
{
struct _mx : unity::t_map_tu
{
  typedef meta::assert<(no_elem == -1)>::t_true __check1;

  void __rev_inds() { s_long m = n() / 2; for (s_long i = 0; i < m; ++i) { s_long* p1 = _inds.pval_0u<s_long>(i); s_long* p2 = _inds.pval_0u<s_long>(n() - i - 1); s_long temp = *p1; *p1 = *p2; *p2 = temp; } }
    // 1 - k was inserted, v assigned.
    // 0 - k existed, v assigned on keep_first false, or ignored on keep_first true.
    // -1 - an error occured, the map did not change.
    // NOTE if appending breaks the order, normal insertion occurs.
  s_long __append(bool at_front, const unity& k, const unity& v, bool keep_first)
  {
    const entry* e(0); s_long i1(-1); bool r2(false);
    do {
      if (n() > 0)
      {
        if (at_front)
        {
          e = _d(*_inds.pval_first<s_long>()); if (!e) { return -1; }
          if (pkf()->less12(k, e->k))
          {
            if (_d.insert(k, &e) != 1) { e = 0; break; } r2 = true;
            if (_inds.el_insert_1<s_long>(0, _inds.n()) != 1) { break; }
            i1 = 0;
          }
          else if (!pkf()->less12(e->k, k)) { i1 = 0; }
          else { e = 0; }
        }
        else
        {
          e = _d(*_inds.pval_last<s_long>()); if (!e) { return -1; }
          if (pkf()->less12(e->k, k))
          {
            if (_d.insert(k, &e) != 1) { e = 0; break; } r2 = true;
            if (_inds.el_insert_1<s_long>(_inds.n(), _inds.n()) != 1) { break; }
            i1 = _inds.n() - 1;
          }
          else if (!pkf()->less12(k, e->k)) { i1 = n() - 1; }
          else { e = 0; }
        }
      }
    } while (false);
    if (e)
    {
      if (i1 < 0) { _d.remove_e(e); return -1; }
      if (keep_first && !r2) { return 0; }
      try { e->v = v; } catch (...) { if (r2) { _d.remove_e(e); _inds.el_remove_1<s_long>(i1, false); } return -1; }
      return 1;
    }
    s_long res = insert(k, &e, &i1); if (res < 0) { return -1; }
    if (keep_first && res == 0) { return 0; }
    try { e->v = v; } catch (...) { if (res > 0) { _d.remove_e(e); _inds.el_remove_1<s_long>(i1, false); } return -1; }
    return res;
  }
};
}

unity& unity::map_ensure() { _ensure_m(); return *this; }
unity& unity::map_clear(bool full) { return u_clear(utMap, full ? 0x1 : 0); } // true resets all, false only removes elements
unity unity::map_keys(s_long lb)        { unity u; map_keys_get(u, lb); return u; }
unity& unity::map_keys_get(unity& dest, s_long lb)
{
  _ensure_m();
  unity z0;
  typedef vec2_t<unity, __vecm_tu_selector> Ttu;
  vec2_t<unity, __vecm_tu_selector>& z = (Ttu&)z0.rx<utUnityArray>();
  z.vec2_set_nbase(lb);
  t_map_tu& rm = *_m();
  for (s_long i = 0; i < rm.n(); ++i) { z.push_back(rm(i)->k); }
  dest._xu_move_p1(z0);
  return *this;
}
unity unity::map_values(s_long lb)        { unity u; map_values_get(u, lb); return u; }
unity& unity::map_values_get(unity& dest, s_long lb)
{
  _ensure_m();
  unity z0;
  typedef vec2_t<unity, __vecm_tu_selector> Ttu;
  vec2_t<unity, __vecm_tu_selector>& z = (Ttu&)z0.rx<utUnityArray>();
  z.vec2_set_nbase(lb);
  t_map_tu& rm = *_m();
  for (s_long i = 0; i < rm.n(); ++i) { z.push_back(rm(i)->v); }
  dest._xu_move_p1(z0);
  return *this;
}
s_long unity::mapFlags()        { _ensure_m(); return _m()->pkf()->flags(); }
void unity::mapFlags_set(s_long fk_reset, s_long fk_set)
{
  _ensure_m();
  s_long fk0 = _m()->pkf()->flags();
  s_long fk = ((fk0 & ~fk_reset) | fk_set) & _fkcmp_mask;
  if (fk == fk0) { return; }
  if (_m()->n() == 0) { _m()->pkf()->_set_flags(fk); return; }
  _mx& rm = *static_cast<_mx*>(_m());
  if ((fk0 ^ fk) == fkcmpRevOrder)
  {
    rm.__rev_inds();
    rm.pkf()->_set_flags(fk); return;
  }
  bool b_ro = !!((fk0 ^ fk) & fkcmpRevOrder);
  unity z0; z0.pmsm = pmsm; z0._ensure_m(); _mx& rm2 = *static_cast<_mx*>(z0._m()); rm2.pkf()->_set_flags(fk);
  for (s_long i = 0; i < rm.n(); ++i) { const t_map::entry* e = rm(i); if (rm2.__append(b_ro, e->k, e->v, true) < 0) { throw XUExec("mapFlags_set.1"); } }
  _xu_move_p1(z0);
}

bool unity::map_empty(const unity& k) const
{
  if (!(isMap() && _compat_chk())) { return true; }
  if (_m()->n() == 0) { return true; }
  if (k.isEmpty()) { t_map_tu& rm = *_m(); const t_map::entry* e = (rm.pkf()->flags() & fkcmpRevOrder) ? rm(rm.n() - 1) : rm(0); return e->k.isEmpty() ? e->v.isEmpty() : true; }
  const t_map::entry* e = _m()->find(k); return !e || e->v.isEmpty();
}
bool unity::map_locate(const unity& k, bool insert)
{
  _ensure_m();
  if (insert) { s_long res = _m()->insert(k); if (res >= 0) { return res > 0; } throw XUExec("map_locate.1"); }
  return !!_m()->find(k);
}
bool unity::map_locate(const unity& k, bool insert, s_long& ind)
{
  _ensure_m();
  if (k.isEmpty())
  {
    _mx& rm = *static_cast<_mx*>(_m());
    bool b_ro = !!(rm.pkf()->flags() & fkcmpRevOrder);
    if (rm.find(k)) { ind = b_ro ? rm.n() : 1; return !insert; }
    if (!insert) { ind = b_ro ? rm.n() + 1 : 1; return false; }
    s_long res = rm.__append(!b_ro, k, unity(), false);
    if (res == 1) { ind = b_ro ? rm.n() : 1; return true; }
    throw XUExec("map_locate.3");
  }
  s_long ind2; s_long res;
  if (insert) { res = _m()->insert(k, 0, &ind2); if (res >= 0) { ind = ind2 + 1; return res > 0; } throw XUExec("map_locate.2"); }
  res = _m()->find(k, &ind2) ? 1 : 0; ind = res ? ind2 + 1 : map_noel(); return !!res;
}

unity& unity::map(const unity& k)        { _ensure_m(); return (*_m())[k]; }
bool unity::map_append(const unity& k, const unity& v, bool keep_first)        { _ensure_m(); _mx& rm = *static_cast<_mx*>(_m()); s_long res = rm.__append(false, k, v, keep_first); if (res >= 0) { return res > 0; } throw XUExec("map_append.1"); }
bool unity::map_del(const unity& k)        { _ensure_m(); s_long res = _m()->remove(k); if (res >= 0) { return res > 0; } throw XUExec("map_del.1"); }

s_long unity::mapS()        { _ensure_m(); return _m()->n(); }
unity& unity::mapS_set(s_long new_S)
{
  _ensure_m(); t_map_tu& rm = *_m();
  if (new_S >= rm.n()) { return *this; }
  if (new_S <= 0) { if (rm.remove_all() >= 0) { return *this; } throw XUExec("mapS_set.1"); }
  if (new_S == rm.n() - 1) { if (rm.remove(rm(new_S)->k) >= 0) { return *this; } throw XUExec("mapS_set.2"); }
  unity z0; z0.pmsm = pmsm; z0._ensure_m(); _mx& rm2 = *static_cast<_mx*>(z0._m()); rm2.pkf()->_set_flags(rm.pkf()->flags());
  for (s_long i = 0; i < new_S; ++i) { const t_map::entry* e = rm(i); if (rm2.__append(false, e->k, e->v, true) < 0) { throw XUExec("mapS_set.3"); } }
  _xu_move_p1(z0);
  return *this;
}

bool unity::mapi_has(s_long ind) const        { return isMap() && _compat_chk() && ind >= 1 && ind <= _m()->n(); }
bool unity::mapi_empty(s_long ind) const        { if (!(isMap() && _compat_chk())) { return false; } const t_map::entry* e = (*_m())(ind - 1); return e && e->v.isEmpty(); }
const unity& unity::mapi_key(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { return e->k; } throw XUExec("mapi_key.1"); }
unity& unity::mapi(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { return e->v; } throw XUExec("mapi.1"); }
unity& unity::mapi_del(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { if (_m()->remove(e->k) > 0) { return *this; } throw XUExec("mapi_del.1"); } throw XUExec("mapi_del.2"); }






unity& _unity_hl_impl::operator[] (const unity& k) throw (exc_subscript)
{
  const t_hash::entry* e(0);
  s_long res = this->t_hash::insert(k, &e);
  if (res > 0) { if (!_a_inserted()) { this->t_hash::remove_e(e); res = -1; } }
  if (res >= 0) { return e->v; } throw exc_subscript();
}
  // Returns same as hashx remove_all (>= 0 - success, < 0 - failure, no changes.)
s_long _unity_hl_impl::hl_clear(bool full) throw()
{
  if (full) { s_long n_prev = _n; hashx_clear(); _a_cleared(); return n_prev; }
  s_long res = this->t_hash::remove_all(); if (res >= 0) { _a_cleared(); } return res;
}

bool _unity_hl_impl::_a_rsv_1() throw()  // reserve place in list and indk before possible adding 1 elem. to hash
  { return _list.el_reserve_n(n() + 2, false) && _indk.el_reserve_n(n() + 1, false); }

  // Adjustment notifications for list and keys index, on each hash change.

bool _unity_hl_impl::_a_inserted(s_long ind_before) throw() // adjust list and indk after adding 1 elem. to hash, true on success
{
  if (ind_before == -1) {}
    else { if (!(ind_before >= 0 && ind_before < n())) { return false; } }
  s_long i = n() - 1; if (i < 0 || i != _list.n() - 1) { return false; }
  if (!_list.el_append<meta::s_ll>(-1)) { return false; }
  __insert(i, _prev_of(ind_before));
  if (ind_before == -1 && i == _indk.n() && _indk.el_append<s_long>(i)) { return true; }
  if (_indk.n()) { _indk.vecm_clear(); }
  return true;
}
void _unity_hl_impl::_a_removed(s_long ind) throw() // adjust list and indk after removing 1 elem. from hash
{
  if (!(ind >= 0 && ind <= n() && _list.n() == n() + 2)) { return; }
  if (_indk.n())
  {
    if (_indk.n() - 1 == n() && *_indk.pval_last<s_long>() == n() && ind == n()) { _indk.el_expunge_last<s_long>(); }
      else { _indk.vecm_clear(); }
  }
  if (_list.n() != n() + 2) { return; }
  __eject(ind);
  if (ind == n()) { _list.el_expunge_last<meta::s_ll>(); return; }
  s_long ip = _prev_of(n()); __eject(n()); _list.el_expunge_last<meta::s_ll>(); __insert(ind, ip);
}
void _unity_hl_impl::_a_cleared() throw() // clear list and indk
  {  _list.el_expand_n(1); *_list.pval_first<meta::s_ll>() = _v(-1, -1);  _indk.vecm_clear(); }

bool _unity_hl_impl::_a_indk() const throw() // ensure indk containing full index of hash elems.
{
  if (_indk.n() == n()) { return true; }
  if (!_indk.el_expand_n(n())) { _indk.vecm_clear(); return false; }
  s_long pos = -1; s_long i = 0; while (i < n()) { pos = _next_of(pos); if (!(pos >= 0 && pos < n())) { break; } *_indk.pval_0u<s_long>(i) = pos; ++i; }
  if (i != n()) { _indk.vecm_clear(); return false; }
  return true;
}

void _unity_hl_impl::__eject(s_long ind) throw()
{
  s_long ipr = _prev_of(ind); s_long inx = _next_of(ind);
  if (ipr == inx) { _setv(ipr, _v(ipr, ipr)); }
    else if (_list.n() > 2) { meta::s_ll xp = _v(_prev_of(ipr), inx); meta::s_ll xn =  _v(ipr, _next_of(inx)); _setv(ipr, xp); _setv(inx, xn); }
}
void _unity_hl_impl::__insert(s_long ind, s_long after) throw()
{
  if (_list.n() > 2)
  {
    meta::s_ll xp = _v(_prev_of(after), ind); s_long inx = _next_of(after); meta::s_ll xn = _v(ind, _next_of(inx));
    _setv(ind, _v(after, inx)); _setv(after, xp); _setv(inx, xn);
  }
  else
  {
    if (ind == after) { return; }
    _setv(ind, _v(after, after)); _setv(after, _v(ind, ind));
  }
}


unity::kf_unity::kf_unity(s_long flags_) : _flags(char(flags_)) {} // flags: see static const s_long fkcmp*
s_long unity::kf_unity::flags() const { return _flags; } // see static const s_long fkcmp*
void unity::kf_unity::_set_flags(s_long x) const { _flags = x & unity::_fkcmp_mask; }
s_long unity::kf_unity::hash(const unity& x) const { return x.k_hashf(_flags); }
bool unity::kf_unity::is_eq(const unity& x1, const unity& x2) const { return x1.k_eq(x2, _flags); }
bool unity::kf_unity::less12(const unity& x1, const unity& x2) const { return x1.k_less(x2, _flags); }
bool unity::kf_unity::less21(const unity& x1, const unity& x2) const { return x1.k_less(x2, _flags); }

unity::_hl_i::_hl_i() { pmsm = unity_common::ls_modsm; }
unity::_hl_i::_hl_i(const _hl_i& x) : pmsm(0) { if (x.compatibility() < 1) { throw exc_consistency(); } pmsm = x.pmsm; }
s_long unity::_hl_i::n() const throw() { return static_cast<const _unity_hl_impl*>(this)->n(); }
unity& unity::_hl_i::operator[] (const unity& k) throw (exc_subscript) { return static_cast<_unity_hl_impl*>(this)->operator[](k); }
const unity::_hl_i::entry* unity::_hl_i::operator() (s_long ind) const throw() { return static_cast<const _unity_hl_impl*>(this)->operator()(ind); }
const unity::_hl_i::entry* unity::_hl_i::find(const unity& k, s_long* ret_pind) const throw() { return static_cast<const _unity_hl_impl*>(this)->find(k, ret_pind); }
s_long unity::_hl_i::insert(const unity& k, const entry** ret_pentry, s_long* ret_pind, s_long ind_before) throw() { return static_cast<_unity_hl_impl*>(this)->insert(k, ret_pentry, ret_pind, ind_before); }
s_long unity::_hl_i::remove(const unity& k) throw() { return static_cast<_unity_hl_impl*>(this)->remove(k); }
s_long unity::_hl_i::remove_i(s_long ind) throw() { return static_cast<_unity_hl_impl*>(this)->remove_i(ind); }
const unity::_hl_i::entry* unity::_hl_i::h(s_long ind_h) const throw() { return static_cast<const _unity_hl_impl*>(this)->h(ind_h); }
s_long unity::_hl_i::hl_clear(bool full) throw() { return static_cast<_unity_hl_impl*>(this)->hl_clear(full); }
const unity::_hl_i::t_hash::f_kf* unity::_hl_i::pkf() const throw() { return static_cast<const _unity_hl_impl*>(this)->pkf(); }
bool unity::_hl_i::hl_set_kf(const unity::_hl_i::t_hash::f_kf& kf) throw() { return static_cast<_unity_hl_impl*>(this)->hl_set_kf(kf); }

s_long unity::_hl_i::qi_noel() const throw() { return t_hash::no_elem; }
s_long unity::_hl_i::qi_next(s_long pos) const throw() { return static_cast<const _unity_hl_impl*>(this)->qi_next(pos); }
s_long unity::_hl_i::qi_prev(s_long pos) const throw() { return static_cast<const _unity_hl_impl*>(this)->qi_prev(pos); }
s_long unity::_hl_i::qi_indk(s_long ind) const throw() { return static_cast<const _unity_hl_impl*>(this)->qi_indk(ind); }

s_long unity::_hl_i::compatibility() const throw()
{
  if (pmsm == unity_common::ls_modsm) { return 2; }
  s_long ver = ((cv_ff::cv_ver::PFver)pmsm(unity_common::msm_ver_hl))();
  if ((ver & ~s_long(0xff)) == (cv_ff::cv_ver::Fver_hl() & ~s_long(0xff))) { return static_cast<const _unity_hl_impl*>(this)->compatibility(); }
  return -1;
}
void unity::_hl_i::assign_impl(const _hl_i& x) { static_cast<_unity_hl_impl*>(this)->assign(*static_cast<const _unity_hl_impl*>(&x)); }

_unity_hl_impl::_unity_hl_impl() : _hl_i(), t_hash(), _list(yk_c::typer<meta::s_ll, __vecm_tu_selector>, -1), _indk(yk_c::typer<s_long, __vecm_tu_selector>, 0)        { _list.el_append<meta::s_ll>(_v(-1, -1)); if (!_check()) { throw exc_consistency(); } }
_unity_hl_impl::_unity_hl_impl(const _unity_hl_impl& x) : _hl_i(x), t_hash(x), _list(x._list), _indk(x._indk)        { if (!_check()) { throw exc_consistency(); } }
void _unity_hl_impl::assign(const _unity_hl_impl& x)
{
  if (this == &x) { return; }
  hl_clear(true);
  if (x._hl_i::compatibility() < 1) { throw exc_consistency(); }
  s_long res = 100 * hashx_copy(x, true) + 10 * _list.vecm_copy(x._list, true) + _indk.vecm_copy(x._indk, true);
  if (res != 111 || !_check()) { hl_clear(true);  throw exc_consistency(); }
}
s_long _unity_hl_impl::n() const throw() { return _n; }
const _unity_hl_impl::entry* _unity_hl_impl::operator() (s_long ind) const throw() { return this->t_hash::operator()(qi_indk(ind)); }
const _unity_hl_impl::entry* _unity_hl_impl::find(const unity& k, s_long* ret_pind) const throw() { const entry* e; s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, &e, &ind); if (ret_pind) { *ret_pind = ind; } return e; }
s_long _unity_hl_impl::insert(const unity& k, const entry** ret_pentry, s_long* ret_pind, s_long ind_before) throw() { const t_hash::entry* e(0); s_long ind; s_long res = this->t_hash::insert(k, &e, &ind); if (res > 0) { if (!_a_inserted(ind_before)) { this->t_hash::remove_e(e); e = 0; ind = no_elem; res = -1; } } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; } return res; }
s_long _unity_hl_impl::remove(const unity& k) throw() { s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, 0, &ind); return remove_i(ind); }
s_long _unity_hl_impl::remove_i(s_long ind) throw() { s_long res = this->t_hash::remove_i(ind); if (res == 1) { _a_removed(ind); } return res; }
const _unity_hl_impl::entry* _unity_hl_impl::h(s_long ind_h) const throw() { return this->t_hash::operator()(ind_h); }
const _unity_hl_impl::f_kf* _unity_hl_impl::pkf() const throw() { return this->t_hash::pkf(); }
bool _unity_hl_impl::hl_set_kf(const f_kf& kf) throw() { return this->t_hash::hashx_set_kf(kf); }
s_long _unity_hl_impl::qi_noel() const throw() { return no_elem; } // starting pos for iterating the list in both directions
  // Next, prev. elem. ind. >= 0, or qi_noel() at list end
s_long _unity_hl_impl::qi_next(s_long pos) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _inext(*p) : s_long(no_elem); }
s_long _unity_hl_impl::qi_prev(s_long pos) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _iprev(*p) : s_long(no_elem); }
  // Index of ind-th element in hash, according to list, or qi_noel() for bad ind or indk.
s_long _unity_hl_impl::qi_indk(s_long ind) const throw() { if (!_a_indk()) { return no_elem; } s_long* p = _indk.pval<s_long>(ind); return p ? *p : s_long(no_elem); }
s_long _unity_hl_impl::compatibility() const throw() { const s_long ti = bytes::type_index_t<s_long>::ind(); const s_long ti2 = bytes::type_index_t<meta::s_ll>::ind(); s_long c = __crvx(_list)._t_ind == ti2  && __crvx(_indk)._t_ind == ti && __crvx(_ht)._t_ind == ti && sizeof(*this) == ((char*)&_indk - (char*)this) + sizeof(vecm) ? this->vecm::compatibility() : -1; if (c == 0) { c = -1; } return c; }
bool _unity_hl_impl::_check() const throw() { return nexc() == 0 && _list.n() == n() + 1 && (_indk.n() == 0 || _indk.n() == n()); }
const _unity_hl_impl::_vecm_x& _unity_hl_impl::__crvx(const vecm& v) const throw() { return *static_cast<const _vecm_x*>(&v); } // class method
meta::s_ll _unity_hl_impl::_v(s_long prev, s_long next) const throw() { return (meta::s_ll(prev) << 32) | (meta::s_ll(next) & 0xffffffffll); } // class method
s_long _unity_hl_impl::_iprev(meta::s_ll x) const throw() { return s_long(x >> 32); } // class method
s_long _unity_hl_impl::_inext(meta::s_ll x) const throw() { return s_long(x & 0xffffffffll); } // class method
meta::s_ll _unity_hl_impl::_v_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? *p : -1; }
s_long _unity_hl_impl::_prev_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _iprev(*p) : -1; }
s_long _unity_hl_impl::_next_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _inext(*p) : -1; }
void _unity_hl_impl::_setv(s_long ind, meta::s_ll x) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); if (p) { *p = x; } }

void unity::_ensure_m() { if (!isMap()) { u_clear(utMap); } if (!(_m()->pkf() && _compat_chk())) { throw XUExec("_ensure_m"); } }
void unity::_ensure_sc() { if (!isScalar()) { u_clear(utEmpty); } }
void unity::_ensure_h() { if (!isHash()) { u_clear(utHash); } if (!(_h()->pkf() && _compat_chk())) { throw XUExec("_ensure_h"); } }

const unity* unity::path(const std::wstring& keylist) const throw() { return const_cast<unity*>(this)->_path_u(keylist, false); }
const unity* unity::path(const wchar_t* keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(keylist, false); } catch (...) { return 0; } }
const unity* unity::path(const std::string& keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(bsToWs(keylist), false); } catch (...) { return 0; } }
const unity* unity::path(const char* keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(bsToWs(keylist), false); } catch (...) { return 0; } }
const unity* unity::path(const unity& keylist) const throw() { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.ref<utString>(), false); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), false); } return px->_path_u(keylist.vstr(), false); } catch (...) { return 0; } }

const unity* unity::path(const std::wstring& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
const unity* unity::path(const wchar_t* keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
const unity* unity::path(const std::string& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
const unity* unity::path(const char* keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
const unity* unity::path(const unity& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }

unity* unity::_path_w(const std::wstring& keylist) throw() { return _path_u(keylist, true); }
unity* unity::_path_w(const wchar_t* keylist) throw() { try { return _path_u(keylist, true); } catch (...) { return 0; } }
unity* unity::_path_w(const std::string& keylist) throw() { try { return _path_u(bsToWs(keylist), true); } catch (...) { return 0; } }
unity* unity::_path_w(const char* keylist) throw() { try { return _path_u(bsToWs(keylist), true); } catch (...) { return 0; } }
unity* unity::_path_w(const unity& keylist) throw() { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.ref<utString>(), true); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), true); } return px->_path_u(keylist.vstr(), true); } catch (...) { return 0; } }


std::ostream& operator<<(std::ostream& s, const unity::iofmt& f) { unity::write_fmt(s, *f.px, f, false); return s; }
std::ostream& operator<<(std::ostream& s, const unity& x) { unity::write_fmt(s, x, x.format(), false); return s; }
std::wostream& operator<<(std::wostream& s, const unity::iofmt& f) { unity::write_fmt(s, *f.px, f, false); return s; }
std::wostream& operator<<(std::wostream& s, const unity& x) { unity::write_fmt(s, x, x.format(), false); return s; }

std::ostream& operator<<(std::ostream& s, const unity::_wr_cstring& x) { s << x.cstr(); return s; }
std::ostream& operator<<(std::ostream& s, const unity::_wr_wstring& x) { s << x.cstr(); return s; }
std::wostream& operator<<(std::wostream& s, const unity::_wr_cstring& x) { s << x.wstr(); return s; }
std::wostream& operator<<(std::wostream& s, const unity::_wr_wstring& x) { s << x.wstr(); return s; }


unity& unity::hash_ensure()        { _ensure_h(); return *this; }
unity& unity::hash_clear(bool full)        { return u_clear(utHash, full ? 0x1 : 0); }
unity unity::hash_keys(s_long lb)        { unity u; hash_keys_get(u, lb); return u; }
unity& unity::hash_keys_get(unity& dest, s_long lb)
{
  _ensure_h(); unity z0;
  typedef vec2_t<unity, __vecm_tu_selector> Ttu;
  vec2_t<unity, __vecm_tu_selector>& z = (Ttu&)z0.rx<utUnityArray>();
  z.vec2_set_nbase(lb);
  t_hash& rh = *_h();
  s_long pos = rh.qi_next(rh.qi_noel());
  while (pos != rh.qi_noel()) { z.push_back(rh.h(pos)->k); pos = rh.qi_next(pos); }
  dest._xu_move_p1(z0);
  return *this;
}
unity unity::hash_values(s_long lb)        { unity u; hash_values_get(u, lb); return u; }
unity& unity::hash_values_get(unity& dest, s_long lb)
{
  _ensure_h(); unity z0;
  typedef vec2_t<unity, __vecm_tu_selector> Ttu;
  vec2_t<unity, __vecm_tu_selector>& z = (Ttu&)z0.rx<utUnityArray>();
  z.vec2_set_nbase(lb);
  t_hash& rh = *_h();
  s_long pos = rh.qi_next(rh.qi_noel());
  while (pos != rh.qi_noel()) { z.push_back(rh.h(pos)->v); pos = rh.qi_next(pos); }
  dest._xu_move_p1(z0);
  return *this;
}
s_long unity::hashFlags()        { _ensure_h(); return _h()->pkf()->flags(); }
void unity::hashFlags_set(s_long fk_reset, s_long fk_set)
{
  _ensure_h();
  s_long fk0 = _h()->pkf()->flags();
  s_long fk = ((fk0 & ~fk_reset) | fk_set) & _fkcmp_mask;
  if (fk == fk0) { return; }
  if (((fk0 ^ fk) & ~fkcmpRevOrder) == 0 || _h()->n() == 0) { _h()->pkf()->_set_flags(fk); return; }
  unity z0; z0.pmsm = pmsm; z0._ensure_h(); t_hash& rh2 = *z0._h(); rh2.pkf()->_set_flags(fk);
  t_hash& rh = *_h(); s_long pos = rh.qi_next(rh.qi_noel());
  while (pos != rh.qi_noel()) { const t_hash::entry* e = rh.h(pos); rh2[e->k] = e->v; pos = rh.qi_next(pos); }
  _xu_move_p1(z0);
}

bool unity::hash_empty(const unity& k) const        { if (!(isHash() && _compat_chk())) { return true; } const t_hash::entry* e = _h()->find(k); return !e || e->v.isEmpty(); }
bool unity::hash_locate(const unity& k, bool insert)
{
  _ensure_h();
  if (insert) { s_long res = _h()->insert(k); if (res >= 0) { return res > 0; } throw XUExec("hash_locate.1"); }
  return !!_h()->find(k);
}
unity& unity::hash(const unity& k)        { _ensure_h(); return (*_h())[k]; }
bool unity::hash_set(const unity& k, const unity& v, bool keep_first, s_long pos_before)
{
  _ensure_h();
  const t_hash::entry* e(0);
  s_long res = _h()->insert(k, &e, 0, pos_before);
  if (res > 0) { e->v = v; return true; }
    else if (res == 0) { if (!keep_first) { e->v = v; } return false; }
  throw XUExec("hash_set.1");
}
bool unity::hash_del(const unity& k)
{
  _ensure_h();
  s_long res = _h()->remove(k);
  if (res >= 0) { return res > 0; }
  throw XUExec("hash_del.1");
}

s_long unity::hashS()        { _ensure_h(); return _h()->n(); }
s_long unity::hashS_c() const    { if (isHash() && _compat_chk()) { return _h()->n(); } throw XUExec("hashS_c.1"); }

bool unity::hashi_has(s_long ind) const        { return isHash() && _compat_chk() && ind >= 1 && ind <= _h()->n(); }
bool unity::hashi_empty(s_long ind) const        { if (!(isHash() && _compat_chk())) { return false; } const t_hash::entry* e = (*_h())(ind - 1); return e && e->v.isEmpty(); }
const unity& unity::hashi_key(s_long ind)        { _ensure_h(); const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->k; } throw XUExec("hashi_key.1"); }
const unity& unity::hashi_key_c(s_long ind) const    { if (isHash() && _compat_chk()) { const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->k; } } throw XUExec("hashi_key_c.1"); }
unity& unity::hashi(s_long ind)        { _ensure_h(); const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->v; } throw XUExec("hashi.1"); }
const unity& unity::hashi_c(s_long ind) const        { if (isHash() && _compat_chk()) { const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->v; } } throw XUExec("hashi_c.1"); }

s_long unity::hashl_first() { return hashl_next(hashl_noel()); }
s_long unity::hashl_last() { return hashl_prev(hashl_noel()); }
s_long unity::hashl_next(s_long pos)        { _ensure_h(); return _h()->qi_next(pos); }
s_long unity::hashl_prev(s_long pos)        { _ensure_h(); return _h()->qi_prev(pos); }
const unity& unity::hashl_key(s_long pos)        { _ensure_h(); const t_hash::entry* e = _h()->h(pos); if (e) { return e->k; } throw XUExec("hashl_key.1"); }
unity& unity::hashl(s_long pos)        { _ensure_h(); const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } throw XUExec("hashl.1"); }
s_long unity::hashl_pos_c(const unity& k) const
{
  if (isHash()) { if (_compat_chk()) { s_long pos = hashl_noel(); _h()->find(k, &pos); return pos; } throw XUExec("hashl_pos.1"); }
    else { throw XUExec("hashl_pos.2"); }
}

bool unity::u_has(const unity& ki, s_long tt) const
{
  if (!_compat_chk()) { return false; }
  if ((tt & 1) != 0 && utype() == utUnityArray)
  {
    meta::s_ll ind;
    try { ind = ki.vint(); } catch (...) { return false; }
    if (!(ind >= cv_ff::cv_array::Llb_u(this) && ind <= cv_ff::cv_array::Lub_u(this))) { return false; }
    return true;
  }
  if ((tt & 2) != 0 && utype() == utMap) { return !!_m()->find(ki); }
  if ((tt & 4) != 0 && utype() == utHash) { return !!_h()->find(ki); }
  if ((tt & 8) != 0 && utype() == utObject) { const unity* p = objPtr_c<unity>(); if (p) { return p->u_has(ki, tt); } }
  return false;
}

const unity& unity::operator[] (const unity& ki) const { return const_cast<unity*>(this)->operator [](ki); }
unity& unity::operator[] (const unity& ki)
{
  if (!_compat_chk()) { throw XUExec("operator[].6"); }
  if (utype() == utUnityArray)
  {
    meta::s_ll ind;
    try { ind = ki.vint(); } catch (...) { throw XUExec("operator[].1", ki.vflstr()); }
    typedef valtype_t<utUnityArray>::t V;
    typedef valtype_t<utUnityArray>::t_tu Vtu;
    Vtu* pvec = (Vtu*)reference_t<V>::deref(_data, true);
    try { return *pvec->pc(s_long(ind));  } catch (...) {}
    throw XUExec("operator[].2", ki.vflstr());
  }
  else if (utype() == utMap) { const t_map::entry* e = _m()->find(ki); if (e) { return e->v; } throw XUExec("operator[].3", ki.vflstr()); }
  else if (utype() == utHash) { const t_hash::entry* e = _h()->find(ki); if (e) { return e->v; } throw XUExec("operator[].4", ki.vflstr()); }
  else if (utype() == utObject) { unity* p = objPtr<unity>(); if (p) { return (*p)[ki]; } }
  throw XUExec("operator[].5", ki.vflstr());
}


bool unity::assoc_set(const unity& k, const unity& v, bool keep_first)
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e(0); s_long res = _h()->insert(k, &e); if (res > 0) { e->v = v; return true; } else if (res == 0) { if (!keep_first) { e->v = v; } return false; } } throw XUExec("assoc_set.2"); }
    else if (isMap()) { if (_compat_chk()) { _mx& rm = *static_cast<_mx*>(_m()); s_long res = rm.__append(false, k, v, keep_first); if (res >= 0) { return res > 0; } } throw XUExec("assoc_set.1"); }
    else { throw XUExec("assoc_set.3"); }
}
bool unity::assoc_del(const unity& k)
{
  if (isHash()) { if (_compat_chk()) { s_long res = _h()->remove(k); if (res >= 0) { return res > 0; } } throw XUExec("assoc_del.2"); }
    else if (isMap()) { if (_compat_chk()) { s_long res = _m()->remove(k); if (res >= 0) { return res > 0; } } throw XUExec("assoc_del.1"); }
    else { throw XUExec("assoc_del.3"); }
}
s_long unity::assocl_first() const { return assocl_next(assocl_noel()); }
s_long unity::assocl_last() const { return assocl_prev(assocl_noel()); }
s_long unity::assocl_next(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { return _h()->qi_next(pos); } }
    else if (isMap()) { if (_compat_chk()) { pos = pos > 0 ? pos + 1 : 1; if (pos > _m()->n()) { pos = hashx_common::no_elem; } return pos; } }
  return hashx_common::no_elem;
}
s_long unity::assocl_prev(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { return _h()->qi_prev(pos); } }
    else if (isMap()) { if (_compat_chk()) { pos = pos < 1 ? _m()->n() : pos - 1; if (pos <= 0) { pos = hashx_common::no_elem; } return pos; } }
  return hashx_common::no_elem;
}
const unity& unity::assocl_key(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->k; } } throw XUExec("assocl_key.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->k; } } throw XUExec("assocl_key.1"); }
    else { throw XUExec("assocl_key.3"); }
}
unity& unity::assocl(s_long pos)
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } } throw XUExec("assocl.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->v; } } throw XUExec("assocl.1"); }
    else { throw XUExec("assocl.3"); }
}
const unity& unity::assocl_c(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } } throw XUExec("assocl_c.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->v; } } throw XUExec("assocl_c.1"); }
    else { throw XUExec("assocl_c.3"); }
}
s_long unity::assocS_c() const
{
  if (isHash()) { if (_compat_chk()) { return _h()->n(); } throw XUExec("assocS_c.1"); }
    else if (isMap()) { if (_compat_chk()) { return _m()->n(); } throw XUExec("assocS_c.2"); }
    else { throw XUExec("assocS_c.3"); }
}




  // bsp true once appends a space character before outputting the value.
  // nsp > 0 appends nsp space characters when starting each new line.
void unity::write_fmt(std::ostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp)
{
  const int _nsptab = 2;
  if (bsp) { s << ' '; } bool bsp2(x.isNamed());
  if (bsp2) { s << '`'; write_fmt(s, x.u_name(), x.format(f.fmt), false, 0); s << '`'; }

  switch (x.utype())
  {
    case utEmpty: { s << "Empty"; return; }
    case utInt: { if (bsp2) { s << ' '; }  s << *x._drf_c_i<utInt>(); return; }
    case utFloat:
    {
      if (bsp2) { s << ' '; }
      std::string s3 = ftocs(*x._drf_c_i<utFloat>(), 15, 15);
      _t_sz i = s3.find_first_of("."); if (i != nposc) { s << s3; return; }
      i = s3.find_first_of("eE"); if (i == nposc) { i = s3.length(); } s3.insert(i, 1, '.'); s << s3;
      return;
    }
    case utDate: { if (bsp2) { s << ' '; } s << dtofls(*x._drf_c_i<utDate>()); return; }
    case utChar: { if (bsp2) { s << ' '; } s << '+' << s_long((unsigned char)(*x._drf_c_i<utChar>())); return; }
    case utString: { cref_t<std::wstring> cr = x.cref<utString>(); if (bsp2) { s << ' '; } else { s << 's' << cr.ref().length() << ' '; }  s << wsToBs(cr.ref()); return; }
    case utObject:
    {
      const void* p0 = x.cpvoidkey(); s_long ind = f._rho().n() + 1; bool bnew = false; const iofmt::t_h::entry* e = f._rho().find(p0); if (e) { ind = e->v; } else { bnew = true; f._rho()[p0] = ind; }
      meta::s_ll adr = (char*)p0 - (char*)0;
      s << "obj#" << ind << " @" << adr;
      if (bnew) { const unity* p = x.objPtr_c<unity>(true); if (p) { s << ":\n"; for (s_long i = 0; i < nsp; ++i) { s << ' '; } write_fmt(s, *p, f, false, nsp + _nsptab); s << "\n"; } else { s << '.'; } }
        else { s << '>'; }
      return;
    }
    case utMap:
    {
      t_map_tu& rm = *x._m();
      s << "Map n=" << rm.n() << " f=" << rm.pkf()->flags() << (rm.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rm.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << ' '; }
        s << (i + 1);
        write_fmt(s, rm(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rm(i)->v, f, true, nsp + _nsptab);
        s << "\n";
      }
      return;
    }
    case utHash:
    {
      t_hash& rh = *x._h();
      s << "Hash n=" << rh.n() << " f=" << rh.pkf()->flags() << (rh.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rh.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << ' '; }
        s << (i + 1);
        write_fmt(s, rh(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rh(i)->v, f, true, nsp + _nsptab);
        s << "\n";
      }
      return;
    }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utStringArray:
    case utCharArray:
    case utUnityArray:
    {
      const char* al[7] = { "Int", "Float", "Date", "Char", "String", "?" , "Unity" };
      s_long n = cv_ff::cv_array::Lsz_u(&x); s_long nb = cv_ff::cv_array::Llb_u(&x); s_long nlast = nb + n - 1;
      s << al[x.utype() - utIntArray] << "Array n=" << n << " b=" << nb;
      switch (x.utype())
      {
        case utUnityArray: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); const unity& u = x.ref<utUnity>(i); write_fmt(s, u, f, true, nsp + _nsptab); if (u.isScalar() || u.isObject()) { s << "\n"; } } } else { s << "\n"; } break; }
        case utStringArray: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); write_fmt(s, x.crefx<utString>(i).ref(), f, true, nsp + _nsptab); s << "\n"; } } else { s << "\n"; } break; }
        default: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); write_fmt(s, x.val<utUnity>(i), x.format(f.fmt), true, nsp + _nsptab); s << "\n"; } } else { s << "\n"; } break; }
      }
      return;
    }
    default: { s << "Unknown t=" << x.utype(); return; }
  }
}

unity::iofmt::iofmt(Eiofmt fmt_, const unity& x) : fmt(fmt_), px(&x) {}
unity::iofmt::t_h& unity::iofmt::_rho() const { if (!_ho) { _ho.create0(false); } return *_ho._pnonc_u(); }
unity::iofmt unity::format(Eiofmt fmt) const { return iofmt(fmt, *this); }

void unity::write_fmt(std::wostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp)
{
  const int _nsptab = 2;
  if (bsp) { s << L' '; } bool bsp2(x.isNamed());
  if (bsp2) { s << L'`'; write_fmt(s, x.u_name(), x.format(f.fmt), false, 0); s << L'`'; }

  switch (x.utype())
  {
    case utEmpty: { s << L"Empty"; return; }
    case utInt: { if (bsp2) { s << L' '; }  s << *x._drf_c_i<utInt>(); return; }
    case utFloat:
    {
      if (bsp2) { s << L' '; }
      std::wstring s3 = ftows(*x._drf_c_i<utFloat>(), 15, 15);
      _t_wz i = s3.find_first_of(L"."); if (i != nposc) { s << s3; return; }
      i = s3.find_first_of(L"eE"); if (i == nposc) { i = s3.length(); } s3.insert(i, 1, '.'); s << s3;
      return;
    }
    case utDate: { if (bsp2) { s << L' '; } s << dtofls(*x._drf_c_i<utDate>()).wstr(); return; }
    case utChar: { if (bsp2) { s << L' '; } s << L'+' << s_long((unsigned char)(*x._drf_c_i<utChar>())); return; }
    case utString: { cref_t<std::wstring> cr = x.cref<utString>(); if (bsp2) { s << L' '; } else { s << L's' << cr.ref().length() << L' '; }  s << cr.ref(); return; }
    case utObject:
    {
      const void* p0 = x.cpvoidkey(); s_long ind = f._rho().n() + 1; bool bnew = false; const iofmt::t_h::entry* e = f._rho().find(p0); if (e) { ind = e->v; } else { bnew = true; f._rho()[p0] = ind; }
      meta::s_ll adr = (char*)p0 - (char*)0;
      s << L"obj#" << ind << L" @" << adr;
      if (bnew) { const unity* p = x.objPtr_c<unity>(true); if (p) { s << L":\n"; for (s_long i = 0; i < nsp; ++i) { s << L' '; } write_fmt(s, *p, f, false, nsp + _nsptab); s << L"\n"; } else { s << L'.'; } }
        else { s << L'>'; }
      return;
    }
    case utMap:
    {
      t_map_tu& rm = *x._m();
      s << L"Map n=" << rm.n() << L" f=" << (rm.pkf() ? rm.pkf()->flags() : 0) << (rm.n() ? L":\n" : L"\n");
      for (s_long i = 0; i < rm.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << L' '; }
        s << (i + 1);
        write_fmt(s, rm(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rm(i)->v, f, true, nsp + _nsptab);
        s << L"\n";
      }
      return;
    }
    case utHash:
    {
      t_hash& rh = *x._h();
      s << L"Hash n=" << rh.n() << L" f=" << (rh.pkf() ? rh.pkf()->flags() : 0) << (rh.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rh.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << L' '; }
        s << (i + 1);
        write_fmt(s, rh(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rh(i)->v, f, true, nsp + _nsptab);
        s << L"\n";
      }
      return;
    }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utStringArray:
    case utCharArray:
    case utUnityArray:
    {
      const char* al[7] = { "Int", "Float", "Date", "Char", "String", "?" , "Unity" };
      s_long n = cv_ff::cv_array::Lsz_u(&x); s_long nb = cv_ff::cv_array::Llb_u(&x); s_long nlast = nb + n - 1;
      s << al[x.utype() - utIntArray] << L"Array n=" << n << L" b=" << nb;
      switch (x.utype())
      {
        case utUnityArray: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); const unity& u = x.ref<utUnity>(i); write_fmt(s, u, f, true, nsp + _nsptab); if (u.isScalar() || u.isObject()) { s << L"\n"; } } } else { s << L"\n"; } break; }
        case utStringArray: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); write_fmt(s, x.crefx<utString>(i).ref(), f, true, nsp + _nsptab); s << L"\n"; } } else { s << L"\n"; } break; }
        default: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); write_fmt(s, x.val<utUnity>(i), x.format(f.fmt), true, nsp + _nsptab); s << L"\n"; } } else { s << L"\n"; } break; }
      }
      return;
    }
    default: { s << L"Unknown t=" << x.utype(); return; }
  }
}










////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem); // platform-dependent
std::string _wsToBsUtf8(const wchar_t* ps, meta::s_ll n);
std::string _wsToBsLsb(const wchar_t* ps, meta::s_ll n);

std::string wsToBs(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, false); }
std::string wsToBs(const std::wstring& x) { return _wsToBs(x.c_str(), x.length(), false); }
std::string wsToBsOem(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, true); }
std::string wsToBsOem(const std::wstring& x) { return _wsToBs(x.c_str(), x.length(), true); }
std::string wsToBsUtf8(const wchar_t* x, meta::s_ll n) { return _wsToBsUtf8(x, n); }
std::string wsToBsUtf8(const std::wstring& x) { return _wsToBsUtf8(x.c_str(), x.length()); }
std::string wsToBsLsb(const wchar_t* x, meta::s_ll n) { return _wsToBsLsb(x, n); }
std::string wsToBsLsb(const std::wstring& x) { return _wsToBsLsb(x.c_str(), x.length()); }

std::wstring _bsToWs(const char* ps, meta::s_ll n); // platform-dependent
std::wstring _bsUtf8ToWs(const char* ps, meta::s_ll n);
std::wstring _bsLsbToWs(const char* ps, meta::s_ll n);

std::wstring bsToWs(const char* x, meta::s_ll n){ return _bsToWs(x, n); }
std::wstring bsToWs(const std::string& x){ return _bsToWs(x.c_str(), x.length()); }
std::wstring bsUtf8ToWs(const char* x, meta::s_ll n) { return _bsUtf8ToWs(x, n); }
std::wstring bsUtf8ToWs(const std::string& x) { return _bsUtf8ToWs(x.c_str(), x.length()); }
std::wstring bsLsbToWs(const char* x, meta::s_ll n) { return _bsLsbToWs(x, n); }
std::wstring bsLsbToWs(const std::string& x) { return _bsLsbToWs(x.c_str(), x.length()); }


std::string _wsToBsUtf8(const wchar_t* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_wsToBsUtf8.3"); }
    if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
    std::string s; if (n <= 0) { return s; }
    meta::s_ll nrsv = n; s.reserve(_t_sz(nrsv));
    for (meta::s_ll i = 0; i < n; ++i)
    {
      s_long q = s_long(ps[i]) & 0xffff;
      if (q >= 0xd800 && q < 0xe000)
      {
        if (q >= 0xdc00 || i + 1 >= n) { q = L'?'; }
        else
        {
          s_long q2 = s_long(ps[i + 1]) & 0xffff;
          if (q2 >= 0xdc00 && q2 < 0xe000) { q = (((q - 0xd800) << 10) | (q2 - 0xdc00)) + 0x10000; ++i; }
            else { q = L'?'; }
        }
      }
      if (q <= 0x7f) { s += char(q); }
        else if (q <= 0x7ff) { s += char((q >> 6) | 0xc0); s += char((q & 0x3f) | 0x80); }
        else if (q <= 0xffff) { s += char((q >> 12) | 0xe0); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
        else { s += char((q >> 18) | 0xf0); s += char(((q >> 12) & 0x3f) | 0x80); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
      if ((i & 0xfff) == 0) { meta::s_ll n2 = meta::s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(_t_sz(nrsv)); } }
    }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_wsToBsUtf8.1", e.what()); }
  catch (...) { throw XUExec("_wsToBsUtf8.2"); }
}
std::string _wsToBsLsb(const wchar_t* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_wsToBsLsb.3"); }
    if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
    std::string s;
    if (n > max_scstr()) { n = max_scstr(); }
    s.resize(_t_sz(n));
    for (meta::s_ll i = 0; i < n; ++i) { s[_t_sz(i)] = char(ps[i]); }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_wsToBsLsb.1", e.what()); }
  catch (...) { throw XUExec("_wsToBsLsb.2"); }
}

std::wstring _bsUtf8ToWs(const char* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_bsUtf8ToWs.3"); }
    if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
    std::wstring s; if (n <= 0) { return s; }
    meta::s_ll nrsv = n / 2; s.reserve(_t_sz(nrsv));
    meta::s_ll i = 0, rsvi = 0;
    while (i < n)
    {
      unsigned char c = ps[i];
      if ((c & 0x80) == 0) { s += wchar_t(c); ++i; }
      else
      {
        s_long nbchr = 0;
        while (((c << nbchr) & 0xc0) == 0xc0) { ++nbchr; }
        if (nbchr >= 1 && nbchr <= 5 && i + nbchr < n)
        {
          s_long q(c & (0x3f >> nbchr)); ++i;
          do { c = ps[i]; if ((c & 0xc0) != 0x80) { q = L'?'; break; } q = (q << 6) | (c & 0x3f); ++i; } while (--nbchr);
          if (q > 0x10ffff) { s += L'?'; }
            else if (q >= 0x10000) { q -= 0x10000; s += wchar_t(0xd800 | (q >> 10)); s += wchar_t(0xdc00 | (q & 0x3ff)); }
            else { s += wchar_t(q); }
        }
        else { s += L'?'; ++i; }
      }
      if (i - rsvi > 0xfff) { rsvi = i; meta::s_ll n2 = meta::s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(_t_sz(nrsv)); } }
    }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_bsUtf8ToWs.1", e.what()); }
  catch (...) { throw XUExec("_bsUtf8ToWs.2"); }
}
std::wstring _bsLsbToWs(const char* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_bsLsbToWs.3"); }
    if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
    std::wstring s;
    if (n > max_swstr()) { n = max_swstr(); }
    s.resize(_t_wz(n));
    for (meta::s_ll i = 0; i < n; ++i) { s[_t_wz(i)] = wchar_t(unsigned(ps[i]) & 0xff); }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_bsLsbToWs.1", e.what()); }
  catch (...) { throw XUExec("_bsLsbToWs.2"); }
}


std::string replace(const std::string& s, const std::string& from, const std::string& to, bool ignoreCase, s_ll nmax)
{
  if (from.length() == 0 || nmax == 0) { return s; }
  else if (s.length() > 0)
  {
    s_ll n = 0; if (nmax < 0) { nmax = s.length(); }
    std::string dest;
    _t_sz pos, pos2;
    pos=0;
    if (ignoreCase)
    {
      std::string s_l, from_l;
      if (1)
      {
        setlocale_locked __lock(LC_CTYPE, ""); if (sizeof(__lock)) {}
        s_l = lcase(s);
        from_l = lcase(from);
      }
      do
      {
        if (n >= nmax) { break; }
        pos2 = s_l.find(from_l, pos);
        if (pos2 == nposc) { pos2 = s.length(); dest += s.substr(pos); }
          else { dest += s.substr(pos, pos2 - pos); dest += to; ++n; pos2 += from.length(); }
        pos = pos2;
      }
      while(pos < s.length());
      if (pos < s.length()) { dest += s.substr(pos); }
    }
    else
    {
      do
      {
        if (n >= nmax) { break; }
        pos2 = s.find(from, pos);
        if (pos2 == nposc) { pos2 = s.length(); dest += s.substr(pos); }
          else { dest += s.substr(pos, pos2 - pos); dest += to; ++n; pos2 += from.length(); }
        pos = pos2;
      }
      while(pos < s.length());
      if (pos < s.length()) { dest += s.substr(pos); }
    }
    return dest;
  }
  else return s;
}

std::wstring replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, bool ignoreCase, s_ll nmax)
{
  if (from.length() == 0 || nmax == 0) { return s; }
  else if (s.length() > 0)
  {
    s_ll n = 0; if (nmax < 0) { nmax = s.length(); }
    std::wstring dest;
    _t_wz pos, pos2;
    pos=0;
    if (ignoreCase)
    {
      std::wstring s_l, from_l;
      if (1)
      {
        setlocale_locked __lock(LC_CTYPE, ""); if (sizeof(__lock)) {}
        s_l = lcase(s);
        from_l = lcase(from);
      }
      do
      {
        if (n >= nmax) { break; }
        pos2 = s_l.find(from_l, pos);
        if (pos2 == nposw) { pos2 = s.length(); dest += s.substr(pos); }
          else { dest += s.substr(pos, pos2 - pos); dest += to; ++n; pos2 += from.length(); }
        pos = pos2;
      }
      while(pos < s.length());
      if (pos < s.length()) { dest += s.substr(pos); }
    }
    else
    {
      do
      {
        if (n >= nmax) { break; }
        pos2 = s.find(from, pos);
        if (pos2 == nposw) { pos2 = s.length(); dest += s.substr(pos); }
          else { dest += s.substr(pos, pos2 - pos); dest += to; ++n; pos2 += from.length(); }
        pos = pos2;
      }
      while(pos < s.length());
      if (pos < s.length()) { dest += s.substr(pos); }
    }
    return dest;
  }
  else return s;
}


bool wstring_like(const std::wstring& str, const std::wstring& pattern)
{
  std::vector<std::wstring> ptnParts;
    //- ptnParts[i] having appropriate isPtnRange[i] == true is a wildcard (ptnParts[i].size() == 1 i.e. single character)
    //  or one or more ranges of characters (ptnParts[i].size() is a multiple of 2, each odd-even characters specify
    //  one range of characters, inclusive).
  std::vector<bool> isPtnRange;
  std::wstring sFixed;
  bool isUnclosedBracket = false;
  _t_wz ptnPos = 0;
  while (ptnPos < pattern.length())
  {
    switch (pattern[ptnPos])
    {
    case L'*':
      if (sFixed.size() > 0)
      {
        ptnParts.push_back(sFixed);
        isPtnRange.push_back(false);
        sFixed.clear();
      }
      if (ptnParts.size() == 0 || ptnParts.back() != L"*")
      {
        ptnParts.push_back(L"*");
        isPtnRange.push_back(true);
        ++ptnPos;
      }
      break;
    case L'?':
    case L'#':
      if (sFixed.size() > 0)
      {
        ptnParts.push_back(sFixed);
        isPtnRange.push_back(false);
        sFixed.clear();
      }
      ptnParts.push_back(pattern.substr(ptnPos, 1)); // either ? or #
      isPtnRange.push_back(true);
      ++ptnPos;
      break;
    case L'[':
      if (isUnclosedBracket)
      {
        sFixed += pattern[ptnPos]; // added '[' a fixed character
        ++ptnPos;
      }
      else
      {
        _t_wz pos2 = pattern.find(L']', ptnPos+1);
        if (pos2 == nposw) // means: there is the first unclosed bracket found at pos
        {
          isUnclosedBracket = true;
          sFixed += pattern[ptnPos]; // added '[' a fixed character
          ++ptnPos;
        }
        else
        {
          // extract character ranges specified between closed brackets
          if (pos2 - ptnPos > 1)
          {
            std::wstring sRanges;
            _t_wz pos3 = ptnPos + 1;

            if (pattern[pos3] == L'!')
            {
              if (pos2 > pos3 + 1) { sRanges += L'!'; } else { sRanges += L" !!"; }
              ++pos3;
            }
            else
            {
              sRanges += L' ';
            }
            while (pos3 < pos2)
            {
              if (pos3 + 2 < pos2 && pattern[pos3+1] == L'-')
              {
                // a range of characters specified by 3-character sequence like "a-z"
                sRanges += pattern[pos3];
                sRanges += pattern[pos3+2];
                pos3 += 3;
              }
              else
              {
                // range containing only one character
                sRanges += pattern[pos3];
                sRanges += pattern[pos3];
                ++pos3;
              }
            }

            if (sRanges.size() == 3 && sRanges[0] == L' ' && sRanges[1] == sRanges[2])
            {
              sFixed += sRanges[1];
            }
            else
            {
              if (sFixed.size() > 0)
              {
                ptnParts.push_back(sFixed);
                isPtnRange.push_back(false);
                sFixed.clear();
              }

              ptnParts.push_back(sRanges);
              isPtnRange.push_back(true);
            }

            ptnPos = pos2 + 1; // go to position next after the closing bracket
          }
          else
          {
            ptnPos = pos2 + 1; // skip empty brackets
          }
        }
      }
      break;
    default:
      sFixed += pattern[ptnPos]; // a fixed character
      ++ptnPos;
      break;
    }
  }
  if (sFixed.size() > 0) // care for tail of pattern possibly left in sFixed
  {
    ptnParts.push_back(sFixed);
    isPtnRange.push_back(false);
    sFixed.clear();
  }

  std::vector<_t_wz> strPosStack; // string positions tried to match against *
  std::vector<_t_wz> indPtnStack; // the index of *-s that are matched

  _t_wz strPos = 0;
  _t_wz indPtnPart = 0;

  while (true)
  {
    if (strPos >= str.size()) // case: str is gone, i.e. nothing is left to compare, should return already
    {
      if (indPtnPart >= ptnParts.size())
      {
        // cases:
        // a) both str and pattern are empty, i.e. they match
        // b) both str and pattern are gone at the same time, i.e. they match
        goto lProcessMatch;
      }
      else
      {
        if (indPtnPart == ptnParts.size() - 1 && isPtnRange[indPtnPart] && ptnParts[indPtnPart] == L"*")
        {
          // case: in pattern, only * is left, here this matches with the end of string, i.e. with ""
          goto lProcessMatch;
        }
      }
      goto lProcessMismatch;
    }
    else // case: comparing the current pos in the string with the current pattern
    {
      if (indPtnPart >= ptnParts.size())
      {
        // case: pattern has gone, but string had not gone yet, i.e. mismatch
        goto lProcessMismatch;
      }
      if (isPtnRange[indPtnPart])
      {
        if (ptnParts[indPtnPart].size() == 1) // a wildcard character should be processed
        {
          if (ptnParts[indPtnPart] == L"?")
          {
            // matched successfully (any character matches),
            // move to next position in the string and process the next pattern
            strPos += 1;
            indPtnPart += 1;
          }
          else if (ptnParts[indPtnPart] == L"#")
          {
            if (isdigit(str[strPos]))
            {
              // matched a digit,
              // move to next position in the string and process the next pattern
              strPos += 1;
              indPtnPart += 1;
            }
            else // don't match
            {
              goto lProcessMismatch;
            }
          }
          else if (ptnParts[indPtnPart] == L"*")
          {
            goto lProcessAsterisk;
          }
          else // bad branch, assuming don't match
          {
            goto lProcessMismatch;
          }
        }
        else // the current str character should be matched against one or more character ranges
        {
          bool isInRangeTest = ptnParts[indPtnPart][0] != L'!';
          bool isMatch = false;
          for (unsigned int i = 1; i < ptnParts[indPtnPart].size(); i += 2)
          {
            if (str[strPos] >= ptnParts[indPtnPart][i] && str[strPos] <= ptnParts[indPtnPart][i + 1])
            {
              // found a range that matches to the current str character
              isMatch = true;
              break;
            }
          }
          if (isMatch == isInRangeTest)
          {
            // matched successfully,
            // move to next position in the string and process the next pattern
            strPos += 1;
            indPtnPart += 1;
          }
          else // don't match
          {
            goto lProcessMismatch;
          }
        }
      }
      else // the str characters at the current position should be matched against some fixed string
      {
        if (str.substr(strPos, ptnParts[indPtnPart].size()) == ptnParts[indPtnPart])
        {
          // matched successfully,
          // move to next position in the string and process the next pattern
          strPos += ptnParts[indPtnPart].size();
          indPtnPart += 1;
        }
        else // don't match
        {
          goto lProcessMismatch;
        }
      }
    }
    continue;
lProcessAsterisk: // position at some * wildcard somewhere in str
    // Needs to calculate next guess for strPos, if any exists at all, store it into the stack,
    // and then match the current strPos against the rest of patterns list.
    // cases:
    // a) * is the last pattern, i.e. total match
    // b) * is followed by a fixed string
    // c) * is followed by a range (the worst case, i.e. need processing str char by char)
    if (indPtnPart == ptnParts.size() - 1) // a)
    {
      goto lProcessMatch;
    }
    else if (!isPtnRange[indPtnPart+1]) // b)
    {
      // next guess
      _t_wz pos4 = str.find(ptnParts[indPtnPart+1], strPos);
      if (pos4 == nposw)
      {
        // Failed to find a fixed part of the pattern.
        //  Our matching is not greedy, and processes shorter guesses first,
        //  so here the whole pattern appears longer than str.
        //  This means final mismatch.
        return false;
      }
      else // the fixed part of the pattern matches with the current str position or greater position
      {
        // prepare possibly one more guess, and store it to stack, then process the current match
        _t_wz pos5 = pos4 + 1;
        if (pos5 < str.size())
        {
          strPosStack.push_back(pos5);
          indPtnStack.push_back(indPtnPart);
        }

        // continue matching in the current guess, having skipped the current fixed part of the pattern
        strPos += pos4 + ptnParts[indPtnPart+1].size();
        indPtnPart += 2;
      }
    }
    else // c)
    {
      // next guess
      _t_wz pos4 = strPos + 1;

      // store next guess
      strPosStack.push_back(pos4);
      indPtnStack.push_back(indPtnPart);

      indPtnPart += 1; // continue matching in the current guess, from the next pattern, which is guaranteed to be a range, anything except *
    }
    continue;
lProcessMismatch:
    if (strPosStack.size() == 0)
    {
      return false;
    }
    strPos = strPosStack.back();
      strPosStack.pop_back();
    indPtnPart = indPtnStack.back();
      indPtnStack.pop_back();
    goto lProcessAsterisk;
lProcessMatch:
    return true;
  }
}

unity split(const std::wstring& s, const std::wstring& delim, meta::s_ll nmax)
{
  unity x; x.arr_init<utString>(0); if (nmax == 0) { return x; }
  if (delim.length()==0) { x.arr_append(s); }
  else if (s.length() > 0)
  {
    _t_wz pos, pos2; pos = 0;
    do
    {
      pos2 = s.find(delim,pos);
      if (pos2 == nposw  || (nmax > 0 && x.arrsz() + 1 >= nmax)) { pos2 = s.length(); x.arr_append(s.substr(pos, pos2 - pos)); }
        else { x.arr_append(s.substr(pos, pos2 - pos)); pos2 += delim.length(); if (pos2 >= s.length()) x.arr_append(L""); }
      pos = pos2;
    } while (pos < s.length());
  }
  return x;
}

unity split(const std::string& s, const std::string& delim, meta::s_ll nmax)
{
  unity x;  x.arr_init<utString>(0); if (nmax == 0) { return x; }
  if (delim.length()==0) { x.arr_append(s); }
  else if (s.length() > 0)
  {
    _t_wz pos, pos2;  pos = 0;
    do
    {
      pos2 = s.find(delim,pos);
      if (pos2 == nposc || (nmax > 0 && x.arrsz() + 1 >= nmax)) { pos2 = s.length(); x.arr_append(s.substr(pos, pos2 - pos)); }
        else { x.arr_append(s.substr(pos, pos2 - pos)); pos2 += delim.length(); if (pos2 >= s.length()) x.arr_append(""); }
      pos = pos2;
    } while (pos < s.length());
  }
  return x;
}

std::vector<std::string> splitToVector(const std::string& s, const std::string& delim, meta::s_ll nmax)
{
  std::vector<std::string> x; if (nmax == 0) { return x; }
  if (delim.length() == 0) { x.push_back(s); }
  else if (s.length() > 0)
  {
    _t_sz pos, pos2; pos = 0;
    do
    {
      pos2 = s.find(delim,pos);
      if (pos2 == nposc || (nmax > 0 && x.size() + 1 >= std::size_t(nmax))) { pos2 = s.length(); x.push_back(s.substr(pos, pos2 - pos)); }
        else { x.push_back(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2 >= s.length()) { x.push_back(""); } }
      pos = pos2;
    } while(pos < s.length());
  }
  return x;
}

std::vector<std::wstring> splitToVector(const std::wstring& s, const std::wstring& delim, meta::s_ll nmax)
{
  std::vector<std::wstring> x; if (nmax == 0) { return x; }
  if (delim.length() == 0) { x.push_back(s); }
  else if (s.length() > 0)
  {
    _t_wz pos, pos2; pos = 0;
    do
    {
      pos2 = s.find(delim, pos);
      if (pos2 == nposw || (nmax > 0 && x.size() + 1 >= std::size_t(nmax))) { pos2 = s.length(); x.push_back(s.substr(pos, pos2 - pos)); }
        else { x.push_back(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2 >= s.length()) { x.push_back(L""); } }
      pos = pos2;
    } while(pos < s.length());
  }
  return x;
}

std::string join(const unity& asrc, const std::string& delim)
{
    std::string s;
    if (asrc.isArray())
    {
        for(s_long ind=asrc.arrlb(); ind<=asrc.arrub(); ++ind)
        {
            if (ind==asrc.arrlb()) s=wsToBs(asrc.vstr(ind));
            else (s+=delim)+=wsToBs(asrc.vstr(ind));
        }
    }
    else s=wsToBs(asrc.vstr());
    return s;
}
std::wstring join(const unity& asrc, const std::wstring& delim)
{
    std::wstring s;
    if (asrc.isArray())
    {
        for(s_long ind=asrc.arrlb(); ind<=asrc.arrub(); ++ind)
        {
            if (ind==asrc.arrlb()) s=asrc.vstr(ind);
            else (s+=delim)+=asrc.vstr(ind);
        }
    }
    else s=asrc.vstr();
    return s;
}



std::wstring CStr3(const unity& x)
{
    if (x.isObject()) { return L"?object " + x.tname(); }
    if (x.isArray()) { return L"?array " + x.tname(true); }

    switch (x.utype())
    {
      case utEmpty: return std::wstring();
      case utInt: return x.vstr();
      case utFloat: return x.vstr();
      case utChar: { return x.ref<utChar>() ? L"True" : L"False"; }
      case utString: { return x.vstr(); }
      case utDate:
        {
          // Note: this loses fractional part of seconds,
          //  unlike conv_Date_String.
          _unitydate d = x.val<utDate>();
          std::ostringstream oss;
          oss << std::setw(4) << std::setfill('0') << std::right << d.d_year() << "-" << std::setw(2) <<d.d_month() << "-" << std::setw(2) <<d.d_day();
          if (d.f() != std::floor(d.f())) { oss << " " << std::setw(2) << d.d_hour() << ":" << std::setw(2) << d.d_minute() << ":" << std::setw(2) << d.d_second(); }
          return bsToWs(oss.str());
        }
      default: { return L"?value/" + x.tname(); }
    }
}

namespace
{
  const double _jds_eps = 5.e-5;
  meta::s_ll _n_sec(double jds) { return meta::s_ll(jds + (43200 + _jds_eps)); }
  s_long _n(double jds) { return s_long(_n_sec(jds) / 86400); }

  s_long _is_g(s_long n) { return n >= 2299161; }
  s_long _is_g(s_long y, s_long m, s_long d)
  {
    if (y > 1582) { return true; }
    if (y == 1582) { return m > 10 || (m == 10 && d >= 15); }
    return false;
  }
  meta::s_ll _n_any(s_long y_, s_long m_, s_long d_) // retval may be  > 32 bit
  {
    meta::s_ll year(y_), month(m_), day(d_);
    if (_is_g(y_, m_, d_)) { meta::s_ll a((14 - month) / 12), y(year + 4800 - a), m(month + 12 * a - 3); return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045; }
      else { meta::s_ll a((14 - month) / 12), y(year + 4800 - a + (year < 0 ? 1 : 0)), m(month + 12 * a - 3); return day + (153 * m + 2) / 5 + 365 * y + y / 4 - 32083; }
  }
  meta::s_ll _jds_nhms(s_long n, s_long h, s_long mt, s_long s) { return meta::s_ll(n) * 86400 + (h - 12) * 3600 + mt * 60 + s; }

  void _ymd(s_long n, s_long* py, s_long* pm, s_long* pd)
  {
    if (_is_g(n))
    {
      meta::s_ll a(n + 32044), b((4 * a + 3) / 146097), c(a - (146097 * b) / 4), d((4 * c + 3) / 1461), e(c - (1461 * d) / 4), m((5 * e + 2) / 153);
      if (py) { *py = s_long(100 * b + d - 4800 + m / 10); } if (pm) { *pm = s_long(m + 3 - 12 * (m / 10)); } if (pd) { *pd = s_long(e - (153 * m + 2) / 5 + 1); }
    }
    else
    {
      meta::s_ll c(n + 32082), d((4 * c + 3) / 1461), e(c - (1461 * d) / 4), m((5 * e + 2) / 153);
      if (py) { s_long y = s_long(d - 4800 + m / 10); if (y < 1) { y -= 1; } *py = y; } if (pm) { *pm = s_long(m + 3 - 12 * (m / 10)); } if (pd) { *pd = s_long(e - (153 * m + 2) / 5 + 1); }
    }
  }
  void _hms(double jds, s_long* ph, s_long* pmt, s_long* ps) { meta::s_ll ss = _n_sec(jds); ss %= 86400; if (ph) { *ph = s_long(ss / 3600); } ss %= 3600; if (pmt) { *pmt = s_long(ss / 60); } if (ps) { *ps = s_long(ss % 60); } }
}

s_long _unitydate::jdn() const { return _n(_jds); }
meta::s_ll _unitydate::f_sec() const { return meta::s_ll(_jds + _jds_eps); }

_unitydate::_unitydate(s_long y, s_long m, s_long d, s_long h, s_long min, s_long s)
{
  if (m < 1) { m = 1; } else if (m > 12) { m = 12; }
  if (d < 1) { d = 1; } else if (d > 31) { d = 31; }
  if (h < 0) { h = 0; } else if (h > 23) { h = 23; }
  if (min < 0) { min = 0; } else if (min > 59) { min = 59; }
  if (s < 0) { s = 0; } else if (s > 59) { s = 59; }
  meta::s_ll n2 = _n_any(y, m, d);
  const meta::s_ll max = meta::s_long_mp - 1;
  if (n2 > max) { n2 = max; } else if (n2 < -max) { n2 = -max; }
  _jds = double(_jds_nhms(s_long(n2), h, min, s));
}

_unitydate& _unitydate::add_datetime(s_long dy, s_long dm, s_long dd, s_long dh, s_long dmin, s_long ds)
{
  const meta::s_ll max = meta::s_long_mp - 1;
  meta::s_ll __nsec = _n_sec(_jds);
  meta::s_ll n0 = __nsec / 86400; meta::s_ll ns0 = __nsec % 86400;
  s_long y_, m_, d_; _ymd(s_long(n0), &y_, &m_, &d_);
  meta::s_ll n2 = n0; meta::s_ll ns2 = ns0;
  if (dy || dm)
  {
    meta::s_ll y(y_), m(m_); if (y < 0) { y += 1; }
    y += dy; y += dm / 12; m += dm % 12; if (m < 1) { m += 12; y -= 1; } else if (m > 12) { m -= 12; y += 1; }
    if (y < 1) { y -= 1; }
    if (y > max) { y = max; } else if (y < -max) { y = -max; }
    n2 = _n_any(s_long(y), s_long(m), d_);
  }
  n2 += dd;
  if (dh || dmin || ds)
  {
    if (dh) { ns2 += meta::s_ll(dh) * 3600; }
    if (dmin) { ns2 += meta::s_ll(dmin) * 60; }
    if (ds) { ns2 += ds; }
    n2 += ns2 / 86400;
    ns2 %= 86400;
    if (ns2 < 0) { ns2 += 86400; n2 -= 1; }
  }
  if (n2 > max) { n2 = max; ns2 = 0; } else if (n2 < -max) { n2 = -max; ns2 = 0; }
  if (n2 != n0 || ns2 != ns0)
  {
    double f = _jds - std::floor(_jds + _jds_eps); if (f < 0.) { f = 0.; }
    set(double(n2 * 86400 + ns2 - 43200) + f);
  }
  return *this;
}

s_long _unitydate::d_year() const { s_long x; _ymd(_n(_jds), &x, 0, 0); return x; }
s_long _unitydate::d_month() const { s_long x; _ymd(_n(_jds), 0, &x, 0); return x; }
s_long _unitydate::d_day() const { s_long x; _ymd(_n(_jds), 0, 0, &x); return x; }
s_long _unitydate::d_day_of_week() const { s_long n = _n(_jds); n %= 7; if (n < 0) { n += 7; } n += 1; return n; }
s_long _unitydate::d_hour() const { meta::s_ll ss = _n_sec(_jds); ss %= 86400;  return s_long(ss / 3600); }
s_long _unitydate::d_minute() const { meta::s_ll ss = _n_sec(_jds); ss %= 3600;  return s_long(ss / 60); }
s_long _unitydate::d_second() const { meta::s_ll ss = _n_sec(_jds); return ss % 60; }
double _unitydate::d_ms() const { double f = _jds - std::floor(_jds + _jds_eps); if (f < 0.) { f = 0.; } return f * 1000.; }
void _unitydate::d_get(s_long* py, s_long* pm, s_long* pd, s_long* ph, s_long* pmin, s_long* ps) const
{
  if (py || pm || pd) { _ymd(_n(_jds), py, pm, pd); }
  if (ph || pmin || ps) { _hms(_jds, ph, pmin, ps); }
}

_unitydate d_datetime(s_long y, s_long m, s_long d, s_long h, s_long mt, s_long s) { return _unitydate(y, m, d, h, mt, s); }
_unitydate d_time(s_long h, s_long mt, s_long s) { return _unitydate(0, 0, 0, h, mt, s); }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// o_api non-templates
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* unity::cpvoid() const
      { return o_api(this).pvoid(1); }
void* unity::cpvoidkey() const
      { return o_api(this).pvoid(0); }
o_type_info unity::objTypeInfo(s_long flags) const throw()
      { return o_api(this).type_info(flags); }
o_ref_info unity::objRefInfo() const throw()
      { return o_api(this).ref_info(); }
bool unity::objSetStrongRef(bool b_strong)
      {
        if (!isObject()) { return false; }
        if (b_strong == !!(ut & xfObjStrongRef)) { return true; }
        o_api a(this);
        s_long res = a.rc_incdec(b_strong ? (1+8) : (2+4));
        if (res) { a.upd_ut_ref_type(b_strong); }
        return !!res;
      }
s_long unity::objItfsRemoveInd(s_long ind)
      { return o_api(this).itfs_remove_ind(ind); }
s_long unity::objItfsFindPtr(o_itfset_base* p_itfs) const
      { return o_api(this).find_by_pitfs(p_itfs); }
o_itfset_base* unity::objItfsByInd(s_long ind) const
      {
        if (ind < 0) { return 0; }
        o_api a(this);
        o_api::critsec_rc(a.prc);
        if (!a.pidyn) { return 0; }
        if (ind >= a.pidyn->itfslist.n()) { return 0; }
        return a.pidyn->itfslist[ind].p_itfs;
      }
s_long unity::objItfsN() const
      { return o_api(this).itfslist_n(); }

_o_refcnt::_o_refcnt() : rc_posm(0), rc_pobj(0), rc_pcsdx(0), rc_strong(0), rc_weak(0), rc_flags(0) {}
_o_itfslist::handle::handle() : p_itfs(0), p_itfssm(0) {}
bool _o_itfslist::handle::b_valid() const { return p_itfs && p_itfssm; }
_o_itfslist::_o_itfslist() : pmsm(unity_common::ls_modsm), prc(0) {}

o_type_info::o_type_info(const char* s_, const char* d_, s_long t_ind_, Psm msm_, Psm osm_, void* pobj_, s_long t_size_) : _tstat(s_), _tdyn(d_), _pmsm(msm_), _posm(osm_), _pobj(pobj_) { _t_ind = t_ind_; _t_size = t_size_; }
bool o_type_info::b_valid() const { return _pmsm && (_tdyn || _tstat || _t_ind || _t_size); }
const char* o_type_info::ptstat() const { return _tstat ? _tstat : ""; }
const char* o_type_info::ptdyn() const { return _tdyn ? _tdyn : ""; }
bool o_type_info::b_local() const { return _pmsm == unity_common::ls_modsm; }
bool o_type_info::b_tstat() const { return !!_tstat; }
bool o_type_info::b_tdyn() const { return !!_tdyn; }
bool o_type_info::b_t_ind() const { return !!_t_ind; }
bool o_type_info::b_t_size() const { return _t_ind >= 0; }
bool o_type_info::b_same_tstat_cm(const std::type_info& ti) const { return _tstat && (strcmp(_tstat, ti.name()) == 0); }
bool o_type_info::b_same_tdyn_cm(const std::type_info& ti) const { return _tdyn && (strcmp(_tdyn, ti.name()) == 0); }
bool o_type_info::b_same_tstat(const std::type_info& ti) const { return _tstat && b_local() && (strcmp(_tstat, ti.name()) == 0); }
bool o_type_info::b_same_tdyn(const std::type_info& ti) const { return _tdyn && b_local() && (strcmp(_tdyn, ti.name()) == 0); }
bool o_type_info::operator==(const std::type_info& ti) const { return b_same_tdyn(ti) || b_same_tstat(ti); }
bool o_type_info::operator!=(const std::type_info& ti) const { return !operator==(ti); }


struct unity::o_api::_itfslist_temp1
{
  struct handle
  {
    o_itfset_base* p_itfs; o_itfset_base* p_itfs_src; unity_common::__Psm p_itfssm; __Pitfs_new_copy f_nc; __Pitfs_delete f_del; __Pitfs_xx_after_construct f_ac; __Pitfs_xx_before_destroy f_bd;
    handle();
    bool b_valid_f() const;
    bool b_valid_all() const;
  };
  cpparray_t<handle> list; meta::s_ll n_del; meta::s_ll n_bd;
  _itfslist_temp1();
  ~_itfslist_temp1();
  void clear() throw();
private: _itfslist_temp1(const _itfslist_temp1&); void operator=(const _itfslist_temp1&);
};

unity::o_api::_itfslist_temp1::handle::handle() : p_itfs(0), p_itfs_src(0), p_itfssm(0), f_nc(0), f_del(0), f_ac(0), f_bd(0) {}
bool unity::o_api::_itfslist_temp1::handle::b_valid_f() const { return p_itfssm && f_del && f_nc && f_ac && f_bd; }
bool unity::o_api::_itfslist_temp1::handle::b_valid_all() const { return p_itfs && b_valid_f(); }
unity::o_api::_itfslist_temp1::_itfslist_temp1() : n_del(0), n_bd(0) {}
unity::o_api::_itfslist_temp1::~_itfslist_temp1() { clear(); }
void unity::o_api::_itfslist_temp1::clear() throw()
{
  for (meta::s_ll i = n_bd - 1; i >= 0; --i) { if (i < list.n() && list[i].b_valid_all()) { try { list[i].f_bd(list[i].p_itfs, 0); } catch (...) {} } }
  for (meta::s_ll i = 0; i < n_del; ++i) { if (i < list.n() && list[i].b_valid_all()) { try { list[i].f_del(list[i].p_itfs); } catch (...) {} } }
  list.clear(); n_bd = 0; n_del = 0;
}

unity::o_api::critsec_rc::~critsec_rc()
  { if (!posm) { return; } ((__Posm_lock_destroy)posm(unity_common::osm_lock_destroy))(&stg[0]); }
unity::o_api::critsec_rc::critsec_rc(_o_refcnt* prc)
  { __pad1 = 0; posm = 0; for (int i = 0; i < n; ++i) { stg[i]=0; } if (!prc) { return; } posm = prc->rc_posm; if (!posm) { return; } ((__Posm_lock_create)posm(unity_common::osm_lock_create))(&stg[0], prc, 1); }
s_long unity::o_api::critsec_rc::state() const
  { if (!posm) { return -1; } __Posm_lock_state f = (__Posm_lock_state)posm(unity_common::osm_lock_state); if (!f) { return -1; } return f(&stg[0]); }

  // Creates non-functional object.
unity::o_api::o_api() throw()
    : pu(0), prc(0), pidyn(0) {}

  // Creates fully functional object.
  // NOTE "const" is ignored.
unity::o_api::o_api(const unity* pu_) throw()
{
  prc = 0; pidyn = 0;
  pu = const_cast<unity*>(pu_); if (!(pu && pu->isObject())) { return; }
  void* p11 = 0; s_long ff1 = 0;
  while (true) { p11 = pu->_data.p1; ff1 = pu->ut; void* p12 = pu->_data.p1; s_long ff2 = pu->ut; if (p11 == p12 && ff1 == ff2) { break; } sleep_mcs(100); }
  pidyn = (ff1 & xfObjItfsList) ? (_o_itfslist*)p11 : 0;
  prc = pidyn ? pidyn->prc : (_o_refcnt*)p11;
}

  // Creates partially functional object (not able to manipulate parent unity object).
unity::o_api::o_api(_o_itfslist& list) throw()
{
  pu = 0;
  pidyn = &list;
  prc = list.prc;
}
  // Implementation; assumes p is (b_pidyn ? _o_itfslist* : _o_refcnt*).
unity::o_api::o_api(void* p, bool b_pidyn) throw()
  : pu(0), prc(0), pidyn(0)
{
  if (!p) { return; }
  if (b_pidyn) { pidyn = (_o_itfslist*)p; prc = pidyn->prc; return; }
  prc = (_o_refcnt*)p;
}
void* unity::o_api::p_head() const throw()
  { if (pidyn) { return pidyn; } return prc; }
  // flags OR-ed:
  //  1 - get static type info,
  //  2 - get dynamic type info,
  //  4 - get type index (see type_index_t in vecm_hashx.h),
  //  8 - return _pobj even if the object is deleted.
  //  16 - get sizeof for object static type.
o_type_info unity::o_api::type_info(s_long flags) const throw()
{
  o_type_info ti(0,0,0,0,0,0,-1);
  if (!prc) { return ti; }
  ((__Posm_type_info)prc->rc_posm(unity_common::osm_type_info))(prc, &ti, flags);
  return ti;
}
  // ref_info contains full info on number of references, plus boolean strong/weak ref. type flag taken from the wrapping unity object (pu).
  //    If pu == 0 (wrapping object not accessible), ref. type will be set to undefined value (0).
o_ref_info unity::o_api::ref_info() const throw()
{
  o_ref_info ri(0, 0, 0);
  if (!prc) { return ri; }
  ((__Posm_ref_info)prc->rc_posm(unity_common::osm_ref_info))(prc, &ri, pu ? ((pu->ut & xfObjStrongRef) ? 3 : 2) : 0);
  return ri;
}
  // Returns a pointer to the referenced object. If b_checked and the object is deleted, returns 0.
  //    Does not check compat. of the referenced object with the current binary module.
void* unity::o_api::pvoid(bool b_checked) const throw()
{
  if (!prc) { return 0; } if (!prc->rc_posm) { return 0; }
  return ((__Posm_pvoid)prc->rc_posm(unity_common::osm_pvoid))(prc, b_checked);
}
  // Same as pvoid(true), + dereference cref_t<T, L> into T.
  //  Does not check compat. of T and cref_t<T> with the current binary module.
  // tname:
  //    b) typeid(T).name().
  //    c) If T is any of cref_t<R>, thname == "|cref_t|" + typeid(R).name().
  // t_size:
  //    a, b, c) -1 - sizeof(T) is not checked.
  //    b) >=0 - sizeof(T) is checked for == t_size in case if tname == typeid(T).name() in the module of T.
  //      (For t_ind matching, t_size is ignored.)
  //    c) >= 0 - sizeof(R), assuming T is a cref_t<R>.
void* unity::o_api::prefo(const char* tname, s_long t_ind, s_long t_size) const throw()
{
  if (!prc) { return 0; } if (!prc->rc_posm) { return 0; }
  return ((__Posm_prefo)prc->rc_posm(unity_common::osm_prefo))(prc, tname, t_ind, t_size);
}

  // Change type of reference.
  //  Argument and returned value: same as _o_sm_t<Obj>::rc_incdec (see).
  //  NOTE If pu != 0, pu and its flags are ignored. See also upd_ut_ref_type.
s_long unity::o_api::rc_incdec(s_long flags) const throw()
{
  if (!prc) { return 0; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  __Prc_incdec f_incdec = (__Prc_incdec)prc->rc_posm(unity_common::osm_rc_incdec); if (!f_incdec) { return 0; }
  try { return f_incdec(prc, flags); } catch (...) {}
  return 0;
}
  // Set flag xfObjStrongRef in pu->ut as specified.
  //    Should be called after rc_incdec, when pu's ref. type has changed.
  //  Returns: 1 - success (if pu != 0, the function always succeeds).
  //    -1 - pu == 0.
s_long unity::o_api::upd_ut_ref_type(bool b_strong) const throw()
{
  if (!pu) { return -1; }
  if (b_strong) { pu->ut |= unity::xfObjStrongRef; return 1; }
  pu->ut &= ~s_long(unity::xfObjStrongRef);
  return 1;
}
  // Creates interface sets list object if it not existed, and sets it to pidyn.
  //    If the list does not exist, pu must be != 0:
  //      1) the function rechecks and updates the wrapping unity obect.
  //      2) the wrapping unity obect may be from another binary module, and its pmsm is used for _o_itfslist creation.
  //  NOTE this function will always return 0 is called from within an attached interface,
  //    because interfaces already are in the list and use pidyn to access the list.
  // Returns: 1 - success. 0 - list object already exists. -1 - pu == 0 or !isObject(), cannot do. -2 - failure.
s_long unity::o_api::itfslist_create() throw()
{
  if (!prc) { return -2; }
  if (pidyn) { return 0; }
  if (!pu || !pu->isObject()) { return -1; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  if (!pu->isObject()) { return -1; }
  if (pu->_data.p1 != prc) { return -2; }
  if (pu->ut & xfObjItfsList) { pidyn = (_o_itfslist*)pu->_data.p1; return 0; }
  pidyn = (_o_itfslist*)pu->_Litfslist_cr(); if (!pidyn) { return -2; }
  pidyn->pmsm = pu->pmsm;
  pidyn->prc = prc;
  pu->_data.p1 = pidyn;
  pu->ut |= xfObjItfsList;
  return 1;
}
  // Returns:
  //  >= 0 - the number of interface sets attached to object.
  //  -2 - failure (o_api is not valid).
s_long unity::o_api::itfslist_n() const throw()
{
  if (!prc) { return -2; }
  if (!pidyn) { return 0; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  return s_long(pidyn->itfslist.n());
}
  // Appends copies of all interface sets from src. In case of any failure, reverts changes.
  // Returns:
  //    1 - success.
  //    0 - src references same object as *this.
  //    -1 - pidyn == 0 (list of interf. sets is not created - call itfslist_create() first).
  //    -2 - failure.
s_long unity::o_api::itfs_append_cp(const o_api& src) const throw()
{
  if (!(prc && src.prc)) { return -2; }
  if (!pidyn) { return -1; }
  if (this == &src || pidyn == src.pidyn) { return 0; }
  _itfslist_temp1 lt;
  if (1)
  {
    critsec_rc __lock(src.prc); if (sizeof(__lock)) {}
    if (!src.pidyn) { return 1; }
    cpparray_t<_o_itfslist::handle>& ls = src.pidyn->itfslist;
      if (ls.n() <= 0) { return 1; }
    if (!lt.list.resize(ls.n())) { return -2; }
    lt.list._set_n_u(0);

    for (meta::s_ll i = 0; i < ls.n(); ++i)
    {
      _o_itfslist::handle& hs = ls[i]; if (!hs.b_valid()) { continue; }
      _itfslist_temp1::handle& ht = lt.list[lt.list.n()];
      ht.p_itfssm = hs.p_itfssm;
      ht.f_nc = (__Pitfs_new_copy)hs.p_itfssm(unity_common::itfssm_new_copy);
      ht.f_del = (__Pitfs_delete)hs.p_itfssm(unity_common::itfssm_delete);
      ht.f_ac = (__Pitfs_xx_after_construct)hs.p_itfssm(unity_common::itfssm_xx_after_construct);
      ht.f_bd = (__Pitfs_xx_before_destroy)hs.p_itfssm(unity_common::itfssm_xx_before_destroy);
        if (!ht.b_valid_f()) { continue; }
      o_itfset_base* p = 0;
      try { p = ht.f_nc(hs.p_itfs); } catch (...) {} if (!p) { return -2; }
      p->__pidyn = pidyn;
      ht.p_itfs = p;
      ht.p_itfs_src = hs.p_itfs;
      lt.list._set_n_u(lt.list.n() + 1);
      lt.n_del = lt.list.n();
    }
  }

  if (1)
  {
    critsec_rc __lock(prc); if (sizeof(__lock)) {}
    cpparray_t<_o_itfslist::handle>& ld = pidyn->itfslist;
    meta::s_ll n0 = ld.n();
    meta::s_ll nr0 = _itfslist_nrsv(n0);
    meta::s_ll nr2 = _itfslist_nrsv(n0 + lt.list.n());
    if (nr2 != nr0) { if (!ld.resize(nr2)) { return -2; } ld._set_n_u(n0); }
    for (meta::s_ll i = 0; i < lt.list.n(); ++i)
    {
      _itfslist_temp1::handle& ht = lt.list[i];
      _o_itfslist::handle& hd = ld[n0 + i];
      hd.p_itfssm = ht.p_itfssm; hd.p_itfs = ht.p_itfs;
      s_long res = 0; try { res = ht.f_ac(ht.p_itfs, ht.p_itfs_src, 0); } catch (...) {} if (!res) { lt.clear(); return -2; }
      lt.n_bd = i + 1;
    }
    lt.n_bd = 0; lt.n_del = 0;
    ld._set_n_u(n0 + lt.list.n());
    lt.clear();
  }

  return 1;
}
  // Removes the specified set of interfaces.
  // Returns: 1 - success, -1 - wrong ind, -2 - failure.
  // NOTE if the last set of interfaces is removed, the function does not remove the empty list of sets.
s_long unity::o_api::itfs_remove_ind(s_long ind) const throw()
{
  if (!prc) { return -2; }
  if (!pidyn) { return -1; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  cpparray_t<_o_itfslist::handle>& ld = pidyn->itfslist;
  meta::s_ll n0 = ld.n();
  if (!(ind >= 0 && ind < n0)) { return -1; }
  meta::s_ll nr0 = _itfslist_nrsv(n0);
  meta::s_ll nr2 = _itfslist_nrsv(n0 - 1);
  _o_itfslist::handle& hd = ld[ind];
  if (hd.b_valid())
  {
    __Pitfs_delete f_del = (__Pitfs_delete)hd.p_itfssm(unity_common::itfssm_delete);
    __Pitfs_xx_before_destroy f_bd = (__Pitfs_xx_before_destroy)hd.p_itfssm(unity_common::itfssm_xx_before_destroy);
      if (!(f_del && f_bd)) { return -2; }
    try { f_bd(hd.p_itfs, 1); } catch (...) {}
    try { f_del(hd.p_itfs); } catch (...) {}
  }
  for (meta::s_ll i = ind + 1; i < ld.n(); ++i) { ld[i - 1] = ld [i]; }
  if (nr2 != nr0) { ld.resize(nr2); }
  ld._set_n_u(n0 - 1);
  return 1;
}
  // Removes all sets of interfaces.
  // Returns: 1 - success, 0 - no list of interface sets (pidyn == 0), -2 - failure.
  // NOTE the function does not remove the empty list of sets.
s_long unity::o_api::itfs_remove_all() const throw()
{
  if (!prc) { return -2; } if (!pidyn) { return 0; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  cpparray_t<_o_itfslist::handle>& ld = pidyn->itfslist;
  for (meta::s_ll i = ld.n() - 1; i >= 0; --i)
  {
    _o_itfslist::handle& hd = ld[i];
    __Pitfs_xx_before_destroy f_bd = (__Pitfs_xx_before_destroy)hd.p_itfssm(unity_common::itfssm_xx_before_destroy);
    if (f_bd) { try { f_bd(hd.p_itfs, 0); } catch (...) {} }
  }
  for (meta::s_ll i = ld.n() - 1; i >= 0; --i)
  {
    _o_itfslist::handle& hd = ld[i];
    __Pitfs_delete f_del = (__Pitfs_delete)hd.p_itfssm(unity_common::itfssm_delete);
    if (f_del) { try { f_del(hd.p_itfs); } catch (...) {} }
  }
  return 1;
}
  // Create new copy of structures associated with existing object reference, increase ref. count.
  //  pmsm_itfslist: static methods getter of the module for which the copy is created.
  //  Must be != 0 if pidyn != 0.
  //  flags:
  //    type of reference: 1 - strong, 2 - weak (only one of them must be set).
  //  NOTE If pu != 0, its pmsm and flag xfObjStrongRef are ignored.
  // Returns: o_api with pu == 0, prc != 0, and also pidyn != 0 if the current o_api references a set of interface lists.
unity::o_api unity::o_api::_ref_newcp(unity_common::__Psm pmsm_dest, s_long flags) const throw()
{
  o_api x;
  if (!prc || (flags & 3) == 3 || (flags & 3) == 0) { return x; }
  if (pidyn && !pmsm_dest) { return x; }
  __Prc_incdec f_incdec = (__Prc_incdec)prc->rc_posm(unity_common::osm_rc_incdec); if (!f_incdec) { return x; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  s_long res = f_incdec(prc, ((flags & 1) ? 1 : 0) | ((flags & 2) ? 4 : 0)); if (res == 0) { return x; }
  x.prc = prc;
  if (pidyn)
  {
    x.pidyn = unity::_Litfslist_cr(pmsm_dest); if (!x.pidyn) { x._ref_del(flags & 3); return o_api(); }
    x.pidyn->prc = x.prc;
    x.pidyn->pmsm = pmsm_dest;
    res = x.itfs_append_cp(*this); if (res < 1) { x._ref_del(flags & 3); return o_api(); }
  }
  return x;
}
  // Decrease the object ref. count and remove, as necessary, structures associated with that reference.
  //  flags must specify type of reference to release: 1 - strong, 2 - weak (only one of them must be set).
  //  NOTE Even if pu != 0, its pmsm and flag xfObjStrongRef are ignored.
  // Returns:
  //  2: all ref. counts are 0, all structures deleted, o_api is no longer valid -- prc == 0, pidyn == 0.
  //  1: one reference released, pidyn (if existed) is deleted and set to 0.
  //    prc remains valid.
  //    (The referenced object may have been deleted if the last strong ref. released.)
  //  -1: bad flags.
  //  -2: failure (o_api does not reference a correct object).
s_long unity::o_api::_ref_del(s_long flags) throw()
{
  if (!prc) { return -2; }
  if ((flags & 3) == 3 || (flags & 3) == 0) { return -1; }
  __Prc_incdec f_incdec = (__Prc_incdec)prc->rc_posm(unity_common::osm_rc_incdec);
    if (!f_incdec) { return -2; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  if (pidyn)
  {
    itfs_remove_all();
    if (pidyn->pmsm) { unity::_Litfslist_del(pidyn->pmsm, pidyn); }
    pidyn = 0;
  }
  s_long res = f_incdec(prc, ((flags & 1) ? 2 : 0) | ((flags & 2) ? 8 : 0));
  if (res & 0x20) { prc = 0; return 2; }
  return 1;
}
  // Returns: >= 0 - index of the found set. -1 - not found. -2 - failure.
s_long unity::o_api::find_by_pitfs(o_itfset_base* p) const throw()
{
  if (!prc) { return -2; } if (!pidyn) { return -1; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  cpparray_t<_o_itfslist::handle>& ls = pidyn->itfslist;
  for (meta::s_ll i = 0; i < ls.n(); ++i) { _o_itfslist::handle& hs = ls[i]; if (!hs.b_valid()) { continue; } if (hs.p_itfs == p) { return s_long(i); } }
  return -1;
}
  // NOTE Class method.
meta::s_ll unity::o_api::_itfslist_nrsv(meta::s_ll n) const throw()
  { if (n < 0) { return 0; } if (n < 8) { return n; } if (n < 64) { return (n | 7) + 1; } return (n | 31) + 1; }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// paramline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string paramline::cpterm = "; ";
const std::string paramline::ceterm = " = ";
const std::string paramline::cvterm = "|";

const std::wstring paramline::wpterm = L"; ";
const std::wstring paramline::weterm = L" = ";
const std::wstring paramline::wvterm = L"|";

const unity& paramline::_0 = unity::_0;

unity& paramline::decode1v(const std::wstring& ssrc0, unity& dest)
{
  dest.clear();
  try
  {
    std::wstring s; s_long flags(0);
    x_replace4(ssrc0, s, flags);
    _t_wz pos0 = s.find(L";"); if (pos0 != nposw) { s.resize(pos0); }
    if (dest.isLocal())
    {
      dest = _trim(s, L" ");
      x_decode1v(dest, false, flags);
    }
    else
    {
      unity temp = _trim(s, L" ");
      x_decode1v(temp, false, flags);
      dest = temp;
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode1v.1", e.what(), ssrc0); }
  catch(...) { throw XUExec("decode1v.2", ssrc0); }
  return dest;
}

unity& paramline::decode(const std::wstring& ssrc0, unity& mh, bool useMap, const std::wstring& pterm_0)
{
  std::wstring src;
  try
  {
      std::wstring pterm = pterm_0.length() == 0 ? wCRLF : pterm_0;
      if (useMap) { mh.map_clear(false); } else { mh.hash_clear(false); }

      std::wstring s, n; unity v;
      s_long flags(0);
      x_replace4(ssrc0, src, flags);
      src = replace(src, L";", pterm);
      _t_wz pos0(0), pos2(0), pos_eq(0);
      while(pos0 < src.length())
      {
          pos2 = src.find(pterm, pos0);
          if (pos2 == nposw) { pos2 = src.length(); }
          s = src.substr(pos0, pos2 - pos0);

          pos_eq = s.find(L"=");
          if (pos_eq != nposw)
          {
              n = _trim(s.substr(0, pos_eq), L" ");
              v = _trim(s.substr(pos_eq + 1), L" ");
              x_replace2a(n, flags);
              x_decode1v(v, false, flags);
              if (v.objPtr<unity>()) { v.objPtr<unity>()->u_name_set(n); }
              mh.assoc_set(n, v, true);
          }
          else
          {
              n = _trim(s, L" ");
              if (n.length() > 0)
              {
                x_replace2a(n, flags);
                mh.assoc_set(n, unity(), true);
              }
          }
          pos0 = pos2 + pterm.length();
      }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode.1", e.what(), src); }
  catch(...) { throw XUExec("decode.2", src); }
  return mh;
}

unity& paramline::decode_tree(const std::wstring& ssrc0, unity& mh, s_long flags, const std::wstring& pterm_0)
{
  try {
    std::wstring pterm2 = pterm_0.length() == 0 ? wCRLF : pterm_0;
    bool b_map = !!(flags & 0x1); bool b_clear = (flags & 0x4) == 0; bool b_keep = (flags & 0x2) == 0; bool b_skipslc = !!(flags & 0x8); bool b_convcr = !!(flags & 0x10); bool b_noinclpath = (flags & 0x20) == 0; bool b_braces = !!(flags & 0x40);
      if (!b_clear && mh.isMap()) { b_map = true; }
      if (b_map) { if (b_clear || !mh.isMap()) { mh.map_clear(true); } } else { if (b_clear || !mh.isHash()) { mh.hash_clear(true); } }
    const unity k_empty_str = L"";

    std::wstring s2;
    if (b_convcr)
    {
      s2.reserve(ssrc0.size());
      for (_t_wz pos = 0; pos < ssrc0.size(); ++pos)
      {
        wchar_t c = ssrc0[pos];
        if (c == L'\r')
        {
          _t_wz pos2 = pos + 1;
          if (pos2 < ssrc0.size() && ssrc0[pos2] == L'\n') { pos = pos2; }
          s2 += L"\r\n";
        }
        else if (c == L'\n')
        {
          _t_wz pos2 = pos + 1;
          if (pos2 < ssrc0.size() && ssrc0[pos2] == L'\r') { pos = pos2; }
          s2 += L"\r\n";
        }
        else { s2 += c; }
      }
    }

    _t_wz pos(0); const std::wstring& ssrc = b_convcr ? s2 : ssrc0;
    unity stack, last_path; if (b_braces) { last_path.u_clear(utUnityArray); stack.ua_append(last_path); }
    while (pos < ssrc.length())
    {
      _t_wz pos2 = ssrc.find(pterm2, pos); if (pos2 == nposw) { pos2 = ssrc.length(); }
      if (pos2 > pos)
      {
        bool b_skip = false;
        if (b_skipslc) // test/skip C-style comments
        {
          _t_wz pos3 = _find_char_except(ssrc, pos, pos2, L"\t ");
          if (pos3 + 2 <= pos2 && ssrc[pos3] == '/' && ssrc[pos3 + 1] == '/')
            { b_skip = true; }
        }
        if (!b_skip) // skip lines w/o data
        {
          _t_wz pos3 = _find_char_except(ssrc, pos, pos2, L" ");
          if (pos3 >= ssrc.length()) { b_skip = true; }
        }
        if (!b_skip && b_braces) // test/process lines with braces only
        {
          _t_wz pos3 = _find_char_except(ssrc, pos, pos2, L" {}");
          if (pos3 >= ssrc.length())
          {
            for (_t_wz i = pos; i < pos2; ++i)
            {
              if (ssrc[i] == L'{') { if (last_path == stack.ua_last()) { last_path.ua_append(unity()); } stack.ua_append(last_path); }
                else if (ssrc[i] == L'}') { last_path = stack.ua_last(); if (stack.uaS() > 1) { stack.ua_resize(stack.uaUb(), -1); } }
            }
            b_skip = true;
          }
        }
        if (!b_skip) // decode branch, calc. path, merge
        {
          unity h; paramline().decode(ssrc.substr(pos, pos2 - pos), h, false, pterm2);

          bool b_hk = h.u_has(k_empty_str, 6);
          const unity* pk = b_hk ? &h[k_empty_str] : &k_empty_str;
          if (b_braces)
          {
            last_path = stack.ua_last();
            if (pk->utype() == utUnityArray)
            {
              if (last_path.uaS() > 0) { for (int j = pk->arrlb(); j <= pk->arrub(); ++j)  { last_path.ua_append(pk->ref<utUnity>(j)); } *(unity*)pk = last_path; }
                else { last_path = *pk; }
              pk = &last_path;
            }
            else
            {
              if (last_path.uaS() > 0) { if (b_hk) { *(unity*)pk = last_path; } else { h.hash_set(k_empty_str, last_path, 0, h.hashl_first()); } pk = &last_path; }
            }
          }

          unity* node = &mh;
          if (pk->utype() == utUnityArray)
          {
            for (int j = pk->arrlb(); j <= pk->arrub(); ++j)
            {
              const unity& k2 = pk->ref<utUnity>(j);
              node->assoc_set(k2, unity(), true);
              node = &(*node)[k2];
              if (node->isAssoc()) {}
              else if (!node->isEmpty() && b_keep) { node = 0; break; }
              else
              {
                if (b_map) { node->u_clear(utMap); node->mapFlags_set(0, mh.mapFlags()); }
                  else { node->u_clear(utHash); node->hashFlags_set(0, mh.hashFlags()); }
              }
            }
          }
          if (node) { if (b_noinclpath) { h.assoc_del(k_empty_str); } if (h.hashS_c() > 0) { paramline().merge(*node, h, b_keep); } }
        }
      }
      pos = pos2 + pterm2.length();
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode_tree.1", e.what(), ssrc0); }
  catch(...) { throw XUExec("decode_tree.2", ssrc0); }
  return mh;
}

std::wstring& paramline::encode(const unity& mh, std::wstring& sdest, const std::wstring& pterm_0)
{
  try
  {
    sdest.clear(); if (!mh.isAssoc() || mh.assocS_c() == 0) { return sdest; }
    std::wstring s1, s2; std::wstring pterm = pterm_0.length() == 0 ? wpterm : pterm_0;
    s_long pos = mh.assocl_first();
    while (pos != mh.assocl_noel())
    {
      x_encode1(mh.assocl_key(pos), s1, true, false); x_encode1(mh.assocl_c(pos), s2, false, false);
      if (s2.length() || s1.empty()) { s1 += weterm; s1 += s2; } s2.clear(); x_replace2a(s1, 1);
      pos = mh.assocl_next(pos);
      if (pos != mh.assocl_noel()) { s1 += pterm; }
      sdest += s1;
    }
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode.1", e.what(), mh.vflstr()); }
  catch(...) { throw XUExec("encode.2", mh.vflstr()); }
}

std::wstring paramline::encode1n(const unity& name)
{
  try { std::wstring s; x_encode1(name, s, true, false); x_replace2a(s, 1); return s; }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1n.1", e.what(), name.vflstr()); }
  catch(...) { throw XUExec("encode1n.2", name.vflstr()); }
}

std::wstring& paramline::encode1v(const unity& value, std::wstring& s)
{
  try { x_encode1(value, s, false, false); x_replace2a(s, 1); return s; }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1v.1", e.what(), "Failed to encode val.", value.utype()); }
  catch(...) { throw XUExec("encode1v.2", "Failed to encode val.", value.utype()); }
}

void paramline::x_encode_branch(const unity& mh, const std::wstring& path, std::wstring& sdest, hashx<const unity*, int>& hstop, const std::wstring& pterm, const std::wstring& pterm2, const std::wstring& pathpfx)
{
    // return if not assoc or empty assoc
    // append path if not empty
    // append non-branch pairs, collect branch pairs, skip stopped
    // append term2
    // for each branch, call x_encode_branch recursively with path + string(branch key)
  if (!(mh.isAssoc() && mh.assocS_c())) { return; }
  bool bpath = false;
  vec2_t<s_long> ibr;
  hashx<std::wstring, int> hsk;
  s_long res = -1;
  for (s_long ind = mh.assocl_first(); ind != mh.assocl_noel(); ind = mh.assocl_next(ind))
  {
    const unity& k = mh.assocl_key(ind);
      res = hstop.insert(&k);
        if (res == 0) { continue; } if (res != 1) { throw XUExec("x_encode_branch.1"); }
    const unity& v = mh.assocl_c(ind); const unity* pv = v.objPtr_c<unity>();
    if (v.isAssoc() || (pv && pv->isAssoc())) { ibr.push_back(ind); }
    else
    {
      meta::s_ll ls = k.lenstr();
      if (ls < -2) { throw XUExec("x_encode_branch.4"); }
      if (ls == -1 || ls == 0) { continue; }
      std::wstring s1, s2;
      x_encode1(k, s1, true, false);
        res = hsk.insert(s1);
          if (res == 0) { continue; } if (res != 1) { throw XUExec("x_encode_branch.2"); }
      x_encode1(v, s2, false, false);
      if (!bpath) { bpath = true; if (path.length()) { sdest += path; sdest += pterm; } }
        else { sdest += pterm; }
      if (s2.length() || s1.empty()) { s1 += weterm; s1 += s2; } s2.clear(); x_replace2a(s1, 1);
      sdest += s1;
    }
  }
  if (bpath) { sdest += pterm2; }
  for (s_long i = 0; i < ibr.n(); ++i)
  {
    const unity& k = mh.assocl_key(ibr[i]);
    const unity& v = mh.assocl_c(ibr[i]); const unity* pv = v.objPtr_c<unity>();
      if (!pv) { pv = &v; }
    std::wstring s1, sp;
      x_encode1(k, s1, false, true);
        res = hsk.insert(s1);
          if (res == 0) { continue; } if (res != 1) { throw XUExec("x_encode_branch.3"); }
      x_replace2a(s1, 1);
      sp = path; if (sp.empty()) { sp += pathpfx; } sp += wvterm; sp += s1; s1.clear();
    x_encode_branch(*pv, sp, sdest, hstop, pterm, pterm2, pathpfx);
  }
}

std::wstring& paramline::encode_tree(const unity& mh, std::wstring& sdest, const std::wstring& pterm_0, const std::wstring& pterm2_0)
{
  try
  {
    sdest.clear(); if (!mh.isAssoc() || mh.assocS_c() == 0) { return sdest; }
    hashx<const unity*, int> hstop;
    std::wstring pterm = pterm_0.length() == 0 ? wpterm : pterm_0;
    std::wstring pterm2 = pterm2_0.length() == 0 ? wCRLF : pterm2_0;
    std::wstring pathpfx = _trim(weterm, L" ", true, true);
    x_encode_branch(mh, L"", sdest, hstop, pterm, pterm2, pathpfx);
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode_tree.1", e.what(), mh.vflstr()); }
  catch(...) { throw XUExec("encode_tree.2", mh.vflstr()); }
}

unity& paramline::merge(unity& set1, const unity& set2, bool keep_first)
{
  if (!(set1.compatibility() > 0 && set2.compatibility() > 0)) { throw XUExec("merge.4", set1.utype(), set1.compatibility(), set2.utype(), set2.compatibility()); }
  if (set1.isMap())
  {
    _mx& rm1 = *static_cast<_mx*>(set1._m());
    if (set2.isMap())
      { unity::t_map_tu& rm2 = *set2._m(); for(s_long ia = 0; ia < rm2.n(); ++ia) { rm1.__append(false, rm2(ia)->k, rm2(ia)->v, keep_first); } }
    else if (set2.isHash())
      { unity::t_hash& rh2 = *set2._phash(); for(s_long ia = 0; ia < rh2.n(); ++ia) { rm1.__append(false, rh2(ia)->k, rh2(ia)->v, keep_first); } }
    else { throw XUTypeMismatch("merge.1", "set2", set2.utype(), "utMap or utHash"); }
  }
  else if (set1.isHash())
  {
    unity& rh1 = set1;
    if (set2.isMap())
      { unity::t_map_tu& rm2 = *set2._m(); for(s_long ia = 0; ia < rm2.n(); ++ia) { rh1.hash_set(rm2(ia)->k, rm2(ia)->v, keep_first); } }
    else if (set2.isHash())
      { unity::t_hash& rh2 = *set2._phash(); for(s_long ia = 0; ia < rh2.n(); ++ia) { rh1.hash_set(rh2(ia)->k, rh2(ia)->v, keep_first); } }
    else { throw XUTypeMismatch("merge.2", "set2", set2.utype(), "utMap or utHash"); }
  }
  else { throw XUTypeMismatch("merge.3", "set1", set1.utype(), "utMap or utHash"); }
  return set1;
}

unity& paramline::merge(unity& set1, const std::wstring& set2_pl, bool keep_first)
  { return merge(set1, decode(set2_pl, true), keep_first); }

void paramline::_list_mx_set_u ( unity& mdest, s_long fk, _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58, _rcu x59, _rcu x60 )
{
  vec2_t<const unity*> a; mdest.map_clear(true); mdest.mapFlags_set(-1, fk);
  do { if (&x1!=&_0) a.el_append(&x1); else break; if (&x2!=&_0) a.el_append(&x2); else break; if (&x3!=&_0) a.el_append(&x3); else break; if (&x4!=&_0) a.el_append(&x4); else break; if (&x5!=&_0) a.el_append(&x5); else break; if (&x6!=&_0) a.el_append(&x6); else break; if (&x7!=&_0) a.el_append(&x7); else break; if (&x8!=&_0) a.el_append(&x8); else break; if (&x9!=&_0) a.el_append(&x9); else break; if (&x10!=&_0) a.el_append(&x10); else break; if (&x11!=&_0) a.el_append(&x11); else break; if (&x12!=&_0) a.el_append(&x12); else break; if (&x13!=&_0) a.el_append(&x13); else break; if (&x14!=&_0) a.el_append(&x14); else break; if (&x15!=&_0) a.el_append(&x15); else break; if (&x16!=&_0) a.el_append(&x16); else break; if (&x17!=&_0) a.el_append(&x17); else break; if (&x18!=&_0) a.el_append(&x18); else break; if (&x19!=&_0) a.el_append(&x19); else break; if (&x20!=&_0) a.el_append(&x20); else break; if (&x21!=&_0) a.el_append(&x21); else break; if (&x22!=&_0) a.el_append(&x22); else break; if (&x23!=&_0) a.el_append(&x23); else break; if (&x24!=&_0) a.el_append(&x24); else break; if (&x25!=&_0) a.el_append(&x25); else break; if (&x26!=&_0) a.el_append(&x26); else break; if (&x27!=&_0) a.el_append(&x27); else break; if (&x28!=&_0) a.el_append(&x28); else break; if (&x29!=&_0) a.el_append(&x29); else break; if (&x30!=&_0) a.el_append(&x30); else break; if (&x31!=&_0) a.el_append(&x31); else break; if (&x32!=&_0) a.el_append(&x32); else break; if (&x33!=&_0) a.el_append(&x33); else break; if (&x34!=&_0) a.el_append(&x34); else break; if (&x35!=&_0) a.el_append(&x35); else break; if (&x36!=&_0) a.el_append(&x36); else break; if (&x37!=&_0) a.el_append(&x37); else break; if (&x38!=&_0) a.el_append(&x38); else break; if (&x39!=&_0) a.el_append(&x39); else break; if (&x40!=&_0) a.el_append(&x40); else break; if (&x41!=&_0) a.el_append(&x41); else break; if (&x42!=&_0) a.el_append(&x42); else break; if (&x43!=&_0) a.el_append(&x43); else break; if (&x44!=&_0) a.el_append(&x44); else break; if (&x45!=&_0) a.el_append(&x45); else break; if (&x46!=&_0) a.el_append(&x46); else break; if (&x47!=&_0) a.el_append(&x47); else break; if (&x48!=&_0) a.el_append(&x48); else break; if (&x49!=&_0) a.el_append(&x49); else break; if (&x50!=&_0) a.el_append(&x50); else break; if (&x51!=&_0) a.el_append(&x51); else break; if (&x52!=&_0) a.el_append(&x52); else break; if (&x53!=&_0) a.el_append(&x53); else break; if (&x54!=&_0) a.el_append(&x54); else break; if (&x55!=&_0) a.el_append(&x55); else break; if (&x56!=&_0) a.el_append(&x56); else break; if (&x57!=&_0) a.el_append(&x57); else break; if (&x58!=&_0) a.el_append(&x58); else break; if (&x59!=&_0) a.el_append(&x59); else break; if (&x60!=&_0) a.el_append(&x60); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}
void paramline::_list_hx_set_u ( unity& hdest, s_long fk, _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58, _rcu x59, _rcu x60 )
{
  vec2_t<const unity*> a; hdest.hash_clear(true); hdest.hashFlags_set(-1, fk);
  do { if (&x1!=&_0) a.el_append(&x1); else break; if (&x2!=&_0) a.el_append(&x2); else break; if (&x3!=&_0) a.el_append(&x3); else break; if (&x4!=&_0) a.el_append(&x4); else break; if (&x5!=&_0) a.el_append(&x5); else break; if (&x6!=&_0) a.el_append(&x6); else break; if (&x7!=&_0) a.el_append(&x7); else break; if (&x8!=&_0) a.el_append(&x8); else break; if (&x9!=&_0) a.el_append(&x9); else break; if (&x10!=&_0) a.el_append(&x10); else break; if (&x11!=&_0) a.el_append(&x11); else break; if (&x12!=&_0) a.el_append(&x12); else break; if (&x13!=&_0) a.el_append(&x13); else break; if (&x14!=&_0) a.el_append(&x14); else break; if (&x15!=&_0) a.el_append(&x15); else break; if (&x16!=&_0) a.el_append(&x16); else break; if (&x17!=&_0) a.el_append(&x17); else break; if (&x18!=&_0) a.el_append(&x18); else break; if (&x19!=&_0) a.el_append(&x19); else break; if (&x20!=&_0) a.el_append(&x20); else break; if (&x21!=&_0) a.el_append(&x21); else break; if (&x22!=&_0) a.el_append(&x22); else break; if (&x23!=&_0) a.el_append(&x23); else break; if (&x24!=&_0) a.el_append(&x24); else break; if (&x25!=&_0) a.el_append(&x25); else break; if (&x26!=&_0) a.el_append(&x26); else break; if (&x27!=&_0) a.el_append(&x27); else break; if (&x28!=&_0) a.el_append(&x28); else break; if (&x29!=&_0) a.el_append(&x29); else break; if (&x30!=&_0) a.el_append(&x30); else break; if (&x31!=&_0) a.el_append(&x31); else break; if (&x32!=&_0) a.el_append(&x32); else break; if (&x33!=&_0) a.el_append(&x33); else break; if (&x34!=&_0) a.el_append(&x34); else break; if (&x35!=&_0) a.el_append(&x35); else break; if (&x36!=&_0) a.el_append(&x36); else break; if (&x37!=&_0) a.el_append(&x37); else break; if (&x38!=&_0) a.el_append(&x38); else break; if (&x39!=&_0) a.el_append(&x39); else break; if (&x40!=&_0) a.el_append(&x40); else break; if (&x41!=&_0) a.el_append(&x41); else break; if (&x42!=&_0) a.el_append(&x42); else break; if (&x43!=&_0) a.el_append(&x43); else break; if (&x44!=&_0) a.el_append(&x44); else break; if (&x45!=&_0) a.el_append(&x45); else break; if (&x46!=&_0) a.el_append(&x46); else break; if (&x47!=&_0) a.el_append(&x47); else break; if (&x48!=&_0) a.el_append(&x48); else break; if (&x49!=&_0) a.el_append(&x49); else break; if (&x50!=&_0) a.el_append(&x50); else break; if (&x51!=&_0) a.el_append(&x51); else break; if (&x52!=&_0) a.el_append(&x52); else break; if (&x53!=&_0) a.el_append(&x53); else break; if (&x54!=&_0) a.el_append(&x54); else break; if (&x55!=&_0) a.el_append(&x55); else break; if (&x56!=&_0) a.el_append(&x56); else break; if (&x57!=&_0) a.el_append(&x57); else break; if (&x58!=&_0) a.el_append(&x58); else break; if (&x59!=&_0) a.el_append(&x59); else break; if (&x60!=&_0) a.el_append(&x60); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}

void paramline::x_encode1(const unity& x, std::wstring& retval, bool b_name, bool b_ar_elem, bool x_nourecur)
{
    if (b_name && x.utype() != utString) { x_encode1(CStr3(x), retval, true, false); }

    if (x.isArray())
    {
      if (b_ar_elem) { retval = L"\\z"; }
      else
      {
          if (x.arrsz() == 0) { retval = L"\\z"; }
          else
          {
            unity ar; ar.arr_init<utString>(x.arrlb() - 1); ar.arrub_(x.arrub());
            if (x.utype() == utUnityArray)
              { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.ref<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
            else { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.val<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
            retval = join(ar, L"|");
          }
      }
    }
    else
    {
        switch(x.utype())
        {
            case utEmpty: if (b_ar_elem) retval = L""; else retval = L"\\e"; break;
            case utChar: retval = x.val<utChar>() ? L"\\1" : L"\\0"; break;
            case utInt: retval = x.vstr(); break;
            case utDate: retval = x.vstr(); break;
            case utFloat:
            {
              double x1 = x.vfp();

              if (!bmdx_str::conv::is_finite(x1)) { retval = L"\\f"; retval += _fls75(x1).wstr(); break; }

              const int prec_mimi = 11;
              int prec = 13;
              while (prec >= prec_mimi)
              {
                _fls75 z1(x1, prec + 1, prec);
                if (prec == prec_mimi) { retval = z1.wstr(); break; }
                double x2 = str2f(z1.c_str(), 0, true, false); _fls75 z2(x2, prec + 1, prec);
                if (z2 == z1) { retval = z1.wstr(); break; }
                --prec;
              }

              if (retval.find(L".") == nposw) { retval.insert(0,L"\\f"); }
              break;
            }
            case utString:
                {
                  cref_t<std::wstring> r_s = x.cref<utString>();
                  x_repl_e1(r_s.ref(), retval, b_name, b_ar_elem);
                  if (b_name) { return; }
//                  std::wstring s = _trim(retval, L" ");
//                  if (s.length() == 0) { goto lInsertStringPrefix; }
//                  if (wstring_like(s.substr(0,1), L"[0123456789.+-]")) { goto lInsertStringPrefix; }
//                  if (lcase(s) == L"true") { goto lInsertStringPrefix; }
//                  if (lcase(s) == L"false") { goto lInsertStringPrefix; }
                  _t_wz pos1 = retval.find_first_not_of(' '); if (pos1 == nposw) { goto lInsertStringPrefix; }
                  wchar_t c = retval[pos1]; if ((c >= L'0' && c <= L'9') || c == L'.' || c == L'+' || c == L'-') { goto lInsertStringPrefix; }
                  if (_wcsileftincl1(&retval[pos1], L"true") || _wcsileftincl1(&retval[pos1], L"false")) { goto lInsertStringPrefix; }
                  break;
                }
lInsertStringPrefix:
                retval.insert(0, L"\\s");
                break;
            case utObject:
                {
                  const unity* pv = x.objPtr_c<unity>();
                  if (!pv || x_nourecur)
                    { x_encode1(CStr3(x), retval, false, b_ar_elem); }
                  else if (pv->isArray())
                  {
                    if (b_ar_elem) { retval = L"\\z"; }
                    else
                    {
                        const unity& x = *pv;
                        if (x.arrsz()==0) { retval = L"\\z"; }
                        else
                        {
                          unity ar; ar.arr_init<utString>(x.arrlb() - 1); ar.arrub_(x.arrub());
                          if (x.utype() == utUnityArray)
                            { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.ref<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
                          else { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.val<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
                          retval = join(ar, L"|");
                        }
                    }
                  }
                  else { x_encode1(*pv, retval, false, b_ar_elem, true); }
                  break;
                }
            default: // unknown type
                x_encode1(CStr3(x), retval, false, b_ar_elem);
                break;
        }
    }
}

void paramline::x_repl_e1(const std::wstring& s1, std::wstring& s2, bool b_name, bool b_ar_elem)
{
    switch(s1.length())
    {
        case 0: { break; }
        case 1:
        {
          if (b_name) { switch (s1[0]) { case L';': case L' ': case L'=': case L'\\': s2.clear(); s2 += L'\\'; s2 += s1[0]; break; default: s2 = s1; break; } }
            else { switch (s1[0]) { case L';': case L' ': case L'=': case L'|': case L'\\': s2.clear(); s2 += L'\\'; s2 += s1[0]; break; default: s2 = s1; break; } }
          break;
        }
        default:
        {
          _t_wz ps = 0, n = s1.size();
          s2.clear(); if (n > 40) { s2.reserve(n + 10 + (n >> 4)); }
          wchar_t c0 = s1[ps];
          switch (c0)
          {
            case L'\\':
            {
              ++ps; wchar_t c = s1[ps];
              switch (c)
              {
                case L'\\': s2 += L"`b`b`b`b"; ++ps; break;
                case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b`b`b~"; ++ps; } else { s2 += L"`b`b\r"; } break;
                case L' ': case L';': case L'=': case L'~': case L'|': s2 += L"`b`b"; break;
                default: if (b_name) { s2 += L"`b"; } else { s2 += L"`b`b"; } break;
              }
              break;
            }
            case L' ': s2 += L"`b "; ++ps; break;
            case L';': s2 += L"`b`,"; ++ps; break;
            case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b~"; ++ps; } else { s2 += c0; } break;
            case L'=': if (b_name) { s2 += L"`b`e"; } else { s2 += c0; } ++ps; break;
            case L'|': if (b_name) { s2 += c0; } else { s2 += L"`b`v"; } ++ps; break;
            case L'`': s2 += L"`a"; ++ps; break;
            default: s2 += c0; ++ps; break;
          }
          while (ps < n)
          {
            c0 = s1[ps];
            switch (c0)
            {
              case L'\\':
              {
                ++ps;
                if (ps < n)
                {
                  wchar_t c = s1[ps];
                  switch (c)
                  {
                    case L'\\': s2 += L"`b`b`b`b"; ++ps; continue;
                    case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b`b`b~"; ++ps; } else { s2 += L"`b`b\r"; } continue;
                    case L';': case L'=': case L'|': case L'~': case L' ': s2 += L"`b`b"; continue;
                    default: s2 += c0; continue;
                  }
                }
                else { s2 += c0; continue; }
              }
              case L';': s2 += L"`b`,"; ++ps; continue;
              case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b~"; ++ps; } else { s2 += c0; } continue;
              case L'=': if (b_name) { s2 += L"`b`e"; } else { s2 += c0; } ++ps; continue;
              case L'|': if (b_ar_elem) { s2 += L"`b`v"; } else { s2 += c0; } ++ps; continue;
              case L'`': s2 += L"`a"; ++ps; continue;
              default: s2 += c0; ++ps; continue;
            }
          }
          ps = s2.size() - 1; c0 = s2[ps]; if (c0 == L' ' || c0 == L'\\') { s2[ps] = L'\\'; s2 += c0; }
          break;


//          s2 = s1;
//            if (s2.find(L"`") != nposw) s2 = replace(s2, L"`", L"`a");
//            if (s2.find(L"\\\\") != nposw) s2 = replace(s2, L"\\\\", L"`b`b`b`b");
//            if (s2.find(L"\\;") != nposw) s2 = replace(s2, L"\\;", L"`b`b;");
//            if (s2.find(L"\\~") != nposw) s2 = replace(s2, L"\\~", L"`b`b~");
//            if (s2.find(__wBSCRLF()) != nposw) s2 = replace(s2, __wBSCRLF(), L"`b`b`b~");
//            if (s2.find(L"\\=") != nposw) s2 = replace(s2, L"\\=", L"`b`b=");
//            if (s2.find(L"\\ ") != nposw) s2 = replace(s2, L"\\ ", L"`b`b ");
//            if (s2.find(L"\\|") != nposw) s2 = replace(s2, L"\\|", L"`b`b|");

//            if (s2.find(L";") != nposw) s2 = replace(s2, L";", L"`b`,");
//            if (s2.find(wCRLF) != nposw) s2 = replace(s2, wCRLF, L"`b~");

//            if (s2.substr(0,1).find_first_of(L" \\") != nposw) s2.insert(0,L"\\");
//            if (s2.substr(s2.length() - 1,1).find_first_of(L" \\") != nposw) s2.insert(s2.length() - 1,L"\\");

//            if (b_name) { if (s2.find(L"=") != nposw) s2 = replace(s2, L"=", L"`b`e"); }
//            else
//            {
//                if (b_ar_elem) { if (s2.find(L"|") != nposw) s2 = replace(s2, L"|", L"`b`v"); }
//                  else { if (s2.substr(0,1) == L"|") s2.insert(0, L"\\"); }
//            }
        }
    }
}

void paramline::x_replace2a(std::wstring& s, s_long flags)
{
  if (flags & 1)
  {
    _t_wz ps = 0, pd = 0, n = s.size();
    while (ps < n)
    {
      wchar_t c = s[ps++];
      if (c == L'`' && ps < n)
      {
        switch (s[ps])
        {
          case L'b': s[pd++] = L'\\'; ++ps; continue;
          case L'v': s[pd++] = L'|'; ++ps; continue;
          case L'n': s[pd++] = L'\r'; s[pd++] = L'\n'; ++ps; continue;
          case L',': s[pd++] = L';'; ++ps; continue;
          case L's': s[pd++] = L' '; ++ps; continue;
          case L'e': s[pd++] = L'='; ++ps; continue;
          case L'a': s[pd++] = L'`'; ++ps; continue;
          default: break;
        }
      }
      s[pd++] = c;
    }
    s.resize(pd);
  }
}

void paramline::x_replace4(const std::wstring& s1, std::wstring& s2, s_long& flags)
{
  _t_wz n = s1.size(), n2 = 0, nmax = _t_wz(max_swstr() - 2), ps = 0, pd = 0;
  while (ps < n)
  {
    wchar_t c = s1[ps++];
    if (c == L'`') { if (n2 > nmax) { throw XUExec("x_replace4.1"); } n2 += 2; flags |= 1; continue; }
    if (c == L'\\')
    {
      switch (s1[ps])
      {
        case L'\\':  case L';':  case L'~':  case L' ':  case L'=':  case L'|':
          if (n2 > nmax) { throw XUExec("x_replace4.2"); } n2 += 2; ++ps; flags |= 1; continue;
        default: break;
      }
    }
    n2 += 1; if (n2 > nmax) { throw XUExec("x_replace4.3"); }
  }
  if ((flags & 1) == 0) { s2 = s1; return; }
  ps = 0; s2.resize(n2);
  while (ps < n)
  {
    wchar_t c = s1[ps++];
    if (c == L'`') { s2[pd++] = L'`'; s2[pd++] = L'a'; continue; }
    if (c == L'\\')
    {
      switch (s1[ps])
      {
        case L'\\': s2[pd++] = L'`'; s2[pd++] = L'b'; ++ps; continue;
        case L';': s2[pd++] = L'`'; s2[pd++] = L','; ++ps; continue;
        case L'~': s2[pd++] = L'`'; s2[pd++] = L'n'; ++ps; continue;
        case L' ': s2[pd++] = L'`'; s2[pd++] = L's'; ++ps; continue;
        case L'=': s2[pd++] = L'`'; s2[pd++] = L'e'; ++ps; continue;
        case L'|': s2[pd++] = L'`'; s2[pd++] = L'v'; ++ps; continue;
        default: break;
      }
    }
    s2[pd++] = c;
  }
}

  // NOTE v must contain string, already passed
  //   through x_replace4 and _trim(, L" ")
void paramline::x_decode1v(unity& v, bool v_ar_elem, s_long flags)
{
    std::wstring s;
    try
    {
        std::wstring pfx = v.ref<utString>().substr(0,1);
        if (pfx == L"") { v.clear(); }
        else if (pfx == L"\\")
        {
            pfx = v.ref<utString>().substr(0,2);
            if (pfx == L"\\e") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v.clear(); }
            else if (pfx == L"\\s") { v = v.ref<utString>().substr(2); x_replace2a(v.ref<utString>(), flags); }
            else if (pfx == L"\\i") { s = v.ref<utString>().substr(2); if (x_incorrect_integer_value_str(s, false)) { goto lLogicalDecodeError; } v = str2i(s); }
            else if (pfx == L"\\f") { s = v.ref<utString>().substr(2); if (x_incorrect_numeric_value_str(s, true)) { goto lLogicalDecodeError; } v = str2f(s, 0, true, true); }
            else if (pfx == L"\\d") { s = v.ref<utString>().substr(2); try { v = unity(s).val<utDate>(); } catch (...) { goto lLogicalDecodeError; } }
            else if (pfx == L"\\0") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v = false; }
            else if (pfx == L"\\1") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v = true; }
            else if (pfx == L"\\z") { v.arr_init<utUnity>(1); }
            else
            {
                if (v.ref<utString>().length()<=2) { v.clear(); return; }
                v = v.ref<utString>().substr(2);
                s = _trim(v.ref<utString>(), L" ");
                goto lAutoDetectType;
            }
        }
        else if (pfx == L"|")
        {
            if (v_ar_elem) goto lLogicalDecodeError;
            unity ar2 = split(v.ref<utString>(),L"|");
            v.arr_init<utUnity>(1);
            v.arrub_(ar2.arrub());
            for(s_long ind = 1; ind <= ar2.arrub(); ++ind)
            {
                v.arr_set(ind, _trim(ar2.ref<utString>(ind), L" "));
                x_decode1v(v.ref<utUnity>(ind), true, flags);
            }
        }
        else { s = v.ref<utString>(); goto lAutoDetectType; }
        return;

lAutoDetectType:

        if (x_decode1v_auto_date(s, v)) return;
        if (!x_incorrect_integer_value_str(s, false)) { v = str2i(s); return; }
        if (!x_incorrect_numeric_value_str(s, false)) { v = str2f(s); return; }
        if (s == L"true") { v = true; return; }
        if (s == L"false") { v = false; return; }
        x_replace2a(v.ref<utString>(), flags);
        return;

    }
  catch(std::exception& e) { throw XUExec("x_decode1v.1", e.what(), v.vflstr()); }
    catch(...) { throw XUExec("x_decode1v.2", v.vflstr()); }
lLogicalDecodeError:
    throw XUExec("x_decode1v.3", v.vflstr());
    return;
}

bool paramline::x_decode1v_auto_date(const std::wstring& s, unity& retval) throw()
{
  try { _unitydate d; if (!_static_conv::conv_String_Date0(s, d, true)) { return false; } retval = d; return true; }
    catch(...) { return false; }
}

bool paramline::x_incorrect_numeric_value_str(const std::wstring& s, bool b_nans)
{
    if (s.find_first_not_of(L"0123456789.Eeinfa +-") != nposw) return true;
    try { (void)str2f(s, 0., false, b_nans); return false; } catch(...) { return true; }
}
bool paramline::x_incorrect_integer_value_str(const std::wstring& s, bool allow_float_str)
{
  if (allow_float_str)
  {
    if (s.find_first_not_of(L"0123456789.Eeinfa +-") != nposw) { return true; }
    try { (void)str2i(s, 0, false); double y = str2f(s, 0., false, false); return std::floor(y) != y; } catch(...) { return true; }
  }
  else
  {
    if (s.find_first_not_of(L"0123456789 +-") != nposw) { return true; }
    try { (void)str2i(s, 0, false); return false; } catch(...) { return true; }
  }
}

unity paramline::decode(const std::wstring& ssrc, bool useMap, const std::wstring& pterm2) { unity x; decode(ssrc, x, useMap, pterm2); return x; }
unity paramline::decode1v(const std::wstring& ssrc) { unity x; decode1v(ssrc, x); return x; }
unity paramline::decode_tree(const std::wstring& ssrc, s_long flags, const std::wstring& pterm2) { unity x; decode_tree(ssrc, x, flags, pterm2); return x; }
std::wstring paramline::encode(const unity& mhsrc, const std::wstring& pterm) { std::wstring s; encode(mhsrc, s, pterm); return s; }
std::wstring paramline::encode1v(const unity& value) { std::wstring s; encode1v(value, s); return s; }
std::wstring paramline::encode_tree(const unity& mhsrc, const std::wstring& pterm, const std::wstring& pterm2) { std::wstring s; encode_tree(mhsrc, s, pterm, pterm2); return s; }


unity paramline::list_m
(
  _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58,
  _rcu x59, _rcu x60
) { unity u; _list_mx_set_u(u,0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50,x51,x52,x53,x54,x55,x56,x57,x58,x59,x60); return u; }

unity paramline::list_mx
(
  s_long fk, // unity::fkcmp* flags
  _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58,
  _rcu x59, _rcu x60
) { unity u; _list_mx_set_u(u,fk,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50,x51,x52,x53,x54,x55,x56,x57,x58,x59,x60); return u; }

unity paramline::list_h
(
  _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58,
  _rcu x59, _rcu x60
) { unity u; _list_hx_set_u(u,0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50,x51,x52,x53,x54,x55,x56,x57,x58,x59,x60); return u; }

unity paramline::list_hx
(
  s_long fk, // unity::fkcmp* flags
  _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58,
  _rcu x59, _rcu x60
) { unity u; _list_hx_set_u(u,fk,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50,x51,x52,x53,x54,x55,x56,x57,x58,x59,x60); return u; }


} // bmdx













////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS-dependent code. Part 2. Common library.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _bmdxpl_Wnds

#ifdef h_addr
  #undef h_addr
#endif
#ifdef min
  #undef min
#endif
#ifdef max
  #undef max
#endif

namespace bmdx
{
  std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) // n < 0: autodetect length based on null char.
  {
    try {
      #if bmdx_wsbs_force_utf8
        return _wsToBsUtf8(ps, n);
      #else
        if (!ps) { throw XUExec("_wsToBs.3"); }
        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
        if (n > std::numeric_limits<int>::max()) { n = std::numeric_limits<int>::max(); }
        std::string s;
        if (n <= 0) { return s; }
        s_long n2 = WideCharToMultiByte(is_oem ? CP_OEMCP : CP_ACP, 0, ps, int(n), 0, 0, 0, 0);
        if (n2 <= 0) { throw XUExec("_wsToBs.4"); }
        s.resize(_t_sz(n2), ' ');
        s_long n3 = WideCharToMultiByte(is_oem ? CP_OEMCP : CP_ACP, 0, ps, int(n), &s[0], n2, 0, 0);
        if (n3 != n2) { throw XUExec("_wsToBs.5"); }
        return s;
      #endif
    }
    catch (_XUBase&) { throw; }
    catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
    catch (...) { throw XUExec("_wsToBs.2"); }
  }

  std::wstring _bsToWs(const char* ps, meta::s_ll n) // n < 0: autodetect length based on null char.
  {
    try {
      #if bmdx_bsws_force_utf8
        return _bsUtf8ToWs(ps, n);
      #else
        if (!ps) { throw XUExec("_bsToWs.3"); }
        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
        if (n > std::numeric_limits<int>::max()) { n = std::numeric_limits<int>::max(); }
        std::wstring s;
        if (n <= 0) { return s; }
        s_long n2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ps, int(n), 0, 0);
        if (n2 <= 0) { throw XUExec("_bsToWs.4"); }
        s.resize(_t_wz(n2), L' ');
        s_long n3 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ps, int(n), &s[0], n2);
        if (n3 != n2) { throw XUExec("_bsToWs.5"); }
        return s;
      #endif
    }
    catch (_XUBase&) { throw; }
    catch (const std::exception& e) { throw XUExec("_bsToWs.1", e.what()); }
    catch (...) { throw XUExec("_bsToWs.2"); }
  }


    // 1: s1 > s2, 0: s1 == s2, -1: s1 < s2.
  s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case)
  {
    if (ignore_case)
    {
      s_long n = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE|SORT_STRINGSORT, s1.c_str(), int(std::min(_t_wz(std::numeric_limits<int>::max()), s1.length())), s2.c_str(), int(std::min(_t_wz(std::numeric_limits<int>::max()), s2.length())));
      if (n == 0) { throw XUExec("wscompare.1"); }
      return n - 2;
    }
    else { int cmp1 = s1.compare(s2); return cmp1 == 0 ? 0 : (cmp1 > 0 ? 1 : -1); }
  }

    _unitydate d_now(bool allow_fracsec) { SYSTEMTIME t; GetLocalTime(&t); _unitydate x(t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond); if (allow_fracsec) { x.add_seconds(t.wMilliseconds / 1000.); } return x; }
    _unitydate d_nowutc(bool allow_fracsec) { SYSTEMTIME t; GetSystemTime(&t); _unitydate x(t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond); if (allow_fracsec) { x.add_seconds(t.wMilliseconds / 1000.); } return x; }

      // single path separator
    std::string cpathsep() {  return "\\"; }
      // single path separator
    std::wstring wpathsep() {  return L"\\"; }
      // two path separators
    std::string cpathsep2() {  return "\\\\"; }
      // two path separators
    std::wstring wpathsep2() {  return L"\\\\"; }
      // full path and filename of this exe (calculated once)
    std::wstring cmd_myexe()
    {
      static bool initDone(false); static wchar_t buf[500] = { L'\0' };
      if (!initDone) { critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {} GetModuleFileNameW(0,buf,500); initDone = true;  }
      return buf;
    }

    bool __cl2argv(const wchar_t* cl, std::vector<std::wstring>& retv) throw()
    {
      typedef std::wstring::size_type _t_wz;
      // const _t_wz end = std::wstring::npos;
      try {
        retv.clear();
        if (!cl) { return true; }
        std::wstring arg;
        _t_wz pos = 0;

        char b_arg = 0, b_q = 0;
        while (true)
        {
          wchar_t c = cl[pos];
          if (!c) { if (b_arg) { retv.push_back(arg); } break; }
          if (b_q)
          {
            if (c == L'\\')
            {
              _t_wz pos2 = pos + 1;
              wchar_t c2 = 0;
              while (true) { c2 = cl[pos2]; if (c2 == L'\\') { ++pos2; continue; } break; }
              _t_wz n = pos2 - pos;
              if (c2 == L'"')
              {
                ++pos2;
                char b_bsq = char(n & 1);
                n /= 2;
                for (_t_wz i = 0; i < n; ++i) { arg += L'\\'; }
                if (b_bsq) { arg += L'"'; }
                  else { b_q = 0; }
              }
              else { for (_t_wz i = 0; i < n; ++i) { arg += L'\\'; } }
              pos = pos2;
            }
            else if (c == L'"')
            {
              _t_wz pos2 = pos + 1;
              wchar_t c2 = 0;
              while (true) { c2 = cl[pos2]; if (c2 == L'"') { ++pos2; continue; } break; }
              _t_wz n = pos2 - pos;
              char b_keepq = n % 3 == 0;
              n = (n + 1) / 3; for (_t_wz i = 0; i < n; ++i) { arg += L'"'; }
              pos = pos2;
              if (!b_keepq) { b_q = 0; }
              continue;
            }
            else
            {
              arg += c;
              ++pos;
            }
            continue;
          }

          if (c == L' ' || c == L'\0' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\f' || c == L'\v')
          {
            if (b_arg) { retv.push_back(arg); arg.clear(); b_arg = 0; }
          }
          else if (c == L'\\')
          {
            b_arg = 1;

            _t_wz pos2 = pos + 1;
            wchar_t c2 = 0;
            while (true) { c2 = cl[pos2]; if (c2 == L'\\') { ++pos2; continue; } break; }
            _t_wz n = pos2 - pos;
            if (c2 == L'"')
            {
              char b_bsq = char(n & 1);
              n /= 2;
              for (_t_wz i = 0; i < n; ++i) { arg += L'\\'; }
              if (b_bsq) { arg += L'"'; }
                else { b_q = 1; }
              ++pos2;
            }
            else { for (_t_wz i = 0; i < n; ++i) { arg += L'\\'; } }
            pos = pos2;
            continue;
          }
          else if (c == L'"')
          {
            b_arg = 1;

            _t_wz pos2 = pos + 1;
            wchar_t c2 = 0;
            while (true) { c2 = cl[pos2]; if (c2 == L'"') { ++pos2; continue; } break; }
            _t_wz n = pos2 - pos;
            char b_setq = n % 3 == 1;
            n = n / 3; for (_t_wz i = 0; i < n; ++i) { arg += L'"'; }
            if (b_setq) { b_q = 1; }
            pos = pos2;
            continue;
          }
          else
          {
            b_arg = 1;
            arg += c;
          }
          ++pos;
        }

        return true;
      } catch (...) {}
      return false;
    }

      // command line, without executable path/filename
    std::wstring cmd_string(bool b_quoting)
    {
      std::wstring s;
      if (b_quoting)
      {
        std::wstring scl = GetCommandLineW();
        if (scl.size() > 0)
        {
          _t_wz pos2;
          if (scl[0] == L'\"')
          {
            pos2 = scl.find(L"\"", 1);
            if (pos2 == nposw) { pos2 = scl.size(); }
            else
            {
              pos2 = scl.find_first_not_of(L" \t", pos2 + 1);
              if (pos2 == nposw) { pos2 = scl.size(); }
            }
            s = scl.substr(pos2);
          }
          else
          {
            pos2 = scl.find_first_of(L" \t");
            if (pos2 == nposw) { pos2 = scl.size(); }
            else
            {
              pos2 = scl.find_first_not_of(L" \t", pos2);
              if (pos2 == nposw) { pos2 = scl.size(); }
            }
          }
          s = scl.substr(pos2);
        }
      }
      else
      {
        std::vector<std::wstring> argv;
        if (__cl2argv(GetCommandLineW(), argv)) { for (size_t i = 1; i < argv.size(); ++i) { if (i >= 2) { s += L' '; } s += argv[i]; } }
      }
      return s;
    }

      // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
    unity cmd_array()
    {
      std::vector<std::wstring> argv;
      if (__cl2argv(GetCommandLineW(), argv)) { unity a = argv; if (a.arrsz() == 0) { a.arr_append(L""); } return a; }
        else { unity a; a.arr_init<utString>(0); a.arr_append(L""); return a; }
    }

  std::string cmd_arg1(const std::string& s, bool b_shell)
    { return processctl::ff_mc().arg1(s, b_shell); }
  std::string cmd_arg1(const std::wstring& s, bool b_shell)
    { return processctl::ff_mc().arg1(wsToBs(s), b_shell); }

}

#endif // _bmdxpl_Wnds

#ifdef _bmdxpl_Psx

  #ifdef __FreeBSD__
    #include <sys/sysctl.h>
  #endif

  #if __APPLE__ && __MACH__
    #if TARGET_OS_IPHONE
//    NOTE libproc.h may be missing for particular architecture.
//      Patched: libproc is part of libSystem, which is always loaded,
//      so the needed symbol (proc_pidpath) address is got via dlsym.
    #else
      #include <libproc.h>
    #endif
  #endif

static int __buf_argc = 0;
static char __buf_argv[65000] = "_\0\0";

  // NOTE POSIX only. Enables reading argc, argv during static initialization (portable method).
  //    For building shared libraries, the compiler will require additional flag (-fPIC),
  //    but bmdx_static_buf_argv is private because same problem is anyway caused by pthread use.
#define bmdx_static_buf_argv
#ifdef bmdx_static_buf_argv
  static void __read_argv(int argc, char** argv, char** envp)
  {
    __buf_argc = 1; if (!argv) { return; }
    const int n = sizeof(__buf_argv);
    int pos = 0;
    for (int i = 0; i < argc; ++i)
    {
      const char* p = argv[i]; if (!p) { break; }
      while (*p && pos < n - 1) { __buf_argv[pos++] = *p++; } __buf_argv[pos++] = '\0';
      if (i > 0) { __buf_argc += 1; }
      if (pos >= n) { break; }
    }
  }
  #if __APPLE__ && __MACH__
    __attribute__((section("__DATA,__mod_init_func"))) void (* p___read_argv)(int, char*[], char*[]) = &__read_argv;
  #elif defined(__SUNPRO_CC) || defined(__sun)
  #elif defined(__ANDROID__)
  #else
    __attribute__((section(".init_array"))) void (* p___read_argv)(int, char*[], char*[]) = &__read_argv;
  #endif
#endif


#ifdef __ANDROID__
  static int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, size_t n)
  {
    wchar_t c1, c2;
    if (n == 0) { return 0; }
    for (; *s1; s1++, s2++)
    {
      c1 = wchar_t(std::towlower(*s1)); c2 = wchar_t(std::towlower(*s2));
      if (c1 != c2) { return (int)c1 - c2; }
      if (--n == 0) { return 0; }
    }
    return -*s2;
  }
  static int __wctomb(char* ps, wchar_t c) { return int(wcrtomb(ps, c, 0)); }
  static int __mbtowc(wchar_t* pc, const char* ps, size_t n) { return int(mbrtowc(pc, ps, n, 0)); }
#else
  static int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, size_t n) { return wcsncasecmp(s1, s2, n); }
  static int __wctomb(char* ps, wchar_t c) { return wctomb(ps, c); }
  static int __mbtowc(wchar_t* pc, const char* ps, size_t n) { return mbtowc(pc, ps, n); }
#endif

namespace bmdx
{
    namespace { struct _tcnvchk { static bool _test_mbrtowc() { char cc[MB_LEN_MAX+1]; size_t res; wchar_t c; cc[0] = 'A'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'A') { return false; } cc[0] = '1'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'1') { return false; } cc[0] = ';'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L';') { return false; } cc[0] = ' '; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L' ') { return false; } return true; } static bool _test_wcrtomb() { char cc[MB_LEN_MAX+1]; size_t res; res = wcrtomb(cc, L'A', 0); if (res != 1 || cc[0] != 'A') { return false; } res = wcrtomb(cc, L'1', 0); if (res != 1 || cc[0] != '1') { return false; } res = wcrtomb(cc, L';', 0); if (res != 1 || cc[0] != ';') { return false; } res = wcrtomb(cc, L' ', 0); if (res != 1 || cc[0] != ' ') { return false; } return true; } }; }
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) // n < 0: autodetect length based on null char.
    {
      try {
        #if bmdx_wsbs_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif
        if (_mb == 2) { return _wsToBsUtf8(ps, n); }

        if (!ps) { throw XUExec("_wsToBs.3"); }
        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }

        std::string s;
        char cc[MB_LEN_MAX+1];
        setlocale_locked __lock(LC_CTYPE, ""); if (sizeof(__lock)) {}
        if (__wctomb(0, 0)) {}

        if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); }
        if (_mb == 2) { return _wsToBsUtf8(ps, n); }

        meta::s_ll nrsv = n; s.reserve(_t_sz(nrsv));
        for(meta::s_ll pos = 0; pos < n; ++pos)
        {
          if (ps[pos] == L'\0') { s += '\0'; }
            else { int cnt = __wctomb(cc, ps[pos]); if (cnt <= -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
          if ((pos & 0xfff) == 0) { meta::s_ll n2 = meta::s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(_t_sz(nrsv)); } }
        }
        return s;
      }
      catch (_XUBase&) { throw; }
      catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
      catch (...) { throw XUExec("_wsToBs.2"); }
    }

    std::wstring _bsToWs(const char* ps, meta::s_ll n) // n < 0: autodetect length based on null char.
    {
      try {
        #if bmdx_bsws_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif
        if (_mb == 2) { return _bsUtf8ToWs(ps, n); }

        if (!ps) { throw XUExec("_bsToWs.3"); }
        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
        std::wstring s;
        if (n <= 0) { return s; }
        setlocale_locked __lock(LC_CTYPE, ""); if (sizeof(__lock)) {}

        if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_mbrtowc(); }
        if (_mb == 2) { return _bsUtf8ToWs(ps, n); }

        meta::s_ll pos(0), rsvpos(0);
        if (__mbtowc(0, 0, 0)) {}
        meta::s_ll nrsv = n; s.reserve(_t_sz(nrsv));
        while (pos < n)
        {
          if (ps[pos] == '\0') { s += L'\0'; pos += 1; }
          else
          {
            wchar_t c; int cnt = __mbtowc(&c, &ps[pos], size_t(n - pos));
            if (cnt <= -1) { s += L'?'; pos += 1; }
              else if (cnt > 0) { s += c; pos += cnt; }
              else { s += L'\0'; pos += 1; }
          }
          if (pos - rsvpos > 0xfff) { rsvpos = pos; meta::s_ll n2 = meta::s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(_t_sz(nrsv)); } }
        }
        return s;
      }
      catch (_XUBase&) { throw; }
      catch (const std::exception& e) { throw XUExec("_bsToWs.1", e.what()); }
      catch (...) { throw XUExec("_bsToWs.2"); }
    }

      // wide character string comparison
    s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case)
    {
        if (ignore_case)
        {
            _t_wz minlen= s1.length(); if (s2.length() < minlen) { minlen = s2.length(); }
            int cmp1 = __wcsncasecmp(s1.c_str(), s2.c_str(), minlen);
//            int cmp1 = _wcsnicmp(s1.c_str(),s2.c_str(),minlen); // just to test compiling in Windows
            if (cmp1 != 0) { return (cmp1 > 0 ? 1 : -1); }
            if (s1.length() == s2.length()) { return 0; }
            return s1.length() < s2.length() ? -1 : 1;
        }
        else { int cmp1 = s1.compare(s2); return cmp1 == 0 ? 0 : (cmp1 > 0 ? 1 : -1); }
    }

    _unitydate d_now(bool allow_fracsec)
    {
      static s_long init(0); static s_long hdelta(0);
      if (init == 0)
      {
        std::time_t t0 = std::time(0);
        std::tm t1; std::tm* __t1 = std::gmtime(&t0); if (__t1) { t1 = *__t1; }
        std::tm t2; std::tm* __t2 = std::localtime(&t0); if (__t2) { t2 = *__t2; }
        if (__t1 && __t2) { s_long d = t2.tm_hour - t1.tm_hour; if (d > 12) { d -= 24; } hdelta = d; init = 1; }
        else { init = -1; }
      }
      _unitydate x = d_nowutc(allow_fracsec); x.add_seconds(hdelta * 3600.); return x;
    }
    _unitydate d_nowutc(bool allow_fracsec)
    {
      static s_long init(0); static meta::s_ll sec0(0); static meta::s_ll ms0(0);
      meta::s_ll ms1 = meta::s_ll(std::clock()) * 1000 / CLOCKS_PER_SEC;
      meta::s_ll sec1 = meta::s_ll(std::time(0));
      if (init != 0) { if ((ms1 - ms0) - (sec1 - sec0) * 1000 < -1000) { init = 0; } }
      if (init == 0) { sec0 = sec1; ms0 = ms1; init = 1; }
      sec1 = sec0 + (ms1 - ms0) / 1000;
      s_long n = s_long((((sec1 + 43200) / (86400 >> 1)) + (2440587 << 1) + 1) >> 1); s_long h = (sec1 / 3600) % 24; s_long m = (sec1 / 60) % 60; s_long s = sec1 % 60;
      _unitydate x(double(_jds_nhms(n, h, m, s)));
      if (allow_fracsec) { x.add_seconds(double((ms1 - ms0) % 1000) / 1000.); }
      return x;
    }
      // single path separator
    std::string cpathsep() {  return "/" ; }
      // single path separator
    std::wstring wpathsep() {  return L"/"; }
      // two path separators
    std::string cpathsep2() {  return "//"; }
      // two path separators
    std::wstring wpathsep2() {  return L"//"; }
      // full path and filename of this exe (calculated once)
    static const char* _cmd_myexe_buf()
    {
      static bool initDone(false); static char buf[2048] = "_";
      if (!initDone)
      {
        critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
        #if defined(__FreeBSD__)
          int mib[4]; mib[0] = CTL_KERN; mib[1] = KERN_PROC; mib[2] = KERN_PROC_PATHNAME; mib[3] = -1;
          size_t n = sizeof(buf);
          sysctl(mib, 4, buf, &n, 0, 0);
          buf[n] = '\0';
        #elif __APPLE__ && __MACH__
          typedef int (*Fpidpath)(int pid, void* buf, uint32_t bufsize);
          #if TARGET_OS_IPHONE
            Fpidpath f = (Fpidpath)dlsym(RTLD_DEFAULT, "proc_pidpath");
          #else
            Fpidpath f = &proc_pidpath;
          #endif
          if (!f || f(getpid(), buf, sizeof(buf)) <= 0) { buf[0] = '_'; buf[1] = '\0'; }
        #else
          int n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
          if (n <= 0) { n = readlink("/proc/curproc/exe", buf, sizeof(buf) - 1); }
          if (n <= 0) { n = readlink("/proc/curproc/file", buf, sizeof(buf) - 1); }
          if (n <= 0) { n = readlink("/proc/self/path/a.out", buf, sizeof(buf) - 1); } // __sun
          if (n > 0) { buf[n] = '\0'; }
            else { buf[0] = '_'; buf[1] = '\0'; }
        #endif
        initDone = true;
      }
      return buf;
    }
    std::wstring cmd_myexe()
    {
      static bool initDone(false); static std::wstring spath;
      if (!initDone) { critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {} spath = bsToWs(_cmd_myexe_buf()); initDone = true; }
      return spath;
    }

    #if defined(__SUNPRO_CC) || defined(__sun)
      static std::string __sol_arg1_subs(const std::string& s)
      {
        typedef std::string::size_type _t_sz;
        const _t_sz end = std::string::npos;

        _t_sz pos = s.find('\\');
        if (pos == end) { return s; }
        std::string s2 = s.substr(0, pos);
        while (pos < s.length())
        {
          char c = s[pos];
          if (c == '\\') { ++pos; if (pos >= s.length()) { break; } c = s[pos]; if (c == 'n') { s2 += '\n'; } else if (c == '\\') { s2 += '\\'; } else if (c == 'r') { s2 += '\r'; } else if (c == 'f') { s2 += '\f'; } else if (c == 'v') { s2 += '\v'; } else { s2 += c; } }
            else { s2 += c; }
          ++pos;
        }
        return s2;
      }

      static std::vector<std::string> __sol_argv()
      {
        std::vector<std::string> argv;
        std::string s;
        int n2;
        char buf[100];
        n2 = std::sprintf(buf, "pargs %i", int(getpid()));
        if (n2 <= 0) { return argv; }

        FILE* f = popen(buf, "r");
        if (!f) { return argv; }
        size_t n;
        try { while (!feof(f) && !ferror(f)) { n = fread(buf, 1, sizeof(buf), f); s.append(buf, n); } } catch (...) {}
        pclose(f);
        if (s.empty()) { return argv; }


        int iarg = 0;
        typedef std::string::size_type _t_sz;
        const _t_sz end = std::string::npos;
        _t_sz pos = 0;
        do
        {
          n2 = std::sprintf(buf, "argv[%i]: ", iarg);
          if (n2 <= 0) { break; }
          _t_sz pos2 = s.find(buf, pos);
          if (pos2 == end) { break; }
          pos = pos2 + _t_sz(n2);
          pos2 = s.find('\n', pos);
          if (pos2 == end) { pos2 = s.length(); }
          argv.push_back(__sol_arg1_subs(s.substr(pos, pos2 - pos)));
          pos = pos2 + 1;
          iarg += 1;
        } while (1);
        return argv;
      }


        // command line, without executable path/filename
      std::wstring cmd_string(bool b_quoting)
      {
        static bool initDone(false); static std::wstring s;
        if (!initDone)
        {
          critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
          std::vector<std::string> args = __sol_argv();
          s.clear(); std::string s2;
          for (unsigned int i = 1; i < args.size(); ++i)
          {
            if (i > 1) { s += L' '; }
            s2 = args[i];
            if (b_quoting && (s2.empty() || s2.find_first_of(" \t\r\n\f\v;'\"\\$[](){}^?*") != nposc))
            {
              if (s2.find("'") != nposc) { s2 = replace(s2, "'", "'\\''"); }
              s2 = "'" + s2 + "'";
            }
            s += bsToWs(s2);
          }
          initDone = true;
        }
        return s;
      }

        // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
      unity cmd_array()
      {
        static bool initDone(false); static std::vector<std::string> args;
        if (!initDone)
        {
          critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
          args = __sol_argv();
          if (args.size() == 0) { args.push_back(_cmd_myexe_buf()); }
          initDone = true;
        }
        unity a; a.arr_init<utString>(0);
        for (unsigned int i = 0; i < args.size(); ++i)
        {
          a.arr_append(args[i]);
        }
        return a;
      }
    #else
      static void __buf_argv_ensure()
      {
        static bool initDone(false);
        if (!initDone)
        {
          critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
          if (__buf_argc <= 0)
          {
            __buf_argc = 0;
            while (true) // once
            {
              const int n0 = sizeof(__buf_argv) - 1;
              int fd;
              fd = open("/proc/self/cmdline", O_RDONLY);
              if (fd == 0) { fd = open("/proc/curproc/cmdline", O_RDONLY); }
              if (fd == 0) { break; }
              int n = (int)read(fd, __buf_argv, n0);
              close(fd);
              if (n <= 0) { break; }
              if (__buf_argv[n - 1] != '\0') { __buf_argv[n] = '\0'; ++n; }
              int pos = 0; while (pos < n) { if (__buf_argv[pos++] == '\0') { __buf_argc += 1; } }
              break;
            }
          }
          if (__buf_argc == 0)
          {
            const char* p1 = _cmd_myexe_buf();
            size_t n = 0;
            while (*p1 && n < sizeof(__buf_argv) - 1) { __buf_argv[n] = *p1; ++p1; ++n; }
            __buf_argv[n] = '\0';
            __buf_argc = 1;
          }
          initDone = true;
        }
      }

        // command line, without executable path/filename
      std::wstring cmd_string(bool b_quoting)
      {
        static bool initDone(false); static std::wstring s;
        if (!initDone)
        {
          critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
          __buf_argv_ensure();
          s.clear(); std::string s2; int pos = 0; int j = 0;
          while (j < __buf_argc)
          {
            char c = __buf_argv[pos++];
            if (c == '\0')
            {
              if (j > 1) { s += L' '; }
              if (j > 0)
              {
                if (b_quoting && (s2.empty() || s2.find_first_of(" \t\r\n\f\v;'\"\\$[](){}^?*") != nposc))
                {
                  if (s2.find("'") != nposc) { s2 = replace(s2, "'", "'\\''"); }
                  s2 = "'" + s2 + "'";
                }
                s += bsToWs(s2);
              }
              s2.clear();
              j += 1;
            }
            else { s2 += c;  }
          }
          initDone = true;
        }
        return s;
      }

        // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
      unity cmd_array()
      {
        unity a; a.arr_init<utString>(0);
        __buf_argv_ensure();
        std::string s2; int pos = 0; int j = 0;
        while (j < __buf_argc)
        {
          char c = __buf_argv[pos++];
          if (c == '\0') { a.arr_append(bsToWs(s2)); s2.clear(); j += 1; }
          else { s2 += c;  }
        }
        return a;
      }
    #endif
}

#endif // _bmdxpl_Psx


//==============================
// OS or library-dependent code template
//==============================

#ifdef _bmdxpl_OS_NAME_HERE
namespace bmdx
{
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) { } // n < 0: autodetect length based on null char.
    std::wstring _bsToWs(const char* ps, meta::s_ll n) { } // n < 0: autodetect length based on null char.
    s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case) { }
    _unitydate d_now(bool allow_fracsec) { }
    _unitydate d_nowutc(bool allow_fracsec) { }
    std::string cpathsep() { }
    std::wstring wpathsep() { }
    std::string cpathsep2() { }
    std::wstring wpathsep2() { }
    std::wstring cmd_myexe() { }
    std::wstring cmd_string(bool b_quoting) { }
    unity cmd_array() { }
}
#endif














////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS-dependent code. Part 3. File utils.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _bmdxpl_Wnds

#ifndef F_OK
#define F_OK  0 /* Check for file existence */
#endif


namespace bmdx
{
  bool file_utils::is_full_path(const std::wstring& pathstr) const { return pathstr.length() >= 2 && (wstring_like(pathstr.substr(0, 2), L"[a-zA-Z]:") || pathstr.substr(0, 2) == wpathsep2()); } // <Wnds> </Wnds>
  bool file_utils::is_full_path(const std::string& pathstr) const { return pathstr.length() >= 2 && (wstring_like(bsToWs(pathstr.substr(0, 2)), L"[a-zA-Z]:") || pathstr.substr(0, 2) == cpathsep2()); } // <Wnds> </Wnds>

  bool file_utils::is_valid_path(const std::wstring& s) const
  {
    // <Wnds>
    if (s.find_first_of(L"*/?\"<>|\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37") != nposw) { return false; }
    if (s.length() >= 2 && s[1] == L':') { if (s.find(L":", 2) != nposw) { return false; } }
      else { if (s.find(L":") != nposw) { return false; } }
    return true;
    // </Wnds>
  }

  bool file_utils::is_valid_path(const std::string& s) const
  {
    // <Wnds>
    if (s.find_first_of("*/?\"<>|\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37") != nposc) { return false; }
    if (s.length() >= 1 && s[0] == ':') { return false; }
    if (s.length() >= 2 && s[1] == ':') { if (s.find(":", 2) != nposc) { return false; } }
      else { if (s.find(":") != nposc) { return false; } }
    return true;
    // </Wnds>
  }

  bool file_utils::xmk_subdir(const std::string& sPath, int level)
  {
    std::string wp;

    if (level == 0)
    {
      wp = trim(file_utils().join_path(file_utils().complete_path(sPath, pdCurDir)), cpathsep(), false, true);
      if (wp.length() == 0) { return false; }
    }
    else
    {
      wp = trim(sPath, cpathsep(), false, true);
      if (wp.length() == 0) { return true; }
    }
    if (file_utils().is_ex_dir(wp)) { return true; }
    if (!xmk_subdir(file_utils().strip_path(wp), level+1)) { return false; }

    // <Wnds>
    return 0 == __bmdx_std_mkdir(wp.c_str());
    // </Wnds>
  }

  std::wstring file_utils::complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
    { return bsToWs(complete_path(wsToBs(sPath), pd, wsToBs(sUserDefDir))); }
  std::string file_utils::complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
      if (is_full_path(sPath)) { return sPath; }
      if (xHasCurDirShortCut(sPath)) { char buf[2048]; return join_path(__bmdx_std_getcwd(buf, sizeof(buf)), sPath.substr(1)); }
      std::string s;
      switch(pd)
      {
          case pdCurDir: { char buf[2048]; s = join_path(__bmdx_std_getcwd(buf,sizeof(buf)), sPath); break; }
          case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), sPath); break; }
          case pdTempDir:
          {
              // <Wnds>
              std::string sv;
              if (__bmdx_std_getenv("TEMP", sv) || __bmdx_std_getenv("TMP", sv))
                { s = join_path(sv, sPath); }
              else { throw XUExec("file_utils::complete_path.1"); }
              // </Wnds>
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
              break;
          }
          case pdDoNotChange: { s = sPath; break; }
          default: //pdUserDefinedDir
              s = join_path(sUserDefDir,sPath);
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
      }
      return s;
  }

} // bmdx ns
#endif // _bmdxpl_Wnds


#ifdef _bmdxpl_Psx

namespace bmdx
{
  bool file_utils::is_full_path(const std::wstring& pathstr) const {  return pathstr.length() && pathstr.substr(0, 1) == wpathsep(); } // <Psx> </Psx>
  bool file_utils::is_full_path(const std::string& pathstr) const { return pathstr.length() && pathstr.substr(0, 1) == cpathsep(); } // <Psx> </Psx>

  bool file_utils::is_valid_path(const std::wstring& s) const { if (s.find_first_of(L"\0") != nposw) return false; return true; } // <Psx> </Psx>
  bool file_utils::is_valid_path(const std::string& s) const { if (s.find_first_of("\0") != nposc) return false; return true; } // <Psx> </Psx>

  bool file_utils::xmk_subdir(const std::string& sPath, int level)
  {
    std::string wp;

    if (level == 0)
    {
      wp = trim(file_utils().join_path(file_utils().complete_path(sPath, pdCurDir)), cpathsep(), false, true);
      if (wp.length() == 0) { return false; }
    }
    else
    {
      wp = trim(sPath, cpathsep(), false, true);
      if (wp.length() == 0) { return true; }
    }
    if (file_utils().is_ex_dir(wp)) { return true; }
    if (!file_utils().xmk_subdir(file_utils().strip_path(wp), level+1)) { return false; }

    // <Psx>
    return 0==__bmdx_std_mkdir(wp.c_str(), 0777);
    // </Psx>
  }

  std::wstring file_utils::complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
  { return bsToWs(complete_path(wsToBs(sPath), pd, wsToBs(sUserDefDir))); }
  std::string file_utils::complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
    if (is_full_path(sPath)) { return sPath; }
    if (xHasCurDirShortCut(sPath)) { char buf[2048]; return join_path(__bmdx_std_getcwd(buf, sizeof(buf)), sPath.substr(1)); }
    std::string s;
    switch(pd)
    {
        case pdCurDir: { char buf[2048]; s = join_path(__bmdx_std_getcwd(buf,sizeof(buf)), sPath); break; }
          case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), sPath); break; }
          case pdTempDir:
              // <Psx>
              s = join_path(getenv("TMPDIR"), sPath);
              // </Psx>
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
              break;
          case pdDoNotChange: { s = sPath; break; }
          default: //pdUserDefinedDir
              s = join_path(sUserDefDir,sPath);
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
      }
      return s;
  }
}
#endif // _bmdxpl_Psx



//=============================================================================================================================
// OS-independent part
//=============================================================================================================================

namespace bmdx
{
std::wstring file_utils::strip_path(const std::wstring& pathstr) const { _t_wz pos = pathstr.rfind(wpathsep()); return pos == nposw ? pathstr : pathstr.substr(0, pos); }
std::string file_utils::strip_path(const std::string& pathstr) const { _t_sz pos = pathstr.rfind(cpathsep()); return pos == nposc ? pathstr : pathstr.substr(0, pos); }

std::wstring file_utils::strip_filename(const std::wstring& pathstr) const { _t_wz pos = pathstr.rfind(wpathsep()); return pos == nposw ? pathstr : pathstr.substr(pos+1); }
std::string file_utils::strip_filename(const std::string& pathstr) const { _t_sz pos = pathstr.rfind(cpathsep()); return pos == nposc ? pathstr : pathstr.substr(pos+1); }

std::wstring file_utils::remove_ext(const std::wstring& pathstr, const std::wstring& extsep) const
{
  _t_wz end = nposw, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(wpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1); }
  return pathstr;
}
std::string file_utils::remove_ext(const std::string& pathstr, const std::string& extsep) const
{
  _t_sz end = nposc, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(cpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1); }
  return pathstr;
}

std::wstring file_utils::add_to_name(const std::wstring& pathstr, const std::wstring& s_add, const std::wstring& extsep) const
{
  _t_wz end = nposw, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(wpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1) + s_add + pathstr.substr(pos1); }
  return pathstr + s_add;
}
std::string file_utils::add_to_name(const std::string& pathstr, const std::string& s_add, const std::string& extsep) const
{
  _t_sz end = nposc, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(cpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1) + s_add + pathstr.substr(pos1); }
  return pathstr + s_add;
}

std::wstring file_utils::replace_filename(const std::wstring& fnp, const std::wstring& fn) const { return join_path(strip_path(fnp), fn); }
std::string file_utils::replace_filename(const std::string& fnp, const std::string& fn) const { return join_path(strip_path(fnp), fn); }

std::wstring file_utils::join_path(const std::wstring& pathstr, const std::wstring& pathstr2) const
{
    std::wstring s0, s; s = pathstr;
    if (s.substr(0, ps2len) == wpathsep2()) { s0 = wpathsep2(); s = s.substr(ps2len); }
    s += wpathsep(); s += pathstr2;
    while(s.find(wpathsep2()) != nposw) { s = replace(s, wpathsep2(), wpathsep()); }
    if (s_long(s.length()) >= pslen && s.substr(s.length() - pslen) == wpathsep()) { s.erase(s.length() - pslen); }
    #ifdef _bmdxpl_Wnds
      if (s.substr(0, pslen) == wpathsep()) { s.erase(0, pslen); }
    #endif
    s0 += s;
    return s0;
}
std::wstring file_utils::join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3) const { return join_path(join_path(ps1, ps2), ps3); }
std::wstring file_utils::join_path(const std::wstring& ps1, const std::wstring& ps2, const std::wstring& ps3, const std::wstring& ps4) const { return join_path(join_path(join_path(ps1, ps2), ps3), ps4); }

std::string file_utils::join_path(const std::string& pathstr, const std::string& pathstr2) const
{
    std::string s0, s; s = pathstr;
    if (s.substr(0, ps2len) == cpathsep2()) { s0 = cpathsep2(); s = s.substr(ps2len); }
    s += cpathsep(); s += pathstr2;
    while(s.find(cpathsep2()) != nposc) { s = replace(s, cpathsep2(), cpathsep()); }
    if (s_long(s.length()) >= pslen && s.substr(s.length() - pslen) == cpathsep()) { s.erase(s.length() - pslen); }
    #ifdef _bmdxpl_Wnds
      if (s.substr(0, pslen) == cpathsep()) { s.erase(0, pslen); }
    #endif
    s0 += s;
    return s0;
}
std::string file_utils::join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3) const { return join_path(join_path(ps1, ps2), ps3); }
std::string file_utils::join_path(const std::string& ps1, const std::string& ps2, const std::string& ps3, const std::string& ps4) const { return join_path(join_path(join_path(ps1, ps2), ps3), ps4); }

bool file_utils::has_rightmost_patshep(const std::wstring& s) const { return s_long(s.length())>=pslen && s.substr(s.length()-pslen)==wpathsep(); }
bool file_utils::has_rightmost_patshep(const std::string& s) const { return s_long(s.length())>=pslen && s.substr(s.length()-pslen)==cpathsep(); }

bool file_utils::is_ex_file(const std::wstring& sPath) const { return is_ex_file(wsToBs(sPath)); }
bool file_utils::is_ex_file(const std::string& sPath) const
{
    if (has_rightmost_patshep(sPath))return false;
    std::string p = sPath;
    if ( 0 == __bmdx_std_access(p.c_str(),F_OK) ) // file exists...
    {
        p += cpathsep();
        return 0 != __bmdx_std_access(p.c_str(),F_OK); // ...and the directory does not
    }
    else { return false; }
}

bool file_utils::is_ex_dir(const std::wstring& sPath) const { return is_ex_dir(wsToBs(sPath)); }
bool file_utils::is_ex_dir(const std::string& sPath) const
{
    std::string p = sPath;
    if (!has_rightmost_patshep(sPath)) { p += cpathsep(); }
    return 0 == __bmdx_std_access(p.c_str(), F_OK); // directory exists
}

std::wstring file_utils::expand_env_nr(const std::wstring& s) const
{
  std::wstring s2; _t_wz pos0(0), pos2;
  do {
    pos2 = s.find('%', pos0);
    if (pos2 != nposw)
    {
      s2 += s.substr(pos0, pos2 - pos0);
      pos0 = pos2 + 1;
      pos2 = s.find('%', pos0);
      if (pos2 == nposw) { pos0 -= 1; s2 += s.substr(pos0, s.length() - pos0); break; }
        else if (pos2 > pos0) { std::string sv; if (__bmdx_std_getenv(wsToBs(s.substr(pos0, pos2 - pos0)).c_str(), sv)) { s2 += bsToWs(sv); } pos0 = pos2 + 1; }
        else { pos0 = pos2 + 1; }
    }
    else { s2 += s.substr(pos0, s.length() - pos0); break; }
  } while (pos0 < s.length());
  return s2;
}

std::string file_utils::expand_env_nr(const std::string& s) const
{
  std::string s2; _t_wz pos0(0), pos2;
  do {
    pos2 = s.find('%', pos0);
    if (pos2 != nposc)
    {
      s2 += s.substr(pos0, pos2 - pos0);
      pos0 = pos2 + 1;
      pos2 = s.find('%', pos0);
      if (pos2 == nposc) { pos0 -= 1; s2 += s.substr(pos0, s.length() - pos0); break; }
        else if (pos2 > pos0) { std::string sv; if (__bmdx_std_getenv(s.substr(pos0, pos2 - pos0).c_str(), sv)) { s2 += sv; } pos0 = pos2 + 1; }
        else { pos0 = pos2 + 1; }
    }
    else { s2 += s.substr(pos0, s.length() - pos0); break; }
  } while (pos0 < s.length());
  return s2;
}

bool file_utils::mk_subdir(const std::wstring& sPath) const { return xmk_subdir(wsToBs(sPath),0); }
bool file_utils::mk_subdir(const std::string& sPath) const { return xmk_subdir(sPath,0); }
unity file_utils::load_string(const std::string& format_string, const std::string& sPath, EFileUtilsPredefinedDir pd, unity& ret_s) const
    { return load_string(format_string, bsToWs(sPath), pd, ret_s); }
unity file_utils::load_string(const std::string& format_string, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, unity& ret_s) const
{
  typedef unsigned char u_char;

  const int ReadBufSize = 2048;
  enum EEncodings { local8bit, lsb8bit, utf16le, utf16be };
  bool is_binary(false); bool is_text(false); bool is_local8bit(false); bool is_utf16le(false); bool is_utf16be(false); bool is_lsb8bit(false);
  std::vector<int> encs;
  std::vector<std::string> args = splitToVector(format_string, " ");
  for (unsigned int i = 0; i < args.size(); ++i)
  {
    if (!is_binary && args[i] == "binary") is_binary = true;
    if (!is_text && args[i] == "text") is_text = true;
    if (!is_local8bit && args[i] == "local8bit") { is_local8bit = true; encs.push_back(local8bit); }
    if (!is_utf16le && args[i] == "utf16le") { is_utf16le = true; encs.push_back(utf16le); }
    if (!is_utf16be && args[i] == "utf16be") { is_utf16le = true; encs.push_back(utf16be); }
    if (!is_lsb8bit && args[i] == "lsb8bit") { is_lsb8bit = true; encs.push_back(lsb8bit); }
  }
  bool is_successful = false; std::wstring s0;

  while(true) // once
  {
    if (is_binary == is_text) break;
    if (encs.size() == 0) break;
    std::wstring sPath = complete_path(sPath0,pd);
    if (!is_ex_file(sPath)) { break; }
    std::fstream f(wsToBs(sPath).c_str(), std::ios_base::openmode(std::ios_base::binary|std::ios_base::in));
    if (!f.is_open()) { break; }
    f >> std::noskipws;
    char fileBuf[ReadBufSize];
    f.rdbuf()->pubsetbuf(fileBuf, ReadBufSize);
      // load whole file
    std::string s(static_cast<std::stringstream const&>(std::stringstream() << f.rdbuf()).str());
      // Test for allowed encodings.
    unsigned int pos = 0;
    int ind_enc = -1;
    int ind_enc_local8bit = -1;
    for (unsigned int i = 0; i < encs.size(); ++i)
    {
      switch (encs[i])
      {
          // local8bit, if allowed and all others do not match, is used as the default encoding
        case local8bit: { ind_enc_local8bit = i;  continue; }
        case utf16le: { if (s.size() >= 2 && u_char(s[0]) == 0xff && u_char(s[1]) == 0xfe) { if (is_text) { pos = 2; } ind_enc = i; goto lExitFor1; } break; }
        case utf16be: { if (s.size() >= 2 && u_char(s[0]) == 0xfe && u_char(s[1]) == 0xff) { if (is_text) { pos = 2; } ind_enc = i; goto lExitFor1; } break; }
        case lsb8bit: { ind_enc = i; goto lExitFor1; break; }
        default: { break; }
      }
    }
lExitFor1:
    if (ind_enc < 0) { if (ind_enc_local8bit >= 0) { ind_enc = ind_enc_local8bit; } else { ind_enc = 0; } }

    // Recode file data into the required encoding.
    switch (encs[ind_enc])
    {
    case local8bit:
      if (is_binary) { s0 = bsToWs(s); }
      else // is_text
      {
        std::string s2;
        while (pos < s.size())
        {
          char c = s[pos];
          if (c == '\n') { s2 += "\r\n"; if (pos < s.size() - 1 && s[pos + 1] == '\r') { ++pos; } }
            else if (c == '\r') { s2 += "\r\n"; if (pos < s.size() - 1 && s[pos + 1] == '\n') { ++pos; } }
            else { s2 += c; }
          ++pos;
        }
        s0 = bsToWs(s2);
      }
      is_successful = true;
      break;

    case utf16le:
      while (pos < s.size() - 1)
      {
        wchar_t c = wchar_t(u_char(s[pos])) | (wchar_t(u_char(s[pos + 1])) << 8);
        if (is_text && c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\r' && s[pos + 3] == '\0') { pos += 2; } }
          else if (is_text && c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\n' && s[pos + 3] == '\0') { pos += 2; } }
          else { s0 += c; }
        pos += 2;
      }
      is_successful = true;
      break;

    case utf16be:
      while (pos < s.size() - 1)
      {
        wchar_t c = (wchar_t(u_char(s[pos])) << 8) | wchar_t(u_char(s[pos + 1]));
        if (is_text && c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\0' && s[pos + 3] == '\r') { pos += 2; } }
          else if (is_text && c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\0' && s[pos + 3] == '\n') { pos += 2; } }
          else { s0 += c; }
        pos += 2;
      }
      is_successful = true;
      break;

    case lsb8bit:
      if (is_binary) { while (pos < s.size()) { s0 += wchar_t(u_char(s[pos])); ++pos; } }
      else // is_text
      {
        while (pos < s.size())
        {
          wchar_t c = wchar_t(u_char(s[pos]));
          if (c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 1 && s.at(pos + 1) == '\r') { ++pos; } }
            else if (c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 1 && s.at(pos + 1) == '\n') { ++pos; } }
            else { s0 += c; }
          ++pos;
        }
      }
      is_successful = true;
      break;

    default:
      break;
    }

    break;
  } // while (true)

  // Create the resulting string.

  if (is_successful) { if (&ret_s!=&unity::_0nc) { ret_s = s0; return true; } else { return s0; } }
    else { if (&ret_s!=&unity::_0nc) { ret_s.clear(); return false; } else { return unity(); } }
}

bool file_utils::save_string(const std::string& format_string, const std::wstring& str, const std::string& sTargetFilePath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const throw()
  { return save_string(format_string, str, bsToWs(sTargetFilePath), pd, sUserDefDir); }
bool file_utils::save_string(const std::string& format_string, const std::wstring& s0, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const throw()
{
  try {
    typedef unsigned char u_char;

    bool is_binary(false);
    bool is_text(false);
    bool is_truncate(false);
    bool is_append(false);
    bool is_local8bit(false);
    bool is_lsb8bit(false);
    bool is_utf16le(false);
    bool is_utf16be(false);

    std::vector<std::string> args = splitToVector(format_string, " ");
    for (unsigned int i = 0; i < args.size(); ++i)
    {
      if (!is_binary && args[i] == "binary") is_binary = true;
      if (!is_text && args[i] == "text") is_text = true;
      if (!is_truncate && args[i] == "truncate") is_truncate = true;
      if (!is_append && args[i] == "append") is_append = true;
      if (!is_local8bit && args[i] == "local8bit") is_local8bit = true;
      if (!is_utf16le && args[i] == "utf16le") is_utf16le = true;
      if (!is_utf16be && args[i] == "utf16be") is_utf16be = true;
      if (!is_lsb8bit && args[i] == "lsb8bit") is_lsb8bit = true;
    }

    if (is_binary == is_text) return false;
    if (is_truncate == is_append) return false;
    if (int(is_local8bit) + int(is_lsb8bit) + int(is_utf16le) + int(is_utf16be) != 1) return false;

    const std::wstring* ps2 = &s0;

    std::fstream f(wsToBs(complete_path(sPath0, pd, sUserDefDir)).c_str(), std::ios_base::openmode(std::ios_base::binary|(is_append?std::ios_base::app|std::ios_base::ate:std::ios_base::trunc)|std::ios_base::out));
    if (!f.is_open()) { return false; }
    f << std::noskipws;

    if (is_utf16le)
    {
      if (is_text && (is_truncate || f.tellp() == std::ostream::pos_type(0))) { f << '\xff' << '\xfe'; }
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        wchar_t wc = ps2->at(i);
        if (is_text && wc == L'\n') { f << '\r' << '\0' << '\n' << '\0'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && wc == L'\r') { f << '\r' << '\0' << '\n' << '\0'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << u_char(wc) << u_char(wc >> 8); }
      }
    }
    else if (is_utf16be)
    {
      if (is_text && (is_truncate || f.tellp() == std::ostream::pos_type(0))) { f << '\xfe' << '\xff'; }
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        wchar_t wc = ps2->at(i);
        if (is_text && wc == L'\n') { f << '\0' << '\r' << '\0' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && wc == L'\r') { f << '\0' << '\r' << '\0' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << u_char(wc >> 8) << u_char(wc); }
      }
    }
    else if (is_lsb8bit)
    {
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        wchar_t wc = ps2->at(i);
        if (is_text && wc == L'\n') { f << '\r' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && wc == L'\r') { f << '\r' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << char(wc); }
      }
    }
    else // local8bit
    {
      const std::string s2(wsToBs(*ps2));
      for (_t_sz i = 0; i < s2.size(); ++i)
      {
        char c = s2[i];
        if (is_text && c == '\n') { f << '\r' << '\n'; if (i < s2.size() - 1 && s2[i + 1] == '\r') { ++i; } }
          else if (is_text && c == '\r') { f << '\r' << '\n'; if (i < s2.size() - 1 && s2[i + 1] == '\n') { ++i; } }
          else { f << c; }
      }
    }

    bool good=f.good(); f.close(); return good;
  } catch (...) { return false; }
}

int file_utils::load_cstr(const std::wstring& fnp, std::string& dest) throw() { return file_io::load_bytes(wsToBs(fnp), dest); }
int file_utils::load_cstr(const std::string& fnp, std::string& dest) throw() { return file_io::load_bytes(fnp, dest); }
int file_utils::load_cstr(const char* fnp, std::string& dest) throw() { return file_io::load_bytes(fnp, dest); }

  // Saves bytes from src to the given file.
  //    b_append == false truncates the file before writing, if it exists.
  //    if n == 0, pdata may be 0.
  // 1 - success.
  // 0 - failed to create file (or open the existing file for writing).
  // -1 - data size too large, or memory alloc. error, or wrong arguments.
  // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
int file_utils::save_cstr(const std::wstring& fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(wsToBs(fnp), src, b_append); }
int file_utils::save_cstr(const std::string& fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(fnp, src, b_append); }
int file_utils::save_cstr(const char* fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(fnp, src, b_append); }
int file_utils::save_cstr(const char* fnp, const char* pdata, meta::s_ll n, bool b_append) throw() { return file_io::save_bytes(fnp, pdata, n, b_append); }

bool file_utils::xHasCurDirShortCut(const std::wstring& sPath) { return sPath==L"." || sPath.substr(0,1+pslen)==L"."+wpathsep(); }
bool file_utils::xHasCurDirShortCut(const std::string& sPath) { return sPath=="." || sPath.substr(0,1+pslen)=="."+cpathsep(); }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary module handle.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bmdx
{

s_long unity::compatibility() const throw() { return _compatibility(); }
s_long unity::sig_struct() throw()
{
  unity x;
  return ((sizeof(&x) - 1) << 28)
   ^ ((sizeof(unity) - 4 * sizeof(&x)) << 22)
   ^ s_long(((const char*)&x.pmsm - (const char*)&x) << 18)
   ^ s_long(((const char*)&x.ut - (const char*)&x.pmsm) << 12)
   ^ s_long(((const char*)&x._data.p1 - (const char*)&x.ut) << 6)
   ^ s_long(((const char*)&x._data.p2 - (const char*)&x._data.p1));
}

#ifdef _bmdxpl_Wnds
  struct unity::_mod_exhandle
  {
    typedef s_long (*prequest)(s_long, s_long, const unity*, unity*);
    typedef unity_common::__Psm (*pget_modsm)();

    unity_common::__Psm pmsm_rl; prequest prq; union { meta::s_ll __2; HMODULE handle; }; pget_modsm pgms; struct _flags { char b_au; char htype; }; union { _flags f; void* __3; };

    _mod_exhandle(bool b_autounload = true) throw() { __2 = 0; __3 = 0; pmsm_rl = 0; handle = 0; prq = 0; pgms = 0; f.htype = 0; f.b_au = b_autounload; }

    bool b_h() const throw() { return f.htype > 0; }
    bool b_msm() const throw() { return b_h() && pmsm_rl; }
    bool b_rq() const throw() { return b_h() && prq; }
    bool b_gms() const throw() { return b_h() && pgms; }
    bool b_mainexe() const throw() { return f.htype == 2; }

    #if bmdx_part_dllmgmt
      ~_mod_exhandle() throw() { if (f.b_au) { mod_unload(); } }

        // 1 - success, 0 - already loaded (may be name or other), -1 - failure.
      s_long mod_load(const char* name, s_long flags) throw()
      {
        if (f.htype > 0) { return 0; } if (!name) { return -1; }

        HMODULE h = 0;
        if (*name == '\0') { h = GetModuleHandleA(0); f.htype = 2; }
          else { h = LoadLibraryA(name); if (!h) { return -1; } f.htype = 1; }
        handle = h;

        prq = (prequest)sym("bmdx_mod_request");
        pgms = (pget_modsm)sym("__bmdx_ls_modsm");
        return 1;
      }
      void mod_unload() throw()
      {
        if (f.htype == 1) { FreeLibrary(handle); }
        if (f.htype > 0) { handle = 0; f.htype = 0; }
        prq = 0;
        pgms = 0;
      }
      void* sym(const char* name) const throw() { if (!name || !*name) { return 0; } if (f.htype <= 0) { return 0; } return (void*)GetProcAddress(handle, name); }
    #endif
  private: _mod_exhandle(const _mod_exhandle&); void operator=(const _mod_exhandle&);
  };
#endif

#ifdef _bmdxpl_Psx
  struct unity::_mod_exhandle
  {
    typedef s_long (*prequest)(s_long, s_long, const unity*, unity*);
    typedef unity_common::__Psm (*pget_modsm)();

    unity_common::__Psm pmsm_rl; prequest prq; union { meta::s_ll __2; void* handle; }; pget_modsm pgms; struct _flags { char b_au; char htype; }; union { _flags f; void* __3; };

    bool b_h() const throw() { return f.htype > 0; }
    bool b_msm() const throw() { return b_h() && pmsm_rl; }
    bool b_rq() const throw() { return b_h() && prq; }
    bool b_gms() const throw() { return b_h() && pgms; }
    bool b_mainexe() const throw() { return f.htype == 2; }

    _mod_exhandle(bool b_autounload = true) throw() { __2 = 0; __3 = 0; pmsm_rl = 0; handle = 0; prq = 0; pgms = 0; f.htype = 0; f.b_au = b_autounload; }


    #if bmdx_part_dllmgmt
      ~_mod_exhandle() throw() { if (f.b_au) { mod_unload(); } }

        // 1 - success, 0 - already loaded (may be name or other), -1 - failure.
      s_long mod_load(const char* name, s_long flags) throw()
      {
        if (f.htype > 0) { return 0; } if (!name) { return -1; }

        int mode = RTLD_NOW|RTLD_LOCAL;
        #ifndef __ANDROID__
          mode |= RTLD_NOLOAD;
        #endif
        #ifdef RTLD_DEEPBIND
          if ((flags & 3) == 1) { mode |= RTLD_DEEPBIND; } // set on
            else if ((flags & 3) == 0) { } // leave deepb. off by dflt.
            else // enable only for g++ (detect based on __GNUC__)
            {
              #if defined(__clang__)
              #elif defined(__GNUC__)
                  mode |= RTLD_DEEPBIND;
              #else
              #endif
            }
        #endif
        #ifdef RTLD_FIRST
          mode |= RTLD_FIRST;
        #endif
        #ifdef RTLD_NODELETE
          if (!f.b_au) { mode |= RTLD_NODELETE; }
        #endif
        void* h = 0;
        if (*name == '\0')
        {
          #ifdef RTLD_MAIN_ONLY
            h = RTLD_MAIN_ONLY;
          #else
            h = dlopen(0, mode);
            if (!h) { return -1; }
          #endif
          f.htype = 2;
        }
        else
        {
          h = dlopen(name, mode);
          #ifndef __ANDROID__
            if (!h) { mode &= ~RTLD_NOLOAD; h = dlopen(name, mode); }
          #endif
          if (!h) { return -1; }
          f.htype = 1;
        }
        handle = h;

        prq = (prequest)sym("bmdx_mod_request");
        pgms = (pget_modsm)sym("__bmdx_ls_modsm");
        return 1;
      }
      void mod_unload() throw()
      {
        #ifdef RTLD_MAIN_ONLY
        #else
          if (f.htype == 2) { dlclose(handle); }
        #endif
        if (f.htype == 1) { dlclose(handle); }
        if (f.htype > 0) { handle = 0; f.htype = 0; }
        prq = 0; pgms = 0;
      }
      void* sym(const char* name) const throw() { if (!name || !*name) { return 0; } if (f.htype <= 0) { return 0; } return (void*)dlsym(handle, name); }
    #endif
  private: _mod_exhandle(const _mod_exhandle&); void operator=(const _mod_exhandle&);
  };
#endif
  // _stg_mh(), ~_stg_mh() may be called only in root loader.
unity::mod_handle::_stg_mh::_stg_mh() { for (int i = 0; i < n; ++i) { _d[i] = 0; } new (&_d[0]) _mod_exhandle(); }
unity::mod_handle::_stg_mh::~_stg_mh() { (*this)().~_mod_exhandle(); }
unity::_mod_exhandle& unity::mod_handle::_stg_mh::operator()() const { return *(_mod_exhandle*)&_d[0]; }
}

namespace yk_c
{
  using namespace bmdx;
  template<> struct vecm::spec<unity::_mod_exhandle> { typedef config_t<unity::_mod_exhandle, 1, 4, 0> config; };
}

namespace bmdx
{

struct t_hmods
{
  hashx<unity::mod_handle::t_nativehandle, cref_t<unity::mod_handle::_stg_mh> > hmods;
  hashx<void*, unity::mod_handle::t_nativehandle> hmsm; // ls_modsm --> mod. handle; may be shorter than hmods, because sometimes ls_modsm is not accessible

  typedef hashx<unity::mod_handle::t_nativehandle, cref_t<unity::mod_handle::_stg_mh> > t_hmods1;
  typedef hashx<void*, unity::mod_handle::t_nativehandle> t_hmsm;
};
typedef svf_m_t<t_hmods, unity, 0, void, 100, 60000> t_hmods_svf;

bool unity::mod_handle::is_valid() const throw()
  { return _rex && _rex.ref()().b_h(); } // implementation-independent
unity::mod_handle::operator bool() const throw()
  { return _rex && _rex.ref()().b_h(); } // implementation-independent
bool unity::mod_handle::is_mainexe() const throw()
  { return _rex && _rex.ref()().b_mainexe(); } // implementation-independent
unity::mod_handle::t_nativehandle unity::mod_handle::native() const throw()
  { if (!_rex) { return 0; } return _rex.ref()().handle; } // implementation-independent
bool unity::mod_handle::operator==(const mod_handle& x) const throw()
  { if (!_rex) { return !x._rex; } if (!x._rex) { return false; } return _rex.ref()().handle == x._rex.ref()().handle; } // implementation-independent
bool unity::mod_handle::operator!=(const mod_handle& x) const throw()
  { return !operator==(x); }

unity::mod_handle::mod_handle() throw()
  : _rex()
    { __rsv1 = 0; }
unity::mod_handle::~mod_handle() throw()
  { clear(); }
unity::mod_handle::mod_handle(const mod_handle& h1) throw()
  : _rex()
{
  __rsv1 = 0;
  if (!h1._rex) { return; } // OK
  unity::_mod_exhandle& h = h1._rex.ref()();
  cv_ff::cv_rootldr::PFcopy_handle f_ch = (cv_ff::cv_rootldr::PFcopy_handle)h.pmsm_rl(unity_common::msm_rl_cp_handle);
    if (!f_ch) { return; } // this is not expected to occur (see ls_modsm).
  try { f_ch(&h1, this); } catch (...) {}
}
void unity::mod_handle::operator=(const mod_handle& h1) throw()
  { mod_handle h3(h1); this->clear(); this->swap(h3); }
void unity::mod_handle::swap(mod_handle& h1) throw()
  { bmdx_str::words::swap_bytes(*this, h1); }
void unity::mod_handle::clear() throw()
{
  if (!_rex) { return; }
  do
  {
    unity::_mod_exhandle& h = _rex.ref()();
    if (!h.pmsm_rl) { break; } // this is not expected to occur because pmsm_rl is set at once after _rex object creation (see Finit_handle).
    cv_ff::cv_rootldr::PFdestroy_handle f_dh = (cv_ff::cv_rootldr::PFdestroy_handle)h.pmsm_rl(unity_common::msm_rl_des_handle);
      if (!f_dh) { break; } // this is not expected to occur (see ls_modsm).
    try { f_dh(this); } catch (...) {}
    return;
  } while (0);
  new (&_rex) cref_t<_stg_mh>();
}

void* unity::mod_handle::sym(const char* name) const throw()
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.pmsm_rl) { return 0; }
  cv_ff::cv_rootldr::PFsym f1 = (cv_ff::cv_rootldr::PFsym)h.pmsm_rl(unity_common::msm_rl_sym);
    if (!f1) { return 0; } // this is not expected to occur (see ls_modsm).
  try { return f1(this, name); } catch (...) {}
  return 0;
}

s_long unity::mod_handle::request(const unity& para, unity& retval) const throw()
{
  if (!_rex) { return -4; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.pmsm_rl) { return -2; }
  cv_ff::cv_rootldr::PFrequest f1 = (cv_ff::cv_rootldr::PFrequest)h.pmsm_rl(unity_common::msm_rl_rq);
    if (!f1) { return -2; } // this is not expected to occur (see ls_modsm).
  try { return f1(this, &para, &retval); } catch (...) {}
  return -2;
}
unity_common::f_ls_modsm unity::mod_handle::_pmsm() const throw()
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.b_gms()) { return 0; }
  return h.pgms();
}
unity_common::f_ls_modsm unity::mod_handle::_pmsm_rl() const throw()
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.b_msm()) { return 0; }
  return h.pmsm_rl;
}
unity::mod_handle unity::mod_handle::hself(bool b_autounload) throw()
{
  unity::mod_handle h;
  unity_common::f_ls_modsm pmsm = unity_common::pls_modsm();
  #if bmdx_part_dllmgmt
    unity_common::f_ls_modsm offer = pmsm;
  #else
    unity_common::f_ls_modsm offer = 0;
  #endif
  unity_common::f_ls_modsm pmsm_rl = 0;
  cv_ff::cv_rootldr::Frootldr(offer, &pmsm_rl);
  if (pmsm_rl)
  {
    cv_ff::cv_rootldr::PFinit_by_pmsm f_ibp = (cv_ff::cv_rootldr::PFinit_by_pmsm)pmsm_rl(unity_common::msm_rl_init_by_pmsm);
    if (f_ibp) { try { f_ibp(pmsm, b_autounload, &h); } catch (...) {} }

      // If handle is not found, but the current module is root loader, check if it's main executable
      //  (can be done because exe's handle can be got without knowing its name).
      // ~!!! Later, this can be replaced by system-dependent getting shared object handle.
    if (!h && pmsm_rl == pmsm)
    {
      mod_handle he;
      cv_ff::cv_rootldr::PFinit_handle f_ih = (cv_ff::cv_rootldr::PFinit_handle)pmsm_rl(unity_common::msm_rl_init_handle);
      if (f_ih)
      {
        try { f_ih(&he, "", 1, (bmdx_mod_load_def_flags & 0)); } catch (...) {}
        if (he._pmsm() == pmsm) { h = he; }
      }
    }
  }
  return h;
}

}

#if bmdx_part_dllmgmt || bmdx_expose_ls_modsm
  extern "C" __BMDX_DLLEXPORT bmdx::unity_common::__Psm __bmdx_ls_modsm() { return bmdx::unity_common::ls_modsm; }
#endif

namespace bmdx
{
#if bmdx_part_dllmgmt
  unity::mod_handle unity::mod(const char* name, bool b_au, s_long flags) throw()
  {
    unity::mod_handle mh;

    unity_common::__Psm pmsm_rl = 0;
    cv_ff::cv_rootldr::Frootldr(unity_common::ls_modsm, &pmsm_rl); // init own rootldr pointer if necessary
      if (!pmsm_rl) { return mh; }

    cv_ff::cv_rootldr::PFinit_handle f_ih = (cv_ff::cv_rootldr::PFinit_handle)pmsm_rl(unity_common::msm_rl_init_handle);
      if (!f_ih) { return mh; }

    try { f_ih(&mh, name, b_au ? 1 : 0, flags); } catch (...) {}
    return mh;
  }

    // NOTE Finit_handle assumes that it's called in root loader module (see also unity::mod).
    // flags0: 0x1 - remember to autounload when ref. cnt. == 0 (if possible).
  s_long cv_ff::cv_rootldr::Finit_handle(unity::mod_handle* pmh, const char* name, s_long flags0, s_long flags)
  {
    if (!pmh) { return 0; }
    if (pmh->_rex) { return -1; }

    cref_t<unity::mod_handle::_stg_mh> rd;
      if (!rd.create0(true)) { return -2; }
    rd.ref()().pmsm_rl = unity_common::ls_modsm;

    t_hmods_svf::L __lock; if (!__lock.is_locked()) { return -3; }
        t_hmods_svf rsth; if (rsth.b_noinit()) { rsth.init0(1); }
        t_hmods* phm = rsth.px(); if (!phm) { return -4; }

    char b_au = flags0 & 1;
    rd.ref()().f.b_au = b_au;
    if (rd.ref()().mod_load(name, flags) < 0) { return -5; } // +1 load if successful

    unity::_mod_exhandle::pget_modsm pgms = rd.ref()().pgms;
    unity_common::f_ls_modsm pmsm = pgms ? pgms() : 0;
    if (pmsm)
    {
      PFrootldr f_rl = (PFrootldr)pgms()(unity_common::msm_rootldr);
      if (f_rl)
      {
        s_long f = 0;
        try { f = f_rl(unity_common::ls_modsm, 0); } catch (...) {}
        if (!f) { return -7; } // this is not expected to occur, because Frootldr does not fail on offer != 0

          //~!!! This disables autounloading shared library - rootldr on last ref. des., for case when it has loaded itself at least once.
          //  May be replaced with check "if root loader still contains any other libraries handles, which cannot be immediately released".
        if ((f & 4) && *name != '\0') { b_au = 0; }
      }
    }

    unity::mod_handle::t_nativehandle handle = rd.ref()().handle;
    const t_hmods::t_hmods1::entry* e(0);
    int res = phm->hmods.insert(handle, &e);
    if (res == 1) { e->v = rd; }
      else if (res == 0) { rd = e->v; } // rd is overwritten, so 1 unload is called (this does not close DLL, because e->v exists)
      else { return -6; }

    if (pmsm)
    {
      try { phm->hmsm[(void*)pmsm] = handle; } catch (...) {} // if this fails, it's not critical (no need to unload the library)
    }

    if (!b_au) { e->v.ref()().f.b_au = 0; }
    pmh->_rex = rd; // does not fail here
    return 1;
  }
    // NOTE Fdestroy_handle assumes that it's called in root loader module (see also unity::mod).
  s_long cv_ff::cv_rootldr::Fdestroy_handle(unity::mod_handle* pmh)
  {
    if (!(pmh && pmh->_rex)) { return 0; }
    unity::_mod_exhandle& h = pmh->_rex.ref()();
    if (!h.b_h() || !h.f.b_au) { pmh->_rex.clear(); return 1; }
    t_hmods_svf::L __lock;
      if (!__lock.is_locked()) { pmh->_rex.clear(); return -1; }
      t_hmods_svf rsth; if (rsth.b_noinit()) { rsth.init0(1); }
      t_hmods* phm = rsth.px(); if (!phm) { pmh->_rex.clear(); return -2; }
    const t_hmods::t_hmods1::entry* e = phm->hmods.find(h.handle); if (!e) { pmh->_rex.clear(); return -3; }
    if (e->v.ref()().f.b_au && e->v.n_refs() <= 2)
    {
      phm->hmods.remove_e(e);
      if (h.b_gms()) { phm->hmsm.remove((void*)h.pgms()); }
    }
    pmh->_rex.clear();
    return 1;
  }
  s_long cv_ff::cv_rootldr::Fcopy_handle(const unity::mod_handle* pmhsrc, unity::mod_handle* pmhdest)
  {
    if (!(pmhsrc && pmhdest)) { return 0; }
    if (pmhsrc == pmhdest) { return 1; }
    pmhdest->_rex = pmhsrc->_rex; // does not fail
    pmhdest->__rsv1 = 0;
    return 1;
  }
  s_long cv_ff::cv_rootldr::Finit_by_pmsm(unity_common::f_ls_modsm pmsm, s_long b_au, unity::mod_handle* pmhdest)
  {
    if (!pmhdest) { return -1; }
    if (!pmsm) { pmhdest->clear(); return 0; }
    try {
      do { // once
        t_hmods_svf::L __lock; if (!__lock.is_locked()) { break; }
          t_hmods_svf rsth; if (rsth.b_noinit()) { rsth.init0(1); }
          t_hmods* phm = rsth.px(); if (!phm) { break; }
        const t_hmods::t_hmsm::entry* e = phm->hmsm.find((void*)pmsm);
          if (!e) { break; }
        const t_hmods::t_hmods1::entry* e1 = phm->hmods.find(e->v);
          if (!e1) { break; }
        pmhdest->_rex = e1->v; // does not fail
        pmhdest->__rsv1 = 0;
        if (!b_au) { pmhdest->_rex.ref()().f.b_au = 0; }
        return 1;
      } while (0);
    } catch (...) {}
    return -2;
  }
  void* cv_ff::cv_rootldr::Fsym(const unity::mod_handle* pmh, const char* name)
  {
    if (!pmh) { return 0; }
    if (!pmh->_rex) { return 0; }
    if (!pmh->_rex.ref()().b_h()) { return 0; }
    return pmh->_rex.ref()().sym(name);
  }
  s_long cv_ff::cv_rootldr::Frequest(const unity::mod_handle* pmh, const unity* para, unity* retval)
  {
    if (!pmh) { return -4; }
    if (!pmh->_rex) { return -4; }
    if (!pmh->_rex.ref()().b_h()) { return -4; }

    t_hmods_svf::L __lock; if (!__lock.is_locked()) { return -2; }
        t_hmods_svf rsth; if (rsth.b_noinit()) { rsth.init0(1); }
        t_hmods* phm = rsth.px(); if (!phm) { return -2; }
    const t_hmods::t_hmods1::entry* e = phm->hmods.find(pmh->_rex.ref()().handle); if (!e) { return -2; }

    unity::_mod_exhandle& h = e->v.ref()(); if (!h.b_rq()) { return -3; }

    s_long res = -5;
    try { res = h.prq(unity::sig_struct(), 1, para, retval); } catch (...) {}
    return res;
  }
#else
  s_long cv_ff::cv_rootldr::Finit_handle(unity::mod_handle* pmh, const char* name, s_long flags0, s_long flags) { return -10; }
  s_long cv_ff::cv_rootldr::Fdestroy_handle(unity::mod_handle* pmh) { return -10; }
  s_long cv_ff::cv_rootldr::Fcopy_handle(const unity::mod_handle* pmhsrc, unity::mod_handle* pmhdest) { return -10; }
  s_long cv_ff::cv_rootldr::Finit_by_pmsm(unity_common::f_ls_modsm pmsm, s_long b_au, unity::mod_handle* pmhdest) { return -10; }
  void* cv_ff::cv_rootldr::Fsym(const unity::mod_handle* pmh, const char* name) { return 0; }
  s_long cv_ff::cv_rootldr::Frequest(const unity::mod_handle* pmh, const unity* para, unity* retval) { return -2; }
#endif // bmdx_part_dllmgmt
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common objects storage.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bmdx
{
  #define __PCOS_VER_MIN 1
  #define __PCOS_VER_CURRENT 1
  #define __PCOS_VER_MAX_COMPAT 100
  namespace
  {
    typedef hashx<unity, cref_t<unity> > t_hpcos;
    typedef svf_m_t<t_hpcos, cv_ff_pcos>::L t_lock_hpcos;
    svf_m_t<t_hpcos, cv_ff_pcos> hpcos;
    struct pcos_aux_v1
    {
      s_long sig; s_long ver;
      unity::mod_handle hmc; // handle of the module that created the contained object of cref_t of which *this is aux. object

      pcos_aux_v1(const unity::mod_handle& hmc_) : hmc(hmc_) { sig = sig_pcos_aux(); ver = __PCOS_VER_CURRENT; }

      bool b_ver_match() { return sig == sig_pcos_aux() && ver >= __PCOS_VER_MIN && ver <= __PCOS_VER_MAX_COMPAT; }
      static s_long sig_pcos_aux() { return yk_c::bytes::uind_t<char, 2019, 07, 21, 18, 8 + sizeof(unity::mod_handle)>::x; }
    };
      // The lock data for all new pcos objects, created in this module.
    critsec_t<unity>::csdata* pcsd_pcos_obj() { return (critsec_t<unity>::csdata*)critsec_t<t_hpcos>::ff_mc().pdefcsd(); }

  }

    // 1 - success, *pd contains valid object, created in the root loader of *ph,
    //  0 - success, *pd is cleared in-place (*ph is null),
    //  -2 - failure, *pd is cleared in-place (*ph is non-null but not valid, or object creation failed),
    //  -4 - failure, nothing modified (bad args).
  s_long cv_ff_pcos::Fcons(cref_t<unity::_pcos_d_base>* pd, const unity::mod_handle* ph)
  {
    if (!(pd && ph)) { return -4; }
    pd->clear();
    t_msm pmsm_rl = ph->_pmsm_rl();
      if (!pmsm_rl) { return -2; }
    typedef unity::pcos::f_pcos_cons tf; tf f = (tf)pmsm_rl(t_uc::msm_pcos_cons_rl);
      if (!f) { return -2; }
    s_long res = -2;
    try { res = f(pd, ph); } catch (...) {}
    return res;
  }
  s_long cv_ff_pcos::Fcons_rl(cref_t<unity::_pcos_d_base>* pd, const unity::mod_handle* ph)
  {
    if (!(pd && ph)) { return -4; }
    pd->clear();
    if (!ph->_pmsm()) { return -2; }
    cref_t<unity::_pcos_d_base> r = cref_t<unity::_pcos_d_base>::iref2<>::create0(1);
      if (!r) { return -2; }
    r->hhost = *ph;
    bmdx_str::words::swap_bytes(r, *pd);
    return 1;
  }
  s_long cv_ff_pcos::Foppar(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, cref_t<unity>* prv)
  {
    if (!(pd && pk && prv)) { return -2; }
    try {
      cref_t<unity> z; // will be deleted later than __lock
      t_lock_hpcos __lock; if (sizeof(__lock)) {}
      t_hpcos& h = hpcos.rxai0();
      const t_hpcos::entry* e = h.find(*pk);
      if (e && e->v) { z = e->v; }
      bmdx_str::words::swap_bytes(z, *prv);
      return *prv ? 1 : 0;
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fsetref(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, cref_t<unity>* pxr)
  {
    if (!(pd && pk && pxr)) { return -2; }
    s_long res1 = Fcheckref(pd, pxr);
    if (res1 < 0) { return res1; }
    try {
      cref_t<unity>& xr = *pxr;
      if (xr)
      {
        t_lock_hpcos __lock; if (sizeof(__lock)) {}
        t_hpcos& h = hpcos.rxai0();
        const t_hpcos::entry* e = 0; s_long res = h.insert(*pk, &e);
        if (res < 0) { return -2; }
        bmdx_str::words::swap_bytes(xr, e->v); // z will be deleted later than __lock
        return res == 1 ? 2 : 1;
      }
      else
      {
        cref_t<unity> z0; // will be deleted later than __lock
        t_lock_hpcos __lock; if (sizeof(__lock)) {}
        t_hpcos& h = hpcos.rxai0();
        const t_hpcos::entry* e = h.find(*pk);
        if (!e) { return 0; }
        bmdx_str::words::swap_bytes(z0, e->v);
        s_long res = h.remove_e(e);
        if (res == 1) { return 1; }
        bmdx_str::words::swap_bytes(z0, e->v);
        return -2;
      }
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fcheckref(const cref_t<unity::_pcos_d_base>* pd, cref_t<unity>* pxr)
  {
    if (!(pd && pxr)) { return -2; }
    cref_t<unity>& xr = *pxr;
    if (!xr) { return 0; }

    pcos_aux_v1* pa = (pcos_aux_v1*)xr.paux(); unity* pu = xr._pnonc_u(); s_long fl = s_long(xr.flags());
    if (!(pa && pu && (fl & iref2_flags::use_hst) && (fl & iref2_flags::use_aux) && (fl & iref2_flags::use_critsec) && ((char*)pu-(char*)pa) >= s_ll(sizeof(pcos_aux_v1)) && pa->b_ver_match())) { return -1; }
    if (!(pa->hmc && pa->hmc._pmsm() == pu->_pmsm())) { return -1; }
    if (pu->isObject())
    {
      unity::o_api a(pu); if (!a.prc) { return -1; }
      unity::o_api::critsec_rc(a.prc);
      t_msm pmsmo = (t_msm)a.prc->rc_posm(t_uc::osm_modsm);
      if (pmsmo != pa->hmc._pmsm()) { return -1; }
    }

    return 1;
  }
  s_long cv_ff_pcos::Fsetmv(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, unity* pxm, const unity::mod_handle* phx)
  {
    if (!(pd && pk && pxm && phx)) { return -2; }
    // form handle
    // get handle pmsm
    // check if *pxm and its object (if utObject) is same pmsm as handle
    // create a non-local empty object o1
    // create an empty unity object z
    //
    // set lock on storage
    // find entry, create if not exists
    // swap o1 with entry
    // swap z with entry
    // swap *pxm entry
    // ret. success
    try {
      const unity::mod_handle& hxm = *phx ? *phx : (*pd)->hhost;
      t_msm pmsm = hxm._pmsm();
      t_msm pmsm_rl = hxm._pmsm_rl();
        if (!(pmsm && pmsm_rl)) { return -2; }
        if (pxm->_pmsm() != pmsm) { return -1; }
        if (pxm->isObject())
        {
          unity::o_api a(pxm); if (!a.prc) { return -2; }
          unity::o_api::critsec_rc(a.prc);
          t_msm pmsmo = (t_msm)a.prc->rc_posm(t_uc::osm_modsm);
          if (pmsmo != pmsm) { return -1; }
        }
      typedef f_makepcoscref fmcr; fmcr f = (fmcr)pmsm_rl(t_uc::msm_pcos_makecref);
        if (!f) { return -2; }
      cref_t<unity> o1;
        if (f(&hxm, &o1) < 1) { return -2; }
      unity* pu1 = o1._pnonc_u();
        if (!pu1) { return -2; } // pu1 is not expected to be 0 here
      unity z; // has local pmsm == that of the storage
      t_lock_hpcos __lock; if (sizeof(__lock)) {}
      t_hpcos& h = hpcos.rxai0();
      const t_hpcos::entry* e = 0;
      s_long res = h.insert(*pk, &e);
        if (res < 0) { return -2; }
      bmdx_str::words::swap_bytes(o1, e->v);
      bmdx_str::words::swap_bytes(z, *pu1);
      bmdx_str::words::swap_bytes(*pxm, *pu1);
      return res == 1 ? 2 : 1;
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fsetcp(const cref_t<unity::_pcos_d_base>* pd, const unity* pk, const unity* pxc, const unity::mod_handle* phx)
  {
    if (!(pd && pk && pxc && phx)) { return -2; }
    // form handle
    // get handle pmsm
    // check if *pxc's object (if utObject) is same pmsm as handle
    // create a non-local copy of *pxc: o1
    //
    // set lock on storage
    // find entry, create if not exists
    // swap o1 with entry
    // ret. success
    try {
      const unity::mod_handle& hxc = *phx ? *phx : (*pd)->hhost;
      t_msm pmsm = hxc._pmsm();
      t_msm pmsm_rl = hxc._pmsm_rl();
        if (!(pmsm && pmsm_rl)) { return -2; }
        if (pxc->isObject())
        {
          unity::o_api a(pxc); if (!a.prc) { return -2; }
          unity::o_api::critsec_rc(a.prc);
          t_msm pmsmo = (t_msm)a.prc->rc_posm(t_uc::osm_modsm);
          if (pmsmo != pmsm) { return -1; }
        }
      typedef f_makepcoscrefcp fmcr; fmcr f = (fmcr)pmsm_rl(t_uc::msm_pcos_makecrefcp);
        if (!f) { return -2; }
      cref_t<unity> o1;
        if (f(&hxc, pxc, &o1) < 1) { return -2; }
      t_lock_hpcos __lock; if (sizeof(__lock)) {}
      t_hpcos& h = hpcos.rxai0();
      const t_hpcos::entry* e = 0;
      s_long res = h.insert(*pk, &e);
        if (res < 0) { return -2; }
      bmdx_str::words::swap_bytes(o1, e->v);
      return res == 1 ? 2 : 1;
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fclearstg(const cref_t<unity::_pcos_d_base>* pd)
  {
    if (!pd) { return -2; }
    try {
      t_lock_hpcos __lock; if (sizeof(__lock)) {}
      if (!hpcos.b_valid()) { return 0; }
      hpcos.rx().hashx_clear();
      return 1;
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fmakemv(const cref_t<unity::_pcos_d_base>* pd, unity* pxm, const unity::mod_handle* phx, cref_t<unity>* prv)
  {
    if (!(pd && pxm && phx && prv)) { return -2; }
    try {
      const unity::mod_handle& hxm = *phx ? *phx : (*pd)->hhost;
      t_msm pmsm = hxm._pmsm();
      t_msm pmsm_rl = hxm._pmsm_rl();
        if (!(pmsm && pmsm_rl)) { return -2; }
        if (pxm->_pmsm() != pmsm) { return -1; }
        if (pxm->isObject())
        {
          unity::o_api a(pxm); if (!a.prc) { return -2; }
          unity::o_api::critsec_rc(a.prc);
          t_msm pmsmo = (t_msm)a.prc->rc_posm(t_uc::osm_modsm);
          if (pmsmo != pmsm) { return -1; }
        }
      typedef f_makepcoscref fmcr; fmcr f = (fmcr)pmsm_rl(t_uc::msm_pcos_makecref);
        if (!f) { return -2; }
      cref_t<unity> o1;
        if (f(&hxm, &o1) < 1) { return -2; }
      unity* pu1 = o1._pnonc_u();
        if (!pu1) { return -2; } // pu1 is not expected to be 0 here
      unity z; // has local pmsm == that of the storage
      bmdx_str::words::swap_bytes(z, *pxm);
      bmdx_str::words::swap_bytes(z, *pu1);
      *prv = o1;
      return 1;
    } catch (...) {}
    return -2;
  }
  s_long cv_ff_pcos::Fmakecp(const cref_t<unity::_pcos_d_base>* pd, const unity* pxc, const unity::mod_handle* phx, cref_t<unity>* prv)
  {
    if (!(pd && pxc && phx && prv)) { return -2; }
    try {
      const unity::mod_handle& hxc = *phx ? *phx : (*pd)->hhost;
      t_msm pmsm = hxc._pmsm();
      t_msm pmsm_rl = hxc._pmsm_rl();
        if (!(pmsm && pmsm_rl)) { return -2; }
        if (pxc->isObject())
        {
          unity::o_api a(pxc); if (!a.prc) { return -2; }
          unity::o_api::critsec_rc(a.prc);
          t_msm pmsmo = (t_msm)a.prc->rc_posm(t_uc::osm_modsm);
          if (pmsmo != pmsm) { return -1; }
        }
      typedef f_makepcoscrefcp fmcr; fmcr f = (fmcr)pmsm_rl(t_uc::msm_pcos_makecrefcp);
        if (!f) { return -2; }
      cref_t<unity> o1;
        if (f(&hxc, pxc, &o1) < 1) { return -2; }
      *prv = o1;
      return 1;
    } catch (...) {}
    return -2;
  }

    // Creates cref_t<unity> in *prv, such that all is true:
    //    1) cref_t belongs to the current module.
    //    2) cref_t contains an empty unity object, belonging to the module of *phx.
    // *phx must be valid module handle.
    // The current module must be the root module of *phx.
    // Returns:
    //  On success,  1 and *prv, containing unity/utEmpty, with pmsm of the current module.
    //  On failure: -2, *prv is not modified.
  s_long cv_ff_pcos::Fmakepcoscref(const unity::mod_handle* phx, cref_t<unity>* prv)
  {
    if (!(phx && prv)) { return -2; }
    if (phx->_pmsm_rl() != t_uc::pls_modsm()) { return -2; }
    t_msm pmsm = phx->_pmsm(); if (!pmsm) { return -2; }
    f_recreate fre = (f_recreate)pmsm(t_uc::msm_cv_recreate); if (!fre) { return -2; }
    typedef cref_t<unity>::iref2<unity, pcos_aux_v1> t_iref;
    cref_t<unity> o1 = t_iref::create0(1, pcsd_pcos_obj(), 0x14 | iref2_flags::use_aux, t_iref::handler_dflt, iref2_args_t<pcos_aux_v1>::args(*phx));
      if (!o1) { return -2; }
    s_long res = -2;
    try { res = fre(o1._pnonc_u()); } catch (...) {}
      if (res < 1) { return -2; }
    bmdx_str::words::swap_bytes(o1, *prv);
    return 1;
  }

    // Creates cref_t<unity> in *prv, such that all is true:
    //    1) cref_t belongs to the current module.
    //    2) cref_t contains a unity object, belonging to the module of *phx, with copy of *psrc.
    //    3) if *psrc is unity/utObject, the wrapped object may be from any module (this is not checked here).
    // *phx must be valid module handle.
    // The current module must be the root module of *phx.
    // Returns:
    //  On success,  1 and *prv, containing a copy of psrc, but  with pmsm of the current module.
    //  On failure: -2, *prv is not modified.
  s_long cv_ff_pcos::Fmakepcoscrefcp(const unity::mod_handle* phx, const unity* psrc, cref_t<unity>* prv)
  {
    if (!(phx && psrc && prv)) { return -2; }
    cref_t<unity> o1;
    s_long res = Fmakepcoscref(phx, &o1);
      if (res < 1) { return res; }
    res = -2;
    try { *o1._pnonc_u() = *psrc; bmdx_str::words::swap_bytes(o1, *prv); res = 1; } catch (...) {}
    return res;
  }
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message dispatcher.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if bmdx_part_dispatcher_mt

namespace bmdx
{

struct cch_slot
{
  struct qbi_value { cref_t<std::string, cref_nonlock> msg; cref_t<std::wstring, cref_nonlock> src; };
  struct qs_value { unity hmsg; cref_t<carray_t<char>, cref_nonlock> bin; }; // bin may contain no object

  critsec_t<dispatcher_mt>::csdata lkd; // for locking pins and queues against multithreaded read/write
  cref_t<std::string, cref_nonlock> r_pin; // input pin data
  cref_t<fifo_m11_t<std::string, meta::nothing, __vecm_tu_selector>, cref_nonlock> r_q; // input queue
  cref_t<fifo_m11_t<qbi_value, meta::nothing, __vecm_tu_selector>, cref_nonlock> r_qbi; // input command queue
  cref_t<fifo_m11_t<qs_value, meta::nothing, __vecm_tu_selector>, cref_nonlock> r_qs; // subscription queue
  cref_t<std::wstring, cref_nonlock> r_lsrc; // last source address, for input command pins and queues

  cref_t<hashx<unity, s_long>, cref_nonlock> r_hsubs;
    //--
    //  a) for qs slot: "output list" { target address (string), 0 }
    //  b) for subscriber slot (pi, qi): "input list" { source address (string), flags }
    //    flags: remote op. in progress: 0x1 - subscribing, 0x2 - unsubscribing, 0x4 - checking.
    //  c) for any slot (incl. pi, qi), except qs: no reference (no inputs, or this slot type cannot subscribe)
    // NOTE Exact correspondence between input (subscriber (any)) and output (supplier (qs)) lists
    //  is NOT guaranteed. The lists may become unequal due to:
    //    a) mem. alloc failure when creating/removing whole thread or multiple slots,
    //    b) netwrok transfer failure (remote list update notification won't pass),
    //    c) qs output list is fixed, so it's not modified if a subscriber is removed
    //      (and starts function automatically if a subscriber is added).
    //    d) adding a slot which is factually subscribed to some qs supplier, but it's not mentioned in its input list.

  s_long phase;
    //--
    //  0 - no msg.
    //  1 - input msg arrived.
    //      2 - input msg popped.
    //      3 - command response pushed by the receiver and forwarded to sender.
    //  4 - command sent.
    //      5 - command response received.
    //      6 - command response popped.
    //  pbo: 0, 6 --> 4 --> 5 --> 6
    //  pbi: 0, 3 --> 1 --> 2 --> 3
    //  qbi: 0, 3 --> 2 --> 3
    //  pi: 0, 2 --> 1 --> 2

    // Constant flags, part of slot configuration. May be read without locking.
  s_long ccfgflags;
    bool b_input_any_th() const;
    bool b_input_lpa() const;
    s_long qs_delivery_type() const;
    bool qs_deliv_imm() const;
    bool qs_deliv_thread() const;
    bool qs_deliv_disp() const;
    bool qs_output_fixed() const;

  cch_slot();
};
struct cch_thread
{
    // RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, cref_t<cch_slot, dispatcher_mt> > h_sl;

    // { qs slot name (string), 0 } thread's qs slots with immediate delivery
    //  RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, int> h_qs_imm;

    // { qs slot name (string), 0 } thread's qs slots with delivery by thread periodic()
    //  RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, int> h_qs_thread;

    // true if this thread is responsible for disp. (LPA) delivery type.
    //  RW lock: mst_semaphore m_sl_acquire or m_th_acquire,
    //    + cch_session lkm_ths for cch_thread name_th_disp.
    //    Both locks must be set/acquired to modify b_disp and cch_thread name_th_disp consistently.
    //  Ro lock: m_th_ro_acquire.
  bool b_disp;

    // "Modification state".
    // Bit 0 (+-sl): 1 during slots a) adding, b) removal.
    //  Bit 1 (+-th): 1 during a) thread creation, b) as intention for and during removal.
    //  Bit 2 (read-only access to thread object): 1 during long thread object reading. Same as +-sl, but thread slots are accessible for i/o.
    //  Bits 3..63: count of read-only accesses currently acquired
    // RW lock: mst_semaphore multiple functions.
  meta::s_ll mst;

  cch_thread();
};
} // namespace bmdx
namespace yk_c
{
  template<> struct vecm::spec<cch_slot> { struct aux : vecm::config_aux<cch_slot> { }; typedef config_t<cch_slot, 1, 4, 1, aux> config; };
  template<> struct vecm::spec<cch_thread> { struct aux : vecm::config_aux<cch_thread> { }; typedef config_t<cch_thread, 1, 4, 1, aux> config; };
}
namespace bmdx
{
struct dispatcher_mt::cch_session
{
  cch_session();

  bmdx_shm::critsec_gn gm; // interprocess lock to ensure unique process name
  int exitmode;
  volatile int ses_state; // -1 during initialization, 1 during the current session, 0 after the session

    // Desc.: see dispatcher_mt frqperm();
  volatile s_long frqperm;

    // Short-time lock for main cch_session members read/write: hg_ths, hg_qs_disp, name_th_disp.
  critsec_t<dispatcher_mt>::csdata lkm_ths;

    //  RW lock: lkm_ths.
    // NOTE hg_ths destruction is safe, because its enclosing object (cch_session)
    //    is held reference-counted by all clients.
  hashx<unity, cch_thread> hg_ths;

    // { qs slot name (string), owning thread name (string) } -- process-global qs slots from different threads.
    //  Delivery by disp. thread periodic().
    //  Modified by: _s_add_slots_nl, _s_slot_remove.
    //  RW lock: lkm_ths.
    //  Integrity condition: value --> valid thread, key --> valid slot in that thread.
  hashx<unity, unity> hg_qs_disp;

    // { pi or qi slot name (string), owning thread name (string) } -- process-global input slots from different threads.
    //  Modified by: _s_add_slots_nl, _s_slot_remove.
    //  RW lock: lkm_ths.
    //  Integrity condition: value --> valid thread, key --> valid slot in that thread.
  hashx<unity, unity> hg_lpai;

  critsec_t<dispatcher_mt>::csdata lkm_nprx; // nprx incr/decr lock
  volatile s_long nprx; // nprx counts the dispatcher_mt single object + all thread_proxy objects

  meta::s_ll qsdt; // (const) >= 0: global qs delivery period, mcs; == -1: no automatic delivery

  std::wstring name_pr; // process name (const)

    // Disp thread name. May be modified on thread creation/removal.
    //  RW lock for setting together with thread's b_disp flag: lkm_ths.
  struct lkm_name_th_disp {};
  cref_t<std::wstring, lkm_name_th_disp> name_th_disp;

  threadctl tc_qsd; // qs messages automatic delivery
  s_long qsd_prio; // last set tc_qsd priority
};
  // cch_thread mst flags control.
  //  Used by all data transfer functions and slot/thread modifications,
  //  to set actions in order and also cancel them in certain cases.
struct dispatcher_mt::mst_semaphore
{
  int m_sl_acquire() throw();
  int m_th_acquire() throw();
  int m_th_ro_acquire() throw();
  void release() throw();
  void set_refs(dispatcher_mt::cch_session& ths_, const std::wstring& thn_);
  mst_semaphore(dispatcher_mt::cch_session& ths_, const std::wstring& thn_);
  mst_semaphore(const mst_semaphore& x);
  mst_semaphore();
  ~mst_semaphore() throw();

  int r_sl(const std::wstring& sln, cref_t<cch_slot, dispatcher_mt>& ret_sl) throw();
  cch_thread* p_thread() throw();
  dispatcher_mt::cch_session* p_ses();
  const std::wstring& thn();

  private:
    dispatcher_mt::cch_session* _p_ths;
    std::wstring _thn;
    int rsv, m_allowed; // in both: 1 (+-sl) or 2 (+-th) from mst
    void operator=(const mst_semaphore&);
};
} // namespace bmdx
namespace yk_c
{
  template<> struct vecm::spec<dispatcher_mt::mst_semaphore> { struct aux : vecm::config_aux<dispatcher_mt::mst_semaphore> { }; typedef config_t<dispatcher_mt::mst_semaphore, 1, 4, 1, aux> config; };
}
namespace bmdx
{
namespace
{
  struct hangdet
  {
    double t0;
    double to_ms;
    hangdet(double to_ms_ = 10000.) : to_ms(to_ms_) { t0 = clock_ms(); }
    operator bool() const { return clock_ms() - t0 >= to_ms; }
  };
}
struct dispatcher_mt::thread_proxy : i_dispatcher_mt
{
  typedef hashx<std::wstring, hashx<std::wstring, s_long> > t_hsubs;
  typedef fifo_m11_t<std::string, meta::nothing, __vecm_tu_selector> t_q;
  typedef fifo_m11_t<cch_slot::qbi_value, meta::nothing, __vecm_tu_selector> t_qbi;
  typedef fifo_m11_t<std::string, meta::nothing, __vecm_tu_selector>::t_elem t_qel;
  typedef fifo_m11_t<cch_slot::qbi_value, meta::nothing, __vecm_tu_selector>::t_elem t_qbiel;

    // A queue in a slot. Accessed only under slot lock, so does not need its own element-wise lock.
  template<class T, class _bs = __vecm_tu_selector>
  struct aux_fifo : fifo_m11_t<T, meta::nothing, _bs>
  {
    typedef typename fifo_m11_t<T>::t_elem t_elem;
    s_long _push_nonblk(const t_elem& x) throw() { if (!this->is_valid()) { return -5; } s_long s(this->_isup), s2(s + 1), r(this->_irec); if (s2 >= this->_elems.n()) { s2 = 0; } if (s2 == r) { return -2; }  t_elem* p = this->_elems.template pval_0u<t_elem>(s); *p = x; if (p->has_ref()) { this->_isup = s2; return 1; } return -4; }
    t_elem _pop_nonblk(bool b_pop) throw() { if (!this->is_valid()) { return t_elem(); } s_long r(this->_irec), s(this->_isup); if (r == s) { return t_elem(); } s_long r2 = r + 1; if (r2 >= this->_elems.n()) { r2 = 0; } t_elem* p = this->_elems.template pval_0u<t_elem>(r); t_elem x = *p; if (b_pop) { p->clear(); this->_irec = r2; } return x; }
  };

  struct tc_autodelivery : threadctl::ctx_base
    { virtual void _thread_proc(); };

  static void __append_vals(unity& dest, const unity& src, s_long n_skip = 0);
  static s_long __recode_slotname(const unity& sn, std::wstring& retval, std::wstring* pn1 = 0, std::wstring* pntail = 0) throw();

  struct address
  {
    const unity& addr() const throw();
    bool is_empty() const throw();
    s_long n() const throw();
    const unity& addr(s_long ind) const;
    std::wstring wstr_addr() const;
    std::wstring wstr_sln() const;
    unity sln_v() const;
    std::wstring wstr_sln_1() const;
    std::wstring wstr_sln_tail() const;
    std::wstring wstr_thn() const;
    std::wstring wstr_pn() const;
    std::wstring wstr_hostn() const;

    s_long set_addr(const unity& x) throw();
    s_long set_addr_LP(const std::wstring& thn, const unity& sln);

    bool isLP_any() const throw();
    bool isLP() const throw();
    bool isLPA() const throw();
    bool isLM() const throw();
    bool isLMA() const throw();
    bool isR() const throw();
    bool isRMA() const throw();
    bool isRPA() const throw();

    s_long sln_ind() const throw();
    _fls75 slt() const throw();

  private:
    unity _addr;
  };

  static s_long __aux_msg_encode(const unity& hm, const arrayref_t<char>* buf, std::string& retmsg) throw();
  static s_long __aux_msg_decode(const std::string& msg, bool b_rmstr, bool b_discard_bin, unity& retmsg, _carray_base_t<char>* retbuf) throw();


  static bool thnchk(const std::wstring& s) throw();
  static bool pnchk(const std::wstring& s) throw();
  static bool hostportchk(const std::wstring& s) throw();
  static bool is_addr_cat(const std::wstring& s) throw();

  static bool sln1chk_main(const std::wstring& sln1);
  static bool sln1chk_main_qs(const std::wstring& sln1);
  static bool sln1chk_qs(const std::wstring& sln1);
  static bool sln1chk_subscriber(const std::wstring& sln1) throw();
  static bool sln1chk_main_o(const std::wstring& sln1);
  static bool sln1chk_main_i(const std::wstring& sln1);
  static std::wstring sln1_root(const std::wstring& sln1);
  static bool sln1chk_iomatch(const std::wstring& ssln1, const std::wstring& dsln1);

private:
  std::wstring _name_th;
  std::wstring _name_pr;
  cref_t<dispatcher_mt::cch_session> _r_ths;

  thread_proxy(const thread_proxy&); thread_proxy& operator=(const thread_proxy& x);

public:

  static s_long _s_proxy_init(thread_proxy& x, const cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thread_name) throw();
  static s_long _s_pop(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw();
  static s_long _s_write(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& hm, const arrayref_t<char>* buf, bool b_delivery) throw();
  static s_long _s_qs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, s_long flags) throw();
  static s_long _s_subs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, cref_t<cch_slot, dispatcher_mt>& r_qs, const std::wstring& _name_th, const std::wstring& _name_qs) throw();
  static s_long _s_subscribe(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& add_qs0, const unity& recv_sl0, s_long rt, const std::wstring& _name_th_recv) throw();
  static s_long _s_request(cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, unity& retval, const unity& args, const std::wstring& _name_th, int frqperm) throw();
  static s_long _s_slot_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotname0, const std::wstring& _name_th) throw();
  static s_long _s_add_slots_nl(cch_session& rses, const std::wstring& k_th, const unity& slotscfg, t_hsubs& hsubs_acc, mst_semaphore* pms = 0) throw();
  static s_long _s_update_subs_lists(cch_session& rses, const t_hsubs& hsubs, int actions) throw();
  static s_long _s_proxy_new(const cref_t<dispatcher_mt::cch_session>& _r_ths, unity& retval, const std::wstring& _name_th) throw();
  static s_long _s_thread_create(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& _cfg0) throw();
  static s_long _s_thread_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thn) throw();

  thread_proxy();
  virtual ~thread_proxy();
  virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw();
  virtual s_long write(const unity& msg, const arrayref_t<char>* buf) throw();
  virtual s_long request(s_long rt, unity& retval, const unity& args = unity()) throw();
  virtual s_long slots_create(const unity& _cfg0) throw();
  virtual s_long slot_remove(const unity& slotname0) throw();
  virtual s_long thread_name(unity& retname) throw();
  virtual s_long process_name(unity& retname) throw();
  virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw();
  virtual s_long periodic(s_long flags) throw();
};



      // input slot related flags
      //  0x4 -- accept input from all threads (if 0, accepts input from its own thread only).
      //  0x8 -- accept global input from disp. thread.
    bool cch_slot::b_input_any_th() const { return (ccfgflags & 0x4) != 0; }
    bool cch_slot::b_input_lpa() const { return (ccfgflags & 0x8) != 0; }
      // qs-related flags
      // mask 0x3 -- delivery type:
      //    1 - immediate, 2 - thread, 3 - disp.
      //    0 - slot is not a qs.
      // 0x10 -- fixed recipients list (no subscribing during run-time)
    s_long cch_slot::qs_delivery_type() const { return ccfgflags & 0x3; }
    bool cch_slot::qs_deliv_imm() const { return (ccfgflags & 0x3) == 1; }
    bool cch_slot::qs_deliv_thread() const { return (ccfgflags & 0x3) == 2; }
    bool cch_slot::qs_deliv_disp() const { return (ccfgflags & 0x3) == 3; }
    bool cch_slot::qs_output_fixed() const { return (ccfgflags & 0x10) != 0; }
cch_slot::cch_slot() : phase(0), ccfgflags(0) {}
cch_thread::cch_thread() : b_disp(false), mst(0) {}
dispatcher_mt::cch_session::cch_session() : gm(0), exitmode(1), ses_state(-1), frqperm(0x1f), nprx(0), qsdt(-1), qsd_prio(4) {}


  // m_* ask for possibility of thread adding/removal, or slots collection modification (slots adding/removal),
  //    or thread object read-only access.
  //    The client may call *_acquire() multiple times until allowed to make modification or read data.
  //    Modification itself is not guarded by a lock, because only one client at a time is allowed to make it.
  //    Read-only access is allowed for multiple clients at a time, and it's also not guarded by a lock.
  //    When mst_semaphore of that client goes out of scope, the appropriate mst flag
  //      is automatically cleared as necessary, allowing for further operations on the thread object and threads collection.
  // Return values:
  //    -1, -3 - failure, the client should stop further efforts.
  //        (-1 - multiple causes. -3 - thread does not exist.)
  //    0 - the client should wait until allowed to operate or fail.
  //    1 - success, the client should make intended operations.
int dispatcher_mt::mst_semaphore::m_sl_acquire() throw()
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 1) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 1)) { return -1; }
  critsec_t<dispatcher_mt> __lock(100, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
  const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v;
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 1 && (th.mst & 2)) { rsv = 0; return -1; }
  if (rsv == 0) { rsv = 1; } // ask for +-sl
  if (th.mst & (1+4)) { return 0; } // need waiting for someone else's op. completion
  th.mst |= 1; m_allowed = 1; // start with +-sl in the current thread
  return 1;
}
int dispatcher_mt::mst_semaphore::m_th_acquire() throw()
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 2) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 2)) { return -1; }
  critsec_t<dispatcher_mt> __lock(100, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
  const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v;
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 0) { th.mst |= 2;  rsv = 2; } // reserve +-th
  if (th.mst & (1+4)) { return 0; } // need waiting for someone else's op. completion
  m_allowed = 2; // start with +-th in the current thread
  return 1;
}
int dispatcher_mt::mst_semaphore::m_th_ro_acquire() throw()
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 4) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 4)) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
  const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v;
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 4 && (th.mst & 2)) { rsv = 0; return -1; }
  if (rsv == 0) { rsv = 4; } // ask for read-only access to thread object
  if (th.mst & 1) { return 0; } // need waiting for someone else's completing +-sl
  th.mst |= 4; th.mst += 8; m_allowed = 4; // add to read-only acquire count
  return 1;
}

void dispatcher_mt::mst_semaphore::release() throw()
{
  if (rsv)
  {
    if (!_p_ths) { rsv = 0; m_allowed = 0; return; }
    critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
    const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) { return; } if (!e_th) { rsv = 0; m_allowed = 0; return; }
    cch_thread& th = e_th->v;
    if (rsv == 1) { if (m_allowed == 1) { th.mst &= ~1ll; } }
      else if (rsv == 2) { if (m_allowed == 2) { th.mst &= ~2ll; } }
      else if (rsv == 4) { if (m_allowed == 4) { th.mst -= 8; if ((th.mst & ~7ll) == 0) { th.mst &= ~4ll; } } }
    rsv = 0; m_allowed = 0;
  }
}

void dispatcher_mt::mst_semaphore::set_refs(dispatcher_mt::cch_session& ths_, const std::wstring& thn_) { if (_p_ths == &ths_ && _thn == thn_) { return; } release(); _thn = thn_; _p_ths = &ths_; }

dispatcher_mt::mst_semaphore::mst_semaphore(dispatcher_mt::cch_session& ths_, const std::wstring& thn_) : _p_ths(&ths_), _thn(thn_), rsv(0), m_allowed(0) {}

  // Semaphore state is not copied - only object references.
dispatcher_mt::mst_semaphore::mst_semaphore(const mst_semaphore& x): _p_ths(x._p_ths), _thn(x._thn), rsv(0), m_allowed(0) {}
dispatcher_mt::mst_semaphore::mst_semaphore(): _p_ths(0), _thn(), rsv(0), m_allowed(0) {}
dispatcher_mt::mst_semaphore::~mst_semaphore() throw() { release(); }

  // sln: valid (recoded) slot name.
  // Returns:
  //    1 and valid reference to slot sln in thread thn if the slot exists
  //      and both thread and list of slots are not under modification.
  //    0 if the client should wait until slots list modification is completed. ret_sl == const.
  //        (This occurs only when m_sl is acquired by someone.
  //        If m_th_ro is acquired, for existing slot, 1 and slot reference is returned.)
  //    -1 if the slot does not exist or cannot be accessed during unknown amount of time. ret_sl == const.
  //    -3 if the thread does not exist. ret_sl == const.
  // NOTE Any access to slot itself is guarded by its individual lock (cch_slot lkd),
  //    which is never mixed with session lock (cch_session lkm_ths).
  //    If the slot removal from its containing thread occurs in between session lock and slot lock,
  //    the slot may be still used to complete operation (e.g. read from message queue) without additional checks
  //    or cancelling. The slot object is automatically deleted when the last cref_t<cch_slot, dispatcher_mt>
  //    goes out of scope.
int dispatcher_mt::mst_semaphore::r_sl(const std::wstring& sln, cref_t<cch_slot, dispatcher_mt>& ret_sl) throw()
{
  if (!_p_ths) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
  const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) { return -1; } if (!e_th) { return -3; }
  cch_thread& th = e_th->v;
  if (m_allowed == 0) { while (1) { if (th.mst & 4) { break; } if (th.mst & 2) { return -1; } if (th.mst & 1) { return 0; } break; } }
  const hashx<unity, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl = 0;
  try { e_sl = e_th->v.h_sl.find(sln); } catch (...) { return -1; } if (!e_sl) { return -1; }
  ret_sl = e_sl->v;
  return 1;
}

  // Returns ptr. to associated thread object, if it exists in session.
cch_thread* dispatcher_mt::mst_semaphore::p_thread() throw()
{
  if (!_p_ths) { return 0; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkm_ths); if (sizeof(__lock)) {}
  const hashx<unity, cch_thread>::entry* e_th = 0; try { e_th = _p_ths->hg_ths.find(_thn); } catch (...) {} if (!e_th) { return 0; }
  return &e_th->v;
}

dispatcher_mt::cch_session* dispatcher_mt::mst_semaphore::p_ses() { return _p_ths; }
const std::wstring& dispatcher_mt::mst_semaphore::thn() { return _thn; }



dispatcher_mt::thread_proxy::thread_proxy() {}
dispatcher_mt::thread_proxy::~thread_proxy()
{
  if (_r_ths)
  {
    critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
    if ((_r_ths._pnonc_u()->nprx -= 1) <= 0) { _r_ths._pnonc_u()->ses_state = 0; }
  }
  _r_ths.clear();
}

s_long dispatcher_mt::thread_proxy::pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw() { return _s_pop(_r_ths, _name_th, slotname, retmsg, retbuf, flags); }
s_long dispatcher_mt::thread_proxy::write(const unity& msg, const arrayref_t<char>* buf) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  unity hm; if (msg.isString()) { try { paramline().decode(msg.cref<utString>().ref(), hm, false); } catch (...) { return -2; } }
  return _s_write(_r_ths, _name_th, msg.isString() ? hm : msg, buf, false);
}
s_long dispatcher_mt::thread_proxy::request(s_long rt, unity& retval, const unity& args) throw() { return _s_request(_r_ths, rt, retval, args, _name_th, _r_ths ? _r_ths.ref().frqperm : 0); }
s_long dispatcher_mt::thread_proxy::slots_create(const unity& _cfg0) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }

  if (!(_cfg0.isString() || (_cfg0.isAssoc() && _cfg0.compatibility() > 0))) { return -1; }
  unity _cfg1; try { if (_cfg0.isString()) { paramline().decode_tree(_cfg0.vstr(), _cfg1, 0x3a); } } catch (...) { return -2; }
  const unity& cfg = _cfg0.isString() ? _cfg1 : _cfg0;

  s_long res = -2;
  cch_session& rses = *_r_ths._pnonc_u(); t_hsubs hsubs;
  res = _s_add_slots_nl(rses, _name_th, cfg, hsubs);
    if (res == 1) { _s_update_subs_lists(rses, hsubs, 4); }
      else if (res == -3) { res = -7; }
  return res;
}
s_long dispatcher_mt::thread_proxy::slot_remove(const unity& slotname0) throw() { return _s_slot_remove(_r_ths, slotname0, _name_th); }
s_long dispatcher_mt::thread_proxy::thread_name(unity& retname) throw() { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_th; return 1; } catch (...) { return -2; } }
s_long dispatcher_mt::thread_proxy::process_name(unity& retname) throw() { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_pr; return 1; } catch (...) { return -2; } }
s_long dispatcher_mt::thread_proxy::subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw() { return _s_subscribe(_r_ths, addr_qs, recv_sl, rt, _name_th); }
s_long dispatcher_mt::thread_proxy::periodic(s_long flags) throw() { s_long res = _s_qs_deliver(_r_ths, _name_th, flags); if (res > 0) { return 1; } if (res == 0 || res == -3) { return res; } if (res == -4) { return -1; } return -2; }


void dispatcher_mt::thread_proxy::tc_autodelivery::_thread_proc()
{
  cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >();
  try {
    cch_session& rses = *_r_ths._pnonc_u();
    while (true)
    {
      cref_t<std::wstring, cch_session::lkm_name_th_disp> rthn = rses.name_th_disp;
      if (rses.ses_state == 1 && rthn)
      {
        bool b = false;
        if (1)
        {
          mst_semaphore ms;
          try { ms.set_refs(rses, rthn.ref()); } catch (...) {}
          int res = 0; hangdet hd; while (true) { if (hd) { res = -1; break; } res = ms.m_th_ro_acquire(); if (res != 0) { break; } sleep_mcs(50); } // res > 0 is checked below
          cch_thread* pth = ms.p_thread();
          if (res > 0 && pth && pth->b_disp) { b = true; }
        }
        if (b) { _s_qs_deliver(_r_ths, rthn.ref(), 0); }
      }
      const meta::s_ll delta = 100000;
      for (meta::s_ll dt = rses.qsdt; dt > 0; dt -= delta)
      {
        sleep_mcs(dt < delta ? dt : delta);
        if (rses.ses_state == 0 || rses.nprx <= 0) { goto lExit; } // during dispatcher_mt(), ses_state == -1, but exit does not occur since nprx == 1
      }
    }
  lExit:;
  } catch (...) {}
  _r_ths.clear();
}

  // Append values from array src to array dest.
  //  If dest is not utUnityArray or utStringArray, it's cleared and initialized as 1-based empty utUnityArray.
  //  If src as not an array, it's treated as single element.
  // NOTE &dest == &src is processed correctly.
  // NOTE If operation fails, it generates exception, leaving dest partially modified.
void dispatcher_mt::thread_proxy::__append_vals(unity& dest, const unity& src, s_long n_skip)
{
  if (n_skip < 0) { n_skip = 0; }
  if (dest.utype() == utStringArray)
  {
    s_long ub = src.arrub();
    if (src.isArray()) { for (s_long i = src.arrlb() + n_skip; i <= ub; ++i) { dest.arr_append(src.vstr(i)); } }
      else { dest.arr_append(src.vstr()); }
  }
  else
  {
    if (dest.utype() != utUnityArray)
    {
      if (&dest == &src)
      {
        unity temp = dest;
        dest.clear(); dest.arr_init<utUnityArray>(1);
        if (temp.isArray()) { s_long ub = temp.arrub(); for (s_long i = temp.arrlb() + n_skip; i <= ub; ++i) { dest.arr_append(temp.val<unity>(i)); } }
          else { dest.arr_append(temp); }
        return;
      }
      dest.clear(); dest.arr_init<utUnityArray>(1);
    }
    if (src.utype() == utUnityArray) { s_long ub = src.arrub(); for (s_long i = src.arrlb() + n_skip; i <= ub; ++i) { dest.arr_append(src.ref<unity>(i)); } }
      else if (src.isArray()) { s_long ub = src.arrub(); for (s_long i = src.arrlb() + n_skip; i <= ub; ++i) { dest.arr_append(src.val<unity>(i)); } }
      else { dest.arr_append(src); }
  }
}

  // sn may be string or already decoded array.
  //  pn1 may be specified to receive 1st part of slot name (in decoded form).
  //  pntail may be specified to receive [2nd..end) parts of slot name.
  // Returns:
  //    1 and recoded slot name, if sn is valid.
  //    -1 - if the name is not valid. retval is not changed.
  //    -2 - failure. retval is not changed.
s_long dispatcher_mt::thread_proxy::__recode_slotname(const unity& sn, std::wstring& retval, std::wstring* pn1, std::wstring* pntail) throw()
{
  try {
    unity _a; const unity* pa(0);
    if (sn.isString()) { paramline().decode1v(sn.vstr(), _a); pa = &_a; }
      else { pa = &sn; }
    if (!(pa->isString() || (pa->isArray() && pa->arrsz() >= 2))) { return -1; }
    std::wstring sln1 = pa->isArray() ? pa->vstr(pa->arrlb()) : pa->vstr();
    if (!sln1chk_main(sln1)) { return -1; }
    if (pn1) { *pn1 = sln1; }
    if (pntail) { if (pa->isArray()) { unity x; __append_vals(x, *pa, 1); *pntail = paramline().encode1v(x); } else { pntail->clear(); } }
    retval = paramline().encode1v(*pa);
    return 1;
  } catch (...) { return -2; }
}

  // Empty, or valid address array (1-based, utUnityArray, containing only strings).
const unity& dispatcher_mt::thread_proxy::address::addr() const throw() { return _addr; }

bool dispatcher_mt::thread_proxy::address::is_empty() const throw() { return _addr.isEmpty(); }
s_long dispatcher_mt::thread_proxy::address::n() const throw() { return _addr.isEmpty() ? 0 : _addr.arrsz(); }

  // ind is 1..n().
const unity& dispatcher_mt::thread_proxy::address::addr(s_long ind) const { return _addr.ref<utUnity>(ind); }

  // Returns string representation of the address,
  //    or empty string if addr() is empty.
std::wstring dispatcher_mt::thread_proxy::address::wstr_addr() const { return paramline().encode1v(_addr); }

  // Returns string representation of slot name part of the address,
  //    or empty string if addr() is empty.
std::wstring dispatcher_mt::thread_proxy::address::wstr_sln() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1) { return s; }
  if (ind >= _addr.arrub()) { s = paramline().encode1v(_addr.vstr(ind)); }
    else { unity _a; __append_vals(_a, _addr, ind - _addr.arrlb()); s = paramline().encode1v(_a); }
  return s;
}

  // Returns slot name part of the address in the form of scalar (if slot name has 1 part)
  //    or array (if slot name has >= 2 parts).
  // Returns empty value if addr() is empty.
unity dispatcher_mt::thread_proxy::address::sln_v() const
{
  unity x; if (_addr.isEmpty()) { return x; } s_long ind = sln_ind(); if (ind < 1) { return x; }
  if (ind >= _addr.arrub()) { x = _addr.ref<utUnity>(ind); }
    else { __append_vals(x, _addr, ind - _addr.arrlb()); }
  return x;
}

  // Returns string representation of the first element of slot name
  //  (including slot type and name root),
  //    or empty string if addr() is empty.
std::wstring dispatcher_mt::thread_proxy::address::wstr_sln_1() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1) { return s; }
  s = _addr.vstr(ind);
  return s;
}

  // Returns string representation of slot name part of the address,
  //    without the first part (type/name root).
  // Returns empty string if addr() is empty or slot name has only one part.
std::wstring dispatcher_mt::thread_proxy::address::wstr_sln_tail() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1 || ind + 1 > _addr.arrub()) { return s; }
  unity _a; __append_vals(_a, _addr, ind + 1 - _addr.arrlb());
  paramline().encode1v(_a, s);
  return s;
}

  // Returns string representation of thread name part of the address,
  //    or empty string if addr() does not contain thread name.
std::wstring dispatcher_mt::thread_proxy::address::wstr_thn() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; }
  const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
  s_long ind = -1;
  if (k1 == L"LP") { ind = 2; }
    else if (k1 == L"LM") { ind = 3; }
    else if (k1 == L"R") { ind = 4; }
  if (ind >= 0) { s = _addr.vstr(ind); }
  return s;
}

  // Returns string representation of process name part of the address,
  //    or empty string if addr() does not contain process name.
std::wstring dispatcher_mt::thread_proxy::address::wstr_pn() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; }
  const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
  s_long ind = -1;
  if (k1 == L"LM") { ind = 2; }
    else if (k1 == L"R") { ind = 3; }
    else if (k1 == L"RPA") { ind = 3; }
  if (ind >= 0) { s = _addr.vstr(ind); }
  return s;
}

  // Returns string representation of host name part of the (remote) address,
  //    or empty string if addr() does not contain host name.
std::wstring dispatcher_mt::thread_proxy::address::wstr_hostn() const
{
  std::wstring s; if (_addr.isEmpty()) { return s; }
  const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
  if (k1.length() > 0 && k1[0] == L'R') { s = _addr.vstr(2); }
  return s;
}

  // x may be string or already decoded array.
  // Returns:
  //    1 if set decoded, corrected address.
  //    -1 if x is not a valid address. addr() is cleared.
  //    -2 if setting failed. addr() is cleared.
s_long dispatcher_mt::thread_proxy::address::set_addr(const unity& x) throw()
{
  _addr.clear();
  try {
    unity _a; const unity* pa(0); if (x.isString()) { paramline().decode1v(x.vstr(), _a); pa = &_a; } else { pa = &x; }
    if (!(pa->isArray() && pa->arrsz() >= 2)) { return -1; }
    s_long nb = pa->arrlb();
    const std::wstring k1 = pa->vstr(nb + 0);
    if (!is_addr_cat(k1)) { return -1; }
    if (k1 == L"LP") { if (pa->arrsz() < 3 || !thnchk(pa->vstr(nb + 1)) || !sln1chk_main(pa->vstr(nb + 2))) { return -1; } }
      else if (k1 == L"LPA") { if (!sln1chk_main_qs(pa->vstr(nb + 1))) { return -1; } }
      else if (k1 == L"LM") { if (pa->arrsz() < 4 || !pnchk(pa->vstr(nb + 1)) || !thnchk(pa->vstr(nb + 2)) || !sln1chk_main(pa->vstr(nb + 3))) { return -1; } }
      else if (k1 == L"LMA") { if (!sln1chk_main_qs(pa->vstr(nb + 1))) { return -1; } }
      else if (k1 == L"R") { if (pa->arrsz() < 5 || !hostportchk(pa->vstr(nb + 1)) || !pnchk(pa->vstr(nb + 2)) || !thnchk(pa->vstr(nb + 3)) || !sln1chk_main(pa->vstr(nb + 4))) { return -1; } }
      else if (k1 == L"RMA") { if (pa->arrsz() < 3 || !hostportchk(pa->vstr(nb + 1)) || !sln1chk_main_qs(pa->vstr(nb + 2))) { return -1; } }
      else if (k1 == L"RPA") { if (pa->arrsz() < 4 || !hostportchk(pa->vstr(nb + 1)) || !pnchk(pa->vstr(nb + 2)) || !sln1chk_main_qs(pa->vstr(nb + 3))) { return -1; } }
      else { return -1; }
    _addr.arr_init<utUnity>(1); _addr.arrsz_(pa->arrsz());
    for (s_long i = 0; i < pa->arrsz(); ++i) { _addr.arr_set(1 + i, pa->vstr(nb + i)); }
    return 1;
  } catch (...) { _addr.clear(); return -2; }
}

  // thn - thread name, sln - slot name (array, or string, representing paramline-encoded scalar or array).
  // Returns:
  //    1 set successfully.
  //    -1 thn or sl is not valid. addr() is cleared.
  //    -2 if setting failed. addr() is cleared.
s_long dispatcher_mt::thread_proxy::address::set_addr_LP(const std::wstring& thn, const unity& sln)
{
  _addr.clear();
  try {
    if (!thnchk(thn)) { return -1; }

    unity _a; const unity* pa(0); if (sln.isString()) { paramline().decode1v(sln.vstr(), _a); pa = &_a; } else { pa = &sln; }
    if (pa->isArray())
    {
      s_long nb = pa->arrlb(); if (!sln1chk_main(pa->vstr(nb + 0))) { return -1; }
      _addr.arr_init<utUnity>(1); _addr.arr_append(L"LP"); _addr.arr_append(thn);
      for (s_long i = 0; i < pa->arrsz(); ++i) { _addr.arr_append(pa->vstr(nb + i)); }
    }
    else
    {
      if (!sln1chk_main(pa->vstr())) { return -1; }
      _addr.arr_init<utUnity>(1); _addr.arr_append(L"LP"); _addr.arr_append(thn); _addr.arr_append(pa->vstr());
    }
    return 1;
  } catch (...) { _addr.clear(); return -2; }
}

bool dispatcher_mt::thread_proxy::address::isLP_any() const throw() { return isLP() || isLPA(); }
bool dispatcher_mt::thread_proxy::address:: isLP() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LP"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLPA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LPA"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLM() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LM"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLMA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LMA"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isR() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"R"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isRMA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"RMA"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isRPA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"RPA"; } catch (...) {} return false; }

  // Returns base index of slot name in addr():
  //  >= 2 on success.
  //  -1 if addr() is empty.
  //  -2 on failure.
s_long dispatcher_mt::thread_proxy::address::sln_ind() const throw()
{
  try {
    if (_addr.isEmpty()) { return -1; }
    const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
    s_long ind = -2;
    if (k1 == L"LP") { ind = 3; }
      else if (k1 == L"LPA") { ind = 2; }
      else if (k1 == L"LM") { ind = 4; }
      else if (k1 == L"LMA") { ind = 2; }
      else if (k1 == L"R") { ind = 5; }
      else if (k1 == L"RMA") { ind = 3; }
      else if (k1 == L"RPA") { ind = 4; }
    return ind;
  } catch (...) { return -2; }
}

  // Returns slot type part,
  //  or empty string if addr() is empty or on failure.
_fls75 dispatcher_mt::thread_proxy::address::slt() const throw()
{
  try {
    s_long ind = sln_ind(); if (ind < 1) { return _fls75(); }
    _fls75 sln1 = _addr.ref<utUnity>(ind).vflstr();
    return sln1.substr(0, sln1.find('_'));
  } catch (...) {}
  return _fls75();
}


  // Encodes message hash or map (hm), converts the result to char std::string,
  //    adds header length prefix. If buf is given, buf length and data are appended.
  // retmsg: the resulting string.
  // Limits:
  //    max. hm size when encoded into string - 2^30 16-bit characters.
  //    max. buf size - 2^31-1 chars.
  //    sum of sizes must be < min(std::string::max_size(), 2^60).
  // Returns:
  //    1 - success.
  //    -1 - hm is not a hash or map.
  //    -2 - failure (mem. alloc. or smth. else).
  // Message format:
  //  <part1 length>[<space><part2 length>][<space><additional info>]:<part1>[<part2>]
  //    [] - optional parts.
  //    part1: input message hash, paramline-encoded into wide-character string, and then into UTF-8.
  //    part2: optional binary part of message (bytes).
  //    length format: decimal number >= 0 in ASCII, up to 15 digits.
  //    space: any number of any " \r\n\t".
  //    part1 length: > 0.
  //    part2 length: >= 0.
  //    ':' character separates header from message parts.
  //    additional info: reserved. NOTE Max. header length (incl. ':') == 1000.
s_long dispatcher_mt::thread_proxy::__aux_msg_encode(const unity& hm, const arrayref_t<char>* buf, std::string& retmsg) throw()
{
  try {
      if (!hm.isAssoc()) { return -1; }
    const short max_digits = 15; meta::s_ll max_len(0);  max_len += 999999999999999ll;

      if (buf && buf->n() > max_len) { return -2; }
    std::string msg1 = wsToBsUtf8(paramline().encode(hm));
      if (meta::s_ll(msg1.length()) > max_len) { return -2; }
      if (meta::s_ll(msg1.length()) + (buf ? buf->n() : 0) + 2 + max_digits * 2 > max_scstr()) { return -2; }

    std::string hdr(2 + max_digits * 2, ' ');
    s_long nc1 = 0; s_long nc2 = 0;
    nc1 = bmdx_str::conv::str_from_s_ll(meta::s_ll(msg1.length()), &hdr[0], max_digits); if (nc1 < 1) { return -2; }
    if (buf) { nc2 = bmdx_str::conv::str_from_s_ll(meta::s_ll(buf->n()), &hdr[nc1 + 1], max_digits); if (nc2 < 1) { return -2; } }

    _t_sz n0 = buf ? nc1 + nc2 + 2 : nc1 + 1;
    _t_sz n1 = msg1.size();
    _t_sz n2 = buf ? _t_sz(buf->n()) : 0;

    hdr[n0 - 1] = ':';
    retmsg.resize(n0 + n1 + n2);
    yk_c::bytes::memmove_t<char>::F(&retmsg[0], &hdr[0], meta::t_pdiff(n0));
    yk_c::bytes::memmove_t<char>::F(&retmsg[0] + n0, &msg1[0], meta::t_pdiff(n1)); msg1.clear();
    if (buf) { yk_c::bytes::memmove_t<char>::F(&retmsg[0] + n0 + n1, buf->pd(), meta::t_pdiff(n2)); }

    return 1;
  } catch (...) { return -2; }
}

  // Decodes message and binary attachment (if exists).
  //    Message must have message part, and optional binary data part.
  //    Part format:
  //    [ \t\r\n]*([0-9]{1,10})([^:]*):(.*)
  //    \1 is decimal length of data (\3). \2 is optional metadata.
  //    Length must not exceed 2^31 - 1.
  // b_rmstr:
  //    true - retmsg must be std::wstring (unity / utString).
  //    false - retmsg must be hash, containing decoded data (unity / utHash).
  // b_discard_bin: ignore message binary part if it is received but no retbuf given.
  // Returns:
  //  2 - success, got 2 parts.
  //  1 - success, got 1 part.
  //  0 - got empty message, no parts.
  //  -1 - msg has wrong format.
  //  -2 - failure (mem. alloc. or smth else).
s_long dispatcher_mt::thread_proxy::__aux_msg_decode(const std::string& msg, bool b_rmstr, bool b_discard_bin, unity& retmsg, _carray_base_t<char>* retbuf) throw()
{
  try {
    retmsg.clear();

    typedef unsigned char u_char;
    _t_sz n(msg.length());
    if (n == 0) { if (retbuf) { retbuf->realloc(0, 0, 0, 0); } return 0; }
    const char *ptr1(0), *ptr2(0); meta::s_ll l1(0), l2(0);
    bool b_bin = false; // binary part presence
    if (1)
    {
      const _t_sz max_digits = 15; const _t_sz max_hdr = 1000;
      _t_sz pos_term = 0;
      for (_t_sz i = 0; i < msg.size(); ++i) { if (msg[i] == ':') { pos_term = i; ptr1 = msg.c_str() + i + 1; break; } if (i >= max_hdr) { return -1; } }
        if (!ptr1) { return -1; }
      _t_sz pos1 = 0; for (_t_sz i = pos1; i < pos_term + 1; ++i) { char c = msg[i]; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { pos1 = i; break; } }
      _t_sz pos2 = pos1; for (_t_sz i = pos1; i < pos_term + 1; ++i) { char c = msg[i]; if (!(c >= '0' && c <= '9')) { pos2 = i; break; } l1 = l1 * 10 + meta::s_ll(c - '0'); }
        if (pos2 <= pos1) { return -1; }
        if (pos2 - pos1 > max_digits) { return -1; }
        if (pos2 < pos_term) { char c = msg[pos2]; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { return -1; } }

      pos1 = pos2; for (_t_sz i = pos1; i < pos_term + 1; ++i) { char c = msg[i]; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { pos1 = i; break; } }
      pos2 = pos1; for (_t_sz i = pos1; i < pos_term + 1; ++i) { char c = msg[i]; if (!(c >= '0' && c <= '9')) { pos2 = i; break; } l2 = l2 * 10 + meta::s_ll(c - '0'); }
        if (pos2 - pos1 > max_digits) { return -1; }
        if (pos2 < pos_term) { char c = msg[pos2]; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { return -1; } }
      if (pos2 > pos1) { b_bin = true; ptr2 = ptr1 + size_t(l1); } else { l2 = 0; }
        if (meta::s_ll(ptr1 - msg.c_str()) + l1 + l2 > meta::s_ll(n)) { return -1; }
    }

    bool b_bin_key = b_bin && retbuf == 0 && !b_discard_bin; // store binary part in "bin" key of the returned message

    std::wstring wmsg1 = bsUtf8ToWs(ptr1, l1);

    if (b_rmstr)
    {
      if (b_bin_key)
      {
        unity h; unity k_bin = L"bin";
        paramline().decode(wmsg1, h, false); wmsg1.clear();
        unity& x = h.hash(k_bin); x.u_clear(utString);
        if (l2)
        {
          std::wstring& ws = x.ref<utString>();
          ws.resize(_t_wz(l2));
          if (1) { _yk_reg wchar_t* pw = &ws[0]; _yk_reg const char* p = ptr2; _yk_reg const char* p_end = p + l2; while (p != p_end) { *pw++ = wchar_t(u_char(*p++)); } }
        }
        paramline().encode(h, wmsg1);
      }
      else if (b_bin && retbuf) { if (!retbuf->realloc(l2, 0, 0, 0)) { return -2; } bytes::memmove_t<char>::F(retbuf->pd(), ptr2, meta::t_pdiff(l2)); }
      else if (retbuf) { retbuf->realloc(0, 0, 0, 0); }

      retmsg = wmsg1; wmsg1.clear();
    }
    else
    {
      paramline().decode(wmsg1, retmsg, false); wmsg1.clear();

      if (b_bin_key)
      {
        unity k_bin = L"bin";
        unity& x = retmsg.hash(k_bin); x.u_clear(utString);
        if (l2)
        {
          std::wstring* pws = x.pval<utString>(); // 0 if another module strings are incompatible
          std::wstring s2; if (!pws) { pws = &s2; }
          pws->resize(_t_wz(l2));
          if (1) { _yk_reg wchar_t* pw = &(*pws)[0]; _yk_reg const char* p = ptr2; _yk_reg const char* p_end = p + l2; while (p != p_end) { *pw++ = wchar_t(u_char(*p++)); } }
          if (pws == &s2) { x = s2; } // copy to incompat. string
        }
      }
      else if (b_bin && retbuf) { if (!retbuf->realloc(l2, 0, 0, 0)) { return -2; } bytes::memmove_t<char>::F(retbuf->pd(), ptr2, meta::t_pdiff(l2)); }
      else if (retbuf) { retbuf->realloc(0, 0, 0, 0); }
    }

    return b_bin ? 2 : 1;
  } catch (...) { return -2; }
}



bool dispatcher_mt::thread_proxy::thnchk(const std::wstring& s) throw() { return !(s.length() == 0 || (s.length() >= 2 && s[0] == L'_' && s[1] == L'_')); }
bool dispatcher_mt::thread_proxy::pnchk(const std::wstring& s) throw() { return s.length() != 0 && !is_addr_cat(s); }
bool dispatcher_mt::thread_proxy::hostportchk(const std::wstring& s) throw() { return s.length() != 0 && !is_addr_cat(s); }
bool dispatcher_mt::thread_proxy::is_addr_cat(const std::wstring& s) throw()
{
  if (s.length() == 0) { return false; }
  if (s[0] == L'L')
  {
    switch (s.length())
    {
      case 2: return s[1] == L'P' || s[1] == L'M';
      case 3: return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
      default: return false;
    }
  }
  else if (s[0] == L'R')
  {
    if (s.length() == 1) { return true; }
    if (s.length() != 3) { return false; }
    return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
  }
  return false;
}

bool dispatcher_mt::thread_proxy::sln1chk_main(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b')
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; }
    if (sln1[0] == 'p' && (sln1[2] == 'o' || sln1[2] == 'i')) { return true; }
      else if (sln1[0] == L'q' && sln1[2] == 'i') { return true; }
  }
  else
  {
    if (sln1[2] != L'_') { return false; }
    if (sln1[0] == L'p' && (sln1[1] == 'o' || sln1[1] == 'i')) { return true; }
      else if (sln1[0] == L'q' && (sln1[1] == 'i' || sln1[1] == 's')) { return true; }
  }
  return false;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_qs(const std::wstring& sln1) { return sln1chk_main(sln1) && sln1[0] == L'q' && sln1[1] == L's'; }
bool dispatcher_mt::thread_proxy::sln1chk_qs(const std::wstring& sln1) { return sln1.length() >= 4 && sln1[0] == L'q' && sln1[1] == L's' && sln1[2] == L'_'; }
bool dispatcher_mt::thread_proxy::sln1chk_subscriber(const std::wstring& sln1) throw() { return sln1.length() >= 4 && (sln1[0] == 'p' || sln1[0] == 'q') && sln1[1] == 'i' && sln1[2] == L'_'; }
bool dispatcher_mt::thread_proxy::sln1chk_main_o(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b')
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; }
    if (sln1[0] == 'p' && (sln1[2] == 'o' || sln1[2] == 'i')) { return true; }
      else if (sln1[0] == L'q') { if (sln1[2] == 'i') { return true; } }
  }
  else
  {
    if (sln1[2] != L'_') { return false; }
    if (sln1[0] == L'p' && sln1[1] == 'o') { return true; }
      else if (sln1[0] == L'q' && sln1[1] == 's') { return true; }
  }
  return false;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_i(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b')
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; }
    if (sln1[0] == 'p' && (sln1[2] == 'o' || sln1[2] == 'i')) { return true; }
      else if (sln1[0] == L'q' && sln1[2] == 'i') { return true; }
  }
  else
  {
    if (sln1[2] != L'_') { return false; }
    if (sln1[0] == L'p' && sln1[1] == 'i') { return true; }
      else if (sln1[0] == L'q' && (sln1[1] == 'i' || sln1[1] == 's')) { return true; }
  }
  return false;
}
std::wstring dispatcher_mt::thread_proxy::sln1_root(const std::wstring& sln1)
{
  _t_wz i = sln1.find(L'_');
  return i == nposw ? std::wstring() : sln1.substr(i+1);
}
bool dispatcher_mt::thread_proxy::sln1chk_iomatch(const std::wstring& ssln1, const std::wstring& dsln1)
{
  if (ssln1.length() < 4 || dsln1.length() < 4) { return false; }
  if (ssln1[1] == L'b')
  {
    if (ssln1.length() < 5 || dsln1.length() < 5 || ssln1[3] != L'_' || dsln1[3] != L'_') { return false; }
    if (ssln1[0] == 'p' && ssln1[2] == 'o' && (dsln1[0] == 'p' || dsln1[0] == 'q') && dsln1[2] == 'i') { return true; }
      else if (ssln1[0] == 'p' && ssln1[2] == 'i' && dsln1[0] == 'p' && dsln1[2] == 'o') { return true; }
      else if (ssln1[0] == 'q' && ssln1[2] == 'i' && dsln1[0] == 'p' && dsln1[2] == 'o') { return true; }
  }
  else
  {
    if (ssln1[2] != L'_') { return false; }
    if (ssln1[0] == 'p' && ssln1[1] == 'o' && dsln1[0] == 'p' && dsln1[1] == 'i') { return true; }
      else if (ssln1[0] == 'p' && ssln1[1] == 'o' && dsln1[0] == 'q' && (dsln1[1] == 'i' || dsln1[1] == 's')) { return true; }
      else if (ssln1[0] == 'q' && ssln1[1] == 's' && (dsln1[0] == 'p' || dsln1[0] == 'q') && dsln1[1] == 'i') { return true; }
  }
  return false;
}



  //    1 - success.
  //    -2 - failed.
  //    -3 - disp. session is closed.
  //    -5 - invalid thread name.
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_proxy_init(thread_proxy& x, const cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thread_name) throw()
{
  if (!thnchk(thread_name)) { return -5; }
  if (!_r_ths) { return -2; }
  try {
    std::wstring process_name;
    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_ths); if (sizeof(__lock)) {}
      if (_r_ths.ref().ses_state != 1) { return -3; }
      process_name = _r_ths.ref().name_pr;
      if (!_r_ths.ref().hg_ths.find(thread_name)) { return -7; }
    }
    x._name_th = thread_name;
    x._name_pr = process_name;

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
      x._r_ths = _r_ths;
      x._r_ths._pnonc_u()->nprx += 1;
    }
    return 1;
  } catch (...) { return -2; }
}

s_long dispatcher_mt::thread_proxy::_s_pop(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw()
{
  if (!_r_ths) { return -2; } if (_r_ths.ref().ses_state != 1) { return -3; }
  try {

      //  Det. exact slot name.
    std::wstring ssln1, ssln_r;
    s_long res = __recode_slotname(slotname, ssln_r, &ssln1);
      if (res != 1) { return res == -1 ? -1 : -2; }

      // Check if slot exists / get ref.
    cref_t<cch_slot, dispatcher_mt> ri_sl;
    if (1)
    {
      mst_semaphore ms(*_r_ths._pnonc_u(), _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(ssln_r, ri_sl); if (res < 0) { if (!ms.p_thread()) { return -7; } return -6; } if (res > 0) { break; } sleep_mcs(50); }
    }
    cch_slot& i_sl = *ri_sl._pnonc_u();

      // Check if message exists, get message, set slot phase as necessary for slot type
    if (ssln1[1] == L'i') // pi, qi
    {
      if (ssln1[0] == L'p')
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase != 1) { return 0; }

        s_long res = 3;
        if ((flags & 2) == 0) { res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf); }
        if (res <= 0) { return -22; }

        if ((flags & 8) != 0) { } // peek only, do not pop
        else
        {
          i_sl.r_pin.clear();
          i_sl.phase = 2;
        }
        return res;
      }
      else if (ssln1[0] == L'q')
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}

        t_qel r_msg = i_sl.r_q._pnonc_u()->pop(-1, false);
          if (!r_msg) { return 0; }

        s_long res;
        if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
        else
        {
          res = __aux_msg_decode(r_msg.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
            if (res <= 0) { return -22; }
        }

        if ((flags & 8) != 0) { } // peek only, do not pop
        else
        {
          i_sl.r_q._pnonc_u()->pop(-1, true);
        }
        return res;
      }
      else { return -1; }
    }
    else if (ssln1[1] == L'b') // pbo, pbi, qbi
    {
    //  1 - input msg arrived.
    //      2 - input msg popped.
    //      3 - command response pushed by the receiver and forwarded to sender.
    //  4 - command sent.
    //      5 - command response received.
    //      6 - command response popped.
    //  pbo: 0, 6 --> 4 --> 5 --> 6
    //  pbi: 0, 3 --> 1 --> 2 --> 3
    //  qbi: 0, 3 --> 2 --> 3
    //  pi: 0, 2 --> 1 --> 2
      if (ssln1[2] == L'o') // pbo (popping response)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase == 4) { return -20; }
        if (i_sl.phase != 5) { return 0; }

        s_long res;
        if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
        else
        {
          res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
            if (res <= 0) { return -22; }
        }

        if ((flags & 8) == 0)
        {
          i_sl.r_pin.clear();
          i_sl.phase = 6;
        }
        return res;
      }
      else if (ssln1[0] == 'p' && ssln1[2] == L'i') // pbi (popping command)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase == 2) { return -21; }
        if (i_sl.phase != 1) { return 0; }

        s_long res;
        if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
        else
        {
          res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
            if (res <= 0) { return -22; }
        }

        if ((flags & 8) != 0) { } // peek only, do not pop
        else
        {
          i_sl.r_pin.clear();
          i_sl.phase = 2;
        }
        return res;
      }
      else if (ssln1[0] == 'q' && ssln1[2] == L'i') // qbi (popping command from queue)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase == 2) { return -21; }
        if (i_sl.r_qbi.ref().is_empty()) { return 0; }

        t_qbiel r_msg = i_sl.r_qbi._pnonc_u()->pop(-1, false);
          if (!r_msg.has_ref()) { return -2; }

        s_long res;
        if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
        else
        {
          res = __aux_msg_decode(r_msg.ref().msg.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
            if (res <= 0) { return -22; }
        }

        if ((flags & 8) != 0) { } // peek only, do not pop
        else
        {
          i_sl.r_qbi._pnonc_u()->pop(-1, true);
          i_sl.r_lsrc = r_msg.ref().src;
          i_sl.phase = 2;
        }
        r_msg.clear();

        return res;
      }
      return -1;
    }
    return -1; // po, qo, qs
  } catch (...) { return -2; }
}

  // _name_th: name of the thread, calling _s_write (should be same as thread name in source address (src = ...), if it's specified)
  // hm: must be already decoded mesage (hash or map).
  //  Must have "src", "trg", "text" fields.
  //  During processing, "src" and "trg" fields are modified, to reflect the receiving side context.
  // NOTE _s_write is called multiple times for one message from _s_subs_deliver,
  //  each time with different "trg" and re-set "src".
s_long dispatcher_mt::thread_proxy::_s_write(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& hm0, const arrayref_t<char>* buf, bool b_delivery) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }
  try {
    const unity k_src(L"src"), k_trg(L"trg"), k_text(L"text");
    if (!(hm0.isAssoc() && hm0.u_has(k_src, 4) && hm0.u_has(k_trg, 4) && hm0.u_has(k_text, 4))) { return -1; }

    const unity& __src0 = hm0[k_src];
    const unity& __trg0 = hm0[k_trg];
    unity hm1;
    unity& __src = hm1.hash(k_src); __src = __src0;
    for (s_long pos = hm0.assocl_first(); pos != hm0.assocl_noel(); pos = hm0.assocl_next(pos))
    {
      const unity& v = hm0.assocl_c(pos); if (&v == &__src0) { continue; }
      if (!v.isLocal()) { if (v.compatibility() < 0) { return -2; } }
      hm1.hash(hm0.assocl_key(pos)).set_obj_atop(const_cast<unity&>(v), false);
    }

    s_long res;

      // Source address:
      // a) On b_delivery == false:
      //    <slot name> - thread sending a message from itself.
      //  Before encoding, the address is completed: |LP|<thread name>|<slot name>.
      // b) On b_delivery == true:
      //    |LP|<thread name>|<qs slot name>
      //    |LPA|<qs slot name>
      //  -- thread's periodic() / _s_qs_deliver() sends a message to a subscriber.
      //  Thread name must be same as _name_th.
      //  Source qs slot existence is checked, but the slot itself is not accessed, because this is done already by _s_qs_deliver.
    address sa; std::wstring ssln_r, ssln1, sslntail;
    if (b_delivery)
    {
      sa.set_addr(__src);
          // For delivery, source address should be that of sourcing qs slot, belonging to _name_th (in the current process).
        if (sa.isLP()) { if (sa.wstr_thn() != _name_th) { return -1; } }
          else if (sa.is_empty() || sa.isLPA()) { return -1; }
          else { return -1; }
      res = __recode_slotname(sa.sln_v(), ssln_r, &ssln1, &sslntail);
        if (res != 1) { return -1; }
        if (!sln1chk_main_o(ssln1)) { return -1; }
    }
    else
    {
      unity a;
      __append_vals(a, L"LP");
      __append_vals(a, _name_th);
      __append_vals(a, __src);
      sa.set_addr(a);
        if (sa.is_empty()) { return -1; }
      res = __recode_slotname(sa.sln_v(), ssln_r, &ssln1, &sslntail);
        if (res != 1) { return -1; }
        if (!sln1chk_main_o(ssln1)) { return -1; }
      __src = sa.addr();
    }

    address da;
    res = da.set_addr(__trg0); if (res != 1) { return res == -1 ? -1 : -2; }

    if (da.isLP_any())
    {
      std::wstring dthn;
        // Det. dest. slot type and exact name.
        // Check source / dest. type match.
      std::wstring dsln_r = da.wstr_sln();
      std::wstring dsln1 = da.wstr_sln_1();
        if (!sln1chk_main_i(dsln1)) { return -1; }
        if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
        if (sslntail.length() > 0 && da.wstr_sln_tail() != sslntail) { return -1; }
        if (!sln1chk_iomatch(ssln1, dsln1)) { return -1; }

        // Get slot refs.
      cref_t<cch_slot, dispatcher_mt> rs_sl, rd_sl;
      if (1)
      {
        if (da.isLP()) { dthn = da.wstr_thn(); }
        else // LPA
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
          const hashx<unity, unity>::entry* e_sl2th = rses.hg_lpai.find(dsln_r);
            if (!e_sl2th) { return -8; }
          dthn = e_sl2th->v.vstr();
        }
        mst_semaphore ms_d(rses, dthn), ms_s(rses, _name_th);
        hangdet hd;
        while (true)
        {
          if (hd) { return -2; }
          int res1 = 1, res2 = 1;
          if (!rs_sl) { res2 = ms_s.r_sl(ssln_r, rs_sl); if (res2 < 0) { if (!ms_s.p_thread()) { return -7; } return -6; } }
          if (!rd_sl) { res1 = ms_d.r_sl(dsln_r, rd_sl); if (res1 < 0) { if (!ms_d.p_thread()) { return -9; } return -8; } }
          if (res1 > 0 && res2 > 0) { break; }
          sleep_mcs(50);
        }
      }
      cch_slot& d_sl = *rd_sl._pnonc_u();
      cch_slot& s_sl = *rs_sl._pnonc_u();

      if (dsln1[1] == L'i' || dsln1[1] == L's' || (dsln1[1] == L'b' && dsln1[2] == L'i')) // all inputs: pi, qi, qs, pbi, qbi
      {
        if (da.isLPA()) { if (!d_sl.b_input_lpa()) { return -10; } }
          else if (!d_sl.b_input_any_th()) { if (_name_th != dthn) { return -10; } }
      }

        // Check phases, recode message, write dest. slot, update phases.
      if (dsln1[1] == L'i') // pi, qi
      {
        if (dsln1[0] == L'p')
        {
          cref_t<std::string, cref_nonlock> msg2; if (!msg2.create0(true)) { return -2; }
          s_long res = __aux_msg_encode(hm1, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          d_sl.r_pin = msg2; msg2.clear();
          d_sl.phase = 1;
          return 1;
        }
        else if (dsln1[1] == L'i')
        {
          cref_t<std::string, cref_nonlock> msg2; if (!msg2.create0(true)) { return -2; }
          s_long res = __aux_msg_encode(hm1, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          if (static_cast<aux_fifo<std::string>*>(d_sl.r_q._pnonc_u())->_push_nonblk(msg2) < 0) { return -20; }
          msg2.clear();
          return 1;
        }
        else { return -2; }
      }
      else if (dsln1[1] == L'b') // pbo, pbi, qbi
      {
        cref_t<std::string, cref_nonlock> msg2; if (!msg2.create0(true)) { return -2; }
        s_long res = __aux_msg_encode(hm1, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

        cref_t<std::wstring, cref_nonlock> cmdsrc;
        if (dsln1[2] == L'o')
        {
          cmdsrc.copy(da.wstr_addr(), true);
            if (!cmdsrc.has_ref()) { return -2; }
        }
        else if (dsln1[2] == L'i')
        {
          cmdsrc.copy(sa.wstr_addr(), true);
            if (!cmdsrc.has_ref()) { return -2; }
        }

// phase:
//  1 - input msg arrived, 2 - input msg popped.
//    3 - command response pushed by the receiver and forwarded to sender.
//  4 - command sent, 5 - command response received.
//    6 - command response popped.
//  pbo: 0, 6 --> 4 --> 5 --> 6
//  pbi: 0, 3 --> 1 --> 2 --> 3
//  qbi: 0, 3 --> 2 --> 3
//  pi: 0, 2 --> 1 --> 2
        if (dsln1[2] == L'o' && (dsln1[0] == 'p' || dsln1[0] == 'q')) // pbo (writing response to sender pin)
        {
            // Reverse order of locks for pbo == forward order of locks for pbi and qbi.
          critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          s_long& d_ph = d_sl.phase;
          s_long& s_ph = s_sl.phase;

          if (!(d_ph == 4 && s_ph == 2)) { return -5; }
          if (cmdsrc.ref() != s_sl.r_lsrc.ref()) { return -1; } // previously -13
          d_sl.r_pin = msg2; msg2.clear();
          s_sl.r_lsrc.clear();
          d_ph = 5;
          s_ph = 3;
          return 1;
        }
        else if (dsln1[0] == 'p' && dsln1[2] == L'i') // pbi (writing command to receiver pin)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
          s_long& d_ph = d_sl.phase;
          s_long& s_ph = s_sl.phase;

          if (!((d_ph == 0 || d_ph == 3) && (s_ph == 0 || s_ph == 6))) { return -5; }
          d_sl.r_pin = msg2; msg2.clear();
          d_sl.r_lsrc = cmdsrc; cmdsrc.clear();
          d_ph = 1;
          s_ph = 4;
          return 1;
        }
        else if (dsln1[0] == 'q' && dsln1[2] == L'i') // qbi (writing command to receiver queue)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
          s_long& s_ph = s_sl.phase;

          if (!(s_ph == 0 || s_ph == 6)) { return -5; }
          cref_t<cch_slot::qbi_value, cref_nonlock> rz;
            if (!rz.create0(true)) { return -2; }
            rz._pnonc_u()->msg = msg2; msg2.clear();
            rz._pnonc_u()->src = cmdsrc; cmdsrc.clear();
          if (static_cast<aux_fifo<cch_slot::qbi_value>*>(d_sl.r_qbi._pnonc_u())->_push_nonblk(rz) < 0) { return -20; }
          rz.clear();
          s_ph = 4;
          return 1;
        }
      }
      else if (dsln1[1] == L's') // qs
      {
        if (dsln1[0] == L'q')
        {
            // Creating subscription message.
          cref_t<cch_slot::qs_value, cref_nonlock> r_x; if (!r_x.create0(true)) { return -2; }
          cch_slot::qs_value& x = *r_x._pnonc_u();
          try { x.hmsg = hm0; x.hmsg[k_src] = __src; } catch (...) { return -2; }
          if (buf)
          {
            if (!x.bin.create0(true)) { return -2; }
            if (!x.bin._pnonc_u()->resize(buf->n())) { return -2; }
            bytes::memmove_t<char>::F(x.bin._pnonc_u()->pd(), buf->pd(), meta::t_pdiff(buf->n()));
          }

            // Writing to qs slot.
          if (1)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            s_long res = static_cast<aux_fifo<cch_slot::qs_value>*>(d_sl.r_qs._pnonc_u())->_push_nonblk(r_x);
            r_x.clear();
            if (res < 0) { return -20; }
          }

            // Immediate delivery if necessary.
          if (d_sl.qs_deliv_imm()) { _s_subs_deliver(_r_ths, rd_sl, dthn, dsln_r); }
          return 1;
        }
      }
      return -2;
    }
    else
    {
      // ~!!! not impl. LM, R, LMA, RPA, RMA
      return -11;
    }
  } catch (...) { return -2; }
}

  // _s_qs_deliver delivers messages
  //  1) from the calling _name_th's qs slots with delivery type "thread" (listed in cch_thread::h_qs_thread).
  //  2) (only if _name_th is disp thread) from qs slots of different threads, with delivery type "disp" (listed in global ths.hg_qs_disp).
  //
  // Returns:
  //    >=0 - number of subscription queues processed.
  //    -1 - failure, no messages sent.
  //    -2 - failure, some part of messages has been sent.
  //    -3 - session is closed, no messages sent.
  //    -4 - invalid flags.
s_long dispatcher_mt::thread_proxy::_s_qs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, s_long flags) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  if (flags != 0) { return -4; }
  if (_name_th.empty()) { return -1; }
  s_long res = 0;
  cch_session& rses = *_r_ths._pnonc_u();
  try {
    bool b_disp = false;
    vec2_t<cref_t<cch_slot, dispatcher_mt> > sls_disp;
    vec2_t<std::wstring> slsn_disp, sls_thn_disp;
    vec2_t<cref_t<cch_slot, dispatcher_mt> > sls_th;
    vec2_t<std::wstring> slsn_th;
    if (1)
    {
      mst_semaphore ms(rses, _name_th); hashx<std::wstring, mst_semaphore> h_ms_disp;
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_ro_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth_qs = ms.p_thread(); if (!pth_qs) { return -2; }
      if (pth_qs->b_disp)
      {
        hashx<dispatcher_mt::mst_semaphore*, int> h_waits;
        if (1) // get refs. for (disp. delivery) slots that are ready to receive messages
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
          b_disp = true;
          for (s_long i = 0; i < rses.hg_qs_disp.n(); ++i)
          {
            std::wstring sln = rses.hg_qs_disp(i)->k.vstr();
            std::wstring thn = rses.hg_qs_disp(i)->v.vstr();
            mst_semaphore& ms_x = h_ms_disp[thn];
            ms_x.set_refs(rses, thn);

            slsn_disp.push_back(sln);
            sls_thn_disp.push_back(thn);
            sls_disp.push_back(cref_t<cch_slot, dispatcher_mt>());

            int res = ms_x.m_th_ro_acquire();
              if (res == 0) { h_waits[&ms_x] = 1; }
              if (res == 1) { ms_x.r_sl(sln, sls_disp.back()); }
          }
        }

        bool b_waited = h_waits.n() > 0;
        hangdet hd;
        while (h_waits.n()) // wait for 1 or more slots to become ready
        {
          if (hd) { break; }
          sleep_mcs(50);
          s_long i = 0;
          while (i < h_waits.n())
          {
            mst_semaphore& ms_x = *h_waits(i)->k;
            int res = ms_x.m_th_ro_acquire(); // try to acquire thread lock
              if (res == 0) { ++i; continue; }
                else { h_waits.remove_i(i); }
          }
        }

        if (b_waited) // get remaining refs. for (disp. delivery) slots that have been waited for
        {
          for (s_long i = 0; i < slsn_disp.n(); ++i)
          {
            cref_t<cch_slot, dispatcher_mt>& r_qs = sls_disp[i];
              if (r_qs) { continue; }
            std::wstring& sln = slsn_disp[i]; std::wstring& thn = sls_thn_disp[i]; mst_semaphore& ms_x = h_ms_disp[thn];
            if (ms_x.m_th_ro_acquire() == 1) { ms_x.r_sl(sln, r_qs); } // try to get slot ref. if having thread lock
          }
        }
      }

      if (1) // get refs. for (thread delivery) slots that are ready to receive messages
      {
        hashx<unity, int>& h_qs = pth_qs->h_qs_thread;
        for (s_long i = 0; i < h_qs.n(); ++i)
        {
          std::wstring sln = h_qs(i)->k.vstr(); cref_t<cch_slot, dispatcher_mt> r_qs;
          if (ms.r_sl(sln, r_qs) == 1) { sls_th.push_back(r_qs); slsn_th.push_back(sln); }
        }
      }
    } // end semaphores scope

    for (s_long i = 0; i < sls_th.n(); ++i)
    {
      s_long res2 = _s_subs_deliver(_r_ths, sls_th[i], _name_th, slsn_th[i]);
      if (res2 < res) { res = res2; if (res == -3) { return res; } }
      if (res >= 0 && res2 >= 0) { ++res; }
    }
    if (b_disp)
    {
      for (s_long i = 0; i < sls_disp.n(); ++i)
      {
        cref_t<cch_slot, dispatcher_mt>& r_qs = sls_disp[i];
          if (!r_qs) { continue; }
        s_long res2 = _s_subs_deliver(_r_ths, r_qs, sls_thn_disp[i], slsn_disp[i]);
        if (res2 < res) { res = res2; if (res == -3) { return res; } }
        if (res >= 0 && res2 >= 0) { ++res; }
      }
    }
    return res;
  } catch (...) { return -2; }
}

  // _name_th - r_qs owner thread name.
  // _name_qs: r_qs name.
  // Returns:
  //    >=0 - number of messages processed (sent + failed).
  //    -1 - failure, no messages sent.
  //    -2 - failure, some part of messages may have been sent.
  //    -3 - session is closed, no messages sent.
s_long dispatcher_mt::thread_proxy::_s_subs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, cref_t<cch_slot, dispatcher_mt>& r_qs, const std::wstring& _name_th, const std::wstring& _name_qs) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  if (!r_qs.has_ref()) { return -1; }

  s_long res0 = -1;
  try {
    const unity k_src(L"src"), k_trg(L"trg"), k_src0(L"src0"), k_trg0(L"trg0");
    address sa;
      if (1)
      {
        if (!sln1chk_main_qs(_name_qs)) { return -1; }
        unity a;
        __append_vals(a, L"LP");
        __append_vals(a, _name_th);
        __append_vals(a, paramline().decode1v(_name_qs));
        sa.set_addr(a);
          if (!sln1chk_qs(sa.wstr_sln_1())) { return -1; }
      }
    cch_slot& qs_sl = *r_qs._pnonc_u();

    typedef aux_fifo<cch_slot::qs_value>::t_elem t_elem;
    vec2_t<t_elem> msgs;
    hashx<unity, s_long> hsubs;
    try {
      critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
      if (!(qs_sl.r_qs.has_ref() && qs_sl.r_hsubs.has_ref())) { return -2; }
      aux_fifo<cch_slot::qs_value>* pqs = static_cast<aux_fifo<cch_slot::qs_value>*>(qs_sl.r_qs._pnonc_u());
      while (true) { t_elem x = pqs->_pop_nonblk(true); if (!x) { break; } msgs.push_back(x); }
      if (hsubs.hashx_copy(qs_sl.r_hsubs.ref(), false) < 1) { return -2; }
    } catch (...) {}

    res0 = -2; s_long n = 0;
    for (s_long imsg = 0; imsg < msgs.n(); ++imsg)
    {
      cch_slot::qs_value* pv = msgs[imsg]._pnonc_u(); if (!pv) { continue; }
      unity& hmsg = pv->hmsg; if (!hmsg.isAssoc()) { continue; }
      arrayref_t<char> bin;  if (pv->bin) { bin = pv->bin.ref(); }
      for (s_long isubs = 0; isubs < hsubs.n(); ++isubs)
      {
        try
        {
          address da;
          da.set_addr(hsubs(isubs)->k); if (da.is_empty()) { continue; }
          hmsg.assoc_set(k_src0, hmsg[k_src], true);
          hmsg.assoc_set(k_trg0, hmsg[k_trg], true);
          hmsg.assoc_set(k_src, sa.addr(), false);
          hmsg.assoc_set(k_trg, da.addr(), false);
        } catch (...) { continue; }
        _s_write(_r_ths, _name_th, hmsg, pv->bin ? &bin : 0, true);
      }
      ++n;
    }

    return n;
  } catch (...) { return res0; }
}

s_long dispatcher_mt::thread_proxy::_s_subscribe(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& add_qs0, const unity& recv_sl0, s_long rt, const std::wstring& _name_th_recv) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  if (!(rt >= 1 && rt <= 5)) { return -1; }
  cch_session& rses = *_r_ths._pnonc_u();
  try {
    address sa, da;
      int res;
      res = sa.set_addr(unity(add_qs0)); if (res != 1) { return res == -1 ? -1 : -2; }
        std::wstring ssln1 = sa.wstr_sln_1();
        std::wstring sslntail = sa.wstr_sln_tail();
          if (!sln1chk_qs(ssln1)) { return -1; }
      if (sa.isLP_any()) // local subscription
      {
        if (rt == 4 || rt == 5) { return 0; }
        res = da.set_addr_LP(_name_th_recv, recv_sl0); if (res != 1) { return res == -1 ? -1 : -2; }
        std::wstring dsln1 = da.wstr_sln_1();
          if (!sln1chk_subscriber(dsln1)) { return -1; }
          if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
          if (sslntail.length() > 0 && da.wstr_sln_tail() != sslntail) { return -1; }

        cref_t<cch_slot, dispatcher_mt> rqs_sl; std::wstring sln_s = sa.wstr_sln();
        if (sa.isLPA())
        {
          std::wstring thn_qs;
          if (1)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
            const hashx<unity, unity>::entry* e1 = rses.hg_lpai.find(sln_s); if (!e1) { return 10; }
            thn_qs = e1->v.vstr();
          }
          if (sa.set_addr_LP(thn_qs, sln_s) != 1) { return -2; }
          mst_semaphore ms(rses, thn_qs);
          hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return 10; } if (res > 0) { break; } sleep_mcs(50); }
        }
        else
        {
          mst_semaphore ms(rses, sa.wstr_thn());
          hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return 10; } if (res > 0) { break; } sleep_mcs(50); }
        }

        cref_t<cch_slot, dispatcher_mt> rsub_sl;
        if (1)
        {
          mst_semaphore ms(rses, _name_th_recv);
          hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(da.wstr_sln(), rsub_sl); if (res < 0) { return -5; } if (res > 0) { break; } sleep_mcs(50); }
        }

        cch_slot& qs_sl = *rqs_sl._pnonc_u();
        cch_slot& sub_sl = *rsub_sl._pnonc_u();
        unity sda = da.wstr_addr(), ssa = sa.wstr_addr();
        hashx<unity, s_long> *ph1(0), *ph2(0);
        bool b_sub1(false), b_sub2(false);
        if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1 = qs_sl.r_hsubs._pnonc_u(); if (!ph1) { return -2; } b_sub1 = !! ph1->find(da.wstr_addr()); }
        if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} ph2 = sub_sl.r_hsubs._pnonc_u(); if (!ph2) { sub_sl.r_hsubs.create0(true); ph2 = sub_sl.r_hsubs._pnonc_u(); } if (!ph2) { return -2; } b_sub2 = !! ph2->find(ssa); }

        if (rt == 1) // sub
        {
          if (b_sub1 && b_sub2) { return 2; }
          if (qs_sl.qs_output_fixed()) { if (!b_sub1) { return 12; } }
          if (!b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} if (ph1->insert(sda) < 0) { return -2; } }
          if (!b_sub2)
          {
            bool bf = false;
            if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}  bf = ph2->insert(ssa) < 0; }
            if (bf && !b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1->remove(sda); }
            if (bf && !b_sub1) { return -2; }
          }
          return 2;
        }
        else if (rt == 2) // uns
        {
          if (!b_sub1 && !b_sub2) { return 1; }
          if (qs_sl.qs_output_fixed()) { if (b_sub1) { return 12; } }
          if (b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1->remove(sda); }
          if (b_sub2) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} ph2->remove(ssa); }
          return 1;
        }
        else // rt == 3, chk sub
          { return b_sub1 && b_sub2 ? 2 : 1; }
      }
      else
      {
        //~!!! not impl. remote request:
        //  recode and verify qs and recv, also verify their matching
        //  the current op. is pending:
        //    match rt
        //    check for timeout
        //    return the result as for now
        //    remove the current op. state if it's completed
        //  no current op.:
        //    create op. state
        //    add op. state into states hash, check memory limit
        //    send ipc or remote request
        // ----------------------------
        // ipc or network thread:
        //  receive remote response
        //  update the current op. state
        return -2;
      }
  } catch (...) { return -2; }
}

  // empty _name_th means the call directly from dispatcher_mt (not associated with any thread).
s_long dispatcher_mt::thread_proxy::_s_request(cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, unity& retval, const unity& args, const std::wstring& _name_th, int frqperm) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  cch_session& rses = *_r_ths._pnonc_u();
  switch (rt)
  {
  case 1: // get list of threads
    try {
      if ((frqperm & 0x8) == 0) { return -4; }
      retval.arr_init<utStringArray>(0);
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      for (s_long i = 0; i < rses.hg_ths.n(); ++i) { retval.arr_append(rses.hg_ths(i)->k.vstr()); }
      return 1;
    } catch (...) { return -2; }
  case 2: // get all slots of a thread
    try {
      if ((frqperm & 0x8) == 0) { return -4; }
      retval.arr_init<utStringArray>(0);
      std::wstring thn = args.vstr();
      mst_semaphore ms(rses, thn);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_ro_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -7; }
      for (s_long i = 0; i < pth->h_sl.n(); ++i) { retval.arr_append(pth->h_sl(i)->k.vstr()); }
      return 1;
    } catch (...) { return -2; }
  case 11: // get N of threads
    try {
      retval.clear();
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      retval = rses.hg_ths.n();
      return 1;
    } catch (...) { return -2; }
  case 12: // get N of slots of a thread
    try {
      retval.clear();
      std::wstring thn = args.vstr();
      mst_semaphore ms(rses, thn);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_ro_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -7; }
      retval = pth->h_sl.n();
      return 1;
    } catch (...) { return -2; }
  case 3: // set priority and timing for internal subscription delivery
    try {
      if ((frqperm & 0x10) == 0) { return -4; }
      if (_r_ths.ref().qsdt < 0) { return -7; }
      unity x = args; x.arrlb_(1); if (!(x.arrsz() == 2)) { return -5; }
      meta::s_ll pr = x.ref<utInt>(1); meta::s_ll dt = x.ref<utInt>(2); if (!(pr >= 1 && pr <= 7 && dt >= 0)) { return -5; }
      if (!_r_ths._pnonc_u()->tc_qsd.set_priority(s_long(pr))) { return -2; }
      _r_ths._pnonc_u()->qsd_prio = s_long(pr);
      _r_ths._pnonc_u()->qsdt = dt;
      return 1;
    } catch (...) { return -2; }
  case 4: // get priority and timing of internal subscription delivery
    try {
      if ((frqperm & 0x10) == 0) { return -4; }
      if (_r_ths.ref().qsdt < 0) { return -7; }
      unity x; x.arr_init<utString>(1);
      x.arr_append(_r_ths.ref().qsd_prio);
      x.arr_append(_r_ths.ref().qsdt);
      retval = x;
      return 1;
    } catch (...) { return -2; }
  case 5: // get new proxy
    try {
      if ((frqperm & 0x4) == 0) { return -4; }
      return _s_proxy_new(_r_ths, retval, args.vstr());
    } catch (...) { return -2; }
  case 6: // create thread
    try {
      if ((frqperm & 0x1) == 0) { return -4; }
      return _s_thread_create(_r_ths, args);
    } catch (...) { return -2; }
  case 7: // remove thread
    try {
      if ((frqperm & 0x2) == 0) { return -4; }
      return _s_thread_remove(_r_ths, args.vstr());
    } catch (...) { return -2; }
  default: { return -1; }
  }
}

s_long dispatcher_mt::thread_proxy::_s_slot_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotname0, const std::wstring& _name_th) throw()
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  cch_session& rses = *_r_ths._pnonc_u();
  try {
    t_hsubs hsubs;
    unity sln = slotname0; if (!(sln.isString() || sln.isArray())) { return -1; }
    unity _a; // exact slot name
    if (sln.isString()) { paramline().decode1v(sln.vstr(), _a); if (!sln1chk_main((_a.isArray() ? _a[1] : _a).vstr())) { return -1; } }
      else { if (!sln1chk_main(sln.vstr(sln.arrlb()))) { return -1; } }
    std::wstring k_sl; s_long res = __recode_slotname(sln, k_sl); if (res != 1) { return res == -1 ? -1 : -2; }
    const unity ku_sl = k_sl;

    s_long deliv = 0; cref_t<cch_slot, dispatcher_mt> r_sl;
    if (1)
    {
      mst_semaphore ms(rses, _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_sl_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -2; }
      ms.r_sl(k_sl, r_sl); if (!r_sl) { return 0; }
      pth->h_sl.remove(ku_sl);

        // The following hash elem. removals do not fail.
        //  cch_thread h_qs_*, cch_session hg_qs_disp, hg_lpai are consistent with cch_thread h_sl.
      deliv = r_sl.ref().qs_delivery_type();
      if (deliv == 1)
      {
        pth->h_qs_imm.remove(ku_sl);
        if (r_sl.ref().b_input_lpa()) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {} rses.hg_lpai.remove(ku_sl); }
      }
      else if (deliv == 2)
      {
        pth->h_qs_thread.remove(ku_sl);
        if (r_sl.ref().b_input_lpa()) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {} rses.hg_lpai.remove(ku_sl); }
      }
      else if (deliv == 3 || r_sl.ref().b_input_lpa())
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
        if (deliv == 3) { rses.hg_qs_disp.remove(ku_sl); }
        if (r_sl.ref().b_input_lpa()) { rses.hg_lpai.remove(ku_sl); }
      }
    }

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &r_sl._pnonc_u()->lkd); if (sizeof(__lock)) {}
      if (r_sl.ref().r_hsubs)
      {
        const hashx<unity, s_long>& hs_sl = r_sl.ref().r_hsubs.ref();
        address _a; _a.set_addr_LP(_name_th, ku_sl);
        if (hs_sl.n() && !_a.is_empty())
        {
          if (deliv) // r_sl is a qs slot
          {
            std::wstring sla = _a.wstr_addr();
            try { for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs[hs_sl(i)->k.vstr()][sla] = 0; } } catch (...) {}
//            _s_update_subs_lists(rses, hsubs, 1);
          }
          else // r_sl may have subscriptions
          {
            std::wstring sla = _a.wstr_addr();
            try { for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs[sla][hs_sl(i)->k.vstr()] = 0; } } catch (...) {}
//            _s_update_subs_lists(rses, hsubs, 2);
          }
        }
      }
    }

    if (hsubs.n()) { _s_update_subs_lists(rses, hsubs, deliv ? 1 : 2); }

    return 1;
  } catch (...) { return -2; }
}

  // Adds slots to existing thread.
  // Returns:
  //  1 - success.
  //    hsubs_acc - the list, accumulating inputs for pins, subscribed to newly created qs slots. Format:
  //      { <ADDR slot-subscriber (local or remote)>, { <ADDR qs slot-supplier>, 4 } }
  //    NOTE on _s_add_slots_nl success, hsubs_acc must be passed to _s_update_subs_lists to complete operation.
  //    NOTE on error, all dispatcher structure changes are cancelled.
  //      But, hsubs_acc may be left modified. It must not be used for any further updates.
  //  -1 - bad arg. (_name_th empty, slotscfg invalid).
  //  -2 - failure (mem. alloc.).
  //  -3 - thread does not exist.
  //  -4 - one of new slot names is not unique.
s_long dispatcher_mt::thread_proxy::_s_add_slots_nl(cch_session& rses, const std::wstring& _name_th, const unity& slotscfg, t_hsubs& hsubs_acc, mst_semaphore* _pms0) throw()
{
  try {
    unity k_slots(L"slots");
    if (!(_name_th.length() > 0 && slotscfg.compatibility() > 0 && slotscfg.isAssoc())) { return -1; }

    mst_semaphore _ms0;
    if (!_pms0)
    {
      _ms0.set_refs(rses, _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = _ms0.m_sl_acquire(); if (res < 0) { return res == -3 ? -3 : -2; } if (res > 0) { break; } sleep_mcs(50); }
    }
    mst_semaphore& ms = _pms0 ? *_pms0 : _ms0;
    cch_thread* pth = ms.p_thread(); if (!pth) { return -3; }

    unity u_empty;
    const unity& sls_cfgroot = slotscfg.u_has(k_slots, 6) ? slotscfg[k_slots] : u_empty;
    struct _aux_qsh
    {
      hashx<unity, unity> h_qsd_, h_lpai_;
      hashx<unity, int> h_qst_, h_qsimm_;
    };
    _aux_qsh qsh;
    hashx<unity, cref_t<cch_slot, dispatcher_mt> > h_sl2;
    hashx<unity, int>& h_qst = pth->h_qs_thread; hashx<unity, int>& h_qsimm = pth->h_qs_imm;
    unity ku_input_all(L"input all"), ku_input_lpa(L"input lpa");
    for (s_long pos = sls_cfgroot.assocl_first(); pos != sls_cfgroot.assocl_noel(); pos = sls_cfgroot.assocl_next(pos))
    {
      const unity& ku_sl0 = sls_cfgroot.assocl_key(pos); if (ku_sl0.isEmpty() || ku_sl0.lenstr() == 0) { continue; }
      std::wstring k_sl, sln1, slntail;
        s_long res = __recode_slotname(ku_sl0.vstr(), k_sl, &sln1, &slntail);
          if (res != 1) { return res == -1 ? -1 : -2; }
          if (!sln1chk_main(sln1)) { return -1; }
      const unity ku_sl = k_sl;

        // Create uninitialized slot.
      cref_t<cch_slot, dispatcher_mt>& rr_sl = h_sl2[ku_sl];
        if (rr_sl.has_ref()) { return -4; }
        if (!rr_sl.create0(true)) { return -2; }
        cch_slot& new_sl = *rr_sl._pnonc_u();

        // Initialize slot data according with slot type.
      const unity* psc = &sls_cfgroot.assocl_c(pos);
        if (!psc->isAssoc()) { psc = 0; }

      s_long cf_inp = 0x4; // allowed inputs (used only by qs, qi, pi, pbi, qbi), default: accept input from all threads
        if (psc && psc->u_has(ku_input_all, 6)) { const unity& x = (*psc)[ku_input_all]; if (!x.isBool()) { return -1; } if (x.isBoolFalse()) { cf_inp &= ~s_long(0x4); } }
        if (psc && psc->u_has(ku_input_lpa, 6)) { const unity& x = (*psc)[ku_input_lpa]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { cf_inp |= 0x8; } }

      if (sln1[0] == L'q') // qi, qbi, qs
      {
        unity k_size(L"size");
        s_long ncap = -1;
           if (psc && psc->u_has(k_size, 6) && (*psc)[k_size].isInt()) { ncap = s_long((*psc)[k_size].vint()); if (ncap < 3) { return -1; } ncap += 1; }

        if (sln1[1] == L'i')
        {
          new_sl.ccfgflags = cf_inp;

          new_sl.r_q.create1(false, ncap >= 0 ? ncap : (sln1 == L"qi_char" ? 201 : 11));
          if (!new_sl.r_q.ref().is_valid()) { return -2; }
        }
        else if (sln1[1] == L's')
        {

          if (!sln1chk_qs(sln1)) { return -1; }
          unity k_delivery(L"delivery");
          s_long qsd = 1;
          if (psc && psc->u_has(k_delivery, 6) && (*psc)[k_delivery].isString())
          {
            std::wstring sd = (*psc)[k_delivery].vstr();
            if (sd == L"immediate") { }
              else if (sd == L"thread") { qsd = 2; }
              else if (sd == L"disp") { qsd = 3; }
              else { return -1; }
          }

          if (!new_sl.r_hsubs.has_ref()) { if (!new_sl.r_hsubs.create0(true)) { return -2; } }

          unity k_output(L"output"), k_output_fixed(L"output fixed");
          s_long qsoutp = 0x0;
          if (psc && psc->u_has(k_output, 6))
          {
            const unity& subs = (*psc)[k_output]; if (!subs.isAssoc()) { return -1; }
            std::wstring s_sa;
            if (1)
            {
              unity a, a2; a.arr_init<utUnityArray>(1);
                __append_vals(a, L"LP");
                __append_vals(a, _name_th);
                __append_vals(a, paramline().decode1v(k_sl, a2));
              s_sa = paramline().encode1v(a);
            }
            for (s_long pos = subs.assocl_first(); pos != subs.assocl_noel(); pos = subs.assocl_next(pos))
            {
              const unity& ku_addr0 = subs.assocl_key(pos); if (ku_addr0.isEmpty() || ku_addr0.lenstr() == 0) { continue; }
              int res; address da;
              res = da.set_addr(ku_addr0.vstr()); if (res != 1) { return res == -1 ? -1 : -2; }
              std::wstring dsln1 = da.wstr_sln_1();
                if (!sln1chk_subscriber(dsln1)) { return -1; }
                if (sln1_root(sln1) != sln1_root(dsln1)) { return -1; }
                if (slntail.length() > 0 && da.wstr_sln_tail() != slntail) { return -1; }
              std::wstring s_da = da.wstr_addr();
              if (new_sl.r_hsubs._pnonc_u()->insert(s_da) != 1) { return -1; }
              hsubs_acc[s_da][s_sa] = 0;
            }
          }
          if (psc && psc->u_has(k_output_fixed, 6)) { const unity& x = (*psc)[k_output_fixed]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { qsoutp |=  0x10; } }

          new_sl.ccfgflags = qsd | cf_inp | qsoutp;

          if (qsd == 3)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
            if (rses.hg_qs_disp.find(ku_sl) || qsh.h_qsd_.find(ku_sl)) { return -4; }
            qsh.h_qsd_[ku_sl] = _name_th;
          }
          else if (qsd == 2) { if (h_qst.find(ku_sl) || qsh.h_qst_.find(ku_sl)) { return -4; } qsh.h_qst_[ku_sl] = 0; }
          else { if (h_qsimm.find(ku_sl) || qsh.h_qsimm_.find(ku_sl)) { return -4; } qsh.h_qsimm_[ku_sl] = 0; }

          //--------------------------------

          new_sl.r_qs.create1(false, ncap >= 0 ? ncap : 11);
          if (!new_sl.r_qs.ref().is_valid()) { return -2; }
        }
        else if (sln1[1] == L'b')
        {
          new_sl.ccfgflags = cf_inp;

          new_sl.r_qbi.create1(false, ncap >= 0 ? ncap : 11);
          if (!new_sl.r_qbi.ref().is_valid()) { return -2; }
        }
      }
      else if (sln1[0] == L'p') // pi, po, pbi, pbo
      {
        if (sln1[1] == L'i') // pi
        {
          new_sl.ccfgflags = cf_inp;
        }
        else if (sln1[1] == L'b' && sln1[2] == L'i') // pbi
        {
          new_sl.ccfgflags = cf_inp;
        }
      }
      else { return -1; }

      if (new_sl.b_input_lpa())
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
        if (rses.hg_lpai.find(ku_sl) || qsh.h_lpai_.find(ku_sl)) { return -4; }
        qsh.h_lpai_[ku_sl] = _name_th;
      }
    }

    for (s_long i = 0; i < h_sl2.n(); ++i) { if (pth->h_sl.find(h_sl2(i)->k)) { return -4; } }

    bool b_done(false);

    s_long n_sl2(0), n_t(0), n_i(0);
    for (s_long i = 0; i < h_sl2.n(); ++i) { if (pth->h_sl.insert(h_sl2(i)->k) != 1) { goto lSkip1; } ++n_sl2; pth->h_sl(pth->h_sl.n() - 1)->v = h_sl2(i)->v; }
    if (qsh.h_qsd_.n() || qsh.h_lpai_.n())
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      bool bf2 = false; s_long n_d1(0), n_d2(0);

        // Insert slot names and their owning threads into the list of LPA ("disp thread") delivery.
      for (s_long i = 0; i < qsh.h_qsd_.n(); ++i) { if (rses.hg_qs_disp.insert(qsh.h_qsd_(i)->k) != 1) { bf2 = true; break; } ++n_d1; rses.hg_qs_disp(rses.hg_qs_disp.n() - 1)->v = qsh.h_qsd_(i)->v; }
      if (!bf2) { for (s_long i = 0; i < qsh.h_lpai_.n(); ++i) { if (rses.hg_lpai.insert(qsh.h_lpai_(i)->k) != 1) { bf2 = true; break; } ++n_d2; rses.hg_lpai(rses.hg_lpai.n() - 1)->v = qsh.h_lpai_(i)->v; } }

      if (bf2)
      {
        for (s_long i = 0; i < n_d1; ++i) { rses.hg_qs_disp.remove(qsh.h_qsd_(i)->k); }
        for (s_long i = 0; i < n_d2; ++i) { rses.hg_lpai.remove(qsh.h_lpai_(i)->k); }
        goto lSkip1;
      }
    }
    for (s_long i = 0; i < qsh.h_qst_.n(); ++i) { if (h_qst.insert(qsh.h_qst_(i)->k) != 1) { goto lSkip1; } ++n_t; h_qst(h_qst.n() - 1)->v = qsh.h_qst_(i)->v; }
    for (s_long i = 0; i < qsh.h_qsimm_.n(); ++i) { if (h_qsimm.insert(qsh.h_qsimm_(i)->k) != 1) { goto lSkip1; } ++n_i; h_qsimm(h_qsimm.n() - 1)->v = qsh.h_qsimm_(i)->v; }

    b_done = true;


lSkip1:;

    if (!b_done)
    {
      for (s_long i = 0; i < n_sl2; ++i) { pth->h_sl.remove(h_sl2(i)->k); }
      for (s_long i = 0; i < n_t; ++i) { h_qst.remove(qsh.h_qst_(i)->k); }
      for (s_long i = 0; i < n_i; ++i) { h_qsimm.remove(qsh.h_qsimm_(i)->k); }
      return -2;
    }

    h_sl2.hashx_clear();

    return 1;
  } catch (...) {}
  return -2;
}

  // Update input list for each of the given subscribers, or remove subscriber from all suppliers output lists.
  //    This function processes subscriber slots in a loop, all necessary locking is done in each iteration.
  //    hsubs: similar to hsubs_acc in _s_add_slots_nl:
  //      { <ADDR slot-subscriber>, { <ADDR qs slot-supplier>, 0 } }
  //    actions:
  //        4 - add supplier to subscriber's input list.
  //        1 - remove supplier from subscriber's input list.
  //        2 - remove subscriber from supplier's output list.
  //        Flags 1 and 2 may be combined.
  // Returns:
  //    2 - success.
  //    1 - some part of subscriptions was not updated due to non-local or invalid address.
  //    0 - some part of subscriptions was not updated due to failure during update.
s_long dispatcher_mt::thread_proxy::_s_update_subs_lists(cch_session& rses, const t_hsubs& hsubs, int actions) throw()
{
  s_long n1 = 0, n2 = 0;
  for (s_long i1 = 0; i1 < hsubs.n(); ++i1)
  {
    bool bf = false;
    try {
      cref_t<cch_slot, dispatcher_mt> rsub_sl;

      const hashx<std::wstring, s_long>& h_qs = hsubs(i1)->v;
      address da; da.set_addr(hsubs(i1)->k);
        if (!da.isLP()) { ++n1; continue; } //~!!! not impl. send request to remote subscriber (remove qs from input list)
        if (!sln1chk_subscriber(da.wstr_sln_1())) { ++n1; continue; }
      std::wstring dsln = da.wstr_sln();
      unity da_u = da.addr();

      if (actions & (4+1))
      {
        mst_semaphore ms(rses, da.wstr_thn());
        int res = -1;
        hangdet hd(2000); while (true) { if (hd) { break; } res = ms.r_sl(dsln, rsub_sl); if (res != 0) { break; } sleep_mcs(50); } if (hd || res < 0) { ++n1; continue; }
      }

      if ((actions & 4) && rsub_sl)
      {
        cch_slot& sub_sl = *rsub_sl._pnonc_u();
        try {
          critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
          for (s_long i2 = 0; i2 < h_qs.n(); ++i2)
          {
            address sa; sa.set_addr(h_qs(i2)->k); if (sa.is_empty()) { continue; }
            if (!sub_sl.r_hsubs.has_ref()) { sub_sl.r_hsubs.create0(false); }
            sub_sl.r_hsubs._pnonc_u()->insert(sa.wstr_addr());
          }
        } catch (...) { bf = true; }
      }

      if ((actions & 1) && rsub_sl)
      {
        cch_slot& sub_sl = *rsub_sl._pnonc_u();
        if (sub_sl.r_hsubs.has_ref())
        {
          const hashx<std::wstring, s_long>& h_qs = hsubs(i1)->v;
          try {
            critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
            for (s_long i2 = 0; i2 < h_qs.n(); ++i2)
            {
              address sa; sa.set_addr(h_qs(i2)->k); if (sa.is_empty()) { continue; }
              sub_sl.r_hsubs._pnonc_u()->remove(sa.wstr_addr());
            }
          } catch (...) { bf = true; }
        }
      }

      if (actions & 2)
      {
        try {
          for (s_long i2 = 0; i2 < h_qs.n(); ++i2)
          {
            address sa; sa.set_addr(h_qs(i2)->k); if (sa.is_empty()) { continue; }
            std::wstring sln_s = sa.wstr_sln();
            if (sa.isLP()) // unsubscribe locally
            {
              cref_t<cch_slot, dispatcher_mt> rqs_sl; int res = -1;
              mst_semaphore ms(rses, sa.wstr_thn());
              hangdet hd(2000); while (true) { if (hd) { break; } res = ms.r_sl(sln_s, rqs_sl); if (res != 0) { break; } sleep_mcs(50); } if (hd) { continue; } if (res < 0) { continue; }

              cch_slot& qs_sl = *rqs_sl._pnonc_u();
              if (!qs_sl.qs_output_fixed())
              {
                critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
                hashx<unity, s_long>* ph1 = qs_sl.r_hsubs._pnonc_u();
                  if (ph1) { ph1->remove(da_u); }
              }
            }
            else if (sa.isLPA()) { continue; } // subscription source addresses are expected to be dereferenced
            else {} //~!!! not impl. send request to remote qs slot (remove subscriber from output list)
          }
        } catch (...) { bf = true; }
      }

      ++n1;
    } catch (...) { bf = true; }
    if (!bf) { ++n2; }
  }
  return n2 == hsubs.n() ? 2 : (n1 == hsubs.n() ? 1 : 0);
}

  // Ret. vals. are same as _s_proxy_init().
  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - failed.
  //    -3 - disp. session is closed.
  //    -5 - invalid thread name.
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_proxy_new(const cref_t<dispatcher_mt::cch_session>& _r_ths, unity& retval, const std::wstring& _name_th) throw()
{
  unity temp; retval.swap(temp);
  if (!_r_ths) { return -2; } if (_r_ths.ref().ses_state != 1) { return -3; }

  dispatcher_mt::thread_proxy* p(0); try { p = new dispatcher_mt::thread_proxy(); } catch (...) {}
    if (!p) { return -2; }
  s_long res = dispatcher_mt::thread_proxy::_s_proxy_init(*p, _r_ths, _name_th);
    if (res < 0) { try { delete p; } catch (...) {} return res; }
  try { retval.set_obj<thread_proxy, o_interfaces<thread_proxy, i_dispatcher_mt> >(*p, true); } catch (...) { try { delete p; } catch (...) {} return -2; }
  return 1;
}

  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - operation failed.
  //    -3 - session is closed.
  //    -5 - invalid argument (args and/or retval as required by request type).
  //    -6 - thread already exists.
s_long dispatcher_mt::thread_proxy::_s_thread_create(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& _cfg0) throw()
{
  if (!_r_ths) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }

  unity _cfg1; const unity* pcfg = 0; const unity* pcfg_th = 0; cref_t<std::wstring, cch_session::lkm_name_th_disp> rthn; const hashx<unity, cch_thread>::entry* e_th = 0; cch_thread* pth = 0; bool b_disp = false; unity ku_thn;
  mst_semaphore ms; int _state = 0; s_long res = -2;
  try {
    if (!(_cfg0.isString() || (_cfg0.isAssoc() && _cfg0.compatibility() > 0))) { return -5; }
    try { if (_cfg0.isString()) { paramline().decode_tree(_cfg0.vstr(), _cfg1, 0x3a); } } catch (...) { return -2; }
    pcfg = _cfg0.isString() ? &_cfg1 : &_cfg0;

    const unity kth_disp(L"disp");

      // Extract and primarily check thread configuration.
    for (s_long i = pcfg->assocl_first(); i != pcfg->assocl_noel(); i = pcfg->assocl_next(i))
    {
      std::wstring k_th = pcfg->assocl_key(i).vstr();
        if (!thnchk(k_th)) { continue; }
        if (rthn) { return -5; }
      pcfg_th = &pcfg->assocl_c(i);
      rthn.create1(false, k_th);
      ku_thn = rthn.ref();

      if (pcfg_th->u_has(kth_disp, 6) && (*pcfg_th)[kth_disp].isBoolTrue())
      {
        b_disp = true;
        cref_t<std::wstring, cch_session::lkm_name_th_disp> rthn_disp = rses.name_th_disp;
        if (rthn_disp && rthn_disp.ref().length() && rthn_disp.ref() != rthn.ref()) { return -2; }
      }
    }
    if (!rthn) { return -5; }

      // Create/insert thread object, lock for further initialization.
    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      s_long res = rses.hg_ths.insert(ku_thn, &e_th);
        if (res < 0) { return -2; } if (res == 0) { return -6; }
      _state |= 1;
      try { ms.set_refs(rses, rthn.ref()); } catch (...) {}
      if (ms.m_th_acquire() < 0) { _state |= 2; }
    }
    pth = &e_th->v;

      // Initialize thread object as configured.
    if ((_state & 2) == 0)
    {
      if (b_disp)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
        cref_t<std::wstring, cch_session::lkm_name_th_disp>& r0 = rses.name_th_disp;
        if (r0 && r0.ref().length() && r0.ref() != rthn.ref()) { _state |= 2; }
          else { rses.name_th_disp = rthn; pth->b_disp = true; _state |= 4; }
      }
    }

      // Add slots.
    t_hsubs hsubs;
    if ((_state & 2) == 0)
    {
      res = _s_add_slots_nl(rses, rthn.ref(), *pcfg_th, hsubs, &ms);
      if (res == 1) { _state = 0;  }
        else { if (res == -1) { res = -5; } else { res = -2; } _state |= 2; }
    }

      // Release lock.
    ms.release();

      // Update subscribers input lists.
    if ((_state & 2) == 0)
    {
      _s_update_subs_lists(rses, hsubs, 4);
    }
  } catch (...) {}

  if (_state & (4 + 1))
  {
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
    if (_state & 4) { pth->b_disp = false; rses.name_th_disp.clear(); }
    if (_state & 1) { rses.hg_ths.remove(ku_thn); }
  }
  return res;
}

  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - operation failed.
  //    -3 - session is closed.
  //    -5 - invalid argument (args and/or retval as required by request type).
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_thread_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thn) throw()
{
  if (!_r_ths) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }

  // get thread lock
  //  remove disp. flag if set
  //  for each slot: slot removal sequence --> hsubs_outs, hsubs_ins
  //  remove thread object
  // release thread lock
  // update subs. lists

  cch_thread* pth = 0;
  hashx<std::wstring, hashx<std::wstring, s_long> > hsubs_outs, hsubs_ins;
  try {
    unity ku_thn = thn;
    mst_semaphore ms(rses, thn);
    pth = ms.p_thread(); if (!pth) { return -7; }
    hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }

    if (pth->b_disp)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      pth->b_disp = false;
      rses.name_th_disp.clear();
    }

    for (s_long j = 0; j < pth->h_sl.n(); ++j)
    {
      try {
        const unity& ku_sl = pth->h_sl(j)->k;
        cref_t<cch_slot, dispatcher_mt>& r_sl = pth->h_sl(j)->v;

        s_long deliv = 0;

          // The following hash elem. removals do not fail.
          //  cch_thread h_qs_*, cch_session hg_qs_disp, hg_lpai are consistent with cch_thread h_sl.
        deliv = r_sl.ref().qs_delivery_type();
        if (deliv == 3 || r_sl.ref().b_input_lpa())
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
          if (deliv == 3) { rses.hg_qs_disp.remove(ku_sl); }
          if (r_sl.ref().b_input_lpa()) { rses.hg_lpai.remove(ku_sl); }
        }

        if (1)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &r_sl._pnonc_u()->lkd); if (sizeof(__lock)) {}
          try {
            if (r_sl.ref().r_hsubs)
            {
              const hashx<unity, s_long>& hs_sl = r_sl.ref().r_hsubs.ref();
              address _a; _a.set_addr_LP(thn, ku_sl);
              if (hs_sl.n() && !_a.is_empty())
              {
                if (deliv) // r_sl is a qs slot
                {
                  std::wstring sla = _a.wstr_addr();
                  for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs_ins[hs_sl(i)->k.vstr()][sla] = 0; }
                }
                else // r_sl may have subscriptions
                {
                  std::wstring sla = _a.wstr_addr();
                    for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs_outs[sla][hs_sl(i)->k.vstr()] = 0; }
                }
              }
            }
          } catch (...) {}
        }
      } catch (...) {}
    }

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
      rses.hg_ths.remove(ku_thn); // does not fail
    }
  } catch (...) { return -2; }

  _s_update_subs_lists(rses, hsubs_ins, 1);
  _s_update_subs_lists(rses, hsubs_outs, 2);

  return 1;
}

dispatcher_mt::dispatcher_mt(const std::string& process_name, const unity& cfg) throw()
  { try { new (this) dispatcher_mt(bsToWs(process_name), cfg); } catch (...) {} }
dispatcher_mt::dispatcher_mt(const std::wstring& process_name, const unity& _cfg0) throw()
{
  if (!(_cfg0.isString() || (_cfg0.isAssoc() && _cfg0.compatibility() > 0))) { return; }
  unity _cfg1; try { if (_cfg0.isString()) { paramline().decode_tree(_cfg0.vstr(), _cfg1, 0x3a); } } catch (...) { return; }
  const unity& cfg = _cfg0.isString() ? _cfg1 : _cfg0;

  if (!_r_ths.create0(true)) { return; }
    _r_ths._pnonc_u()->nprx += 1;

  cch_session& rses = *_r_ths._pnonc_u();

  bool b = true;
  try { while (true) { // once
    const unity kg_qsdt(L"__qsdt");
    const unity kg_exitmode(L"__exitmode");
    const unity kth_disp(L"disp");

    if (!thread_proxy::pnchk(process_name)) { b = false; break; }
    rses.name_pr = process_name;


    if (1) { int e = +cfg / kg_exitmode / 1; if (!(e >= 0 && e <=2)) { b = false; break; } rses.exitmode = e; }
    if (1) { meta::s_ll dt = +cfg / kg_qsdt / -1; if (!(dt >= -1)) { b = false; break; } rses.qsdt = dt; }


      // Lock to make process name unique in the system.
    std::string pnutf("bmdxdmt\t"); pnutf += wsToBsUtf8(process_name);
    rses.gm.~critsec_gn(); new (&rses.gm) bmdx_shm::critsec_gn(pnutf.c_str());
    if (!rses.gm.lock()) { b = false; break; }


      // Initialize threads and slots.
    thread_proxy::t_hsubs hsubs;
    for (s_long i = cfg.assocl_first(); i != cfg.assocl_noel(); i = cfg.assocl_next(i))
    {
      std::wstring k_th = cfg.assocl_key(i).vstr();
      if (!thread_proxy::thnchk(k_th)) { continue; }

      const hashx<unity, cch_thread>::entry* e_th = 0;
        if (rses.hg_ths.insert(k_th, &e_th) != 1) { b = false; break; }

      const unity& th_cfg = cfg.assocl_c(i);
      if (th_cfg.u_has(kth_disp, 6))
      {
        const unity& v = th_cfg[kth_disp];
        if (!v.isBool() || (v.isBoolTrue() && rses.name_th_disp)) { b = false; break; }
        if (v.isBoolTrue()) { if (!rses.name_th_disp.create1(true, k_th)) { b = false; break; } e_th->v.b_disp = true; }
      }

      if (thread_proxy::_s_add_slots_nl(rses, k_th, th_cfg, hsubs) != 1) { b = false; break; }
    }
    if (!b) { break; }

    thread_proxy::_s_update_subs_lists(rses, hsubs, 4);

    if (rses.qsdt >= 0) { if (!rses.tc_qsd.start_auto<thread_proxy::tc_autodelivery, cref_t<cch_session> >(_r_ths)) { b = false; break; } }

    break;
  } } catch (...) { b = false; }
  if (1)
  {
    if (!b) 
    { 
      if (1)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_nprx); if (sizeof(__lock)) {}
        rses.nprx -= 1; 
      }
      _r_ths.clear(); 
      return; 
    }
    rses.ses_state = 1;
  }
}
dispatcher_mt::~dispatcher_mt() throw()
{
  if (_r_ths)
  {
    cch_session& rses = *_r_ths._pnonc_u();
    int e = rses.exitmode;
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_nprx); if (sizeof(__lock)) {}
    rses.nprx -= 1;
    if (e > 0 || rses.nprx <= 0) { rses.ses_state = 0; }
  }
  if (_r_ths)
  {
    cch_session& rses = *_r_ths._pnonc_u();
    int e = rses.exitmode;
    if (e == 2) { while (rses.nprx > 0) { sleep_mcs(50); } } // ~!!! NOTE this may hang if any of CPU threads holding proxy objects, had been forcefully terminated (maybe add timeout param. to cfg)
    _r_ths.clear();
  }
}
bool dispatcher_mt::is_valid() const throw() { return _r_ths; }
bool dispatcher_mt::has_session() const throw() { return _r_ths && _r_ths.ref().ses_state == 1; }
void dispatcher_mt::end_session() throw()
{  if (_r_ths)  {
    cch_session& rses = *_r_ths._pnonc_u();
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_nprx); if (sizeof(__lock)) {}
    rses.ses_state = 0;
} }
bool dispatcher_mt::frqperm(s_long& f, bool b_get, bool b_set) const throw()
{
  if (!has_session()) { return false; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_ths); if (sizeof(__lock)) {}
  volatile s_long& x = _r_ths._pnonc_u()->frqperm; int x0 = x; if (b_set) { x = f; } if (b_get) { f = x0; }
  return true;
}
s_long dispatcher_mt::new_proxy(unity& dest, const std::wstring& _name_th) const throw() { return thread_proxy::_s_proxy_new(_r_ths, dest, _name_th); }
s_long dispatcher_mt::new_proxy(unity& dest, const std::string& _name_th) const throw() { try { return thread_proxy::_s_proxy_new(_r_ths, dest, bsToWs(_name_th)); } catch (...) {} return -2; }

s_long dispatcher_mt::request(s_long rt, unity& retval, const unity& args) throw()
  { try { return thread_proxy::_s_request(_r_ths, rt, retval, args, std::wstring(), -1); } catch (...) { return -2; } }

} // bmdx ns

#endif // bmdx_part_dispatcher_mt

#endif // bmdx_main_H
