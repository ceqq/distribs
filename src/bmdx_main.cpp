// BMDX library 1.5 RELEASE for desktop & mobile platforms
//  (binary modules data exchange)
// rev. 2023-10-07
// See bmdx_main.h for details.

#ifndef bmdx_main_H
  #include "bmdx_main.h"
#else
  #ifndef __bmdx_main_impl_skip
    #define __bmdx_main_impl_skip
  #endif
#endif

  // Fix for GCC issue: in shared module,
  //  '#pragma GCC diagnostic ignored "-Wunused-function"'
  //  is not working if '#pragma GCC diagnostic pop" is present.
#undef __bmdx_GNUC_allow_unused
#if defined(__GNUC__) && !defined(__clang__)
  #define __bmdx_GNUC_allow_unused __attribute__((unused))
#else
  #define __bmdx_GNUC_allow_unused
#endif

#ifndef bmdx_main_intl_lib_H
#define bmdx_main_intl_lib_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// bmdx_main_intl_lib:
//    Internal base library. May be included in the following way
//      #include "bmdx_main.h"
//      #include "bmdx_main.cpp"
//    for testing purposes and as temporary utility.
//    There are NO guarantees of any kind on bmdx_main_intl_lib API.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
  #pragma clang diagnostic ignored "-Wunused-function"
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#if defined(__GNUC__) && !defined(__clang__)
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


#ifndef __bmdx_use_link_h
  #if defined(__linux__) || defined(__SUNPRO_CC) || defined(__sun)

    #define __bmdx_use_link_h 1

    #if defined(__SUNPRO_CC) || defined(__sun)
      #include <sys/link.h>
    #endif
    #include <link.h>

  #elif defined(__FreeBSD__)

    #define __bmdx_use_link_h 2
    #include <link.h>

  #elif (__APPLE__ && __MACH__)
    #define __bmdx_use_link_h 3

    #if bmdx_part_dllmgmt
      #include <mach-o/dyld.h>
    #endif

  #elif defined(_bmdxpl_Wnds)

    #define __bmdx_use_link_h -1
    typedef BOOL (WINAPI *PF_GetModuleHandleExW)(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
    static PF_GetModuleHandleExW f_GetModuleHandleExW()
    {
      #ifdef _APISETLIBLOADER_
        return &GetModuleHandleExW;
      #else
        static void* f = 0;
        if (!f) { f = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetModuleHandleExW"); }
        return (PF_GetModuleHandleExW)f;
      #endif
    }

  #else

    #define __bmdx_use_link_h 0

  #endif
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
#if bmdx_part_dllmgmt && __bmdx_use_link_h > 0 && __bmdx_use_link_h != 3
    // 1. On all systems, the first callback info describes the main executable.
    // 2. Address of the first PT_LOAD section in that info can be used to calculate executable base address.
  static int __x_utils_dl_cb_mod_main_get_addr(struct dl_phdr_info* info, size_t size, void* data_)
  {
    (void)size; void** pd = (void**)data_; *pd = 0;
    if (info->dlpi_phnum == 0) { return 0; } // may occur on Android
    for (unsigned i = 0; i < info->dlpi_phnum; i++)
    {
      if (info->dlpi_phdr[i].p_type == PT_LOAD)
      {
        *pd = (void*)(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
        break;
      }
    }
    return 1;
  }
  extern "C" typedef int (*PF__x_utils_dl_cb_mod_main_get_addr)(struct dl_phdr_info* info, size_t size, void* data_);
#endif

namespace bmdx_main_intl_lib
{
using namespace bmdx;

#if bmdx_part_dllmgmt
  namespace utils_dl
  {
      // 1 - _in_main_exe() is in main executable
      // 0 - _in_main_exe() is in shared library
      // -1 - failed to determine code location
    static int _in_main_exe(__bmdx_noarg1)
    {
      unity_common::__Psm pmsm = unity_common::pls_modsm();
        if (!pmsm) { return -1; }
      #if __bmdx_use_link_h == 3
        char buf[(4*PATH_MAX)] = "";
        char buf2[(4*PATH_MAX)] = "";
        typedef int (*Fpidpath)(int pid, void* buf, bmdx_meta::u_long bufsize);
        #if TARGET_OS_IPHONE
          Fpidpath f = (Fpidpath)dlsym(RTLD_DEFAULT, "proc_pidpath");
        #else
          Fpidpath f = &proc_pidpath;
        #endif
          if (!f || f(getpid(), buf, sizeof(buf)) <= 0) { return -1; }
        Dl_info di; std::memset(&di, 0, sizeof(di));
          if (0 == dladdr((void*)pmsm, &di)) { return -1; }
        if (!realpath(di.dli_fname, buf2)) { return -1; }
        if (0 == std::strcmp(buf, buf2)) { return 1; }
        return 0;
      #elif __bmdx_use_link_h > 0
        void* pbase1 = 0;
        dl_iterate_phdr((PF__x_utils_dl_cb_mod_main_get_addr)&__x_utils_dl_cb_mod_main_get_addr, (void*)&pbase1);
        void* pbase2 = 0;
        Dl_info di; std::memset(&di, 0, sizeof(di));
          if (0 != dladdr((void*)pmsm, &di)) { pbase2 = di.dli_fbase; }
        if (!(pbase1 && pbase2)) { return -1; }
        if (pbase2 == pbase1) { return 1; }
        return 0;
      #elif __bmdx_use_link_h == -1
        HMODULE h1 = GetModuleHandleA(0);
        HMODULE h2 = 0;
        BOOL b = f_GetModuleHandleExW()(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (WCHAR*)pmsm, &h2);
          if (!b) { h2 = 0; }
        FreeLibrary(h1);
        FreeLibrary(h2);
        if (!(h1 && h2)) { return -1; }
        if (h2 == h1) { return 1; }
        return 0;
      #else
        return -1;
      #endif
    }

    #if __bmdx_use_link_h > 0
      static int _mode_dlopen(bool b_au, bool b_noload __bmdx_noarg)
      {
        int mode = RTLD_NOW|RTLD_LOCAL;
        #ifndef __ANDROID__
          if (b_noload) { mode |= RTLD_NOLOAD; }
        #endif
        #ifdef RTLD_FIRST
          mode |= RTLD_FIRST;
        #endif
        #ifdef RTLD_NODELETE
          if (!b_au) { mode |= RTLD_NODELETE; }
        #endif
        return mode;
      }

        // If non-null handle is returned, it later must be closed by the client.
      __bmdx_GNUC_allow_unused static void* _hmod_self(bool b_au __bmdx_noarg)
      {
        int mode = _mode_dlopen(b_au, true);
        if (_in_main_exe() == 1)
        {
          void* h1 = dlopen(0, mode);
          return h1;
        }
        unity_common::__Psm pmsm = unity_common::pls_modsm();
          if (!pmsm) { return 0; }
        Dl_info di; std::memset(&di, 0, sizeof(di));
          if (0 == dladdr((void*)pmsm, &di)) { return 0; }
          if (!di.dli_fname) { return 0; }
        void* h1 = dlopen(di.dli_fname, mode);
        return h1;
      }
    #elif __bmdx_use_link_h == -1
        // If non-null handle is returned, it later must be closed by the client.
      __bmdx_GNUC_allow_unused static HMODULE _hmod_self(bool b_au __bmdx_noarg)
      {
        unity_common::__Psm pmsm = unity_common::pls_modsm();
          if (!pmsm) { return 0; }
        HMODULE h1 = 0;
        DWORD mode = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
        if (!b_au) { mode |= GET_MODULE_HANDLE_EX_FLAG_PIN; }
        BOOL b = f_GetModuleHandleExW()(mode, (WCHAR*)pmsm, &h1);
          if (b) { return h1; }
        return 0;
      }
    #endif
  }
#endif

template<class T> struct weakref_t;
template<class T>
struct refmaker_t
{
  struct t_lock;
  struct ref_info { typename critsec_t<T>::csdata csd; union { s_long __1; volatile char b_ref_valid; }; ref_info() { __1 = 0; b_ref_valid = 0; } };

  const cref_t<ref_info> info;
  const cref_t<T> strongref;

    // Returns true if strongref refers to an object.
  bool b_nonempty() const { return strongref; }

    // refmaker_t function:
    //    uniquely holding object reference, which may have been created in another (client) binary module;
    //    creating weak references to the object, which do not refer to client binary module's data or functions, except the target object pointer itself;
    //    providing common lock on the target object, for all weak references being able to check the target object pointer before dereferencing,
    //      and for safe clearing of the strong reference at any time.
    // NOTEs:
    //    after refmaker_t creation, check .info == true.
  refmaker_t(const cref_t<T>& strongref_) __bmdx_noex : info(), strongref(strongref_) { cref_t<ref_info>& _info = (cref_t<ref_info>&)info; _info.create0(1); }
  refmaker_t() {} // null ref., no critsec_t

    // Sets this->info to the new object, created by dflt. If succeeds, returns true.
  bool create_info(bool b_may_replace) __bmdx_noex { if (info && !b_may_replace) { return false; } cref_t<ref_info> _info2; if (!_info2.create0(1)) { return false; } (cref_t<ref_info>&)info = _info2; return true; }
  void set_ref(const cref_t<T>& strongref_) __bmdx_noex { cref_t<ref_info>& _info = (cref_t<ref_info>&)info; cref_t<T>& r = (cref_t<T>&)strongref; if (_info) { t_lock __lock(*this); if (sizeof(__lock)) {} r = strongref_; _info->b_ref_valid = 1; } else { r = strongref_; } }
  void clear() __bmdx_noex { cref_t<ref_info>& _info = (cref_t<ref_info>&)info; cref_t<T>& r = (cref_t<T>&)strongref; if (_info) { t_lock __lock(*this); if (sizeof(__lock)) {} _info->b_ref_valid = 0; r.clear(); } else { r.clear(); } _info.clear(); }
  void operator=(const refmaker_t& rm) __bmdx_noex { if (this == &rm) { return; } clear(); new (this) refmaker_t(rm); }
  ~refmaker_t() __bmdx_noex { clear(); }

  struct t_lock
  {
    critsec_t<T> __lock;
    t_lock(const refmaker_t& rm) : __lock(rm.info ? 10 : -1, -1, rm.info ? &rm.info->csd : 0) {}
    t_lock(const weakref_t<T>& w) : __lock(w.info ? 10 : -1, -1, w.info ? &w.info->csd : 0) {}
  };
};
template<class T>
struct weakref_t
{
  typedef typename refmaker_t<T>::t_lock t_lock;
  cref_t<typename refmaker_t<T>::ref_info> info;
    // Dereferencing sequence:
    //  1) set a lock: weakref_t::t_lock __lock(weakref_t object),
    //  2) check b_valid() (returns !!(info && info->b_ref_valid && pwk)),
    //  3) use *pwk.
  T* pwk;
    // Returns true if this weakref contains valid non-null object pointer.
  bool b_valid() const { return info && info->b_ref_valid && pwk; }
  weakref_t(const refmaker_t<T>& rm) : info(rm.info), pwk(rm.strongref._pnonc_u()) {} // NOTE weakref_t() is not locked => rm must not be modified concurrently with weakref_t()
  weakref_t() : pwk(0) {}
  weakref_t(const weakref_t& w) : info(w.info), pwk(w.pwk) {} // NOTE weakref_t() is not locked => w must not be modified concurrently with weakref_t()
    // NOTE assignment: both *this and x must not be modified concurrently with assignment *this = x
  void operator=(const refmaker_t<T>& x) __bmdx_noex { info = x.info; pwk = x.strongref._pnonc_u(); }
  void operator=(const weakref_t& x) __bmdx_noex { if (this == &x) { return; } info = x.info; pwk = x.pwk; }
};
} // namespace bmdx_main_intl_lib
namespace yk_c
{
  template<class T> struct vecm::spec<bmdx_main_intl_lib::refmaker_t<T> > { typedef bmdx_main_intl_lib::refmaker_t<T> t; typedef config_t<t, 1, 4, 1> config; };
  template<class T> struct vecm::spec<bmdx_main_intl_lib::weakref_t<T> > { typedef bmdx_main_intl_lib::weakref_t<T> t; typedef config_t<t, 1, 4, 1> config; };
}
namespace bmdx_main_intl_lib
{

#define __shmqueue_ms_nbytes_min (10 * 1024ll)
#define __shmqueue_ms_buf_uid __bmdx_shmfifo_ver
#define __shmqueue_ms_nbytes_overhead 12 // see all occurrences before any modification

static s_long _shmqms_hash_len(s_ll __length) __bmdx_noex { const s_ll e = __shmqueue_ms_buf_uid; return yk_c::hashx_common::kf_basic<s_ll>()(e * (e + 3 * __length)); }
//static s_ll _lmsc_hash_pid_deriv(s_ll pid, s_ll key = 0) __bmdx_noex { const s_ll e = __shmqueue_ms_buf_uid; return yk_c::hashx_common::kf_basic<s_ll>()(e * (e + 7 * (key + pid))); }

  // 8 bytes
struct _shmqms_header { s_long length; s_long lhash; _shmqms_header() { length = lhash = 0; } };

template<class AuxData, class _bs = bmdx_meta::nothing>
struct shmqueue_ms_sender_t // shared memory queue for multiple senders - sender side
{
  typedef arrayref_t<char> t_stringref;
  struct _rso
  {
    char __pad1[sizeof(AuxData) > 0 ? (sizeof(AuxData) + 7) / 8 * 8 : 8];
    AuxData& auxd() { return *(AuxData*)this; }
    bmdx_shm::rfifo_nbl11 ringbuf;
    _rso()
    {
      for (size_t i = 0; i < sizeof(__pad1); ++i) { __pad1[i] = 0; }
      new (this) AuxData();
    }
  };
  typedef bmdx_shm::shmobj2s_t<_rso> t_so;

  const bmdx_shm::t_name_shm qname;
  bmdx_shm::shmobj2s_t<_rso> so;
  vnnqueue_t<cref_t<t_stringref> > lq;

  shmqueue_ms_sender_t(const bmdx_shm::t_name_shm& name_ __bmdx_noarg)
    : qname(name_), so(name_.n() ? name_.c_str() : "_", false, __shmqueue_ms_nbytes_min , __shmqueue_ms_buf_uid)
  { lq.set_cap_hints(-1, -2); }

    // Attempts to connect to shared memory.
    //  Does not ensure that the shared object is locked by the current sender.
    //  Returns:
    //    1 - success, the shared object is already initialized by receiver.
    //    0 - the shared memory exists, but not yet accessible, or the contained object is not yet initialized by the receiver.
    //    -2 - failure.
    //    -3 - couldn't get the shared object side 2 lock.
  s_long attempt_open() __bmdx_noex
  {
    if (so.f_constructed() == 1) { return 1; }
    if (qname.n() <= 0) { return -2; }
    s_long res = so.prepare(false);
    if (res == 1) { if (so.nb() < __shmqueue_ms_nbytes_min) { so.close(); return -2; } return so.f_constructed() == 1 ? 1 : 0; }
    if (res == -3) { return -3; }
    if (res == -4) { return 0; }
    return -2;
  }

    // msend_1: locally queues msg, then sends all currently queued messages.
    // msend_n: locally queues all msgs (transactionally), and if succeeds, sends all currently queued messages.
    // msend_pending: tries to send part or all currently queued messages.
    // b_immediately:
    //      behavior when on failure to deliver the message during the current call:
    //      true: 1) deliver queued messages, 2) deliver msg (msgs) and return 2, 3) on error: do not queue msg; return -3.
    //        NOTE Immediate sending is faster, because it copies memory only once, and does not make any dynamic allocations.
    //          But to succeed, the shared queue must currently have free space enough for
    //            msg (or msgs) total length + __shmqueue_ms_nbytes_overhead bytes per each message.
    //      false: 1) queue msg, 2) deliver all queued messages; return 2 or 1 on complete or partial delivery.
    // Returns:
    //  Success:
    //      2 - msg has (all msgs have) been sent.
    //      1 - msg is (part or all msgs are) queued.
    //        The shared queue is valid, but currently has not enough place.
    //        Call msend_pending() later.
    //  Failure:
    //    0 - the shared queue exists, but not yet initialized by the receiver.
    //      No attempt made to queue message(s) locally.
    //    -1 - invalid msg object (or any of msgs objects).
    //    -2 - failure.
    //    -3 (only b_immediately == true) - "try again later" - a) couldn't get the shared object side 2 lock, b) shared queue overflow.
    //    -5 - size of msg or any of msgs is too large for the shared queue (in case of msgs, all msgs not sent and not queued).
  s_long msend_1(t_stringref msg, bool b_immediately) __bmdx_noex
  {
    if (!msg.is_valid()){ return -1; }
    s_long res = attempt_open(); if (res < 1) { return res; }
    if (b_immediately) { res = _try_deliver_lq(arrayref_t<t_stringref>(&msg, 1)); }
    else
    {
      cref_t<t_stringref> rmsg;
      res = _make_rmsg(msg, rmsg); if (res < 1) { return res; }
      if (lq.push_1(rmsg) < 1) { return -2; }
      res = _try_deliver_lq();
    }
    return res;
  }
  s_long msend_n(const arrayref_t<t_stringref>& msgs, bool b_immediately) __bmdx_noex
  {
    if (!msgs.is_valid()) { return -1; } for (s_ll i = 0; i < msgs.n(); ++i) { if (!msgs[i].is_valid()) { return -1; } }
    s_long res = attempt_open(); if (res < 1) { return res; }
    if (b_immediately) { res = _try_deliver_lq(msgs); }
    else
    {
      cpparray_t<cref_t<t_stringref> > rmm; if (!rmm.resize(msgs.n())) { return -2; }
      for (s_ll i = 0; i < msgs.n(); ++i) { res = _make_rmsg(msgs[i], rmm[i]); if (res < 1) { return res; } }
      if (lq.push_n(rmm.pd(), rmm.n()) < 1) { return -2; }
      res = _try_deliver_lq();
    }
    return res;
  }
  s_long msend_pending() __bmdx_noex
  {
    s_long res = attempt_open(); if (res < 1) { return res; }
    if (lq.navl() <= 0) { return 2; }
    res = _try_deliver_lq();
    return res;
  }

private:

    // If _check_for_make_md returns true, __make_md with same arguments will 100% succeed.
  static bool __check_for_make_md(s_ll nbufavl, t_stringref msg)
  {
    if (!msg.is_valid()) { return false; }
    const s_ll __length = __shmqueue_ms_nbytes_overhead + msg.n(); if (__length > 0x7fffff00ll) { return false; } if (__length > nbufavl) { return false; }
    return true;
  }

  static bool __make_md(s_ll nbufavl, t_stringref msg, _shmqms_header& h, s_long& chs)
  {
    if (!__check_for_make_md(nbufavl, msg)) { return false; }
    const s_ll __length = __shmqueue_ms_nbytes_overhead + msg.n();
    bmdx_str::words::set_be4(&h.length, 0, s_long(__length));
    bmdx_str::words::set_be4(&h.lhash, 0, _shmqms_hash_len(__length));
    s_long x = 0; for (s_ll i = 0; i < msg.n(); ++i) { x += (unsigned char)msg[i]; }
    bmdx_str::words::set_be4(&chs, 0, x);
    return true;
  }

    // NOTE _make_rmsg succeeds only if this->so is currently valid.
    // 1 - success (and rmsg contains valid ref.), -1 - bad msg, -2 - failure, -5 - msg is too long.
  s_long _make_rmsg(t_stringref msg, cref_t<t_stringref>& rmsg) __bmdx_noex
  {
    if (!msg.is_valid()){ return -1; }
    if (so.f_constructed() != 1) { return -2; }
    try {
      _shmqms_header header; s_long checksum = 0;
      if (!__make_md(so->ringbuf.n(), msg, header, checksum)) { return -5; }
      rmsg = bmdx_shm::shmqueue_s::make_rba_mp(t_stringref((const char*)&header, 8), msg, t_stringref((const char*)&checksum, 4));
      return rmsg ? 1 : -2;
    } catch (...) {}
    return -2;
  }
    // 2 - success, pushed all (local queue is now empty); 1 - success, pushed part of messages into the valid shared queue (local queue is still not empty); -2 - shared queue is not accessible (or other failure); -3 - added messages are not delivered
    //  added are tried all at once after lq; they are not queued even on delivery failure.
  s_long _try_deliver_lq(const arrayref_t<t_stringref>& added = arrayref_t<t_stringref>())
  {
    if (so.f_constructed() != 1) { return -2; }
    if (!added.is_valid()) { return -2; }
    try {
      typename t_so::shared_lock __lock(so); if (!__lock.b_lk_any()) { return added.n() > 0 ? -3 : (lq.navl() > 0 ? 1 : 2); }
      if (so.f_constructed() != 1) { return -2; }
      bmdx_shm::rfifo_nbl11& rb = so->ringbuf;
      while (lq.navl() > 0)
      {
        cref_t<t_stringref>& data = lq.front();
        if (data && data->is_valid() && data->n() + __shmqueue_ms_nbytes_overhead <= rb.n()) { if (rb.nfree() < data->n()) { break; }  rb.push(data->pd(), data->n()); }
        lq.pop_1();
      }
      if (added.n() > 0)
      {
        s_ll nb_added = added.n() * __shmqueue_ms_nbytes_overhead;
        const s_ll nrb = rb.n();
        for (s_ll i = 0; i < added.n(); ++i)
        {
          if (!__check_for_make_md(nrb, added[i])) { return -3; }
          nb_added += added[i].n();
        }
        if (rb.nfree() < nb_added) { return -3; }
        for (s_ll i = 0; i < added.n(); ++i)
        {
          _shmqms_header header; s_long checksum = 0;
          __make_md(nrb, added[i], header, checksum); // will succeed, checked above (__check_for_make_md)
          rb.push((const char*)&header, 8);
          rb.push(added[i].pd(), added[i].n());
          rb.push((const char*)&checksum, 4);
        }
        return 2;
      }
    } catch (...) {}
    return lq.navl() > 0 ? 1 : 2;
  }
};

template<class AuxData, class _bs = bmdx_meta::nothing>
struct shmqueue_ms_receiver_t // shared memory queue for multiple senders - single receiver side
{
  typedef arrayref_t<char> t_stringref;
  typedef typename shmqueue_ms_sender_t<AuxData, _bs>::_rso _rso;

  const bmdx_shm::t_name_shm qname;
  bmdx_shm::shmobj2s_t<_rso> so;
  vec2_t<cref_t<t_stringref>, _bs> vdd;
  AuxData auxd_init; // used to init _rso::auxd when shared memory becomes available

  shmqueue_ms_receiver_t(const bmdx_shm::t_name_shm& name_, const s_ll nbdflt_ = -1, const AuxData& auxd_init_ = AuxData() __bmdx_noarg)
    : qname(name_), so(name_.n() ? name_.c_str() : "_", true, bmdx_minmax::myllmax(__shmqueue_ms_nbytes_min, nbdflt_), __shmqueue_ms_buf_uid)
  { std::memcpy(&auxd_init, &auxd_init_, sizeof(AuxData)); vdd.vec2_setf_can_shrink(false); }

    // Attempts to connect to shared memory,
    //    permanently lock the shared object, and initialize it if not yet.
    //  *auxd_init: used at auxd object construction.
    //    bytewise copied into auxd field of the shared object .
    //    If auxd_init == 0, all auxd field bytes of the shared object are set to 0 on initialization.
    //  Returns:
    //    2 - success, the shared object is available (just created), no checks needed.
    //    1 - success, connected to existing shared object. The object was already initialized by someone.
    //        The client should check shared object's compatibility before any use.
    //        All succeeding calls to attempt_init() will return 2.
    //        If b_close_if_unchecked == true, attempt_init will return -4 instead of 1 (for use in mpop_*).
    //    -2 - failure.
    //    -3 - couldn't get the shared object side 1 lock.
    //    -4 - the shared queue exists, but not accessible yet - try again later.
  s_long attempt_init(bool b_close_if_unchecked = false)
  {
    if (so.f_constructed() == 1) { return 2; }
    if (qname.n() <= 0) { return -2; }
    s_long res = so.prepare(true);
    if (res == 1)
    {
      if (so.nb() < __shmqueue_ms_nbytes_min) { so.close(); return -2; }
      s_long res2 = so.f_constructed();
        if (res2 == 1) { if (b_close_if_unchecked) { so.close(); return -4; } return 1; }
      so.p()->ringbuf.init_ref(so.nb() - s_ll(sizeof(_rso) - bmdx_shm::rfifo_nbl11::n0));
      std::memcpy(&so.p()->auxd(), &auxd_init, sizeof(AuxData));
      so.set_f_constructed(1);
      return 2;
    }
    if (res == -3 || res == -4) { return res; }
    return -2;
  }

  void close_shm() { so.close(); }

    //  mpop_1: pop one message if it exists - put into dd[0].
    //  mpop_avl: pop max. possible number of existing messages.
    //  Returns:
    //    1 - success, got 1 message (mpop_1) or >= 1 msgs. (mpop_avl). dd contains the messages.
    //    0 - the shared queue is available, but it's empty.
    //    -2 - failure.
    //    -3 - couldn't get the shared object side 1 lock.
    //    -4 - the shared queue exists, but not accessible yet - try again later.
  s_long mpop_1()
  {
    vdd.clear();
    s_long res = attempt_init(true); if (res < 1) { return res; }
    res = _pop_next();
    return res;
  }
  s_long mpop_avl()
  {
    vdd.clear();
    s_long res = attempt_init(true); if (res < 1) { return res; }
    while (true) { res = _pop_next(); if (res < 1) { break; } }
    if (vdd.n() > 0) { return 1; }
    return res;
  }
private:
    // 1 - got one message, 0 - the queue is empty, -2 - failure, -3 - couldn't get side 1 lock, -4 - the queue is not accessible yet
  s_long _pop_next()
  {
    if (so.f_constructed() != 1) { return -1; }
    try {
      bmdx_shm::rfifo_nbl11& rb = so->ringbuf;
      const s_ll ipu = rb.ipush();
      const s_ll ipo = rb.ipop();
      if (ipu < ipo + __shmqueue_ms_nbytes_overhead) { return 0; }
      _shmqms_header header;
      rb.pop((char*)&header, 8, false);
      header.length = bmdx_str::words::be4(&header.length, 0);
      header.lhash = bmdx_str::words::be4(&header.lhash, 0);
      if (!(header.length >= 0 && header.lhash == _shmqms_hash_len(header.length) && ipo + header.length <= ipu)) { rb._rcv_set_ipop(ipu); return -2; } // recovery: completely ignore all existing input, try to start later from the next input
      const s_long nbdata = header.length - __shmqueue_ms_nbytes_overhead;
      cref_t<t_stringref> rdata = bmdx_shm::shmqueue_s::make_rba_z(nbdata, 1); if (!rdata) { return -2; }
      if (!vdd.el_append(rdata)) { return -2; }
      rb._rcv_set_ipop(ipo + 8);
      char* pd = rdata->pd();
      s_long checksum = 0, checksum2 = 0;
      if (nbdata > 0) { rb.pop(pd, nbdata, true, &checksum); }
      rb.pop((char*)&checksum2, 4, true); checksum2 = bmdx_str::words::be4(&checksum2, 0);
      if (checksum2 != checksum) { vdd.el_remove_last(); return -2; }
      return 1;
    } catch (...) {}
    return -2;
  }
};


#if bmdx_part_dispatcher_mt

#define __bmdx_dispatcher_name_prefix "bmdx_disp"
#define __bmdx_disp_lq_cleanup_dtms 1500

#define __lm_slot_controller_name_prefix "bmdx_lmsc"
#define __lm_slot_controller_user_msg_max_ver 1
#define __lm_slot_controller_nbmax_peer_name_root 100 // should be less than ( min(MAX_PATH, capacity(bmdx_shm::t_name_shm))  - (8 + len(__lm_slot_controller_name_prefix)) ) / 2

#define __lm_slot_controller_reinit_short_dtms 50
#define __lm_slot_controller_reinit_long_dtms 400
#define __lm_slot_controller_peer_reinit_dtms 50
#define __lm_slot_controller_sleep_halfen_dtms 40 // should be <= 0.25 * __lm_slot_controller_qum_send_avl_wait_toms

#define __lm_slot_controller_qum_send_avl_wait_toms 400
#define __lm_slot_controller_qum_send_avl_chk_dtms 5
#define __lm_slot_controller_peer_prc_hang_toms 3000 // should be much longer than single LMSC det_periodic() call on high load
#define __lm_slot_controller_thread_hang_toms 3000 // -"-

#define __lm_slot_controller_nb_rqum_min (__bmdx_shmfifo_nbytes_dflt / 10)
#define __lm_slot_controller_nb_rqum_max (__bmdx_shmfifo_msg_nbytes_max)
#define __lm_slot_controller_nb_rqum_dflt (__bmdx_shmfifo_nbytes_dflt)

#define __lm_slot_controller_nb_rqinfo_min (__shmqueue_ms_nbytes_min)
#define __lm_slot_controller_nb_rqinfo_max (__bmdx_shmfifo_msg_nbytes_max / 10)
#define __lm_slot_controller_nb_rqinfo_dflt (100 * 1024)

namespace
{
  static const s_long _fl_msend_anlo_msg = dispatcher_mt_flags::anlo_msg;
  static const s_long _fl_msend_anlo_att = dispatcher_mt_flags::anlo_att;
  static const s_long _fl_msend__mask_clt_msend = dispatcher_mt_flags::ignore_hanged | dispatcher_mt_flags::use_chsbin;
}

using yk_c::hashx;
typedef hashx<s_ll, cref_t<i_dispatcher_mt::tracking_info> > t_htracking_proxy; // { msg. or subscription id, state }
struct t_mtrk_sender_info
{
  weakref_t<t_htracking_proxy> rhtrprx;
  s_ll ipush_shmlq;

  t_mtrk_sender_info() : ipush_shmlq(-1) {}
};
struct t_mtrk_sender_info_id_list
{
  t_mtrk_sender_info_id_list(__bmdx_noarg1) {}
  t_mtrk_sender_info si;
  typedef i_dispatcher_mt::tracking_info tracking_info;
  vec2_t<tracking_info> _vti;
};
struct t_mtrk_qipush_entry
{
  s_ll id_msg;
  s_ll ipush_shmlq;
  t_mtrk_qipush_entry() { id_msg = ipush_shmlq = -1; }
  t_mtrk_qipush_entry(s_ll id_msg_, s_ll ipush_shmlq_) { id_msg = id_msg_; ipush_shmlq = ipush_shmlq_; }
};
} // namespace bmdx_main_intl_lib
namespace yk_c
{
  template<> struct vecm::spec<bmdx_main_intl_lib::t_mtrk_sender_info> { typedef bmdx_main_intl_lib::t_mtrk_sender_info t; typedef config_t<t, 1, 4, 1> config; };
  template<> struct vecm::spec<bmdx_main_intl_lib::t_mtrk_sender_info_id_list> { typedef bmdx_main_intl_lib::t_mtrk_sender_info_id_list t; typedef config_t<t, 1, 4, 1> config; };
  template<> struct vecm::spec<bmdx_main_intl_lib::t_mtrk_qipush_entry> { typedef bmdx_main_intl_lib::t_mtrk_qipush_entry t; typedef config_t<t, 1, 4, 1> config; };
}
namespace bmdx_main_intl_lib
{
namespace
{

  // Creates the given name's representation, suitable for global objects (UTF-8-encoded, limited to bmdx_shm::t_name_shm capacity).
static bmdx_shm::t_name_shm make_fixed_utf8_name(arrayref_t<wchar_t> name) __bmdx_noex { return bmdx_shm::t_name_shm(name.pd(), name.n()); }
static bool is_arch_le() __bmdx_noex { s_long q = 1; return *(unsigned char*)&q == 1; }

//========
//Structure of plain bytearray, passed by dispatcher controllers via IPC and network:
//  (header, data)
//  header:
//    uint2 parts:
//      bit 0 (1) - metadata (md) and its checksum (chsmd) present
//      bit 1 (2) - main (informational) message part (msg) and its checksum (chsmsg) present
//      bit 2 (4) - optional custom part (cus) present
//      bit 3 (8) - custom part checksum (chscus) present
//      bit 4 (16) - optional binary attachment (bin) present
//      bit 5 (32) - binary attachment checksum (chsbin) present
//      bit 6 (64) - msg encoding: 1 - UTF-8, 0 - UTF-16LE
//      bit 7 (128) - 1-byte padding is inserted between md and msg parts (even if any of them is absent)
//      bit 8..15 - reserved, set to 0
//    optional (length3 nmd, uint4 chsmd)
//    optional (length5 nmsg, uint4 chsmsg) (NOTE normally, msg is expected to be 1 kB .. 10 MB long, max. len. 2^40-1 is expected to be technically limited)
//    optional length5 ncus (NOTE max. len. 2^40-1 is expected to be technically limited)
//    optional uint6 chscus
//    optional length5 nbin (NOTE max. len. 2^40-1 is expected to be technically limited)
//    optional uint6 chsbin
//    uint2 - chshdr - checksum of whole header, i.e. [parts..chsnbin]
//  data:
//    optional md
//    optional uchar padding1 = 0: if msg part is present, may (not must) be inserted to make UTF-16 msg start from even address
//    optional msg
//    optional uchar cus[ncus]
//    optional uchar bin[nbin]
//========
//Types:
//  uchar: 8-bit unsigned char
//  length<N>: uchar[N], BE unsigned (8*N)-bit integer
//  uint<N>: uchar[N], BE unsigned (8*N)-bit integer
//========
//md (list of chunks with extensible structure):
//  uint4 mdt:
//    bits 0..30: metadata type; a range of values may be defined to store short data in the type field
//    bit 31: following metadata contents: 1 - present, 0 - absent
//  optional (length3 ndata, uchar data[ndata]) - metadata contents, (ndata == nmd - 4 in single-part metadata, ndata < nmd - 4 in multipart metadata), data format depends on type
//  optional (uint4 mdt2, (length3 ndata2, uchar data2[ndata2])) - next chunk of multipart metadata
//  ...
//========
//Other parts:
//  msg: uchar data[nmsg] - complete message wide string in paramline format, characters encoding is as header.parts bit 6
//  cus: uchar data[ncus] - complete array of user-defined values
//  bin: uchar data[nbin] - complete array of binary attachment
struct netmsg_header
{
  typedef i_dispatcher_mt::t_stringref t_stringref;
private:
  template<class t_fld> void _fld_decode_dref(const char*& ph, t_stringref& z, s_ll& off, s_ll& ndleft, const char*& pd, const char* const orig_pd) const        { z.link(pd, ((t_fld*)ph)->value()); ph += sizeof(t_fld); off = pd - orig_pd; ndleft -= z.n(); pd += z.n(); }
  template<class t_fld, class t_dest> void _fld_decode_value(const char*& ph, t_dest& z) const        { z = ((const t_fld*)ph)->value(); ph += sizeof(t_fld); }
public:
  typedef unsigned char uchar;
  struct length3 { uchar n[3]; length3() { n[0] = 0; n[1] = 0; n[2] = 0; } bmdx_meta::u_long value() const { bmdx_meta::u_long q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; return q; } void set(bmdx_meta::u_long x) { n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct length5 { uchar n[5]; length5() { n[0] = 0; n[1] = 0; n[2] = 0; n[3] = 0; n[4] = 0; } bmdx_meta::u_ll value() const { bmdx_meta::u_ll q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; q <<= 8; q |= n[3]; q <<= 8; q |= n[4]; return q; } void set(bmdx_meta::u_ll x) { n[4] = (uchar)x; x >>= 8; n[3] = (uchar)x; x >>= 8; n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct uint1 { uchar n[1]; uint1() { n[0] = 0; }                            uchar value() const { return n[0]; } void set(bmdx_meta::u_long x) { n[0] = (uchar)x; } };
  struct uint2 { uchar n[2]; uint2() { n[0] = 0; n[1] = 0; }                            bmdx_meta::u_long value() const { bmdx_meta::u_long q = n[0]; q <<= 8; q |= n[1]; return q; } void set(bmdx_meta::u_long x) { n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct uint4 { uchar n[4]; uint4() { *(bmdx_meta::u_long*)this = 0; }                        bmdx_meta::u_long value() const { bmdx_meta::u_long q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; q <<= 8; q |= n[3]; return q; } void set(bmdx_meta::u_long x) { n[3] = (uchar)x; x >>= 8; n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct uint6 { uchar n[6]; uint6() { n[0] = 0; n[1] = 0; n[2] = 0; n[3] = 0; n[4] = 0; n[5] = 0; }   bmdx_meta::u_ll value() const { bmdx_meta::u_ll q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; q <<= 8; q |= n[3]; q <<= 8; q |= n[4]; q <<= 8; q |= n[5]; return q; } void set(bmdx_meta::u_ll x) { n[5] = (uchar)x; x >>= 8; n[4] = (uchar)x; x >>= 8; n[3] = (uchar)x; x >>= 8; n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct uint8 { uchar n[8]; uint8() { *(bmdx_meta::u_ll*)this = 0; }                                           bmdx_meta::u_ll value() const { bmdx_meta::u_ll q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; q <<= 8; q |= n[3]; q <<= 8; q |= n[4]; q <<= 8; q |= n[5]; q <<= 8; q |= n[6]; q <<= 8; q |= n[7]; return q; } void set(bmdx_meta::u_ll x) { n[7] = (uchar)x; x >>= 8; n[6] = (uchar)x; x >>= 8; n[5] = (uchar)x; x >>= 8; n[4] = (uchar)x; x >>= 8; n[3] = (uchar)x; x >>= 8; n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };
  struct sint8 { uchar n[8]; sint8() { *(s_ll*)this = 0; }                                           s_ll value() const { s_ll q = n[0]; q <<= 8; q |= n[1]; q <<= 8; q |= n[2]; q <<= 8; q |= n[3]; q <<= 8; q |= n[4]; q <<= 8; q |= n[5]; q <<= 8; q |= n[6]; q <<= 8; q |= n[7]; return q; } void set(s_ll x) { n[7] = (uchar)x; x >>= 8; n[6] = (uchar)x; x >>= 8; n[5] = (uchar)x; x >>= 8; n[4] = (uchar)x; x >>= 8; n[3] = (uchar)x; x >>= 8; n[2] = (uchar)x; x >>= 8; n[1] = (uchar)x; x >>= 8; n[0] = (uchar)x; } };

  typedef uint2 t_parts; typedef uint2 t_chshdr;
  typedef length3 t_nmd; typedef uint4 t_chsmd;
  typedef length5 t_nmsg; typedef uint4 t_chsmsg;
  typedef length5 t_ncus; typedef uint6 t_chscus;
  typedef length5 t_nbin; typedef uint6 t_chsbin;

  typedef uint4 t_md1_type;
  typedef length3 t_nmd1;
  struct t_md1_hdr_max { t_md1_type t; t_nmd1 n; };

  enum Epart_flags { fpt_md = 1, fpt_msg = 2, fpt_cus = 4, fpt_chscus = 8, fpt_bin = 16, fpt_chsbin = 32, fl_msg_utf8 = 64, fl_padding1 = 128  };

  s_long parts;
    bool has_md() const { return !!(parts & fpt_md); }
    bool has_msg() const { return !!(parts & fpt_msg); }
    bool has_cus() const { return !!(parts & fpt_cus); }
    bool has_chscus() const { return !!(parts & fpt_chscus); }
    bool has_bin() const { return !!(parts & fpt_bin); }
    bool has_chsbin() const { return !!(parts & fpt_chsbin); }
    bool b_msg_utf8() const { return !!(parts & fl_msg_utf8); }
    bool b_padding1() const { return !!(parts & fl_padding1); }
  t_stringref md, msg, cus, bin;
  bmdx_meta::u_long chshdr, chsmd, chsmsg;
  s_ll chscus, chsbin, off_md, off_msg, off_cus, off_bin;

  netmsg_header() { fields_clear(); }
  void clear() { fields_clear(); }
  void fields_clear() { parts = 0; md.clear(); msg.clear(); cus.clear(); bin.clear(); chshdr = chsmd = chsmsg = 0; chscus = chsbin = off_md = off_msg = off_cus = off_bin = 0; }
  static inline s_long nbhdr(s_long fl_parts)
  {
    s_long nh = sizeof(t_parts) + sizeof(t_chshdr);
      if (fl_parts & fpt_md) { nh += sizeof(t_nmd) + sizeof(t_chsmd); }
      if (fl_parts & fpt_msg) { nh += sizeof(t_nmsg) + sizeof(t_chsmsg); }
      if (fl_parts & fpt_cus) { nh += sizeof(t_ncus); }
      if (fl_parts & fpt_chscus) { nh += sizeof(t_chscus); }
      if (fl_parts & fpt_bin) { nh += sizeof(t_nbin); }
      if (fl_parts & fpt_chsbin) { nh += sizeof(t_chsbin); }
    return nh;
  }
    // Decodes orig's header and verifies checksums as necessary.
    //  fl_chkdchs: bits are same a in .parts: if the bit set, verify data checksum if it's available (note that header checksum is verified anyway)
    // Returns:
    //    true - decoding successful, the specified checksums are OK
    //    false - logical error during decoding, or any of the specified checksums does not match with that in orig; *this is not modified
  bool fields_decode(const t_stringref& orig, s_long fl_chkdchs = fpt_md|fpt_msg|fpt_cus|fpt_bin)
  {
    netmsg_header h2; bool b = false;
    do { // once
      if (orig.n() < s_ll(sizeof(t_parts) + sizeof(t_chshdr))) { break; }
      const char* ph = orig.pd();
        if (1) { this->_fld_decode_value<t_parts>(ph, h2.parts); }
      const s_long nh = nbhdr(h2.parts);
      s_ll ndleft = orig.n() - nh;
      if (ndleft < 0) { break; }

      if (1) { const uchar* ph = (uchar*)orig.pd(); const uchar* const ph2 = ph + nh - sizeof(t_chshdr); bmdx_meta::u_long chs = 0; while (ph != ph2) { chs += *ph++; } h2.chshdr = ((const t_chshdr*)ph)->value(); if (chs != h2.chshdr) { break; } }
      const char* pd = orig.pd() + nh;
        if (h2.has_md()) { this->_fld_decode_dref<t_nmd>(ph, h2.md, h2.off_md, ndleft, pd, orig.pd()); }
        if (h2.has_md()) { this->_fld_decode_value<t_chsmd>(ph, h2.chsmd); }
        if (h2.b_padding1()) { pd += 1; ndleft -= 1; }
        if (h2.has_msg()) { this->_fld_decode_dref<t_nmsg>(ph, h2.msg, h2.off_msg, ndleft, pd, orig.pd()); }
        if (h2.has_msg()) { this->_fld_decode_value<t_chsmsg>(ph, h2.chsmsg); }
        if (h2.has_cus()) { this->_fld_decode_dref<t_ncus>(ph, h2.cus, h2.off_cus, ndleft, pd, orig.pd()); }
        if (h2.has_chscus()) { this->_fld_decode_value<t_chscus>(ph, h2.chscus); }
        if (h2.has_bin()) { this->_fld_decode_dref<t_nbin>(ph, h2.bin, h2.off_bin, ndleft, pd, orig.pd()); }
        if (h2.has_chsbin()) { this->_fld_decode_value<t_chsbin>(ph, h2.chsbin); }
      if (ndleft < 0) { break; }

      if ((fl_chkdchs & fpt_md) && h2.has_md() && !chkchs(h2.md, h2.chsmd)) { break; }
      if ((fl_chkdchs & fpt_msg) && h2.has_msg() && !chkchs(h2.msg, h2.chsmsg)) { break; }
      if ((fl_chkdchs & fpt_cus) && h2.has_cus() && h2.has_chscus() && !chkchs(h2.cus, h2.chscus)) { break; }
      if ((fl_chkdchs & fpt_bin) && h2.has_bin() && h2.has_chsbin() && !chkchs(h2.bin, h2.chsbin)) { break; }
      parts = h2.parts; md = h2.md; msg = h2.msg; cus = h2.cus; bin = h2.bin; chshdr = h2.chshdr; chsmd = h2.chsmd; chsmsg = h2.chsmsg; chscus = h2.chscus; chsbin = h2.chsbin; off_md = h2.off_md; off_msg = h2.off_msg; off_cus = h2.off_cus; off_bin = h2.off_bin;
      b = true;
    } while (false);
    return b;
  }

  static bool chkchs(t_stringref x, s_ll chs) { if (!x.is_valid()) { return false; } const uchar* p = (uchar*)x.pd(); const uchar* const p2 = p + x.n(); s_ll chs2 = 0; while (p != p2) { chs2 += *p++; } return chs2 == chs; }
  static s_ll calcchs(t_stringref x) { if (!x.is_valid()) { return 0; } const uchar* p = (uchar*)x.pd(); const uchar* const p2 = p + x.n(); s_ll chs2 = 0; while (p != p2) { chs2 += *p++; } return chs2; }
  static s_ll calcchs(void* pd, s_ll n) { return calcchs(t_stringref((char*)pd, n)); }

  struct msg_builder;
  struct mdd_deliv_reply { enum  { stype = 51 }; uint8 id_object; sint8 res; uint1 flags; s_long type() const { return stype; } operator char*() { return (char*)this; } };
  struct mdd_subs_request
  {
    enum  { stype = 52 };
    s_long type() const { return stype; }

    s_long rt; // valid value: 1..6, dflt. -1
    s_ll id_trk;
    s_long nb_qsa, nb_suba;
    void* pmsg;

    mdd_subs_request() { clear(); }
    void clear() { rt = -1; id_trk = -1; nb_qsa = nb_suba = 0; pmsg = 0; }

      // Message structure (influences is_valid, read_msg, pqsa_utf8, psuba_utf8, set_msg):
      //        uint1 rt
      //            1..3: type of query to qs, see _s_subscribe.
      //            4..6: type of query to subscribed slot, analogous to (1..3) for qs.
      //        uint8 id_trk
      //        length3 nb_qsa
      //        length3 nb_suba
      //        uchar qsa_utf8[nb_qsa] - qs slot address (converted to LP type)
      //        uchar suba[nb_suba] - subscribed slot address (converted to LM type)
    enum { nb_fixed = sizeof(uint1) + sizeof(uint8) + 2 * sizeof(length3) };

    bool is_valid() const { return rt >= 1 && rt <= 6 && id_trk != -1 && nb_qsa > 0 && nb_suba > 0 && (nb_fixed + nb_qsa + nb_suba) < (1 << sizeof(t_nmd) * 8) && !!pmsg; }

    bool read_msg(t_stringref msg)
    {
      clear();
      if (!(msg.is_valid() && msg.n() >= nb_fixed)) { return false; }
      const char* p = msg.pd();
      if (1) { typedef uint1 t_field; rt = ((t_field&)*p).value(); p += sizeof(t_field); }
      if (1) { typedef uint8 t_field; id_trk = ((t_field&)*p).value(); p += sizeof(t_field); }
      if (1) { typedef length3 t_field; nb_qsa = ((t_field&)*p).value(); p += sizeof(t_field); }
      if (1) { typedef length3 t_field; nb_suba = ((t_field&)*p).value(); p += sizeof(t_field); }
      if (nb_fixed + nb_qsa + nb_suba != msg.n()) { return false; }
      pmsg = msg.pd();
      return is_valid();
    }
      // != 0 on is_valid() == true.
    uchar* pqsa_utf8() const { if (!pmsg) { return 0; } return (uchar*)pmsg + nb_fixed; }
      // != 0 on is_valid() == true.
    uchar* psuba_utf8() const { if (!pmsg) { return 0; } return (uchar*)pmsg + nb_fixed + nb_qsa; }

      // rt:
      //    1..3: type of query to qs, see _s_subscribe.
      //    4..6: type of query to subscribed slot, analogous to (1..3) for qs.
    static cref_t<netmsg_header::msg_builder> make_msg(s_long rt, s_ll id_trk, arrayref_t<wchar_t> qsa, arrayref_t<wchar_t> suba)
    {
      cref_t<netmsg_header::msg_builder> rmb;
      if (!(rt >= 1 && rt <= 6)) { return rmb; }
      if (!(qsa.is_valid() && qsa.n() > 0 && suba.is_valid() && suba.n() > 0)) { return rmb; }
      carray_r_t<char> d;
      try {
        std::string qsa_s(bmdx_str::conv::wsbs_utf8(qsa.pd(), qsa.n()));
        std::string suba_s(bmdx_str::conv::wsbs_utf8(suba.pd(), suba.n()));
        if (!(qsa_s.size() > 0 && qsa_s.size() < 0x1fffff && suba_s.size() > 0 && suba_s.size() < 0x1fffff)) { return rmb; }
        s_long nb_qsa = (s_long)qsa_s.size();
        s_long nb_suba = (s_long)suba_s.size();
        if (!d.resize(nb_fixed + nb_qsa + nb_suba, false, true)) { return rmb; }
        char* p = d.pd();
        if (1) { typedef uint1 t_field; ((t_field&)*p).set(rt); p += sizeof(t_field); }
        if (1) { typedef uint8 t_field; ((t_field&)*p).set(id_trk); p += sizeof(t_field); }
        if (1) { typedef length3 t_field; ((t_field&)*p).set(nb_qsa); p += sizeof(t_field); }
        if (1) { typedef length3 t_field; ((t_field&)*p).set(nb_suba); p += sizeof(t_field); }
        bmdx_str::words::memmove_t<char>::sf_memcpy(p, qsa_s.c_str(), nb_qsa); p += nb_qsa;
        bmdx_str::words::memmove_t<char>::sf_memcpy(p, suba_s.c_str(), nb_suba); p += nb_suba;
        qsa_s.clear(); suba_s.clear();
        if (!(rmb.create2(1, fpt_md, sizeof(netmsg_header::t_md1_hdr_max) + d.n()) && rmb->set_md1(stype, d) && rmb->set_final())) { rmb.clear(); }
      } catch (...) {}
      return rmb;
    }
  };
  struct mdd_subs_reply { enum  { stype = 53 }; uint8 id_object; sint8 res; s_long type() const { return stype; } operator char*() { return (char*)this; } };
  struct mdd_id_msg { enum  { stype = 54 }; uint8 id_msg; uint1 flags; s_long type() const { return stype; } operator char*() { return (char*)this; } };
  struct mdd_id_msg_cmd { enum  { stype = 55 }; uint8 id_msg; s_long type() const { return stype; } operator char*() { return (char*)this; } };

  struct md_entry
  {
      // 31-bit metadata type.
      //  If entry contains metadata, bit 31 == 0, i.e. type >= 0; dflt. type == -1 (no entry).
      // Available types and their formats:
      //  type (1, 2): request for making input queue for messages.
      //      type 1 states that this is the first request from the peer process.
      //      type 2 states that this request is a hint for shared queues ownership regaining (may appear after the current process restart).
      //      data: uchar name_peerf[data.n()]: make_fixed_utf8_name of peer's dispatcher process name (one that has sent the current message)
      //  type [3..50]: user message format version id; no data field
      //  type 51: peer process returns user message receival status
      //      data:
      //        uint8 id_msg
      //        sint8 result_code (one of related i_dispatcher_mt::msend result codes)
      //        uint1 flags: see type 54
      //  type 52: peer process requests _s_subscribe
      //      data: see struct mdd_subs_request
      //  type 53: peer process returns the result of _s_subscribe
      //  type 54: user msg id with requirements for delivery report
      //      data:
      //        uint8 id_msg
      //        uint1 ORed flags of delivery report requirements (1 - for tracking, 2 - for command slot phase change)
      //  type 55: user command msg id (for being sent back as part of command response)
      //      data: uint8 id_msg
    s_long type;
    t_stringref data;
    md_entry() { type = -1; }
    bool b_type(s_long t) const { return type == t; }
    bool b_type(s_long t1, s_long t2) const { return type == t1 || type == t2; }
    template<class T> bool b_valid_size() { return data.n() == sizeof(T); }
  };
    // Get metadata parts.
    //    *this must be valid object, pointing to valid original data (see also fields_decode).
    //    b_full_scan: true - count (loop through) all parts, false - count only parts, fitting into dest_mde.n().
    // Returns:
    //    >= 0 - the full number of parts available.
    //      May be > dest_mde.n().
    //      In dest_mde, min(dest_mde.n(), ret. val.) entries are set to parts data. The rest of them (if any) are not modified.
    //    -2 - failure.
  s_long get_mde(arrayref_t<md_entry> dest_mde, bool b_full_scan)
  {
    if (!dest_mde.is_valid()) { return -2; }
    if (!has_md()) { return 0; }
    if (!md.is_valid()) { return -2; }
    const char* pd = md.pd(); s_ll n = md.n();
    s_long imde = 0;
    while (n >= s_ll(sizeof(t_md1_type)))
    {
      if (!b_full_scan && imde >= dest_mde.n()) { break; }
      bmdx_meta::u_long t = ((const t_md1_type*)pd)->value();
      pd += sizeof(t_md1_type); n -= sizeof(t_md1_type);
      const char* pdata = 0; s_long ndata = 0;
      if (t & 0x80000000l)
      {
        t &= 0x7fffffffl;
        if (n < s_ll(sizeof(t_nmd1))) { break; }
        ndata = ((const t_nmd1*)pd)->value();
        pd += sizeof(t_nmd1); n -= sizeof(t_nmd1);
        if (n < ndata) { break; }
        pdata = pd;
        pd += ndata; n -= ndata;
      }
      if (imde < dest_mde.n())
      {
        dest_mde[imde].type = t;
        if (pdata) { dest_mde[imde].data.link(pdata, ndata); } else { dest_mde[imde].data.clear(); }
      }
      ++imde;
    }
    return imde;
  }

  struct msg_builder
  {
    cref_t<t_stringref> msg;

    typedef carray_r_t<char> t_rstr_ct;

    msg_builder(s_long fl_parts, s_ll nbrsv = 0) { reset(fl_parts, nbrsv); }

    bool b_valid() const { return msg && pa; } // returns true if the object is valid and contains incomplete or complete message
    bool b_complete() const { return b_valid() && !!msg->pd(); } // returns true if complete message exists in msg
    bmdx_meta::u_long parts() const { return pparts ? pparts->value() : 0; }

      // true if reset successfully, false if failed (in this case, b_nem() == false).
      //  nbrsv > 0 reserves in data array, in addition to header, nbrsv bytes, to later faultless message parts adding.
    bool reset(s_long fl_parts, s_ll nbrsv = 0)
    {
      msg.clear();
      pa = 0; pparts = 0; pchshdr = 0; pnmd = 0; pnmsg = 0; pncus = 0; pnbin = 0;
      _nbhdr = nbhdr(fl_parts);
      max_part = 0; chsmd = 0;

      typedef cref_t<t_stringref>::iref2<t_stringref, t_rstr_ct> t_iref2;
      msg = t_iref2::create0(1, 0, 0x14 | iref2_flags::use_aux, t_iref2::handler_dflt, iref2_args_t<t_rstr_ct>::args(), sizeof(t_rstr_ct));
      pa = (t_rstr_ct*)msg.paux();
      if (!pa) { msg.clear(); return false; }
      if (nbrsv > 0 && !pa->reserve(_nbhdr + nbrsv)) { pa = 0; msg.clear(); return false; }
      if (!pa->resize(_nbhdr, false, false)) { pa = 0; msg.clear(); return false; }
      pparts = (t_parts*)pa->pd();
      pchshdr = (t_chshdr*)((char*)pa->pd() + _nbhdr - sizeof(t_chshdr));
      char* ph = (char*)pparts + sizeof(t_parts);
      if (fl_parts & fpt_md) { pnmd = (t_nmd*)ph; ph += sizeof(t_nmd); ph += sizeof(t_chsmd); }
      if (fl_parts & fpt_msg) { pnmsg = (t_nmsg*)ph; ph += sizeof(t_nmsg); ph += sizeof(t_chsmsg); }
//      if (fl_parts & fpt_cus) { pncus = (t_ncus*)ph; ph += sizeof(t_ncus); }
//      if (fl_parts & fpt_chscus) { ph += sizeof(t_chscus); }
      if (fl_parts & fpt_bin) { pnbin = (t_nbin*)ph; ph += sizeof(t_nbin); }
      if (fl_parts & fpt_chsbin) { ph += sizeof(t_chsbin); }
      pparts->set(fl_parts);
      return true;
    }

      // Adds 1 metadata entry. May be called multiple times.
      // NOTE according to metadata entry format, set_md1 allocates
      //    1) sizeof(t_md1_type) for metadata type.
      //    2) sizeof(t_nmd1) for storing data1.n() value, or 0 if data1.n() == 0.
      //    3) data1.n() for copy of data1.
    bool set_md1(s_long type, t_stringref data1 = t_stringref())
    {
      if (!data1.is_valid()) { return false; }
      if (!(b_valid() && pnmd && max_part <= fpt_md)) { return false; }
      const bool b_data = data1.n() > 0;
      const s_ll n0 = pa->n();
      const s_ll nmd1 = sizeof(t_md1_type) + (b_data ? sizeof(t_nmd1) + data1.n() : 0);
      const s_ll nmd_total = n0 + nmd1 - _nbhdr;
      if (nmd_total >> sizeof(t_nmd) * 8) { return false; } // total metadata size value may not exceed t_nmd field size (i.e. 24 bits)
      if (!_pa_resize(n0 + nmd1)) { return false; }
      uchar* p0 = (uchar*)(pa->pd() + n0);
      ((t_md1_type*)p0)->set(bmdx_meta::u_long(type) | (b_data ? 0x80000000l : 0)); p0 += sizeof(t_md1_type);
      if (b_data) { ((t_nmd1*)p0)->set(bmdx_meta::u_long(data1.n())); p0 += sizeof(t_nmd1); }
      if (b_data) { bmdx_str::words::memmove_t<char>::sf_memcpy(p0, data1.pd(), data1.n()); p0 += data1.n(); }

      chsmd += s_long(calcchs(pa->pd() + n0, nmd1));
      pnmd[0].set(bmdx_meta::u_long(nmd_total));
      ((t_chsmd&)pnmd[1]).set(chsmd);
      max_part = fpt_md;
      return true;
    }
      // Adds user message.
    bool set_umsg(arrayref_t<wchar_t> data)
    {
      if (!data.is_valid()) { return false; }
      if (!(b_valid() && pnmsg && max_part < fpt_msg)) { return false; }
      if (data.n() >> (sizeof(t_nmsg) * 8 - 4)) { return false; } // fact. msg. length in bytes is limited here to 37-bit unsigned integer.
      s_ll off = pa->n(), nb_add = 0; // both values are adjusted later
      s_long chsmsg = 0;

      if (is_arch_le() && sizeof(wchar_t) == 2) // direct copy of data into message, as UTF-16LE
      {
        bool b_pad = false;
        const s_ll nbmsg = 2 * data.n();
        nb_add = nbmsg;
        if (off & 1) { off += 1; nb_add += 1; b_pad = true; }
        if (!_pa_resize(pa->n() + nb_add)) { return false; }
        if (data.n() > 0) { bmdx_str::words::memmove_t<char>::sf_memcpy(pa->pd() + off, data.pd(), nbmsg, &chsmsg); }
        if (b_pad) { pparts->set(pparts->value() | fl_padding1); }
        pnmsg[0].set(nbmsg);
      }
      else
      {
        cref_t<std::string, cref_nonlock> s_utf8; bool b_lsb = true;
        const wchar_t* pd = data.pd(); const wchar_t* const pd2 = pd + data.n();
        while (pd != pd2) { uchar c1 = (uchar)*pd; uchar c2 = (uchar)(*pd>>8); chsmsg += c1; chsmsg += c2; if (c2 || (c1 & 128)) { b_lsb = false; } ++pd; }
        if (!b_lsb) { try { s_utf8.create1(0, bmdx_str::conv::wsbs_utf8(data.pd(), data.n())); } catch (...) { return false; } }
        if (s_utf8)
        {
          nb_add = s_utf8->size();
          if (!_pa_resize(pa->n() + nb_add)) { return false; }
          bmdx_str::words::memmove_t<char>::sf_memcpy(pa->pd() + off, s_utf8->c_str(), nb_add, &chsmsg); // NOTE checksum is recalculated, because data has been converted
          pparts->set(pparts->value() | fl_msg_utf8);
          pnmsg[0].set(s_utf8->size());
        }
        else // b_lsb == true: simple copy of LSB of each character
        {
          nb_add = data.n();
          if (!_pa_resize(pa->n() + nb_add)) { return false; }
          char* pdest = pa->pd() + off;
          const wchar_t* pd = data.pd(); const wchar_t* const pd2 = pd + data.n();
          while (pd != pd2) { *pdest++ = (char)(uchar)*pd++; }
          pnmsg[0].set(data.n());
        }
      }

      ((t_chsmsg&)pnmsg[1]).set(chsmsg);
      max_part = fpt_msg;
      return true;
    }
      // Sets info on binary attachment into the header (attachment itself is passed to shmqueue_s::msend as separate object, to avoid copying).
    bool set_bin_info(t_stringref bin)
    {
      if (!bin.is_valid()) { return false; }
      if (!(b_valid() && pnbin && max_part < fpt_bin)) { return false; }
      if (bin.n() >> (sizeof(t_nbin) * 8 - 2)) { return false; } // fact. bin. data length is limited here to 38-bit unsigned integer.
      pnbin[0].set(bin.n());
      if (pparts->value() & fpt_chsbin)
      {
        s_ll chsbin = 0;
        const char* pd = bin.pd(); const char* const pd2 = pd + bin.n();
        while (pd != pd2) { chsbin += (uchar)*pd++; }
        ((t_chsbin&)pnbin[1]).set(chsbin);
      }
      max_part = fpt_chsbin; // NOTE no parts can be added later
      return true;
    }
      // Required: completes msg building; anyway succeeds if the message is already non-empty.
      //  NOTE set_final() does not add new data (data array is not reallocated).
      //    If succeeds anyway if b_valid() == true.
    bool set_final()
    {
      if (!b_valid()) { return false; }
      pchshdr->set(bmdx_meta::u_long(calcchs(pa->pd(), (char*)pchshdr - pa->pd())));
      msg->link(pa->pd(), pa->n());
      return true;
    }
  private:
    t_rstr_ct* pa;
      t_parts* pparts; t_chshdr* pchshdr;
      t_nmd* pnmd;
      t_nmsg* pnmsg;
      t_ncus* pncus;
      t_nbin* pnbin;
      s_long _nbhdr, max_part; bmdx_meta::u_long chsmd;
    bool _pa_resize(s_ll n2)
    {
      if (!pa) { return false; }
      char* pd1 = pa->pd();
      if (!pa->resize(n2, true, false)) { return false; }
      const yk_c::meta::t_pdiff delta = pa->pd() - pd1;
      if (delta != 0)
      {
        if (pparts) { *(char**)&pparts += delta; }
        if (pchshdr) { *(char**)&pchshdr += delta; }
        if (pnmd) { *(char**)&pnmd += delta; }
        if (pnmsg) { *(char**)&pnmsg += delta; }
        if (pncus) { *(char**)&pncus += delta; }
        if (pnbin) { *(char**)&pnbin += delta; }
      }
      return true;
    }
  };
};


struct lm_slot_controller // IPC controller for dispatcher_mt
{
  typedef arrayref_t<char> t_stringref;
  typedef i_dispatcher_mt::tracking_info tracking_info;

  enum { fpt_md = netmsg_header::fpt_md, fpt_msg = netmsg_header::fpt_msg, fpt_bin = netmsg_header::fpt_bin, fpt_chsbin = netmsg_header::fpt_chsbin };
  typedef  netmsg_header::mdd_deliv_reply mdd_deliv_reply;
  typedef  netmsg_header::mdd_subs_request mdd_subs_request;
  typedef  netmsg_header::mdd_subs_reply mdd_subs_reply;
  typedef  netmsg_header::mdd_id_msg mdd_id_msg;
  typedef  netmsg_header::mdd_id_msg_cmd mdd_id_msg_cmd;

  struct i_callback
  {
      // See local_write "enc" arg. for comments on ret. val.
    inline static s_long umsg_enc_type(bool b_utf8) __bmdx_noex { if (b_utf8) { return 2; } if (is_arch_le() && sizeof(wchar_t) == 2) { return 1; } return 0; }

      // Ensure calling dispatcher_mt::thread_proxy::_s_write with the correct reference to user message WCS.
      //  umsg: user message in string form.
      //  enc:
      //    2: umsg refers to UTF-8 string, that must be additionally decoded.
      //    1: umsg refers to (as byte array) UTF-16LE string (which is the type of wchar_t string), ready for use.
      //    0: umsg refers to single-byte characters in range 0..127. They should be converted directly into wchar_t.
      //    See also umsg_enc_type.
      //  bin: is non-null, this is binary attachment to message, that should be returned to user as is.
      //  id_msg_cmd: original command message id (for command messages)
      // Returns:
      //  a) result of _s_write,
      //  b) -2 if additional allocations on umsg conversion have failed (before _s_write).
    virtual s_long local_write(t_stringref umsg, s_long enc, const cref_t<t_stringref>& bin, s_ll id_msg, s_ll id_msg_cmd) = 0;

      // mde references metadata entry of type 52 (mdd_subs_request).
      // dm contains the valid decoded header of mde.
      //    Impl. should 1) decode strings from mde, 2) check peer process name matching with name_peerf,
      //      3) call _s_subscribe, and 4) return its result.
    virtual s_long local_subscribe(t_stringref mde, const mdd_subs_request& dm, const bmdx_shm::t_name_shm& name_peerf) = 0;

      // Find command slots for the given list of msg. ids. Set command slot state according to associated delivery state.
      // Returns: 1 - success, -2 - failure.
    virtual s_long cslph_update(vec2_t<tracking_info>& tii) = 0;
    virtual ~i_callback() __bmdx_exany {}
  };

    // name_pr: process_name arg. of dispatcher_mt ctor.
  lm_slot_controller(arrayref_t<wchar_t> name_pr, s_ll nb_qum_, s_ll nb_qinfo_, const unity& apeer_names) __bmdx_noex
  :
    _nb_qum(nb_qum_),
    _fl_chs(fpt_md|fpt_msg),
    _apns(apeer_names),
    _name_prf(make_fixed_utf8_name(name_pr)),
    _qinfo_rcv(make_name_onef(_name_prf), limit_nbqueue(nb_qinfo_, __lm_slot_controller_nb_rqinfo_min, __lm_slot_controller_nb_rqinfo_max, __lm_slot_controller_nb_rqinfo_dflt))
  {
    _state = 0; _tms_next_init = 0; _b_qinfo_rcv_valid = false; _qinfo_rcv_idd0 = 0;
    _hnew_name_peerf.hashx_setf_can_shrink(0);
    _qinfo_rcv.auxd_init.disp_ses_state(-2);
  }
  ~lm_slot_controller() __bmdx_noex { _x_close(); }


private:
  struct _r_prc_uid
  {
    volatile s_ll _sig, _pid, _pid_deriv, _livecnt, _disp_ses_state, _prc_inst_id; // NOTE prc_inst_id must be the last in sequence
    s_ll sig() volatile const { return bmdx_str::words::atomrdal64_g(&_sig); } void sig(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_sig, x); }
    s_ll pid() volatile const { return bmdx_str::words::atomrdal64_g(&_pid); } void pid(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_pid, x); }
    s_ll pid_deriv() volatile const { return bmdx_str::words::atomrdal64_g(&_pid_deriv); } void pid_deriv(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_pid_deriv, x); }
    s_ll livecnt() volatile const { return bmdx_str::words::atomrdal64_g(&_livecnt); } void livecnt(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_livecnt, x); }
    s_ll disp_ses_state() volatile const { return bmdx_str::words::atomrdal64_g(&_disp_ses_state); } void disp_ses_state(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_disp_ses_state, x); }
    s_ll prc_inst_id() volatile const { return bmdx_str::words::atomrdal64_g(&_prc_inst_id); } void prc_inst_id(s_ll x) volatile { bmdx_str::words::atomwral64_g(&_prc_inst_id, x); }

    _r_prc_uid() __bmdx_noex { sig(Fsig()); pid(0); pid_deriv(0); livecnt(0); disp_ses_state(-2); prc_inst_id(0); }

    static inline s_long Fsig() { return yk_c::bytes::cmti_base_t<_r_prc_uid, 2020, 2, 28, 23>::ind() + 1; }

      // NOTE The order of copying in cp_ids_* takes into account the order of async. checking/modifying values by other side.
    void cp_ids_from_volatile(const _r_prc_uid& src, bool b_copy_livecnt)
    {
      s_ll idinst0;
      do {
        idinst0 = src.prc_inst_id();
        sig(src.sig()); pid(src.pid()); pid_deriv(src.pid_deriv());
        if (b_copy_livecnt) { livecnt(src.livecnt()); }
        prc_inst_id(src.prc_inst_id());
      } while (prc_inst_id() != idinst0);
    }
    void cp_ids_to_volatile(const _r_prc_uid& src, bool b_copy_livecnt)
    {
      s_ll idinst_temp = -1; if (idinst_temp == src.prc_inst_id()) { idinst_temp = -2; }
      prc_inst_id(idinst_temp);
      sig(src.sig()); pid(src.pid()); pid_deriv(src.pid_deriv());
      if (b_copy_livecnt) { livecnt(src.livecnt()); }
      prc_inst_id(src.prc_inst_id());
    }

    bool b_same_inst(const _r_prc_uid& src) const { return src.pid() == pid() && src.pid_deriv() == pid_deriv() && src.prc_inst_id() == prc_inst_id(); }

  };
  static s_ll pid_deriv_v1(s_ll pid) { return pid; }
  enum { nmdemax = 3 };
  inline static s_ll limit_nbqueue(s_ll nb_orig, s_ll nbmin, s_ll nbmax, s_ll nbdflt) __bmdx_noex { if (nb_orig < 0) { nb_orig = nbdflt; } return bmdx_minmax::myllrange_ub(nb_orig, nbmin, nbmax); }
public:
    // Peer process activity tracker; one for each working dispatcher_mt in other processes, with which IPC occurs.
  struct peer_tracker
  {
    inline peer_tracker(const bmdx_shm::t_name_shm& name_prf_, const bmdx_shm::t_name_shm& name_peerf_, s_ll nb_qum_) __bmdx_noex;
    inline ~peer_tracker() __bmdx_noex { _cltpt_close(true); }
    inline s_long state() const __bmdx_noex { return _state; }
    inline s_ll id_peer() const __bmdx_noex { return _id_peer; } // for local process use only

  private:
    //================================
      // for locking 1) _qum_send read/modify/push, 2) mtrk_*_send modify, 3) inside _x_init, _cltpt_ensure_comm.
      // NOTE lkd_pt_access must always be set without or after lkd_peer_hpt.
    critsec_t<lm_slot_controller>::csdata lkd_pt_access;
    //================================
    const bmdx_shm::t_name_shm _name_prf, _name_peerf;
    const bool b_self; // = _name_prf == _name_peerf
    s_ll _id_peer;
    double _tm_lastinit, _tm_mtrk_nem;
    volatile s_long _state; // 1 ready; 0 not initialized yet; -2 init. failed, will be tried again later; -3 closing; -4 already closed
    volatile bool _b_clt_mdsend_wait_once;
    //================================
    shmqueue_ms_sender_t<_r_prc_uid, __vecm_tu_selector> _qinfo_send; // shared queue for sending informational messages to peer process
    bmdx_shm::shmqueue_s _qum_rcv; // shared queue for input user messages from peer process; serviced in LMSC det_periodic()
    bmdx_shm::shmqueue_s _qum_send; // shared queue for sending user messages to peer process; external client (dispatcher_mt) may only push messages in b_state() 1; locking: lkd_pt_access
    //================================
    _r_prc_uid _qinfo_ids; // actual peer IDs copy (non-volatile)
    double _qinfo_livecnt_tm;
    bool _qinfo_peer_conn_techmsg_sent;
    //=================
    hashx<s_ll, t_mtrk_sender_info> mtrk_htrk_send; // { msg or subscription id; weak refs. to tracking_info and its container }; locking: lkd_pt_access
    vnnqueue_t<t_mtrk_qipush_entry> mtrk_qipush_send; // sequence of ids from mtrk_htrk_send; locking: lkd_pt_access
    struct __msg_tr
    {
      cref_t<t_stringref> msg;
      void swap_msg(cref_t<t_stringref>& x) { bmdx_str::words::swap_bytes(msg, x); }
    };
      // Queued tech. replies of various kinds, gathered and sent through _qum_send.
      //  If sending fails, _lq_send_tr keeps the unsent msgs. till next LMSC the det_periodic() call,
      //  so that their info is not lost for the peer unless the current process is terminated..
    vnnqueue_t<__msg_tr> _lq_send_tr; // locking: none (LMSC thread use only)
    bool _qum_send_avl;
    bool _qum_send_conf_reset_on_exit;
    double _qum_send_avl_tm_lastchk;
    //=================
    struct __tracking_info_reply
    {
      tracking_info ti;
      unsigned char flags; // see md_entry type 54
      __tracking_info_reply(const tracking_info& ti_, const unsigned char flags_) : ti(ti_), flags(flags_) {}
    };
      // Queue of received tracking results for msg. delivery and subscriptions.
      //  Contains also hints (flags) on what actions should be done for each message.
      //  If any of actions fails, _lq_recv_trk_res keeps the unprocessed msgs. till the next LMSC det_periodic() call,
      //  so that their info is not lost unless the current process is terminated..
    vnnqueue_t<__tracking_info_reply> _lq_recv_tr; // locking: none (LMSC thread use only)
    s_ll _ipop_qum_rcv_fail; // ind. of the head msg. in _qum_rcv (ipop()), for which _s_write failed (returned -2)
    bool _qum_rcv_conf_reset_on_exit;
    //================================
    unsigned char _x_close_state;
    s_ll _x_close_ipush;

    friend struct lm_slot_controller;
    struct lks_idgen {};

      // normally called from both lm_slot_controller and peer_tracker
    inline double _cltpt_tm_last_init_ms() { return _tm_lastinit; }
    inline s_long _cltpt_ensure_comm(bool b_wait, bool b_force_check) __bmdx_noex;
    inline bool _cltpt_close(bool b_final) __bmdx_noex;
    inline void _cltpt_qum_conf_reset(bool b_rcv, bool b_forced) __bmdx_noex;

      // purely private (peer_tracker use only)
    static s_ll _x_idgen_peer() { static s_ll x(0); critsec_t<lks_idgen> __lock(0, -1); if (sizeof(__lock)) {} return ++x; }
    inline s_long _x_qinfo_upd_prc_act_time();
    inline void _x_init() __bmdx_noex;
  };

    // 1. Helper functions (for any thread).

  static inline char make_identifier_char(wchar_t c0)  __bmdx_noex;
  static inline bmdx_shm::t_name_shm __make_name_one_impl(arrayref_t<char> name) __bmdx_noex;
  static inline bmdx_shm::t_name_shm __make_name_two_impl(arrayref_t<char> name1, arrayref_t<char> name2) __bmdx_noex;
  static inline bmdx_shm::t_name_shm make_name_onef(const bmdx_shm::t_name_shm& name) __bmdx_noex { return __make_name_one_impl(arrayref_t<char>(name.pd(), name.n())); }
  static inline bmdx_shm::t_name_shm make_name_twof(const bmdx_shm::t_name_shm& name1, const bmdx_shm::t_name_shm& name2) __bmdx_noex { return __make_name_two_impl(arrayref_t<char>(name1.pd(), name1.n()), arrayref_t<char>(name2.pd(), name2.n())); }

    // 2. Servicing thread API.
    //
    //  Called from LMSC servicing thread only (except det_init() once called by dispatcher_mt).

  inline s_long det_state() const __bmdx_noex { return _state; }
  inline void det_init() __bmdx_noex;
  inline void det_periodic(i_callback& cb, bool& b_active) __bmdx_noex;
  inline s_long det_check_qinfo() __bmdx_noex;
  inline void det_close() __bmdx_noex { _x_close(); }

    // 3. Client API (exclusively for dispatcher_mt).

  inline void clt_setparam_chsbin(bool b_enable);
  inline void clt_set_ses_state(s_long st);

  inline s_long clt_ensure_comm(arrayref_t<wchar_t> name_peer, s_long comm_mode) __bmdx_noex;
  inline s_long clt_msend(s_ll mtrk_id_msg, s_ll id_msg_cmd_sender, unsigned char flags_mtrk, arrayref_t<wchar_t> name_peer, arrayref_t<wchar_t> msg, const cref_t<t_stringref>& bin, s_long flags_clt_msend, const weakref_t<t_htracking_proxy>& rhtrprx) __bmdx_noex;
  inline s_long clt_mdsend(s_ll mtrk_id, arrayref_t<wchar_t> name_peer, const netmsg_header::msg_builder& msg, const weakref_t<t_htracking_proxy>& rhtrprx, s_long comm_mode) __bmdx_noex;

  inline s_long clt_qum_conf_reset(arrayref_t<wchar_t> name_peer, bool b_rcv, bool b_forced) __bmdx_noex;
  inline s_long clt_qum_conf_set_rcv(arrayref_t<wchar_t> name_peer, bool b_al, bool b_lqcap, s_long& res_al, s_long& res_lqcap, const cref_t<bmdx_shm::i_allocctl>* p_al, double timeout_ms_al, s_ll ncapmin, s_ll ncapmax, s_ll nrsv, double timeout_ms_lqcap) __bmdx_noex;
  inline s_long clt_qum_conf_set_send(arrayref_t<wchar_t> name_peer, s_long& res_lqcap, s_ll ncapmin, s_ll ncapmax, s_ll nrsv, double timeout_ms_lqcap) __bmdx_noex;

private:
  s_ll _nb_qum; volatile s_long _fl_chs;
  unity _apns;
  const bmdx_shm::t_name_shm _name_prf;
  volatile s_long _state; // 1 ready; 0 not initialized yet; -2 init. failed, will be tried again later; -3 closing; -4 already closed
  double _tms_next_init;

    // Shared queue for informational messages from multiple peer processes.
    //  Associated 1:1 with owning dispatcher_mt instance.
    //  Presence of _qinfo_rcv is recognized by peer processes
    //    as presence (or trace of presence) of the process, owning the queue.
    // Locking: for queue state modification (check/init/open/close/pop) only.
    //    _qinfo_rcv is accessed only by single LMSC servicing thread,
    //    except for clt_set_ses_state(),
    //    which is called by dispatcher_mt, to immediately "publish" its session state changes.
  critsec_t<lm_slot_controller>::csdata lkd_qinfo_rcv_state;
  shmqueue_ms_receiver_t<_r_prc_uid, __vecm_tu_selector> _qinfo_rcv;

  bool _b_qinfo_rcv_valid; // becomes true when shared queue is ensured to be initialized and compatible
  s_long _qinfo_rcv_idd0; // index in _qinfo_rcv.dd, marks that all entries with index < _qinfo_rcv_idd0 are already processed; locking: none (LMSC thread use only)
  hashx<bmdx_shm::t_name_shm, s_long> _hnew_name_peerf; // { shared queue name, 0 }; names for queues to create as requested by peer processes via _qinfo_rcv; locking: none (LMSC thread use only)

  critsec_t<lm_slot_controller>::csdata lkd_peer_hpt; // for locking *_hpt* read/modify
  hashx<bmdx_shm::t_name_shm, cref_t<peer_tracker> > _hpt; // grow-only { name_peerf (global), peer tracker }; locking: lkd_peer_hpt

  inline bool _x_qinfo_rcv_init() __bmdx_noex;
  inline void _x_qinfo_rcv_update_ses_state() __bmdx_noex;
  inline void _x_qinfo_rcv_mark_process_closed() __bmdx_noex;
  inline cref_t<peer_tracker> _x_pt_getcr(const bmdx_shm::t_name_shm& name_peerf, bool b_comm_once, bool b_autocreate) __bmdx_noex;
  inline void _x_close() __bmdx_noex;

  struct _mtrk_fn_handler
  {
    peer_tracker& pt;
    const weakref_t<t_htracking_proxy>& rhtrprx;
    int state; // 0 - initial state or "do nothing", -1 mtrk prep. failed, 1 - mtrk prepared (cancel on des.), 2 - mtrk prepared (apply on des.)
    s_ll id_msg, ipush_shmlq;
    _mtrk_fn_handler(peer_tracker& pt_, s_ll id_msg_, const weakref_t<t_htracking_proxy>& rhtrprx_) : pt(pt_), rhtrprx(rhtrprx_), state(0), id_msg(id_msg_), ipush_shmlq(-1) {}
    bool mtrk_prep()
    {
      if (state != 0) { return false; }
      if (pt.mtrk_qipush_send.ncapeff() < 1 && pt.mtrk_qipush_send.set_rsv(10 + pt.mtrk_qipush_send.navl()) < 1) { state = -1; return false; }
      const hashx<s_ll, t_mtrk_sender_info>::entry* e = 0;
      if (pt.mtrk_htrk_send.insert(id_msg, &e) != 1) { state = -1; return false; }
      e->v.ipush_shmlq = -1;
      e->v.rhtrprx = rhtrprx;
      state = 1;
      return true;
    }
      // ipush_shmlq >= 0: apply on des.; < 0: cancel on des.
    void mtrk_prep2(s_ll ipush_shmlq_) { ipush_shmlq = ipush_shmlq_; if (ipush_shmlq_ >= 0 && state == 1) { state = 2; } }
    ~_mtrk_fn_handler() { if (state == 1) { _mtrk_cancel(); } else if (state == 2) { _mtrk_apply(); } }
  private:
    void _mtrk_apply()
    {
      pt.mtrk_qipush_send.push_1(t_mtrk_qipush_entry(id_msg, ipush_shmlq));
      try { t_mtrk_sender_info& x = pt.mtrk_htrk_send[id_msg]; x.rhtrprx = rhtrprx; x.ipush_shmlq = ipush_shmlq; } catch (...) {}
    }
    void _mtrk_cancel() { pt.mtrk_htrk_send.remove(id_msg); }
  };

  struct new_mv_msg_tr { typedef cref_t<t_stringref> t_msg; typedef peer_tracker::__msg_tr t; t_msg& m; new_mv_msg_tr(t_msg& m_) : m(m_) {} void operator()(t* p) const { bmdx_str::words::swap_bytes(p->msg, m); new(&m) t_msg(); } };
};

  // name_prf_: make_fixed_utf8_name of process name of the parent dispatcher_mt.
  // name_peerf_: make_fixed_utf8_name of process peer's process name, taken from message destination address, supplied by client.
lm_slot_controller::peer_tracker::peer_tracker(const bmdx_shm::t_name_shm& name_prf_, const bmdx_shm::t_name_shm& name_peerf_, s_ll nb_qum_) __bmdx_noex
:
  _name_prf(name_prf_), _name_peerf(name_peerf_), b_self(name_peerf_ == name_prf_),
  _qinfo_send(make_name_onef(name_peerf_)),
  _qum_rcv(make_name_twof(_name_peerf, _name_prf), limit_nbqueue(nb_qum_, __lm_slot_controller_nb_rqum_min, __lm_slot_controller_nb_rqum_max, __lm_slot_controller_nb_rqum_dflt), 8|1),
  _qum_send(make_name_twof(_name_prf, _name_peerf), -1, 4|1)
{
  _id_peer = _x_idgen_peer();
  _tm_lastinit = clock_ms() - 10000; _tm_mtrk_nem = clock_ms();
  _state = 0;
  _b_clt_mdsend_wait_once = true;

  _qinfo_ids.livecnt(-1); _qinfo_livecnt_tm = clock_ms();
  _qinfo_peer_conn_techmsg_sent = false;

  mtrk_qipush_send.set_cap_hints(-1, -2);
  _lq_send_tr.set_cap_hints(-1, -2);

  _lq_recv_tr.set_cap_hints(-1, -2);
  _qum_send_avl = false; _qum_send_conf_reset_on_exit = false; _qum_send_avl_tm_lastchk = clock_ms() - 2 * __lm_slot_controller_qum_send_avl_chk_dtms;
  _ipop_qum_rcv_fail = -1; _qum_rcv_conf_reset_on_exit = false;

  _x_close_state = 0; _x_close_ipush = -1;
}
  // b_wait:
  //    false: for LMSC thread, to check state and perform non-blocking operations if necessary.
  //    true: for LMSC clients (check + non-blocking operations + waiting for required state or timeout).
  // b_force_check (only on b_wait == false):
  //    true: anyway check _qum_rcv, _qum_send state.
  //    false: check their state only if __lm_slot_controller_qum_send_avl_chk_dtms has passed after the last check.
  // Returns:
  //  1 - the peer process is communicable normally (all needed queues are working).
  //  0 - the peer process appears a) hanged, lagging, terminated, b) normally exited.
  //  -1 - the peer process is not available.
s_long lm_slot_controller::peer_tracker::_cltpt_ensure_comm(bool b_wait, bool b_force_check) __bmdx_noex
{
  if (_state != 1)
  {
    _x_init();
    if (_state != 1) { return -1; }
  }


  if (1)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}
    const s_long state_peer = _x_qinfo_upd_prc_act_time(); // peer process hanging/termination/inactivity check
    if (state_peer == -1) { return 0; } // peer process is not responding
    if (state_peer < 0) { return -1; } // peer process is N/A
    if (this->b_self)
    {
      _qinfo_peer_conn_techmsg_sent = true;
      _qum_send_avl = 3 == _qum_send.bufstate(0);
      return _qum_send_avl ? 1 : -1;
    }
    else if (state_peer == 1 || (state_peer == 0 && !_qinfo_peer_conn_techmsg_sent))
    {
      _qinfo_peer_conn_techmsg_sent = false;
      netmsg_header::msg_builder mb(fpt_md, sizeof(netmsg_header::t_md1_hdr_max) + _name_prf.n());
      if (!(mb.set_md1(1,  t_stringref(_name_prf.pd(), _name_prf.n())) && mb.set_final())) { return -1; }
      if (2 != _qinfo_send.msend_1(mb.msg.ref(), true)) { return -1; }
      _qinfo_peer_conn_techmsg_sent = true;
    }
  }

  if (b_wait) // waiting for input msgs queue to be created by peer process
  {
    if (_qum_send_avl) { return 1; }
    const double t0 = clock_ms();
    bool bres = false;
    while (clock_ms() - t0 < __lm_slot_controller_qum_send_avl_wait_toms)
    {
      if (1)
      {
        critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}
        if (_qum_send_avl) { bres = true; break; } // may have been set by _cltpt_ensure_comm, called from e.g. LMSC thread with b_wait == false
        if (3 == _qum_send.bufstate(0)) { bres = true; break; }
      }
      const s_ll tsleep_mcs = 100;
      sleep_mcs(tsleep_mcs);
    }
    if (bres) { _qum_send_avl = true; }
    return bres ? 1 : -1;
  }
  else // non-blocking waiting (check only)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}
    if (_qum_send_avl) { return 1; }
    const double tnow = clock_ms();
    if (tnow - _qum_send_avl_tm_lastchk < __lm_slot_controller_qum_send_avl_chk_dtms && !b_force_check) { return -1; }
    _qum_send_avl_tm_lastchk = tnow;
    bool bres = 3 == _qum_send.bufstate(0);
    _qum_send_avl = bres;
    return bres ? 1 : -1;
  }
}
  // For calling from lm_slot_controller many times.
  //  1st call initiates closing, next calls are only checking if all done.
  //  Returns true if all done, false - not yet.
bool lm_slot_controller::peer_tracker::_cltpt_close(bool b_final) __bmdx_noex
{
  if (_state == -4) { return true; }
  critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}

  if (_x_close_state == 0)
  {
    if (_state == 1)
    {
      _qum_send.lqstate(false, &_x_close_ipush, 0, -1, __bmdx_disp_lq_cleanup_dtms); // NOTE this only initiates queue cleanup; lqstate itself exits immediately
    }
    // NOTE See also notes near cch_thread :: mtrk_htracking.
    _cltpt_qum_conf_reset(true, false); // reset receiver queue conf., only if it was modified
    _cltpt_qum_conf_reset(false, false); // reset sender queue conf., only if it was modified
    _x_close_state = 1;
    _state = -3;
  }

  if (_x_close_state == 1)
  {
    if (_x_close_ipush != -1)
    {
      s_ll ipop = -1;
      _qum_send.lqstate(false, 0, &ipop);
      if (ipop != -1 && ipop - _x_close_ipush < 0)
      {
        if (!b_final) { return false; }
      }
    }
    _qum_send.bufstate(8);
    _qum_rcv.bufstate(9);
    _x_close_state = 2;
  }

  if (_x_close_state == 2)
  {
    bool b1 = _qum_send.b_rq();
      if (b1) { s_long res = _qum_send.bufstate(0); if (res < 0) { _qum_send.clear_rq(); b1 = false; } }
    bool b2 = _qum_rcv.b_rq();
      if (b2) { s_long res = _qum_rcv.bufstate(1); if (res < 0) { _qum_rcv.clear_rq(); b2 = false; } }
    if (b1 || b2)
      { return false; }
    _x_close_state = 3;
  }

  _state = -4;

  return true;
}

  // Tracks the peer process public activity status.
  // Indirectly (i.e. from within _cltpt_ensure_comm) called by LMSC thread and client threads.
  //  The call must be done under lkd_pt_access.
  //  Updates cached _r_prc_uid object's data from _qinfo_send.
  //  Returns:
  //    1 -  the peer process has just started or restarted - should be informed on the current process activity.
  //    0 - the peer process is available for normal communication, no special action needed.
  //    -1 - the peer process is not available: a) hanged, b) terminated and not restarted yet, c) LMSC is not active in the peer process (e.g. peer's dispatcher_mt exited).
  //    -2 - the peer process is available, but should not be used for communication (communication type or version is different).
  //    -3 - the shared queue object is not available.
s_long lm_slot_controller::peer_tracker::_x_qinfo_upd_prc_act_time()
{
  if (_qinfo_send.attempt_open() != 1) // no system access to shared object
    { return -3; }

  const double t = clock_ms();
  _r_prc_uid vids; vids.cp_ids_from_volatile(_qinfo_send.so->auxd(), true);

  if (this->b_self) // self-connected (means: the peer is always available).
    { _qinfo_ids.cp_ids_from_volatile(vids, true); _qinfo_livecnt_tm = t; return 0; }

  if (vids.sig() != _r_prc_uid::Fsig()) // incompatible shared object
    { _qinfo_send.so.close(); return -2; }

  if (!_qinfo_ids.b_same_inst(vids)) // one of: a) just started new peer process (init. not complete yet), b) peer's LMSC exited
  {
    if (vids.prc_inst_id() == 0) { return -1; }

    if (vids.prc_inst_id() == -1)
    {
      s_ll ipush1 = 0, ipop1 = 0;
      s_long res1 = _qum_rcv.bufstate(1, &ipush1, &ipop1);
      s_long res2 = _qum_rcv.lqstate(1);
      // When peer process had normally exited, but some data from peer remains
      //  in the local input queue or in shared queue,
      //  reporting hanged status is delayed (until LMSC pops all available messages).
      if (res2 == 1 || (res1 == 3 && !(ipop1 - ipush1 < 0)))
      {
        if (vids.livecnt() != _qinfo_ids.livecnt()) { _qinfo_ids.livecnt(vids.livecnt()); _qinfo_livecnt_tm = t; return 0; }
          else if (t - _qinfo_livecnt_tm < __lm_slot_controller_peer_prc_hang_toms) { return 0; }
      }
      return -1;
    }

      // New peer process instance detected. Checking pid_deriv (i.e. if communication with such process is allowed).
    if (vids.pid() == 0 || pid_deriv_v1(vids.pid()) != vids.pid_deriv()) { return -2; }
    _qinfo_ids.cp_ids_from_volatile(vids, true); _qinfo_livecnt_tm = t;
    return 1;
  }
  if (vids.livecnt() != _qinfo_ids.livecnt()) { _qinfo_ids.livecnt(vids.livecnt()); _qinfo_livecnt_tm = t; return 0; }
    else if (t - _qinfo_livecnt_tm < __lm_slot_controller_peer_prc_hang_toms) { return 0; }
  return -1; // timeout (the peer is hanged or terminated)
}
  // When x_init succeeds:
  //  1) _qum_rcv is working (the shared object belongs to LMSC of the current process),
  //  2) _qinfo_send is working (the shared object belongs to the peer process),
  //  3) _qum_send is unavailable (or working, but only if the the peer is restarted).
  //    To make _qum_send surely available, _cltpt_ensure_comm sends tech. msg.
  //    via _qinfo_send, and waits until the peer reads it and creates its own _qum_rcv with the offered name;
  //    it thus initializes the shared memory; if successful, _qum_send here is able to connect to the memory and thus comes into working state.
void lm_slot_controller::peer_tracker::_x_init() __bmdx_noex
{
  if (!(_state == 0 || _state == -2)) { return; }

  critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}
  if (!(_state == 0 || _state == -2)) { return; }

  _tm_lastinit = clock_ms();
  _qinfo_peer_conn_techmsg_sent = false;
  _qum_send_avl = false;

  s_long res2 = _qinfo_send.attempt_open();
  s_long res3 = _qum_rcv.bufstate(3);
  s_long res4 = _qum_send.bufstate(2);
  if (!(res2 >= 1 && res3 == 3 && res4 >= 0 && res4 != 4)) { _state = -2; return; }

  if (this->b_self) { _qinfo_peer_conn_techmsg_sent = true; } // self has already created all its structures and thus does not need any dialog with itself

  _state = 1;
}







  // NOTE If init. fails, it will really retry only after some delay (see _x_qinfo_rcv_init).
  //  b_active: is set to true by det_periodic each time it makes useful action.
  //    If it did not set b_active for long, LMSC thread will make itself more sleepy.
void lm_slot_controller::det_periodic(i_callback& cb, bool& b_active) __bmdx_noex
{
  if (_state != 1) { return; }
  if (!_x_qinfo_rcv_init()) { return; }

  if (1) // servicing peer trackers (input messages etc.)
  {
    cpparray_t<netmsg_header::md_entry> __mdee2;
    netmsg_header::md_entry __mdee1[nmdemax];

    hashx<bmdx_shm::t_name_shm, cref_t<peer_tracker> > hpt2;
    if (1) // copying the list of peer trackers + making certain updates
    {
      critsec_t<lm_slot_controller> __lock(10, -1, &lkd_peer_hpt); if (sizeof(__lock)) {}
        //  - create peer_tracking objects in _hpeernm if not yet in hpt, move to hpt
        //  - copy _hpt
      while (_hnew_name_peerf.n() > 0)
      {
        b_active = true;
        const bmdx_shm::t_name_shm& k = _hnew_name_peerf(0)->k;
        if (_hpt.find(k)) { _hnew_name_peerf.remove(k); continue; }
        cref_t<peer_tracker> pt = _x_pt_getcr(k, true, true);
          if (!pt) { break; } // will retry on the next det_periodic() call
          try { bmdx_str::words::swap_bytes(_hpt.opsub(k), pt); } catch (...) { break; } // will retry on the next det_periodic() call
        _hnew_name_peerf.remove(k); // removal does not fail (both k, v are movable)
      }
      if (hpt2.hashx_copy(_hpt, false) < 1) { goto lBreak2; }
      goto lCont2;
lBreak2: return;
lCont2:;
    }

    if (1) // working on each peer tracker (hpt2)
    {
      // - init. peer if not yet or dt has passed after prv. init. failure
      // - detect peer process hanging/termination
      // - detect peer process restart, send own name for reopening the pair of user msg. queues
      // - for the currently avl. messages in the input queue:
      //    - tracking msg: process msg. tracking; pop on success or break
      //    - user. msg: decode, update src./trg. addresses, queue backw. tracking msg. if needed, _s_write; a) pop on _s_write success b) pop/break on 2nd decode failure c) break on other failure
      //    - unknown msg: ignore (pop only)
      // - for the currently unsent tracking msgs: send and pop from local queue
      for (s_long i = 0; i < hpt2.n(); ++i)
      { try {
        peer_tracker& pt = hpt2(i)->v._rnonc();
        if (pt._state != 1 && clock_ms() - pt._cltpt_tm_last_init_ms() < __lm_slot_controller_peer_reinit_dtms) { continue; }
        if (pt._cltpt_ensure_comm(false, false) < 1) { continue; }

          // Sending tech. messages (subs. request results and tracking infos), possibly remaining from prev. det_periodic() calls.
        if (pt._lq_send_tr.navl() > 0)
        {
          b_active = true;
          critsec_t<lm_slot_controller> __lock(10, -1, &pt.lkd_pt_access); if (sizeof(__lock)) {}
          while (pt._lq_send_tr.navl() > 0) { peer_tracker::__msg_tr& m = pt._lq_send_tr.front(); if (m.msg) { s_long res = pt._qum_send.msend(m.msg); if (res < 1) { break; } } pt._lq_send_tr.pop_1(); }
        }

        while (1)
        {
          s_long res_mget = pt._qum_rcv.mget(0, false); // peek
          if (res_mget <= 0) { break; }
          b_active = true;
          if (pt._lq_send_tr.ncapeff() - pt._lq_send_tr.navl() <= 0 && pt._lq_send_tr.set_rsv(10 + pt._lq_send_tr.navl()) < 1) { break; } // failed to reserve place in tech. reply queue
          bool b_pop = true;
          do { // once
            netmsg_header hdr; t_stringref orig = pt._qum_rcv.d._rnonc();
            if (!hdr.fields_decode(orig, fpt_md|fpt_msg|_fl_chs)) { break; }

            arrayref_t<netmsg_header::md_entry> mdee(__mdee1, hdr.get_mde(arrayref_t<netmsg_header::md_entry>(__mdee1, nmdemax), true));
            if (mdee.n() > nmdemax)
            {
              if (__mdee2.n() < mdee.n() && !__mdee2.resize(mdee.n())) { b_pop = false; break; }
              mdee.link(__mdee2.pd(), hdr.get_mde(arrayref_t<netmsg_header::md_entry>(__mdee2.pd(), mdee.n()), true));
            }

            if (hdr.has_msg()) // user message
            {
              unsigned char fl_deliv_rpt = 0; s_ll id_msg = -1, id_msg_cmd = -1; // fl_deliv_rpt != 0, id_msg is anyway set to valid id
              s_long ver = 1;
                // Analyze all available metadata entries in the current message.
              for (s_ll i = mdee.n() - 1; i >= 0; --i)
              {
                const s_long t = mdee[i].type;
                if (t >= 3 && t <= 50) { ver = t; }
                else if (mdee[i].b_type(mdd_id_msg::stype)) { if (mdee[i].b_valid_size<mdd_id_msg>()) { mdd_id_msg* pmd = ((mdd_id_msg*)mdee[i].data.pd()); id_msg = pmd->id_msg.value(); fl_deliv_rpt = pmd->flags.value(); } }
                else if (mdee[i].b_type(mdd_id_msg_cmd::stype)) { if (mdee[i].b_valid_size<mdd_id_msg_cmd>()) { mdd_id_msg_cmd* pmd = ((mdd_id_msg_cmd*)mdee[i].data.pd()); id_msg_cmd = pmd->id_msg.value(); } }
                else {}
              }

              enum { res_dec_failed = -13, res_accepted = 3 }; // see i_dispatcher_mt::msend ret. codes
              if (ver > __lm_slot_controller_user_msg_max_ver) // msg. version is not supported
              {
                if (fl_deliv_rpt) // reply with "message decoding failed" result
                {
                  typedef mdd_deliv_reply t_reply;
                  netmsg_header::msg_builder mb(fpt_md, sizeof(netmsg_header::t_md1_hdr_max) + sizeof(t_reply)); t_reply dr; dr.id_object.set(id_msg); dr.res.set(res_dec_failed); dr.flags.set(fl_deliv_rpt);
                  if (!(mb.set_md1(dr.type(), t_stringref(dr, sizeof(dr))) && mb.set_final())) { b_pop = false; break; }
                  pt._lq_send_tr.push_1f(new_mv_msg_tr(mb.msg)); // push_1 succeeds because the place is pre-reserved
                }
                break;
              }
              cref_t<t_stringref> att;
              if (hdr.has_bin() && hdr.bin.is_valid())
              {
                att = pt._qum_rcv.d;
                att._rnonc() = hdr.bin;
              }
              if (!fl_deliv_rpt)
              {
                  // Calling the requested function. No backward sending for the result required.
                cb.local_write(hdr.msg, i_callback::umsg_enc_type(hdr.b_msg_utf8()), att, id_msg, id_msg_cmd);
              }
              else
              {
                  // Pre-reserving storages for reply.
                typedef mdd_deliv_reply t_reply;
                if (pt._lq_send_tr.ncapeff() < 1 && pt._lq_send_tr.set_rsv(10 + pt._lq_send_tr.navl()) < 1) { b_pop = false; break; }
                netmsg_header::msg_builder mb(fpt_md, sizeof(netmsg_header::t_md1_hdr_max) + sizeof(t_reply));
                    if (!mb.b_valid()) { b_pop = false; break; }

                  // Calling the requested function.
                s_long res_s_write = cb.local_write(hdr.msg, i_callback::umsg_enc_type(hdr.b_msg_utf8()), att, id_msg, id_msg_cmd);

                  // Checking for continuous failure (1st failure is ignored, after 2nd the message is discarded).
                if (res_s_write == -2)
                {
                  s_ll ipop_lq = -1; pt._qum_rcv.lqstate(1, 0, &ipop_lq);
                  if (ipop_lq != pt._ipop_qum_rcv_fail) { pt._ipop_qum_rcv_fail = ipop_lq; b_pop = false; break; }
                }

                    // Replying. The following sequence doesn't fail because all storages for data are pre-reserved.
                if (res_s_write == -2) { res_s_write = res_dec_failed; }
                  else if (res_s_write >= 0) { res_s_write = res_accepted; }
                t_reply dr;
                dr.id_object.set(id_msg); dr.res.set(res_s_write); dr.flags.set(fl_deliv_rpt);
                mb.set_md1(dr.type(), t_stringref(dr, sizeof(dr)));
                mb.set_final();
                pt._lq_send_tr.push_1f(new_mv_msg_tr(mb.msg));
              }
              break;
            }
            else if (hdr.has_md()) // pure tech. message
            {
              for (s_long i = 0; i < mdee.n(); ++i)
              {
                const s_long t = mdee[i].type;
                if (mdee[i].b_type(mdd_deliv_reply::stype)) // got tech. reply with result of peer's _s_write on user message
                {
                  if (!mdee[i].b_valid_size<mdd_deliv_reply>()) { continue; }
                  mdd_deliv_reply* pmd = (mdd_deliv_reply*)mdee[i].data.pd();
                  if (pt._lq_recv_tr.push_1(peer_tracker::__tracking_info_reply(tracking_info(pmd->id_object.value(), pmd->res.value()), pmd->flags.value())) < 1) { b_pop = false; break; }
                }
                else if (mdee[i].b_type(mdd_subs_reply::stype)) // got tech. reply with result of peer's _s_subscribe
                {
                  if (!mdee[i].b_valid_size<mdd_subs_reply>()) { continue; }
                  mdd_subs_reply* pmd = (mdd_subs_reply*)mdee[i].data.pd();
                  if (pt._lq_recv_tr.push_1(peer_tracker::__tracking_info_reply(tracking_info(pmd->id_object.value(), pmd->res.value()), 1)) < 1) { b_pop = false; break; }
                }
                else if (t == mdd_subs_request::stype) // peer process requests _s_subscribe
                {
                  mdd_subs_request dm;
                  if (!dm.read_msg(mdee[i].data)) { continue; } // won't fail if the data is correct

                    // Pre-reserving storages for reply.
                  typedef mdd_subs_reply t_reply;
                  if (pt._lq_send_tr.ncapeff() < 1 && pt._lq_send_tr.set_rsv(10 + pt._lq_send_tr.navl()) < 1) { b_pop = false; break; }
                  netmsg_header::msg_builder mb(fpt_md, sizeof(netmsg_header::t_md1_hdr_max) + sizeof(t_reply));
                    if (!mb.b_valid()) { b_pop = false; break; }

                    // Calling the requested function.
                  s_long res_s_subscribe = cb.local_subscribe(mdee[i].data, dm, pt._name_peerf);

                    // Replying. The following sequence doesn't fail because all storages for data are pre-reserved.
                  t_reply dr;
                  dr.id_object.set(dm.id_trk); dr.res.set(res_s_subscribe);
                  mb.set_md1(dr.type(), t_stringref(dr, sizeof(dr)));
                  mb.set_final();
                  pt._lq_send_tr.push_1f(new_mv_msg_tr(mb.msg));
                }
              }
              break;
            }
          } while (false);
          if (b_pop) { pt._qum_rcv.mget(); } // pop the message if successfully processed
          pt._qum_rcv.d.clear(); // clear cached message, if any
          if (!b_pop) { break; } // end msg. analysis for this peer (common-case failure); continue on the next call to det_periodic()
        }

          // Sending tech. messages (subs. request results and tracking infos), possibly created inside the above loop.
          //  NOTE Queuing is used to minimize time, spend by the current thread on pt.lkd_pt_access,
          //    because pt._qum_send may be freqently used by _s_write, called from a client thread.
        if (pt._lq_send_tr.navl() > 0 || pt._lq_recv_tr.navl() > 0)
        {
          b_active = true;
            // htrk2: for user tracking_info updates; filled from pt.mtrk_htrk_send
          hashx<t_htracking_proxy*, t_mtrk_sender_info_id_list, hashx_common::kf_basic<t_htracking_proxy*>, __vecm_tu_selector> htrk2;
          vec2_t<tracking_info> vtrkcsl; // for command slot phase updates; filled from pt.mtrk_htrk_send
          hashx<s_ll, s_long> hstop_trk; // set of msg. ids for which all types of tracking should be finished
          bool b_failed = false;
          if (1)
          {
            critsec_t<lm_slot_controller> __lock(10, -1, &pt.lkd_pt_access); if (sizeof(__lock)) {}

              // independent operation 1 under pt.lkd_pt_access
            while (pt._lq_send_tr.navl() > 0) { peer_tracker::__msg_tr& m = pt._lq_send_tr.front(); if (m.msg) { s_long res = pt._qum_send.msend(m.msg); if (res < 1) { b_failed = true; break; } } pt._lq_send_tr.pop_1(); }

              // independent operation 2 under same pt.lkd_pt_access
            if (!b_failed && pt.mtrk_htrk_send.n() > 0)
            {
              s_ll imax_shmlq = -1;
              for (vnnqueue_t<peer_tracker::__tracking_info_reply>::const_iterator i = pt._lq_recv_tr.begin(); !i.b_aend(); ++i)
              {
                const peer_tracker::__tracking_info_reply& ti = *i;
                if (ti.flags & 1) // user tracking_info object update required
                {
                  const hashx<s_ll, t_mtrk_sender_info>::entry* e = pt.mtrk_htrk_send.find(ti.ti.id);
                  if (e)
                  {
                    const t_mtrk_sender_info& si = e->v;
                    imax_shmlq = bmdx_minmax::myllmax(imax_shmlq, si.ipush_shmlq);
                    try {
                      t_mtrk_sender_info_id_list& silst = htrk2.opsub(si.rhtrprx.pwk);
                      if (!silst.si.rhtrprx.pwk) { silst.si = si; }
                      silst._vti.push_back(ti.ti);
                    } catch (...) { b_failed = true; break; }
                  }
                }
                if (ti.flags & 2) // command slot phase modification required
                  { try { vtrkcsl.push_back(ti.ti); } catch (...) { b_failed = true; break; } }
                try { hstop_trk.opsub(ti.ti.id); } catch (...) { b_failed = true; break; }
              }
              if (!b_failed)
              {
                  // If peer process has restarted, some part of messages may have been lost.
                  //  Ids of lost messages are taken from full sequence of ids, for which tracking is required (mtrk_qipush_send).
                  //  All messages with no "hope" already in getting valid tracking status, get tracking result code -12.
                while (pt.mtrk_qipush_send.navl() > 0)
                {
                  t_mtrk_qipush_entry e_q = pt.mtrk_qipush_send.front();
                  if (e_q.ipush_shmlq >= imax_shmlq) { break; }
                  if (!hstop_trk.find(e_q.id_msg))
                  {
                    const hashx<s_ll, t_mtrk_sender_info>::entry* e = pt.mtrk_htrk_send.find(e_q.id_msg);
                      if (!e) { pt.mtrk_qipush_send.pop_1(); continue; }
                    const t_mtrk_sender_info& si = e->v;
                    try { t_mtrk_sender_info_id_list& silst = htrk2.opsub(si.rhtrprx.pwk); if (!silst.si.rhtrprx.pwk) { silst.si = si; } silst._vti.push_back(tracking_info(e_q.id_msg, -12)); hstop_trk.opsub(e_q.id_msg); } catch (...) { b_failed = true; break; }
                  }
                  pt.mtrk_qipush_send.pop_1();
                }
              }
            }
          } // end lock on pt.lkd_pt_access

          if (!b_failed)
          {
            b_failed = cb.cslph_update(vtrkcsl) < 1; // independent op.
          }
          if (!b_failed)
          {
            if (htrk2.n() > 0) // independent op.
            {
              for (s_long i = 0; i < htrk2.n(); ++i)
              {
                const t_mtrk_sender_info_id_list& silst = htrk2(i)->v;
                weakref_t<t_htracking_proxy>::t_lock __lock(silst.si.rhtrprx); if (sizeof(__lock)) {}
                if (silst.si.rhtrprx.b_valid())
                {
                  for (s_long i = 0; i < silst._vti.n(); ++i)
                  {
                    const tracking_info& ti = silst._vti[i];
                    const t_htracking_proxy::entry* e = silst.si.rhtrprx.pwk->find(ti.id);
                    if (e && e->v)
                    {
                      s_ll s = e->v->state;
                      if (ti.state < 0) { s = ti.state; }
                        else if (ti.state > 0) { if (ti.state > s) { s = ti.state; } }
                        else { s = -2; }
                      e->v->state = s;
                    }
                    silst.si.rhtrprx.pwk->remove(ti.id);
                  }
                }
              }
              htrk2.hashx_clear();
            }
            if (1) // remove the processed entries
            {
              if (hstop_trk.n() > 0)
              {
                critsec_t<lm_slot_controller> __lock(10, -1, &pt.lkd_pt_access); if (sizeof(__lock)) {}
                for (s_long i = 0; i < hstop_trk.n(); ++i) { pt.mtrk_htrk_send.remove(hstop_trk(i)->k); }
              }
              pt._lq_recv_tr.pop_n(-1);
            }
          }
        }
      } catch (...) {} }
    }

    if (1) // cleanup (faster when done under single lock)
    {
      critsec_t<lm_slot_controller> __lock(10, -1, &lkd_peer_hpt); if (sizeof(__lock)) {}
      hpt2.hashx_clear();
    }
  }
}
  // Returns:
  //  1 - _qinfo_rcv has already received any data from a process.
  //  0 - _qinfo_rcv is valid, but not received any data from any process yet.
  //  -1 - _qinfo_rcv is not initialized.
  //  -2 - _qinfo_rcv initialization has failed because the shared object already exists and is not compatible.
s_long lm_slot_controller::det_check_qinfo() __bmdx_noex
{
  critsec_t<lm_slot_controller> __lock(10, -1, &lkd_qinfo_rcv_state); if (sizeof(__lock)) {}
  if (_qinfo_rcv.so.f_constructed() == 1)
  {
    if (!_b_qinfo_rcv_valid) { return -2; }
    try { return _qinfo_rcv.so->ringbuf.ipush() > 0 ? 1 : 0; }
      catch (...) { return -2; } // should not occur
  }
  return -1;
}
  // NOTE _x_qinfo_rcv_init depends on _tms_next_init.
  //  After failure, it does real init. attempt only when pre-calculated _tms_next_init is reached.
bool lm_slot_controller::_x_qinfo_rcv_init() __bmdx_noex
{
  const double t = clock_ms();
    if (t < _tms_next_init) { return false; }

  if (!_b_qinfo_rcv_valid) // ensure initializing the shared part of _qinfo_rcv
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_qinfo_rcv_state); if (sizeof(__lock)) {}
    if (_qinfo_rcv.so.f_constructed() != 1)
    {
      s_long res = _qinfo_rcv.attempt_init(); // NOTE If init. successful, res == 1, and, below queue aux data init. will be done
      if (res < 1) { _tms_next_init = t + __lm_slot_controller_reinit_short_dtms; return false; } // will try again later
    }

    _r_prc_uid& qids_shared = _qinfo_rcv.so->auxd();
    if (qids_shared.sig() != _r_prc_uid::Fsig()) // the queue is just created; check signature; update process and session info
    {
      _qinfo_rcv.close_shm();
      _tms_next_init = t + __lm_slot_controller_reinit_long_dtms;
      return false;
    }
    _b_qinfo_rcv_valid = true;
    qids_shared.cp_ids_to_volatile(_qinfo_rcv.auxd_init, false);
  }

  if (1) // here: f_constructed() == 1
  {
    _r_prc_uid& qids_shared = _qinfo_rcv.so->auxd();

    if (_qinfo_rcv.auxd_init.disp_ses_state() != qids_shared.disp_ses_state()) { _x_qinfo_rcv_update_ses_state(); }

    cpparray_t<netmsg_header::md_entry> __mdee2;
    netmsg_header::md_entry __mdee1[nmdemax];

      // Update livecnt.
    if (1) { s_ll cnt = qids_shared.livecnt(); ++cnt; qids_shared.livecnt(cnt); }
      // Pop and process all available tech. msgs.
    if (_qinfo_rcv.vdd.n() == 0)
    {
      _qinfo_rcv_idd0 = 0;
      _qinfo_rcv.mpop_avl(); // NOTE this call is not under lock, because it internally will not do queue initialization, because f_constructed() == 1 already (ensured above)
    }
    if (_qinfo_rcv_idd0 < _qinfo_rcv.vdd.n())
    {
      for (s_long i = _qinfo_rcv_idd0; i < _qinfo_rcv.vdd.n(); ++i)
      {
        netmsg_header hdr; t_stringref orig = _qinfo_rcv.vdd[i]._rnonc();
        if (hdr.fields_decode(orig, fpt_md))
        {
          arrayref_t<netmsg_header::md_entry> mdee(__mdee1, hdr.get_mde(arrayref_t<netmsg_header::md_entry>(__mdee1, nmdemax), true));
          if (mdee.n() > nmdemax)
          {
            if (__mdee2.n() < mdee.n() && !__mdee2.resize(mdee.n())) { goto lBreak1; }
            mdee.link(__mdee2.pd(), hdr.get_mde(arrayref_t<netmsg_header::md_entry>(__mdee2.pd(), mdee.n()), true));
          }
          for (s_long imde = 0; imde < mdee.n(); ++imde)
          {
            netmsg_header::md_entry& e = mdee[imde];
            if (e.type == 1 || e.type == 2) { if (_hnew_name_peerf.insert(bmdx_shm::t_name_shm(e.data.pd(), e.data.n())) < 0) { goto lBreak1; } }
            else {}
          }
        }
        _qinfo_rcv_idd0 = i + 1;
      }
lBreak1:;
    }
    if (_qinfo_rcv_idd0 >= _qinfo_rcv.vdd.n()) { if (_qinfo_rcv.vdd.n() > 0) { _qinfo_rcv.vdd.clear(); } _qinfo_rcv_idd0 = 0; }
  }
  return true;
}
  // WARNING _x_pt_getcr uses lkd_peer_hpt. It may not be called when any lkd_pt_access is set.
cref_t<lm_slot_controller::peer_tracker> lm_slot_controller::_x_pt_getcr(const bmdx_shm::t_name_shm& name_peerf, bool b_comm_once, bool b_autocreate) __bmdx_noex
{
  if (name_peerf.n() > 0)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_peer_hpt); if (sizeof(__lock)) {}
    const hashx<bmdx_shm::t_name_shm, cref_t<peer_tracker> >::entry* e = _hpt.find(name_peerf);
    do { // once
      if (!e) { if (b_autocreate) { _hpt.insert(name_peerf, &e); } }
      if (!e) { break; }
      cref_t<peer_tracker>& pt = e->v;
      const bool b_newpt = !pt;
      if (!pt) { pt.create3(1, this->_name_prf, name_peerf, _nb_qum); }
      if (!pt) { break; }
      if (b_newpt && b_comm_once) { pt->_cltpt_ensure_comm(false, true); }
      return pt;
    } while (false);
  }
  return cref_t<peer_tracker>();
}
  // NOTE det_init() is called 1st time right after LMSC creation, in dispatcher_mt::thread_proxy::_s_disp_ctor.
  //  Servicing thread is not run yet at this time.
  //  If init. fails, it will be retried on the next call to det_init(). See also _x_qinfo_rcv_init.
void lm_slot_controller::det_init() __bmdx_noex
{
  if (!(_state == 0 || _state == -2)) { return; }

  if (_state == 0)
  {
    _r_prc_uid& qids_init = _qinfo_rcv.auxd_init;
    qids_init.pid((bmdx_meta::u_ll)processctl::ff_mc().pid_self());
    qids_init.pid_deriv(pid_deriv_v1(qids_init.pid()));
    if (1)
    {
      static s_ll n0 = 0; ++n0;
      union { s_ll n; double t; };
      n = 0; t = clock_ms(); n += n0; n ^= qids_init.pid();
      if (n == 0 || n == -1) { n = n0; }
      qids_init.prc_inst_id(n);
    }
    qids_init.livecnt(0);
  }
  if (!_x_qinfo_rcv_init()) { _state = -2; return; }


  try {
    if (_apns.isArray()) { for (s_long i = _apns.arrlb(); i <= _apns.arrub(); ++i) { std::wstring pn = _apns.vstr(i); if (pn.empty()) { continue; } _x_pt_getcr(make_fixed_utf8_name(pn), true, true); } }
      else if (_apns.isAssoc()) { for (s_long pos = _apns.assocl_first(); pos != _apns.assocl_noel(); pos = _apns.assocl_next(pos)) { std::wstring pn = _apns.assocl_key(pos).vstr(); if (pn.empty()) { continue; } _x_pt_getcr(make_fixed_utf8_name(pn), true, true); } }
  } catch (...) { _state = -2; return; }

  _state = 1;
}

  // NOTE clt_setparam_chsbin must be called once, before 1st call to det_init().
void lm_slot_controller::clt_setparam_chsbin(bool b_enable) { s_long f = fpt_md | fpt_msg; if (b_enable) { f |= fpt_bin; } _fl_chs = f; }

void lm_slot_controller::clt_set_ses_state(s_long st)
{
  critsec_t<lm_slot_controller> __lock(10, -1, &lkd_qinfo_rcv_state); if (sizeof(__lock)) {}
  _qinfo_rcv.auxd_init.disp_ses_state(st);
  _x_qinfo_rcv_update_ses_state();
}
void lm_slot_controller::_x_qinfo_rcv_update_ses_state() __bmdx_noex
{
  if (_b_qinfo_rcv_valid)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_qinfo_rcv_state); if (sizeof(__lock)) {}
    try { _qinfo_rcv.so->auxd().disp_ses_state(_qinfo_rcv.auxd_init.disp_ses_state()); } catch (...) {}
  }
}
void lm_slot_controller::_x_qinfo_rcv_mark_process_closed() __bmdx_noex
{
  if (_b_qinfo_rcv_valid)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_qinfo_rcv_state); if (sizeof(__lock)) {}
    try { _qinfo_rcv.so->auxd().prc_inst_id(-1); } catch (...) {}
  }
}

  // Ensures creating peer_tracker and establishing the communication with that peer, as specified by comm_mode.
  // comm_mode: see clt_mdsend.
  // Returns:
  //    1 - success.
  //    -2 - failed to create or access peer tracker.
  //    -3 - the peer process is not available.
  //    -14 - peer process appears to be hanged or terminated.
s_long lm_slot_controller::clt_ensure_comm(arrayref_t<wchar_t> name_peer, s_long comm_mode) __bmdx_noex
{
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, true); if (!pt) { return -2; }
  bool b_comm_wait = comm_mode == 1;
  if (comm_mode == 0) { b_comm_wait = pt->_b_clt_mdsend_wait_once; }
  s_long res_comm = pt->_cltpt_ensure_comm(b_comm_wait, false);
    if (comm_mode == 0) { pt->_b_clt_mdsend_wait_once = false; }
    if (res_comm < 0) { return -3; }
    if (res_comm == 0) { return -14; }
    return 1;
}
  // mtrk_id_msg:
  //    unique ID of the message being sent.
  // id_msg_cmd_sender:
  //    original command message id (sent from pbo, hbo), associated with the current response;
  //    for dflt. and none: -1
  // flags_mtrk: ORed flags, see also md_entry type 54:
  //    1 - user message tracking is required,
  //    2 - command slot state update notification is required.
  // flags_clt_msend:
  //    same as _s_write (flags_msend & _fl_msend__mask_clt_msend).
  //    See _fl_msend__mask_clt_msend.
  // rhtrprx: should be valid ref. to mtrk_htracking object of the proxy that is the current clt_msend client.
  // Returns:
  //    1 - success.
  //    -2 - common-case failure.
  //        NOTE This code is particularly returned:
  //          1) if peer process is not available (e.g. not created yet).
  //          2) if peer process has no valid dispatcher session on its side. Session state must be 1 (working).
  //    -14 - (only: with b_ignore_hanged == false): failed to send because peer process appears to be hanged or terminated.
s_long lm_slot_controller::clt_msend(s_ll mtrk_id_msg, s_ll id_msg_cmd_sender, unsigned char flags_mtrk, arrayref_t<wchar_t> name_peer, arrayref_t<wchar_t> msg, const cref_t<t_stringref>& bin, s_long flags_clt_msend, const weakref_t<t_htracking_proxy>& rhtrprx) __bmdx_noex
{
  if (mtrk_id_msg == -1) { return -2; }
  const bool b_ignore_hanged = !!(flags_clt_msend & dispatcher_mt_flags::ignore_hanged);
  const bool b_chsbin = bin && ((flags_clt_msend & dispatcher_mt_flags::use_chsbin) || (_fl_chs & fpt_chsbin));
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, true); if (!pt) { return -2; }
  s_long res_comm = pt->_cltpt_ensure_comm(true, false);
    if (res_comm < 0) { return -2; }
    if (res_comm == 0 && !b_ignore_hanged) { return -14; }

  netmsg_header::msg_builder mb(fpt_md | fpt_msg | (bin ? fpt_bin : 0) | (b_chsbin ? fpt_chsbin : 0));
  if (1)
  {
    mdd_id_msg d; d.id_msg.set(mtrk_id_msg); d.flags.set(flags_mtrk);
    if (!(mb.set_md1(d.type(), t_stringref(d, sizeof(d))))) { return -2; }
  }
  if (id_msg_cmd_sender >= 0)
  {
    mdd_id_msg_cmd d; d.id_msg.set(id_msg_cmd_sender);
    if (!(mb.set_md1(d.type(), t_stringref(d, sizeof(d))))) { return -2; }
  }
  if (!mb.set_umsg(msg)) { return -2; }
  if (bin && !mb.set_bin_info(bin.ref())) { return -2; }
  if (!mb.set_final()) { return -2; }
  if (1)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &pt->lkd_pt_access); if (sizeof(__lock)) {}
    try {
      s_ll st = pt->_qinfo_send.so->auxd().disp_ses_state();
      if (st == 1) {}
      else if (st == 0)
      {
        if (!b_ignore_hanged) { return -14; }
      }
      else { return -2; }
    } catch (...) {}
    _mtrk_fn_handler ha(pt._rnonc(), mtrk_id_msg, rhtrprx);
    if (flags_mtrk != 0 && !ha.mtrk_prep()) { return -2; }
    s_long res = 0;
    s_ll ipush_lq = -1;
    if (bin) { res = pt->_qum_send.msend(bin, 0, mb.msg, &ipush_lq); }
      else { res = pt->_qum_send.msend(mb.msg, 0, cref_t<t_stringref>(), &ipush_lq); }
    if (res < 1) { return -2; }
    if (flags_mtrk != 0) { ha.mtrk_prep2(ipush_lq); }
  }
  return 1;
}
  // Sends pre-composed technical message, with tracking.
  //    msg must 1) be complete, 2) contain metadata, 3) must not contain user message or binary part.
  // mtrk_id: required tracking ID, must be != -1.
  // rhtrprx: should be valid ref. to mtrk_htracking object of the proxy that is the current clt_msend client.
  // comm_mode:
  //    1: ensure peer being communicable (try and wait for it's reaction till timeout), and then send the message.
  //    0: once for this peer, do same as comm_mode 1, all the next times do same as comm_mode -1.
  //    -1: try comm. once. If peer is communicable, send the message. Otherwise return code -2 (don't wait).
  // Returns:
  //    1 - success.
  //    -2 - common-case failure.
  //        NOTE This code is particularly returned:
  //          1) if peer process is not available (e.g. not created yet).
  //          2) if peer process session state is not valid for technical messages. Must be 1 (working) or -1 (late initialization).
  //    -14 - failed to send because peer process appears to be hanged or terminated.
s_long lm_slot_controller::clt_mdsend(s_ll mtrk_id, arrayref_t<wchar_t> name_peer, const netmsg_header::msg_builder& msg, const weakref_t<t_htracking_proxy>& rhtrprx, s_long comm_mode) __bmdx_noex
{
  if (mtrk_id == -1) { return -2; }
  if (!msg.b_complete()) { return -2; }
  const s_long parts = msg.parts(); if ((parts & (fpt_msg | fpt_bin)) || !(parts & fpt_md)) { return -2; }
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, true); if (!pt) { return -2; }
  bool b_comm_wait = comm_mode == 1;
  if (comm_mode == 0) { b_comm_wait = pt->_b_clt_mdsend_wait_once; }
  s_long res_comm = pt->_cltpt_ensure_comm(b_comm_wait, false);
    if (comm_mode == 0) { pt->_b_clt_mdsend_wait_once = false; }
    if (res_comm < 0) { return -2; }
    if (res_comm == 0) { return -14; }

  if (1)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &pt->lkd_pt_access); if (sizeof(__lock)) {}
    try { s_ll st = pt->_qinfo_send.so->auxd().disp_ses_state(); if (!(st == 1 || st == -1)) { return -2; } } catch (...) {}
    _mtrk_fn_handler ha(pt._rnonc(), mtrk_id, rhtrprx);
    if (!ha.mtrk_prep()) { return -2; }
    s_ll ipush_lq = -1;
    s_long res = pt->_qum_send.msend(msg.msg, 0, cref_t<t_stringref>(), &ipush_lq);
    if (res < 1) { return -2; }
    ha.mtrk_prep2(ipush_lq);
  }
  return 1;
}

  // clt_qum_conf_set_*:
  // For the existing peer tracker object, sets its shared queue parameters as specified.
  //  NOTE If b_forced == false, clt_qum_conf_reset does not force reset,
  //    it makes any changes/delays only if pt->_qum_*_conf_reset_on_exit == true.
  // Returns:
  //    1 - success.
  //    -2 - failed to create or access peer tracker.
s_long lm_slot_controller::clt_qum_conf_reset(arrayref_t<wchar_t> name_peer, bool b_rcv, bool b_forced) __bmdx_noex
{
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, false);
    if (!pt) { return -2; }
    if (pt->_state == -4) { return -2; }
  critsec_t<lm_slot_controller> __lock(10, -1, &pt->lkd_pt_access); if (sizeof(__lock)) {}
    if (pt->_state == -4) { return -2; }
  pt->_cltpt_qum_conf_reset(b_rcv, b_forced);
  return 1;
}
s_long lm_slot_controller::clt_qum_conf_set_rcv(arrayref_t<wchar_t> name_peer, bool b_al, bool b_lqcap, s_long& res_al, s_long& res_lqcap, const cref_t<bmdx_shm::i_allocctl>* p_al, double timeout_ms_al, s_ll ncapmin, s_ll ncapmax, s_ll nrsv, double timeout_ms_lqcap) __bmdx_noex
{
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, true);
    if (!pt) { return -2; }
    s_long st = pt->_state;
    if (!(st == 0 || st == 1 || st == -2)) { return -2; }
  critsec_t<lm_slot_controller> __lock(10, -1, &pt->lkd_pt_access); if (sizeof(__lock)) {}
    st = pt->_state;
    if (!(st == 0 || st == 1 || st == -2)) { return -2; }
  res_al = 0; res_lqcap = 0;
  if (b_al || b_lqcap) { pt->_qum_rcv_conf_reset_on_exit = true; }
  if (b_al) { res_al = pt->_qum_rcv.conf_set_al_in(p_al, timeout_ms_al); }
  if (b_lqcap) { res_lqcap = pt->_qum_rcv.conf_set_lqcap(true, ncapmin, ncapmax, nrsv, timeout_ms_lqcap); }
  return 1;
}
s_long lm_slot_controller::clt_qum_conf_set_send(arrayref_t<wchar_t> name_peer, s_long& res_lqcap, s_ll ncapmin, s_ll ncapmax, s_ll nrsv, double timeout_ms_lqcap) __bmdx_noex
{
  cref_t<peer_tracker> pt = _x_pt_getcr(make_fixed_utf8_name(name_peer), false, true);
    if (!pt) { return -2; }
    s_long st = pt->_state;
    if (!(st == 0 || st == 1 || st == -2)) { return -2; }
  critsec_t<lm_slot_controller> __lock(10, -1, &pt->lkd_pt_access); if (sizeof(__lock)) {}
    if (!(st == 0 || st == 1 || st == -2)) { return -2; }
  //res_lqcap = 0;
  if (1) { pt->_qum_send_conf_reset_on_exit = true; }
  if (1) { res_lqcap = pt->_qum_send.conf_set_lqcap(false, ncapmin, ncapmax, nrsv, timeout_ms_lqcap); }
  return 1;
}
void lm_slot_controller::peer_tracker::_cltpt_qum_conf_reset(bool b_rcv, bool b_forced) __bmdx_noex
{
  bool b1 = b_rcv && (b_forced || this->_qum_rcv_conf_reset_on_exit);
  bool b2 = !b_rcv && (b_forced || this->_qum_send_conf_reset_on_exit);
  if (b1 || b2)
  {
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_pt_access); if (sizeof(__lock)) {}
    if (b1)
    {
      cref_t<bmdx_shm::i_allocctl> empty;
      this->_qum_rcv.conf_set_al_in(&empty, -1);
      this->_qum_rcv.conf_set_lqcap(true, 0, -2, -3, -1);
      this->_qum_rcv_conf_reset_on_exit = false;
    }
    if (b2)
    {
      this->_qum_send.conf_set_lqcap(false, 0, -2, -3, -1);
      this->_qum_send_conf_reset_on_exit = false;
    }
  }
}


  // NOTE By design, _x_close() is called 1st time right before exit
  //  from servicing LMSC thread.
  //  Formally, _x_close() is also called from ~lm_slot_controller(),
  //  but this call does nothing because of the above.
  //  (_thread_proc, servicing LMSC, exits when notices dispatcher session end and/or thread stop flag set.)
void lm_slot_controller::_x_close() __bmdx_noex
{
  if (_state == -4) { return; }
  if (_state != 1) { _state = -4; return; }
  _state = -3;
  double t0 = clock_ms();
  bool b_1st = true;
  while (1)
  {
    double t1 = clock_ms();
    critsec_t<lm_slot_controller> __lock(10, -1, &lkd_peer_hpt); if (sizeof(__lock)) {}
    if (b_1st)
    {
      for (s_long i = _hpt.n() - 1; i >= 0; --i) { cref_t<peer_tracker>& pt = _hpt(i)->v; pt->_cltpt_close(false); } // initiates each peer tracker closing (fast)
      b_1st = false;
    }
    if (_hpt.n() > 0) { for (s_long i = _hpt.n() - 1; i >= 0; --i) {
      cref_t<peer_tracker>& pt = _hpt(i)->v;
      bool b_tmo = t1 - t0 > (__bmdx_disp_lq_cleanup_dtms + 200);
      if ((pt->_cltpt_close(b_tmo) || b_tmo)  && pt.n_refs() <= 1) { _hpt.remove_i(i); }
    } }
    if (_hpt.n() <= 0) { break; }
    sleep_mcs(1000, 1);
  }
  _x_qinfo_rcv_mark_process_closed();
  _state = -4;
}
  // Converts any wchar_t to a char of subset of programmatic identifier: [a-zA-Z0-9_].
char lm_slot_controller::make_identifier_char(wchar_t c0)  __bmdx_noex
{
  static const char* const map = "abcdefghijklmnop0123456789abcdefgABCDEFGHIJKLMNOPQRSTUVWXYZhijk_labcdefghijklmnopqrstuvwxyzmnopq";
  unsigned int c = (unsigned int)c0 & 0xffff; c ^= c >> 8; c &= 0x7f; if (c < 32) { c += '0'; } c -= 32;
  return map[c];
}
  // Converts an arbitrary name (wide string) to fixed-length string, suitable for unique naming the corresponding shared object.
  //  The resulting string consists of concatenated:
  //    1) __lm_slot_controller_name_prefix,
  //    2) 4-byte hash of the given name, converted with make_identifier_char() into symbolic form (also 4 characters),
  //    3) characters of the given name itself, converted with make_identifier_char() into symbolic form, up to least of name length and t_name_shm capacity.
bmdx_shm::t_name_shm lm_slot_controller::__make_name_one_impl(arrayref_t<char> name) __bmdx_noex
{
  if (!name.is_valid()) { return bmdx_shm::t_name_shm(); }
  t_stringref prefix(__lm_slot_controller_name_prefix);
  s_long h = yk_c::hashx_common::_skf_pchars<char>().Fhash(name.pd(), name.n());
  const s_long nc3 = (s_long)bmdx_minmax::myllmin(name.n(), __lm_slot_controller_nbmax_peer_name_root);
  const s_long np = s_long(prefix.n());
  const s_long i3 = s_long(np + 4);
  //
  bmdx_shm::t_name_shm x; x.resize(i3 + nc3);
  for (s_long i = 0; i < np; ++i) { x[i] = prefix[i]; }
  bmdx_str::words::set_be4(&x[np], 0, h); for (s_long i = 0; i < 4; ++i) { x[np + i] = make_identifier_char(x[np + i]); }
  for (s_long i = 0; i < nc3; ++i) { x[i3 + i] = make_identifier_char(name[i]); }
  return x;
}
  // Converts two names (wide strings) to fixed-length string, suitable for unique naming the shared object,
  //    associated with combination of that names.
  //  The resulting string consists of concatenated:
  //    1) __lm_slot_controller_name_prefix,
  //    2) 4-byte hash of name1, converted with make_identifier_char() into symbolic form (4 characters),
  //    3) 4-byte hash of name2, converted with make_identifier_char() into symbolic form (4 characters),
  //    4) characters of name1 (max. __lm_slot_controller_nbmax_peer_name_root, which is less than 1/2 of t_name_shm capacity),
  //    5) characters of name2 (max. __lm_slot_controller_nbmax_peer_name_root, which is less than 1/2 of t_name_shm capacity).
  // See also make_name_one().
bmdx_shm::t_name_shm lm_slot_controller::__make_name_two_impl(arrayref_t<char> name1, arrayref_t<char> name2) __bmdx_noex
{
  if (!(name1.is_valid() && name2.is_valid())) { return bmdx_shm::t_name_shm(); }
  t_stringref prefix(__lm_slot_controller_name_prefix);
  s_long h1 = yk_c::hashx_common::_skf_pchars<char>().Fhash(name1.pd(), name1.n());
  s_long h2 = yk_c::hashx_common::_skf_pchars<char>().Fhash(name2.pd(), name2.n());
  const s_long nc4 = (s_long)bmdx_minmax::myllmin(name1.n(), __lm_slot_controller_nbmax_peer_name_root);
  const s_long nc5 = (s_long)bmdx_minmax::myllmin(name2.n(), __lm_slot_controller_nbmax_peer_name_root);
  const s_long np = s_long(prefix.n());
  const s_long i3 = s_long(np + 4);
  const s_long i4 = s_long(i3 + 4);
  const s_long i5 = s_long(i4 + nc4);
  //
  bmdx_shm::t_name_shm x; x.resize(i5 + nc5);
  for (s_long i = 0; i < np; ++i) { x[i] = prefix[i]; }
  bmdx_str::words::set_be4(&x[np], 0, h1); for (s_long i = 0; i < 4; ++i) { x[np + i] = make_identifier_char(x[np + i]); }
  bmdx_str::words::set_be4(&x[i3], 0, h2); for (s_long i = 0; i < 4; ++i) { x[i3 + i] = make_identifier_char(x[i3 + i]); }
  for (s_long i = 0; i < nc4; ++i) { x[i4 + i] = make_identifier_char(name1[i]); }
  for (s_long i = 0; i < nc5; ++i) { x[i5 + i] = make_identifier_char(name2[i]); }
  return x;
}

}
#endif // bmdx_part_dispatcher_mt
} // namespace bmdx_main_intl_lib

#ifdef _MSC_VER
  #pragma warning(pop)
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif // bmdx_main_intl_lib_H
















#ifndef __bmdx_main_impl_skip

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wunused-parameter"
  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"
  #pragma clang diagnostic ignored "-Wunused-function"
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#if defined(__GNUC__) && !defined(__clang__)
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

using namespace bmdx_main_intl_lib;
using namespace bmdx_str::words;
using namespace bmdx_str::conv;

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits>
#include <clocale>
#include <locale.h>
#include <ctype.h>
#include <cwctype>
#include <cerrno>

// NOTE The below header is not used, because it does not supply wcsncasecmp_l together with other functions (like towlower_l).
#if defined(__FreeBSD__)
  #include <xlocale.h>
#endif

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
  struct wsbs_lks {};
  typedef critsec_t<wsbs_lks> t_critsec_locale;

  #ifdef _bmdxpl_Wnds
      #if defined(__BORLANDC__) || (defined(__GNUC__) && !defined(__clang__))
        #define __bmdx_char_case_tables 1
        #define __bmdx_use_locale_t 0
      #else
        #define __bmdx_char_case_tables 0
        #define __bmdx_use_locale_t 1
      #endif
      #define __bmdx_use_wcrtomb_l 0
  #endif

  #ifdef _bmdxpl_Psx

      // NOTE Both (__bmdx_use_locale_t, __bmdx_char_case_tables) 0 may be used only in Android.
      // NOTE Defining __bmdx_char_case_tables 1 replaces certain part of usages, enabled by __bmdx_use_locale_t 1,
      //    but does not completely suppress them all.

      #if defined(__FreeBSD__)
        #define __bmdx_char_case_tables 1
        #define __bmdx_use_locale_t 1
        #define __bmdx_use_wcrtomb_l 1
      #elif (__APPLE__ && __MACH__)
        #if defined(__clang__)
          #define __bmdx_char_case_tables 0
          #define __bmdx_use_locale_t 1
          #define __bmdx_use_wcrtomb_l 1
        #else
          #define __bmdx_char_case_tables 1
          #define __bmdx_use_locale_t 0
          #define __bmdx_use_wcrtomb_l 0
        #endif
      #elif defined(__ANDROID__)
        #define __bmdx_char_case_tables 1
        #define __bmdx_use_locale_t 0
        #define __bmdx_use_wcrtomb_l 0
      #elif defined(__SUNPRO_CC) || defined(__sun)
        #define __bmdx_char_case_tables 1
        #define __bmdx_use_locale_t 0
        #define __bmdx_use_wcrtomb_l 0
      #else
        #define __bmdx_char_case_tables 0
        #define __bmdx_use_locale_t 1
        #define __bmdx_use_wcrtomb_l 0
      #endif
  #endif

  #if !__bmdx_use_locale_t
      #define __bmdx_setlocale_name ""
      s_long wsbs_lk_dt() { return 10; } // for _wsToBs, _bsToWs only
      namespace{ __bmdx_GNUC_allow_unused bool ensure_loc(s_long loc_type) { return false;  } }
  #else
    #ifdef _bmdxpl_Wnds
      s_long wsbs_lk_dt() { return -1; } // for _wsToBs, _bsToWs only

      #define __bmdx_locale_t_null ((_locale_t)0)
      #define __bmdx_tolower_l _tolower_l
      #define __bmdx_towlower_l _towlower_l
      #define __bmdx_toupper_l _toupper_l
      #define __bmdx_towupper_l _towupper_l

      struct _locale_t_base { _locale_t h; };
      struct _locale_t_handle : _locale_t_base
      {
        void ensure_init(int cat, const char* loc) { if (!b_valid()) { t_critsec_locale __lock(10, -1); if (sizeof(__lock)) {} if (!b_valid()) { h = _create_locale(cat, loc); } } }
        bool b_valid() const { return h != __bmdx_locale_t_null; }
        _locale_t_handle() { _reset(); }
        ~_locale_t_handle() { if (b_valid()) { t_critsec_locale __lock(10, -1); if (b_valid()) { _free_locale(h); _reset(); } } }
        private:
          _locale_t_handle(const _locale_t_handle&); void operator=(const _locale_t_handle&);
          void _reset() { h = __bmdx_locale_t_null; }
      };
      namespace
      {
        static _locale_t_base __hlocale[3] = { { __bmdx_locale_t_null }, { __bmdx_locale_t_null }, { __bmdx_locale_t_null } };
        inline _locale_t_handle* phloc(s_long lt) { return static_cast<_locale_t_handle*>(&__hlocale[lt]); }
        bool ensure_loc(s_long loc_type)
        {
          if (loc_type == 0) { return false; } // this loc. is not created ever, because the current locale is not constant in the program
            else if (loc_type == 1) { if (!phloc(1)->b_valid()) { phloc(1)->ensure_init(LC_ALL, ""); } }
            else if (loc_type == 2) { if (!phloc(2)->b_valid()) { phloc(2)->ensure_init(LC_ALL, "C"); } }
            else { return false; }
          return phloc(loc_type)->b_valid();
        }
      }
    #endif
    #ifdef _bmdxpl_Psx
      #if __bmdx_use_wcrtomb_l
        #define __bmdx_setlocale_name 0
        s_long wsbs_lk_dt() { return -1; } // for _wsToBs, _bsToWs only
      #else
        #define __bmdx_setlocale_name ""
        s_long wsbs_lk_dt() { return 10; } // for _wsToBs, _bsToWs only
      #endif

      #define __bmdx_locale_t_null ((locale_t)0)
      #define __bmdx_tolower_l tolower_l
      #define __bmdx_towlower_l towlower_l
      #define __bmdx_toupper_l toupper_l
      #define __bmdx_towupper_l towupper_l

      struct _locale_t_base { locale_t h; };
      struct _locale_t_handle : _locale_t_base
      {
        void ensure_init(int cat, const char* loc) { if (!b_valid()) { t_critsec_locale __lock(10, -1); if (sizeof(__lock)) {} if (!b_valid()) { h = newlocale(cat, loc, __bmdx_locale_t_null); } } }
        bool b_valid() const { return h != __bmdx_locale_t_null; }
        _locale_t_handle() { _reset(); }
        ~_locale_t_handle() { if (b_valid()) { t_critsec_locale __lock(10, -1); if (b_valid()) { freelocale(h); _reset(); } } }
        private:
          _locale_t_handle(const _locale_t_handle&); void operator=(const _locale_t_handle&);
          void _reset() { h = __bmdx_locale_t_null; }
      };
      namespace
      {
        static _locale_t_base __hlocale[3] = { { __bmdx_locale_t_null }, { __bmdx_locale_t_null }, { __bmdx_locale_t_null } };
        inline _locale_t_handle* phloc(s_long lt) { return static_cast<_locale_t_handle*>(&__hlocale[lt]); }
        bool ensure_loc(s_long loc_type)
        {
          if (loc_type == 0) { return false; } // this loc. is not created ever, because the current locale is not constant in the program
            else if (loc_type == 1) { if (!phloc(1)->b_valid()) { phloc(1)->ensure_init(LC_CTYPE_MASK | LC_COLLATE_MASK, ""); } }
            else if (loc_type == 2) { if (!phloc(2)->b_valid()) { phloc(2)->ensure_init(LC_CTYPE_MASK | LC_COLLATE_MASK, "C"); } }
            else { return false; }
          return phloc(loc_type)->b_valid();
        }
      }
    #endif
    #ifdef _bmdxpl_OS_NAME_HERE
        s_long wsbs_lk_dt() { } // for _wsToBs, _bsToWs only
    #endif
  #endif

  _fls75 __locname_du;
  bool __b_locname_du(false);
  struct t_lock_locale
  {
    int cat;
    _fls75 name0;
    t_critsec_locale __lock;
    t_lock_locale(int cat_, const char* locname_, s_ll lk_dtmcs)
      : cat(-1), name0(), __lock(lk_dtmcs, -1)
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
    ~t_lock_locale() { if (cat >= 0) { std::setlocale(cat, name0.c_str()); } }
  };


char __bmdx_tolower_c(char c) { enum { n = 256 }; static char d[n]; static bool b_init(0); if (!b_init) { t_lock_locale __lock(LC_ALL, "C", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = char(tolower(char(i))); } b_init = 1; } return d[(unsigned char)c]; }
char __bmdx_toupper_c(char c) { enum { n = 256 }; static char d[n]; static bool b_init(0); if (!b_init) { t_lock_locale __lock(LC_ALL, "C", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = char(toupper(char(i))); } b_init = 1; } return d[(unsigned char)c]; }
namespace { struct __init_char_case_tables1 { __init_char_case_tables1() { __bmdx_tolower_c(0); __bmdx_toupper_c(0); } }; __init_char_case_tables1 __inst___init_char_case_tables1; }
#if __bmdx_char_case_tables
  wchar_t __bmdx_towlower_sys(wchar_t c0)
  {
    s_long c1 = c0; c1 &= 0xffff;
    if (wchar_t(c1) != c0) { return c0; }
    enum { n = 65536 }; static bmdx_meta::s_short d[n]; static bool b_init(0);
    if (!b_init) { t_lock_locale __lock(LC_ALL, "", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = towlower(i & 0x8000 ? i | ~int(0xffff) : i); } b_init = 1; }
    return wchar_t(d[c1]) & 0xffff;
  }
  wchar_t __bmdx_towupper_sys(wchar_t c0)
  {
    s_long c1 = c0; c1 &= 0xffff;
    if (wchar_t(c1) != c0) { return c0; }
    enum { n = 65536 }; static bmdx_meta::s_short d[n]; static bool b_init(0);
    if (!b_init) { t_lock_locale __lock(LC_ALL, "", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = towupper(i & 0x8000 ? i | ~int(0xffff) : i); } b_init = 1; }
    return wchar_t(d[c1]) & 0xffff;
  }

  wchar_t __bmdx_towlower_c(wchar_t c0)
  {
    s_long c1 = c0; c1 &= 0x7f;
    if (wchar_t(c1) != c0) { return c0; }
    enum { n = 128 }; static bmdx_meta::s_short d[n]; static bool b_init(0);
    if (!b_init) { t_lock_locale __lock(LC_ALL, "C", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = towlower(i); } b_init = 1; }
    return d[c1];
  }
  wchar_t __bmdx_towupper_c(wchar_t c0)
  {
    s_long c1 = c0; c1 &= 0x7f;
    if (wchar_t(c1) != c0) { return c0; }
    enum { n = 128 }; static bmdx_meta::s_short d[n]; static bool b_init(0);
    if (!b_init) { t_lock_locale __lock(LC_ALL, "C", 10); if (sizeof(__lock)) {} for (int i = 0; i < n; ++i) { d[i] = towupper(i); } b_init = 1; }
    return d[c1];
  }
  namespace { struct __init_char_case_tables2 { __init_char_case_tables2() { __bmdx_towlower_sys(0); __bmdx_towupper_sys(0); __bmdx_towlower_c(0); __bmdx_towupper_c(0); } }; __init_char_case_tables2 __inst___init_char_case_tables2; }
#endif

namespace {
  static int __wcsncasecmp_curr(const wchar_t* s1, const wchar_t* s2, s_ll n)
  {
    if (n <= 0) { return 0; }
    wchar_t c1, c2;
    for (; *s1; s1++, s2++) { c1 = towlower(*s1); c2 = towlower(*s2); if (c1 != c2) { return (int)c1 - c2; } if (--n == 0) { return 0; } }
    return -*s2;
  }
  #if __bmdx_char_case_tables
    __bmdx_GNUC_allow_unused static int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, s_ll n, s_long loc_type)
    {
      if (loc_type == 0) { return __wcsncasecmp_curr(s1, s2, n); }
      if (n <= 0) { return 0; }
      wchar_t c1, c2;
      if (loc_type == 1) { for (; *s1; s1++, s2++) { c1 = __bmdx_towlower_sys(*s1); c2 = __bmdx_towlower_sys(*s2); if (c1 != c2) { return (int)c1 - c2; } if (--n == 0) { return 0; } } }
        else if (loc_type == 2) { for (; *s1; s1++, s2++) { c1 = __bmdx_towlower_c(*s1); c2 = __bmdx_towlower_c(*s2); if (c1 != c2) { return (int)c1 - c2; } if (--n == 0) { return 0; } } }
      return -*s2;
    }
  #elif __bmdx_use_locale_t
    #ifdef _bmdxpl_Psx
      __bmdx_GNUC_allow_unused static int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, s_ll n, s_long loc_type)
      {
        if (loc_type == 0) { return __wcsncasecmp_curr(s1, s2, n); }
        if (!(loc_type >= 1 && loc_type <= 2 && phloc(loc_type)->b_valid())) { return 0; } // NOTE the caller must not pass wrong loc_type
        return wcsncasecmp_l(s1, s2, (size_t)n, phloc(loc_type)->h);
      }
    #endif
  #else
    #ifdef _bmdxpl_Psx
        // Android-specific.
      __bmdx_GNUC_allow_unused static int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, s_ll n, s_long)
      {
        wchar_t c1, c2;
        if (n <= 0) { return 0; }
        for (; *s1; s1++, s2++)
        {
          c1 = wchar_t(towlower(*s1)); c2 = wchar_t(towlower(*s2));
          if (c1 != c2) { return (int)c1 - c2; }
          if (--n == 0) { return 0; }
        }
        return -*s2;
      }
    #endif
  #endif
}

}

namespace bmdx
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal utilities.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // NOTE This cannot trim \0 char, but more efficient since does not create additional string argument.
  template<class Char>
  static arrayref_t<Char> _trim_arrayref(const arrayref_t<Char>& s_, const Char* pwhat, bool b_left = true, bool b_right = true)
  {
    arrayref_t<Char> s_empty(s_.pd(), 0);
    if (s_.is_empty() || !pwhat) { return s_empty; }
    s_ll lw = 0;
      const Char* p = pwhat; while (*p++) { ++lw; }
    const s_ll ls = s_ll(s_.n());
      if (ls < lw) { return s_; }
    s_ll pos1(0); s_ll pos2(ls);
    if (b_left) { while (pos1 <= ls - lw) { if (0 == std::memcmp(&s_[pos1], pwhat, size_t(lw) * sizeof(Char))) { pos1 += lw; } else { break; } } }
    if (b_right) { while (pos2 > pos1 + lw) { if (0 == std::memcmp(&s_[pos2 - lw], pwhat, size_t(lw) * sizeof(Char))) { pos2 -= lw; } else { break; } } }
    if (pos2 - pos1 == s_.n()) { return s_; }
    if (pos2 <= pos1) { return s_empty; }
    return s_.range_intersect(pos1, pos2);
  }
  template<class Char>
  static std::basic_string<Char> _trim(const arrayref_t<Char>& s_, const Char* pwhat, bool b_left = true, bool b_right = true)
    { return _trim_arrayref(s_, pwhat, b_left, b_right).str(); }

    // NOTE This is able to trim \0 char.
  template<class Char>
  static arrayref_t<Char> _trim_arrayref_s(const arrayref_t<Char>& s_, const arrayref_t<Char>& what_, bool b_left = true, bool b_right = true)
  {
    arrayref_t<Char> s_empty(s_.pd(), 0);
    if (s_.is_empty() || what_.is_empty()) { return s_empty; }
    const s_ll lw = what_.n();
    const s_ll ls = s_.n();
      if (ls < lw) { return s_; }
    s_ll pos1(0); s_ll pos2(ls);
    if (b_left) { while (pos1 <= ls - lw) { if (0 == std::memcmp(&s_[pos1], what_.pd(), size_t(lw) * sizeof(Char))) { pos1 += lw; } else { break; } } }
    if (b_right) { while (pos2 > pos1 + lw) { if (0 == std::memcmp(&s_[pos2 - lw], what_.pd(), size_t(lw) * sizeof(Char))) { pos2 -= lw; } else { break; } } }
    if (pos2 - pos1 == s_.n()) { return s_; }
    if (pos2 <= pos1) { return s_empty; }
    return s_.range_intersect(pos1, pos2);
  }
  template<class Char>
  static std::basic_string<Char> _trim_s(const arrayref_t<Char>& s_, const arrayref_t<Char>& what_, bool b_left = true, bool b_right = true)
    { return _trim_arrayref_s(s_, what_, b_left, b_right).str(); }

  std::wstring trim(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& swhat, bool b_left, bool b_right) { return _trim_s(s, swhat, b_left, b_right); }
  std::string trim(const arrayref_t<char>& s, const arrayref_t<char>& swhat, bool b_left, bool b_right) { return _trim_s(s, swhat, b_left, b_right); }

  std::string lcase_c(const arrayref_t<char>& s_)
  {
    std::string s2;
    if (s_.is_empty()) { return s2; }
    s2.reserve(size_t(s_.n()));
    for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += char(__bmdx_tolower_c(s_[pos])); }
    return s2;
  }
  std::string ucase_c(const arrayref_t<char>& s_)
  {
    std::string s2;
    if (s_.is_empty()) { return s2; }
    s2.reserve(size_t(s_.n()));
    for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += char(__bmdx_toupper_c(s_[pos])); }
    return s2;
  }
  #if __bmdx_char_case_tables
    std::wstring lcase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      if (loc_type == 0) { for (s_ll pos = 0; pos < s_.n(); ++pos)  {  s2 += wchar_t(towlower(s_[pos])); } }
        else if (loc_type == 1) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += __bmdx_towlower_sys(s_[pos]); } }
        else if (loc_type == 2) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += __bmdx_towlower_c(s_[pos]); } }
      return s2;
    }
    std::wstring ucase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      if (loc_type == 0) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += wchar_t(towupper(s_[pos])); } }
        else if (loc_type == 1) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += __bmdx_towupper_sys(s_[pos]); } }
        else if (loc_type == 2) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += __bmdx_towupper_c(s_[pos]); } }
      return s2;
    }
  #elif __bmdx_use_locale_t
    std::wstring lcase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      if (loc_type == 0) { for (s_ll pos = 0; pos < s_.n(); ++pos) {  s2 += wchar_t(towlower(s_[pos])); } }
      else
      {
        if (!ensure_loc(loc_type)) { throw XUExec("lcase_la.w.1"); }
        for (s_ll pos = 0; pos < s_.n(); ++pos) {  s2 += wchar_t(__bmdx_towlower_l(s_[pos], phloc(loc_type)->h)); }
      }
      return s2;
    }
    std::wstring ucase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      if (loc_type == 0) { for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += wchar_t(towupper(s_[pos])); } }
      else
      {
        if (!ensure_loc(loc_type)) { throw XUExec("ucase_la.w.1"); }
        for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += wchar_t(__bmdx_towupper_l(s_[pos], phloc(loc_type)->h)); }
      }
      return s2;
    }
  #else
    std::wstring lcase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      for (s_ll pos = 0; pos < s_.n(); ++pos)  {  s2 += wchar_t(towlower(s_[pos])); }
      return s2;
    }
    std::wstring ucase_la(const arrayref_t<wchar_t>& s_, s_long loc_type)
    {
      std::wstring s2;
      if (s_.is_empty()) { return s2; }
      s2.reserve(size_t(s_.n()));
      for (s_ll pos = 0; pos < s_.n(); ++pos) { s2 += wchar_t(towupper(s_[pos])); }
      return s2;
    }
  #endif

  static bool _eq_wcs_ascii_nocase(const wchar_t* ps1, const wchar_t* ps2)
  {
    if (!ps1 || !ps2) { return false; }
    while (1)
    {
      wchar_t a = *ps1++, b = *ps2++;
      if (a >= L'A' && a <= L'Z') { a += L'a' - L'A'; }
      if (b >= L'A' && b <= L'Z') { b += L'a' - L'A'; }
      if (b != a) { return false; }
      if (!a) { return true; }
    }
  }

  static meta::s_ll max_scstr() { static meta::s_ll n(0); if (!n) { meta::s_ll n2 = meta::s_ll(std::string().max_size()); if (n2 >> 60) { n2 = (1ll << 60) - 1; } n = n2; } return n; }
  static meta::s_ll max_swstr() { static meta::s_ll n(0); if (!n) { meta::s_ll n2 = meta::s_ll(std::wstring().max_size()); if (n2 >> 59) { n2 = (1ll << 59) - 1; } n = n2; } return n; }

    // Returns <=n chars starting from s[pos], like any of the given chars.
    //    n == -1 means get all matching chars.
  static std::string trim_n(const std::string& s, _t_sz pos, const std::string& chars, s_long n)
  {
    struct __local1 { static int _0() { return 0; } };
    if (meta::s_ll(pos) < __local1::_0()) { pos = 0; } if (pos >= s.size() || n < -1) { return std::string(); }
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

  namespace
  {
      // Explicit conversion from arrayref_t to std::basic_string.
    template<class C> void _arrayref2str(const arrayref_t<C>& s, std::basic_string<C>& dest)
    {
      typedef std::basic_string<C> t;
      if (s.is_empty()) { dest.clear(); return; }
      dest.swap(t(s.pd(), size_t(s.n())));
    }

      // Find s_what in s.
      // Returns:
      //    a) if found -- the position in s.
      //    b) if not found -- s.n().
    template<class C>
    s_ll _find_str_linear(arrayref_t<C> s, arrayref_t<C> s_what, s_ll pos)
    {
      arrayref_t<C> x = s.range_intersect(pos);
      s_ll pos2 = x.find_str(s_what);
      if (pos2 < 0) { return s.n(); }
      return pos + pos2;
    }

    template<class C> unity _split2unity_impl(const arrayref_t<C>& s, const arrayref_t<C>& delim, meta::s_ll nmax)
    {
      unity x; x.arr_init<utString>(0);
        if (nmax == 0) { return x; }

      if (s.is_empty()) {}
      else if (delim.is_empty()) { x.arr_append(s); }
      else
      {
        s_ll pos, pos2; pos = 0;
        do
        {
          pos2 = _find_str_linear(s, delim, pos);
          if (pos2 >= s.n() || (nmax > 0 && x.arrsz() + 1 >= nmax)) { pos2 = s.n(); x.arr_append(s.range_intersect(pos, pos2)); }
            else { x.arr_append(s.range_intersect(pos, pos2)); pos2 += delim.n(); if (pos2 >= s.n()) x.arr_append(L""); }
          pos = pos2;
        } while (pos < s.n());
      }

      return x;
    }
    template<class C> std::vector<std::basic_string<C> > _split2vector_impl(const arrayref_t<C>& s, const arrayref_t<C>& delim, meta::s_ll nmax)
    {
      std::vector<std::basic_string<C> > x;
        if (nmax == 0) { return x; }

      if (s.is_empty()) {}
      else if (delim.is_empty()) { x.push_back(s.str()); }
      else
      {
        s_ll pos, pos2; pos = 0;
        do
        {
          pos2 = _find_str_linear(s, delim, pos);
          if (pos2 >= s.n() || (nmax > 0 && x.size() + 1 >= size_t(nmax))) { pos2 = s.n(); x.push_back(s.range_intersect(pos, pos2).str()); }
            else { x.push_back(s.range_intersect(pos, pos2).str()); pos2 += delim.n(); if (pos2 >= s.n()) x.push_back(std::basic_string<C>()); }
          pos = pos2;
        } while (pos < s.n());
      }

      return x;
    }
  }

    // Find (in s) pos. of first character which does not occur in chars.
    //  Search only in range [begin..end). The range is automatically adjusted to fit into s.
    // chars: 0-terminated string. (\0 character cannot be specified as ignored by search.)
    // Returns:
    //    a) if found -- the position in range [begin..end).
    //    b) if not found -- s.n().
  s_ll _find_char_except(arrayref_t<wchar_t> s, s_ll begin, s_ll end, const wchar_t* chars) __bmdx_noex
  {
    if (begin <= 0) { begin = 0; }
    if (end > s.n()) { end = s.n(); }
    if (!*chars || end <= begin) { return s.n(); }
    for (s_ll ic = begin; ic < end; ++ic)
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
    return s.n();
  }

namespace
{
  typedef meta::assert<meta::same_t<unity::t_hash::entry, hashx<unity, unity>::entry>::result>::t_true __check1;
  typedef meta::assert<meta::same_t<unity::t_map::entry, hashx<unity, unity>::entry>::result>::t_true __check2;
  typedef meta::assert<(sizeof(unity) == 4 * (sizeof(void*) >= sizeof(s_long) ? sizeof(void*) : sizeof(s_long)))>::t_true __check4;
  typedef meta::assert<(sizeof(unsigned long long) >= 8)>::t_true __check5;

  #if defined(_bmdxpl_Wnds)
    bool __bmdx_wnds_wgetenv(const wchar_t* varname, std::wstring& retval)
    {
      #if defined(_CRT_USE_WINAPI_FAMILY_DESKTOP_APP)
        wchar_t* p = 0; size_t n = 0;
        _wdupenv_s(&p, &n, varname);
        if (p) { try { retval = p; } catch (...) { free(p); throw; } free(p); return true; }
        retval.clear();
        return false;
      #else
        const wchar_t* p = _wgetenv(varname);
        if (p) { retval = p; return true; }
        retval.clear();
        return false;
      #endif
    }
  #endif
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

  s_long n() const __bmdx_noex;
  unity& operator[] (const unity& k) __bmdx_exs(exc_subscript);
  const entry* operator() (s_long ind) const __bmdx_noex;
  const entry* find(const unity& k, s_long* ret_pind = 0) const __bmdx_noex;
  s_long insert(const unity& k, const entry** ret_pentry = 0, s_long* ret_pind = 0, s_long ind_before = -1) __bmdx_noex;
  s_long remove(const unity& k) __bmdx_noex;
  s_long remove_i(s_long ind) __bmdx_noex;
  const entry* h(s_long ind_h) const __bmdx_noex;
  s_long hl_clear(bool full) __bmdx_noex;
  const f_kf* pkf() const __bmdx_noex;
  bool hl_set_kf(const f_kf& kf) __bmdx_noex;

  s_long qi_noel() const __bmdx_noex;
  s_long qi_next(s_long pos) const __bmdx_noex;
  s_long qi_prev(s_long pos) const __bmdx_noex;
  s_long qi_indk(s_long ind) const __bmdx_noex;
  s_long compatibility() const __bmdx_noex;
private:
  struct _vecm_x : vecm { friend struct _unity_hl_impl; };

  vecm _list; // n == 1 + hash n
  mutable vecm _indk; // n == hash n

  bool _a_rsv_1() __bmdx_noex;
  bool _a_inserted(s_long ind_before = -1) __bmdx_noex;
  void _a_removed(s_long ind) __bmdx_noex;
  void _a_cleared() __bmdx_noex;
  bool _a_indk() const __bmdx_noex;

  bool _check() const __bmdx_noex;
  const _vecm_x& __crvx(const vecm& v) const __bmdx_noex;
  meta::s_ll _v(s_long prev, s_long next) const __bmdx_noex;
  s_long _iprev(meta::s_ll x) const __bmdx_noex;
  s_long _inext(meta::s_ll x) const __bmdx_noex;
  meta::s_ll _v_of(s_long ind) const __bmdx_noex;
  s_long _prev_of(s_long ind) const __bmdx_noex;
  s_long _next_of(s_long ind) const __bmdx_noex;
  void _setv(s_long ind, meta::s_ll x) const __bmdx_noex;
  void __eject(s_long ind) __bmdx_noex;
  void __insert(s_long ind, s_long after) __bmdx_noex;

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
      if (flags & unity::fkcmpSNocase)
      {
        _yk_reg s_long h = 0; _yk_reg const wchar_t* p = key.c_str(); _yk_reg _t_wz n = key.size();
        #if __bmdx_char_case_tables
            while (n > 0) { h *= 31; h += __bmdx_towlower_sys(*p); ++p; --n; }
        #elif __bmdx_use_locale_t
            if (!ensure_loc(1)) { throw XUExec("_kfx::Fhash.4"); }
            while (n > 0) { h *= 31; h += __bmdx_towlower_l(*p, phloc(1)->h); ++p; --n; }
        #else
            while (n > 0) { h *= 31; h += towlower(*p); ++p; --n; }
        #endif
        return h;
      }
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
        _t_wz n = p1->size();
        #if __bmdx_char_case_tables
          for (_t_wz pos = 0; pos < n; ++pos)  {  if (__bmdx_towlower_sys((*p1)[pos]) != __bmdx_towlower_sys((*p2)[pos])) { return false; }  }
        #elif __bmdx_use_locale_t
          if (!ensure_loc(1)) { throw XUExec("_kfx::Feq.6"); }
          for (_t_wz pos = 0; pos < n; ++pos)  {  if (__bmdx_towlower_l((*p1)[pos], phloc(1)->h) != __bmdx_towlower_l((*p2)[pos], phloc(1)->h)) { return false; }  }
        #else
          for (_t_wz pos = 0; pos < n; ++pos)  {  if (towlower((*p1)[pos]) != towlower((*p2)[pos])) { return false; }  }
        #endif
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
      #if __bmdx_char_case_tables
        do {
          if (la == lae) { return lb != lbe; } if (lb == lbe) { return false; }
          if (flags & unity::fkcmpSNocase) { T c1 = T(__bmdx_towlower_sys(*la)); T c2 = T(__bmdx_towlower_sys(*lb)); if (c1 != c2) { return c1 < c2; } }
            else { if (*la < *lb) { return true; } if (*lb < *la) { return false; } }
          ++la; ++lb;
        } while (true);
      #elif __bmdx_use_locale_t
        if (!ensure_loc(1)) { throw XUExec("_kfx::Fless.5"); }
        do {
          if (la == lae) { return lb != lbe; } if (lb == lbe) { return false; }
          if (flags & unity::fkcmpSNocase) { T c1 = T(__bmdx_towlower_l(*la, phloc(1)->h)); T c2 = T(__bmdx_towlower_l(*lb, phloc(1)->h)); if (c1 != c2) { return c1 < c2; } }
            else { if (*la < *lb) { return true; } if (*lb < *la) { return false; } }
          ++la; ++lb;
        } while (true);
      #else
        do {
          if (la == lae) { return lb != lbe; } if (lb == lbe) { return false; }
          if (flags & unity::fkcmpSNocase) { T c1 = T(towlower(*la)); T c2 = T(towlower(*lb)); if (c1 != c2) { return c1 < c2; } }
            else { if (*la < *lb) { return true; } if (*lb < *la) { return false; } }
          ++la; ++lb;
        } while (true);
      #endif
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
      //  < 0 - failure (the value indicates the place of error occurrence).
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
    static const void* Fp0_arrayref(const arrayref_t<wchar_t>& s);
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
      //  < 0 - failure (the number shows where it occurred.)
    static s_long Fcnv_this(unity* p, s_long cs, s_long utt_dest, s_long keep_name) __bmdx_noex;

      // flags:
      //    0x1 - keep object name, if it exists.
      //    0x2 - for any type, create new object dynamically, even if 0x1 is not set or the type is trivial.
      //      (Nonetheless, if px->isEmpty() and the name is not copied from it,
      //        p->_data.p1, p2 will be 0, and p->ut == utEmpty, without xfPtr.)
      //  NOTE Fasg generates exceptions on error (the object is left unchanged).
      // Fasg returns:
      //  1 - success.
      //   0 - p and px are the same.
      //  < 0 - failure (the number shows where it occurred.)
    typedef s_long (*PFasg)(unity* p, const unity* px, s_long flags);
    static void Lasg(unity* p, const unity* px, s_long flags);
    static s_long Fasg(unity* p, const unity* px, s_long flags) __bmdx_noex;
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

      // Returns: 1 - success, 0 - pointer == 0, -1 - deleted but exception occurred during op. delete.
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

  struct cv_paramline
  {
      // Encodes hashlist or map *phm as string *ps, using term *pterm.
      //  1 - success, -1 - bad arg, -2 - failure.
    typedef s_long (*PFpl_encode)(const unity* phm, unity* ps, const unity* pterm);
    static s_long Fpl_encode(const unity* phm, unity* ps, const unity* pterm);
  };
  struct cv_mget
  {
    typedef arrayref_t<char> t_stringref;
    typedef s_long (*PFmget_set_retvals)(unity* phm, const cref_t<t_stringref>* patt, unity* pretmsg, cref_t<t_stringref>* retatt, s_long proc_flags);
    static s_long Fmget_set_retvals(unity* phm, const cref_t<t_stringref>* patt, unity* pretmsg, cref_t<t_stringref>* retatt, s_long proc_flags);
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
const void* cv_ff::cv_wstring::Fp0_arrayref(const arrayref_t<wchar_t>& s) { if (!(s.is_valid() && s.pd())) { static const wchar_t* pc0 = L""; return pc0; } return s.pd(); }
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
unity_common::__Psm _ls_psm_find(const char* piname) __bmdx_noex
{
  if (!piname) { return 0; }
  svf_m_t<t_hipsm, unity_common>::L __lock; if (!__lock.b_valid()) { return 0; }
      svf_m_t<t_hipsm, unity_common> rsth; if (rsth.b_noinit()) { rsth.init0(1); }
      t_hipsm* phi = rsth.px(); if (!phi) { return 0; }
  const t_hipsm::entry* e = phi->find(piname);
  unity_common::__Psm pf = e ? e->v : 0;
  return pf;
}
void unity_common::_ls_psm_set(const char* piname, unity_common::__Psm pf) __bmdx_noex
{
  if (!piname) { return; }
  svf_m_t<t_hipsm, unity_common>::L __lock; if (!__lock.b_valid()) { return; }
      svf_m_t<t_hipsm, unity_common> rsth; if (rsth.b_noinit()) { rsth.init0(1); }
      t_hipsm* phi = rsth.px(); if (!phi) { return; }
  try { phi->opsub(piname) = pf; } catch (...) {}
}
void* _ls_obj_pi(void*, const char*) __bmdx_noex { return 0; }
void* _ls_obj_cpvoid(void*, s_long) __bmdx_noex { return 0; }
unity* _ls_obj_punity(void*) __bmdx_noex { return 0; }
void* _ls_obj_prefo(void*, const char*) __bmdx_noex { return 0; }

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
    (void*)cv_ff::cv_paramline::Fpl_encode,
    (void*)cv_ff::cv_mget::Fmget_set_retvals,
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
    static bool conv_String_Date0(const arrayref_t<wchar_t>& x, _unitydate& retval, bool no_exc);
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

bool _static_conv::conv_String_Date0(const arrayref_t<wchar_t>& x, _unitydate& retval, bool no_exc)
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
    bool b_signed(false); bool b(false); s_long sign(1), y(0), m(0), d(0), h(0), mt(0), sec(0); bool is_frac(false); double frac(0.);
    do // once
    {
        // leading spaces
      s = trim_n(s0, pos, space, -1); pos += s.size();

        // year sign, year, month, day
      s = trim_n(s0, pos, pm, 1); pos += s.size();
        if (s.size() > 0) { b_signed = true; sign = s[0] == '+' ? 1 : -1; }
      s = trim_n(s0, pos, digit, 9); pos += s.size();
        if (!(s.size() >= 4 || (b_signed && s.size() >= 1))) { break; }
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
    o_ref_info ri(0, 0, 0);
    if (a.prc) { ((unity::o_api::__Posm_ref_info)a.prc->rc_posm(unity_common::osm_ref_info))(a.prc, &ri, !!(fc & unity::xfObjStrongRef) ? 3 : 2); }
    o_type_info ti = a.type_info(1+8);

    typedef bmdx_meta::u_ll u_ll;
      u_ll q = u_ll((char*)ti._pobj - (char*)0);
      if (sizeof(void*) < 8) { volatile u_ll nsh = 8 * sizeof(void*); q &= ~(~u_ll(0) << nsh); }
    char qsbuf[20] = "";
      for (int pos = 0, i = 15; i >= 0; --i) { int x = int(0xf & (q >> (i << 2))); if (x || pos || i == 0) { qsbuf[pos] = char(x) + (x >= 10 ? 'A' - 10 : '0'); ++pos; qsbuf[pos] = 0; } }

    std::wstring buf;
      buf += bsToWs(ti.ptstat());
      buf += L':';
      buf += ( ri.b_deleted() ? L'D' : (ri.b_strong() ? L'S' : (ri.b_weak() ? L'W' : L'_')) );
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
  if (retval.ordhs_copy((const unity::t_map_tu&)x) == 1) { return; }
  if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Map_Map"); }
  retval.ordhs_clear();
}
void _static_conv::conv_Map_Hash(s_long fc, const unity::t_map& x, unity::t_hash& retval)
  { retval.hl_clear(true); bool b = retval.hl_set_kf(*x.pkf()); if (b) { try { for (s_long i = 0; i < x.n(); ++i) { const unity::t_map::entry* e = x(i); retval.opsub(e->k) = e->v; } } catch (...) { b = false; } } if (b) { return; } if ((fc & cs_mask) != csLazy) { throw XUConvFailed("conv_Map_Hash"); } retval.hl_clear(true); }
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

s_long unity::_def_size(s_long utt) __bmdx_noex
{
  enum { sp = sizeof(void*) };
  static s_long size_table[2 * _ut_end] =
  {
    0, sizeof(meta::s_ll), sizeof(double), sizeof(_unitydate), 1, sp, sp, -1, sp, sp
    -1, sp, sp, sp, sp, sp, -1, sp, -1, -1,
  };
  return size_table[(utt & utMaskNoArray) + (utt & utArray ? _ut_end : 0)];
}

s_long unity::_compat_id(unity_common::__Psm pmsm_, s_long utype) const __bmdx_noex // class method
  { meta::s_ll n = ((char*)pmsm_ - (char*)0) + utype; n ^= (n >> (32 - _xCompatShift)); return s_long(n & _xCompatMask); }
bool unity::_compat_chk() const __bmdx_noex
  { if (!this) { return false; } if (pmsm == unity_common::ls_modsm) { return true; } const s_long v1 = (ut >> _xCompatShift) & _xCompatMask; const s_long v2 = _compat_id(&unity_common::ls_modsm, utype()); if (v1 == v2) { return true; } const s_long v3 = _compat_id(&unity_common::ls_modsm, -1); if (v1 == v3) { return false; } if (_compatibility() > 0) { ut = _uttf() | (v2 << _xCompatShift); return true; } ut = _uttf() | (v3 << _xCompatShift); return false; }
s_long unity::_compatibility() const __bmdx_noex
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
s_long cv_ff::cv_convert::Fcnv_this(unity* p, s_long cs, s_long utt_dest, s_long keep_name) __bmdx_noex
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
s_long cv_ff::cv_convert::Fasg(unity* p, const unity* px, s_long flags) __bmdx_noex
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



unity* unity::_path_u(const arrayref_t<wchar_t>& keylist, bool forced) __bmdx_noex
{
  try {
    unity k;
    if (1)
    {
      if (keylist.has1(L';'))
      {
        std::wstring s3;
          arrayref_t<wchar_t> z = _trim_arrayref(keylist, L" ", true, false);
          if (z.is_empty()) { s3 = L"=|"; }
            else if (z[0] == L'=') { s3.assign(z.pd(), z._end_u()); }
            else if (z[0] == L'|') { s3 = L"="; s3.append(z.pd(), z._end_u()); }
            else { s3 = L"=|"; s3.append(z.pd(), z._end_u()); }
        k = paramline().decode(s3).hash(L"");
      }
      else
      {
        s_ll pos1 = keylist.find1not(L' ');
        if (pos1 >= 0 && pos1 < keylist.n())
        {
          if (keylist[pos1] == L'=')
          {
            bool b_v = keylist.has1(L'|', pos1);
            std::wstring s2;
              s2.assign(keylist.pd(), keylist._end_u());
              s2[(size_t)pos1] = b_v ? L' ' : L'|';
            paramline().decode1v(s2, k);
          }
          else if (keylist[pos1] == '|')
            { paramline().decode1v(keylist, k); }
          else
          {
            if (keylist.has1(L'|', pos1))
              { std::wstring s2 = L"|"; s2.append(keylist.pd() + pos1, keylist._end_u()); paramline().decode1v(s2, k); }
            else
              { paramline().decode1v(keylist, k); }
          }
        }
        // else { k.clear(); }
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
      meta::s_ll ind(0); try { b = k.utype() == utInt; if (b) { ind = k.vint(); } } catch (...) { return 0; }
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
      meta::s_ll ind(0); try { b = k.ua(j).utype() == utInt; if (b) { ind = k.ua(j).vint(); } } catch (...) { return 0; }
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
unity::unity(const arrayref_t<wchar_t>& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
unity::unity(const meta::s_ll& x)        { ut = utInt; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf_c_i<utInt>() = x; }
unity::unity(const double& x)        { ut = utFloat; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf_c_i<utFloat>() = x; }
unity::unity(const _unitychar& x)        { ut = utChar; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *_drf_c_i<utChar>() = x; }
unity::unity(const _unitydate& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
unity::~unity() __bmdx_noex { clear(); }
void unity::clear() __bmdx_noex
{
  if (!this) { return; }
  s_long uttf = _uttf();
  if (uttf < utString) { _data.p1 = 0; _data.p2 = 0; ut = utEmpty; return; }
  cv_ff::cv_create::Ldestroy(pmsm, &_data, uttf, 0x4);
  ut = utEmpty;
}

unity& unity::operator=(const unity& src) { if (this == &src) { return *this; } _x_asg(src, 0); return *this; }
unity& unity::operator=(const std::wstring& src)
{
  std::wstring* next = cv_ff::cv_wstring::Lnew(pmsm, src.length(), cv_ff::cv_wstring::Fp0(&src), cv_ff::cv_wstring::Fwcs(), 0);
  if (next) { clear(); _data.p1 = next; ut = utString | xfPtr; return *this; }
  throw XUExec("operator=(const std::wstring&)");
}
unity& unity::operator=(const arrayref_t<wchar_t>& src)
{
  std::wstring* next = 0;
  if (src.is_valid()) { next = cv_ff::cv_wstring::Lnew(pmsm, src.n(), cv_ff::cv_wstring::Fp0_arrayref(src), cv_ff::cv_wstring::Fwcs(), 0); }
  if (next) { clear(); _data.p1 = next; ut = utString | xfPtr; return *this; }
  throw XUExec("operator=(const std::arrayref_t<wchar_t>&)");
}
unity& unity::operator=(const meta::s_ll& src) { clear(); ut = utInt; *_drf_c_i<utInt>() = src; return *this; }
unity& unity::operator=(const double& src) { clear(); ut = utFloat; *_drf_c_i<utFloat>() = src; return *this; }
unity& unity::operator=(const _unitydate& src) { clear(); ut = utDate; *_drf_c_i<utDate>() = src; return *this; }
unity& unity::operator=(const _unitychar& src) { clear(); ut = utChar; *_drf_c_i<utChar>() = src; return *this; }

void unity::swap(unity& x) __bmdx_noex
{
  if (this == &x) { return; }
  meta::s_ll temp[1 + sizeof(unity) / 8];
  bytes::memmove_t<char>::F(temp, this, sizeof(unity));
  bytes::memmove_t<char>::F(this, &x, sizeof(unity));
  bytes::memmove_t<char>::F(&x, temp, sizeof(unity));
}

bool unity::unity_delete() __bmdx_noex { return _Ldelete_this() > 0; }
unity& unity::recreate() __bmdx_noex { this->~unity(); new (this) unity(); return *this; }
unity& unity::recreate_as(const unity& modsrc) __bmdx_noex { this->~unity(); new (this) unity(); this->pmsm = modsrc.pmsm; return *this; }
unity& unity::_rnonc_self() const __bmdx_noex { return *const_cast<unity*>(this); }

s_long unity::arrlb() const { if (isArray()) { return cv_ff::cv_array::Llb_u(this); } throw XUTypeMismatch("arrlb", _tname0(utype()), "array"); }
s_long unity::arrub() const { if (isArray()) { return cv_ff::cv_array::Lub_u(this); } throw XUTypeMismatch("arrub", _tname0(utype()), "array"); }
s_long unity::arrsz()const { if (isArray()) { return cv_ff::cv_array::Lsz_u(this); } throw XUTypeMismatch("arrsz", _tname0(utype()), "array"); }
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



meta::s_ll unity::lenstr() const __bmdx_noex
{
  if (!this || !pmsm) { return -3; }
  if (isEmpty()) { return -1; }
  if (!isString()) { return -2; }
  std::wstring* p = reference_t<std::wstring>::deref(this->_data, true);
  return cv_ff::cv_wstring::Ln(pmsm, p);
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
const std::wstring& unity::rstr() const        { return this->ref<utString>(); }
const std::wstring& unity::rstr(s_long ind) const        { return this->utype() == utStringArray ? this->ref<utString>(ind) : this->ref<utUnity>(ind).rstr(); }
std::wstring& unity::rstr()        { return this->ref<utString>(); }
std::wstring& unity::rstr(s_long ind)        { return this->utype() == utStringArray ? this->ref<utString>(ind) : this->ref<utUnity>(ind).rstr(); }
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
_fls75 unity::vflstr() const __bmdx_noex
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
        if (n > s.nmax()) { n = s.nmax(); }
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

#if bmdx_part_unity_oppar_assoc
  unity& unity::operator()(const unity& k) { if (isMap()) { return map(k); } else { return hash(k); } }
  unity& unity::operator()(const unity& k, const unity& v) { if (isMap()) { map_append(k, v); } else { hash_set(k, v); } return *this; }
#endif

namespace {
  static bool _b_awc_may_eq_rstr(const arrayref_t<wchar_t>& s, const unity& v)
  {
    if (!s.pd() || !v.isString()) { return false; }
    try {
      arrayref_t<wchar_t> s2 = v.rstr();
      const wchar_t* p1 = s2.pd();
      const wchar_t* p2 = s2._end_u();
      if (s.pd() >= p1 && s.pd() <= p2) { return true; }
      if (s.is_nonempty() && p1 >= s.pd() && p1 <= s._end_u()) { return true; }
      return false;
    } catch (...) {}
    return true;
  }
}

unity& unity::pl_dech(arrayref_t<wchar_t> ssrc)
{
  if (_b_awc_may_eq_rstr(ssrc, *this))
    { return paramline().decode(std::wstring(ssrc.pd(), ssrc._end_u()), *this, 0); }
  return paramline().decode(ssrc, *this, 0);
}
unity& unity::pl_decm(arrayref_t<wchar_t> ssrc)
{
  if (_b_awc_may_eq_rstr(ssrc, *this))
    { return paramline().decode(std::wstring(ssrc.pd(), ssrc._end_u()), *this, 1); }
  return paramline().decode(ssrc, *this, 1);
}
unity& unity::pl_dech_tree(arrayref_t<wchar_t> ssrc, s_long flags)
{
  if (_b_awc_may_eq_rstr(ssrc, *this))
    { return paramline().decode_tree(std::wstring(ssrc.pd(), ssrc._end_u()), *this, flags & ~s_long(1)); }
  return paramline().decode_tree(ssrc, *this, flags & ~s_long(1));
}
unity& unity::pl_decm_tree(arrayref_t<wchar_t> ssrc, s_long flags)
{
  if (_b_awc_may_eq_rstr(ssrc, *this))
    { return paramline().decode_tree(std::wstring(ssrc.pd(), ssrc._end_u()), *this, flags | 1); }
  return paramline().decode_tree(ssrc, *this, flags | 1);
}
unity& unity::pl_dec1v(arrayref_t<wchar_t> ssrc)
{
  if (_b_awc_may_eq_rstr(ssrc, *this))
    { return paramline().decode1v(std::wstring(ssrc.pd(), ssrc._end_u()), *this); }
  return paramline().decode1v(ssrc, *this);
}
unity& unity::pl_dech(arrayref_t<char> ssrc) { if (!ssrc.is_valid()) { ssrc.clear(); } return pl_dech(bsToWs(ssrc.pd(), ssrc.n())); }
unity& unity::pl_decm(arrayref_t<char> ssrc) { if (!ssrc.is_valid()) { ssrc.clear(); } return pl_decm(bsToWs(ssrc.pd(), ssrc.n())); }
unity& unity::pl_dech_tree(arrayref_t<char> ssrc, s_long flags) { if (!ssrc.is_valid()) { ssrc.clear(); } return pl_dech_tree(bsToWs(ssrc.pd(), ssrc.n()), flags); }
unity& unity::pl_decm_tree(arrayref_t<char> ssrc, s_long flags) { if (!ssrc.is_valid()) { ssrc.clear(); } return pl_decm_tree(bsToWs(ssrc.pd(), ssrc.n()), flags); }
unity& unity::pl_dec1v(arrayref_t<char> ssrc) { if (!ssrc.is_valid()) { ssrc.clear(); } return pl_dec1v(bsToWs(ssrc.pd(), ssrc.n())); }
std::wstring unity::pl_enc() const { return paramline().encode(*this); }
std::wstring unity::pl_enc_tree() const { return paramline().encode_tree(*this); }
std::wstring unity::pl_enc1v() const { return paramline().encode1v(*this); }

unity::_wr_cstring::_wr_cstring() {}
unity::_wr_cstring::_wr_cstring(const std::string& x) : t_base(x) {}
const unity::_wr_cstring::t_base& unity::_wr_cstring::cstr() const { return *this; }
std::wstring unity::_wr_cstring::wstr() const { return bsToWs(*this); }

unity::_wr_wstring::_wr_wstring() {}
unity::_wr_wstring::_wr_wstring(const std::wstring& x) : t_base(x) {}
std::string unity::_wr_wstring::cstr() const { return wsToBs(*this); }
const unity::_wr_wstring::t_base& unity::_wr_wstring::wstr() const { return *this; }

bool unity::isAssoc() const __bmdx_noex { if (!this) { return false; } s_long t = utype(); return t == utHash || t == utMap; } // true if unity is utHash or utMap
bool unity::isBoolTrue() const __bmdx_noex { return this && utype() == utChar && *_drf_c_i<utChar>() != 0; } // true if *this contains char != 0
bool unity::isBoolFalse() const __bmdx_noex { return this && utype() == utChar && *_drf_c_i<utChar>() == 0; } // true if *this contains char == 0
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

_fls75 unity::_tname0(s_long uttf) __bmdx_noex
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
    // -1 - an error occurred, the map did not change.
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






unity& _unity_hl_impl::operator[] (const unity& k) __bmdx_exs(exc_subscript)
{
  const t_hash::entry* e(0);
  s_long res = this->t_hash::insert(k, &e);
  if (res > 0) { if (!_a_inserted()) { this->t_hash::remove_e(e); res = -1; } }
  if (res >= 0) { return e->v; } throw exc_subscript();
}
  // Returns same as hashx remove_all (>= 0 - success, < 0 - failure, no changes.)
s_long _unity_hl_impl::hl_clear(bool full) __bmdx_noex
{
  if (full) { s_long n_prev = _n; hashx_clear(); _a_cleared(); return n_prev; }
  s_long res = this->t_hash::remove_all(); if (res >= 0) { _a_cleared(); } return res;
}

bool _unity_hl_impl::_a_rsv_1() __bmdx_noex  // reserve place in list and indk before possible adding 1 elem. to hash
  { return _list.el_reserve_n(n() + 2, false) && _indk.el_reserve_n(n() + 1, false); }

  // Adjustment notifications for list and keys index, on each hashlist change.

bool _unity_hl_impl::_a_inserted(s_long ind_before) __bmdx_noex // adjust list and indk after adding 1 elem. to hashlist, true on success
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
void _unity_hl_impl::_a_removed(s_long ind) __bmdx_noex // adjust list and indk after removing 1 elem. from hash
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
void _unity_hl_impl::_a_cleared() __bmdx_noex // clear list and indk
  {  _list.el_expand_n(1); *_list.pval_first<meta::s_ll>() = _v(-1, -1);  _indk.vecm_clear(); }

bool _unity_hl_impl::_a_indk() const __bmdx_noex // ensure indk containing full index of hashlist elems.
{
  if (_indk.n() == n()) { return true; }
  if (!_indk.el_expand_n(n())) { _indk.vecm_clear(); return false; }
  s_long pos = -1; s_long i = 0; while (i < n()) { pos = _next_of(pos); if (!(pos >= 0 && pos < n())) { break; } *_indk.pval_0u<s_long>(i) = pos; ++i; }
  if (i != n()) { _indk.vecm_clear(); return false; }
  return true;
}

void _unity_hl_impl::__eject(s_long ind) __bmdx_noex
{
  s_long ipr = _prev_of(ind); s_long inx = _next_of(ind);
  if (ipr == inx) { _setv(ipr, _v(ipr, ipr)); }
    else if (_list.n() > 2) { meta::s_ll xp = _v(_prev_of(ipr), inx); meta::s_ll xn =  _v(ipr, _next_of(inx)); _setv(ipr, xp); _setv(inx, xn); }
}
void _unity_hl_impl::__insert(s_long ind, s_long after) __bmdx_noex
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
s_long unity::_hl_i::n() const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->n(); }
unity& unity::_hl_i::opsub(const unity& k __bmdx_noargt) __bmdx_exs(exc_subscript) { return static_cast<_unity_hl_impl*>(this)->operator[](k); }
const unity::_hl_i::entry* unity::_hl_i::operator() (s_long ind) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->operator()(ind); }
const unity::_hl_i::entry* unity::_hl_i::find(const unity& k, s_long* ret_pind) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->find(k, ret_pind); }
s_long unity::_hl_i::insert(const unity& k, const entry** ret_pentry, s_long* ret_pind, s_long ind_before) __bmdx_noex { return static_cast<_unity_hl_impl*>(this)->insert(k, ret_pentry, ret_pind, ind_before); }
s_long unity::_hl_i::remove(const unity& k) __bmdx_noex { return static_cast<_unity_hl_impl*>(this)->remove(k); }
s_long unity::_hl_i::remove_i(s_long ind) __bmdx_noex { return static_cast<_unity_hl_impl*>(this)->remove_i(ind); }
const unity::_hl_i::entry* unity::_hl_i::h(s_long ind_h) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->h(ind_h); }
s_long unity::_hl_i::hl_clear(bool full) __bmdx_noex { return static_cast<_unity_hl_impl*>(this)->hl_clear(full); }
const unity::_hl_i::t_hash::f_kf* unity::_hl_i::pkf() const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->pkf(); }
bool unity::_hl_i::hl_set_kf(const unity::_hl_i::t_hash::f_kf& kf) __bmdx_noex { return static_cast<_unity_hl_impl*>(this)->hl_set_kf(kf); }

s_long unity::_hl_i::qi_noel() const __bmdx_noex { return t_hash::no_elem; }
s_long unity::_hl_i::qi_next(s_long pos) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->qi_next(pos); }
s_long unity::_hl_i::qi_prev(s_long pos) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->qi_prev(pos); }
s_long unity::_hl_i::qi_indk(s_long ind) const __bmdx_noex { return static_cast<const _unity_hl_impl*>(this)->qi_indk(ind); }

s_long unity::_hl_i::compatibility() const __bmdx_noex
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
s_long _unity_hl_impl::n() const __bmdx_noex { return _n; }
const _unity_hl_impl::entry* _unity_hl_impl::operator() (s_long ind) const __bmdx_noex { return this->t_hash::operator()(qi_indk(ind)); }
const _unity_hl_impl::entry* _unity_hl_impl::find(const unity& k, s_long* ret_pind) const __bmdx_noex { const entry* e; s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, &e, &ind); if (ret_pind) { *ret_pind = ind; } return e; }
s_long _unity_hl_impl::insert(const unity& k, const entry** ret_pentry, s_long* ret_pind, s_long ind_before) __bmdx_noex { const t_hash::entry* e(0); s_long ind; s_long res = this->t_hash::insert(k, &e, &ind); if (res > 0) { if (!_a_inserted(ind_before)) { this->t_hash::remove_e(e); e = 0; ind = no_elem; res = -1; } } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; } return res; }
s_long _unity_hl_impl::remove(const unity& k) __bmdx_noex { s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, 0, &ind); return remove_i(ind); }
s_long _unity_hl_impl::remove_i(s_long ind) __bmdx_noex { s_long res = this->t_hash::remove_i(ind); if (res == 1) { _a_removed(ind); } return res; }
const _unity_hl_impl::entry* _unity_hl_impl::h(s_long ind_h) const __bmdx_noex { return this->t_hash::operator()(ind_h); }
const _unity_hl_impl::f_kf* _unity_hl_impl::pkf() const __bmdx_noex { return this->t_hash::pkf(); }
bool _unity_hl_impl::hl_set_kf(const f_kf& kf) __bmdx_noex { return this->t_hash::hashx_set_kf(kf); }
s_long _unity_hl_impl::qi_noel() const __bmdx_noex { return no_elem; } // starting pos for iterating the list in both directions
  // Next, prev. elem. ind. >= 0, or qi_noel() at list end
s_long _unity_hl_impl::qi_next(s_long pos) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _inext(*p) : s_long(no_elem); }
s_long _unity_hl_impl::qi_prev(s_long pos) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _iprev(*p) : s_long(no_elem); }
  // Index of ind-th element in hashlist, according to list, or qi_noel() for bad ind or indk.
s_long _unity_hl_impl::qi_indk(s_long ind) const __bmdx_noex { if (!_a_indk()) { return no_elem; } s_long* p = _indk.pval<s_long>(ind); return p ? *p : s_long(no_elem); }
s_long _unity_hl_impl::compatibility() const __bmdx_noex { const s_long ti = bytes::type_index_t<s_long>::ind(); const s_long ti2 = bytes::type_index_t<meta::s_ll>::ind(); s_long c = __crvx(_list)._t_ind == ti2  && __crvx(_indk)._t_ind == ti && __crvx(_ht)._t_ind == ti && sizeof(*this) == ((char*)&_indk - (char*)this) + sizeof(vecm) ? this->vecm::compatibility() : -1; if (c == 0) { c = -1; } return c; }
bool _unity_hl_impl::_check() const __bmdx_noex { return nexc() == 0 && _list.n() == n() + 1 && (_indk.n() == 0 || _indk.n() == n()); }
const _unity_hl_impl::_vecm_x& _unity_hl_impl::__crvx(const vecm& v) const __bmdx_noex { return *static_cast<const _vecm_x*>(&v); } // class method
meta::s_ll _unity_hl_impl::_v(s_long prev, s_long next) const __bmdx_noex { return (meta::s_ll(prev) << 32) | (meta::s_ll(next) & 0xffffffffll); } // class method
s_long _unity_hl_impl::_iprev(meta::s_ll x) const __bmdx_noex { return s_long(x >> 32); } // class method
s_long _unity_hl_impl::_inext(meta::s_ll x) const __bmdx_noex { return s_long(x & 0xffffffffll); } // class method
meta::s_ll _unity_hl_impl::_v_of(s_long ind) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? *p : -1; }
s_long _unity_hl_impl::_prev_of(s_long ind) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _iprev(*p) : -1; }
s_long _unity_hl_impl::_next_of(s_long ind) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _inext(*p) : -1; }
void _unity_hl_impl::_setv(s_long ind, meta::s_ll x) const __bmdx_noex { meta::s_ll* p = _list.pval<meta::s_ll>(ind); if (p) { *p = x; } }

void unity::_ensure_m() { if (!isMap()) { u_clear(utMap); } if (!(_compat_chk() && _m()->pkf())) { throw XUExec("_ensure_m"); } }
void unity::_ensure_sc() { if (!isScalar()) { u_clear(utEmpty); } }
void unity::_ensure_h() { if (!isHash()) { u_clear(utHash); } if (!(_compat_chk() && _h()->pkf())) { throw XUExec("_ensure_h"); } }

unity::checked_ptr<const unity> unity::path(const unity& keylist) const __bmdx_noex { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.rstr(), false); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), false); } return px->_path_u(keylist.vstr(), false); } catch (...) { return 0; } }
unity::checked_ptr<const unity> unity::path(const arrayref_t<wchar_t>& keylist) const __bmdx_noex { return const_cast<unity*>(this)->_path_u(keylist, false); }
unity::checked_ptr<const unity> unity::path(const arrayref_t<char>& keylist) const __bmdx_noex { try { return const_cast<unity*>(this)->_path_u(bsToWs(keylist), false); } catch (...) { return 0; } }
unity::checked_ptr<const unity> unity::path(const wchar_t* keylist) const __bmdx_noex { return path(arrayref_t<wchar_t>(keylist)); }
unity::checked_ptr<const unity> unity::path(const char* keylist) const __bmdx_noex { return path(arrayref_t<char>(keylist)); }
unity::checked_ptr<const unity> unity::path(const std::wstring& keylist) const __bmdx_noex { return path(arrayref_t<wchar_t>(keylist)); }
unity::checked_ptr<const unity> unity::path(const std::string& keylist) const __bmdx_noex { return path(arrayref_t<char>(keylist)); }

unity::checked_ptr<const unity> unity::path(const unity& keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(keylist); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const arrayref_t<wchar_t>& keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(keylist); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const arrayref_t<char>& keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(keylist); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const wchar_t* keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(arrayref_t<wchar_t>(keylist)); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const char* keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(arrayref_t<char>(keylist)); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const std::wstring& keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(arrayref_t<wchar_t>(keylist)); return p ? p : &x_dflt; }
unity::checked_ptr<const unity> unity::path(const std::string& keylist, const unity& x_dflt) const __bmdx_noex { const unity* p = path(arrayref_t<char>(keylist)); return p ? p : &x_dflt; }

unity::checked_ptr<unity> unity::path_w(const unity& keylist) __bmdx_noex { return _path_w(keylist); }
unity::checked_ptr<unity> unity::path_w(const arrayref_t<wchar_t>& keylist) __bmdx_noex { return _path_w(keylist); }
unity::checked_ptr<unity> unity::path_w(const arrayref_t<char>& keylist) __bmdx_noex { return _path_w(keylist); }
unity::checked_ptr<unity> unity::path_w(const wchar_t* keylist) __bmdx_noex { return _path_w(arrayref_t<wchar_t>(keylist)); }
unity::checked_ptr<unity> unity::path_w(const char* keylist) __bmdx_noex { return _path_w(arrayref_t<char>(keylist)); }
unity::checked_ptr<unity> unity::path_w(const std::wstring& keylist) __bmdx_noex { return _path_w(arrayref_t<wchar_t>(keylist)); }
unity::checked_ptr<unity> unity::path_w(const std::string& keylist) __bmdx_noex { return _path_w(arrayref_t<char>(keylist)); }

unity* unity::_path_w(const arrayref_t<wchar_t>& keylist) __bmdx_noex { return _path_u(keylist, true); }
unity* unity::_path_w(const arrayref_t<char>& keylist) __bmdx_noex { try { return _path_u(bsToWs(keylist), true); } catch (...) { return 0; } }
unity* unity::_path_w(const unity& keylist) __bmdx_noex { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.rstr(), true); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), true); } return px->_path_u(keylist.vstr(), true); } catch (...) { return 0; } }


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
  while (pos != rh.qi_noel()) { const t_hash::entry* e = rh.h(pos); rh2.opsub(e->k) = e->v; pos = rh.qi_next(pos); }
  _xu_move_p1(z0);
}

bool unity::hash_empty(const unity& k) const        { if (!(isHash() && _compat_chk())) { return true; } const t_hash::entry* e = _h()->find(k); return !e || e->v.isEmpty(); }
bool unity::hash_locate(const unity& k, bool insert)
{
  _ensure_h();
  if (insert) { s_long res = _h()->insert(k); if (res >= 0) { return res > 0; } throw XUExec("hash_locate.1"); }
  return !!_h()->find(k);
}
unity& unity::hash(const unity& k)        { _ensure_h(); return _h()->opsub(k); }
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
s_long unity::assocFlags_c() const
{
  if (isHash()) { if (_compat_chk() && _h()->pkf()) { return _h()->pkf()->flags(); } throw XUExec("assocFlags_c.1"); }
    else if (isMap()) { if (_compat_chk() && _m()->pkf()) { return _m()->pkf()->flags(); } throw XUExec("assocFlags_c.2"); }
    else { throw XUExec("assocFlags_c.3"); }
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
std::string _wsToBsLsb(const wchar_t* ps, meta::s_ll n);

std::string wsToBs(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, false); }
std::string wsToBs(arrayref_t<wchar_t> x) { return _wsToBs(x.pd(), x.n(), false); }
std::string wsToBsOem(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, true); }
std::string wsToBsOem(arrayref_t<wchar_t> x) { return _wsToBs(x.pd(), x.n(), true); }
std::string wsToBsLsb(const wchar_t* x, meta::s_ll n) { return _wsToBsLsb(x, n); }
std::string wsToBsLsb(arrayref_t<wchar_t> x) { return _wsToBsLsb(x.pd(), x.n()); }

std::wstring _bsToWs(const char* ps, meta::s_ll n); // platform-dependent
std::wstring _bsLsbToWs(const char* ps, meta::s_ll n);

std::wstring bsToWs(const char* x, meta::s_ll n){ return _bsToWs(x, n); }
std::wstring bsToWs(arrayref_t<char> x){ return _bsToWs(x.pd(), x.n()); }
std::wstring bsLsbToWs(const char* x, meta::s_ll n) { return _bsLsbToWs(x, n); }
std::wstring bsLsbToWs(arrayref_t<char> x) { return _bsLsbToWs(x.pd(), x.n()); }


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


std::string replace_c(const arrayref_t<char>& s_, const arrayref_t<char>& from_, const arrayref_t<char>& to_, bool ignoreCase, s_ll nmax)
{
  typedef arrayref_t<char> t_ar;
  if (s_.is_empty() || from_.is_empty() || nmax == 0) { return s_.str(); }

  arrayref_t<char> to = ""; if (to_.is_nonempty()) { to = to_; }

  s_ll n = 0; if (nmax < 0) { nmax = s_.n(); }
  std::string dest;
  s_ll pos, pos2;
  pos = 0;
  if (ignoreCase)
  {
    std::string s_l_, from_l_;
    t_ar s_l, from_l;
    if (1)
    {
      s_l_ = lcase_c(s_); s_l = s_l_;
      from_l_ = lcase_c(from_); from_l = from_l_;
    }
    do
    {
      if (n >= nmax) { break; }
      pos2 = _find_str_linear(s_l, from_l, pos);
      if (pos2 >= s_l.n()) { t_ar x = s_.range_intersect(pos); dest.append(x.pd(), x._end_u()); }
        else { dest.append(&s_[pos], &s_[pos2]); dest.append(to.pd(), to._end_u()); ++n; pos2 += from_.n(); }
      pos = pos2;
    }
    while(pos < s_.n());
    if (pos < s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
  }
  else
  {
    do
    {
      if (n >= nmax) { break; }
      pos2 = _find_str_linear(s_, from_, pos);
      if (pos2 >= s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
        else { dest.append(&s_[pos], &s_[pos2]); dest.append(to.pd(), to._end_u()); ++n; pos2 += from_.n(); }
      pos = pos2;
    }
    while(pos < s_.n());
    if (pos < s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
  }
  return dest;
}

std::wstring replace(const arrayref_t<wchar_t>& s_, const arrayref_t<wchar_t>& from_, const arrayref_t<wchar_t>& to_, bool ignoreCase, s_ll nmax, s_long loc_type)
{
  typedef arrayref_t<wchar_t> t_ar;
  if (s_.is_empty() || from_.is_empty() || nmax == 0) { return s_.str(); }

  arrayref_t<wchar_t> to = L""; if (to_.is_nonempty()) { to = to_; }

  s_ll n = 0; if (nmax < 0) { nmax = s_.n(); }
  std::wstring dest;
  s_ll pos, pos2;
  pos = 0;
  if (ignoreCase)
  {
    std::wstring s_l_, from_l_;
    t_ar s_l, from_l;
    if (1)
    {
      s_l_ = lcase_la(s_, loc_type); s_l = s_l_;
      from_l_ = lcase_la(from_, loc_type); from_l = from_l_;
    }
    do
    {
      if (n >= nmax) { break; }
      pos2 = _find_str_linear(s_l, from_l, pos);
      if (pos2 >= s_l.n()) { t_ar x = s_.range_intersect(pos); dest.append(x.pd(), x._end_u()); }
        else { dest.append(&s_[pos], &s_[pos2]); dest.append(to.pd(), to._end_u()); ++n; pos2 += from_.n(); }
      pos = pos2;
    }
    while(pos < s_.n());
    if (pos < s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
  }
  else
  {
    do
    {
      if (n >= nmax) { break; }
      pos2 = _find_str_linear(s_, from_, pos);
      if (pos2 >= s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
        else { dest.append(&s_[pos], &s_[pos2]); dest.append(to.pd(), to._end_u()); ++n; pos2 += from_.n(); }
      pos = pos2;
    }
    while(pos < s_.n());
    if (pos < s_.n()) { dest.append(&s_[pos], &s_[s_.n()]); }
  }
  return dest;
}

unity split(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& delim, meta::s_ll nmax)
  { return _split2unity_impl(s, delim, nmax); }

unity split(const arrayref_t<char>& s, const arrayref_t<char>& delim, meta::s_ll nmax)
  { return _split2unity_impl(s, delim, nmax); }

std::vector<std::wstring> splitToVector(const arrayref_t<wchar_t>& s, const arrayref_t<wchar_t>& delim, meta::s_ll nmax)
  { return _split2vector_impl(s, delim, nmax); }

std::vector<std::string> splitToVector(const arrayref_t<char>& s, const arrayref_t<char>& delim, meta::s_ll nmax)
  { return _split2vector_impl(s, delim, nmax); }

std::string join(const unity& asrc, const arrayref_t<char>& delim_)
{
  std::string s;
  if (asrc.isArray())
  {
    for (s_long ind = asrc.arrlb(); ind <= asrc.arrub(); ++ind)
    {
      if (delim_.is_nonempty() && ind != asrc.arrlb()) { s.append(delim_.pd(), delim_._end_u()); }
      s += asrc.vcstr(ind);
    }
  }
  else { s = asrc.vcstr(); }
  return s;
}
std::string joinVector(const std::vector<std::string>& asrc, const arrayref_t<char>& delim_)
{
  std::string s;
  for (size_t ind = 0; ind < asrc.size(); ++ind)
  {
    if (delim_.is_nonempty() && ind != 0) { s.append(delim_.pd(), delim_._end_u()); }
    s += asrc[ind];
  }
  return s;
}
std::wstring join(const unity& asrc, const arrayref_t<wchar_t>& delim_)
{
  std::wstring s;
  if (asrc.isArray())
  {
    for (s_long ind = asrc.arrlb(); ind <= asrc.arrub(); ++ind)
    {
      if (delim_.is_nonempty() && ind != asrc.arrlb()) { s.append(delim_.pd(), delim_._end_u()); }
      s += asrc.vstr(ind);
    }
  }
  else { s = asrc.vstr(); }
  return s;
}
std::wstring joinVector(const std::vector<std::wstring>& asrc, const arrayref_t<wchar_t>& delim_)
{
  std::wstring s;
  for (size_t ind = 0; ind < asrc.size(); ++ind)
  {
    if (delim_.is_nonempty() && ind != 0) { s.append(delim_.pd(), delim_._end_u()); }
    s += asrc[ind];
  }
  return s;
}

namespace
{
    // Returns a short string representation of x.
    // If x is a string, it is just copied.
    // If x is an array, object, or unknown value, only a short prefix and the object type name
    //    is included into output (but NO CONTENTS).
    // If x is a date, it is converted, using fixed format YYYY-MM-DD HH-MM-SS,
    //    where H, M, S are missing if all are 0.
    // If x is char, its boolean value is returned: "true" on 0 or "false" on non-0.
    // For other values (utEmpty, utInt, utFloat), x.vstr() is returned.
  std::wstring _vstr_scalar(const unity& x)
  {
      if (x.isObject()) { return L"?object " + x.tname(); }
      if (x.isArray()) { return L"?array " + x.tname(true); }

      switch (x.utype())
      {
        case utEmpty: return std::wstring();
        case utInt: return x.vstr();
        case utFloat: return x.vstr();
        case utChar: { return x.ref<utChar>() ? L"true" : L"false"; }
        case utString: { return x.vstr(); }
        case utDate:
          {
            // Note: this loses fractional part of seconds,
            //  unlike conv_Date_String.
            _unitydate d = x.val<utDate>();
            std::ostringstream oss;
            oss << std::setw(4) << std::setfill('0') << std::right << d.d_year() << "-" << std::setw(2) <<d.d_month() << "-" << std::setw(2) <<d.d_day();
            s_long h, m, s;
            h = d.d_hour();
            m = d.d_minute();
            s = d.d_second();
            if (h || m || s) { oss << " " << std::setw(2) << h << ":" << std::setw(2) << m << ":" << std::setw(2) << s; }
            return bsToWs(oss.str());
          }
        default: { return L"?value/" + x.tname(); }
      }
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
o_type_info unity::objTypeInfo(s_long flags) const __bmdx_noex
      { return o_api(this).type_info(flags); }
o_ref_info unity::objRefInfo() const __bmdx_noex
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
bool o_type_info::b_same_tstat_cm(const std::type_info& ti) const { return _tstat && (std::strcmp(_tstat, ti.name()) == 0); }
bool o_type_info::b_same_tdyn_cm(const std::type_info& ti) const { return _tdyn && (std::strcmp(_tdyn, ti.name()) == 0); }
bool o_type_info::b_same_tstat(const std::type_info& ti) const { return _tstat && b_local() && (std::strcmp(_tstat, ti.name()) == 0); }
bool o_type_info::b_same_tdyn(const std::type_info& ti) const { return _tdyn && b_local() && (std::strcmp(_tdyn, ti.name()) == 0); }
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
  void clear() __bmdx_noex;
private: _itfslist_temp1(const _itfslist_temp1&); void operator=(const _itfslist_temp1&);
};

unity::o_api::_itfslist_temp1::handle::handle() : p_itfs(0), p_itfs_src(0), p_itfssm(0), f_nc(0), f_del(0), f_ac(0), f_bd(0) {}
bool unity::o_api::_itfslist_temp1::handle::b_valid_f() const { return p_itfssm && f_del && f_nc && f_ac && f_bd; }
bool unity::o_api::_itfslist_temp1::handle::b_valid_all() const { return p_itfs && b_valid_f(); }
unity::o_api::_itfslist_temp1::_itfslist_temp1() : n_del(0), n_bd(0) {}
unity::o_api::_itfslist_temp1::~_itfslist_temp1() { clear(); }
void unity::o_api::_itfslist_temp1::clear() __bmdx_noex
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
unity::o_api::o_api() __bmdx_noex
    : pu(0), prc(0), pidyn(0) {}

  // Creates fully functional object.
  // NOTE "const" is ignored.
unity::o_api::o_api(const unity* pu_) __bmdx_noex
{
  prc = 0; pidyn = 0;
  pu = const_cast<unity*>(pu_); if (!(pu && pu->isObject())) { return; }
  void* p11 = 0; s_long ff1 = 0;
  while (true) { p11 = pu->_data.p1; ff1 = pu->ut; void* p12 = pu->_data.p1; s_long ff2 = pu->ut; if (p11 == p12 && ff1 == ff2) { break; } sleep_mcs(100, 1); }
  pidyn = (ff1 & xfObjItfsList) ? (_o_itfslist*)p11 : 0;
  prc = pidyn ? pidyn->prc : (_o_refcnt*)p11;
}

  // Creates partially functional object (not able to manipulate parent unity object).
unity::o_api::o_api(_o_itfslist& list) __bmdx_noex
{
  pu = 0;
  pidyn = &list;
  prc = list.prc;
}
  // Implementation; assumes p is (b_pidyn ? _o_itfslist* : _o_refcnt*).
unity::o_api::o_api(void* p, bool b_pidyn) __bmdx_noex
  : pu(0), prc(0), pidyn(0)
{
  if (!p) { return; }
  if (b_pidyn) { pidyn = (_o_itfslist*)p; prc = pidyn->prc; return; }
  prc = (_o_refcnt*)p;
}
void* unity::o_api::p_head() const __bmdx_noex
  { if (pidyn) { return pidyn; } return prc; }
  // flags OR-ed:
  //  1 - get static type info,
  //  2 - get dynamic type info,
  //  4 - get type index (see type_index_t in vecm_hashx.h),
  //  8 - return _pobj even if the object is deleted.
  //  16 - get sizeof for object static type.
o_type_info unity::o_api::type_info(s_long flags) const __bmdx_noex
{
  o_type_info ti(0,0,0,0,0,0,-1);
  if (!prc) { return ti; }
  ((__Posm_type_info)prc->rc_posm(unity_common::osm_type_info))(prc, &ti, flags);
  return ti;
}
  // ref_info contains full info on number of references, plus boolean strong/weak ref. type flag taken from the wrapping unity object (pu).
  //    If pu == 0 (wrapping object not accessible), ref. type will be set to undefined value (0).
o_ref_info unity::o_api::ref_info() const __bmdx_noex
{
  o_ref_info ri(0, 0, 0);
  if (!prc) { return ri; }
  ((__Posm_ref_info)prc->rc_posm(unity_common::osm_ref_info))(prc, &ri, pu ? ((pu->ut & xfObjStrongRef) ? 3 : 2) : 0);
  return ri;
}
  // Returns a pointer to the referenced object. If b_checked and the object is deleted, returns 0.
  //    Does not check compat. of the referenced object with the current binary module.
void* unity::o_api::pvoid(bool b_checked) const __bmdx_noex
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
void* unity::o_api::prefo(const char* tname, s_long t_ind, s_long t_size) const __bmdx_noex
{
  if (!prc) { return 0; } if (!prc->rc_posm) { return 0; }
  return ((__Posm_prefo)prc->rc_posm(unity_common::osm_prefo))(prc, tname, t_ind, t_size);
}

  // Change type of reference.
  //  Argument and returned value: same as _o_sm_t<Obj>::rc_incdec (see).
  //  NOTE If pu != 0, pu and its flags are ignored. See also upd_ut_ref_type.
s_long unity::o_api::rc_incdec(s_long flags) const __bmdx_noex
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
s_long unity::o_api::upd_ut_ref_type(bool b_strong) const __bmdx_noex
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
s_long unity::o_api::itfslist_create() __bmdx_noex
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
s_long unity::o_api::itfslist_n() const __bmdx_noex
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
s_long unity::o_api::itfs_append_cp(const o_api& src) const __bmdx_noex
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
s_long unity::o_api::itfs_remove_ind(s_long ind) const __bmdx_noex
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
s_long unity::o_api::itfs_remove_all() const __bmdx_noex
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
unity::o_api unity::o_api::_ref_newcp(unity_common::__Psm pmsm_dest, s_long flags) const __bmdx_noex
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
s_long unity::o_api::_ref_del(s_long flags) __bmdx_noex
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
s_long unity::o_api::find_by_pitfs(o_itfset_base* p) const __bmdx_noex
{
  if (!prc) { return -2; } if (!pidyn) { return -1; }
  critsec_rc __lock(prc); if (sizeof(__lock)) {}
  cpparray_t<_o_itfslist::handle>& ls = pidyn->itfslist;
  for (meta::s_ll i = 0; i < ls.n(); ++i) { _o_itfslist::handle& hs = ls[i]; if (!hs.b_valid()) { continue; } if (hs.p_itfs == p) { return s_long(i); } }
  return -1;
}
  // NOTE Class method.
meta::s_ll unity::o_api::_itfslist_nrsv(meta::s_ll n) const __bmdx_noex
  { if (n < 0) { return 0; } if (n < 8) { return n; } if (n < 64) { return (n | 7) + 1; } return (n | 31) + 1; }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// paramline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string paramline::cpterm = "; ";
const std::string paramline::cpterm_short = ";";
const std::string paramline::ceterm = " = ";
const std::string paramline::ceterm_short = "=";
const std::string paramline::cvterm = "|";
const std::string paramline::cao_appseq = "++";
const std::string paramline::cao_app1 = "+";
const std::string paramline::cao_setseq = "..";
const std::string paramline::cao_set1 = ".";

const std::wstring paramline::wpterm = L"; ";
const std::wstring paramline::wpterm_short = L";";
const unity paramline::uwpterm(wpterm);
const unity paramline::uwpterm_short(wpterm_short);
const std::wstring paramline::weterm = L" = ";
const std::wstring paramline::weterm_short = L"=";
const std::wstring paramline::wvterm = L"|";
const std::wstring paramline::wao_appseq = L"++";
const std::wstring paramline::wao_app1 = L"+";
const std::wstring paramline::wao_setseq = L"..";
const std::wstring paramline::wao_set1 = L".";

const unity& paramline::_0 = unity::_0;

enum Eparamilne_replace_flags
{
  plrf_has_esc_seq = 1,
  plrf_value_exists = 2,
  plrf_allow_modify_all = 4,
  plrf_use_path_array_modifiers = 8,
  _plrf__nullflags = 0
};

enum Eparamilne_encdec_flags
{
  pled_use_map = 0x1,
  pled_overwrite_dup_keys = 0x2,
  pled_encdec1v_for_names = 0x100,
  pled_enc_empty_key_as_bse = 0x200,
  pled__enc_assoc_as_empty = 0x1000,
  pled__ar_elem = 0x2000,
  pled__use_array = 0x4000,
  pled__get_strvals = 0x8000,
  pled__enc1v_for_array_area = 0x10000,
  _pled__nullflags = 0,
  _pled__mask__decode = pled_use_map | pled_overwrite_dup_keys | pled_encdec1v_for_names,
  _pled__mask__encode = pled_enc_empty_key_as_bse | pled_encdec1v_for_names
};

struct paramline::_opt_unity
{
  bool b_has_v;
  unity v;
  _opt_unity(__bmdx_noarg1) { b_has_v = false; }
};

  // Private utilities + { branch path; branch } structure.
struct _paramline_branch
{
  unity usp; // branch path as utString; utEmpty means uninitialized
  const unity* pv; // valid pointer to value; 0 means uninitialized

  _paramline_branch(__bmdx_noarg1) { pv = 0; }
  void swap(_paramline_branch& x) { if (this != &x) { swap_bytes(*this, x); } }

    // Appends sk to curr_path via wvterm. If curr_path is empty, also prepends pathpfx.
    //  Sets result into ret_spath.
    // Returns: ret_spath.
    // NOTE ret_spath may be safely the same object as any of arguments.
  static arrayref_t<wchar_t> add_to_path(const arrayref_t<wchar_t>& pathpfx, const arrayref_t<wchar_t>& curr_path, const arrayref_t<wchar_t>& sk, std::wstring& ret_spath __bmdx_noarg)
  {
    std::wstring x;
    x.assign(curr_path.pd(), curr_path._end_u());
      if (x.empty()) { x.append(pathpfx.pd(), pathpfx._end_u()); }
        x += paramline::wvterm;
        x.append(sk.pd(), sk._end_u());
    ret_spath.swap(x);
    return ret_spath;
  }
    // Helper for x_encode_branch.
    // Encodes k to string - with x_encode1v as array element, then appends to curr_path (see also add_to_path).
    //  Sets result into ret_spath.
    // Returns: ret_spath.
    // NOTE ret_spath may be safely the same object as any of arguments.
  static arrayref_t<wchar_t> _add_to_path_with_enc1v_ar(bool b_enc1v, bool b_bse, const arrayref_t<wchar_t>& pathpfx, const arrayref_t<wchar_t>& curr_path, const unity& k, hashx<const unity*, s_long>& hstopv, std::wstring& ret_spath, const arrayref_t<wchar_t>& pterm2 __bmdx_noarg)
  {
    std::wstring s1;
    if (b_enc1v)
    {
      if (k.isArray())
        { paramline::x_repl_e1(_vstr_scalar(k), s1, false, true, false, pterm2); }
      else
        { paramline::x_encode1v(k, s1, pled__ar_elem, 0, &hstopv, pterm2); }
    }
    else
    {
        // NOTE The following cases are same as in x_encode1n with unset pled_encdec1v_for_names.
        //  This leads to key in path being always decoded (x_decode1v)
        //  to string that is exactly equal to trivially decoded (unescaped)
        //  string key from the upper text line,
        //  where the addressed array is originally defined.
        //  (Decoding keys in paths and terminal keys uses different rules,
        //    but decoded key, in both cases, yields the same string,
        //    i.e. points to the same array inside the tree.)
      if (k.isEmpty()) { if (b_bse) { s1 = L"\\e"; } }
        else if (k.isString()) { s1 = k.rstr(); }
        else { s1 = _vstr_scalar(k); }
      paramline::x_encode1v(unity(s1), s1, pled__ar_elem, 0, 0, pterm2);
    }
    paramline::x_replace2a(s1, 1, pterm2);
    add_to_path(pathpfx, curr_path, s1, s1);
    ret_spath.swap(s1);
    return ret_spath;
  }
    // Recursively finds branches in array a (non-array is ignored) and all its subarrays.
    //  Adds branches, together with their paths, into abr.
    //  NOTE abr not cleared by collect_array_subbranches.
    // curr_path:
    //    specifies path of a itself, not including its \z<spec.> (added automatically if necessary)
    // b_use_am_in_paths:
    //    true - add \z<spec.> to path to address an array element.
    //      Generated paths will contain sequences like |\z<spec.>|<index>.
    //    false - do not add \z<spec.> to paths.
    //      Generated paths will contain |<index> only to address an array element,
    //      presuming that array itself is fully constructed in the line, describing upper-level assoc. array.
    // NOTE If array element contains an object (utObject / unity), it is automatically recursively dereferenced.
    // Returns: >= 0 - number of branches, added to abr.
  static s_long collect_array_subbranches(const arrayref_t<wchar_t>& pathpfx, const arrayref_t<wchar_t>& curr_path, const unity& a, hashx<const unity*, s_long>& hstopv, vec2_t<_paramline_branch, __vecm_tu_selector>& abr, bool b_use_am_in_paths __bmdx_noarg)
  {
    if (a.utype() != utUnityArray) { return 0; }
    s_long n = 0;
    std::wstring curr_path_full; bool b_cpf = false;
    unity _empty;
    for (s_long i = a.arrlb(); i <= a.arrub(); ++i)
    {
      const unity& u = deref_pu_once(a.ref<utUnity>(i), _empty, &hstopv);
      const bool b_subobj = u.isAssoc() || u.utype() == utUnityArray;
      if (!b_subobj) { continue; }
      if (!b_cpf)
      {
        if (b_use_am_in_paths)
        {
          add_to_path(pathpfx, curr_path, L"\\z", curr_path_full);
          if (a.arrlb() != 1) { curr_path_full += L"b"; curr_path_full += itows(a.arrlb(), true); }
        }
        else
        {
          curr_path_full.assign(curr_path.pd(), curr_path._end_u());
        }
        b_cpf = true;
      }
      if (u.isAssoc())
      {
        _paramline_branch rb;
        add_to_path(pathpfx, curr_path_full, itows(i, true), rb.usp.rx<utString>());
        rb.pv = &u;
        abr.push_back(_paramline_branch());
        abr.back().swap(rb);
        n += 1;
      }
      else if (u.utype() == utUnityArray)
      {
        std::wstring sp;
          add_to_path(pathpfx, curr_path_full, itows(i, true), sp);
        n += collect_array_subbranches(pathpfx, sp, u, hstopv, abr, b_use_am_in_paths);
      }
    }
    return n;
  }

  static const unity& deref_pu_once(const unity& _x0, const unity& _empty, hashx<const unity*, s_long>* phstop __bmdx_noarg)
  {
    const unity* _px = &_x0;
    if (_px->isObject())
    {
      while (1)
      {
        const unity* pv = _px->objPtr_c<unity>();
        if (!pv) { break; }
        if (phstop)
        {
          if (phstop->find(pv)) { _px = &_empty; break; }
          phstop->opsub(pv);
        }
        _px = pv;
      }
    }
    return *_px;
  }

};

  // 1 - found >0 spaces.
  //    ret_iend points to after-spaces part or string end.
  // 0 - pos is out of string or points to string end.
  //    ret_iend points to string end.
  // -1 - found non-space character at pos.
  //    ret_iend = pos.
int paramline::_match_spaces(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend)
{
  if (s.is_empty() || !(pos >= 0 && pos < s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return 0; }
  s_ll pos2 = pos;
  while (1)
  {
    if (pos2 >= s.n()) { ret_iend = s.n(); return pos2 > pos ? 1 : 0; }
    if (s[pos2] != L' ') { ret_iend = pos2; return pos2 > pos ? 1 : -1; }
    pos2 += 1;
  }
}

  // 1 - s[pos] is any of cc.
  // 0 - pos is exactly end of string.
  // -1 - s[pos] is any character not in cc.
  // -2 - pos is out of [0..s.n()], or s is not valid.
int paramline::_match_character(arrayref_t<wchar_t> s, s_ll pos, arrayref_t<wchar_t> cc)
{
  if (!s.is_valid()) { return -2; }
  if (pos == s.n()) { return 0; }
  if (pos >= 0 && pos < s.n()) { s_ll i = cc.find1(s[pos]); return i >= 0 && i < cc.n() ? 1 : -1; }
  return -2;
}

  // Acts similarly to _match_uint64, only for signed ints.
  //  NOTE If sign character (+, -) is present, between sign and digits only space characters may appear.
  //  NOTE If terms contain + or -, leading sign of the integer is interpreted as sign, not as term.
int paramline::_match_int32(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_x, arrayref_t<wchar_t> terms)
{
  if (s.is_empty() || !(pos >= 0 && pos < s.n())) { ret_x = 0; ret_iend = bmdx_minmax::myllmax(0, s.n()); return 0; }

  s_ll pos2 = pos;
  wchar_t c = s[pos2];
  bool b_sign_mark = false;
  bool b_negative = false;
    if (c == L'+' || c == L'-')
    {
      b_sign_mark = true;
      b_negative = c == L'-';
      pos2 += 1;
    }
  int res;
  if (b_sign_mark)
  {
    res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_x = 0; ret_iend = pos; return -1; }
  }
  s_ll pos3 = pos2;
  bmdx_meta::u_ll x64 = 0;
  res = _match_uint64(s, pos3, pos3, x64, terms);
  if (res == 1 || (res != 0 && pos3 > pos2))
  {
    ret_iend = pos3;
    bool b_overflow = false;
    if (b_negative && x64 > 0x80000000ull) { while (x64 > 0x80000000ull) { x64 /= 10; } b_overflow = true; }
    if (!b_negative && x64 > 0x7fffffffull) { while (x64 > 0x7fffffffull) { x64 /= 10; } b_overflow = true; }
    if (b_negative) { ret_x = s_long(-s_ll(x64)); }
      else { ret_x = s_long(x64); }
    if (res != 1 || b_overflow) { return -1; }
    return 1;
  }
  ret_x = 0; ret_iend = pos3 > pos2 ? pos3 : pos;
  return -1;
}

  // NOTE digits in terms are considered as terminating characters (not as digits).
  // 1 - found an unsigned integer, followed by terminating character or string end.
  //    ret_iend points to that character or string end.
  // 0 - pos is out of string or points to string end.
  //    ret_iend points to string end.
  // -1 - found one of
  //    a) non-terminating character
  //    b) terminating character at once (without digits)
  //    c) too large number
  //    ret_iend points to first bad character, or to end of digits.
int paramline::_match_uint64(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, bmdx_meta::u_ll& ret_x, arrayref_t<wchar_t> terms)
{
  if (s.is_empty() || !(pos >= 0 && pos < s.n())) { ret_x = 0; ret_iend = bmdx_minmax::myllmax(0, s.n()); return 0; }
  s_ll pos2 = pos;
  bmdx_meta::u_ll x = 0;
  bool b_overflow = false;
  while (1)
  {
    bool b_end = pos2 >= s.n();
    const wchar_t c = b_end ? 0 : s[pos2];
    bool b_term = false;
      if (!b_end) { s_ll i = terms.find1(c); if (i >= 0 && i < terms.n()) { b_term = true; } }
    bool b_digit = !b_end && !b_term && c >= L'0' && c <= L'9';
    if (b_digit)
    {
      if (!b_overflow && x > (~0ull / 10)) { b_overflow = true; }
      if (!b_overflow) { x *= 10; x += c - L'0'; }
    }
    if (b_end || b_term || !b_digit)
    {
      ret_x = x; ret_iend = pos2;
      if (b_overflow) { return -1; }
      if (pos2 > pos && (b_end || b_term)) { return 1; }
      return -1;
    }
    pos2 += 1;
  }
}

  // 2 - found <index>=(.*), ret_iend points to rvalue (right after equality sign).
  // 1 - found <index>( *).
  //    ret_iend points to string end.
  // 0 - pos is exactly end of string, or all characters starting from pos are spaces.
  //    ret_iend points to string end.
  // -1 - pos points to something other than tested data type.
  //    ret_iend = pos.
  // -2 - pos is out of [0..s.n()], or s is not valid.
  //    ret_iend points to string end.
int paramline::_msp_op_index(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_i)
{
  ret_i = 0;
  if (!s.is_valid() || !(pos >= 0 && pos <= s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return -2; }
  if (pos == s.n()) { ret_iend = s.n(); return 0; }
  s_ll pos2 = pos;
  int res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_iend = s.n(); return 0; }
  s_ll pos3 = pos2; s_long x32 = 0;
  res = _match_int32(s, pos3, pos3, x32, L" =");
    if (res != 1) { ret_iend = pos; return -1; }
  s_ll pos4 = pos3; res = _match_spaces(s, pos4, pos4);
    if (pos4 < s.n())
    {
      if (s[pos4] != L'=') { ret_iend = pos; return -1; }
      ret_i = x32; ret_iend = pos4 + 1;
      return 2;
    }
  ret_i = x32; ret_iend = s.n();
  return 1;
}
  // 2 - found +=(.*), ret_iend points to rvalue (right after equality sign).
  // 0 - pos is exactly end of string, or all characters starting from pos are spaces.
  //    ret_iend points to string end.
  // -1 - pos points to something other than tested data type.
  //    ret_iend = pos.
  // -2 - pos is out of [0..s.n()], or s is not valid.
  //    ret_iend points to string end.
int paramline::_msp_op_pluseq(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend)
{
  if (!s.is_valid() || !(pos >= 0 && pos <= s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return -2; }
  if (pos == s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos2 = pos;
  int res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos3 = pos2;
  res = _match_character(s, pos3, L"+");
    if (res != 1) { ret_iend = pos; return -1; }
  pos3 += 1;

  s_ll pos4 = pos3; res = _match_spaces(s, pos4, pos4);
    if (pos4 >= s.n()) { ret_iend = pos; return -1; }
    if (s[pos4] != L'=') { ret_iend = pos; return -1; }
  ret_iend = pos4 + 1;
  return 2;
}
  // Similar to _msp_op_plus, but looks for ++=(.*)
int paramline::_msp_op_pluspluseq(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend)
{
  if (!s.is_valid() || !(pos >= 0 && pos <= s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return -2; }
  if (pos == s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos2 = pos;
  int res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos3 = pos2;
  res = _match_character(s, pos3, L"+");
    if (res != 1) { ret_iend = pos; return -1; }
  pos3 += 1;

  res = _match_character(s, pos3, L"+");
    if (res != 1) { ret_iend = pos; return -1; }
  pos3 += 1;

  s_ll pos4 = pos3; res = _match_spaces(s, pos4, pos4);
    if (pos4 >= s.n()) { ret_iend = pos; return -1; }
    if (s[pos4] != L'=') { ret_iend = pos; return -1; }
  ret_iend = pos4 + 1;
  return 2;
}
  // Similar to _msp_op_plus, but looks for b<integer>
int paramline::_msp_b(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_b)
{
  ret_b = 0;
  if (!s.is_valid() || !(pos >= 0 && pos <= s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return -2; }
  if (pos == s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos2 = pos;
  int res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos3 = pos2;
  res = _match_character(s, pos3, L"b");
    if (res != 1) { ret_iend = pos; return -1; }
  pos3 += 1;

  s_long x32 = 0;
  s_ll pos4 = pos3; res = _match_int32(s, pos4, pos4, x32, L" bn+-");
    if (res != 1) { ret_iend = pos; return -1; }
  ret_b = x32; ret_iend = pos4;
  return 2;
}
  // Similar to _msp_op_plus, but looks for n<unsigned integer>
int paramline::_msp_n(arrayref_t<wchar_t> s, s_ll pos, s_ll& ret_iend, s_long& ret_n)
{
  ret_n = 0;
  if (!s.is_valid() || !(pos >= 0 && pos <= s.n())) { ret_iend = bmdx_minmax::myllmax(0, s.n()); return -2; }
  if (pos == s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos2 = pos;
  int res = _match_spaces(s, pos2, pos2);
    if (pos2 >= s.n()) { ret_iend = s.n(); return 0; }

  s_ll pos3 = pos2;
  res = _match_character(s, pos3, L"n");
    if (res != 1) { ret_iend = pos; return -1; }
  pos3 += 1;

  s_long x32 = 0;
  s_ll pos4 = pos3; res = _match_int32(s, pos4, pos4, x32, L" bn+-");
    if (res != 1) { ret_iend = pos; return -1; }
    if (x32 < 0) { ret_iend = pos; return -1; }
  ret_n = x32; ret_iend = pos4;
  return 2;
}


struct _paramline_array_spec
{
  enum { _no_op = 0, op_append = 1, op_assign_at_index = 3, ibase_dflt = 1 };

  bool b_has_base;
    s_ll ibase;
  bool b_has_n;
    s_ll n_elem;
  bool b_has_op() const { return op_type != _no_op; }
    int op_type;
    bool b_op_append_vec;
    s_ll iassign;
    arrayref_t<wchar_t> rvalue; // pos. of rvalue in _init_from_string spec arg.

  _paramline_array_spec(__bmdx_noarg1) { clear(); }

  void clear() { b_has_base = false; ibase = ibase_dflt; b_has_n = false; n_elem = 0; op_type = _no_op; b_op_append_vec = false; iassign = ibase_dflt; }

  static bool sb_valid_int32(s_ll x __bmdx_noarg) { return x >= -0x80000000ll && x <= 0x7fffffffll; }
  static bool sb_valid_size31(s_ll x __bmdx_noarg) { return x >= 0 && x <= 0x7fffffffll; }

  bool b_valid_ibase32() const { return sb_valid_int32(ibase); }
  bool b_valid_n_elem32() const { return sb_valid_size31(n_elem); }
  bool b_valid_iassign32() const { return sb_valid_int32(iassign); }

    // Decode array spec. (\z<spec>).
    //  The function is meant to be called from x_decode1v,
    //  so that input spec is already preprocessed with x_replace4 and _trim(, L" ").
    // true if succeeded (all fields are correctly initialized).
    // false if failed due to bad spec (fields are partially modified).
  bool _init_from_string(arrayref_t<wchar_t> spec)
  {
    clear();

    // Simplified spec. for parsing:
    //  " *(+|-)? *\d+ *(|=.*)$" --> <integer index> <opt. rvalue>
    //  " *+{1,2} *=.*$" --> <array simple ops.> <opt. rvalue>
    //  " *b(+|-)? *\d+(.|$)" --> <base index>
    //  " *n(+)? *\d+(.|$)" --> <size>

    if (spec.is_empty()) { return true; }
    s_ll pos2 = 0;
    int res = 0; bool b_has_value = false;
    while (1)
    {
      res = paramline::_match_spaces(spec, pos2, pos2);
        if (res == 0 || pos2 == spec.n()) { return true; }
      s_ll pos = pos2;
      s_long x32 = 0;
      res = paramline::_msp_b(spec, pos, pos2, x32);
        if (res == 2) { b_has_base = true; ibase = x32; continue; }
          else if (res != -1) { return false; }
      res = paramline::_msp_n(spec, pos, pos2, x32);
        if (res == 2) { b_has_n = true; n_elem = x32; continue; }
          else if (res != -1) { return false; }
      res = paramline::_msp_op_index(spec, pos, pos2, x32);
        if (res == 2 || res == 1) { op_type = op_assign_at_index; iassign = x32; b_has_value = res == 2; break; }
          else if (res != -1) { return false; }
      res = paramline::_msp_op_pluseq(spec, pos, pos2);
        if (res == 2) { op_type = op_append; iassign = x32; b_op_append_vec = false; b_has_value = true; break; }
          else if (res != -1) { return false; }
      res = paramline::_msp_op_pluspluseq(spec, pos, pos2);
        if (res == 2) { op_type = op_append; iassign = x32; b_op_append_vec = true; b_has_value = true; break; }
          else if (res != -1) { return false; }
      return false;
    }
    if (b_has_value) { rvalue.link(&spec[pos2], spec.n() - pos2); }
      else { rvalue.link(spec._end_u(), 0); }

    return true;
  }

  bool _ensure_array_shape(unity& vdest) const __bmdx_noex
  {
    try {
      if (vdest.utype() != utUnityArray)
      {
        vdest.arr_init<utUnity>(1);
      }
      if (b_has_base)
      {
        if (!b_valid_ibase32()) { return false; }
        vdest.arrlb_(s_long(ibase & 0xffffffffll));
      }
      if (b_has_n)
      {
        if (!b_valid_n_elem32()) { return false; }
        vdest.arrsz_(s_long(n_elem & 0x7fffffffll));
      }
      return true;
    } catch (...) {}
    return false;
  }
};

unity& paramline::decode1v(arrayref_t<wchar_t> ssrc0, unity& dest)
{
  dest.clear();
  try
  {
    std::wstring s; s_long replflags(0);
    x_replace4(ssrc0, s, replflags);
    _t_wz pos0 = s.find(wpterm_short); if (pos0 != nposw) { s.resize(pos0); }
    arrayref_t<wchar_t> ssv = _trim_arrayref(arrayref_t<wchar_t>(s), L" ");
    x_decode1v(ssv, false, replflags, dest, x_pterm2_dflt());
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode1v.1", e.what(), _fls75(ssrc0.pd(), ssrc0.n())); }
  catch(...) { throw XUExec("decode1v.2", _fls75(ssrc0.pd(), ssrc0.n())); }
  return dest;
}

  // mha:
  //  will be a) 0-based array (key, value, key, value...) if pled__use_array flag is given,
  //    b) else map if pled_use_map flag is given,
  //    c) else hashlist.
  // pret_path:
  //      a) if given:
  //        1) must have pret_path->v.isLocal() == true.
  //        2) values with implicit empty key ("=<value>") are considered branch paths,
  //          and treated separately (decoded in special way and put into pret_path->v)
  //          from regular values with explicit empty key "\e=<value>" (put into mha).
  //      b) If pret_path == 0, all values with empty key are processed as usual,
  //        according to pled_overwrite_dup_keys flag.
  // pret_replflags: optional, receives flags, returned by x_replace4.
  // pterm_0:
  //    must be valid. If empty, wCRLF is assumed.
  // ff_encdec:
  //  pled__use_array, pled_use_map: see mha above.
  //  pled_encdec1v_for_names
  //  pled_overwrite_dup_keys
  //  pled__get_strvals:
  //    if set, the returned values will be
  //      a) undecoded strings (result of applying x_replace4 on ssrc0 and splitting into key and value substrings).
  //        The client should later call x_decode1v on them and supply *pret_replflags, returned by x_decode.
  //      b) utEmpty, where no value assignment was specified (key only existed).
  //    NOTE This flag should be used with pled__use_array only,
  //      because direct overwriting string values in associative mha
  //      might lose multipart array modifiers (multiple pairs "key = \z<spec.>")
void paramline::x_decode(arrayref_t<wchar_t> ssrc0, unity& mha, s_long ff_encdec, arrayref_t<wchar_t> pterm2_0, _opt_unity* pret_path, s_long* pret_replflags)
{
  if (!(ssrc0.is_valid() && pterm2_0.is_valid())) { throw XUExec("decode.3"); }
  const bool b_use_array = !!(ff_encdec & pled__use_array);
  const bool b_strvals = !!(ff_encdec & pled__get_strvals);
  const bool b_use_map = !b_use_array && !!(ff_encdec & pled_use_map);
  const bool b_1v_for_names = !!(ff_encdec & pled_encdec1v_for_names);
  const bool b_overwrite = !!(ff_encdec & pled_overwrite_dup_keys);
  std::wstring src_;
  try
  {
    if (pret_path) { pret_path->b_has_v = false; pret_path->v.clear(); }

    arrayref_t<wchar_t> pterm2 = pterm2_0.is_empty() ? wCRLF : pterm2_0;
    if (b_use_map) { mha.map_clear(false); }
      else if (b_use_array) { mha.arr_init<utUnity>(0); }
      else { mha.hash_clear(false); }

    s_long replflags(0);
    x_replace4(ssrc0, src_, replflags);
      if (pret_replflags) { *pret_replflags = replflags; }
    src_ = replace(src_, wpterm_short, pterm2);
    arrayref_t<wchar_t> src_ar = src_;
    s_ll pos0(0), pos2(0);
    while(pos0 < src_ar.n())
    {
      pos2 = _find_str_linear(src_ar, pterm2, pos0);

      arrayref_t<wchar_t> s_(&src_ar[pos0], pos2 - pos0);
      s_ll pos_eq = _find_str_linear(s_, arrayref_t<wchar_t>(weterm_short), 0);
      const bool b_has_asg = pos_eq < s_.n();

      arrayref_t<wchar_t> rsk = _trim_arrayref(s_.range_intersect(0, pos_eq), L" ");
      arrayref_t<wchar_t> ssv;
        if (b_has_asg) { ssv = _trim_arrayref(s_.range_intersect(pos_eq + 1), L" "); }

      pos0 = pos2 + pterm2.n();
        if (rsk.is_empty() && !b_has_asg) { continue; }

        // Special case: values with implicit empty key ("=<value>") are considered branch paths.
      const bool b_branch_path = rsk.is_empty();
      if (b_branch_path && pret_path)
      {
        if (!pret_path->b_has_v || b_overwrite)
        {
          pret_path->v.clear();
          x_decode1v(ssv, false, replflags | plrf_use_path_array_modifiers, pret_path->v, pterm2);
          pret_path->b_has_v = true;
        }
        continue;
      }

      unity ku;
        if (b_1v_for_names)
        {
          x_decode1v(rsk, false, replflags, ku, pterm2);
        }
        else
        {
          ku.u_clear(utString);
          std::wstring& ks = ku.rstr();
          ks.append(rsk.pd(), rsk._end_u());
          x_replace2a(ks, replflags, pterm2);
        }

      if (b_use_array)
      {
        mha.arr_append(ku);
        mha.arr_append(unity());
        unity& v = mha.ua_last();
        if (b_has_asg)
        {
          if (b_strvals)
          {
            std::wstring s; s.assign(ssv.pd(), ssv._end_u());
            if (v.isLocal()) { v.rx<utString>().swap(s); }
              else { v = s; }
          }
          else { x_decode1v(ssv, false, replflags, v, pterm2); }
        }
        continue;
      }

      const bool b_v_ex = mha.u_has(ku, 6);
      if (b_has_asg)
      {
        unity& v = mha(ku);
        s_long f1 = b_v_ex ? plrf_value_exists : _plrf__nullflags;
        s_long f2 = b_overwrite ? plrf_allow_modify_all : _plrf__nullflags;
        if (b_strvals)
        {
          if (!(b_v_ex && !b_overwrite))
          {
            std::wstring s; s.assign(ssv.pd(), ssv._end_u());
            if (v.isLocal()) { v.rx<utString>().swap(s); }
              else { v = s; }
          }
        }
        else
        {
          x_decode1v(ssv, false, replflags | f1 | f2, v, pterm2);
        }
      }
      else
      {
        if (!(b_v_ex && !b_overwrite)) { mha.assoc_set(ku, unity(), true); }
      }
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode.1", e.what(), src_); }
  catch(...) { throw XUExec("decode.2", src_); }
}

unity& paramline::decode_tree(arrayref_t<wchar_t> ssrc0, unity& mha, s_long ff_encdec, arrayref_t<wchar_t> pterm2_0)
{
  if (!(ssrc0.is_valid() && pterm2_0.is_valid())) { throw XUExec("decode_tree.3"); }
  try {
    arrayref_t<wchar_t> pterm2 = pterm2_0.length() == 0 ? wCRLF : pterm2_0;
    const bool b_1v_for_names = !!(ff_encdec & pled_encdec1v_for_names);
    const bool b_overwrite = !!(ff_encdec & pled_overwrite_dup_keys);
    const bool b_clear = (ff_encdec & 0x4) == 0;
    bool b_map = !!(ff_encdec & 0x1);
      if (!b_clear && mha.isMap()) { b_map = true; }
    const bool b_skipslc = !!(ff_encdec & 0x8);
    const bool b_convcr = !!(ff_encdec & 0x10);
    const bool b_inclpath = !!(ff_encdec & 0x20) && !b_1v_for_names;
    const bool b_braces = !!(ff_encdec & 0x40);

    if (b_map) { if (b_clear || !mha.isMap()) { mha.map_clear(true); } }
      else { if (b_clear || !mha.isHash()) { mha.hash_clear(true); } }
    const s_long flags_assoc = mha.assocFlags_c();

    carray_r_t<wchar_t> s2;
    if (b_convcr)
    {
      if (!s2.reserve(ssrc0.n())) { throw XUExec("decode_tree.4"); }
      for (s_ll pos = 0; pos < ssrc0.n(); ++pos)
      {
        wchar_t c = ssrc0[pos];
        if (c == L'\r')
        {
          s_ll pos2 = pos + 1;
          if (pos2 < ssrc0.n() && ssrc0[pos2] == L'\n') { pos = pos2; }
          s2 += L"\r\n";
        }
        else if (c == L'\n')
        {
          s_ll pos2 = pos + 1;
          if (pos2 < ssrc0.n() && ssrc0[pos2] == L'\r') { pos = pos2; }
          s2 += L"\r\n";
        }
        else { s2 += c; }
      }
    }

    const unity k_path(L"");
    s_ll pos(0); arrayref_t<wchar_t> ssrc(b_convcr ? s2 : ssrc0);
    unity stack, eff_path;
      eff_path.arr_init<utUnityArray>(1);
      if (b_braces) { stack.ua_append(eff_path); }
    s_long iassign = 1; // the current array assignment index
      bool b_reset_iassign = true;
    while (pos < ssrc.n())
    {
      s_ll pos2 = _find_str_linear(ssrc, pterm2, pos);
      if (pos2 > pos)
      {
        bool b_skip = false;
        if (b_skipslc) // test/skip C-style comments
        {
          s_ll pos3 = _find_char_except(ssrc, pos, pos2, L"\t ");
          if (pos3 + 2 <= pos2 && ssrc[pos3] == L'/' && ssrc[pos3 + 1] == L'/')
            { b_skip = true; }
        }
        if (!b_skip) // skip lines w/o data
        {
          s_ll pos3 = _find_char_except(ssrc, pos, pos2, L" ");
          if (pos3 >= ssrc.n()) { b_skip = true; }
        }
        if (!b_skip && b_braces) // test/process lines with braces only
        {
          s_ll pos3 = _find_char_except(ssrc, pos, pos2, L" {}");
          if (pos3 >= ssrc.n())
          {
            for (s_ll i = pos; i < pos2; ++i)
            {
              if (ssrc[i] == L'{')
              {
                if (eff_path == stack.ua_last())
                {
                  eff_path.ua_append(unity());
                  b_reset_iassign = true;
                }
                stack.ua_append(eff_path);
              }
              else if (ssrc[i] == L'}')
              {
                if (stack.uaS() > 1) { stack.ua_resize(stack.uaUb(), -1); }
                eff_path = stack.ua_last();
                b_reset_iassign = true;
              }
            }
            b_skip = true;
          }
        }
        if (!b_skip) // decode branch, calc. path, merge
        {
          unity aksv; // 0-based (key, string, key, string...)
          bool b_has_brpath = false;
          unity _path_modifier;
          const unity* ppm = &_path_modifier;
          s_long replflags = 0;
          if (1)
          {
            s_long ff2 = 0;
              ff2 |= pled__use_array;
              ff2 |= pled__get_strvals;
              if (b_1v_for_names) { ff2 |= pled_encdec1v_for_names; }
              if (b_overwrite) { ff2 |= pled_overwrite_dup_keys; }
            _opt_unity brpath;
            x_decode(arrayref_t<wchar_t>(&ssrc[pos], pos2 - pos), aksv, ff2, pterm2, &brpath, &replflags);
            b_has_brpath = brpath.b_has_v;
            if (brpath.b_has_v) { b_reset_iassign = true; }
            if (brpath.v.utype() == utUnityArray)
            {
              if (b_braces) { _path_modifier = stack.ua_last(); }
                else { _path_modifier.arr_init<utUnity>(1); }
              for (int j = brpath.v.arrlb(); j <= brpath.v.arrub(); ++j)
                { _path_modifier.ua_append(brpath.v[j]); }
            }
            else
            {
              if (b_braces) { ppm = &stack.ua_last(); }
                else { ppm = &eff_path; }
            }
          }

          unity* node = &mha;
          bool b_rootnode = true;
          unity _new_path;
            if (b_has_brpath) { _new_path.arr_init<utUnity>(1); }
            for (int j = 1; j <= ppm->arrub(); ++j)
            {
                // At iteration start, node is granted to be container:
                //  a) map or hashlist,
                //  b) utUnityArray.
              const unity& rpe = (*ppm)[j];
              if (rpe.isObject()) // expecting: array modifier
              {
                if (!(
                  b_overwrite
                  || node->isAssoc()
                  || node->utype() == utUnityArray
                ))
                { node = 0; break; }
                const _paramline_array_spec& asp = rpe.objRef_c<_paramline_array_spec>();
                if (!asp._ensure_array_shape(*node)) { throw XUExec("decode_tree.5"); }
                continue;
              }

              if (b_has_brpath) { _new_path.ua_append(rpe); }

              if (node->utype() == utUnityArray)
              {
                if (!rpe.isIntOrEmpty()) { throw XUExec("decode_tree.6"); }
                s_ll ind = rpe.vint();
                if (!_paramline_array_spec::sb_valid_int32(ind)) { throw XUExec("decode_tree.7"); }
                node = &node->ua(s_long(ind));
              }
              else
              {
                node->assoc_set(rpe, unity(), true);
                node = &(*node)[rpe];
              }

              b_rootnode = false;

              if (node->isAssoc()) {}
              else if (node->utype() == utUnityArray) {}
              else if (node->isEmpty() || b_overwrite)
              {
                if (b_map) { node->u_clear(utMap); node->mapFlags_set(0, flags_assoc); }
                  else { node->u_clear(utHash); node->hashFlags_set(0, flags_assoc); }
              }
              else { node = 0; break; }
            }
            if (node && b_has_brpath && b_braces) { eff_path = _new_path; }

          if (node)
          {
            if (node->utype() == utUnityArray)
            {
              if (b_reset_iassign) { iassign = node->arrlb(); b_reset_iassign = false; }
              for (int ikv = 0; ikv < aksv.arrsz(); ikv += 2)
              {
                const unity& k0 = aksv[ikv];
                const unity& sv0 = aksv[ikv + 1];
                enum EBuildOp
                {
                  _noop = 0,
                  op_append_seq = 1,
                  op_append_1 = 2,
                  op_set_seq = 3,
                  op_set_1 = 4
                };
                int op_type = _noop;
                if (k0.isString())
                {
                  const std::wstring& sk = k0.rstr();
                  if (sk == wao_appseq) { op_type = op_append_seq; }
                    else if (sk == wao_app1) { op_type = op_append_1; }
                    else if (sk == wao_setseq) { op_type = op_set_seq; }
                    else if (sk == wao_set1) { op_type = op_set_1; }
                }
                if (op_type != _noop)
                {
                  unity v;
                    if (sv0.isString()) { x_decode1v(sv0.rstr(), false, replflags, v, pterm2); }

                  if (op_type == op_append_seq && v.utype() == utUnityArray)
                  {
                    iassign = node->arrub() + 1;
                    node->arrub_(node->arrub() + v.arrsz());
                    for (s_long j = v.arrlb(); j <= v.arrub(); ++j)
                      { node->ua(iassign++).swap(v[j]); }
                    continue;
                  }
                  else if (op_type == op_append_1 || op_type == op_append_seq)
                  {
                    node->ua_append(unity());
                    node->ua_last().swap(v);
                    iassign = node->arrub() + 1;
                    continue;
                  }

                  if (op_type == op_set_seq && v.utype() == utUnityArray)
                  {
                    for (s_long j = v.arrlb(); j <= v.arrub(); ++j)
                      { node->ua(iassign++).swap(v[j]); }
                    continue;
                  }
                  else if (op_type == op_set_1 || op_type == op_set_seq)
                  {
                    node->ua(iassign++).swap(v);
                    continue;
                  }

                }

                unity k1;
                const unity* pk = &k0;
                  //  With strictly string keys, still case is possible for
                  //    <integer_index>=<value>
                  //  where key remains in string form, and must be converted to integer in normal way.
                if (!b_1v_for_names && pk->isString())
                {
                  try { k1 = str2i(pk->rstr(), 0, false); pk = &k1; } catch (...) {}
                }

                if (pk->isIntOrEmpty())
                {
                  s_ll ind64 = pk->vint();
                  if (!_paramline_array_spec::sb_valid_int32(ind64)) { throw XUExec("decode_tree.8"); }
                  s_long j = s_long(ind64);
                  bool b_v_ex = node->u_has(j, 1);
                  unity& vdest = node->ua(j);
                  if (vdest.isEmpty()) { b_v_ex = false; }
                    if (sv0.isString())
                    {
                      s_long rff = replflags;
                        if (b_v_ex) { rff |= plrf_value_exists; }
                        if (b_overwrite) { rff |= plrf_allow_modify_all; }
                      x_decode1v(sv0.rstr(), false, rff, vdest, pterm2);
                    }
                    else
                    {
                      vdest.clear(); // key only (assigns utEmpty)
                    }
                  iassign = j + 1;
                  continue;
                }
                throw XUExec("decode_tree.9"); // unknown key type specified in the array area
              }
            }
            else
            {
              if (b_inclpath && !node->u_has(k_path, 6) && (!b_rootnode || b_has_brpath))
                { (*node)(k_path).clear(); }
              for (int ikv = 0; ikv < aksv.arrsz(); ikv += 2)
              {
                const unity& k = aksv[ikv];
                const unity& sv0 = aksv[ikv + 1];
                if (b_inclpath && k == k_path) { continue; }
                bool b_v_ex = node->u_has(k, 6);
                unity& vdest = (*node)(k);
                  if (sv0.isString())
                  {
                    s_long rff = replflags;
                      if (b_v_ex) { rff |= plrf_value_exists; }
                      if (b_overwrite) { rff |= plrf_allow_modify_all; }
                    x_decode1v(sv0.rstr(), false, rff, vdest, pterm2);
                  }
                  else
                  {
                    if (!b_v_ex || b_overwrite) { vdest.clear(); } // key only (assigns utEmpty)
                  }
              }
            }
          }
        }

      }
      pos = pos2 + pterm2.length();
    }
    if (b_inclpath)
    {
      struct _path_setter
      {
        const unity k_path;
        _path_setter() : k_path(L"") {}
        void f_top(unity& mha)
        {
          unity apath; apath.arr_init<utUnity>(1);
          f(mha, apath);
        }
        void f(unity& branch, unity& apath)
        {
          if (branch.isAssoc())
          {
            for (s_long pos = branch.assocl_first(); pos != branch.assocl_noel(); pos = branch.assocl_next(pos))
            {
              unity& x = branch.assocl(pos);
              if (x.isAssoc() || x.utype() == utUnityArray)
              {
                apath.ua_append(branch.assocl_key(pos));
                f(x, apath);
                apath.arrsz_(apath.arrsz() - 1);
              }
            }
            if (branch.u_has(k_path, 6)) { unity& k = branch[k_path]; if (k.isEmpty()) { k = apath; } }
          }
          else if (branch.utype() == utUnityArray)
          {
            for (s_long i = branch.arrlb(); i <= branch.arrub(); ++i)
            {
              unity& x = branch[i];
              if (x.isAssoc() || x.utype() == utUnityArray)
              {
                apath.ua_append(i);
                f(x, apath);
                apath.arrsz_(apath.arrsz() - 1);
              }
            }
          }
        }
      };
      _path_setter().f_top(mha);
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode_tree.1", e.what(), _fls75(ssrc0.pd(), ssrc0.n())); }
  catch(...) { throw XUExec("decode_tree.2", _fls75(ssrc0.pd(), ssrc0.n())); }
  return mha;
}

std::wstring& paramline::encode(const unity& mh, std::wstring& sdest, s_long ff_encdec, arrayref_t<wchar_t> pterm1_0)
{
  if (!pterm1_0.is_valid()) { throw XUExec("encode.3"); }
  try
  {
    sdest.clear(); if (!mh.isAssoc() || mh.assocS_c() == 0) { return sdest; }
    arrayref_t<wchar_t> pterm1 = pterm1_0.length() == 0 ? wpterm : pterm1_0;
    hashx<const unity*, s_long> hstop;
    s_long pos = mh.assocl_first();
    while (pos != mh.assocl_noel())
    {
      std::wstring sk, s1, s2;
      x_encode1n(mh.assocl_key(pos), sk, ff_encdec & _pled__mask__encode, x_pterm2_dflt());
      vec2_t<std::wstring, __vecm_tu_selector> xsubarrs;
      x_encode1v(mh.assocl_c(pos), s2, _pled__nullflags, &xsubarrs, &hstop, x_pterm2_dflt());
      if (1)
      {
        s1 += sk;
        if (s2.length() || s1.empty()) { s1 += weterm; s1 += s2; }
      }
      for (s_long j = 0; j < xsubarrs.n(); ++j)
      {
        s1.append(pterm1.pd(), pterm1._end_u());
        s1 += sk;
        s1 += weterm;
        s1 += xsubarrs[j];
      }
      x_replace2a(s1, 1, x_pterm2_dflt());
      pos = mh.assocl_next(pos);
      if (pos != mh.assocl_noel()) { s1.append(pterm1.pd(), pterm1._end_u()); }
      sdest += s1;
    }
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode.1", e.what(), mh.vflstr()); }
  catch(...) { throw XUExec("encode.2", mh.vflstr()); }
}

std::wstring paramline::encode1n(const unity& name, s_long ff_encdec)
{
  try { std::wstring s; x_encode1n(name, s, ff_encdec & _pled__mask__encode, x_pterm2_dflt()); x_replace2a(s, 1, x_pterm2_dflt()); return s; }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1n.1", e.what(), name.vflstr()); }
  catch(...) { throw XUExec("encode1n.2", name.vflstr()); }
}

std::wstring& paramline::encode1v(const unity& value, std::wstring& s)
{
  try
  {
    hashx<const unity*, s_long> hstop;
    x_encode1v(value, s, _pled__nullflags, 0, &hstop, x_pterm2_dflt());
    x_replace2a(s, 1, x_pterm2_dflt());
    return s;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1v.1", e.what(), "Failed to encode val.", value.utype()); }
  catch(...) { throw XUExec("encode1v.2", "Failed to encode val.", value.utype()); }
}

  // mha: map, hash, or array. Expected to be dereferenced (no case of utObject/unity).
  // path: path to mha in the tree.
  //  If mha is array, path should not contain its \z<spec.>
  //  (this will be done by x_encode_branch as necessary).
void paramline::x_encode_branch(s_long ff_encdec, const unity& mha, const arrayref_t<wchar_t>& path, std::wstring& sdest, hashx<const unity*, s_long>& hstopk, hashx<const unity*, s_long>& hstopv, arrayref_t<wchar_t> pterm, arrayref_t<wchar_t> pterm2, arrayref_t<wchar_t> pathpfx)
{
    // return if not assoc or empty assoc
    // append path if not empty
    // append non-branch pairs, collect branch pairs, skip stopped
    // append term2
    // for each branch, call x_encode_branch recursively with path + string(branch key)
  if (!(pterm.is_valid() && pterm2.is_valid())) { throw XUExec("x_encode_branch.5"); }
  const bool b_1v_for_names = !!(ff_encdec & pled_encdec1v_for_names);
  const bool b_bse = !!(ff_encdec & pled_enc_empty_key_as_bse);

  if (mha.isAssoc())
  {
    bool b_line1_data = false;
      if (path.is_nonempty()) { b_line1_data = true; sdest.append(path.pd(), path._end_u()); }
    hashx<std::wstring, s_long, hashx_common::kf_basic<std::wstring>, __vecm_tu_selector> hsk;
    s_long res = -1;
    unity _empty;
    vec2_t<_paramline_branch, __vecm_tu_selector> abr;
    for (s_long pos = mha.assocl_first(); pos != mha.assocl_noel(); pos = mha.assocl_next(pos))
    {
      const unity* pk = &_paramline_branch::deref_pu_once(mha.assocl_key(pos), _empty, &hstopk);
        if (pk == &_empty) { continue; }
        res = hstopk.insert(pk);
          if (res == 0) { continue; }
          if (res != 1) { throw XUExec("x_encode_branch.1"); }

      const unity* px = &_paramline_branch::deref_pu_once(mha.assocl_c(pos), _empty, &hstopv);

      std::wstring sk;
        x_encode1n(*pk, sk, ff_encdec & _pled__mask__encode, pterm2);
            // Key, represented as empty string, is ignored because it conflicts with path specifier key.
            // Except for when the key occurs in path only (i.e. leads to assoc. array).
          if (sk.empty() && !px->isAssoc()) { continue; }
        res = hsk.insert(sk);
          if (res == 0) { continue; }
          if (res != 1) { throw XUExec("x_encode_branch.2"); }

      if (px->isAssoc())
      {
        if (b_line1_data) { sdest.append(pterm2.pd(), pterm2._end_u()); b_line1_data = false; }
        std::wstring _sp;
            // NOTE b_enc1v = true ensures compaibility with non-array-aware paramline,
            //    when encoding paths of assoc. arrays.
          _paramline_branch::_add_to_path_with_enc1v_ar(true, b_bse, pathpfx, path, *pk, hstopv, _sp, pterm2);
        x_encode_branch(ff_encdec, *px, _sp, sdest, hstopk, hstopv, pterm, pterm2, pathpfx);
        continue;
      }
        // Whem *px is utUnityArray, it may contain subbranches, directly of through nested arrays.
        //  Such subbranches are saved into abr, to be encoded later.
        //  First, *px array is encoded as one or more entries (array building with \z<spec.>).
      const bool b_uar = px->utype() == utUnityArray;
      if (b_uar)
      {
        std::wstring _sp;
          _paramline_branch::_add_to_path_with_enc1v_ar(b_1v_for_names, b_bse, pathpfx, path, *pk, hstopv, _sp, pterm2);
          _paramline_branch::collect_array_subbranches(pathpfx, _sp, *px, hstopv, abr, false);
      }

      vec2_t<std::wstring, __vecm_tu_selector> xsubarrs;
      std::wstring s1, s2;
        // NOTE If b_uar, hstopv is not used with *px twice, because it's already been used
        //  during collect_array_subbranches call, see above.
      x_encode1v(*px, s2, pled__enc_assoc_as_empty, &xsubarrs, b_uar ? 0 : &hstopv, pterm2);
      if (1)
      {
        s1 += sk;
        if (s2.length() || s1.empty()) { s1 += weterm; s1 += s2; }
      }
      for (s_long j = 0; j < xsubarrs.n(); ++j)
      {
        s1.append(pterm.pd(), pterm._end_u());
        s1 += sk;
        s1 += weterm;
        s1 += xsubarrs[j];
      }
      x_replace2a(s1, 1, pterm2);

      if (!b_line1_data && path.is_nonempty()) { b_line1_data = true; sdest.append(path.pd(), path._end_u()); }
      if (b_line1_data) { sdest.append(pterm.pd(), pterm._end_u()); }
      b_line1_data = true;
      sdest += s1;
      if (abr.n() > 0)
      {
        sdest.append(pterm2.pd(), pterm2._end_u());
        b_line1_data = false;
        for (s_long i = 0; i < abr.n(); ++i)
          { x_encode_branch(ff_encdec, *abr[i].pv, abr[i].usp.rstr(), sdest, hstopk, hstopv, pterm, pterm2, pathpfx); }
        abr.clear();
      }
    }
    if (b_line1_data) { sdest.append(pterm2.pd(), pterm2._end_u()); }

    return;
  }
  if (mha.isArray()) // this case is for root array, but also can work with non-empty path
  {
    const unity* pa = &mha;
    const bool b_empty_array = pa->isArray() && pa->arrsz() == 0;

    std::wstring path_full;
    _paramline_branch::add_to_path(pathpfx, path, L"\\z", path_full);
    if (pa->arrlb() != 1) { path_full += L"b"; path_full += itows(pa->arrlb(), true); }

    if (b_empty_array)
    {
      path_full += L"n0";
      sdest += path_full;
      sdest.append(pterm2.pd(), pterm2._end_u());
      return;
    }

    const bool b_uar = pa->utype() == utUnityArray;
    vec2_t<_paramline_branch, __vecm_tu_selector> abr;
    if (b_uar)
      { _paramline_branch::collect_array_subbranches(pathpfx, path, *pa, hstopv, abr, false); }

    vec2_t<std::wstring, __vecm_tu_selector> xsubarrs;
    std::wstring s1, s2;
      // NOTE If b_uar, hstopv is not used with *px twice, because it's already been used
      //  during collect_array_subbranches call, see above.
    x_encode1v(*pa, s2, pled__enc_assoc_as_empty | pled__enc1v_for_array_area, &xsubarrs, b_uar ? 0 : &hstopv, pterm2);
    if (1)
    {
      s1 += wao_setseq;
      s1 += weterm_short;
      s1 += s2;
    }
    for (s_long j = 0; j < xsubarrs.n(); ++j)
    {
      s1.append(pterm.pd(), pterm._end_u());
      s1 += xsubarrs[j];
    }
    x_replace2a(s1, 1, pterm2);

    sdest += path_full;
    sdest.append(pterm.pd(), pterm._end_u());
    sdest += s1;
    sdest.append(pterm2.pd(), pterm2._end_u());
    if (abr.n() > 0)
    {
      for (s_long i = 0; i < abr.n(); ++i)
        { x_encode_branch(ff_encdec, *abr[i].pv, abr[i].usp.rstr(), sdest, hstopk, hstopv, pterm, pterm2, pathpfx); }
    }
    return;
  }
}

std::wstring& paramline::encode_tree(const unity& _mha0, std::wstring& sdest, s_long ff_encdec, arrayref_t<wchar_t> pterm1_0, arrayref_t<wchar_t> pterm2_0)
{
  if (!(pterm1_0.is_valid() && pterm2_0.is_valid())) { throw XUExec("encode_tree.3"); }
  try
  {
    hashx<const unity*, s_long> hstopk, hstopv;
    unity _empty;
    const unity& mha = _paramline_branch::deref_pu_once(_mha0, _empty, &hstopv);
    sdest.clear();
      if (!(mha.isAssoc() || mha.isArray())) { return sdest; }
    arrayref_t<wchar_t> pterm1 = pterm1_0.length() == 0 ? wpterm : pterm1_0;
    arrayref_t<wchar_t> pterm2 = pterm2_0.length() == 0 ? wCRLF : pterm2_0;
    arrayref_t<wchar_t> pathpfx = _trim_arrayref(arrayref_t<wchar_t>(weterm), L" ", true, true);
    s_long ff = 0;
      if (ff_encdec & pled_encdec1v_for_names) { ff |= pled_encdec1v_for_names; }
      if (ff_encdec & pled_enc_empty_key_as_bse) { ff |= pled_enc_empty_key_as_bse; }
    x_encode_branch(ff, mha, L"", sdest, hstopk, hstopv, pterm1, pterm2, pathpfx);
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode_tree.1", e.what(), _mha0.vflstr()); }
  catch(...) { throw XUExec("encode_tree.2", _mha0.vflstr()); }
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

unity& paramline::merge(unity& set1, const arrayref_t<wchar_t>& set2_pl, bool keep_first)
  { return merge(set1, decode(set2_pl, true), keep_first); }

void paramline::_list_mx_set_u ( unity& mdest, s_long fk, _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58, _rcu x59, _rcu x60 )
{
  vec2_t<const unity*, __vecm_tu_selector> a; mdest.map_clear(true); mdest.mapFlags_set(-1, fk);
  do { if (&x1!=&_0) a.el_append(&x1); else break; if (&x2!=&_0) a.el_append(&x2); else break; if (&x3!=&_0) a.el_append(&x3); else break; if (&x4!=&_0) a.el_append(&x4); else break; if (&x5!=&_0) a.el_append(&x5); else break; if (&x6!=&_0) a.el_append(&x6); else break; if (&x7!=&_0) a.el_append(&x7); else break; if (&x8!=&_0) a.el_append(&x8); else break; if (&x9!=&_0) a.el_append(&x9); else break; if (&x10!=&_0) a.el_append(&x10); else break; if (&x11!=&_0) a.el_append(&x11); else break; if (&x12!=&_0) a.el_append(&x12); else break; if (&x13!=&_0) a.el_append(&x13); else break; if (&x14!=&_0) a.el_append(&x14); else break; if (&x15!=&_0) a.el_append(&x15); else break; if (&x16!=&_0) a.el_append(&x16); else break; if (&x17!=&_0) a.el_append(&x17); else break; if (&x18!=&_0) a.el_append(&x18); else break; if (&x19!=&_0) a.el_append(&x19); else break; if (&x20!=&_0) a.el_append(&x20); else break; if (&x21!=&_0) a.el_append(&x21); else break; if (&x22!=&_0) a.el_append(&x22); else break; if (&x23!=&_0) a.el_append(&x23); else break; if (&x24!=&_0) a.el_append(&x24); else break; if (&x25!=&_0) a.el_append(&x25); else break; if (&x26!=&_0) a.el_append(&x26); else break; if (&x27!=&_0) a.el_append(&x27); else break; if (&x28!=&_0) a.el_append(&x28); else break; if (&x29!=&_0) a.el_append(&x29); else break; if (&x30!=&_0) a.el_append(&x30); else break; if (&x31!=&_0) a.el_append(&x31); else break; if (&x32!=&_0) a.el_append(&x32); else break; if (&x33!=&_0) a.el_append(&x33); else break; if (&x34!=&_0) a.el_append(&x34); else break; if (&x35!=&_0) a.el_append(&x35); else break; if (&x36!=&_0) a.el_append(&x36); else break; if (&x37!=&_0) a.el_append(&x37); else break; if (&x38!=&_0) a.el_append(&x38); else break; if (&x39!=&_0) a.el_append(&x39); else break; if (&x40!=&_0) a.el_append(&x40); else break; if (&x41!=&_0) a.el_append(&x41); else break; if (&x42!=&_0) a.el_append(&x42); else break; if (&x43!=&_0) a.el_append(&x43); else break; if (&x44!=&_0) a.el_append(&x44); else break; if (&x45!=&_0) a.el_append(&x45); else break; if (&x46!=&_0) a.el_append(&x46); else break; if (&x47!=&_0) a.el_append(&x47); else break; if (&x48!=&_0) a.el_append(&x48); else break; if (&x49!=&_0) a.el_append(&x49); else break; if (&x50!=&_0) a.el_append(&x50); else break; if (&x51!=&_0) a.el_append(&x51); else break; if (&x52!=&_0) a.el_append(&x52); else break; if (&x53!=&_0) a.el_append(&x53); else break; if (&x54!=&_0) a.el_append(&x54); else break; if (&x55!=&_0) a.el_append(&x55); else break; if (&x56!=&_0) a.el_append(&x56); else break; if (&x57!=&_0) a.el_append(&x57); else break; if (&x58!=&_0) a.el_append(&x58); else break; if (&x59!=&_0) a.el_append(&x59); else break; if (&x60!=&_0) a.el_append(&x60); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}
void paramline::_list_hx_set_u ( unity& hdest, s_long fk, _rcu x1, _rcu x2, _rcu x3, _rcu x4, _rcu x5, _rcu x6, _rcu x7, _rcu x8, _rcu x9, _rcu x10, _rcu x11, _rcu x12, _rcu x13, _rcu x14, _rcu x15, _rcu x16, _rcu x17, _rcu x18, _rcu x19, _rcu x20, _rcu x21, _rcu x22, _rcu x23, _rcu x24, _rcu x25, _rcu x26, _rcu x27, _rcu x28, _rcu x29, _rcu x30, _rcu x31, _rcu x32, _rcu x33, _rcu x34, _rcu x35, _rcu x36, _rcu x37, _rcu x38, _rcu x39, _rcu x40, _rcu x41, _rcu x42, _rcu x43, _rcu x44, _rcu x45, _rcu x46, _rcu x47, _rcu x48, _rcu x49, _rcu x50, _rcu x51, _rcu x52, _rcu x53, _rcu x54, _rcu x55, _rcu x56, _rcu x57, _rcu x58, _rcu x59, _rcu x60 )
{
  vec2_t<const unity*, __vecm_tu_selector> a; hdest.hash_clear(true); hdest.hashFlags_set(-1, fk);
  do { if (&x1!=&_0) a.el_append(&x1); else break; if (&x2!=&_0) a.el_append(&x2); else break; if (&x3!=&_0) a.el_append(&x3); else break; if (&x4!=&_0) a.el_append(&x4); else break; if (&x5!=&_0) a.el_append(&x5); else break; if (&x6!=&_0) a.el_append(&x6); else break; if (&x7!=&_0) a.el_append(&x7); else break; if (&x8!=&_0) a.el_append(&x8); else break; if (&x9!=&_0) a.el_append(&x9); else break; if (&x10!=&_0) a.el_append(&x10); else break; if (&x11!=&_0) a.el_append(&x11); else break; if (&x12!=&_0) a.el_append(&x12); else break; if (&x13!=&_0) a.el_append(&x13); else break; if (&x14!=&_0) a.el_append(&x14); else break; if (&x15!=&_0) a.el_append(&x15); else break; if (&x16!=&_0) a.el_append(&x16); else break; if (&x17!=&_0) a.el_append(&x17); else break; if (&x18!=&_0) a.el_append(&x18); else break; if (&x19!=&_0) a.el_append(&x19); else break; if (&x20!=&_0) a.el_append(&x20); else break; if (&x21!=&_0) a.el_append(&x21); else break; if (&x22!=&_0) a.el_append(&x22); else break; if (&x23!=&_0) a.el_append(&x23); else break; if (&x24!=&_0) a.el_append(&x24); else break; if (&x25!=&_0) a.el_append(&x25); else break; if (&x26!=&_0) a.el_append(&x26); else break; if (&x27!=&_0) a.el_append(&x27); else break; if (&x28!=&_0) a.el_append(&x28); else break; if (&x29!=&_0) a.el_append(&x29); else break; if (&x30!=&_0) a.el_append(&x30); else break; if (&x31!=&_0) a.el_append(&x31); else break; if (&x32!=&_0) a.el_append(&x32); else break; if (&x33!=&_0) a.el_append(&x33); else break; if (&x34!=&_0) a.el_append(&x34); else break; if (&x35!=&_0) a.el_append(&x35); else break; if (&x36!=&_0) a.el_append(&x36); else break; if (&x37!=&_0) a.el_append(&x37); else break; if (&x38!=&_0) a.el_append(&x38); else break; if (&x39!=&_0) a.el_append(&x39); else break; if (&x40!=&_0) a.el_append(&x40); else break; if (&x41!=&_0) a.el_append(&x41); else break; if (&x42!=&_0) a.el_append(&x42); else break; if (&x43!=&_0) a.el_append(&x43); else break; if (&x44!=&_0) a.el_append(&x44); else break; if (&x45!=&_0) a.el_append(&x45); else break; if (&x46!=&_0) a.el_append(&x46); else break; if (&x47!=&_0) a.el_append(&x47); else break; if (&x48!=&_0) a.el_append(&x48); else break; if (&x49!=&_0) a.el_append(&x49); else break; if (&x50!=&_0) a.el_append(&x50); else break; if (&x51!=&_0) a.el_append(&x51); else break; if (&x52!=&_0) a.el_append(&x52); else break; if (&x53!=&_0) a.el_append(&x53); else break; if (&x54!=&_0) a.el_append(&x54); else break; if (&x55!=&_0) a.el_append(&x55); else break; if (&x56!=&_0) a.el_append(&x56); else break; if (&x57!=&_0) a.el_append(&x57); else break; if (&x58!=&_0) a.el_append(&x58); else break; if (&x59!=&_0) a.el_append(&x59); else break; if (&x60!=&_0) a.el_append(&x60); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}

arrayref_t<wchar_t> paramline::x_pterm2_dflt()
{
  static arrayref_t<wchar_t> x(wCRLF);
  return x;
}
  // ff_encdec:
  //    pled__ar_elem,
  //    _pled__mask__encode: pled_enc_empty_key_as_bse, pled_encdec1v_for_names
void paramline::x_encode1n(const unity& x, std::wstring& retval, s_long ff_encdec, const arrayref_t<wchar_t>& pterm2)
{
  const bool b_ar_elem = !!(ff_encdec & pled__ar_elem);
  const bool b_bse = !!(ff_encdec & pled_enc_empty_key_as_bse);
  const bool b_1v_for_names = !!(ff_encdec & pled_encdec1v_for_names);
  if (b_1v_for_names)
  {
    hashx<const unity*, s_long> hstop;
    x_encode1v(x, retval, pled_encdec1v_for_names | (b_bse ? pled_enc_empty_key_as_bse : _pled__nullflags), 0, &hstop, pterm2);
    return;
  }
  retval.clear();
  if (x.isEmpty())
  {
    if (b_bse) { retval = L"\\e"; }
    return;
  }
  if (x.isString())
  {
    x_repl_e1(x.cref<utString>().ref(), retval, !b_ar_elem, b_ar_elem, false, pterm2);
    return;
  }
  x_repl_e1(_vstr_scalar(x), retval, !b_ar_elem, b_ar_elem, false, pterm2);
}

  // _x0 - a) if value - encoded as is.
  //    b) if utObject/unity - recursively dereferenced and then encoded.
  //    (the below description uses "_x0" in sense of "dereferenced _x0".
  // ret_psubarrs (optional):
  //    if _x0 is array with subarrays, all subarrays are encoded recursively and appended to *ret_psubarrs.
  //    *ret_psubarrs is not cleared by x_encode1v, only appended.
  // ff_encdec:
  //  0x1000 (pled__enc_assoc_as_empty): encode assoc. array as empty value
  //    (for use in x_encode_branch, meaning: ignore branches)
  //  0x2000 (pled__ar_elem): _x0 is an element of encoded array
  //  0x10000 (pled__enc1v_for_array_area):
  //      a) (main case) _x0 is an array, encoded by x_encode_branch in the form of array area.
  //        The array area path is constructed by the client. Values, returned by x_encode1v,
  //        should be appended to the path spec., e.g.
  //            =|<path elem.>|<etc.>|\z<array shape>; ..=<retval of x_encode1v>; [<(*ret_psubarrs)[i]>;]
  //          retval of x_encode1v is a list of top-level array elements, separated by "|".
  //            The list is expected to be assigned into pre-shaped array, starting from the current index.
  //            The array shape is expected to be encoded by the client.
  //        *ret_psubarrs will be list of
  //            <index in top-level array>=<encoded lower level array>
  //          (The index is not prepended with \z, as when pled__enc1v_for_array_area is unset.)
  //      b) (formal case, not used) if _x0 is not an array, it is encoded in usual way.
  //        The above array area example may also work in this case,
  //        because ..=<retval of x_encode1v> would simply add 1 element (string repr. of _x0) to top-level array.
  //  0x100 (pled_encdec1v_for_names): if _x0 is utString, always escape equality sign character '='.
  //    (For encoding keys in same fashion as values.)
  //  0x200 (pled_enc_empty_key_as_bse): if x is utEmpty, force encoding it as "\e".
  //
  // Returns:
  //  retval, *ret_psubarrs.
  //  The resulting strings must be processed with x_replace2a, to get final strings for external client.
void paramline::x_encode1v(const unity& _x0, std::wstring& retval, s_long ff_encdec, vec2_t<std::wstring, __vecm_tu_selector>* ret_psubarrs, hashx<const unity*, s_long>* phstop, const arrayref_t<wchar_t>& pterm2)
{
  const bool b_ar_elem = !!(ff_encdec & pled__ar_elem);
  const bool b_enc_assoc_as_empty = !!(ff_encdec & pled__enc_assoc_as_empty);
  const bool b_enc_name = !!(ff_encdec & pled_encdec1v_for_names);
  const bool b_enc_name_allow_array = false;
  const bool b_bse = !!(ff_encdec & pled_enc_empty_key_as_bse);
  const bool b_enc_for_array_area = !!(ff_encdec & pled__enc1v_for_array_area);
    ff_encdec &= ~pled__enc1v_for_array_area;
  retval.clear();

  unity _empty;
  const unity& x = _paramline_branch::deref_pu_once(_x0, _empty, phstop);

  if (x.isArray() && !(b_enc_name && !b_enc_name_allow_array))
  {
    const bool b_uar = x.utype() == utUnityArray;
    if (ret_psubarrs && b_uar && !b_enc_name && x.arrsz() > 0) // full processing with possibly nested arrays (the resulting string will keep all data)
    {
      std::wstring vlist;
      bool b_vals_empty = true;
      for(s_long i = 0; i < x.arrsz(); ++i)
      {
        std::wstring sv;
        vlist += L'|';
        const s_long ind = i + x.arrlb();
        const unity& rxelem = _paramline_branch::deref_pu_once(x[ind], _empty, phstop);
        if (rxelem.isArray())
        {
          s_long ff = 0;
            if (b_enc_assoc_as_empty) { ff |= pled__enc_assoc_as_empty; }
          vec2_t<std::wstring, __vecm_tu_selector> xsubarrs;
          x_encode1v(rxelem, sv, ff, &xsubarrs, phstop, pterm2);
          std::wstring prefix;
            if (!b_enc_for_array_area) { prefix += L"\\z"; }
            prefix += itows(ind, true);
            prefix += weterm_short;

          // Contents of the array x[ind] are pushed into ret_psubarrs, and will be joined at the topmost call level.
          ret_psubarrs->push_back(prefix + sv);
          for (s_long j = 0; j < xsubarrs.n(); ++j)
            { ret_psubarrs->push_back(prefix + xsubarrs[j]); }
        }
        else
        {
          if (!rxelem.isEmpty()) { b_vals_empty = false; }
          s_long ff = 0;
            if (1) { ff |= pled__ar_elem; }
            if (b_enc_assoc_as_empty) { ff |= pled__enc_assoc_as_empty; }
          x_encode1v(rxelem, sv, ff, 0, phstop, pterm2);
          vlist += sv;
        }
      }

      if (b_vals_empty && (b_enc_for_array_area || x.arrsz() > 10))
      {
        if (b_enc_for_array_area)
        {
          retval += L"\\zn"; retval += itows(x.arrsz(), true);
        }
        else
        {
          retval += L"\\z";
          retval += L"b"; retval += itows(x.arrlb(), true);
          retval += L"n0++=\\zn"; retval += itows(x.arrsz(), true);
        }
        return;
      }
      if (b_enc_for_array_area || x.arrlb() == 1)
      {
        retval.swap(vlist);
        return;
      }

      retval += L"\\z";
      retval += L"b"; retval += itows(x.arrlb(), true);
      retval += L"n0++=";
      retval += vlist;
      return;
    }

    if (b_enc_for_array_area && x.arrsz() == 0)
    {
      retval += L"\\z";
      return;
    }
    if (b_ar_elem || b_enc_name || x.arrsz() == 0)
    {
      retval += L"\\z";
      if (!(x.arrlb() == 1 && x.arrsz() == 0))
      {
        retval += L"b"; retval += itows(x.arrlb(), true);
        retval += L"n"; retval += itows(x.arrsz(), true);
      }
      return;
    }
    if (x.arrlb() != 1 && !b_enc_for_array_area)
    {
      retval += L"\\z";
      retval += L"b"; retval += itows(x.arrlb(), true);
      retval += L"n0++=";
    }
    s_long ff = 0;
      if (1) { ff |= pled__ar_elem; }
      if (b_enc_assoc_as_empty) { ff |= pled__enc_assoc_as_empty; }
    for(s_long i = 0; i < x.arrsz(); ++i)
    {
      const s_long ind = i + x.arrlb();
      std::wstring sv;
      x_encode1v(b_uar ? x[ind] : x.val<utUnity>(ind), sv, ff, 0, phstop, pterm2);
      retval += L'|';
      retval += sv;
    }
    return;
  }

  switch(x.utype())
  {
    case utEmpty:
    {
      if (b_ar_elem || (b_enc_name && !b_bse)) { retval = L""; }
        else { retval = L"\\e"; }
      break;
    }
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
        x_repl_e1(r_s.ref(), retval, false, b_ar_elem, b_enc_name, pterm2);
//                  std::wstring s = _trim(retval, L" ");
//                  if (s.length() == 0) { goto lInsertStringPrefix; }
//                  if (wstring_like(s.substr(0,1), L"[0123456789.+-]")) { goto lInsertStringPrefix; }
//                  if (lcase(s) == L"true") { goto lInsertStringPrefix; }
//                  if (lcase(s) == L"false") { goto lInsertStringPrefix; }
        _t_wz pos1 = retval.find_first_not_of(' '); if (pos1 == nposw) { goto lInsertStringPrefix; }
        wchar_t c = retval[pos1]; if ((c >= L'0' && c <= L'9') || c == L'.' || c == L'+' || c == L'-') { goto lInsertStringPrefix; }
        if (_eq_wcs_ascii_nocase(&retval[pos1], L"true") || _eq_wcs_ascii_nocase(&retval[pos1], L"false")) { goto lInsertStringPrefix; }
        break;
      }
lInsertStringPrefix:
      retval.insert(0, L"\\s");
      break;
    case utObject:
      {
        s_long ff = 0;
          if (b_ar_elem) { ff |= pled__ar_elem; }
        x_encode1v(_vstr_scalar(x), retval, ff, 0, phstop, pterm2);
        break;
      }
    default: // unknown type, maybe assoc. array
      if (x.isAssoc() && b_enc_assoc_as_empty)
        { x_encode1v(unity(), retval, ff_encdec, 0, phstop, pterm2); }
      else
      {
        s_long ff = 0;
          if (b_ar_elem) { ff |= pled__ar_elem; }
        x_encode1v(_vstr_scalar(x), retval, ff, 0, phstop, pterm2);
      }
      break;
  }
}

void paramline::x_repl_e1(const arrayref_t<wchar_t>& s1, std::wstring& s2, bool b_name, bool b_ar_elem, bool b_esc_eqsign, const arrayref_t<wchar_t>& pterm2)
{
  if (s1.is_empty())
    { return; }
  s_ll ps = 0, n = s1.n();
  s2.clear();
    if (n > 40)
    {
      s_ll n2 = n + 10 + (n >> 4);
      if (n2 > n)
      {
        size_t z = size_t(n2);
        if (z > s2.max_size()) { z = s2.max_size(); }
        s2.reserve(z);
      }
    }
  const wchar_t cterm2_0 = pterm2[0];
  while (ps < n)
  {
    wchar_t c0 = s1[ps];
    if (c0 == cterm2_0 && pterm2.is_eq(s1.range_intersect(ps, ps + pterm2.n()))) // "c0 == cterm2_0" - just an optimization
      { s2 += L"`b~"; ps += pterm2.n(); continue; }
    switch (c0)
    {
      case L'\\':
      {
        ++ps;
        if (ps >= n) { s2 += c0; continue; }

        wchar_t c = s1[ps];
        if (c == cterm2_0 && pterm2.is_eq(s1.range_intersect(ps, ps + pterm2.n()))) // "c == cterm2_0" - just an optimization
          { s2 += L"`b`b`b~"; ps += pterm2.n(); continue; }
        switch (c)
        {
          case L'\\': s2 += L"`b`b`b`b"; ++ps; continue;
          case L' ': case L';': case L'=': case L'~': case L'|': s2 += L"`b`b"; continue;
          default:
            if (ps == 1) { if (b_name) { s2 += L"`b"; } else { s2 += L"`b`b"; } }
              else { s2 += c0; }
            continue;
        }
      }
      case L';': s2 += L"`b`,"; ++ps; continue;
      case L'=': if (b_name || b_esc_eqsign) { s2 += L"`b`e"; } else { s2 += c0; } ++ps; continue;
      case L' ': if (ps == 0) { s2 += L"`b"; } s2 += L' '; ++ps; continue;
      case L'|':
      {
        if (ps == 0) { if (b_name) { s2 += c0; } else { s2 += L"`b`v"; } }
          else { if (b_ar_elem) { s2 += L"`b`v"; } else { s2 += c0; } }
        ++ps; continue;
      }
      case L'`': s2 += L"`a"; ++ps; continue;
      default: s2 += c0; ++ps; continue;
    }
  }
  if (s2.size() > 0)
  {
    size_t i_back = s2.size() - 1;
    wchar_t c0 = s2[i_back];
    if (c0 == L' ' || c0 == L'\\')
      { s2[i_back] = L'\\'; s2 += c0; }
  }
}

void paramline::x_replace2a(std::wstring& s, s_long replflags, const arrayref_t<wchar_t>& pterm2)
{
  if (replflags & plrf_has_esc_seq)
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
          case L'n': { for (s_ll i = 0; i < pterm2.n(); ++i) { s[pd++] = pterm2[i]; } ++ps; continue; }
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

void paramline::x_replace4(arrayref_t<wchar_t> s1, std::wstring& s2, s_long& replflags)
{
  _t_wz n = _t_wz(s1.n()), n2 = 0, nmax = _t_wz(max_swstr() - 2), ps = 0, pd = 0;
  while (ps < n)
  {
    wchar_t c = s1[ps++];
    if (c == L'`') { if (n2 > nmax) { throw XUExec("x_replace4.1"); } n2 += 2; replflags |= plrf_has_esc_seq; continue; }
    if (c == L'\\')
    {
      switch (s1[ps])
      {
        case L'\\':  case L';':  case L'~':  case L' ':  case L'=':  case L'|':
          if (n2 > nmax) { throw XUExec("x_replace4.2"); } n2 += 2; ++ps; replflags |= plrf_has_esc_seq; continue;
        default: break;
      }
    }
    n2 += 1; if (n2 > nmax) { throw XUExec("x_replace4.3"); }
  }
  if ((replflags & plrf_has_esc_seq) == 0) { if (s1.n() > 0) { s2.assign(s1.pd(), _t_wz(s1.n())); } else { s2.clear(); } return; }
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

  // NOTE sv must contain string, already passed
  //    through x_replace4 and _trim(, L" ")
  // replflags: ORed
  //    plrf_has_esc_seq:
  //      flag must be set by x_replace4 call that generated ssv.
  //    plrf_value_exists:
  //      means vdest referencing the existing value (which may be, in part., array to be updated)
  //    plrf_allow_modify_all:
  //      means
  //        a) create new value,
  //        b) overwrite the current value if it exists,
  //        c) update the current array if sv is array spec.
  //      If this flag is not set, only
  //        1) creating new values and
  //        2) updating existing arrays with array spec.
  //        is allowed.
  //    plrf_use_path_array_modifiers:
  //      means that vdest is path array (for decode_tree),
  //      so if ssv specifies an array in the form "|<elem>|<elem>...",
  //      for all elements in the form "\z<spec.>",
  //      return the corresponding utObject _paramline_array_spec
  //      instead of actual array.
  //      NOTE With plrf_use_path_array_modifiers, must have vdest.isLocal() == true.
  // vdest may reference already existing value or be empty.
  //    According to replflags, vdest may be
  //      a) kept,
  //      b) cleared (if plrf_value_exists is unset) and assigned a new value,
  //      c) modified (if contains an array and plrf_value_exists is set, and sv specifies some array updates).

void paramline::x_decode1v(const arrayref_t<wchar_t>& ssv, bool v_ar_elem, s_long replflags, unity& vdest, const arrayref_t<wchar_t>& pterm2)
{
  typedef _paramline_array_spec t_asp;
  arrayref_t<wchar_t> part;
  try
  {
    unity _sv_dflt;
    const bool b_v_ex = !!(replflags & plrf_value_exists);
    const bool b_mod_all = !!(replflags & plrf_allow_modify_all);
    const bool b_use_pam = !!(replflags & plrf_use_path_array_modifiers);

    if (ssv.is_empty() || ssv[0] == L'\0')
    {
      if (b_v_ex && !b_mod_all) { return; }
      vdest.clear();
      return;
    }

    wchar_t char0 = ssv[0];

    if (char0 == L'\\')
    {
      wchar_t char1 = ssv.n() >= 2 ? ssv[1] : 0;
      if (char1 == L'z')
      {
          // Check read-only condition.
        if (b_v_ex && !b_mod_all && vdest.utype() != utUnityArray)
          { return; }

          // Parse array spec.
        _paramline_array_spec asp;
          if (!asp._init_from_string(ssv.range_intersect(2))) { goto lLogicalDecodeError; }

          // Grant vdest being an array of correct type.
        bool _b_assign_empty_ar = !(asp.b_has_base || asp.b_has_n || asp.b_has_op());
        if (_b_assign_empty_ar || !b_v_ex || vdest.utype() != utUnityArray)
          { vdest.arr_init<utUnity>(1); }
        if (_b_assign_empty_ar)
          { return; }

          // Assign base index and size.
        if (asp.b_has_base)
        {
          if (!asp.b_valid_ibase32()) { goto lLogicalDecodeError; }
          vdest.arrlb_(s_long(asp.ibase & 0xffffffffll));
        }
        if (asp.b_has_n)
        {
          if (!asp.b_valid_n_elem32()) { goto lLogicalDecodeError; }
          vdest.arrsz_(s_long(asp.n_elem & 0x7fffffffll));
        }

          // Set or append values if any.
        if (asp.b_has_op())
        {
          if (asp.op_type == t_asp::op_append)
          {
            s_long replf2 = replflags & ~plrf_value_exists;
            unity u;
              paramline::x_decode1v(_trim_arrayref(asp.rvalue, L" "), v_ar_elem, replf2, u, pterm2);
            if (asp.b_op_append_vec && u.utype() == utUnityArray)
            {
              s_long j = vdest.arrub() + 1;
              s_ll n2 = s_ll(vdest.arrsz()) + u.arrsz();
              if (n2 > 0x7fffffffll) { goto lLogicalDecodeError; }
              vdest.uaS_set(s_long(n2));
              if (vdest.isLocal())
              {
                for (s_long i = u.arrlb(); i <= u.arrub(); ++i)
                  { vdest[j].swap(u[i]); ++j; }
              }
              else
              {
                for (s_long i = u.arrlb(); i <= u.arrub(); ++i)
                  { vdest[j] = u[i]; ++j; }
              }
            }
            else
            {
              if (vdest.isLocal())
              {
                vdest.ua_append(unity());
                vdest.ua_last().swap(u);
              }
              else { vdest.ua_append(u); }
            }
          }
          else if (asp.op_type == t_asp::op_assign_at_index)
          {
            if (!asp.b_valid_iassign32()) { goto lLogicalDecodeError; }
            s_long ind = s_long(asp.iassign);
            s_long replf2 = replflags & ~plrf_value_exists;
              if (vdest.u_has(ind, 1)) { replf2 |= plrf_value_exists; replf2 |= plrf_allow_modify_all; }
            paramline::x_decode1v(_trim_arrayref(asp.rvalue, L" "), v_ar_elem, replf2, vdest.ua(ind), pterm2);
          }
        }

        return;
      }

      if (b_v_ex && !b_mod_all) { return; }

      if (char1 == L'e') { if (_trim_arrayref(ssv.range_intersect(2), L" ").length()>0) { goto lLogicalDecodeError; } vdest.clear(); }
      else if (char1 == L's')
      {
        std::wstring s;
        s.assign(&ssv[2], ssv._end_u());
        x_replace2a(s, replflags, pterm2);
        if (vdest.isLocal()) { vdest.clear(); vdest.rx<utString>().swap(s); }
          else { vdest = s; }
      }
      else if (char1 == L'i') { part = ssv.range_intersect(2); if (x_incorrect_integer_value_str(part, false)) { goto lLogicalDecodeError; } vdest = str2i(part); }
      else if (char1 == L'f') { part = ssv.range_intersect(2); if (x_incorrect_numeric_value_str(part, true)) { goto lLogicalDecodeError; } vdest = str2f(part, 0, true, true); }
      else if (char1 == L'd') { part = ssv.range_intersect(2); try { vdest = unity(part).val<utDate>(); } catch (...) { goto lLogicalDecodeError; } }
      else if (char1 == L'0') { if (_trim_arrayref(ssv.range_intersect(2), L" ").length()>0) { goto lLogicalDecodeError; } vdest = false; }
      else if (char1 == L'1') { if (_trim_arrayref(ssv.range_intersect(2), L" ").length()>0) { goto lLogicalDecodeError; } vdest = true; }
      else
      {
        if (ssv.n() <= 2) { vdest.clear(); return; }
        _sv_dflt = ssv.range_intersect(2);
        part = _trim_arrayref(ssv.range_intersect(2), L" ");
        goto lAutoDetectType;
      }
      return;
    }

    if (b_v_ex && !b_mod_all) { return; }

    if (char0 == L'|') // == wvterm
    {
      if (v_ar_elem) { goto lLogicalDecodeError; }
      unity ar2 = split(ssv, wvterm);
      vdest.arr_init<utUnity>(1);
      vdest.arrub_(ar2.arrub());
      for(s_long ind = 1; ind <= ar2.arrub(); ++ind)
      {
        unity& rx = vdest[ind];
        arrayref_t<wchar_t> ssv2 = _trim_arrayref(arrayref_t<wchar_t>(ar2.rstr(ind)), L" ");
        if (b_use_pam && ssv2.range_intersect(0, 2) == L"\\z")
        {
          rx.objt<t_asp>(0)();
          t_asp& asp = rx.objRef<t_asp>();
          if (!asp._init_from_string(ssv2.range_intersect(2))) { goto lLogicalDecodeError; }
        }
        else
        {
          x_decode1v(ssv2, true, replflags, rx, pterm2);
        }
      }
      return;
    }

    part = ssv;

lAutoDetectType:

    if (x_decode1v_auto_date(part, vdest)) { return; }
    if (!x_incorrect_integer_value_str(part, false)) { vdest = str2i(part); return; }
    if (!x_incorrect_numeric_value_str(part, false)) { vdest = str2f(part); return; }
    if (part == L"true") { vdest = true; return; }
    if (part == L"false") { vdest = false; return; }
    if (vdest.isLocal())
    {
      if (_sv_dflt.isNonempty()) { vdest.swap(_sv_dflt); }
        else { vdest = ssv; }
      x_replace2a(vdest.rstr(), replflags, pterm2);
    }
    else
    {
      std::wstring s;
      if (_sv_dflt.isNonempty()) { s.swap(_sv_dflt.rstr()); }
        else { s.assign(ssv.pd(), ssv._end_u()); }
      x_replace2a(s, replflags, pterm2);
      vdest = s;
    }
    return;
  }
  catch(std::exception& e) { throw XUExec("x_decode1v.1", e.what(), ssv); }
  catch(...) { throw XUExec("x_decode1v.2", ssv); }
lLogicalDecodeError:
  throw XUExec("x_decode1v.3", ssv);
  return;
}

bool paramline::x_decode1v_auto_date(const arrayref_t<wchar_t>& s_, unity& retval) __bmdx_noex
{
  try { _unitydate d; if (!_static_conv::conv_String_Date0(s_, d, true)) { return false; } retval = d; return true; }
    catch(...) { return false; }
}

bool _str_has_any_not_of(const arrayref_t<wchar_t>& s_, const arrayref_t<wchar_t>& chars_)
{
  if (s_.is_empty()) { return false; }
  if (chars_.is_empty()) { return true; }
  for (s_ll i = 0; i < s_.n(); ++i) { if (chars_.find1(s_[i]) >= chars_.n()) { return true; } }
  return false;
}

bool paramline::x_incorrect_numeric_value_str(const arrayref_t<wchar_t>& s_, bool b_nans)
{
  if (_str_has_any_not_of(s_, L"0123456789.Eeinfa +-")) { return true; }
  try { (void)str2f(s_, 0., false, b_nans); return false; } catch(...) { return true; }
}
bool paramline::x_incorrect_integer_value_str(const arrayref_t<wchar_t>& s_, bool allow_float_str)
{
  if (allow_float_str)
  {
    if (_str_has_any_not_of(s_, L"0123456789.Eeinfa +-")) { return true; }
    try { (void)str2i(s_, 0, false); double y = str2f(s_, 0., false, false); return std::floor(y) != y; } catch(...) { return true; }
  }
  else
  {
    if (_str_has_any_not_of(s_, L"0123456789 +-")) { return true; }
    try { (void)str2i(s_, 0, false); return false; } catch(...) { return true; }
  }
}

unity& paramline::decode(arrayref_t<wchar_t> ssrc0, unity& mh, s_long flags_encdec, arrayref_t<wchar_t> pterm2)
  { x_decode(ssrc0, mh, flags_encdec & _pled__mask__decode, pterm2, 0, 0); return mh; }
unity paramline::decode(arrayref_t<wchar_t> ssrc, s_long flags_encdec, arrayref_t<wchar_t> pterm2)
  { unity x; decode(ssrc, x, flags_encdec, pterm2); return x; }
unity paramline::decode1v(arrayref_t<wchar_t> ssrc)
  { unity x; decode1v(ssrc, x); return x; }
unity paramline::decode_tree(arrayref_t<wchar_t> ssrc, s_long flags_encdec, arrayref_t<wchar_t> pterm2)
  { unity x; decode_tree(ssrc, x, flags_encdec, pterm2); return x; }
std::wstring paramline::encode(const unity& mhsrc, s_long flags_encdec, arrayref_t<wchar_t> pterm)
  { std::wstring s; encode(mhsrc, s, flags_encdec, pterm); return s; }
std::wstring paramline::encode1v(const unity& value)
  { std::wstring s; encode1v(value, s); return s; }
std::wstring paramline::encode_tree(const unity& mhsrc, s_long flags_encdec, arrayref_t<wchar_t> pterm, arrayref_t<wchar_t> pterm2)
  { std::wstring s; encode_tree(mhsrc, s, flags_encdec, pterm, pterm2); return s; }


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
        return wsToBsUtf8(ps, n);
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
        return bsUtf8ToWs(ps, n);
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
  s_long wscompare(arrayref_t<wchar_t> s1, arrayref_t<wchar_t> s2, bool ignore_case, s_long loc_type)
  {
    if (!(loc_type >= 0 && loc_type <= 2)) { throw XUExec("wscompare.1");  }
    s_ll minlen = s1.n(); if (s2.n() < minlen) { minlen = s2.n(); }
    if (ignore_case)
    {
      if (loc_type == 0)
      {
        int cmp1 = __wcsncasecmp_curr(s1.pd(), s2.pd(), minlen);
        if (cmp1 != 0) { return cmp1 > 0 ? 1 : -1; }
        if (s1.size() != s2.size()) { return s1.size() < s2.size() ? -1 : 1; }
        return 0;
      }
      else if (loc_type == 1) // "user" loc. (system setting)
      {
        s_long n = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE|SORT_STRINGSORT, s1.pd(), int(bmdx_minmax::myllmin(std::numeric_limits<int>::max(), s1.length())), s2.pd(), int(bmdx_minmax::myllmin(std::numeric_limits<int>::max(), s2.length())));
        if (n == 0) { throw XUExec("wscompare.2"); }
        return n - 2;
      }
      else // loc_type == 2, C loc.
      {
        s_long n = CompareStringW(LOCALE_NEUTRAL, NORM_IGNORECASE|SORT_STRINGSORT, s1.pd(), int(bmdx_minmax::myllmin(std::numeric_limits<int>::max(), s1.length())), s2.pd(), int(bmdx_minmax::myllmin(std::numeric_limits<int>::max(), s2.length())));
        if (n == 0) { throw XUExec("wscompare.3"); }
        return n - 2;
      }
    }
    else { int res = wcsncmp(s1.pd(), s2.pd(), size_t(minlen)); if (res != 0) { return res > 0 ? 1 : -1; } if (s1.n() == s2.n()) { return 0; } return s1.n() < s2.n() ? -1 : 1; }
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

    bool __cl2argv(const wchar_t* cl, std::vector<std::wstring>& retv) __bmdx_noex
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

  std::string cmd_arg1(const arrayref_t<char>& s, bool b_shell)
    { return processctl::ff_mc().arg1(s, b_shell); }
  std::string cmd_arg1(const arrayref_t<wchar_t>& s, bool b_shell)
    { return processctl::ff_mc().arg1(wsToBs(s), b_shell); }

}

#endif // _bmdxpl_Wnds

#ifdef _bmdxpl_Psx

  #ifdef __FreeBSD__
    #include <sys/sysctl.h>
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
  #elif defined(__ANDROID__) || defined(__EMSCRIPTEN__)
  #else
    __attribute__((section(".init_array"))) void (* p___read_argv)(int, char*[], char*[]) = &__read_argv;
  #endif
#endif


#if __bmdx_use_wcrtomb_l
  static int __wctomb(char* ps, wchar_t c, mbstate_t* q) { return int(wcrtomb_l(ps, c, q, phloc(1)->h)); } // NOTE the caller must have ensure_loc(1) == true
  static int __mbtowc(wchar_t* pc, const char* ps, size_t n, mbstate_t* q) { return int(mbrtowc_l(pc, ps, n, q, phloc(1)->h)); } // -"-
#else
  static int __wctomb(char* ps, wchar_t c, mbstate_t* q) { return int(wcrtomb(ps, c, q)); }
  static int __mbtowc(wchar_t* pc, const char* ps, size_t n, mbstate_t* q) { return int(mbrtowc(pc, ps, n, q)); }
#endif

namespace bmdx
{
    namespace
    {
      static bool _test_mbrtowc() { mbstate_t q; memset(&q, 0, sizeof(q)); char cc[MB_LEN_MAX+1]; size_t res; wchar_t c; cc[0] = 'A'; res = mbrtowc(&c, cc, 1, &q); if (res != 1 || c != L'A') { return false; } cc[0] = '1'; res = mbrtowc(&c, cc, 1, &q); if (res != 1 || c != L'1') { return false; } cc[0] = ';'; res = mbrtowc(&c, cc, 1, &q); if (res != 1 || c != L';') { return false; } cc[0] = ' '; res = mbrtowc(&c, cc, 1, &q); if (res != 1 || c != L' ') { return false; } return true; }
      static bool _test_wcrtomb() { mbstate_t q; memset(&q, 0, sizeof(q)); char cc[MB_LEN_MAX+1]; size_t res; res = wcrtomb(cc, L'A', &q); if (res != 1 || cc[0] != 'A') { return false; } res = wcrtomb(cc, L'1', &q); if (res != 1 || cc[0] != '1') { return false; } res = wcrtomb(cc, L';', &q); if (res != 1 || cc[0] != ';') { return false; } res = wcrtomb(cc, L' ', &q); if (res != 1 || cc[0] != ' ') { return false; } return true; }
    }
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) // n < 0: autodetect length based on null char.
    {
      try {
        if (!ps) { throw XUExec("_wsToBs.3"); }

        #if bmdx_wsbs_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif

        #if __bmdx_use_wcrtomb_l
          if (!ensure_loc(1)) { throw XUExec("_wsToBs.4"); }
        #endif

        mbstate_t q; memset(&q, 0, sizeof(q));

        if (!_mb) // this check is done once
        {
          t_lock_locale __lock(LC_CTYPE, __bmdx_setlocale_name, wsbs_lk_dt()); if (sizeof(__lock)) {}
          if (__wctomb(0, 0, &q)) {}
          _mb = 2 + (int)_test_wcrtomb();
        }
        if (_mb == 2) { return wsToBsUtf8(ps, n); }

        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }

        std::string s;
        meta::s_ll nrsv = n; s.reserve(_t_sz(nrsv));
        meta::s_ll pos(0);
        while (pos < n)
        {
          wchar_t c = ps[pos];
          if (c & ~0x7f) { break; }
          s += char(c);
          pos += 1;
        }
        if (pos >= n) { return s; }

        t_lock_locale __lock(LC_CTYPE, __bmdx_setlocale_name, wsbs_lk_dt()); if (sizeof(__lock)) {}
        char cc[MB_LEN_MAX+1];
        if (__wctomb(0, 0, &q)) {}
        for(; pos < n; ++pos)
        {
          if (ps[pos] == L'\0') { s += '\0'; }
            else { int cnt = __wctomb(cc, ps[pos], &q); if (cnt <= -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
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
        if (!ps) { throw XUExec("_bsToWs.3"); }

        #if bmdx_bsws_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif

        #if __bmdx_use_wcrtomb_l
          if (!ensure_loc(1)) { throw XUExec("_bsToWs.4"); }
        #endif

        mbstate_t q; memset(&q, 0, sizeof(q));

        if (!_mb) // this check is done once
        {
          t_lock_locale __lock(LC_CTYPE, __bmdx_setlocale_name, wsbs_lk_dt()); if (sizeof(__lock)) {}
          if (__mbtowc(0, 0, 0, &q)) {}
          _mb = 2 + (int)_test_mbrtowc();
        }
        if (_mb == 2) { return bsUtf8ToWs(ps, n); }

        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }

        std::wstring s;
        if (n <= 0) { return s; }
        meta::s_ll nrsv = n; s.reserve(_t_sz(nrsv));
        meta::s_ll pos(0);
        while (pos < n)
        {
          unsigned char c = ps[pos];
          if (c >= 128) { break; }
          s += wchar_t(c);
          pos += 1;
        }
        if (pos >= n) { return s; }

        t_lock_locale __lock(LC_CTYPE, __bmdx_setlocale_name, wsbs_lk_dt()); if (sizeof(__lock)) {}
        meta::s_ll rsvpos(0);
        if (__mbtowc(0, 0, 0, &q)) {}
        while (pos < n)
        {
          if (pos - rsvpos > 0xfff) { rsvpos = pos; meta::s_ll n2 = meta::s_ll(s.length()); if (n2 - nrsv > -200) { if (n2 > nrsv) { nrsv = n2; } nrsv += nrsv >> 2; s.reserve(_t_sz(nrsv)); } }
          if (ps[pos] == '\0') { s += L'\0'; pos += 1; }
          else
          {
            wchar_t c; int cnt = __mbtowc(&c, &ps[pos], size_t(n - pos), &q);
            if (cnt <= -1) { s += L'?'; pos += 1; }
              else if (cnt > 0) { s += c; pos += cnt; }
              else { s += L'\0'; pos += 1; }
          }
        }
        return s;
      }
      catch (_XUBase&) { throw; }
      catch (const std::exception& e) { throw XUExec("_bsToWs.1", e.what()); }
      catch (...) { throw XUExec("_bsToWs.2"); }
    }

      // wide character string comparison
    s_long wscompare(arrayref_t<wchar_t> s1, arrayref_t<wchar_t> s2, bool ignore_case, s_long loc_type)
    {
      s_ll minlen = s1.n(); if (s2.n() < minlen) { minlen = s2.n(); }
      if (ignore_case)
      {
        int cmp1;
        if (loc_type == 0) { cmp1 = __wcsncasecmp_curr(s1.pd(), s2.pd(), minlen); }
        else
        {
          #if __bmdx_char_case_tables
          #elif __bmdx_use_locale_t
            if (!ensure_loc(loc_type)) { throw XUExec("wscompare.1");  }
          #endif
          cmp1 = __wcsncasecmp(s1.pd(), s2.pd(), minlen, loc_type);
        }
        if (cmp1 != 0) { return cmp1 > 0 ? 1 : -1; }
        if (s1.n() != s2.n()) { return s1.n() < s2.n() ? -1 : 1; }
        return 0;
      }
      else { int res = wcsncmp(s1.pd(), s2.pd(), size_t(minlen)); if (res != 0) { return res > 0 ? 1 : -1; } if (s1.n() == s2.n()) { return 0; } return s1.n() < s2.n() ? -1 : 1; }
    }

    struct _d_now_lks {};

    _unitydate d_now(bool allow_fracsec)
    {
      critsec_t<_d_now_lks> __lock(10, -1); if (sizeof(__lock)) {}
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
      static s_long init(0);
      static meta::s_ll sec0(0);
      static double ms0(0);
      meta::s_ll sec1 = meta::s_ll(std::time(0));
      double ms1 = clock_ms();
      critsec_t<_d_now_lks> __lock(10, -1); if (sizeof(__lock)) {}
      if (init != 0) { const double d = (ms1 - ms0) - (double(sec1 - sec0) * 1000); if (d > 1000 || d < -1000) { init = 0; } }
      if (init == 0) { sec0 = sec1; ms0 = ms1; init = 1; }
      sec1 = sec0 + s_ll((ms1 - ms0) / 1000);
      s_long n = s_long((((sec1 + 43200) / (86400 >> 1)) + (2440587 << 1) + 1) >> 1);
      s_long h = (sec1 / 3600) % 24;
      s_long m = (sec1 / 60) % 60;
      s_long s = sec1 % 60;
      _unitydate x(double(_jds_nhms(n, h, m, s)));
      if (allow_fracsec) { x.add_seconds(double(s_ll((ms1 - ms0) * 1000) % 1000000) * 1.e-6); }
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
      static bool initDone(false);
      #if __APPLE__ && __MACH__
        static char buf[(4*PATH_MAX)] = "_";
      #else
        static char buf[2048] = "_";
      #endif
      if (initDone) { return buf; }
      critsec_t<static_init_lks> __lock(10,-1); if (sizeof(__lock)) {}
      #if defined(__FreeBSD__)
        int mib[4]; mib[0] = CTL_KERN; mib[1] = KERN_PROC; mib[2] = KERN_PROC_PATHNAME; mib[3] = -1;
        size_t n = sizeof(buf);
        sysctl(mib, 4, buf, &n, 0, 0);
        buf[n] = '\0';
      #elif __APPLE__ && __MACH__
        typedef int (*Fpidpath)(int pid, void* buf, bmdx_meta::u_long bufsize);
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
        n2 = __bmdx_snprintf(buf, sizeof(buf), "pargs %i", int(getpid()));
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
          n2 = __bmdx_snprintf(buf, sizeof(buf), "argv[%i]: ", iarg);
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
              if (s2.find("'") != nposc) { s2 = replace_c(s2, "'", "'\\''"); }
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
              fd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
              if (fd == 0) { fd = open("/proc/curproc/cmdline", O_RDONLY | O_CLOEXEC); }
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
                  if (s2.find("'") != nposc) { s2 = replace_c(s2, "'", "'\\''"); }
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


//========================================================================
//== OS- or library-dependent code template

#ifdef _bmdxpl_OS_NAME_HERE
namespace bmdx
{
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) { } // n < 0: autodetect length based on null char.
    std::wstring _bsToWs(const char* ps, meta::s_ll n) { } // n < 0: autodetect length based on null char.
    s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case, s_long loc_type) { }
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

namespace bmdx
{
  struct file_utils::__utils_l2
  {
    static const int _access_f_ok = 0;
    int get_st_mode(const wchar_t* path)
    {
      if (0 != __bmdx_wnds_waccess(path, _access_f_ok)) { return 0; }
      struct _stat st;
      if (0 != __bmdx_wnds_wstat(path, &st)) { return 0; }
      return st.st_mode;
    }
    bool is_ex_dir_u(const wchar_t* path) { return !!file_io::_mask_match_mode(get_st_mode(path), S_IFDIR); }
    bool is_ex_file_u(const wchar_t* path) { return !!file_io::_mask_match_mode(get_st_mode(path), S_IFREG); }
  };

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

  bool file_utils::is_ex_file(const std::wstring& pathstr) const { return __utils_l2().is_ex_file_u(pathstr.c_str()); }
  bool file_utils::is_ex_dir(const std::wstring& pathstr) const { return __utils_l2().is_ex_dir_u(pathstr.c_str()); }

  bool file_utils::xmk_subdir(const std::wstring& pathstr, int level) const
  {
    std::wstring wp;

    if (level == 0)
    {
      wp = trim(this->join_path(this->complete_path(pathstr, pdCurDir)), wpathsep(), false, true);
      if (wp.length() == 0) { return false; }
    }
    else
    {
      wp = trim(pathstr, wpathsep(), false, true);
      if (wp.length() == 0) { return true; }
    }
    if (__utils_l2().is_ex_dir_u(wp.c_str())) { return true; }
    if (!xmk_subdir(this->strip_path(wp), level+1)) { return false; }

    return 0 == __bmdx_wnds_wmkdir(wp.c_str());
  }

  std::wstring file_utils::complete_path(const std::wstring& pathstr, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
  {
    if (is_full_path(pathstr)) { return pathstr; }
    if (xHasCurDirShortCut(pathstr)) { wchar_t buf[2048]; return join_path(__bmdx_wnds_wgetcwd(buf, sizeof(buf) / sizeof(wchar_t)), pathstr.substr(1)); }
    std::wstring s;
    switch(pd)
    {
      case pdCurDir: { wchar_t buf[2048]; s = join_path(__bmdx_wnds_wgetcwd(buf, sizeof(buf) / sizeof(wchar_t)), pathstr); break; }
      case pdThisAppDir: { s = join_path(strip_path(cmd_myexe()), pathstr); break; }
      case pdTempDir:
      {
        std::wstring sv;
        if (__bmdx_wnds_wgetenv(L"TEMP", sv) || __bmdx_wnds_wgetenv(L"TMP", sv))
          { s = join_path(sv, pathstr); }
        else { throw XUExec("file_utils::complete_path(wstring).1"); }
        if (!is_full_path(s)) { s = join_path(strip_path(cmd_myexe()), s); }
        break;
      }
      case pdDoNotChange: { s = pathstr; break; }
      default: //pdUserDefinedDir
        s = join_path(sUserDefDir,pathstr);
        if (!is_full_path(s)) { s = join_path(strip_path(cmd_myexe()), s); }
    }
    return s;
  }

  std::string file_utils::complete_path(const std::string& pathstr, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
    if (is_full_path(pathstr)) { return pathstr; }
    if (xHasCurDirShortCut(pathstr)) { char buf[2048]; return join_path(__bmdx_std_getcwd(buf, sizeof(buf)), pathstr.substr(1)); }
    std::string s;
    switch(pd)
    {
      case pdCurDir: { char buf[2048]; s = join_path(__bmdx_std_getcwd(buf,sizeof(buf)), pathstr); break; }
      case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), pathstr); break; }
      case pdTempDir:
      {
        std::string sv;
        if (__bmdx_std_getenv("TEMP", sv) || __bmdx_std_getenv("TMP", sv))
          { s = join_path(sv, pathstr); }
        else { throw XUExec("file_utils::complete_path(wstring).1"); }
        if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
        break;
      }
      case pdDoNotChange: { s = pathstr; break; }
      default: //pdUserDefinedDir
        s = join_path(sUserDefDir,pathstr);
        if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
    }
    return s;
  }

  bool file_utils::mk_subdir(const std::wstring& pathstr) const { return xmk_subdir(pathstr, 0); }
  int file_utils::remove_one(const std::wstring& pathstr, s_long flags)
  {
      if ((flags & 6) == 0) { return -1; }
      try {
        int m1 = __utils_l2().get_st_mode(pathstr.c_str());
          if (m1 == 0) { return 0; }
        const int fl_pair2_low = 0x4; // see _mask_match_mode call with S_IFDIR as arg. below
        int m2 = file_io::_mask_match_mode(m1, (flags & 0x2) ? S_IFREG : 0, (flags & 0x4) ? S_IFDIR : 0);
          if (!m2) { return -4; }
        int res = -1;
        if (m2 & fl_pair2_low) { res = __bmdx_wnds_wrmdir(pathstr.c_str()); }
          else { res = __bmdx_wnds_wremove(pathstr.c_str()); }
        if (res != 0) { return -3; }
        return 1;
      } catch (...) {}
      return -2;
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
          else if (pos2 > pos0) { std::wstring sv; if (__bmdx_wnds_wgetenv(s.substr(pos0, pos2 - pos0).c_str(), sv)) { s2 += sv; } pos0 = pos2 + 1; }
          else { s2 += L'%'; pos0 = pos2 + 1; }
      }
      else { s2 += s.substr(pos0, s.length() - pos0); break; }
    } while (pos0 < s.length());
    return s2;
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

  std::wstring file_utils::complete_path(const std::wstring& pathstr, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
    { return bsToWs(complete_path(wsToBs(pathstr), pd, wsToBs(sUserDefDir))); }

  std::string file_utils::complete_path(const std::string& pathstr, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
    if (is_full_path(pathstr)) { return pathstr; }
    if (xHasCurDirShortCut(pathstr)) { char buf[2048]; return join_path(__bmdx_std_getcwd(buf, sizeof(buf)), pathstr.substr(1)); }
    std::string s;
    switch(pd)
    {
        case pdCurDir: { char buf[2048]; s = join_path(__bmdx_std_getcwd(buf,sizeof(buf)), pathstr); break; }
          case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), pathstr); break; }
          case pdTempDir:
              // <Psx>
              s = join_path(getenv("TMPDIR"), pathstr);
              // </Psx>
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
              break;
          case pdDoNotChange: { s = pathstr; break; }
          default: //pdUserDefinedDir
              s = join_path(sUserDefDir,pathstr);
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
      }
      return s;
  }

  bool file_utils::is_ex_file(const std::wstring& pathstr) const { return file_io::is_ex_file(wsToBs(pathstr)); }
  bool file_utils::is_ex_dir(const std::wstring& pathstr) const { return file_io::is_ex_dir(wsToBs(pathstr)); }
  bool file_utils::mk_subdir(const std::wstring& pathstr) const { return xmk_subdir(wsToBs(pathstr), 0); }
  int file_utils::remove_one(const std::wstring& pathstr, s_long flags) { return file_io::remove_one(wsToBs(pathstr), flags & 0x6); }

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
          else { s2 += L'%'; pos0 = pos2 + 1; }
      }
      else { s2 += s.substr(pos0, s.length() - pos0); break; }
    } while (pos0 < s.length());
    return s2;
  }

}
#endif // _bmdxpl_Psx



//========================================================================

//== OS-independent part

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
    while(s.find(cpathsep2()) != nposc) { s = replace_c(s, cpathsep2(), cpathsep()); }
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

bool file_utils::is_ex_file(const std::string& pathstr) const
  { return file_io::is_ex_file(pathstr); }
bool file_utils::is_ex_dir(const std::string& pathstr) const
  { return file_io::is_ex_dir(pathstr); }

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
        else { s2 += '%'; pos0 = pos2 + 1; }
    }
    else { s2 += s.substr(pos0, s.length() - pos0); break; }
  } while (pos0 < s.length());
  return s2;
}

bool file_utils::xmk_subdir(const std::string& pathstr, int level) const
{
  std::string wp;

  if (level == 0)
  {
    wp = trim(this->join_path(this->complete_path(pathstr, pdCurDir)), cpathsep(), false, true);
    if (wp.length() == 0) { return false; }
  }
  else
  {
    wp = trim(pathstr, cpathsep(), false, true);
    if (wp.length() == 0) { return true; }
  }
  if (this->is_ex_dir(wp)) { return true; }
  if (!this->xmk_subdir(this->strip_path(wp), level+1)) { return false; }

  return 0 == __bmdx_std_mkdir(wp.c_str() __bmdx_std_mkdir_arg2);
}

bool file_utils::mk_subdir(const std::string& pathstr) const { return xmk_subdir(pathstr, 0); }

int file_utils::remove_one(const std::string& pathstr, s_long flags) { return file_io::remove_one(pathstr, flags & 0x6); }

unity file_utils::load_text(const std::string& format_string, const std::string& sPath, EFileUtilsPredefinedDir pd, unity& ret_s) const __bmdx_noex
    { try { return load_text(format_string, bsToWs(sPath), pd, ret_s); } catch (...) {} return unity(); }
unity file_utils::load_text(const std::string& format_string, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, unity& ret_s) const __bmdx_noex
{
try {
  typedef unsigned char u_char;

  enum EEncodings { local8bit, lsb8bit, utf8, utf16le, utf16be };
  bool is_binary(false); bool is_text(false); bool is_local8bit(false); bool is_lsb8bit(false); bool is_utf8(false); bool is_utf16le(false); bool is_utf16be(false);
  std::vector<int> encs;
  std::vector<std::string> args = splitToVector(format_string, " ");
  for (unsigned int i = 0; i < args.size(); ++i)
  {
    if (!is_binary && args[i] == "binary") is_binary = true;
    if (!is_text && args[i] == "text") is_text = true;
    if (!is_local8bit && args[i] == "local8bit") { is_local8bit = true; encs.push_back(local8bit); }
    if (!is_lsb8bit && args[i] == "lsb8bit") { is_lsb8bit = true; encs.push_back(lsb8bit); }
    if (!is_utf8 && args[i] == "utf8") { is_utf8 = true; encs.push_back(utf8); }
    if (!is_utf16le && args[i] == "utf16le") { is_utf16le = true; encs.push_back(utf16le); }
    if (!is_utf16be && args[i] == "utf16be") { is_utf16be = true; encs.push_back(utf16be); }
  }
  bool is_successful = false; std::wstring s0;

  while(true) // once
  {
    if (is_binary == is_text) break;
    if (encs.size() == 0) break;
    std::string sPath = wsToBsUtf8(complete_path(sPath0, pd));
    if (!file_io::is_ex_file(sPath, 1)) { break; }

    std::string s;
    if (file_io::load_bytes(sPath, s, 1) < 1) { break; }

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
        case utf8: { if (s.size() >= 3 && u_char(s[0]) == 0xef && u_char(s[1]) == 0xbb && u_char(s[2]) == 0xbf) { if (is_text) { pos = 3; } ind_enc = i; goto lExitFor1; } break; }
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

    case utf8:
      s0 = bsUtf8ToWs(&s[pos], s_ll(s.size() - pos));
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

} catch (...) { return unity(); }
}

bool file_utils::save_text(const std::string& format_string, const std::wstring& str, const std::string& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const __bmdx_noex
  { try { return save_text(format_string, str, bsToWs(sPath), pd, sUserDefDir); } catch (...) {} return false; }
bool file_utils::save_text(const std::string& format_string, const std::wstring& s0, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const __bmdx_noex
{
try {
  typedef unsigned char u_char;

  bool is_binary(false);
  bool is_text(false);
  bool is_truncate(false);
  bool is_append(false);
  bool is_local8bit(false);
  bool is_lsb8bit(false);
  bool is_utf8(false);
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
    if (!is_lsb8bit && args[i] == "lsb8bit") is_lsb8bit = true;
    if (!is_utf8 && args[i] == "utf8") is_utf8 = true;
    if (!is_utf16le && args[i] == "utf16le") is_utf16le = true;
    if (!is_utf16be && args[i] == "utf16be") is_utf16be = true;
  }

  if (is_binary == is_text) return false;
  if (is_truncate == is_append) return false;
  if (int(is_local8bit) + int(is_lsb8bit)  + int(is_utf8) + int(is_utf16le) + int(is_utf16be) != 1) return false;

  const std::wstring* ps2 = &s0;

  file_io f0;
  f0.open(wsToBsUtf8(complete_path(sPath0, pd, sUserDefDir)).c_str(), true, is_truncate, 1);
    if (f0.result() < 1) { return false; }
  f0.seek_end();
    if (f0.result() < 1) { return false; }
  std::ostringstream f;

  if (is_utf16le)
  {
    if (is_text && (is_truncate || f0.tell() == 0)) { f << '\xff' << '\xfe'; }
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
    if (is_text && (is_truncate || f0.tell() == 0)) { f << '\xfe' << '\xff'; }
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
  else if (is_utf8)
  {
    enum { nbchunk = 200000 };
    f << "\xef\xbb\xbf";
    for (_t_sz i = 0; i < ps2->size(); i += nbchunk)
      { f << wsToBsUtf8(&ps2->at(i), bmdx_minmax::myllmin(ps2->size() - i, nbchunk)); }
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

  std::string s = f.str();
  f0.write(s.c_str(), s.length());
  return f0.result() >= 1;
} catch (...) { return false; }
}

int file_utils::load_bytes(arrayref_t<wchar_t> fnp, std::string& dest, s_long flags) __bmdx_noex
{
  try {
    return file_io::load_bytes(wsbs_utf8(fnp), dest, (flags & 0x2) | 0x1);
  } catch (...) {}
  return -1;
}
int file_utils::load_bytes(arrayref_t<char> fnp, std::string& dest, s_long flags) __bmdx_noex
{
  return file_io::load_bytes(fnp, dest, flags & 0x2);
}
int file_utils::save_bytes(arrayref_t<wchar_t> fnp, arrayref_t<char> src, bool b_append) __bmdx_noex
{
  try {
    return file_io::save_bytes(wsbs_utf8(fnp), src, b_append, 1);
  } catch (...) {}
  return -1;
}
int file_utils::save_bytes(arrayref_t<char> fnp, arrayref_t<char> src, bool b_append) __bmdx_noex
{
  return file_io::save_bytes(fnp, src, b_append);
}

bool file_utils::xHasCurDirShortCut(const std::wstring& pathstr) const { return pathstr==L"." || pathstr.substr(0,1+pslen)==L"."+wpathsep(); }
bool file_utils::xHasCurDirShortCut(const std::string& pathstr) const { return pathstr=="." || pathstr.substr(0,1+pslen)=="."+cpathsep(); }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary module handle.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if bmdx_part_dllmgmt || bmdx_expose_ls_modsm
  extern "C" __BMDX_DLLEXPORT bmdx::unity_common::__Psm __bmdx_ls_modsm() { return bmdx::unity_common::ls_modsm; }
#endif

namespace bmdx
{

s_long unity::compatibility() const __bmdx_noex { return _compatibility(); }
s_long unity::sig_struct() __bmdx_noex
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

    _mod_exhandle(bool b_autounload = true __bmdx_noarg) __bmdx_noex { __2 = 0; __3 = 0; pmsm_rl = 0; handle = 0; prq = 0; pgms = 0; f.htype = 0; f.b_au = b_autounload; }

    bool b_h() const __bmdx_noex { return f.htype > 0; }
    bool b_msm() const __bmdx_noex { return b_h() && pmsm_rl; }
    bool b_rq() const __bmdx_noex { return b_h() && prq; }
    bool b_gms() const __bmdx_noex { return b_h() && pgms; }
    bool b_mainexe() const __bmdx_noex { return f.htype == 2; }

    #if bmdx_part_dllmgmt
      ~_mod_exhandle() __bmdx_noex { if (f.b_au) { mod_unload(); } }

        // 1 - success, 0 - already loaded (may be name or other), -1 - failure.
        // flags:
        //    0x4 - interpret name as UTF-8 and use LoadLibraryW.
        //    0x100 - ignore name, create handle to itself instead.
      s_long mod_load(const char* name, s_long flags __bmdx_noarg) __bmdx_noex
      {
        if (f.htype > 0) { return 0; }

        const bool b_self = !!(flags & 0x100);
        if (b_self)
        {
          int t = utils_dl::_in_main_exe();
            if (!(t == 1 || t == 0)) { return -1; }
          HMODULE h = utils_dl::_hmod_self(!!f.b_au);
            if (!h) { return -1; }
          f.htype = t == 1 ? 2 : 1;
          handle = h;
        }
        else
        {
          if (!name) { return -1; }
          HMODULE h = 0;
          if (*name == '\0') { h = GetModuleHandleA(0); f.htype = 2; }
          else if (flags & 0x4)
          {
            try {
              std::wstring wname = bsws_utf8(name);
              h = LoadLibraryW(wname.c_str());
            } catch (...) { return -1; }
            if (!h) { return -1; }
            f.htype = 1;
          }
          else
          {
            h = LoadLibraryA(name);
              if (!h) { return -1; }
            f.htype = 1;
          }
          handle = h;
        }

        prq = (prequest)sym("bmdx_mod_request");
        pgms = (pget_modsm)sym("__bmdx_ls_modsm");
        return 1;
      }
      void mod_unload(__bmdx_noarg1) __bmdx_noex
      {
        if (f.htype == 1) { FreeLibrary(handle); }
        if (f.htype > 0) { handle = 0; f.htype = 0; }
        prq = 0;
        pgms = 0;
      }
      void* sym(const char* name) const __bmdx_noex { if (!name || !*name) { return 0; } if (f.htype <= 0) { return 0; } return (void*)GetProcAddress(handle, name); }
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

    bool b_h() const __bmdx_noex { return f.htype > 0; }
    bool b_msm() const __bmdx_noex { return b_h() && pmsm_rl; }
    bool b_rq() const __bmdx_noex { return b_h() && prq; }
    bool b_gms() const __bmdx_noex { return b_h() && pgms; }
    bool b_mainexe() const __bmdx_noex { return f.htype == 2; }

    _mod_exhandle(bool b_autounload = true __bmdx_noarg) __bmdx_noex { __2 = 0; __3 = 0; pmsm_rl = 0; handle = 0; prq = 0; pgms = 0; f.htype = 0; f.b_au = b_autounload; }


    #if bmdx_part_dllmgmt
      ~_mod_exhandle() __bmdx_noex { if (f.b_au) { mod_unload(); } }

        // 1 - success, 0 - already loaded (may be name or other), -1 - failure.
        // flags:
        //    0x100 - ignore name, create handle to itself instead.
        //    (flags & 3) == 1 use  RTLD_DEEPBIND if available.
        //    (flags & 3) == 0 do not use  RTLD_DEEPBIND.
        //    (flags & 3) == 2 automatic choice (Linux/g++ - enable, others - disable).
      s_long mod_load(const char* name, s_long flags __bmdx_noarg) __bmdx_noex
      {
        if (f.htype > 0) { return 0; }

        const bool b_self = !!(flags & 0x100);
        if (b_self)
        {
          int t = utils_dl::_in_main_exe();
            if (!(t == 1 || t == 0)) { return -1; }
          void* h = utils_dl::_hmod_self(!!f.b_au);
            if (!h) { return -1; }
          f.htype = t == 1 ? 2 : 1;
          handle = h;
        }
        else
        {
          if (!name) { return -1; }
          int mode = utils_dl::_mode_dlopen(f.b_au, true);
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
          void* h = 0;
          bool b_trg_is_exe = *name == '\0';
          if (b_trg_is_exe)
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
        }

        prq = (prequest)sym("bmdx_mod_request");
        pgms = (pget_modsm)sym("__bmdx_ls_modsm");
        if (!pgms)
        {
          if (b_self) { pgms = (pget_modsm)&__bmdx_ls_modsm; }
          else
          {
              // If the current module has detected h being its own handle, it assigns pgms directly.
              //  This is useful, particularly, in Linux, because it overcomes the effect of "-Wl,-E" switch
              //  missing at build time.
            void* h2 = utils_dl::_hmod_self(true);
            if (h2 == handle) { pgms = (pget_modsm)&__bmdx_ls_modsm; }
            if (h2) { dlclose(h2); }
          }
        }
        return 1;
      }
      void mod_unload(__bmdx_noarg1) __bmdx_noex
      {
        #ifdef RTLD_MAIN_ONLY
        #else
          if (f.htype == 2) { dlclose(handle); }
        #endif
        if (f.htype == 1) { dlclose(handle); }
        if (f.htype > 0) { handle = 0; f.htype = 0; }
        prq = 0; pgms = 0;
      }
      void* sym(const char* name) const __bmdx_noex { if (!name || !*name) { return 0; } if (f.htype <= 0) { return 0; } return (void*)dlsym(handle, name); }
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

bool unity::mod_handle::is_valid() const __bmdx_noex
  { return _rex && _rex.ref()().b_h(); } // implementation-independent
unity::mod_handle::operator bool() const __bmdx_noex
  { return _rex && _rex.ref()().b_h(); } // implementation-independent
bool unity::mod_handle::is_mainexe() const __bmdx_noex
  { return _rex && _rex.ref()().b_mainexe(); } // implementation-independent
unity::mod_handle::t_nativehandle unity::mod_handle::native() const __bmdx_noex
  { if (!_rex) { return 0; } return _rex.ref()().handle; } // implementation-independent
bool unity::mod_handle::operator==(const mod_handle& x) const __bmdx_noex
  { if (!_rex) { return !x._rex; } if (!x._rex) { return false; } return _rex.ref()().handle == x._rex.ref()().handle; } // implementation-independent
bool unity::mod_handle::operator!=(const mod_handle& x) const __bmdx_noex
  { return !operator==(x); }

unity::mod_handle::mod_handle() __bmdx_noex
  : _rex()
    { __rsv1 = 0; }
unity::mod_handle::~mod_handle() __bmdx_noex
  { clear(); }
unity::mod_handle::mod_handle(const mod_handle& h1) __bmdx_noex
  : _rex()
{
  __rsv1 = 0;
  if (!h1._rex) { return; } // OK
  unity::_mod_exhandle& h = h1._rex.ref()();
  cv_ff::cv_rootldr::PFcopy_handle f_ch = (cv_ff::cv_rootldr::PFcopy_handle)h.pmsm_rl(unity_common::msm_rl_cp_handle);
    if (!f_ch) { return; } // this is not expected to occur (see ls_modsm).
  try { f_ch(&h1, this); } catch (...) {}
}
void unity::mod_handle::operator=(const mod_handle& h1) __bmdx_noex
  { if (this == &h1) { return; } mod_handle h3(h1); this->clear(); this->swap(h3); }
void unity::mod_handle::swap(mod_handle& h1) __bmdx_noex
  { bmdx_str::words::swap_bytes(*this, h1); }
void unity::mod_handle::clear() __bmdx_noex
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

void* unity::mod_handle::sym(const char* name) const __bmdx_noex
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.pmsm_rl) { return 0; }
  cv_ff::cv_rootldr::PFsym f1 = (cv_ff::cv_rootldr::PFsym)h.pmsm_rl(unity_common::msm_rl_sym);
    if (!f1) { return 0; } // this is not expected to occur (see ls_modsm).
  try { return f1(this, name); } catch (...) {}
  return 0;
}

s_long unity::mod_handle::request(const unity& para, unity& retval) const __bmdx_noex
{
  if (!_rex) { return -4; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.pmsm_rl) { return -2; }
  cv_ff::cv_rootldr::PFrequest f1 = (cv_ff::cv_rootldr::PFrequest)h.pmsm_rl(unity_common::msm_rl_rq);
    if (!f1) { return -2; } // this is not expected to occur (see ls_modsm).
  try { return f1(this, &para, &retval); } catch (...) {}
  return -2;
}
unity_common::f_ls_modsm unity::mod_handle::_pmsm() const __bmdx_noex
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.b_gms()) { return 0; }
  return h.pgms();
}
unity_common::f_ls_modsm unity::mod_handle::_pmsm_rl() const __bmdx_noex
{
  if (!_rex) { return 0; }
  unity::_mod_exhandle& h = _rex.ref()();
    if (!h.b_msm()) { return 0; }
  return h.pmsm_rl;
}
unity::mod_handle unity::mod_handle::hself(bool b_autounload) __bmdx_noex
{
  unity::mod_handle h;
  unity_common::f_ls_modsm pmsm = unity_common::ls_modsm;
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

      // It the current module is the root loader, it can create and store a handle to itself.
    if (!h && pmsm_rl == pmsm)
    {
      mod_handle he;
      cv_ff::cv_rootldr::PFinit_handle f_ih = (cv_ff::cv_rootldr::PFinit_handle)pmsm_rl(unity_common::msm_rl_init_handle);
      if (f_ih)
      {
        try { f_ih(&he, 0, 0x100 + (b_autounload ? 1 : 0), 0); } catch (...) {}
        if (he._pmsm() == pmsm) { h = he; }
      }
    }
  }
  return h;
}

#if bmdx_part_dllmgmt
  unity::mod_handle unity::mod(const char* name, bool b_au, s_long flags) __bmdx_noex
  {
    unity::mod_handle mh;

    unity_common::__Psm pmsm_rl = 0;
    cv_ff::cv_rootldr::Frootldr(unity_common::ls_modsm, &pmsm_rl); // init own rootldr pointer if necessary
      if (!pmsm_rl) { return mh; }

    cv_ff::cv_rootldr::PFinit_handle f_ih = (cv_ff::cv_rootldr::PFinit_handle)pmsm_rl(unity_common::msm_rl_init_handle);
      if (!f_ih) { return mh; }

    try {
      f_ih(&mh, name, b_au ? 1 : 0, flags);
      #ifdef _bmdxpl_Wnds
        if (!mh && name && (flags & 0x4))
        {
            // UTF-8 name may have failed in older version of BMDX,
            // still, if name contains only characters from the system encoding, it may be possible to use LoadLibraryA.
          std::wstring wn = bsws_utf8(name);
          std::string name2 = wsToBs(wn);
          std::wstring wn2 = bsToWs(name2);
          if (wn2 == wn)
          {
            f_ih(&mh, name2.c_str(), b_au ? 1 : 0, flags & ~0x4);
          }
        }
      #endif
    } catch (...) {}
    return mh;
  }

    // NOTE Finit_handle assumes that it's called in root loader module (see also unity::mod).
    // flags0:
    //    0x1 - remember to autounload when ref. cnt. == 0 (if possible).
    //    0x100 - ignore name, create handle to itself instead.
  s_long cv_ff::cv_rootldr::Finit_handle(unity::mod_handle* pmh, const char* name, s_long flags0, s_long flags)
  {
    if (!pmh) { return 0; }
    if (pmh->_rex) { return -1; }
    flags &= ~0xff00;
    flags |= (flags0 & 0xff00);

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
    PFrootldr f_rl = pmsm ? (PFrootldr)pmsm(unity_common::msm_rootldr) : 0;
    if (f_rl)
    {
      s_long f = 0;
      try { f = f_rl(unity_common::ls_modsm, 0); } catch (...) {}
        if (!f) { return -7; } // this is not expected to occur, because Frootldr does not fail on offer != 0
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

    if (1)
    {
      unity::_mod_exhandle& h = rd.ref()();
        // Even if autounload on ref. count 0 is allowed by client,
        //  but the module is at once shared library and a root loader,
        //  the autounload will be disabled,
        //  because at that time the module may hold handles to child modules,
        //  in rsth, and also in PCOS objects.
        //  (Automatic tracking of these references would be costly.
        //  If automatic unload is quite necessary, the client can ensure it by keeping
        //  main executable single possible root loader,
        //  which means that all its direct and indirect child libraries, which use BMDX,
        //  are loaded via unity::mod().)
      if (!b_au || (!h.b_mainexe() && pmsm && pmsm == h.pmsm_rl)) { h.f.b_au = 0; }
    }
    pmh->_rex = rd; // ref. copy does not fail
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

  s_long cv_ff::cv_paramline::Fpl_encode(const unity* phm, unity* px, const unity* pterm)
  {
    if (!(phm && px && pterm)) { return -1; }
    try {
      unity x2; std::wstring& s = x2.rx<utString>();
      paramline().encode(*phm, s, 0, pterm->vstr());
      px->swap(x2);
      return 1;
    } catch (...) {}
    return -2;
  }
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

namespace
{
  static const s_long _frqperm_msend_anlo = 0x20;
  static const s_long _frqperm_mget_anlo = 0x40;
  static const s_long _frqperm_shmqueue_in_conf = 0x80;

  static const s_long _fl_mget_get_hashlist = 0x1;
  static const s_long _fl_mget_discard_msg = 0x2;
  static const s_long _fl_mget_discard_att = 0x4;
  static const s_long _fl_mget_peek_only = 0x8;
  static const s_long _fl_mget_anlo_msg = 0x10;
  static const s_long _fl_mget_anlo_att = 0x20;
}

struct inprocess_message
{
  typedef arrayref_t<char> t_stringref;

  cref_t<unity, cref_nonlock> hmsg; // message as hashlist, local to dispatcher_mt module
  cref_t<t_stringref> att; // binary attachment as enclosed object, or object reference, local to one of a) sender module, b) dispatcher_mt module

  bool has_hmsg() const __bmdx_noex { return hmsg && hmsg->isHash() && hmsg->hashS_c() > 0; }
  bool has_att() const __bmdx_noex { return att && att->is_valid(); }

  void clear() { hmsg.clear(); att.clear(); }

    // NOTE If retmsg is not set (failure, or disabled by flags), it's not modified at all.
    // Returns:
    //    2 - success, got 2 parts. If b_may_clear == true, the current object is cleared.
    //    1 - success, got 1 part. If b_may_clear == true, the current object is cleared.
    //    0 - no message in hmsg.
    //    -2 - failure (mem. alloc. or smth. else). The current object is not modified.
  s_long move_to_retvals(unity& retmsg, cref_t<t_stringref>* retatt, s_long flags_mget, s_long flags_ses) __bmdx_noex
  {
    const bool b_retstr = (flags_mget & _fl_mget_get_hashlist) == 0;
    const bool b_discard_msg = (flags_mget & _fl_mget_discard_msg) != 0;
    const bool b_discard_bin = (flags_mget & _fl_mget_discard_att) != 0;
    const bool b_may_clear = (flags_mget & _fl_mget_peek_only) == 0;
    const bool b_anlo_msg = (flags_mget & _fl_mget_anlo_msg) && (flags_ses & _frqperm_mget_anlo);
    const bool b_anlo_att = (flags_mget & _fl_mget_anlo_att) && (flags_ses & _frqperm_mget_anlo);
    try {
      if (!has_hmsg()) { return 0; }
      const bool b_2parts = has_att(); // att && att->is_valid()
      const bool b_bin_key = b_2parts && !retatt && !b_discard_bin && !b_discard_msg; // put binary part into "bin" key of the returned message

      cv_ff::cv_mget::PFmget_set_retvals f = (cv_ff::cv_mget::PFmget_set_retvals)retmsg._pmsm()(unity_common::msm_mget_set_retvals);
        if (!f) { return -2; }

      unity __h2;
      unity* phm = hmsg._pnonc_u();
      if (!b_may_clear || (!b_anlo_msg && retmsg._pmsm() != unity_common::ls_modsm))
        { __h2.recreate_as(retmsg); __h2 = *phm; phm = &__h2; }

      const s_long proc_flags = b_bin_key + (b_discard_msg<<1) + (b_discard_bin<<2) + (b_anlo_msg << 3) + (b_anlo_att << 4) + (b_retstr << 5);
      s_long res = f(phm, &this->att, &retmsg, retatt, proc_flags);
        if (res < 1) { return -2; }

      if (b_may_clear) { this->clear(); }
      return b_2parts ? 2 : 1;
    } catch (...) {}
    return -2;
  }

  static void _scan_objs(unity& h, bool b_rmv_objs) // recursive
  {
    if (h.isObject())
    {
      if (b_rmv_objs) { h.clear(); }
    }
    else if (h.isAssoc())
    {
      s_long pos = h.assocl_first();
      while (pos != h.assocl_noel())
      {
        s_long pos2 = h.assocl_next(pos);
        const unity& k = h.assocl_key(pos);
        if (!k.isScalar()) { h.assoc_del(k); }
          else { _scan_objs(h.assocl(pos), b_rmv_objs); }
        pos = pos2;
      }
    }
    else if (h.utype() == utUnityArray)
    {
      if (b_rmv_objs) { for (s_long i = h.arrlb(); i <= h.arrub(); ++i) { _scan_objs(h.ref<utUnity>(i), true); } }
    }
  }

    // 1. Moves (on b_may_move_h == true) or copies (on b_may_move_h == false) h into this->hmsg.
    // 2. Sets this->att = att_ (on b_may_move_h == true), or sets this->att to to copy of att_ data (on b_may_move_h == false).
    // 3. If b_create_string == true, additionally creates this->msg, from this->hmsg.
    // Returns:
    //  1 - success.
    //  -2 - failure (this object will be cleared, but h is not modified). NOTE h may be lost.
  s_long set_parts(unity& h, const cref_t<t_stringref>& att_, bool b_may_move_h, s_long flags_msend, s_long flags_ses)
  {
    const bool b_keep_msg_objects = (flags_msend & _fl_msend_anlo_msg) && (flags_ses & _frqperm_msend_anlo);
    const bool b_assign_att_cref = (flags_msend & _fl_msend_anlo_att) && (flags_ses & _frqperm_msend_anlo);
    try {
      this->clear(); hmsg.create0(0);
      cref_t<t_stringref> att2;
      if (att_ && !b_assign_att_cref) { att2 = i_dispatcher_mt::make_rba(att_.ref(), true); }
      if (b_may_move_h) { hmsg->swap(h); }
        else { *hmsg._pnonc_u() = h; }
      _scan_objs(*hmsg._pnonc_u(), !b_keep_msg_objects);
      if (b_assign_att_cref) { att = att_; }
        else { att = att2; }
      return 1;
    } catch (...) {}
    this->clear();
    return -2;
  }

    // 1 - success, 0 - no message in both hmsg, -2 - failure
  static s_long create_msg_wstr(const unity& hmsg, unity& x, bool b_local_to_x) __bmdx_noex
  {
    try {
      if (b_local_to_x && x._pmsm() != unity_common::ls_modsm)
      {
        cv_ff::cv_paramline::PFpl_encode f = (cv_ff::cv_paramline::PFpl_encode)x._pmsm()(unity_common::msm_pl_encode);
        if (!f) { return -2; }
        s_long res = f(&hmsg, &x, &paramline::uwpterm);
        return res < 1 ? -2 : 1;
      }
      else
      {
        unity x2; std::wstring& s = x2.rx<utString>();
        paramline().encode(hmsg, s);
        x.swap(x2);
        return 1;
      }
    } catch(...) {}
    return -2;
  }
};

  // 1 - success, -2 - failure.
s_long cv_ff::cv_mget::Fmget_set_retvals(unity* phm, const cref_t<t_stringref>* patt, unity* pretmsg, cref_t<t_stringref>* retatt, s_long proc_flags)
{
  if (!(phm && patt && pretmsg)) { return -2; } // but retatt may be 0
  const cref_t<t_stringref>& att = *patt;
  const bool b_2parts = att && att->is_valid();
  const bool b_bin_key = proc_flags & 1;
  const bool b_discard_msg = !!(proc_flags & 2);
  const bool b_discard_bin = !!(proc_flags & 4);
  const bool b_anlo_msg = !!(proc_flags & 8);
  const bool b_anlo_att = !!(proc_flags & 16);
  const bool b_retstr = !!(proc_flags & 32);
  try {
    if (b_bin_key) { unity& rbin = phm->hash(L"bin"); unity bin2(bsLsbToWs(att->pd(), att->n())); rbin.swap(bin2); }
      // Put message into retmsg.
    if (!b_discard_msg)
    {
      if (b_retstr) { unity s; std::wstring& rstr = s.rx<utString>(); paramline().encode(*phm, rstr); pretmsg->swap(s); }
        else { inprocess_message::_scan_objs(*phm, !b_anlo_msg); pretmsg->swap(*phm); }
    }
      // Put attachment into retatt.
    if (retatt) { if (b_2parts && !b_discard_bin) { *retatt = b_anlo_att ? att : i_dispatcher_mt::make_rba(att.ref(), true); } else { retatt->clear(); } }
    return 1;
  } catch(...) {}
  return -2;
}

struct haddrl_phase_info
{
  s_ll id_msg_cmd, value;
  haddrl_phase_info() : id_msg_cmd(-1), value(0) {}
  haddrl_phase_info(s_ll id_, s_ll value_) : id_msg_cmd(id_), value(value_) {}
};

} // namespace bmdx
namespace yk_c
{
  template<> struct vecm::spec<haddrl_phase_info> { typedef haddrl_phase_info t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
}
namespace bmdx
{

struct cch_slot
{
  struct qbci_value { inprocess_message msg; cref_t<std::wstring, cref_nonlock> src; s_ll id_msg_cmd; qbci_value() : id_msg_cmd(-1) {} };
  struct qs_value : inprocess_message {};
  struct csl_tracking { csl_tracking(__bmdx_noarg1) {} unity thn, sln_exact, key_hbo; };
  struct csl_tracking_ext { csl_tracking_ext(__bmdx_noarg1) {} csl_tracking csl_info; i_dispatcher_mt::tracking_info trk_info; };

  typedef vnnqueue_t<inprocess_message, __vecm_tu_selector> t_qumi;
  typedef vnnqueue_t<cref_t<qbci_value, cref_nonlock>, __vecm_tu_selector> t_qubci;
  typedef vnnqueue_t<qs_value, __vecm_tu_selector> t_qus;

  union { bmdx_meta::u_ll type; char type_str[4]; }; // dflt: type = 0
  bool eq_type(const char* t2) const { if (!t2) { return false; } return std::strcmp(type_str, t2) == 0; }

  critsec_t<dispatcher_mt>::csdata lkd; // for locking pins and queues against multithreaded read/write
  inprocess_message r_pin; // input pin data
  cref_t<vnnqueue_t<inprocess_message, __vecm_tu_selector>, cref_nonlock> r_qumi; // input queue for messages (qi)
  cref_t<vnnqueue_t<cref_t<qbci_value, cref_nonlock>, __vecm_tu_selector>, cref_nonlock> r_qubci; // qbi, hbo
  cref_t<vnnqueue_t<qs_value, __vecm_tu_selector>, cref_nonlock> r_qus; // subscription queue (qs, on origin side)
  cref_t<std::wstring, cref_nonlock> r_lsrc; // source address of recently popped command, for command inputs (pbi, qbi)

    // r_haddrl
    //  a) for qs slot: "output list" { target address (string), () }; object must exist
    //  b) for subscribed input slot (pi, qi): "input list" { source address (string); () }
    //    The ref. object may not exist by default, and be autocreated on first subscription request of any type.
    //  c) for hbo slot: { command target address (string), (id_msg, status) } - lists uniquely all targets, for which the command has been issued,
    //    but the response, even if arrived, not popped yet by the issuer.
    //    status (values same as cch_slot::phase):
    //      4 - command sent, response not received yet.
    //      5 - command sent, response received, not popped yet.
    //      Initially, no keys with  target addresses in the hash map.
    //      After particular target's response has been popped -- the is removed from the hash.
    //  d) for any other slot (incl. non-subscribed pi, qi): no reference (no inputs, or this slot type cannot subscribe)
    // NOTE For subscriptions, exact correspondence between subscriber's input list and supplier's (qs) output list
    //    is not guaranteed. The lists may be unequal (and this is normal) in the following situations:
    //      a) when creating slots with predefined local subscriptions, without ensure_local_subs flag: common failure during subscription,
    //          after the slot itself has been created.
    //      b) non-local technical message (subscription request) transfer failure, or subscription failure on remote end.
    //          E.g. CPU process A starts later than B, but only in B the subscription is predefined. (It fails because A does not exist yet. See also (c) below.)
    //      c) qs output list is configured as fixed (see "output fixed" param.), i.e. cannot be changed when a subscriber
    //        with pre-configured subscription to that qs is added or removed.
    //        Though, such subscription automatically starts working when pre-configured on both qs and subscriber side + both slots are created,
    //        even if one later than another.
  typedef hashx<unity, haddrl_phase_info> t_haddrl;
  cref_t<hashx<unity, haddrl_phase_info>, cref_nonlock> r_haddrl;

    // phase
    //  0 - no msg.
    //  1 - input msg arrived.
    //      2 - input msg popped.
    //      3 - command response pushed by the receiver and forwarded to sender.
    //  4 - command sent, response is now expected.
    //      5 - command response received.
    //      6 - command response is popped.
    //      7 - (for non-local messages only: command sending in progress, sending not confirmed yet by peer)
    //  pbo: 0, 6 --> opt. 7 --> 4 --> 5 --> 6
    //  hbo (for each distinct command target): entry inserted --> opt. 7 --> 4 --> 5 --> entry removed
    //  pbi: 0, 3 --> 1 --> 2 --> 3
    //  qbi: 0, 3 --> 2 --> 3
    //  pi: 0, 2 --> 1 --> 2
  s_long phase;

    // Constant flags, part of slot configuration. May be read without locking.
  s_long ccfgflags;
    bool b_input_any_th() const;
    bool b_input_lpa() const;
    s_long qs_delivery_type() const;
    bool qs_deliv_imm() const;
    bool qs_deliv_thread() const;
    bool qs_deliv_disp() const;
    bool qs_output_fixed() const;

  s_ll id_msg_cmd; // original command message id (sent from pbo, hbo), associated with the current response; for dflt. and none: -1

  cch_slot();
};
struct cch_thread
{
    // RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, cref_t<cch_slot> > h_sl;

    // { qs slot name (string), 0 } thread's qs slots with immediate delivery
    //  RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, s_long> h_qs_imm;

    // { qs slot name (string), 0 } thread's qs slots with delivery by thread periodic()
    //  RW lock: mst_semaphore m_sl_acquire. Ro lock: m_th_ro_acquire.
  hashx<unity, s_long> h_qs_thread;

    // true if this thread is allowed to execute LPA delivery (for qs queues with 'input lpa = true", in all threads).
    //  This flag is assigned once on thread creation, and is constant during thread lifetime.
  bool cb_disp;

    // "Modification state".
    // Bit 0 (+-sl): 1 during slots a) adding, b) removal.
    //  Bit 1 (+-th): 1 during a) thread creation, b) as intention for and during removal.
    //  Bit 2 (read-only access to thread object): 1 during long thread object reading. Same as +-sl, but thread slots are accessible for i/o.
    //  Bits 3..63: count of read-only accesses currently acquired
    // RW lock: mst_semaphore multiple functions.
  meta::s_ll mst;

  cch_thread();
  ~cch_thread();
};
} // namespace bmdx
namespace yk_c
{
  template<> struct vecm::spec<cch_slot> { struct aux : vecm::config_aux<cch_slot> { }; typedef config_t<cch_slot, 1, 4, 1, aux> config; };
  template<> struct vecm::spec<cch_slot::csl_tracking> { typedef cch_slot::csl_tracking t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
  template<> struct vecm::spec<cch_slot::csl_tracking_ext> { typedef cch_slot::csl_tracking_ext t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
  template<> struct vecm::spec<cch_thread> { struct aux : vecm::config_aux<cch_thread> { }; typedef config_t<cch_thread, 1, 4, 1, aux> config; };
}
namespace bmdx
{
struct dispatcher_mt::cch_session
{
  cch_session(__bmdx_noarg1)
  :
    gm(0),
    exitmode(2),
    ses_state(-2),
    frqperm(0xff),
    __thm_lqsd(1), __thm_lmsc(1), __thm_nsc(1), __thm_cdcc(1),
    nprx(0),
    lqsd_dt(1000), qsd_prio(4)
  {}

  bmdx_shm::critsec_gn gm; // interprocess lock to ensure unique process name
  int exitmode;
    // -2 in _s_disp_ctor, when dispatcher session is not valid.
    // -1 in _s_disp_ctor, when dispatcher session is initialized, and can handle subscription requests.
    // 1 during the session.
    // 0 after the session
  volatile int ses_state;
  void __set_ses_state(int st __bmdx_noarg)
  {
    if (st == 1) { ses_state = st; if (lmsc) { lmsc->clt_set_ses_state(st); } }
      else { if (lmsc) { lmsc->clt_set_ses_state(st); } ses_state = st; }
  }

    // Dflt. == 0xff. Desc.: see dispatcher_mt frqperm();
  volatile s_long frqperm;
  unsigned char __thm_lqsd, __thm_lmsc, __thm_nsc, __thm_cdcc; // internal threads activity mode; NOTE all by dflt. = -1
  void __thm_lqsd_enable_full(__bmdx_noarg1) { if (th_lqsd) { __thm_lqsd = 2; } }
  void __thm_lmsc_enable_full(__bmdx_noarg1) { if (lmsc) { __thm_lmsc = 2; } }

    // Short-time lock for main cch_session members: hg_threads, hg_qs_disp, hg_lpai (for read/write); name_th_disp (for associated variable modification).
  critsec_t<dispatcher_mt>::csdata lkd_disp_ths;

    //  RW lock: lkd_disp_ths.
    // NOTE hg_threads destruction is safe, because its enclosing object (cch_session)
    //    is held reference-counted by all clients.
  hashx<unity, cref_t<cch_thread> > hg_threads;

    // { qs slot name (string), owning thread name (string) } -- process-global qs slots from different threads.
    //  Delivery by disp. thread periodic().
    //  Modified by: _s_add_slots_nl, _s_slots_remove.
    //  RW lock: lkd_disp_ths.
    //  Integrity condition: value --> valid thread, key --> valid slot in that thread.
  hashx<unity, unity> hg_qs_disp;

    // { pi or qi slot name (string), owning thread name (string) } -- process-global input slots from different threads.
    //  Modified by: _s_add_slots_nl, _s_slots_remove.
    //  RW lock: lkd_disp_ths.
    //  Integrity condition: value --> valid thread, key --> valid slot in that thread.
  hashx<unity, unity> hg_lpai;

  critsec_t<dispatcher_mt>::csdata lkd_disp_nprx; // nprx incr/decr lock
  volatile s_long nprx; // nprx counts the dispatcher_mt single object + all thread_proxy objects (and does NOT count LMSC and any other internal threads)

  meta::s_ll lqsd_dt; // (const) >= 0: global qs delivery period, mcs

  std::wstring name_pr; // (const) original process name, passed by client

  critsec_t<dispatcher_mt>::csdata lkd_disp_s_qs_deliver; // internal lock in _s_qs_deliver

  threadctl th_lqsd; // qs messages automatic delivery
  s_long qsd_prio; // last set th_lqsd priority

  threadctl th_lmsc; // executes lmsc periodic task
  cref_t<lm_slot_controller> lmsc;
    critsec_t<dispatcher_mt>::csdata lkd_htrk_csl;
    hashx<s_ll, cch_slot::csl_tracking> htrk_csl; // { id_msg; temporarily associated command slot tracking info }; locking: lkd_htrk_csl
};
  // cch_thread mst flags control.
  //  Used by all data transfer functions and slot/thread modifications,
  //  to set actions in order and also, in certain cases, cancel them.
struct dispatcher_mt::mst_semaphore
{
  int m_sl_acquire() __bmdx_noex;
  int m_th_acquire() __bmdx_noex;
  int m_th_ro_acquire() __bmdx_noex;
  void release() __bmdx_noex;
  void set_refs(dispatcher_mt::cch_session& ths_, const std::wstring& thn_);
  mst_semaphore(dispatcher_mt::cch_session& ths_, const std::wstring& thn_);
  mst_semaphore(const mst_semaphore& x);
  mst_semaphore();
  ~mst_semaphore() __bmdx_noex;

  int r_sl(const std::wstring& sln, cref_t<cch_slot>& ret_sl) __bmdx_noex;
  cch_thread* p_thread(bool b_inited_only = false) __bmdx_noex;
  dispatcher_mt::cch_session* p_ses();
  const std::wstring& thn();

  private:
    dispatcher_mt::cch_session* _p_ths;
    std::wstring _thn;
    int rsv;
    int m_allowed; // either 1: allows +-sl in the current thread, or 2: allows +-th
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
    hangdet(double to_ms_ = 5000.) : to_ms(to_ms_) { t0 = clock_ms(); }
    operator bool() const { return clock_ms() - t0 >= to_ms; }
    void reset() { t0 = clock_ms(); }
  };

  using bmdx_main_intl_lib::refmaker_t;
  using bmdx_main_intl_lib::weakref_t;

  static double idgen_t0(clock_ms());
}
struct dispatcher_mt::thread_proxy : i_dispatcher_mt
{
  struct lks_idgen {};
    // Common ID generator for messages and subscription requests.
    //  NOTE Non-local msg. delivery and subscription tracking uses the same structures.
  static s_ll idgen();
  static s_ll make_id0();
  typedef hashx<std::wstring, s_long, hashx_common::kf_basic<std::wstring>, __vecm_tu_selector> t_hsubs_L2;
  typedef hashx<std::wstring, t_hsubs_L2, hashx_common::kf_basic<std::wstring>, __vecm_tu_selector> t_hsubs;

  struct th_lqsd_impl : threadctl::ctx_base { virtual void _thread_proc(); };
  struct th_lmsc_impl : threadctl::ctx_base, lm_slot_controller::i_callback
  {
    virtual void _thread_proc();
    virtual s_long local_write(t_stringref umsg, s_long enc, const cref_t<t_stringref>& bin, s_ll id_msg, s_ll id_msg_cmd);
    virtual s_long local_subscribe(t_stringref mde, const netmsg_header::mdd_subs_request& dm, const bmdx_shm::t_name_shm& name_peerf);
    virtual s_long cslph_update(vec2_t<tracking_info>& tii);
  };

  struct address
  {
    address(__bmdx_noarg1) {}

    const unity& addr() const __bmdx_noex;
    bool is_empty() const __bmdx_noex;
    s_long n() const __bmdx_noex;
    const unity& addr(s_long ind) const;
    std::wstring wstr_addr() const;
    std::wstring wstr_sln() const;
    unity sln_v() const;
    std::wstring wstr_sln_1() const;
    std::wstring wstr_sln_tail() const;
    bool has_sln_tail() const __bmdx_noex;
    std::wstring wstr_thn() const;
    std::wstring wstr_pn() const;
//    std::wstring wstr_hostn() const;

    s_long set_addr(const unity& x) __bmdx_noex;
    s_long set_addr_LP(const std::wstring& thn, const unity& sln);

    bool isLP_any() const __bmdx_noex;
    bool isLP() const __bmdx_noex;
    bool isLPA() const __bmdx_noex;
    bool isLM() const __bmdx_noex;
    bool isLMA() const __bmdx_noex;
    bool isR() const __bmdx_noex;
    bool isRMA() const __bmdx_noex;

    s_long sln_ind() const __bmdx_noex;
    _fls75 slt() const __bmdx_noex;

    bool _conv_LP_LM(arrayref_t<wchar_t> name_peer) __bmdx_noex;
    bool _conv_LM_LP() __bmdx_noex;
    s_long _sln_ind0(const _fls75& k1) const __bmdx_noex;

  private:
    unity _addr;
  };
  static void __append_vals(unity& dest, const unity& src, s_long n_skip = 0);
  static s_long __recode_slotname(const unity& sn, std::wstring* retval = 0, std::wstring* pn1 = 0, std::wstring* ptail = 0, const address* pdtailcmp = 0) __bmdx_noex;



  static bool thnchk(arrayref_t<wchar_t> s) __bmdx_noex;
  static bool pnchk(arrayref_t<wchar_t> s) __bmdx_noex;
  static bool hostportchk(const std::wstring& s) __bmdx_noex;
  static bool is_addr_cat_all(arrayref_t<wchar_t> s) __bmdx_noex;

  static std::wstring sln1_root(const std::wstring& sln1);
  static bool sln1chk_main(const std::wstring& sln1);
  static bool sln1chk_main_qs(const std::wstring& sln1);
  static bool sln1chk_qs(const std::wstring& sln1);
  static bool sln1chk_subscriber(const std::wstring& sln1) __bmdx_noex;
  static bool sln1chk_main_o(const std::wstring& sln1);
  static bool sln1chk_main_i(const std::wstring& sln1);
  static bool sln1chk_main_bo(const std::wstring& sln1);
  static bool sln1chk_main_bi(const std::wstring& sln1);
  static bool sln1chk_iomatch(const std::wstring& ssln1, const std::wstring& dsln1);

  typedef i_dispatcher_mt::tracking_info tracking_info;

private:
  std::wstring _name_th;
  std::wstring _name_pr;
  cref_t<dispatcher_mt::cch_session> _r_ths;
    // mtrk_htracking notes:
    //    1) when removed together with parent object: all contained client objects (tracking_info) are released at the same time
    //      => no client tracking_info's from this proxy are left anywhere in the dispatcher session.
    //    2) In _s_proxy_init, .info is created automatically, but .strongref is not created: it will be created on the first request for message tracking from proxy's client.
    //  Locking: any of weakref_t<t_htracking_proxy>::t_lock or refmaker_t<t_htracking_proxy>::t_lock (this is the same) during adding/removing/modifying hash map elements.
  refmaker_t<t_htracking_proxy> mtrk_htracking; // { msg id; strong refs. to tracking_info }

    struct _hpt_conf_reset_lks {};
  cref_t<hashx<unity, s_long> > _hpt_conf_reset; // for ~thread_proxy: hash key: peer tracker name, hash value: peer tracker queues conf. reset (ORed) flags: 0x1 - reset sender, 0x2 - reset receiver

  thread_proxy(const thread_proxy&); thread_proxy& operator=(const thread_proxy& x);

public:
  enum Esender_type
  {
    st_client = 0,
    st_s_subs_deliver = 1,
    st_lmsc = 2,
    st_update_subs_input = 3, // direct call from _s_update_subs_lists to _s_subscribe
    st_update_subs_output = 4 // direct call from _s_update_subs_lists to _s_subscribe
  };

  static s_long _s_pop(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt, s_long flags) __bmdx_noex;
  static s_long _s_write(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& msg, const cref_t<t_stringref>& att, Esender_type sender_type, const s_long flags, bool b_may_swap_msg, s_ll id_msg_nl, s_ll id_msg_orig, refmaker_t<t_htracking_proxy>* prhtrprx, bool* pb_da_is_local) __bmdx_noex;
  static s_long _s_request(thread_proxy* pprx, cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, unity& retval, const unity& args, int frqperm, s_long flags) __bmdx_noex;
  static s_long _s_subscribe(cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, Esender_type sender_type, const address& qsa, const address& suba, s_ll id_rq_subs, refmaker_t<t_htracking_proxy>* prhtrprx, s_long* pret_destloc) __bmdx_noex;

  static s_long _s_qs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, s_long flags, s_ll* pnmsent) __bmdx_noex;
  static s_long _s_subs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, cref_t<cch_slot>& r_qs, const std::wstring& _name_th, const std::wstring& _name_qs, s_ll* pnmsent) __bmdx_noex;

  static s_long _s_thread_create(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& _cfg0, s_long flags_cr) __bmdx_noex;
  static s_long _s_thread_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thn, const unity* pthn = 0) __bmdx_noex;

  static s_long _s_add_slots_nl(cch_session& rses, const std::wstring& k_th, const unity& slotscfg, t_hsubs& hsubs_ins, t_hsubs& hsubs_outs, mst_semaphore* pms = 0) __bmdx_noex;
  static s_long _s_slots_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotnames, const std::wstring& _name_th, s_long flags_rmv) __bmdx_noex;
  static s_long _s_slot_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotname0, const std::wstring& _name_th) __bmdx_noex;
  static s_long _s_update_subs_lists(cref_t<dispatcher_mt::cch_session>& _r_ths, const t_hsubs& hsubs, int actions) __bmdx_noex;

  static void _s_thh_signal_stop(cch_session& rses) __bmdx_noex;
  static bool _s_thh_active(cch_session& rses) __bmdx_noex;

  static void _s_disp_ctor(dispatcher_mt* pdisp, arrayref_t<wchar_t> process_name, const unity& _cfg0, s_long flags_ctor) __bmdx_noex;
  static void _s_disp_dtor(dispatcher_mt* pdisp) __bmdx_noex;
  static s_long _s_proxy_new(const cref_t<dispatcher_mt::cch_session>& _r_ths, unity& retval, arrayref_t<wchar_t> _name_th) __bmdx_noex;
  static s_long _s_proxy_init(thread_proxy& x, const cref_t<dispatcher_mt::cch_session>& _r_ths, arrayref_t<wchar_t> thread_name) __bmdx_noex;

  thread_proxy();
  virtual ~thread_proxy();
  virtual s_long mget(const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt, s_long flags) __bmdx_noex;
  virtual s_long msend(const unity& msg, cref_t<t_stringref> att, s_long flags, cref_t<tracking_info> tracking) __bmdx_noex;
  virtual s_long request(s_long rt, unity& retval, const unity& args, s_long flags) __bmdx_noex;
  virtual s_long slots_create(const unity& _cfg0, s_long flags) __bmdx_noex;
  virtual s_long slots_remove(const unity& slotnames, s_long flags) __bmdx_noex;
  virtual s_long thread_name(unity& retname) __bmdx_noex;
  virtual s_long process_name(unity& retname) __bmdx_noex;
  virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt, cref_t<tracking_info> tracking) __bmdx_noex;
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
cch_slot::cch_slot() : type(0), phase(0), ccfgflags(0), id_msg_cmd(-1) {}
cch_thread::cch_thread() : cb_disp(false), mst(0) {}
cch_thread::~cch_thread() {}


  // m_*_acquire() requests one of
  //    a) exclusive lock on cch_thread for slots collection modification
  //        (slots adding/removal),
  //    b) exclusive lock on cch_thread for its initialization or destruction
  //        (right after adding / right before removal from hg_threads, which itself occurs under lkd_disp_ths
  //        as well as any hg_threads operation),
  //    c) semaphore count increment (one for mst_semaphore object)
  //        for thread object read-only access.
  //  One mst_semaphore object may hold only one of the above locks.
  //  The client may call m_*_acquire() of the chosen type either once,
  //    or multiple times until allowed to make modification or read data.
  //  Thread data modification itself is not additionally guarded by any other lock, because all clients use mst_semaphore
  //    as the only means of locking.
  //  Thread data read-only access is allowed for multiple clients at a time, and is not additionally guarded
  //    by any other lock as well.
  //  When mst_semaphore of that client goes out of scope, the appropriate mst flag, held by this mst_semaphore,
  //      is automatically cleared, allowing for further operations on the thread object and threads collection.
  // Return values:
  //    -1, -3 - failure, the client should stop further efforts.
  //        (-1 - multiple causes. -3 - thread does not exist.)
  //    0 - the client should wait until allowed to operate (ret. code 1) or fail (ret. code < 0).
  //    1 - success, the client should make intended operations.
int dispatcher_mt::mst_semaphore::m_sl_acquire() __bmdx_noex
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 1) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 1)) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v._rnonc();
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 1 && (th.mst & 2)) { rsv = 0; return -1; }
  if (rsv == 0) { rsv = 1; } // ask for +-sl
  if (th.mst & (1+4)) { return 0; } // need waiting for someone else's op. completion
  th.mst |= 1; m_allowed = 1; // start with +-sl in the current thread
  return 1;
}
int dispatcher_mt::mst_semaphore::m_th_acquire() __bmdx_noex
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 2) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 2)) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v._rnonc();
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 0) { th.mst |= 2;  rsv = 2; } // reserve +-th
  if (th.mst & (1+4)) { return 0; } // need waiting for someone else's op. completion
  m_allowed = 2; // start with +-th in the current thread
  return 1;
}
int dispatcher_mt::mst_semaphore::m_th_ro_acquire() __bmdx_noex
{
  if (!_p_ths) { return -1; }
  if (m_allowed == 4) { return 1; } else if (m_allowed != 0 || !(rsv == 0 || rsv == 4)) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) { return -1; } if (!e_th) { rsv = 0; m_allowed = 0; return -3; }
  cch_thread& th = e_th->v._rnonc();
  if (rsv == 0 && (th.mst & 2)) { return -1; } // +-th is already reserved by someone
  if (rsv == 4 && (th.mst & 2)) { rsv = 0; return -1; }
  if (rsv == 0) { rsv = 4; } // ask for read-only access to thread object
  if (th.mst & 1) { return 0; } // need waiting for someone else's completing +-sl
  th.mst |= 4; th.mst += 8; m_allowed = 4; // add to read-only acquire count
  return 1;
}

void dispatcher_mt::mst_semaphore::release() __bmdx_noex
{
  if (rsv)
  {
    if (!_p_ths) { rsv = 0; m_allowed = 0; return; }
    critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
    const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) { return; } if (!e_th) { rsv = 0; m_allowed = 0; return; }
    cch_thread& th = e_th->v._rnonc();
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
dispatcher_mt::mst_semaphore::~mst_semaphore() __bmdx_noex { release(); }

  // sln: valid (recoded) slot name.
  // Returns:
  //    1 and valid reference to slot sln in thread thn(), if the slot exists, and any of the following is true:
  //        a) no locks or semaphores are set on the thread thn(),
  //        b) anyone's m_th_ro_acquire have set and is currently holding at least one lock.
  //    0 if the client should wait until slots list modification is completed. ret_sl is not modified.
  //        This ret. code is possible only if someone's m_sl_acquire has set and is currently holding a lock.
  //    -1 if the slot does not exist, or slot or thread is locked (i.e. slot cannot be accessed during unknown amount of time). ret_sl is not modified.
  //    -3 if the thread does not exist. ret_sl is not modified.
  // NOTE Any access to slot itself is guarded by its individual lock (cch_slot lkd),
  //    which is never mixed with session lock (cch_session lkd_disp_ths).
  //    If the slot removal from its containing thread occurs in between session lock and slot lock,
  //    the slot may be still used to complete operation (e.g. read from message queue) without additional checks
  //    or canceling. The slot object is automatically deleted when the last cref_t<cch_slot>
  //    goes out of scope.
int dispatcher_mt::mst_semaphore::r_sl(const std::wstring& sln, cref_t<cch_slot>& ret_sl) __bmdx_noex
{
  if (!_p_ths) { return -1; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) { return -1; } if (!e_th) { return -3; }
  cch_thread& th = e_th->v._rnonc();
  if (m_allowed == 0) { do { if (th.mst & 4) { break; } if (th.mst & 2) { return -1; } if (th.mst & 1) { return 0; } } while (false); }
  const hashx<unity, cref_t<cch_slot> >::entry* e_sl = 0;
  try { e_sl = e_th->v->h_sl.find(sln); } catch (...) { return -1; } if (!e_sl) { return -1; }
  ret_sl = e_sl->v;
  return 1;
}

  // Returns ptr. to associated thread object, if it exists in session.
  //  To access the object, any
  //    a) m_th_ro_acquire or m_th_acquire must be called first, and succeed (return 1).
  //    b) lock on lkd_disp_ths must be set during use if p_thread's pointer .
  // b_inited_only:
  //    if true, p_thread() will return 0 during cch_thread initialization and destruction, even if thread exists.
  //    This should be used for short-time access to special thread members (mtrk_htracking),
  //    under lkd_disp_ths instead of m_*_acquire().
cch_thread* dispatcher_mt::mst_semaphore::p_thread(bool b_inited_only) __bmdx_noex
{
  if (!_p_ths) { return 0; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_p_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; try { e_th = _p_ths->hg_threads.find(_thn); } catch (...) {} if (!e_th) { return 0; }
  cch_thread* pth = e_th->v._pnonc_u();
  if (!pth) { return 0; }
  if (b_inited_only && !!(pth->mst & 2)) { return 0; }
  return pth;
}

dispatcher_mt::cch_session* dispatcher_mt::mst_semaphore::p_ses() { return _p_ths; }
const std::wstring& dispatcher_mt::mst_semaphore::thn() { return _thn; }



dispatcher_mt::thread_proxy::thread_proxy() {}
dispatcher_mt::thread_proxy::~thread_proxy()
{
  if (this->mtrk_htracking.b_nonempty())
  { try {
    refmaker_t<t_htracking_proxy>::t_lock __lock(this->mtrk_htracking); if (sizeof(__lock)) {}
    t_htracking_proxy& h = this->mtrk_htracking.strongref._rnonc();
    for (s_long i = 0; i < h.n(); ++i)
    {
      cref_t<i_dispatcher_mt::tracking_info>& r = h(i)->v;
      if (r->state >= 0) { r->state = -15; }
    }
    h.hashx_clear();
  } catch (...) {} }
  if (_r_ths)
  {
    dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
    try {
      if (rses.lmsc && this->_hpt_conf_reset)
      {
        critsec_t<_hpt_conf_reset_lks> __lock(10, -1); if (sizeof(__lock)) {}
        const hashx<unity, s_long>& h = this->_hpt_conf_reset.ref();
        for (s_long i = 0; i < h.n(); ++i)
        {
          s_long fl = h(i)->v;
          if (fl & 0x1) { rses.lmsc->clt_qum_conf_reset(h(i)->k.rstr(), false, false); }
          if (fl & 0x2) { rses.lmsc->clt_qum_conf_reset(h(i)->k.rstr(), true, false); }
        }
      }
    } catch (...) {}
    critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths->lkd_disp_nprx); if (sizeof(__lock)) {}
    rses.nprx -= 1; if (rses.nprx <= 0) { rses.__set_ses_state(0); _s_thh_signal_stop(rses); } // the last proxy, on being released, ends dispatcher session
  }
  _r_ths.clear();
}

s_long dispatcher_mt::thread_proxy::mget(const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt, s_long flags) __bmdx_noex { return _s_pop(_r_ths, _name_th, slotname, retmsg, retatt, flags); }
s_long dispatcher_mt::thread_proxy::msend(const unity& msg, cref_t<t_stringref> att, s_long flags, cref_t<tracking_info> tracking) __bmdx_noex
{
  if (!tracking)
  {
    return dispatcher_mt::thread_proxy::_s_write(_r_ths, _name_th, msg, att, st_client, flags, false, -1, -1, 0, 0);
  }
  else
  {
    tracking->id = idgen();
    tracking->state = -1; if (!tracking.is_cm()) { return -1; }
    tracking->state = -2; if (!_r_ths.has_ref()) { return -2; }
    dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
    tracking->state = -3; if (rses.ses_state != 1) { return -3; }
    tracking->state = -2;
    if (1)
    {
      refmaker_t<t_htracking_proxy>::t_lock __lock(this->mtrk_htracking); if (sizeof(__lock)) {}
      try {
        const t_htracking_proxy::entry* e = 0;
        if (this->mtrk_htracking.strongref->insert(tracking->id, &e) != 1) { return -2; }
        e->v = tracking;
      } catch (...) { return -2; }
    }
    bool b_da_is_local = true;
    enum { trkstate_not_modified = -1999 }; // _s_write never returns this code
    tracking->state = trkstate_not_modified;
    s_long res = dispatcher_mt::thread_proxy::_s_write(_r_ths, _name_th, msg, att, st_client, flags, false, -1, tracking->id, &this->mtrk_htracking, &b_da_is_local);
    if (1)
    {
      refmaker_t<t_htracking_proxy>::t_lock __lock(this->mtrk_htracking); if (sizeof(__lock)) {}
      if (b_da_is_local) { tracking->state = res >= 0 ? 3 : res; }
        else { if (tracking->state == trkstate_not_modified) { tracking->state = res; } }
      if (res < 0 || b_da_is_local) { try { this->mtrk_htracking.strongref->remove(tracking->id); } catch (...) {} }
    }
    return res;
  }
}
s_long dispatcher_mt::thread_proxy::request(s_long rt, unity& retval, const unity& args, s_long flags) __bmdx_noex
{
  return dispatcher_mt::thread_proxy::_s_request(this, _r_ths, rt, retval, args, _r_ths ? _r_ths.ref().frqperm : 0, flags);
}
s_long dispatcher_mt::thread_proxy::slots_create(const unity& _cfg0, s_long flags_cr) __bmdx_noex
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }

  if (!(_cfg0.isString() || (_cfg0.isAssoc() && _cfg0.compatibility() > 0))) { return -1; }
  unity _cfg1; try { if (_cfg0.isString()) { paramline().decode_tree(_cfg0.vstr(), _cfg1, 0x3a); } } catch (...) { return -2; }
  const unity& cfg = _cfg0.isString() ? _cfg1 : _cfg0;

  s_long res = -2;
  cch_session& rses = *_r_ths._pnonc_u(); t_hsubs hsubs_ins, hsubs_outs;
  res = dispatcher_mt::thread_proxy::_s_add_slots_nl(rses, _name_th, cfg, hsubs_ins, hsubs_outs);
  if (res == 1)
  {
    s_long resfl = dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_ins, 16|8);
    resfl |= dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_outs, 4|8);
      resfl &= ~s_long(0x10); // "failed to update non-local subscription" is not an error in here
    if (resfl && !!(flags_cr & dispatcher_mt_flags::ensure_local_subs)) { try { dispatcher_mt::thread_proxy::_s_slots_remove(_r_ths, cfg[L"slots"], _name_th, 0); } catch (...) {} return -2; }
  }
  else if (res == -3) { res = -7; }
  return res;
}
s_long dispatcher_mt::thread_proxy::slots_remove(const unity& slotnames, s_long flags) __bmdx_noex { return dispatcher_mt::thread_proxy::_s_slots_remove(_r_ths, slotnames, _name_th, flags); }
s_long dispatcher_mt::thread_proxy::thread_name(unity& retname) __bmdx_noex { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_th; return 1; } catch (...) { return -2; } }
s_long dispatcher_mt::thread_proxy::process_name(unity& retname) __bmdx_noex { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_pr; return 1; } catch (...) { return -2; } }
s_long dispatcher_mt::thread_proxy::subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt, cref_t<tracking_info> tracking) __bmdx_noex
{
  try {
    address qsa, suba;
    s_long res;
    res = suba.set_addr_LP(_name_th, recv_sl); if (res != 1) { return res == -1 ? -1 : -2; }
    res = qsa.set_addr(addr_qs); if (res != 1) { return res == -1 ? -1 : -2; }
    if (!tracking)
    {
      return dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, rt, st_client, qsa, suba, -1, 0, 0);
    }
    else
    {
      tracking->id = idgen();
      tracking->state = -1; if (!tracking.is_cm()) { return -1; }
      tracking->state = -2; if (!_r_ths.has_ref()) { return -2; }
      dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
      tracking->state = -3; if (rses.ses_state != 1) { return -3; }
      tracking->state = -2;
      if (1)
      {
        refmaker_t<t_htracking_proxy>::t_lock __lock(this->mtrk_htracking); if (sizeof(__lock)) {}
        try {
          const t_htracking_proxy::entry* e = 0;
          if (this->mtrk_htracking.strongref->insert(tracking->id, &e) != 1) { return -2; }
          e->v = tracking;
        } catch (...) { return -2; }
      }
      res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, rt, st_client, qsa, suba, tracking->id, &this->mtrk_htracking, 0);
      if (1)
      {
        refmaker_t<t_htracking_proxy>::t_lock __lock(this->mtrk_htracking); if (sizeof(__lock)) {}
        if (res < 0 || res > tracking->state) { tracking->state = res; }
        if (res < 0 || qsa.isLP_any()) { try { this->mtrk_htracking.strongref->remove(tracking->id); } catch (...) {} }
      }
      return res;
    }
  } catch (...) { return -2; }
}


void dispatcher_mt::thread_proxy::th_lqsd_impl::_thread_proc()
{
  const meta::s_ll sleep1_max = 5000; // 5 ms
  const s_ll sleep_total_half_enabled = 100000; // 100 ms, until LQSD is enabled in full (rses.__thm_lqsd_enable_full())

  cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >(); if (!_r_ths) { return; } cch_session& rses = *_r_ths._pnonc_u();

  while (!b_stop())
  {
    s_ll nm = 0;
    try { dispatcher_mt::thread_proxy::_s_qs_deliver(_r_ths, std::wstring(), 2, &nm); } catch (...) {}
    if (nm > 0) { rses.__thm_lqsd_enable_full(); }

    s_ll sleep_total = rses.lqsd_dt;
    if (rses.__thm_lqsd < 2) { sleep_total = sleep_total_half_enabled; }
    double t0 = clock_ms();
    while (true)
    {
      if (rses.ses_state == 0 || rses.nprx <= 0) { goto lExit; }
      if (nm > 0) { break; }
      const s_ll dtmcs = bmdx_minmax::myllmin(sleep1_max, sleep_total - s_ll(1000 * (clock_ms() - t0)));
      if (dtmcs < 0) { break; }
      sleep_mcs(dtmcs, 1);
    }
  }
lExit: _r_ths.clear();
}
s_long dispatcher_mt::thread_proxy::th_lmsc_impl::local_write(t_stringref umsg, s_long enc, const cref_t<t_stringref>& bin, s_ll id_msg, s_ll id_msg_cmd)
{
  cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >(); if (!_r_ths) { return -2; }
  try {
  unity hmsg;
  if (enc == 2) { hmsg.pl_dech(bmdx_str::conv::bsws_utf8(umsg.pd(), umsg.n())); }
    else if (enc == 1) { hmsg.pl_dech(arrayref_t<wchar_t>((wchar_t*)umsg.pd(), umsg.n() / 2)); }
    else if (enc == 0) { std::wstring s; s.reserve(_t_wz(umsg.n())); for (s_ll i = 0; i < umsg.n(); ++i) { s += wchar_t(umsg[i]); } hmsg.pl_dech(s); }
    else { return -2; }
  s_long res = dispatcher_mt::thread_proxy::_s_write(_r_ths, std::wstring(), hmsg, bin, st_lmsc, _fl_msend_anlo_att, true, id_msg, id_msg_cmd, 0, 0);
    return res;
  } catch (...) {}
  return -2;
}
s_long dispatcher_mt::thread_proxy::th_lmsc_impl::local_subscribe(t_stringref mde, const netmsg_header::mdd_subs_request& dm, const bmdx_shm::t_name_shm& name_peerf)
{
  if (!(mde.is_valid() && dm.is_valid() && dm.pmsg == mde.pd())) { return -1; }
  try {
    address qsa; qsa.set_addr(bmdx_str::conv::bsws_utf8((char*)dm.pqsa_utf8(), dm.nb_qsa));
    address suba; suba.set_addr(bmdx_str::conv::bsws_utf8((char*)dm.psuba_utf8(), dm.nb_suba));
    if (!(qsa.isLM() && suba.isLM())) { return -1; }
    if (dm.rt >= 1 && dm.rt <= 3) { if (make_fixed_utf8_name(suba.wstr_pn()) != name_peerf) { return -1; } }
      else if (dm.rt >= 4 && dm.rt <= 6) { if (make_fixed_utf8_name(qsa.wstr_pn()) != name_peerf) { return -1; } }
    cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >(); if (!_r_ths) { return -2; }
    s_long res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, dm.rt, st_lmsc, qsa, suba, -1, 0, 0);
    return res;
  } catch (...) {}
  return -2;
}
s_long dispatcher_mt::thread_proxy::th_lmsc_impl::cslph_update(vec2_t<tracking_info>& tii)
{
  if (tii.empty()) { return 1; }
  cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >(); if (!_r_ths) { return -2; } cch_session& rses = *_r_ths._pnonc_u();
  vec2_t<cch_slot::csl_tracking_ext, __vecm_tu_selector> vtrk2;
  if (1)
  {
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_htrk_csl);
    for (s_long i = 0; i < tii.n(); ++ i)
    {
      const hashx<s_ll, cch_slot::csl_tracking>::entry* e = rses.htrk_csl.find(tii[i].id);
        if (!e) { continue; }
      try { vtrk2.resize(vtrk2.size() + 1); bmdx_str::words::swap_bytes(vtrk2.back().csl_info, e->v); vtrk2.back().trk_info = tii[i]; } catch (...) { return -2; }
    }
  }
  for (s_long i = 0; i < vtrk2.n(); ++ i)
  { try {
    cch_slot::csl_tracking_ext& tr = vtrk2[i];
    cref_t<cch_slot> rcmd_sl;
    if (1)
    {
      mst_semaphore ms_s(rses, tr.csl_info.thn.rstr()); hangdet hd(__lm_slot_controller_thread_hang_toms);
      int bf = 0; while (true) { if (hd) { bf = 2; break; } int res = ms_s.r_sl(tr.csl_info.sln_exact.rstr(), rcmd_sl); if (res < 0) { bf = 1; break; } if (res > 0) { break; } sleep_mcs(50); }
      if (bf == 2) { return -2; }
      if (bf == 1) { continue; }
    }
    cch_slot& cmd_sl = *rcmd_sl._pnonc_u();
    // lock the slot; hbo: get the sending entry; check if current id_msg of the sending entity is the same as in tr.csl_info; check entity phase; update entity phase and reset id_msg
    critsec_t<dispatcher_mt> __lock(10, -1, &cmd_sl.lkd); if (sizeof(__lock)) {}
    if (cmd_sl.eq_type("hbo"))
    {
      if (!(tr.csl_info.key_hbo.isNonempty() && cmd_sl.r_haddrl)) { continue; }
      const cch_slot::t_haddrl::entry* e = cmd_sl.r_haddrl->find(tr.csl_info.key_hbo);
        if (!e) { continue; }
      s_ll& cmd_ph = e->v.value;
      if (!(tr.trk_info.id == e->v.id_msg_cmd && cmd_ph == 7)) { continue; }
      if (tr.trk_info.state > 0) { cmd_ph = 4; }
        else { cmd_sl.r_haddrl->remove_e(e); }
    }
    else if (cmd_sl.eq_type("pbo"))
    {
      s_long& cmd_ph = cmd_sl.phase;
      if (!(tr.trk_info.id == cmd_sl.id_msg_cmd && cmd_ph == 7)) { continue; }
      if (tr.trk_info.state > 0) { cmd_ph = 4; }
        else { cmd_ph = 6; }
    }
  } catch (...) {} }
  return 1;
}

void dispatcher_mt::thread_proxy::th_lmsc_impl::_thread_proc()
{
  const double idle_toms = 3000; // timeout for entering sleepy state (sleep_total_long) due to inactivity
  const s_ll sleep_short = 50; // 50 mcs (until idle_toms passes in inactivity, then switches to sleep_total_long)
  const s_ll sleep1_long = 5000; // 5 ms (by this value, until sleep_total_long is accumulated)
  const s_ll sleep_total_long = 20000; // 20 ms
  const s_ll sleep_total_half_enabled = __lm_slot_controller_sleep_halfen_dtms * 1000; // until LMSC is enabled in full (rses.__thm_lmsc_enable_full()) - usually, on 1st incoming connection or outgoing message

  cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >(); if (!_r_ths) { return; } cch_session& rses = *_r_ths._pnonc_u();
  cref_t<lm_slot_controller> lmsc = rses.lmsc; if (!lmsc) { return; }

  double t_la = clock_ms(); // time of last activity
  while (!b_stop())
  {
    bool b_active = true;
    try {
      const s_long st = lmsc->det_state();
      if (st == 1) { b_active = false; lmsc->det_periodic(*this, b_active); }
        else if (st == 0 || st == -2) { lmsc->det_init(); }
    } catch (...) {}

    s_ll sleep_total = sleep_short; s_ll sleep1 = sleep_short;
    const bool b_halfen = rses.__thm_lmsc < 2;
    if (b_halfen) { t_la = clock_ms(); sleep_total = sleep_total_half_enabled; sleep1 = sleep1_long; }
    else
    {
      if (b_active) { t_la = clock_ms(); }
        else if (clock_ms() - t_la >= idle_toms) { sleep_total = sleep_total_long; sleep1 = sleep1_long; }
    }

    const double t0 = clock_ms();
    while (true)
    {
      if (b_stop() || rses.ses_state == 0 || rses.nprx <= 0) { goto lExit; }
      if (b_halfen)
      {
        if (!(rses.__thm_lmsc < 2)) { break; }
        if (lmsc->det_check_qinfo() >= 1) { rses.__thm_lmsc_enable_full(); break; }
      }
      const s_ll dtmcs = bmdx_minmax::myllmin(sleep1, sleep_total - s_ll(1000 * (clock_ms() - t0)));
        if (dtmcs < 0) { break; }
      sleep_mcs(dtmcs, sleep_total >= sleep1_long ? 1 : 0);
    }
  }
lExit:
  lmsc->det_close(); // this may delay up to approx. __bmdx_disp_lq_cleanup_dtms
  lmsc.clear(); _r_ths.clear();
}

  // Append values from array src to array dest.
  //  If dest is not utUnityArray or utStringArray, it's cleared and initialized as 1-based empty utUnityArray.
  //  If src as not an array, it's treated as single element.
  // n_skip: number of elements from the beginning src to skip (not append to dest).
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
  //  pdtailcmp != 0: destination address, to compare with (source) sn.
  //    For this check being OK, one of the following must be true:
  //      a) no tails in both sn, *pdtailcmp,
  //      b) (sn slot type is not pbi or qbi) sn has no tail, *pdtailcmp has tail,
  //      c) (sn slot type is pbi or qbi) sn has tail, *pdtailcmp has no tail,
  //      d) both sn and *pdtailcmp have equal tails.
  // Returns:
  //    1 and recoded slot name, if sn is valid.
  //    -1:
  //      a) if the name is not valid. *pretval == const, *pn1, *pntail are undefined.
  //      b) sn tail is != that of *pdtailcmp.
  //    -2 - failure. *pretval == const, *pn1, *pntail are undefined.
s_long dispatcher_mt::thread_proxy::__recode_slotname(const unity& sn, std::wstring* pretval, std::wstring* pn1, std::wstring* pntail, const address* pdtailcmp) __bmdx_noex
{
  try {
    unity _a; const unity* pa(0);
    if (sn.isString()) { paramline().decode1v(sn.vstr(), _a); pa = &_a; }
      else { pa = &sn; }
    if (!(pa->isString() || ((pa->utype() == utUnityArray || pa->utype() == utStringArray) && pa->arrlb() == 1 && pa->arrsz() >= 2))) { return -1; }
    std::wstring sln1 = pa->isArray() ? pa->vstr(1) : pa->vstr();
    if (!sln1chk_main(sln1)) { return -1; }
    if (pdtailcmp)
    {
      const bool b_dest_tail = pdtailcmp->has_sln_tail();
      const bool b_backw = sln1chk_main_bi(sln1); // backward direction (command target responds to command source)
      if (!b_backw && pa->isArray() && !b_dest_tail) { return -1; }
      if (pa->isArray() && b_dest_tail)
      {
        const s_long itail1 = 2;
        const s_long n = pa->arrsz() - 1;
        const unity& a2 = pdtailcmp->addr();
        const s_long itail2 = pdtailcmp->sln_ind() + 1;
        if (!(itail2 + n - 1 == a2.arrub())) { return -1; }
        if (pa->utype() == utUnityArray) { for (s_long i = 0; i < n; ++i) { const unity& v2 = a2[itail2 + i]; if (!((*pa)[itail1 + i] == v2)) { return -1; } } }
          else { for (s_long i = 0; i < n; ++i) { const unity& v2 = a2[itail2 + i]; if (!(v2.isString() && pa->rstr(itail1 + i) == v2.rstr())) { return -1; } } }
      }
    }
    if (pn1) { pn1->swap(sln1); }
    if (pntail)
    {
      if (pa->isArray()) { unity x; __append_vals(x, *pa, 1); *pntail = paramline().encode1v(x); }
      else { pntail->clear(); }
    }
    if (pretval) { std::wstring s(paramline().encode1v(*pa)); pretval->swap(s); }
    return 1;
  } catch (...) { return -2; }
}

  // Empty, or valid address array (1-based, utUnityArray, containing only strings).
const unity& dispatcher_mt::thread_proxy::address::addr() const __bmdx_noex { return _addr; }

bool dispatcher_mt::thread_proxy::address::is_empty() const __bmdx_noex { return _addr.isEmpty(); }
s_long dispatcher_mt::thread_proxy::address::n() const __bmdx_noex { return _addr.isEmpty() ? 0 : _addr.arrsz(); }

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
  std::wstring s; if (_addr.isEmpty()) { return s; }
  const s_long ind = sln_ind(); if (ind < 1 || ind + 1 > _addr.arrub()) { return s; }
  unity _a; __append_vals(_a, _addr, ind + 1 - _addr.arrlb());
  paramline().encode1v(_a, s);
  return s;
}
bool dispatcher_mt::thread_proxy::address::has_sln_tail() const __bmdx_noex
{
  const s_long ind = sln_ind();
  return ind >= 2 && ind < _addr.arrub();
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
  const _fls75 k1 = _addr[1].vflstr();
  s_long ind = -1;
  if (k1 == L"LM") { ind = 2; }
    else if (k1 == L"R") { ind = 3; }
  if (ind >= 0) { s = _addr.vstr(ind); }
  return s;
}

  // Returns string representation of host name part of the remote address,
  //    or empty string if addr() does not contain host name.
//std::wstring dispatcher_mt::thread_proxy::address::wstr_hostn() const
//{
//  std::wstring s; if (_addr.isEmpty()) { return s; }
//  const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
//  if (k1.length() > 0 && k1[0] == L'R') { s = _addr.vstr(2); }
//  return s;
//}

  // x may be string or already decoded array.
  // Returns:
  //    1 if set decoded, corrected address.
  //    -1 if x is not a valid address. addr() is cleared.
  //    -2 if setting failed. addr() is cleared.
s_long dispatcher_mt::thread_proxy::address::set_addr(const unity& x) __bmdx_noex
{
  _addr.clear();
  try {
    unity _a; const unity* pa(0); if (x.isString()) { paramline().decode1v(x.vstr(), _a); pa = &_a; } else { pa = &x; }
    if (!(pa->isArray() && pa->arrsz() >= 2)) { return -1; }
    s_long nb = pa->arrlb();
    _fls75 k1; if (pa->utype() == utStringArray) { k1 = pa->vstr(1); } else { k1 = (*pa)[nb + 0].vflstr(); }
    const s_long npmin = _sln_ind0(k1);
    if (npmin < 2 || pa->arrsz() < npmin) { return -1; }
    unity aprep; aprep.arr_init<utUnity>(1);
    if (pa->utype() == utStringArray) { for (s_long i = 0; i < pa->arrsz(); ++i) { aprep.arr_append(pa->vstr(nb + i)); } }
    else
    {
      for (s_long i = 0; i < npmin; ++i) { aprep.arr_append((*pa)[nb + i].vstr()); }
      for (s_long i = npmin; i < pa->arrsz(); ++i) { const unity& x = (*pa)[nb + i]; if (x.isString() || x.isInt() || x.isEmpty()) { aprep.arr_append(x); } else { aprep.arr_append(x.vstr()); } }
    }
    if (k1 == L"LP") { if (!thnchk(aprep.rstr(2)) || !sln1chk_main(aprep.rstr(3))) { return -1; } } // |LP|thread|slot
      else if (k1 == L"LPA") { if (!sln1chk_main_qs(aprep.rstr(2))) { return -1; } } // |LPA|slot
      else if (k1 == L"LM") { if (!pnchk(aprep.rstr(2)) || !thnchk(aprep.rstr(3)) || !sln1chk_main(aprep.rstr(4))) { return -1; } } // |LM|process|thread|slot
      else if (k1 == L"LMA") { if (!thnchk(aprep.rstr(2)) || !sln1chk_main_qs(aprep.rstr(3))) { return -1; } } // |LMA|thread|slot
      else if (k1 == L"R") { if (!hostportchk(aprep.rstr(2)) || !pnchk(aprep.rstr(3)) || !thnchk(aprep.rstr(4)) || !sln1chk_main(aprep.rstr(5))) { return -1; } } // |R|remote_address|process|thread|slot
      else if (k1 == L"RMA") { if (!hostportchk(aprep.rstr(2)) || !thnchk(aprep.rstr(3)) || !sln1chk_main_qs(aprep.rstr(4))) { return -1; } }  // |RMA|remote_address|thread|slot
      else { return -1; }
    _addr.swap(aprep);
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

bool dispatcher_mt::thread_proxy::address::isLP_any() const __bmdx_noex { return isLP() || isLPA(); }
bool dispatcher_mt::thread_proxy::address:: isLP() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LP"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLPA() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LPA"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLM() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LM"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isLMA() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LMA"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isR() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"R"; } catch (...) {} return false; }
bool dispatcher_mt::thread_proxy::address:: isRMA() const __bmdx_noex { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"RMA"; } catch (...) {} return false; }

  // Returns base index of slot name in addr():
  //  >= 2 on success.
  //  -1 if addr() is empty.
  //  -2 on failure.
s_long dispatcher_mt::thread_proxy::address::sln_ind() const __bmdx_noex
{
  try { return _sln_ind0(_addr[1].vflstr()); } catch (...) {}
  return -2;
}
s_long dispatcher_mt::thread_proxy::address::_sln_ind0(const _fls75& k1) const __bmdx_noex
{
  s_long ind = -2;
  if (k1 == L"LP") { ind = 3; } // |LP|thread|slot
    else if (k1 == L"LPA") { ind = 2; } // |LPA|slot
    else if (k1 == L"LM") { ind = 4; } // |LM|process|thread|slot
    else if (k1 == L"LMA") { ind = 3; } // |LMA|thread|slot
    else if (k1 == L"R") { ind = 5; } // |R|remote_address|process|thread|slot
    else if (k1 == L"RMA") { ind = 4; } // |RMA|remote_address|thread|slot
  return ind;
}

  // Returns slot type part,
  //  or empty string if addr() is empty or on failure.
_fls75 dispatcher_mt::thread_proxy::address::slt() const __bmdx_noex
{
  try {
    s_long ind = sln_ind(); if (ind < 1) { return _fls75(); }
    _fls75 sln1 = _addr.ref<utUnity>(ind).vflstr();
    return sln1.substr(0, sln1.find1('_'));
  } catch (...) {}
  return _fls75();
}

bool dispatcher_mt::thread_proxy::address::_conv_LP_LM(arrayref_t<wchar_t> name_peer) __bmdx_noex
{
  try {
    if (!isLP()) { return false; }
    _addr.arr_append(unity());
    for (s_long i = _addr.arrub() - 1; i >= 2; --i) { _addr[i].swap(_addr[i + 1]); }
    _addr[1] = L"LM";
    _addr[2] = name_peer;
    return true;
  } catch (...) {}
  return false;
}
bool dispatcher_mt::thread_proxy::address::_conv_LM_LP() __bmdx_noex
{
  try {
    if (!isLM()) { return false; }
    _addr[1] = L"LP";
    if (_addr.arrub() >= 2) { _addr.arr_insrem(2, -1); }
    return true;
  } catch (...) {}
  return false;
}


bool dispatcher_mt::thread_proxy::thnchk(arrayref_t<wchar_t> s) __bmdx_noex { return s.is_valid() && !(s.n() == 0 || (s.n() >= 2 && s[0] == L'_' && s[1] == L'_')); }
bool dispatcher_mt::thread_proxy::pnchk(arrayref_t<wchar_t> s) __bmdx_noex { return s.is_valid() && s.n() > 0 && !is_addr_cat_all(s); }
bool dispatcher_mt::thread_proxy::hostportchk(const std::wstring& s) __bmdx_noex { return s.length() != 0 && !is_addr_cat_all(s); }
bool dispatcher_mt::thread_proxy::is_addr_cat_all(arrayref_t<wchar_t> s) __bmdx_noex
{
  if (!(s.is_valid() && s.n() > 0)) { return false; }
  if (s[0] == L'L')
  {
    switch (s.n())
    {
      case 2: return s[1] == L'P' || s[1] == L'M';
      case 3: return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
      default: return false;
    }
  }
  else if (s[0] == L'R')
  {
    if (s.n() == 1) { return true; }
    if (s.n() != 3) { return false; }
    return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
  }
  return false; // required (compiler bug fix)
}

std::wstring dispatcher_mt::thread_proxy::sln1_root(const std::wstring& sln1)
{
  _t_wz i = sln1.find(L'_');
  return i == nposw ? std::wstring() : sln1.substr(i+1);
}

bool dispatcher_mt::thread_proxy::sln1chk_main(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b') // ?b??*
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; } // ?b?_?*
    if (sln1[0] == L'p') { return sln1[2] == L'o' || sln1[2] == L'i'; } // pbo, pbi
      else if (sln1[0] == L'q') { return sln1[2] == L'i'; } // qbi
      else if (sln1[0] == L'h') { return sln1[2] == L'o'; } // hbo
  }
  else
  {
    if (sln1[2] != L'_') { return false; } // ??_?*
    if (sln1[0] == L'p') { return sln1[1] == L'o' || sln1[1] == L'i'; } // po, pi
      else if (sln1[0] == L'q') { return sln1[1] == L'i' || sln1[1] == L's'; } // qi, qs
  }
  return false;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_qs(const std::wstring& sln1) { return sln1chk_main(sln1) && sln1[0] == L'q' && sln1[1] == L's'; }
bool dispatcher_mt::thread_proxy::sln1chk_qs(const std::wstring& sln1) { return sln1.length() >= 4 && sln1[0] == L'q' && sln1[1] == L's' && sln1[2] == L'_'; }
bool dispatcher_mt::thread_proxy::sln1chk_subscriber(const std::wstring& sln1) __bmdx_noex { return sln1.length() >= 4 && (sln1[0] == L'p' || sln1[0] == L'q') && sln1[1] == L'i' && sln1[2] == L'_'; }
bool dispatcher_mt::thread_proxy::sln1chk_main_o(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b') // ?b??*
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; } // ?b?_?*
    if (sln1[0] == L'p') { return sln1[2] == L'o' || sln1[2] == L'i'; } // pbo, pbi
      else if (sln1[0] == L'q') { return sln1[2] == L'i'; } // qbi
      else if (sln1[0] == L'h') { return sln1[2] == L'o'; } // hbo
  }
  else
  {
    if (sln1[2] != L'_') { return false; } // ??_?*
    if (sln1[0] == L'p') { return sln1[1] == L'o'; } // po
      else if (sln1[0] == L'q') { return sln1[1] == L's'; } // qs
  }
  return false;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_i(const std::wstring& sln1)
{
  if (sln1.length() < 4) { return false; }
  if (sln1[1] == L'b') // ?b??*
  {
    if (sln1.length() < 5 || sln1[3] != L'_') { return false; } // ?b?_?*
    if (sln1[0] == L'p') { return sln1[2] == L'o' || sln1[2] == L'i'; } // pbo, pbi
      else if (sln1[0] == L'q') { return sln1[2] == L'i'; } // qbi
      else if (sln1[0] == L'h') { return sln1[2] == L'o'; } // hbo
  }
  else
  {
    if (sln1[2] != L'_') { return false; } // ??_?*
    if (sln1[0] == L'p') { return sln1[1] == L'i'; } // pi
      else if (sln1[0] == L'q') { return sln1[1] == L'i' || sln1[1] == L's'; } // qi, qs
  }
  return false;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_bo(const std::wstring& sln1)
{
  if (sln1.length() < 5) { return false; }
  wchar_t c;
    c = sln1[0]; if (!(c == L'p' || c == L'h')) { return false; }
    c = sln1[1]; if (!(c == L'b')) { return false; }
    c = sln1[2]; if (!(c == L'o')) { return false; }
  return true;
}
bool dispatcher_mt::thread_proxy::sln1chk_main_bi(const std::wstring& sln1)
{
  if (sln1.length() < 5) { return false; }
  wchar_t c;
    c = sln1[0]; if (!(c == L'p' || c == L'q')) { return false; }
    c = sln1[1]; if (!(c == L'b')) { return false; }
    c = sln1[2]; if (!(c == L'i')) { return false; }
  return true;
}
bool dispatcher_mt::thread_proxy::sln1chk_iomatch(const std::wstring& ssln1, const std::wstring& dsln1)
{
  if (ssln1.length() < 4 || dsln1.length() < 4) { return false; }
  if (ssln1[1] == L'b')
  {
    if (ssln1.length() < 5 || dsln1.length() < 5 || ssln1[3] != L'_' || dsln1[3] != L'_') { return false; }
    if (ssln1[0] == L'p')
    {
      if (ssln1[2] == L'o') { return (dsln1[0] == L'p' || dsln1[0] == L'q') && dsln1[2] == L'i'; } // command pbo --> pbi, qbi
        else if (ssln1[2] == L'i') { return (dsln1[0] == L'p' || dsln1[0] == L'h') && dsln1[2] == L'o'; } // response pbi --> pbo, hbo
    }
    else if (ssln1[0] == L'q')
    {
      if (ssln1[2] == L'i') { return (dsln1[0] == L'p' || dsln1[0] == L'h') && dsln1[2] == L'o'; } // response qbi --> pbo, hbo
    }
    else if (ssln1[0] == L'h')
    {
      if (ssln1[2] == L'o') { return (dsln1[0] == L'p' || dsln1[0] == L'q') && dsln1[2] == L'i'; } // command hbo --> pbi, qbi
    }
  }
  else
  {
    if (ssln1[2] != L'_') { return false; }
    if (ssln1[0] == L'p')
    {
      if (!(ssln1[1] == L'o')) { return false; }
      if (dsln1[0] == L'p') { return dsln1[1] == L'i'; } // po --> pi
      if (dsln1[0] == L'q') { return dsln1[1] == L'i' || dsln1[1] == L's'; } // po --> qi, qs
    }
    else if (ssln1[0] == L'q')
    {
      if (!(ssln1[1] == L's' && dsln1[1] == L'i')) { return false; }
      return dsln1[0] == L'p' || dsln1[0] == L'q'; // qs --> pi, qi
    }
  }
  return false;
}





s_long dispatcher_mt::thread_proxy::_s_pop(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& slotname, unity& retmsg, cref_t<t_stringref>* retatt, s_long flags_mget) __bmdx_noex
{
  if (!retmsg.isEmpty()) { retmsg.clear(); }
  if (retatt && *retatt) {}
  if (!_r_ths) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }
  const s_long flags_ses = rses.frqperm;
  try {

      //  Det. exact slot name.
    std::wstring ssln1, ssln_exact;
    s_long res = __recode_slotname(slotname, &ssln_exact, &ssln1);
      if (res != 1) { return res == -1 ? -1 : -2; }

      // Check if slot exists / get ref.
    cref_t<cch_slot> ri_sl;
    if (1)
    {
      mst_semaphore ms(rses, _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(ssln_exact, ri_sl); if (res < 0) { if (!ms.p_thread()) { return -7; } return -6; } if (res > 0) { break; } sleep_mcs(50); }
    }
    cch_slot& i_sl = *ri_sl._pnonc_u();

      // Check if message exists, get message, set slot phase as necessary for slot type.
    if (ssln1[1] == L'i') // pi, qi
    {
      if (ssln1[0] == L'p')
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase != 1) { return 0; }

        s_long res = i_sl.r_pin.move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
        if (!(flags_mget & _fl_mget_peek_only) && res != 0) { i_sl.r_pin.clear(); i_sl.phase = 2; }
        return res;
      }
      else if (ssln1[0] == L'q')
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.r_qumi->navl() == 0) { return 0; }

        s_long res = 0;
        while (1)
        {
          res = i_sl.r_qumi->front().move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
          if (res == 0)
          {
            i_sl.r_qumi->pop_1();
            if (i_sl.r_qumi->navl() == 0) { return 0; }
            continue;
          }
          break;
        }
        if (!(flags_mget & _fl_mget_peek_only) && res != 0) { i_sl.r_qumi->pop_1(); }
        return res;
      }
      else { return -1; }
    }
    else if (ssln1[1] == L'b') // pbo, hbo, pbi, qbi
    {
      if (ssln1[2] == L'o') // pbo, hbo
      {
        if (ssln1[0] == L'h') // hbo (pop a response from queue)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          if (i_sl.r_qubci->navl() <= 0)
          {
            if (i_sl.r_haddrl->n() > 0) { return -20; } // something sent - no replies at all yet
            return 0; // nothing sent currently - nothing to wait
          }

          cref_t<cch_slot::qbci_value, cref_nonlock>& rz = i_sl.r_qubci->front();
          const cch_slot::t_haddrl::entry* e = i_sl.r_haddrl->find(rz->src.ref());
          if (!e)  { i_sl.r_qubci->pop_1(); return -2;  } // broken constraint, not expected to occur
          s_ll ph = e->v.value;
          if (!(ph == 5 || ph == 7)) { i_sl.r_qubci->pop_1(); i_sl.r_haddrl->remove_e(e); return -2; } // broken constraint, not expected to occur

          s_long res = rz->msg.move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
          if (res == 0) { i_sl.r_qubci->pop_1(); return -20; } // invalid message on the queue, not expected to occur
          if (!(flags_mget & _fl_mget_peek_only) && res != 0) { i_sl.r_qubci->pop_1(); i_sl.r_haddrl->remove_e(e); }
          return res;
        }
        else if (ssln1[0] == L'p') // pbo (get a response)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          s_long ph = i_sl.phase;
          if (ph == 4) { return -20; }
          if (!(ph == 5 || ph == 7)) { return 0; }

          s_long res = i_sl.r_pin.move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
          if (res == 0) { return -20; } // no response yet
          if (!(flags_mget & _fl_mget_peek_only) && res != 0) { i_sl.r_pin.clear(); i_sl.id_msg_cmd = -1; i_sl.phase = 6; }
          return res;
        }
      }
      else if (ssln1[0] == L'p' && ssln1[2] == L'i') // pbi (get a command)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase == 2) { return -21; }
        if (i_sl.phase != 1) { return 0; }

        s_long res = i_sl.r_pin.move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
        if (!(flags_mget & _fl_mget_peek_only) && res != 0) { i_sl.r_pin.clear(); i_sl.phase = 2; }
        return res;
      }
      else if (ssln1[0] == L'q' && ssln1[2] == L'i') // qbi (pop a command from queue)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
        if (i_sl.phase == 2) { return -21; }
        if (i_sl.r_qubci->navl() == 0) { return 0; }

        s_long res = 0;
        while (1)
        {
          res = i_sl.r_qubci->front()->msg.move_to_retvals(retmsg, retatt, flags_mget, flags_ses); if (res < 0) { return -2; }
          if (res == 0)
          {
            i_sl.r_qubci->pop_1();
            if (i_sl.r_qubci->navl() == 0) { return 0; }
            continue;
          }
          break;
        }
        if (!(flags_mget & _fl_mget_peek_only) && res != 0)  { i_sl.r_lsrc = i_sl.r_qubci->front()->src; i_sl.id_msg_cmd = i_sl.r_qubci->front()->id_msg_cmd; i_sl.r_qubci->pop_1(); i_sl.phase = 2; }
        return res;
      }
      return -1;
    }
    return -1; // po, qo, qs
  } catch (...) { return -2; }
}

  // _name_th: name of the thread, calling _s_write (should be same as thread name in source address (src = ...), if it's specified)
  // msg: the message to send in form: a) string, b) map, c) hashlist.
  //  Must have "src", "trg", "text" fields.
  //  During processing, "src" and "trg" fields are modified, to reflect the receiving side context.
  // sender_type:
  //    st_client: thread_proxy,
  //      _name_th: client dispatcher's thread name
  //      id_msg_nl: -1
  //      id_msg_orig:
  //        a) id of the current message, if tracking is enabled for it,
  //        b) -1 if tracking is disabled.
  //      prhtrprx (st_client only): ptr. to client proxy's mtrk_htracking object; must be !=0 if dest. addr. is non-local, + must remain valid during _s_write call.
  //    st_s_subs_deliver: _s_subs_deliver(),
  //      _name_th: qs slot owner thread name
  //      id_msg_nl: -1
  //      id_msg_orig = -1.
  //      prhtrprx = 0.
  //    st_lmsc: LMSC.
  //      _name_th: empty ("")
  //      id_msg_nl: the current message ID, generated by non-local sender; used only by pbi, qbi slots, to later send response to the command along with original ID
  //      id_msg_orig:
  //          a) id of the original command message, associated with the current response message, passed into _s_write; not for tracking, only for detecting "out of order" error,
  //          b) -1 for other types of messages, written by LMSC
  //      prhtrprx = 0.
  // pb_da_is_local: may be 0; if != 0, receives da.isLP_any(), where da is destination address, taken from msg.
  // NOTE _s_write is called multiple times for one message from _s_subs_deliver,
  //  each time with different "trg" and re-set "src".
s_long dispatcher_mt::thread_proxy::_s_write(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& msg, const cref_t<t_stringref>& att, Esender_type sender_type, const s_long flags_msend, bool b_may_swap_msg, s_ll id_msg_nl, s_ll id_msg_orig, refmaker_t<t_htracking_proxy>* prhtrprx, bool* pb_da_is_local) __bmdx_noex
{
  if (!_r_ths.has_ref()) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }
  const s_long flags_ses = rses.frqperm | (sender_type == st_s_subs_deliver ? _frqperm_msend_anlo | _frqperm_mget_anlo : 0); // delivery occurs already inside the dispatcher => the global anlo flags must be ignored
  try {
    unity hm1; // message (in the form of hashlist) that will really be sent; values are kept as ptrs. to subobjects, to minimize copying
    if (msg.isHash()) { if (b_may_swap_msg) { hm1.swap((unity&)msg); } else { hm1 = msg; } }
      else if (msg.isMap()) { hm1.u_clear(utHash); paramline().merge(hm1, msg); }
      else if (msg.isString()) { paramline().decode(msg.cref<utString>().ref(), hm1, false); }
      else { return -1; }
    const unity k_src(L"src"), k_trg(L"trg"), k_text(L"text");
    unity* __psrc0 = 0; const unity *__ptrg = 0, *__ptext = 0;
    try { __psrc0 = &hm1[k_src]; __ptrg = &hm1[k_trg]; __ptext = &hm1[k_text]; if (sizeof(__ptext)) {}  } catch (...) { return -1; } // "text" key is only checked for existence
    unity& __src = *__psrc0;
    s_long res;

    address sa;
    address da; res = da.set_addr(*__ptrg); if (res != 1) { return res == -1 ? -1 : -2; }
    if (pb_da_is_local) { *pb_da_is_local = da.isLP_any(); }

      // Source address preparation.
    if (sender_type == st_client)
    {
      // Expected __src format:
      //      <slot name>
      //    (because that local thread sends a message from itself.)
      //    Before encoding, the address is completed: |LP|<thread name>|<slot name>.
      // Expected *__ptrg format:
      //      |LP|<thread name>|<slot name>
      //      |LPA|<qs slot name>
      //      |LM|<peer process name>|<thread name>|<slot name>
      unity a;
      __append_vals(a, L"LP");
      __append_vals(a, _name_th);
      __append_vals(a, __src);
      sa.set_addr(a);
        if (sa.is_empty()) { return -1; }
      __src = sa.addr();
    }
    else if (sender_type == st_s_subs_deliver)
    {
      // Expected __src format:
      //      |LP|<thread name>|<qs slot name>
      //      -- thread's request(rt = 8, unity(), 1) --> _s_qs_deliver() sends a message to a subscriber.
      //      Thread name must be same as _name_th.
      //      Source qs slot existence is checked, but the slot itself is not accessed, because this is done already by _s_qs_deliver.
      // Expected *__ptrg format:
      //      |LP|<thread name>|<slot name>
      //      |LPA|<qs slot name>
      //      |LM|<peer process name>|<thread name>|<slot name>
      sa.set_addr(__src);
          // For delivery, source address should be that of sourcing qs slot, belonging to _name_th (in the current process).
        if (sa.isLP()) { if (sa.wstr_thn() != _name_th) { return -1; } }
          else if (sa.is_empty() || sa.isLPA()) { return -1; }
          else { return -1; }
    }
    else if (sender_type == st_lmsc)
    {
      // Expected __src format (pre-set by sender's _s_write in peer process):
      //      |LM|<sender process name>|<sender thread name>|<sender slot name>
      // Expected *__ptrg format is that specified by original sender:
      //      |LM|<recipient process name>|<recipient thread name>|<recipient slot name>
      // NOTE The original sender knows its process name in full (not a hash of the name),
      //    and converts __src to to reflect backward address from the recipient's perspective,
      //    i.e. the process, in which the current _s_write is called.
      sa.set_addr(__src);
        if (sa.is_empty()) { return -1; }
      if (!sa.isLM()) { return -2; }
    }
    else { return -2; }


    if (da.isLP_any())
    {
      if (sender_type <= st_s_subs_deliver)
      {
        std::wstring ssln_exact, ssln1;
        res = __recode_slotname(sa.sln_v(), &ssln_exact, &ssln1, 0, &da); // (in _s_write)
          if (res != 1) { return -1; }
          if (!sln1chk_main_o(ssln1)) { return -1; }

        std::wstring dthn;
          // Det. dest. slot type and exact name.
          // Check source / dest. type match.
        std::wstring dsln_exact = da.wstr_sln();
        std::wstring dsln1 = da.wstr_sln_1();
          if (!sln1chk_main_i(dsln1)) { return -1; }
          if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
          if (!sln1chk_iomatch(ssln1, dsln1)) { return -1; }

          // Get slot refs.
        cref_t<cch_slot> rs_sl, rd_sl;
        if (1)
        {
          if (da.isLP()) { dthn = da.wstr_thn(); }
          else // LPA
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
            const hashx<unity, unity>::entry* e_sl2th = rses.hg_lpai.find(dsln_exact);
              if (!e_sl2th) { return -8; }
            dthn = e_sl2th->v.vstr();
          }
          mst_semaphore ms_d(rses, dthn), ms_s(rses, _name_th);
          hangdet hd;
          while (true)
          {
            if (hd) { return -2; }
            int res1 = 1, res2 = 1;
            if (!rs_sl) { res2 = ms_s.r_sl(ssln_exact, rs_sl); if (res2 < 0) { if (!ms_s.p_thread()) { return -7; } return -6; } }
            if (!rd_sl) { res1 = ms_d.r_sl(dsln_exact, rd_sl); if (res1 < 0) { if (!ms_d.p_thread()) { return -9; } return -8; } }
            if (res1 > 0 && res2 > 0) { break; }
            sleep_mcs(50);
          }
        }
        cch_slot& d_sl = *rd_sl._pnonc_u();
        cch_slot& s_sl = *rs_sl._pnonc_u();

        if (dsln1[1] == L'i' || dsln1[1] == L's' || (dsln1[1] == L'b' && dsln1[2] == L'i')) // all normal inputs: qs, pi, qi, pbi, qbi
        {
          if (da.isLPA())
          {
            if (!d_sl.b_input_lpa()) { return -10; }
          }
          else if (!d_sl.b_input_any_th()) { if (!(_name_th == dthn)) { return -10; } }
        }

          // Check phases, recode message, write dest. slot, update phases.
        if (dsln1[1] == L'i') // dest.: pi, qi
        {
          if (dsln1[0] == L'p') // msg --> pi
          {
            inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            if (sender_type == st_s_subs_deliver)
            {
              if (!(d_sl.r_haddrl && d_sl.r_haddrl->find(sa.wstr_addr()))) { return -2; }
            }
            bool b_over = d_sl.phase == 1; // the currently holds the previous message
            d_sl.r_pin = msg2; msg2.clear();
            d_sl.phase = 1;
            return b_over ? 0 : 1;
          }
          else if (dsln1[0] == L'q') // msg --> qi
          {
            inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

            if (1)
            {
              critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
              if (sender_type == st_s_subs_deliver)
              {
                if (!(d_sl.r_haddrl && d_sl.r_haddrl->find(sa.wstr_addr()))) { return -2; }
              }
              s_long res = d_sl.r_qumi->push_1(msg2);
              msg2.clear();
                if (res == -1) { return -20; }
                if (res != 1) { return -2; }
            }
            return 1;
          }
          else { return -2; }
        }
        else if (dsln1[1] == L'b') // dest.: pbo, hbo, pbi, qbi
        {
          inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }


          if (dsln1[0] == L'p' && dsln1[2] == L'o') // local response (pbi, qbi) --> pbo
          {
            const std::wstring cmdsrc(da.wstr_addr());

              // Reverse order of locks for pbo == forward order of locks for pbi and qbi.
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            s_long& d_ph = d_sl.phase;
            s_long& s_ph = s_sl.phase;

            if (!(s_ph == 2)) { return -5; } // out of order
            if (cmdsrc != s_sl.r_lsrc.ref()) { return -1; }
            if (!(d_ph == 4 && s_sl.id_msg_cmd == d_sl.id_msg_cmd)) { return -5; } // out of order
            d_sl.r_pin = msg2; msg2.clear(); d_ph = 5; // clearing non-locking object before local locks are removed
            s_sl.r_lsrc.clear(); s_sl.id_msg_cmd = -1; s_ph = 3;
            return 1;
          }
          else if (dsln1[0] == L'h' && dsln1[2] == L'o') // local response (pbi, qbi) --> hbo
          {
            const std::wstring cmdsrc(da.wstr_addr());

              // Reverse order of locks for hbo == forward order of locks for pbi and qbi.
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            s_long& s_ph = s_sl.phase;

            if (!(s_ph == 2)) { return -5; } // out of order
            if (cmdsrc != s_sl.r_lsrc.ref()) { return -1; }

            cref_t<cch_slot::qbci_value, cref_nonlock> rz;
              if (!rz.create0(true)) { return -2; }
              if (!rz->src.copy(sa.wstr_addr(), true)) { return -2; }
                rz->msg = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
            const cch_slot::t_haddrl::entry* e = d_sl.r_haddrl->find(rz->src.ref());
            if (!(e && e->v.value == 4 && s_sl.id_msg_cmd == e->v.id_msg_cmd)) { return -5; } // out of order

            s_long res  = d_sl.r_qubci->push_1(rz); rz.clear();
              if (res == -1) { return -20; }
              if (res != 1) { return -2; }

            e->v.value = 5;
            s_sl.r_lsrc.clear(); s_sl.id_msg_cmd = -1; s_ph = 3;
            return 1;
          }
          else if (dsln1[0] == L'p' && dsln1[2] == L'i') // local command --> pbi
          {
            s_ll id_msg = -1; if (id_msg_orig != -1) { id_msg = id_msg_orig; } else { id_msg = idgen(); }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            cref_t<std::wstring, cref_nonlock> cmdsrc; if (!cmdsrc.create1(1, sa.wstr_addr())) { return -2; }
            s_long& d_ph = d_sl.phase;

            if (ssln1[0] == L'h') // hbo --> pbi
            {
              const cch_slot::t_haddrl::entry* e = 0;
              unity da_u = da.wstr_addr();
              s_long res = s_sl.r_haddrl->insert(da_u, &e);
                if (res == 0)  { return -5; } // out of order
                if (res != 1) { return -2; }
                // NOTE s_sl.r_haddrl is modified before d_ph check, because out-of-order error in source slot has higher priority
                //  than pin is busy error in target slot.
              if (!(d_ph == 0 || d_ph == 3)) { s_sl.r_haddrl->remove(da_u); return -20; } // cmd. target pin is busy

              d_sl.r_pin = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
              d_sl.r_lsrc = cmdsrc; d_sl.id_msg_cmd = id_msg; d_ph = 1;
              e->v.id_msg_cmd = id_msg; e->v.value = 4;
              return 1;
            }
            else if (ssln1[0] == L'p') // pbo --> pbi
            {
              s_long& s_ph = s_sl.phase;
              if (!(s_ph == 0 || s_ph == 6)) { return -5; } // out of order
              if (!(d_ph == 0 || d_ph == 3)) { return -20; } // cmd. target pin is busy
              d_sl.r_pin = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
              d_sl.r_lsrc = cmdsrc; d_sl.id_msg_cmd = id_msg; d_ph = 1;
              s_sl.id_msg_cmd = id_msg; s_ph = 4;
              return 1;
            }
          }
          else if (dsln1[0] == L'q' && dsln1[2] == L'i') // local command  --> qbi
          {
            s_ll id_msg = -1; if (id_msg_orig != -1) { id_msg = id_msg_orig; } else { id_msg = idgen(); }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            cref_t<std::wstring, cref_nonlock> cmdsrc; if (!cmdsrc.create1(1, sa.wstr_addr())) { return -2; }

            if (ssln1[0] == L'h') // hbo --> qbi
            {
              cref_t<cch_slot::qbci_value, cref_nonlock> rz;
                if (!rz.create0(true)) { return -2; }
                rz->msg = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
                rz->src = cmdsrc;
                rz->id_msg_cmd = id_msg;

              const cch_slot::t_haddrl::entry* e = 0;
              s_long res = s_sl.r_haddrl->insert(da.wstr_addr(), &e);
                if (res == 0) { return -5; } // out of order
                if (res != 1) { return -2; }

              res = d_sl.r_qubci->push_1(rz); rz.clear();
                if (res == -1) { s_sl.r_haddrl->remove_e(e); return -20; } // queue overflow
                if (res != 1) { s_sl.r_haddrl->remove_e(e); return -2; }

              e->v.id_msg_cmd = id_msg; e->v.value = 4;
              return 1;
            }
            else if (ssln1[0] == L'p') // pbo --> qbi
            {
              s_long& s_ph = s_sl.phase;
              if (!(s_ph == 0 || s_ph == 6)) { return -5; } // out of order
              cref_t<cch_slot::qbci_value, cref_nonlock> rz;
                if (!rz.create0(true)) { return -2; }
                rz->msg = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
                rz->src = cmdsrc;
                rz->id_msg_cmd = id_msg;

              s_long res = d_sl.r_qubci->push_1(rz); rz.clear();
                if (res == -1) { return -20; } // queue overflow
                if (res != 1) { return -2; }

              s_sl.id_msg_cmd = id_msg; s_ph = 4;
              return 1;
            }
          }
        }
        else if (dsln1[1] == L's') // qs
        {
          rses.__thm_lqsd_enable_full();
          if (dsln1[0] == L'q')
          {
              // Creating subscription message.
            cch_slot::qs_value msgsub;
              if (msgsub.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

              // Writing to qs slot.
            if (1)
            {
              critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
              s_long res = d_sl.r_qus->push_1(msgsub); msgsub.clear();
                if (res == -1) { return -20; } // queue overflow
                if (res != 1) { return -2; }
            }

              // Immediate delivery if necessary.
            if (d_sl.qs_deliv_imm()) { dispatcher_mt::thread_proxy::_s_subs_deliver(_r_ths, rd_sl, dthn, dsln_exact, 0); }
            return 1;
          }
        }
      }
      return -2;
    }
    else if (da.isLM()) // sending a message to peer process
    {
      if (sender_type <= st_s_subs_deliver)
      {
        if (!rses.lmsc) { return -11; }
        std::wstring ssln_exact, ssln1;
        res = __recode_slotname(sa.sln_v(), &ssln_exact, &ssln1, 0, &da); // (in _s_write)
          if (res != 1) { return -1; }
          if (!sln1chk_main_o(ssln1)) { return -1; }

          // Det. dest. slot type and exact name.
          // Check source / dest. type match.
        std::wstring dsln_exact = da.wstr_sln();
        std::wstring dsln1 = da.wstr_sln_1();
          if (!sln1chk_main_i(dsln1)) { return -1; }
          if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
          if (!sln1chk_iomatch(ssln1, dsln1)) { return -1; }
        std::wstring name_peer = da.wstr_pn();

        s_ll id_msg = -1; unsigned char flags_mtrk = 0; // flags_mtrk: see md_entry type 54
        if (id_msg_orig != -1) { id_msg = id_msg_orig; flags_mtrk = 1; } else { id_msg = idgen(); }
        const s_long flags_clt_msend = flags_msend & _fl_msend__mask_clt_msend;

          // Get slot refs.
        cref_t<cch_slot> rs_sl;
        weakref_t<t_htracking_proxy> rhtrprx;
        if (1)
        {
          mst_semaphore ms_s(rses, _name_th);
          hangdet hd; while (true) { if (hd) { return -2; } int res2 = 1; if (!rs_sl) { res2 = ms_s.r_sl(ssln_exact, rs_sl); if (res2 < 0) {  if (!ms_s.p_thread()) { return -7; } return -6; } } if (res2 > 0) { break; } sleep_mcs(50); }
        }
        if (id_msg_orig != -1)
        {
          if (!prhtrprx) { return -2; }
          rhtrprx = *prhtrprx;
        }
        cch_slot& s_sl = *rs_sl._pnonc_u();

          // Check phases, recode message, write dest. slot, update phases.
        if (!sa._conv_LP_LM(rses.name_pr)) { return -2; }
        __src = sa.addr();
        std::wstring wmsg; cref_t<t_stringref> att2;
        if (1)
        {
          inprocess_message __msg; if (__msg.set_parts(hm1, cref_t<t_stringref>(), true, flags_msend & ~_fl_msend_anlo_msg, flags_ses) < 1) { return -2; }
          paramline().encode(__msg.hmsg.ref(), wmsg);
        }
        if (att)
        {
          const bool b_assign_att_cref = (flags_msend & _fl_msend_anlo_att) && (flags_ses & _frqperm_msend_anlo);
          if (b_assign_att_cref) { att2 = att; }
            else { try { att2 = i_dispatcher_mt::make_rba(att.ref(), true); } catch (...) {} if (!att2) { return -2; } }
        }

        if (dsln1[1] == L'i' || dsln1[1] == L's') // msg --> LMSC (peer's pi, qi, qs)
        {
          rses.__thm_lmsc_enable_full();
          s_long res = rses.lmsc->clt_msend(id_msg, -1, flags_mtrk, name_peer, wmsg, att2, flags_clt_msend, rhtrprx);
              if (res < 1) { return res < -2 ? res : -2; }
          return 1;
        }
        else if (dsln1[1] == L'b') // msg --> LMSC (peer's any: pbo, hbo, pbi, qbi)
        {
          if ((dsln1[0] == L'p' || dsln1[0] == L'h') && dsln1[2] == L'o') // response (from pbi or qbi) --> LMSC (peer's pbo or hbo)
          {
            const std::wstring cmdsrc(da.wstr_addr());
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            s_long& s_ph = s_sl.phase;
            if (!(s_ph == 2)) { return -5; } // out of order
            if (cmdsrc != s_sl.r_lsrc.ref()) { return -1; }
            rses.__thm_lmsc_enable_full();
            s_long res = rses.lmsc->clt_msend(id_msg, s_sl.id_msg_cmd, flags_mtrk, name_peer, wmsg, att2, flags_clt_msend, rhtrprx);
              if (res < 1) { return res < -2 ? res : -2; }
            s_sl.r_lsrc.clear(); s_sl.id_msg_cmd = -1; s_ph = 3;
            return 1;
          }
          else if ((dsln1[0] == L'p' || dsln1[0] == L'q') && dsln1[2] == L'i') // command (from hbo or pbo) --> LMSC (peer's pbi or qbi)
          {
            critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}
            struct __csl_trk_rmv
            {
              dispatcher_mt::cch_session& rses; s_ll id_msg;
              __csl_trk_rmv(dispatcher_mt::cch_session& rses_, s_ll id_msg_, cch_slot::csl_tracking& info) : rses(rses_), id_msg(id_msg_)
              {
                critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_htrk_csl);
                const hashx<s_ll, cch_slot::csl_tracking>::entry* e = 0;
                if (rses.htrk_csl.insert(id_msg, &e) < 1) { id_msg = -1; return; }
                bmdx_str::words::swap_bytes(e->v, info);
              }
              ~__csl_trk_rmv()
              {
                if (id_msg < 1) { return; }
                critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_htrk_csl);
                rses.htrk_csl.remove(id_msg);
              }
            };
            cch_slot::csl_tracking _tr1; _tr1.thn = _name_th; _tr1.sln_exact = ssln_exact;
            if (ssln1[0] == L'h') // hbo --> LMSC (peer's pbi or qbi)
            {
              const std::wstring cmdsrc(da.wstr_addr());
              _tr1.key_hbo = cmdsrc;
              __csl_trk_rmv _tr2(rses, id_msg, _tr1); if (_tr2.id_msg < 1) { return -2; }
              const cch_slot::t_haddrl::entry* e = 0;
              s_long res = s_sl.r_haddrl->insert(cmdsrc, &e);
                if (res == 0) { return -5; } // out of order
                if (res != 1) { return -2; }
              rses.__thm_lmsc_enable_full();
              res = rses.lmsc->clt_msend(id_msg, -1, flags_mtrk | 2, name_peer, wmsg, att2, flags_clt_msend, rhtrprx);
                if (res < 1) { s_sl.r_haddrl->remove_e(e); return res < -2 ? res : -2; }
              _tr2.id_msg = -1; // prevents entry autoremoving from rses.htrk_csl
              e->v.id_msg_cmd = id_msg; e->v.value = 7;
              return 1;
            }
            else if (ssln1[0] == L'p') // pbo --> LMSC (peer's pbi or qbi)
            {
              __csl_trk_rmv _tr2(rses, id_msg, _tr1); if (_tr2.id_msg < 1) { return -2; }
              s_long& s_ph = s_sl.phase;
              if (!(s_ph == 0 || s_ph == 6)) { return -5; }
              rses.__thm_lmsc_enable_full();
              s_long res = rses.lmsc->clt_msend(id_msg, -1, flags_mtrk | 2, name_peer, wmsg, att2, flags_clt_msend, rhtrprx);
                if (res < 1) { return res < -2 ? res : -2; }
              _tr2.id_msg = -1; // prevents entry autoremoving from rses.htrk_csl
              s_sl.id_msg_cmd = id_msg; s_ph = 7;
              return 1;
            }
          }
        }
      }
      else if (sender_type == st_lmsc)
      {
        rses.__thm_lmsc_enable_full();
        if (!da.isLM()) { return -1; }
        std::wstring ssln1;
        res = __recode_slotname(sa.sln_v(), 0, &ssln1, 0, &da); // (in _s_write)
          if (res != 1) { return -1; }
          if (!sln1chk_main_o(ssln1)) { return -1; }

        std::wstring dthn = da.wstr_thn();
          // Det. dest. slot type and exact name.
          // Check source / dest. type match.
        std::wstring dsln_exact = da.wstr_sln();
        std::wstring dsln1 = da.wstr_sln_1();
          if (!sln1chk_main_i(dsln1)) { return -1; }
          if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
          if (!sln1chk_iomatch(ssln1, dsln1)) { return -1; }

          // Get dest. slot ref.
        cref_t<cch_slot> rd_sl;
        if (1)
        {
          mst_semaphore ms_d(rses, dthn);
          hangdet hd; while (true) { if (hd) { return -2; } int res1 = 1; if (!rd_sl) { res1 = ms_d.r_sl(dsln_exact, rd_sl); if (res1 < 0) { if (!ms_d.p_thread()) { return -9; } return -8; } } if (res1 > 0) { break; } sleep_mcs(50); }
        }
        cch_slot& d_sl = *rd_sl._pnonc_u();

        if (dsln1[1] == L'i' || dsln1[1] == L's' || (dsln1[1] == L'b' && dsln1[2] == L'i')) // LMSC --> all normal inputs: qs, pi, qi, pbi, qbi
        {
          if (da.isLPA()) { if (!d_sl.b_input_lpa()) { return -10; } }
          else if (!d_sl.b_input_any_th()) { return -10; }
        }

          // Check phases, recode message, write dest. slot, update phases.
        if (dsln1[1] == L'i') // LMSC --> (pi or qi)
        {
          if (dsln1[0] == L'p') // LMSC --> pi
          {
            inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            if (sln1chk_qs(ssln1))
            {
              if (!(d_sl.r_haddrl && d_sl.r_haddrl->find(sa.wstr_addr()))) { return -2; }
            }
            bool b_over = d_sl.phase == 1; // the currently holds the previous message
            d_sl.r_pin = msg2; msg2.clear();
            d_sl.phase = 1;
            return b_over ? 0 : 1;
          }
          else if (dsln1[0] == L'q') // LMSC --> qi
          {
            inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

            if (1)
            {
              critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
              if (sln1chk_qs(ssln1))
              {
                if (!(d_sl.r_haddrl && d_sl.r_haddrl->find(sa.wstr_addr()))) { return -2; }
              }
              s_long res = d_sl.r_qumi->push_1(msg2);
              msg2.clear();
                if (res == -1) { return -20; } // queue overflow
                if (res != 1) { return -2; }
            }
            return 1;
          }
          else { return -2; }
        }
        else if (dsln1[1] == L'b') // LMSC --> cmd. or response --> (any: pbo, hbo, pbi, qbi)
        {
          inprocess_message msg2; if (msg2.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

          if (dsln1[0] == L'p' && dsln1[2] == L'o') // LMSC --> response --> pbo
          {
              // Reverse order of locks for pbo == forward order of locks for pbi and qbi.
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            s_long& d_ph = d_sl.phase;

            if (!((d_ph == 7 || d_ph == 4) && id_msg_orig == d_sl.id_msg_cmd)) { return -5; } // out of order
            d_sl.r_pin = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
            d_ph = 5;
            return 1;
          }
          else if (dsln1[0] == L'h' && dsln1[2] == L'o') // LMSC --> response --> hbo
          {
            const std::wstring cmdsrc(da.wstr_addr());

              // Reverse order of locks for hbo == forward order of locks for pbi and qbi.
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}

            cref_t<cch_slot::qbci_value, cref_nonlock> rz;
              if (!rz.create0(true)) { return -2; }
              if (!rz->src.copy(sa.wstr_addr(), true)) { return -2; }
                rz->msg = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
            const cch_slot::t_haddrl::entry* e = d_sl.r_haddrl->find(rz->src.ref());
            if (!(e && (e->v.value == 7 || e->v.value == 4) && id_msg_orig == e->v.id_msg_cmd)) { return -5; } // out of order

            s_long res  = d_sl.r_qubci->push_1(rz); rz.clear();
              if (res == -1) { return -20; } // queue overflow
              if (res != 1) { return -2; }

            e->v.value = 5;
            return 1;
          }
          else if (dsln1[0] == L'p' && dsln1[2] == L'i') // LMSC --> command --> pbi
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            cref_t<std::wstring, cref_nonlock> cmdsrc; if (!cmdsrc.create1(1, sa.wstr_addr())) { return -2; }
            s_long& d_ph = d_sl.phase;

            if (ssln1[0] == L'h' || ssln1[0] == L'p') // LMSC --> (from hbo or pbo) --> pbi
            {
              if (!(d_ph == 0 || d_ph == 3)) { return -20; }
              d_sl.r_pin = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
              d_sl.r_lsrc = cmdsrc; d_sl.id_msg_cmd = id_msg_nl; d_ph = 1;
              return 1;
            }
          }
          else if (dsln1[0] == L'q' && dsln1[2] == L'i') // LMSC --> command  --> qbi
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            cref_t<std::wstring, cref_nonlock> cmdsrc; if (!cmdsrc.create1(1, sa.wstr_addr())) { return -2; }

            if (ssln1[0] == L'h' || ssln1[0] == L'p') // LMSC --> (from hbo or pbo) --> qbi
            {
              cref_t<cch_slot::qbci_value, cref_nonlock> rz;
                if (!rz.create0(true)) { return -2; }
                rz->msg = msg2; msg2.clear(); // clearing non-locking object before local locks are removed
                rz->src = cmdsrc;
                rz->id_msg_cmd = id_msg_nl;

              res = d_sl.r_qubci->push_1(rz); rz.clear();
                if (res == -1) { return -20; } // queue overflow
                if (res != 1) { return -2; }
              return 1;
            }
          }
        }
        else if (dsln1[1] == L's') // LMSC --> qs
        {
          rses.__thm_lqsd_enable_full();
          if (dsln1[0] == L'q')
          {
              // Creating subscription message.
            cch_slot::qs_value msgsub;
              if (msgsub.set_parts(hm1, att, true, flags_msend, flags_ses) < 1) { return -2; }

              // Writing to qs slot.
            if (1)
            {
              critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
              s_long res = d_sl.r_qus->push_1(msgsub); msgsub.clear();
                if (res == -1) { return -20; } // queue overflow
                if (res != 1) { return -2; }
            }

              // Immediate delivery if necessary.
            if (d_sl.qs_deliv_imm()) { dispatcher_mt::thread_proxy::_s_subs_deliver(_r_ths, rd_sl, dthn, dsln_exact, 0); }
            return 1;
          }
        }
      }
      return -2;
    }
    return -11;
  } catch (...) {}
  return -2;
}

  // pprx == 0 means the call directly from dispatcher_mt (not associated with any thread).
s_long dispatcher_mt::thread_proxy::_s_request(thread_proxy* pprx, cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, unity& retval, const unity& args, int frqperm, s_long flags_rq) __bmdx_noex
{
  retval.clear();
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  cch_session& rses = *_r_ths._pnonc_u();
  switch (rt)
  {
  case 1: // get list of threads
    try {
      if ((frqperm & 0x8) == 0) { return -4; }
      retval.arr_init<utStringArray>(0);
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      for (s_long i = 0; i < rses.hg_threads.n(); ++i) { retval.arr_append(rses.hg_threads(i)->k.vstr()); }
      return 1;
    } catch (...) { return -2; }
  case 2: // get all slots of a thread
    try {
      if ((frqperm & 0x8) == 0) { return -4; }
      retval.arr_init<utStringArray>(0);
      std::wstring thn = args.vstr();
      mst_semaphore ms(rses, thn);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_ro_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -7; }  // under m_th_ro_acquire
      for (s_long i = 0; i < pth->h_sl.n(); ++i) { retval.arr_append(pth->h_sl(i)->k.vstr()); }
      return 1;
    } catch (...) { return -2; }
  case 11: // get N of threads
    try {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      retval = rses.hg_threads.n();
      return 1;
    } catch (...) { return -2; }
  case 12: // get N of slots of a thread
    try {
      std::wstring thn = args.vstr();
      mst_semaphore ms(rses, thn);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_ro_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -7; } // under m_th_ro_acquire
      retval = pth->h_sl.n();
      return 1;
    } catch (...) { return -2; }
  case 3: // set priority and timing for internal subscription delivery
    try {
      if ((frqperm & 0x10) == 0) { return -4; }
      if (_r_ths.ref().__thm_lqsd == 0) { return -7; }
      if (!(args.isArray() && args.arrsz() >= 2)) { return -5; }
      meta::s_ll pr, dt;
      try {
        pr = args.vint(args.arrlb());
        dt = args.vint(args.arrlb() + 1);
      } catch (...) { return -5; }
      if (!(pr >= 1 && pr <= 7 && dt >= 0)) { return -5; }
      if (!_r_ths->th_lqsd.set_priority(s_long(pr))) { return -2; }
      _r_ths->qsd_prio = s_long(pr);
      _r_ths->lqsd_dt = dt;
      return 1;
    } catch (...) { return -2; }
  case 4: // get priority and timing of internal subscription delivery
    try {
      if ((frqperm & 0x10) == 0) { return -4; }
      if (_r_ths.ref().__thm_lqsd == 0) { return -7; }
      unity x; x.arr_init<utInt>(1);
      x.arr_append(_r_ths.ref().qsd_prio);
      x.arr_append(_r_ths.ref().lqsd_dt);
      retval = x;
      return 1;
    } catch (...) { return -2; }
  case 5: // get new proxy
    try {
      if ((frqperm & 0x4) == 0) { return -4; }
      return dispatcher_mt::thread_proxy::_s_proxy_new(_r_ths, retval, args.vstr());
    } catch (...) { return -2; }
  case 6: // create thread
    try {
      if ((frqperm & 0x1) == 0) { return -4; }
      const s_long flags_cr = flags_rq & 0x1;
      return dispatcher_mt::thread_proxy::_s_thread_create(_r_ths, args, flags_cr);
    } catch (...) { return -2; }
  case 7: // remove thread
    try {
      if ((frqperm & 0x2) == 0) { return -4; }
      return dispatcher_mt::thread_proxy::_s_thread_remove(_r_ths, args.vstr());
    } catch (...) { return -2; }
  case 8: // deliver messages
    try {
      if (!args.isInt()) { return -5; }
      s_long res = dispatcher_mt::thread_proxy::_s_qs_deliver(_r_ths, pprx ? pprx->_name_th : std::wstring(), args.vint_l(), 0);
      if (res >= 0) { res = 1; }
        else if (res != -3) { res = -2; }
      return res;
    } catch (...) { return -2; }
  case 9: // reset command slot
    try {
      unity __hm0; s_long res = -2;
      const unity* phm1 = 0;
      if (args.isAssoc()) { phm1 = &args; }
        else if (args.isString()) { paramline().decode(args.cref<utString>().ref(), __hm0, false); phm1 = &__hm0; }
        else { return -1; }
      const unity k_src(L"src"), k_trg(L"trg");
      const unity* __psrc0 = 0; try { __psrc0 = &(*phm1)[k_src]; } catch (...) { return -1; }
      std::wstring k_sl; res = __recode_slotname(*__psrc0, &k_sl); if (res != 1) { return res == -1 ? -1 : -2; }

      cref_t<cch_slot> rcmd_sl;
      if (1)
      {
        mst_semaphore ms_s(rses, pprx ? pprx->_name_th : std::wstring()); hangdet hd;
        int bf = 0; while (true) { if (hd) { bf = 2; break; } int res = ms_s.r_sl(k_sl, rcmd_sl); if (res < 0) { bf = 1; break; } if (res > 0) { break; } sleep_mcs(50); }
        if (bf != 0) { return -2; }
      }
      cch_slot& cmd_sl = *rcmd_sl._pnonc_u();
      if (1)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &cmd_sl.lkd); if (sizeof(__lock)) {}

        if (cmd_sl.eq_type("hbo"))
        {
          const unity *__ptrg = 0; try { __ptrg = &(*phm1)[k_trg]; } catch (...) { return -1; }
          address da; res = da.set_addr(*__ptrg); if (res != 1) { return res == -1 ? -1 : -2; }
          if (cmd_sl.r_haddrl) { cmd_sl.r_haddrl->remove_e(cmd_sl.r_haddrl->find(da.wstr_addr())); }
        }
        else if (cmd_sl.eq_type("pbo")) { cmd_sl.r_pin.clear(); cmd_sl.id_msg_cmd = -1; cmd_sl.phase = 6; }
        else if (cmd_sl.eq_type("pbi")) { cmd_sl.r_pin.clear(); cmd_sl.r_lsrc.clear(); cmd_sl.id_msg_cmd = -1; cmd_sl.phase = 3; }
        else if (cmd_sl.eq_type("qbi")) { cmd_sl.r_lsrc.clear(); cmd_sl.id_msg_cmd = -1; cmd_sl.phase = 3; }
        else { return -2; }

      }
      return 1;
    } catch (...) { return -2; }
  case dispatcher_mt_rt::set_shmqueue_conf: // 10
  {
    bool b_clear_rv = true;
    try {
      using namespace bmdx_shm;
      retval.clear();
      if ((frqperm & _frqperm_shmqueue_in_conf) == 0) { return -4; }
      if (!args.isAssoc()) { return -5; }
      if (!rses.lmsc) { return -6; }

      const unity k_peer_name = L"peer_name";
      const unity kf_set_al_in = L"conf_set_al_in";
      const unity kf_set_lqcap = L"conf_set_lqcap";

        if (!(args.u_has(k_peer_name, 6) && args[k_peer_name].isString())) { return -5; }
        const unity& prnm = args[k_peer_name];

        const unity* pa_set_al = args.u_has(kf_set_al_in, 6) ? &args[kf_set_al_in] : 0;
        const unity* pa_set_lqcap = args.u_has(kf_set_lqcap, 6) ? &args[kf_set_lqcap] : 0;
          if (!(pa_set_al || pa_set_lqcap)) { return 0; }

      const unity ka_al = L"p_al";
      const unity ka_tmo = L"timeout_ms";
      const unity ka_b_rcv = L"b_receiver";
      const unity ka_ncapmin = L"ncapmin";
      const unity ka_ncapmax = L"ncapmax";
      const unity ka_nrsv = L"nrsv";

        bool b_receiver = true;
        s_ll ncapmin = -3, ncapmax = -3, nrsv = -3;
        double tmo_lqcap = -1;
          if (pa_set_lqcap)
          {
            b_receiver = !!((*pa_set_lqcap)[ka_b_rcv].vint()); // NOTE ka_b_rcv key is required here
            tmo_lqcap = +*pa_set_lqcap / ka_tmo / -1.;
            ncapmin = +*pa_set_lqcap / ka_ncapmin / -3ll;
            ncapmax = +*pa_set_lqcap / ka_ncapmax / -3ll;
            nrsv = +*pa_set_lqcap / ka_nrsv / -3ll;
          }
        if (pa_set_al && !b_receiver) { return -5; } // conf_set_al_in request is not valid for sender side

        cref_t<i_allocctl> empty;
        double tmo_al = -1;
        const cref_t<i_allocctl>* p_al = 0;
          if (pa_set_al)
          {
            tmo_al = +*pa_set_al / ka_tmo / -1.;
            if (pa_set_al->u_has(ka_al, 6))
            {
              const unity& r = (*pa_set_al)[ka_al];
              if (r.isEmpty()) { p_al = &empty; }
                else { p_al = (*pa_set_al)[ka_al].objPtr_c<cref_t<i_allocctl> >(true, false); }
              if (!p_al) { return -2; }
            }
          }

      bool b_c_rst_proxy = pprx && !!(flags_rq & 1);
      if (b_c_rst_proxy)
      {
        try {
          critsec_t<_hpt_conf_reset_lks> __lock(10, -1); if (sizeof(__lock)) {}
          if (!pprx->_hpt_conf_reset) { pprx->_hpt_conf_reset.create0(0); }
          pprx->_hpt_conf_reset->opsub(prnm); // ensure key existence
        } catch (...) { return -2; }
      }

      bool b_any_succ = false;
      bool b_all_succ = true;
      if (b_receiver)
      {

        unity& r_ret_al = retval(kf_set_al_in);
        unity& r_ret_lqcap = retval(kf_set_lqcap);
        s_long res_al = 0;
        s_long res_lqcap = 0;
        s_long res2 = rses.lmsc->clt_qum_conf_set_rcv(prnm.vstr(), !!pa_set_al, !!pa_set_lqcap, res_al, res_lqcap, p_al, tmo_al, ncapmin, ncapmax, nrsv, tmo_lqcap);
        if (res2 == 1)
        {
          if (pa_set_al) { r_ret_al = res_al; if (res_al >= 0) { b_any_succ = true; } else { b_all_succ = false; }  }
          if (pa_set_lqcap) { r_ret_lqcap = res_lqcap; if (res_lqcap >= 0) { b_any_succ = true; } else { b_all_succ = false; } }
        }
      }
      else
      {
        unity& r_ret_lqcap = retval(kf_set_lqcap);
        s_long res_lqcap = 0;
        s_long res2 = rses.lmsc->clt_qum_conf_set_send(prnm.vstr(), res_lqcap, ncapmin, ncapmax, nrsv, tmo_lqcap);
        if (res2 == 1)
        {
          if (pa_set_lqcap) { r_ret_lqcap = res_lqcap; if (res_lqcap >= 0) { b_any_succ = true; } else { b_all_succ = false; } }
        }
      }
      b_clear_rv = false;

      if (b_c_rst_proxy && b_any_succ)
      {
        try {
          critsec_t<_hpt_conf_reset_lks> __lock(10, -1); if (sizeof(__lock)) {}
          pprx->_hpt_conf_reset->opsub_c(prnm) |= b_receiver ? 0x2 : 0x1; // mark that shmqueue conf. must be reset on proxy exit
        } catch (...) {} // an exception is not expected (prnm key ensured above)
      }

      s_long res = 1;
      if (!b_all_succ) { res = -7; }
      return res;
    } catch (...) { if (b_clear_rv) { retval.clear(); } return -2; }
  }
  case dispatcher_mt_rt::get_qs_nvsubs: // 13
  {
    try {
      address qsa;
      s_long res;
      res = qsa.set_addr(args); if (res != 1) { return res == -1 ? -5 : -2; }
      if (!qsa.isLP_any()) { return -5; } // the request isn't working on non-local addresses

      rses.__thm_lqsd_enable_full();
      address __qsa_deref;
      const address* pqsa = &qsa;
      cref_t<cch_slot> rqs_sl; std::wstring sln_s = qsa.wstr_sln();
      if (qsa.isLPA())
      {
        std::wstring thn_qs;
        if (1)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
          const hashx<unity, unity>::entry* e1 = rses.hg_lpai.find(sln_s); if (!e1) { return -6; }
          thn_qs = e1->v.vstr();
        }
        if (__qsa_deref.set_addr_LP(thn_qs, sln_s) != 1) { return -2; }
        pqsa = &__qsa_deref;
      }
      mst_semaphore ms(rses, pqsa->wstr_thn());
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return -6; } if (res > 0) { break; } sleep_mcs(50); }

      cch_slot& qs_sl = *rqs_sl._pnonc_u();
      cch_slot::t_haddrl* ph1(0);
      if (1)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
        ph1 = qs_sl.r_haddrl._pnonc_u(); if (!ph1) { return -2; }
        const s_long n = ph1->n();
        if (flags_rq & 1) { retval.arr_init<utStringArray>(0);  for (s_long i = 0; i < n; ++i) { retval.arr_append((*ph1)(i)->k); } }
          else { retval = n; }
        return 1;
      }
    } catch (...) { retval.clear(); }
    return -2;
  }
  default: { return -1; }
  }
}

  // qsa: "broadcasting" local or non-local qs slot address
  // suba: "subscribed" local or non-local recipient address
  // sender_type:
  //    a) st_client: qsa may be any of (LP, LPA, LM), suba must be local (LP-type only) address.
  //      id_rq_subs: if tracking is needed for non-local subscription, must be unique value, != -1.
  //        (In other cases, ID is generated by _s_subscribe internally, as necessary.)
  //      prhtrprx: ptr. to client proxy's mtrk_htracking object; must be !=0 if qsa is non-local, + must remain valid during _s_write call.
  //    b) st_lmsc: qsa must be local (LP or LPA) address, suba must be non-local address.
  //      id_rq_subs, prhtrprx: ignored, may be -1, 0.
  //      For st_lmsc, it is the client responsibility to send _s_subscribe ret. code back to non-local client.
  //        NOTE This case may occur during dispatcher late initialization and any time later.
  //    c, d) st_update_subs_input, st_update_subs_output:
  //        for direct calling from _s_update_subs_lists.
  //        NOTE This case may occur during dispatcher early initialization and any time later.
  //      id_rq_subs: must be generated (!= -1).
  //      prhtrprx: ignored, should be 0. rses.lmsc->clt_mdsend will be anyway called with empty rhtrprx arg.
  // pret_destloc:
  //    may be 0; if != 0 - receives the location of destination slot (regardless of success of operation):
  //    0 - location not determined,
  //    1 - local destination (e.g. direct modification of local qs slot),
  //    2 - non-local destination (e.g. subscription request through LMSC clt_msend).
s_long dispatcher_mt::thread_proxy::_s_subscribe(cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, Esender_type sender_type, const address& qsa, const address& suba, s_ll id_rq_subs, refmaker_t<t_htracking_proxy>* prhtrprx, s_long* pret_destloc) __bmdx_noex
{
  if (!_r_ths.has_ref()) { return -2; }
  cch_session& rses = *_r_ths._pnonc_u();
  int _sst = rses.ses_state;
  if (sender_type == st_client) { if (!(_sst == 1)) { return -3; } }
    else if (sender_type == st_lmsc) { if (!(_sst == 1 || _sst == -1)) { return -3; } }
    else { if (!(_sst == 1 || _sst < 0)) { return -3; } }
  try {
    if (pret_destloc) { *pret_destloc = 0; }
    const std::wstring ssln1 = qsa.wstr_sln_1();
    const std::wstring sslntail = qsa.wstr_sln_tail();
      if (!sln1chk_qs(ssln1)) { return -1; }
    const std::wstring dsln1 = suba.wstr_sln_1();
      if (!sln1chk_subscriber(dsln1)) { return -1; }
      if (sln1_root(ssln1) != sln1_root(dsln1)) { return -1; }
      if (sslntail.length() > 0 && suba.wstr_sln_tail() != sslntail) { return -1; }

    if (sender_type == st_client)
    {
      if (!(rt >= 1 && rt <= 3)) { return -1; }
      if (!suba.isLP()) { return -1; }
      std::wstring _thn_suba = suba.wstr_thn();
      if (qsa.isLP_any()) // subscribe to local address (immediate result)
      {
        if (pret_destloc) { *pret_destloc = 1; }
        rses.__thm_lqsd_enable_full();
        address __qsa_deref;
        const address* pqsa = &qsa;
        cref_t<cch_slot> rqs_sl; std::wstring sln_s = qsa.wstr_sln();
        if (qsa.isLPA())
        {
          std::wstring thn_qs;
          if (1)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
            const hashx<unity, unity>::entry* e1 = rses.hg_lpai.find(sln_s); if (!e1) { return -6; }
            thn_qs = e1->v.vstr();
          }
          if (__qsa_deref.set_addr_LP(thn_qs, sln_s) != 1) { return -2; }
          pqsa = &__qsa_deref;
        }
        mst_semaphore ms(rses, pqsa->wstr_thn());
        hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return -6; } if (res > 0) { break; } sleep_mcs(50); }

        cref_t<cch_slot> rsub_sl;
        if (1) { mst_semaphore ms(rses, _thn_suba); hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(suba.wstr_sln(), rsub_sl); if (res < 0) { return -5; } if (res > 0) { break; } sleep_mcs(50); } }

        cch_slot& qs_sl = *rqs_sl._pnonc_u();
        cch_slot& sub_sl = *rsub_sl._pnonc_u();
        unity suba_u = suba.wstr_addr();
        unity qsa_u = pqsa->wstr_addr();
        cch_slot::t_haddrl *ph1(0), *ph2(0);
        bool b_sub1(false), b_sub2(false);
        if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1 = qs_sl.r_haddrl._pnonc_u(); if (!ph1) { return -2; } b_sub1 = !!ph1->find(suba_u); }
        if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} ph2 = sub_sl.r_haddrl._pnonc_u(); if (!ph2) { sub_sl.r_haddrl.create0(true); ph2 = sub_sl.r_haddrl._pnonc_u(); } if (!ph2) { return -2; } b_sub2 = !!ph2->find(qsa_u); }

        if (rt == 1) // sub
        {
          if (b_sub1 && b_sub2) { return 2; }
          if (qs_sl.qs_output_fixed()) { if (!b_sub1) { return -10; } }
          if (!b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} if (ph1->insert(suba_u) < 0) { return -2; } }
          if (!b_sub2)
          {
            bool bf = false;
            if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}  bf = ph2->insert(qsa_u) < 0; }
            if (bf && !b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1->remove(suba_u); }
            if (bf && !b_sub1) { return -2; }
          }
          return 2;
        }
        else if (rt == 2) // uns
        {
          if (!b_sub1 && !b_sub2) { return 1; }
          if (qs_sl.qs_output_fixed()) { if (b_sub1) { return -10; } }
          if (b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1->remove(suba_u); }
          if (b_sub2) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} ph2->remove(qsa_u); }
          return 1;
        }
        // else rt == 3, chk sub
        return b_sub1 && b_sub2 ? 2 : 1;
      }
      else // subscribe to qsa with non-local address (non-immediate result)
      {
        if (pret_destloc) { *pret_destloc = 2; }
        if (!qsa.isLM()) { return -1; }
        if (!prhtrprx) { return -1; }
        if (!suba.isLP()) { return -1; }
        if (id_rq_subs == -1) { id_rq_subs = idgen(); }

          // Prep. message for LMSC.
        if (!rses.lmsc) { return -11; }
        address suba2(suba);
        if (!suba2._conv_LP_LM(rses.name_pr)) { return -2; }
        cref_t<netmsg_header::msg_builder> rmb = netmsg_header::mdd_subs_request::make_msg(rt, id_rq_subs, qsa.wstr_addr(), suba2.wstr_addr());
          if (!rmb) { return -2; }

        if (rt == 1 || rt ==2)
        {
            // Update local slot input list.
          cref_t<cch_slot> rsub_sl;
          if (1) { mst_semaphore ms(rses, _thn_suba); hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(suba.wstr_sln(), rsub_sl); if (res < 0) { return -5; } if (res > 0) { break; } sleep_mcs(50); } }

          if (1)
          {
            cch_slot& sub_sl = *rsub_sl._pnonc_u();
            critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
            cch_slot::t_haddrl* ph2 = sub_sl.r_haddrl._pnonc_u(); if (!ph2) { sub_sl.r_haddrl.create0(true); ph2 = sub_sl.r_haddrl._pnonc_u(); }
            if (!ph2) { return -2; }
            if (rt == 1) { if (ph2->insert(qsa.wstr_addr()) < 0) { return -2; } }
              else if (rt ==2) { ph2->remove(qsa.wstr_addr()); }
          }
        }

          // Request non-local qs slot output list update or check.
        rses.__thm_lmsc_enable_full();
        s_long res = rses.lmsc->clt_mdsend(id_rq_subs, qsa.wstr_pn(), rmb.ref(), *prhtrprx, 1);
          if (res < 0) { return res; }
          if (res > 0) { return 0; }
        return -2;
      }
    }
    else if (sender_type == st_lmsc) // subscribe non-local recipient to local address
    {
      if (!(rt >= 1 && rt <= 6)) { return -1; }
      if (pret_destloc) { *pret_destloc = 1; }
      if (!suba.isLM()) { return -1; }
      if (!qsa.isLM()) { return -1; }
      if (rt < 4) // request from non-local slot to local qs slot
      {
        rses.__thm_lqsd_enable_full();

        cref_t<cch_slot> rqs_sl; std::wstring sln_s = qsa.wstr_sln();
        mst_semaphore ms(rses, qsa.wstr_thn());
        hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return -6; } if (res > 0) { break; } sleep_mcs(50); }

        if (1)
        {
          cch_slot& qs_sl = *rqs_sl._pnonc_u();
          critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
          cch_slot::t_haddrl* ph1 = qs_sl.r_haddrl._pnonc_u();
          if (!ph1) { return -2; }
          unity suba_u(suba.wstr_addr());
          bool b_sub = !!ph1->find(suba_u);
          if (rt == 1) { if (b_sub) { return 2; } if (qs_sl.qs_output_fixed()) { return -10; } if (ph1->insert(suba_u) < 0) { return -2; } return 2; } // sub
            else if (rt == 2) { if (!b_sub) { return 1; } if (qs_sl.qs_output_fixed()) { return -10; } ph1->remove(suba_u); return 1; } // unsub
            else { return b_sub ? 2 : 1; } // chk
        }
      }
      else // rt 4..6 request from non-local qs slot to local subscriber
      {
        cref_t<cch_slot> rsub_sl;
        if (1) { mst_semaphore ms(rses, suba.wstr_thn()); hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(suba.wstr_sln(), rsub_sl); if (res < 0) { return -5; } if (res > 0) { break; } sleep_mcs(50); } }

        if (1)
        {
          cch_slot& sub_sl = *rsub_sl._pnonc_u();
          critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
          cch_slot::t_haddrl* ph2 = sub_sl.r_haddrl._pnonc_u(); if (!ph2) { sub_sl.r_haddrl.create0(true); ph2 = sub_sl.r_haddrl._pnonc_u(); }
          if (!ph2) { return -2; }
          unity qsa_u(qsa.wstr_addr());
          bool b_sub = !!ph2->find(qsa_u);
          if (rt == 4)  { if (b_sub) { return 2; } if (ph2->insert(qsa_u) < 0) { return -2; } return 2; } // sub
            else if (rt == 5)  { if (!b_sub) { return 1; } ph2->remove(qsa_u); return 1; } // unsub
            else { return b_sub ? 2 : 1; } // check
        }
      }
    }
    else if (sender_type == st_update_subs_input)
    {
        // Add (rt 1) or remove (rt 2) local supplier (qs) to/from any subscriber's input list.
      if (!(rt >= 1 && rt <= 2)) { return -1; }
      if (!qsa.isLP()) { return -1; }
      std::wstring _thn_suba = suba.wstr_thn();
      if (suba.isLP()) // modify local subscriber's input list (immediate result)
      {
        if (pret_destloc) { *pret_destloc = 1; }
        cref_t<cch_slot> rsub_sl;
        if (1) { mst_semaphore ms(rses, _thn_suba); hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(suba.wstr_sln(), rsub_sl); if (res < 0) { return -5; } if (res > 0) { break; } sleep_mcs(50); } }

        if (1)
        {
          cch_slot& sub_sl = *rsub_sl._pnonc_u();
          critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
          cch_slot::t_haddrl* ph2 = sub_sl.r_haddrl._pnonc_u(); if (!ph2) { sub_sl.r_haddrl.create0(true); ph2 = sub_sl.r_haddrl._pnonc_u(); }
          if (!ph2) { return -2; }
          unity qsa_u(qsa.wstr_addr());
          bool b_sub = !!ph2->find(qsa_u);
          if (rt == 1)  { if (b_sub) { return 2; } if (ph2->insert(qsa_u) < 0) { return -2; } return 2; } // sub
            else if (rt == 2)  { if (!b_sub) { return 1; } ph2->remove(qsa_u); return 1; } // unsub
            else { return -2; } // unexpected
        }
      }
      else // send request to modify non-local subscriber's input list (non-immediate result)
      {
        if (pret_destloc) { *pret_destloc = 2; }
        if (!suba.isLM()) { return -1; }
        if (id_rq_subs == -1) { return -1; }
        if (!qsa.isLP()) { return -1; }
          // Prep. message for LMSC.
        if (!rses.lmsc) { return -11; }
        address qsa2(qsa);
        if (!qsa2._conv_LP_LM(rses.name_pr)) { return -2; }
        cref_t<netmsg_header::msg_builder> rmb = netmsg_header::mdd_subs_request::make_msg(rt + 3, id_rq_subs, qsa2.wstr_addr(), suba.wstr_addr()); // rt 1, 2 is converted to 4, 5, as required by branch sender_type == st_lmsc (see above)
          if (!rmb) { return -2; }
          // Request non-local qs slot output list update or check.
        rses.__thm_lmsc_enable_full();
        s_long res = rses.lmsc->clt_mdsend(id_rq_subs, suba.wstr_pn(), rmb.ref(), weakref_t<t_htracking_proxy>(), -1); // comm_mode == -1 because the client (_s_update_subs_lists) already waited for comm. establishing
          if (res < 0) { return res; }
          if (res > 0) { return 0; }
        return -2;
      }
    }
    else if (sender_type == st_update_subs_output)
    {
        // Add (rt 1) or remove (rt 2) local subscriber from any supplier's (qs) output list (similar to _s_subscribe with st_client)
      if (!(rt >= 1 && rt <= 2)) { return -1; }
      if (!suba.isLP()) { return -1; }
      if (qsa.isLP()) // sub./unsub. to local address (immediate result)
      {
        if (pret_destloc) { *pret_destloc = 1; }
        cref_t<cch_slot> rqs_sl; std::wstring sln_s = qsa.wstr_sln();
        mst_semaphore ms(rses, qsa.wstr_thn());
        hangdet hd; while (true) { if (hd) { return -2; } int res = ms.r_sl(sln_s, rqs_sl); if (res < 0) { return -6; } if (res > 0) { break; } sleep_mcs(50); }

        if (1)
        {
          cch_slot& qs_sl = *rqs_sl._pnonc_u();
          critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
          cch_slot::t_haddrl* ph1 = qs_sl.r_haddrl._pnonc_u();
          if (!ph1) { return -2; }
          unity suba_u(suba.wstr_addr());
          bool b_sub = !!ph1->find(suba_u);
          if (rt == 1) { if (b_sub) { return 1; } if (qs_sl.qs_output_fixed()) { return -10; } ph1->insert(suba_u); return 2; } // sub
            else if (rt == 2) { if (!b_sub) { return 1; } if (qs_sl.qs_output_fixed()) { return -10; } ph1->remove(suba_u); return 1; } // unsub
            else { return -2; }
        }
      }
      else // send request (sub./unsub.) to qsa with non-local address (non-immediate result)
      {
        if (pret_destloc) { *pret_destloc = 2; }
        if (!qsa.isLM()) { return -1; }
        if (id_rq_subs == -1) { return -1; }
        if (!suba.isLP()) { return -1; }
          // Prep. message for LMSC.
        if (!rses.lmsc) { return -11; }
        address suba2(suba);
        if (!suba2._conv_LP_LM(rses.name_pr)) { return -2; }
        cref_t<netmsg_header::msg_builder> rmb = netmsg_header::mdd_subs_request::make_msg(rt, id_rq_subs, qsa.wstr_addr(), suba2.wstr_addr());
          if (!rmb) { return -2; }
          // Request non-local qs slot output list update or check.
        rses.__thm_lmsc_enable_full();
        s_long res = rses.lmsc->clt_mdsend(id_rq_subs, qsa.wstr_pn(), rmb.ref(), weakref_t<t_htracking_proxy>(), -1); // comm_mode == -1 because the client (_s_update_subs_lists) already waited for comm. establishing
          if (res < 0) { return res; }
          if (res > 0) { return 0; }
        return -2;
      }
    }
  } catch (...) {}
  return -2;
}

  // _s_qs_deliver delivers messages
  //  1) (on flags & 1) from the calling _name_th's qs slots with delivery type "thread" (listed in cch_thread::h_qs_thread).
  //    _name_th must specify a thread with disp = true.
  //  2) (on flags & 2) from qs slots of different threads, with delivery type "disp" (listed in global ths.hg_qs_disp).
  //    _name_th must be
  //      a) name of a thread with disp = true,
  //      b) empty, when called from dispatcher_mt::thread_proxy::th_lqsd_impl::_thread_proc.
  //
  // pnmsent: if != 0, receives the exact number of messages processed.
  //
  // Returns:
  //    1 - success, >=0 messages delivered. See also _s_subs_deliver.
  //    0 - all queues, specified by flags, were empty, so nothing done.
  //    -1 - failure, no messages sent.
  //    -2 - failure for one or more queues, messages for other queues may have been sent.
  //    -3 - session is closed, some part of messages may have been sent before that.
  //    -4 - invalid flags, no messages sent.
s_long dispatcher_mt::thread_proxy::_s_qs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, s_long flags, s_ll* pnmsent) __bmdx_noex
{
  if (pnmsent) { *pnmsent = 0; }
  if (!_r_ths.has_ref()) { return -1; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  if ((flags & 3) == 0) { return 0; }
  const bool b_internal = _name_th.empty();
  if (b_internal && !!(flags & 1)) { return -1; }
  cch_session& rses = *_r_ths._pnonc_u();
  critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_s_qs_deliver); if (sizeof(__lock)) {}
  try {

      // used on (flags & 1)
    vec2_t<std::wstring, __vecm_tu_selector> sl_th1_thn;
      vec2_t<cref_t<cch_slot>, __vecm_tu_selector> sl_th1;

      // used on (flags & 2)
    vec2_t<std::wstring, __vecm_tu_selector> sl_disp_sln, sl_disp_thn;
      vec2_t<cref_t<cch_slot>, __vecm_tu_selector> sl_disp;
    if (1)
    {
      mst_semaphore ms_th1(rses, _name_th);
      hashx<std::wstring, mst_semaphore, hashx_common::kf_basic<std::wstring>, __vecm_tu_selector> h_ms_disp;

      if (!b_internal) { hangdet hd; while (true) { int res = ms_th1.m_th_ro_acquire(); if (res != 0 || hd) { break; } sleep_mcs(50); } }
      cch_thread* pth_qs = ms_th1.p_thread(); if (!b_internal && !pth_qs) { return -1; } // under m_th_ro_acquire
      if ((flags & 2) && (b_internal || pth_qs->cb_disp))
      {
        hashx<dispatcher_mt::mst_semaphore*, s_long> h_waits;
        if (1) // get refs. for (disp. delivery) slots that are ready to receive messages
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
          for (s_long i = 0; i < rses.hg_qs_disp.n(); ++i)
          {
            std::wstring sln = rses.hg_qs_disp(i)->k.vstr();
            std::wstring thn = rses.hg_qs_disp(i)->v.vstr();
            mst_semaphore& ms_x = h_ms_disp.opsub(thn);
            ms_x.set_refs(rses, thn);

            sl_disp_sln.push_back(sln);
            sl_disp_thn.push_back(thn);
            sl_disp.push_back(cref_t<cch_slot>());

            int res = ms_x.m_th_ro_acquire();
              if (res == 0) { h_waits.opsub(&ms_x) = 1; }
              if (res == 1) { ms_x.r_sl(sln, sl_disp.back()); }
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
          for (s_long i = 0; i < sl_disp_sln.n(); ++i)
          {
            cref_t<cch_slot>& r_qs = sl_disp[i];
              if (r_qs) { continue; }
            std::wstring& sln = sl_disp_sln[i]; std::wstring& thn = sl_disp_thn[i]; mst_semaphore& ms_x = h_ms_disp.opsub(thn);
            if (ms_x.m_th_ro_acquire() == 1) { ms_x.r_sl(sln, r_qs); } // try to get slot ref. if having thread lock
          }
        }
      }

      if ((flags & 1) && pth_qs) // collect refs. for the calling thread's (_name_th) qs slots with conf. "delivery = thread"
      {
        hashx<unity, s_long>& h_qs = pth_qs->h_qs_thread;
        for (s_long i = 0; i < h_qs.n(); ++i)
        {
          std::wstring sln = h_qs(i)->k.vstr(); cref_t<cch_slot> r_qs;
          if (ms_th1.r_sl(sln, r_qs) == 1)
          {
            sl_th1.push_back(r_qs);
            sl_th1_thn.push_back(sln);
          }
        }
      }
    } // end semaphores scope

    s_long res = 0;
    if (flags & (1|2))
    {
      res = 1;
      if (flags & 1) // the current thread's own qs delivery
      {
        for (s_long i = 0; i < sl_th1.n(); ++i)
        {
          s_ll nm = 0;
          s_long res2 = dispatcher_mt::thread_proxy::_s_subs_deliver(_r_ths, sl_th1[i], _name_th, sl_th1_thn[i], &nm);
          if (pnmsent) { *pnmsent += nm; }
          if (res2 == -3) { return -3; }
          if (res2 < res) { res = res2; }
        }
      }
      if (flags & 2) // all threads globally listed qs delivery
      {
        for (s_long i = 0; i < sl_disp.n(); ++i)
        {
          cref_t<cch_slot>& r_qs = sl_disp[i];
            if (!r_qs) { continue; }
          s_ll nm = 0;
          s_long res2 = dispatcher_mt::thread_proxy::_s_subs_deliver(_r_ths, r_qs, sl_disp_thn[i], sl_disp_sln[i], &nm);
          if (pnmsent) { *pnmsent += nm; }
          if (res2 == -3) { return -3; }
          if (res2 < res) { res = res2; }
        }
      }
    }
    return res;
  } catch (...) { return -2; }
}

  // _name_th - r_qs owner thread name.
  // _name_qs: r_qs name.
  // pnmsent: if != 0, receives the exact number of messages processed.
  // Returns:
  //    1 - the queue was not empty, and >=0 messages are delivered (0 may be if for some of them delivery failed as whole).
  //    0 - the queue was empty, no messages delivered.
  //    -1 - failure, no messages sent.
  //    -2 - failure, some part of messages may have been sent.
  //    -3 - session is closed, no messages sent.
s_long dispatcher_mt::thread_proxy::_s_subs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, cref_t<cch_slot>& r_qs, const std::wstring& _name_th, const std::wstring& _name_qs, s_ll* pnmsent) __bmdx_noex
{
  if (pnmsent) { *pnmsent = 0; }
  if (!_r_ths.has_ref()) { return -1; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }
  if (!r_qs.has_ref()) { return -1; }

  s_long res0 = -1;
  try {
    const unity k_src(L"src"), k_trg(L"trg"), k_src0(L"src0"), k_trg0(L"trg0");
    address sa;
      if (1)
      {
        unity a;
        __append_vals(a, L"LP");
        __append_vals(a, _name_th);
        __append_vals(a, paramline().decode1v(_name_qs));
        sa.set_addr(a);
        if (!sln1chk_main_qs(sa.addr(3).rstr())) { return -1; }
          if (!sln1chk_qs(sa.wstr_sln_1())) { return -1; }
      }
    cch_slot& qs_sl = *r_qs._pnonc_u();

    cch_slot::t_haddrl hsubs;
    cref_t<vnnqueue_t<cch_slot::qs_value, __vecm_tu_selector>, cref_nonlock> r_qus;
    s_ll nmsg = 0, nmsgdone = 0;
    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
      if (!(qs_sl.r_qus.has_ref() && qs_sl.r_haddrl)) { return -1; }
      nmsg = qs_sl.r_qus->navl();
      if (nmsg <= 0) { return 0; }
      r_qus = qs_sl.r_qus;
      if (hsubs.hashx_copy(qs_sl.r_haddrl.ref(), false) < 1) { return -1; }
    }

    res0 = -2;
    for (s_long imsg = 0; imsg < nmsg; ++imsg)
    {
      cch_slot::qs_value& qsv = r_qus->front();
      try { do { // once
        if (!qsv.hmsg) { break; }
        unity& hmsg = *qsv.hmsg._pnonc_u(); if (!hmsg.isAssoc()) { break; }
        const unity sa_msg = hmsg[k_src];
        const unity da_msg = hmsg[k_trg];
        unity sa0lm, da0lm;
        for (s_long isubs = 0; isubs < hsubs.n(); ++isubs)
        {
          try {
            address da; da.set_addr(hsubs(isubs)->k);
              if (da.is_empty()) { continue; }
            hmsg.assoc_set(k_src, sa.addr(), false);
            hmsg.assoc_set(k_trg, da.addr(), false);
            if (da.isLM())
            {
              if (sa0lm.isEmpty())
              {
                if (sa_msg[1] == L"LP") { address a; a.set_addr(sa_msg); if (!a._conv_LP_LM(rses.name_pr)) { continue; } sa0lm.swap((unity&)a.addr()); }
                  else { sa0lm = sa_msg; }
              }
              if (da0lm.isEmpty())
              {
                if (da_msg[1] == L"LP") { address a; a.set_addr(da_msg); if (!a._conv_LP_LM(rses.name_pr)) { continue; } da0lm.swap((unity&)a.addr()); }
                  else { da0lm = da_msg; }
              }
              hmsg.assoc_set(k_src0, sa0lm, false);
              hmsg.assoc_set(k_trg0, da0lm, false);
            }
            else
            {
              hmsg.assoc_set(k_src0, sa_msg, false);
              hmsg.assoc_set(k_trg0, da_msg, false);
            }
          } catch (...) { continue; }
          dispatcher_mt::thread_proxy::_s_write(_r_ths, _name_th, hmsg, qsv.att, st_s_subs_deliver, _fl_msend_anlo_msg | _fl_msend_anlo_att, false, -1, -1, 0, 0);
        }
        if (pnmsent) { ++*pnmsent; }
        ++nmsgdone;
      } while (false); } catch (...) {}
      r_qus->pop_1();
    }

    return nmsgdone > 0 ? 1 : 0;
  } catch (...) { return res0; }
}

  // flags:
  //    same as flags arg. of request, slots_create, dispatcher_mt,
  //    but uses only: ensure_local_subs
  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - operation failed.
  //    -3 - session is closed.
  //    -5 - invalid argument (args and/or retval as required by request type).
  //    -6 - thread already exists.
s_long dispatcher_mt::thread_proxy::_s_thread_create(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& _cfg0, s_long flags_cr) __bmdx_noex
{
  if (!_r_ths) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }

  unity _cfg1; const unity* pcfg = 0; const unity* pcfg_th = 0; const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0; cch_thread* pth = 0;
  mst_semaphore ms; s_long res = -2;
  int _undos = 0; // ORed: 2 - do not continue initialization, 1 - before exit, remove thread object only from hg_threads, 4 - before exit, remove whole thread
  unity ku_thn;
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
      if (!ku_thn.isEmpty()) { return -5; }

      ku_thn = k_th; pcfg_th = &pcfg->assocl_c(i);
    }
    if (ku_thn.isEmpty()) { return -5; }

      // Create/insert thread object, lock for further initialization.
    if (1)
    {
      cref_t<cch_thread> _rth; if (!_rth.create0(1)) { return -2; }
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      s_long res = rses.hg_threads.insert(ku_thn, &e_th);
        if (res < 0) { return -2; }
        if (res == 0) { return -6; } // thread already exists
      bmdx_str::words::swap_bytes(e_th->v, _rth);

        // Primary initialization (for cch_thread being valid after releasing the __lock).
      pth = e_th->v._pnonc_u();
      pth->cb_disp = pcfg_th->u_has(kth_disp, 6) && (*pcfg_th)[kth_disp].isBoolTrue();

      _undos |= 1; // will be set to 0 later, on successful init. completion

      try { ms.set_refs(rses, ku_thn.rstr()); } catch (...) {}
      if (ms.m_th_acquire() < 0) { _undos |= 2; } // init. failed
    }

      // Add slots.
    t_hsubs hsubs_ins, hsubs_outs;
    if (!(_undos & 2))
    {
      res = dispatcher_mt::thread_proxy::_s_add_slots_nl(rses, ku_thn.rstr(), *pcfg_th, hsubs_ins, hsubs_outs, &ms);
      if (res == 1) { _undos = 0;  }
        else { if (res == -1) { res = -5; } else { res = -2; } _undos |= 2; }
    }

      // Release lock.
    ms.release();

      // Update pre-configured subscriptions.
    if (!(_undos & 2))
    {
      s_long resfl = dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_ins, 16|8);
      resfl |= dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_outs, 4|8);
        resfl &= ~s_long(0x10); // "failed to update non-local subscription" is not an error in here
      if (resfl != 0 && !!(flags_cr & dispatcher_mt_flags::ensure_local_subs)) { _undos |= 4; res = -2; }
    }
  } catch (...) {}

  if (_undos)
  {
    if (_undos & 4)
      { try { dispatcher_mt::thread_proxy::_s_thread_remove(_r_ths, ku_thn.rstr(), &ku_thn); } catch (...) {} }
    else if (_undos & 1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      rses.hg_threads.remove(ku_thn);
    }
    if (res >= 0) { res = -2; }
  }
  return res;
}

  // pthn (optional) - if specified, must be same string as thn.
  //    With pthn, in all practically possible conditions (i.e. no concurrent thread holding a semaphore for several seconds),
  //        the function completes thread removal.
  //        If any mem. alloc. fails, the function may skip removal of some of subscription slot addresses from other threads,
  //        but anyway removes the thread thn itself, its slots, and additional objects in the session (e.g. slot names in hg_qs_disp, hg_lpai).
  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - operation failed (top level code only, not expected to occur).
  //    -3 - session is closed.
  //    -5 - invalid argument (args and/or retval as required by request type).
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_thread_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thn, const unity* pthn) __bmdx_noex
{
  if (!_r_ths) { return -2; }
  dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
  if (rses.ses_state != 1) { return -3; }

  // get thread lock
  //  for each slot:
  //    slot removal produces lists for unsubscribing (hsubs_outs, hsubs_ins)
  //    certain slots are removed from hg_qs_disp, hg_lpai
  //  remove thread object
  // release thread lock
  // update subs. lists

  cch_thread* pth = 0;
  t_hsubs hsubs_outs, hsubs_ins;
  try {
    unity __thn1; const unity* pku_thn = pthn; if (!pku_thn) { pku_thn = &__thn1; __thn1 = thn; }
    mst_semaphore ms(rses, thn);
    pth = ms.p_thread(); if (!pth) { return -7; } // under m_th_acquire
    hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_th_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }

    for (s_long j = 0; j < pth->h_sl.n(); ++j)
    {
      try {
        const unity& ku_sl = pth->h_sl(j)->k;
        cref_t<cch_slot>& r_sl = pth->h_sl(j)->v;

        s_long deliv = 0;

          // The following hash map elem. removals do not fail.
          //  cch_thread h_qs_*, cch_session hg_qs_disp, hg_lpai are consistent with cch_thread h_sl.
        deliv = r_sl.ref().qs_delivery_type();
        if (deliv == 3 || r_sl.ref().b_input_lpa())
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
          if (deliv == 3) { rses.hg_qs_disp.remove(ku_sl); }
          if (r_sl.ref().b_input_lpa()) { rses.hg_lpai.remove(ku_sl); }
        }

        if (1)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &r_sl->lkd); if (sizeof(__lock)) {}
          try {
            if (r_sl.ref().r_haddrl)
            {
              const cch_slot::t_haddrl& hs_sl = r_sl.ref().r_haddrl.ref();
              address _a; _a.set_addr_LP(thn, ku_sl);
              if (hs_sl.n() && !_a.is_empty())
              {
                if (deliv != 0) // r_sl (sla) is a qs slot
                {
                  std::wstring sla = _a.wstr_addr();
                  for (s_long i = 0; i < hs_sl.n(); ++i) { try { hsubs_ins.opsub(hs_sl(i)->k.vstr()).opsub(sla) = 0; } catch (...) {} }
                }
                else // r_sl (sla) is non-qs slot that may be subscribed
                {
                  std::wstring sla = _a.wstr_addr();
                  for (s_long i = 0; i < hs_sl.n(); ++i) { try { hsubs_outs.opsub(sla).opsub(hs_sl(i)->k.vstr()) = 0; } catch (...) {} }
                }
              }
            }
          } catch (...) {}
        }
      } catch (...) {}
    }

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      rses.hg_threads.remove(*pku_thn); // does not fail
    }
  } catch (...) { return -2; }

  dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_ins, 1);
  dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs_outs, 2);

  return 1;
}

  // Adds slots to existing thread.
  // Returns:
  //  1 - success.
  //    hsubs_ins - the list, accumulating addresses of new slots, that must be later subscribed to their supplying qs slots. Format:
  //      { <ADDR slot-subscriber>, { <ADDR qs slot-supplier>, 0 } }
  //    hsubs_outs - the list, accumulating addresses of qs slots, that must be later inserted into input lists subscribed (pi, qi) slots. Format:
  //      { <ADDR slot-subscriber>, { <ADDR qs slot-supplier>, 0 } }
  //    NOTE on _s_add_slots_nl success, hsubs_* must be passed to _s_update_subs_lists to complete operation.
  //    NOTE on error, all dispatcher structure changes are canceled.
  //      But, hsubs_* may be left modified. It must not be used for any further updates.
  //  -1 - bad arg. (_name_th empty, slotscfg invalid).
  //  -2 - failure (mem. alloc.).
  //  -3 - thread does not exist.
  //  -4 - one of new slot names is not unique.
s_long dispatcher_mt::thread_proxy::_s_add_slots_nl(cch_session& rses, const std::wstring& _name_th, const unity& slotscfg, t_hsubs& hsubs_ins, t_hsubs& hsubs_outs, mst_semaphore* _pms0) __bmdx_noex
{
  try {
    unity k_slots(L"slots");
    if (!(_name_th.length() > 0 && slotscfg.compatibility() > 0 && (slotscfg.isAssoc() || slotscfg.isEmpty()))) { return -1; }

    mst_semaphore _ms0;
    if (!_pms0)
    {
      _ms0.set_refs(rses, _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = _ms0.m_sl_acquire(); if (res < 0) { return res == -3 ? -3 : -2; } if (res > 0) { break; } sleep_mcs(50); }
    }
    mst_semaphore& ms = _pms0 ? *_pms0 : _ms0;
    cch_thread* pth = ms.p_thread(); if (!pth) { return -3; } // under m_sl_acquire

    unity u_empty;
    const unity& sl_cfgroot = slotscfg.u_has(k_slots, 6) ? slotscfg[k_slots] : u_empty;
    struct _aux_qsh
    {
      hashx<unity, unity> h_qsd_, h_lpai_;
      hashx<unity, s_long> h_qst_, h_qsimm_;
    };
    _aux_qsh qsh;
    hashx<unity, cref_t<cch_slot> > h_sl2;
    hashx<unity, s_long>& h_qst = pth->h_qs_thread; hashx<unity, s_long>& h_qsimm = pth->h_qs_imm;
    unity ku_input_all(L"input all"), ku_input_lpa(L"input lpa"), ku_input_qsa(L"input qsa");
    for (s_long pos = sl_cfgroot.assocl_first(); pos != sl_cfgroot.assocl_noel(); pos = sl_cfgroot.assocl_next(pos))
    {
      const unity& ku_sl0 = sl_cfgroot.assocl_key(pos); if (ku_sl0.isEmpty() || ku_sl0.lenstr() == 0) { continue; }
      std::wstring k_sl, sln1, slntail;
        s_long res = __recode_slotname(ku_sl0.vstr(), &k_sl, &sln1, &slntail);
          if (res != 1) { return res == -1 ? -1 : -2; }
          if (!sln1chk_main(sln1)) { return -1; }
      const unity ku_sl = k_sl;

        // Create uninitialized slot.
      cref_t<cch_slot>& rr_sl = h_sl2[ku_sl];
        if (rr_sl.has_ref()) { return -4; }
        if (!rr_sl.create0(true)) { return -2; }
        cch_slot& new_sl = *rr_sl._pnonc_u();

        // Initialize slot data according with slot type.
      const unity* psc = &sl_cfgroot.assocl_c(pos);
        if (!psc->isAssoc()) { psc = 0; }

      s_long cf_inp = 0x4; // allowed inputs (used only by normal inputs qs, qi, pi, pbi, qbi), default: accept input from all threads
        if (psc && psc->u_has(ku_input_all, 6)) { const unity& x = (*psc)[ku_input_all]; if (!x.isBool()) { return -1; } if (x.isBoolFalse()) { cf_inp &= ~s_long(0x4); } }
        if (psc && psc->u_has(ku_input_lpa, 6)) { const unity& x = (*psc)[ku_input_lpa]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { cf_inp |= 0x8; } }

      s_ll capmin = 0, capmax = -2, caporig = 0;
      if (sln1[0] == L'q' || sln1[0] == L'h') // // qi, qbi, qs, hbo - determine queue capacity parameters
      {
        unity k_size(L"size"), k_capmin(L"capmin"), k_capmax(L"capmax"), k_caporig(L"caporig");
        const bool bk_size = psc && psc->u_has(k_size, 6);
        const bool bk_capmin = psc && psc->u_has(k_capmin, 6);
        const bool bk_capmax = psc && psc->u_has(k_capmax, 6);
        const bool bk_caporig = psc && psc->u_has(k_caporig, 6);
        if (bk_capmin || bk_capmax || bk_caporig)
        {
          if (bk_capmin != bk_capmax) { return -1; }
          if (bk_capmin)
          {
            const unity& mi = (*psc)[k_capmin]; if (!mi.isInt()) { return -1; }
            const unity& ma = (*psc)[k_capmax]; if (!ma.isInt()) { return -1; }
            capmin = mi.vint();
            capmax = ma.vint();
          }
          if (bk_caporig)
          {
            const unity& rc = (*psc)[k_caporig]; if (!rc.isInt()) { return -1; }
            caporig = rc.vint();
          }
        }
        else if (bk_size)
        {
          const unity& rs = (*psc)[k_size]; if (!rs.isInt()) { return -1; }
          capmax = caporig = rs.vint();
          if (capmax < 3) { return -1; }
        }
      }

      if (1) { bool b_stop = false; for (size_t i = 0; i < 4; ++i) { if (i >= sln1.size() || sln1[i] == L'_') { b_stop = true; }  unsigned char c = 0; if (!b_stop) { c = (unsigned char)sln1[i]; } new_sl.type_str[i] = c; } }
      if (sln1[0] == L'q') // qi, qbi, qs
      {
        if (sln1[1] == L'i')
        {
          new_sl.ccfgflags = cf_inp;
          new_sl.r_qumi.create0(0); if (!(1 == new_sl.r_qumi->set_cap_hints(capmin, capmax) && 1 == new_sl.r_qumi->set_rsv(caporig))) { return -2; }
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

          if (!new_sl.r_haddrl) { if (!new_sl.r_haddrl.create0(true)) { return -2; } }

          unity k_output(L"output"), k_output_fixed(L"output fixed");
          s_long qsoutp = 0x0;
          if (psc && psc->u_has(k_output, 6))
          {
            const unity& subs = (*psc)[k_output]; if (!subs.isAssoc()) { return -1; }
            std::wstring sa_s; if (1) { unity a, a2; a.arr_init<utUnityArray>(1); __append_vals(a, L"LP"); __append_vals(a, _name_th); __append_vals(a, paramline().decode1v(k_sl, a2)); sa_s = paramline().encode1v(a); }
            for (s_long pos = subs.assocl_first(); pos != subs.assocl_noel(); pos = subs.assocl_next(pos))
            {
              const unity& ku_addr0 = subs.assocl_key(pos); if (ku_addr0.isEmpty() || ku_addr0.lenstr() == 0) { continue; }
              int res; address suba;
              res = suba.set_addr(ku_addr0.vstr()); if (res != 1) { return res == -1 ? -1 : -2; }
              std::wstring dsln1 = suba.wstr_sln_1();
                if (!sln1chk_subscriber(dsln1)) { return -1; }
                if (sln1_root(sln1) != sln1_root(dsln1)) { return -1; }
                if (slntail.length() > 0 && suba.wstr_sln_tail() != slntail) { return -1; }
              std::wstring suba_s = suba.wstr_addr();
              if (new_sl.r_haddrl->insert(suba_s) != 1) { return -1; }
              hsubs_outs.opsub(suba_s).opsub(sa_s) = 0;
            }
          }
          if (psc && psc->u_has(k_output_fixed, 6)) { const unity& x = (*psc)[k_output_fixed]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { qsoutp |=  0x10; } }

          new_sl.ccfgflags = qsd | cf_inp | qsoutp;

          if (qsd == 3)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
            if (rses.hg_qs_disp.find(ku_sl) || qsh.h_qsd_.find(ku_sl)) { return -4; }
            qsh.h_qsd_[ku_sl] = _name_th;
          }
          else if (qsd == 2) { if (h_qst.find(ku_sl) || qsh.h_qst_.find(ku_sl)) { return -4; } qsh.h_qst_[ku_sl] = 0; }
          else { if (h_qsimm.find(ku_sl) || qsh.h_qsimm_.find(ku_sl)) { return -4; } qsh.h_qsimm_[ku_sl] = 0; }

          new_sl.r_qus.create0(0); if (!(1 == new_sl.r_qus->set_cap_hints(capmin, capmax) && 1 == new_sl.r_qus->set_rsv(caporig))) { return -2; }
        }
        else if (sln1[1] == L'b')
        {
          new_sl.ccfgflags = cf_inp;
          new_sl.r_qubci.create0(0); if (!(1 == new_sl.r_qubci->set_cap_hints(capmin, capmax) && 1 == new_sl.r_qubci->set_rsv(caporig))) { return -2; }
        }
      }
      else if (sln1[0] == L'p') // pi, po, pbi, pbo
      {
        if (sln1[1] == L'i')  { new_sl.ccfgflags = cf_inp; } // pi
        else if (sln1[1] == L'b' && sln1[2] == L'i')  { new_sl.ccfgflags = cf_inp; } // pbi
      }
      else if (sln1[0] == L'h')  // hbo
      {
        new_sl.ccfgflags = cf_inp;
        new_sl.r_qubci.create0(0); if (!(1 == new_sl.r_qubci->set_cap_hints(capmin, capmax) && 1 == new_sl.r_qubci->set_rsv(caporig))) { return -2; }
        new_sl.r_haddrl.create0(0);
      }
      else { return -1; }

      if (sln1[1] == L'i' && (sln1[0] == L'p' || sln1[0] == L'q')) // pi, qi (here: as subscribers)
      {
        if (psc && psc->u_has(ku_input_qsa, 6))
        {
          std::wstring sa_s; if (1) { unity a, a2; a.arr_init<utUnityArray>(1); __append_vals(a, L"LP"); __append_vals(a, _name_th); __append_vals(a, paramline().decode1v(k_sl, a2)); sa_s = paramline().encode1v(a); }
          const unity& qsa_u = (*psc)[ku_input_qsa]; if (!qsa_u.isAssoc()) { return -1; }
          for (s_long pos = qsa_u.assocl_first(); pos != qsa_u.assocl_noel(); pos = qsa_u.assocl_next(pos))
          {
            const unity& ku_addr0 = qsa_u.assocl_key(pos); if (ku_addr0.isEmpty() || ku_addr0.lenstr() == 0) { continue; }
            int res; address qsa;
            res = qsa.set_addr(ku_addr0.vstr()); if (res != 1) { return res == -1 ? -1 : -2; }
            std::wstring qssln1 = qsa.wstr_sln_1();
              if (!sln1chk_main_qs(qssln1)) { return -1; }
              if (sln1_root(qssln1) != sln1_root(sln1)) { return -1; }
              std::wstring qsa_slntail = qsa.wstr_sln_tail();
              if (qsa_slntail.length() > 0 && qsa_slntail != slntail) { return -1; }

            if (!new_sl.r_haddrl) { if (!new_sl.r_haddrl.create0(true)) { return -2; } }
            std::wstring qsa_s = qsa.wstr_addr();
            if (new_sl.r_haddrl->insert(qsa_s) != 1) { return -1; }
            hsubs_ins.opsub(sa_s).opsub(qsa_s) = 0;
          }
        }
      }

      if (new_sl.b_input_lpa())
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
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
      critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
      bool bf2 = false; s_long n_d1(0), n_d2(0);

        // Insert slot names and their owning threads into the list of LPA queues (qs queues with delivery = disp).
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

s_long dispatcher_mt::thread_proxy::_s_slots_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotnames, const std::wstring& _name_th, s_long flags_rmv) __bmdx_noex
{
  if (!_r_ths.has_ref()) { return -2; }
  if (_r_ths.ref().ses_state != 1) { return -3; }
  try {
    unity anames;
    if (slotnames.isString()) { anames.array(slotnames); }
    else if (slotnames.isArray() && !!(flags_rmv & dispatcher_mt_flags::array_of_slotnames)) // array of slots
      { anames.array(); for (s_long i = slotnames.arrlb(); i <= slotnames.arrub(); ++i) { anames.arr_append(slotnames[i]); } }
    else if (slotnames.isArray()) // single slot name
      { anames.array(slotnames); }
    else if (slotnames.isAssoc())
      { anames.array(); for (s_long i = slotnames.assocl_first(); i != slotnames.assocl_noel(); i = slotnames.assocl_next(i)) { anames.arr_append(slotnames.assocl_key(i)); } }
    else { return -1; }

    bool bfail = false, bnotf = false;
    s_long res = -2;
    for (s_long i = 1; i <= anames.arrub(); ++i)
    {
      res = _s_slot_remove(_r_ths, anames[i], _name_th);
      if (res <= -3) { return res; }
      if (res == -2) { bfail = true; continue; }
      if (res == -1 || res == 0) { bnotf = true; continue; }
    }
    if (bfail) { return 0; }
    if (bnotf) { return 1; }
    return 2;
  } catch (...) { return -2; }
}
s_long dispatcher_mt::thread_proxy::_s_slot_remove(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& slotname0, const std::wstring& _name_th) __bmdx_noex
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
    std::wstring k_sl; s_long res = __recode_slotname(sln, &k_sl); if (res != 1) { return res == -1 ? -1 : -2; }
    const unity ku_sl = k_sl;

    s_long deliv = 0; cref_t<cch_slot> r_sl;
    if (1)
    {
      mst_semaphore ms(rses, _name_th);
      hangdet hd; while (true) { if (hd) { return -2; } int res = ms.m_sl_acquire(); if (res < 0) { return -2; } if (res > 0) { break; } sleep_mcs(50); }
      cch_thread* pth = ms.p_thread(); if (!pth) { return -2; } // under m_sl_acquire
      ms.r_sl(k_sl, r_sl); if (!r_sl) { return 0; }
      pth->h_sl.remove(ku_sl);

        // The following hash map elem. removals do not fail.
        //  cch_thread h_qs_*, cch_session hg_qs_disp, hg_lpai are consistent with cch_thread h_sl.
      deliv = r_sl.ref().qs_delivery_type();
      if (deliv == 1)
      {
        pth->h_qs_imm.remove(ku_sl);
        if (r_sl.ref().b_input_lpa()) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {} rses.hg_lpai.remove(ku_sl); }
      }
      else if (deliv == 2)
      {
        pth->h_qs_thread.remove(ku_sl);
        if (r_sl.ref().b_input_lpa()) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {} rses.hg_lpai.remove(ku_sl); }
      }
      else if (deliv == 3 || r_sl.ref().b_input_lpa())
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_ths); if (sizeof(__lock)) {}
        if (deliv == 3) { rses.hg_qs_disp.remove(ku_sl); }
        if (r_sl.ref().b_input_lpa()) { rses.hg_lpai.remove(ku_sl); }
      }
    }

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &r_sl->lkd); if (sizeof(__lock)) {}
      if (r_sl.ref().r_haddrl)
      {
        const cch_slot::t_haddrl& hs_sl = r_sl.ref().r_haddrl.ref();
        address _a; _a.set_addr_LP(_name_th, ku_sl);
        if (hs_sl.n() && !_a.is_empty())
        {
          if (deliv) // r_sl is a qs slot
          {
            std::wstring sla = _a.wstr_addr();
            try { for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs.opsub(hs_sl(i)->k.vstr()).opsub(sla) = 0; } } catch (...) {}
          }
          else // r_sl may have subscriptions
          {
            std::wstring sla = _a.wstr_addr();
            try { for (s_long i = 0; i < hs_sl.n(); ++i) { hsubs.opsub(sla).opsub(hs_sl(i)->k.vstr()) = 0; } } catch (...) {}
          }
        }
      }
    }

    if (hsubs.n()) { dispatcher_mt::thread_proxy::_s_update_subs_lists(_r_ths, hsubs, deliv ? 1 : 2); }

    return 1;
  } catch (...) { return -2; }
}

  // Update input list for each of the given subscribers, or remove subscriber from all suppliers output lists.
  //    This function processes subscriber slots in a loop, all necessary locking is done in each iteration.
  //    hsubs: similar to hsubs_* in _s_add_slots_nl:
  //      { <ADDR slot-subscriber>, { <ADDR qs slot-supplier>, 0 } }
  //    actions:
  //        4 - add local supplier (qs) to any subscriber's input list.
  //        16 - add local subscriber (pi, qi) to any supplier's (qs) output list (similar to _s_subscribe with st_client).
  //        1 - remove local supplier (qs) from any subscriber's input list. Suppresses 4.
  //        2 - remove local subscriber (pi, qi) from any supplier's (qs) output list (similar to _s_subscribe with st_client).
  //          Flags may be combined (1 with 2; 4 with 16). Removal flags suppress appropriate adding flags.
  //        8 - extract non-local peer names, and pre-create peer trackers, before executing other flags.
  //          (this flag may be combined with all other flags.)
  // Returns:
  //    0 on success.
  //    > 0: ORed flags, indicating type of failures occurred:
  //      0x1 - complete failure, no changes (e.g. if no session),
  //      0x2 - common-case failure, part of changes are made,
  //      0x4 - an invalid address detected,
  //      0x8 - failed to update local subscription,
  //      0x10 - failed to update non-local subscription (LMSC request failed).
s_long dispatcher_mt::thread_proxy::_s_update_subs_lists(cref_t<dispatcher_mt::cch_session>& _r_ths, const t_hsubs& hsubs, int actions) __bmdx_noex
{
  if (!_r_ths) { return 0x1; }
  cch_session& rses = *_r_ths._pnonc_u();
  if (hsubs.n() <= 0) { return 0x0; }
  s_long resfl = 0x0;
  if (!!(actions & 8) && rses.lmsc)
  {
    hashx<std::wstring, s_long, hashx_common::kf_basic<std::wstring>, __vecm_tu_selector> hpn;
    for (s_long i1 = 0; i1 < hsubs.n(); ++i1)
    {
      if (1) { const std::wstring& k = hsubs(i1)->k; if (k.find(L"LM") != nposw) { address suba; suba.set_addr(k); try { if (suba.isLM()) { hpn.opsub(suba.wstr_pn()); } } catch (...) {} } }
      const t_hsubs_L2& h_qsa = hsubs(i1)->v;
      for (s_long i2 = 0; i2 < h_qsa.n(); ++i2) { try { const std::wstring& k = h_qsa(i2)->k; if (k.find(L"LM") == nposw) { continue; } address qsa; qsa.set_addr(k); if (qsa.isLM()) { hpn.opsub(qsa.wstr_pn()); } } catch (...) {} }
    }
    if (hpn.n() > 0)
    {
      for (s_long i = 0; i < hpn.n(); ++i) { rses.lmsc->clt_ensure_comm(hpn(i)->k, -1); } // try comm. with all peers once
      const double t0 = clock_ms();
      for (s_long i = 0; i < hpn.n(); ++i)
      {
        rses.lmsc->clt_ensure_comm(hpn(i)->k, 0); // internally, this may wait for some time
        if (clock_ms() - t0 >= 1.5 * __lm_slot_controller_qum_send_avl_wait_toms) { break; } // after this time, comm. with all really existing peers is already established
      }
    }
  }
  for (s_long i1 = 0; i1 < hsubs.n(); ++i1)
  {
    bool bf = false;
    try {
      const t_hsubs_L2& h_qsa = hsubs(i1)->v;
      address suba; suba.set_addr(hsubs(i1)->k);
        if (!sln1chk_subscriber(suba.wstr_sln_1())) { resfl |= 0x4; continue; }
      for (s_long i2 = 0; i2 < h_qsa.n(); ++i2)
      {
        address qsa; qsa.set_addr(h_qsa(i2)->k); if (qsa.is_empty()) { resfl |= 0x4; continue; }
        s_long res = 0; bool b_action = false; s_long oploc = 0;
        if (actions & 1)
        {
          b_action = true;
          res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, 2, st_update_subs_input, qsa, suba, idgen(), 0, &oploc);
        }
        else if (actions & 4)
        {
          b_action = true;
          res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, 1, st_update_subs_input, qsa, suba, idgen(), 0, &oploc);
        }
        if (b_action) { if (res == -1) { resfl |= 0x4; } else if (res < 0) { if (oploc == 2) { resfl |= 0x10; } else { resfl |= (oploc == 1 ? 0x8 : 0x2); } } }

        res = 0; b_action = false;
        if (actions & 2)
        {
          b_action = true;
          res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, 2, st_update_subs_output, qsa, suba, idgen(), 0, &oploc);
        }
        else if (actions & 16)
        {
          b_action = true;
          res = dispatcher_mt::thread_proxy::_s_subscribe(_r_ths, 1, st_update_subs_output, qsa, suba, idgen(), 0, &oploc);
        }
        if (b_action) { if (res == -1) { resfl |= 0x4; } else if (res < 0) { if (oploc == 2) { resfl |= 0x10; } else { resfl |= (oploc == 1 ? 0x8 : 0x2); } } }
      }
    } catch (...) { bf = true; }
    if (bf) { resfl |= 0x2; }
  }
  return resfl;
}

s_ll dispatcher_mt::thread_proxy::idgen()
  { static s_ll x(make_id0()); critsec_t<lks_idgen> __lock(0, -1); if (sizeof(__lock)) {} return ++x; }
s_ll dispatcher_mt::thread_proxy::make_id0()
  { s_ll dt = s_ll(1.e4 * (clock_ms() - idgen_t0)) & 0xffffffffll; return (((s_ll(d_nowutc().f()) & 0x7ffffe) + 1) << 40) + (dt << 16); }
void dispatcher_mt::thread_proxy::_s_thh_signal_stop(cch_session& rses) __bmdx_noex
  { rses.th_lqsd.signal_stop(); rses.th_lmsc.signal_stop(); }
bool dispatcher_mt::thread_proxy::_s_thh_active(cch_session& rses) __bmdx_noex
  { return rses.th_lqsd || rses.th_lmsc; }

void dispatcher_mt::thread_proxy::_s_disp_ctor(dispatcher_mt* pdisp, arrayref_t<wchar_t> process_name, const unity& _cfg0, s_long flags_ctor) __bmdx_noex
{
  if (!(pdisp && process_name.is_valid())) { return; }
  if (!(_cfg0.isString() || _cfg0.isEmpty() || (_cfg0.isAssoc() && _cfg0.compatibility() > 0))) { return; }
  unity _cfg1; try { if (_cfg0.isString()) { paramline().decode_tree(_cfg0.vstr(), _cfg1, 0x3a); } else if (_cfg0.isEmpty()) { _cfg1.u_clear(utHash); } } catch (...) { return; }
  const unity& cfg = _cfg1.isHash() ? _cfg1 : _cfg0;

  if (!pdisp->_r_ths.create0(true)) { return; }
    pdisp->_r_ths->nprx += 1;
  cch_session& rses = *pdisp->_r_ths._pnonc_u();

  bool b = true;
  try { while (true) { // once

    const unity kg_lqsd_dt(L"__lqsd_dt");
    const unity kg_exitmode(L"__exitmode");
    const unity kg_msend_anlo(L"__msend_anlo");
    const unity kg_mget_anlo(L"__mget_anlo");
    const unity kg_thm_lqsd(L"__thm_lqsd");
    const unity kg_thm_lmsc(L"__thm_lmsc");
    const unity kg_thm_nsc(L"__thm_nsc");
    const unity kg_thm_cdcc(L"__thm_cdcc");
    const unity kg_lmsc_nb_qum(L"__lmsc_nb_rqum");
    const unity kg_lmsc_nb_qinfo(L"__lmsc_nb_rqinfo");
    const unity kg_lmsc_peers(L"__lmsc_peers");
    const unity kg_lmsc_chsbin(L"__lmsc_chsbin");

    const unity kth_disp(L"disp");

    if (!thread_proxy::pnchk(process_name)) { b = false; break; }
    rses.name_pr.assign(process_name.pd(), _t_wz(process_name.n()));

    if (1) { meta::s_ll q = +cfg / kg_exitmode / 2ll; if (!(q >= 0 && q <= 2)) { b = false; break; } rses.exitmode = (int)q; }
    if (1) { meta::s_ll q = +cfg / kg_lqsd_dt / -1ll; if (q >= 0) { rses.lqsd_dt = q; } }
    if (1) { meta::s_ll q = +cfg / kg_msend_anlo / -1ll; if (!(q >= -1 && q <= 1)) { b = false; break; } if (q >= 0) { const s_long m = _frqperm_msend_anlo; volatile s_long& f = rses.frqperm; if (q > 0) { f |= m; } else { f &= ~m; } } }
    if (1) { meta::s_ll q = +cfg / kg_mget_anlo / -1ll; if (!(q >= -1 && q <= 1)) { b = false; break; } if (q >= 0) { const s_long m = _frqperm_mget_anlo; volatile s_long& f = rses.frqperm; if (q > 0) { f |= m; } else { f &= ~m; } } }
    if (1) { meta::s_ll q = +cfg / kg_thm_lqsd / -1ll; if (q > 2) { b = false; break; } if (q >= 0) { rses.__thm_lqsd = (unsigned char)q; } }
    if (1) { meta::s_ll q = +cfg / kg_thm_lmsc / -1ll; if (q > 2) { b = false; break; } if (q >= 0) { rses.__thm_lmsc = (unsigned char)q; } }
    if (1) { meta::s_ll q = +cfg / kg_thm_nsc / -1ll; if (q > 2) { b = false; break; } if (q >= 0) { rses.__thm_nsc = (unsigned char)q; } }
    if (1) { meta::s_ll q = +cfg / kg_thm_cdcc / -1ll; if (q > 2) { b = false; break; } if (q >= 0) { rses.__thm_cdcc = (unsigned char)q; } }
    s_ll _lmsc_nb_qum = +cfg / kg_lmsc_nb_qum / -1ll;
    s_ll _lmsc_nb_qinfo = +cfg / kg_lmsc_nb_qinfo / -1ll;
    unity _lmsc_peers = +cfg / kg_lmsc_peers / unity(); if (!(_lmsc_peers.isEmpty() || _lmsc_peers.utype() == utUnityArray || _lmsc_peers.utype() == utStringArray || _lmsc_peers.isAssoc())) { _lmsc_peers.clear(); }
    bool _lmsc_chsbin = +cfg / kg_lmsc_chsbin / false;

    if (flags_ctor & dispatcher_mt_flags::disp_off_th_lqsd) { rses.__thm_lqsd = 0; }
    if (flags_ctor & dispatcher_mt_flags::disp_off_th_lmsc) { rses.__thm_lmsc = 0; }


      // Lock to make process name unique in the system.
    std::string pnutf("bmdx_disp\t"); pnutf += wsToBsUtf8(rses.name_pr);
    rses.gm.~critsec_gn(); new (&rses.gm) bmdx_shm::critsec_gn((bmdx_shm::t_name_shm(__bmdx_dispatcher_name_prefix) += make_fixed_utf8_name(rses.name_pr)).c_str());

    if (!rses.gm.lock()) { b = false; break; }


      // Initialize threads and slots.
    thread_proxy::t_hsubs hsubs_ins, hsubs_outs;
    for (s_long i = cfg.assocl_first(); i != cfg.assocl_noel(); i = cfg.assocl_next(i))
    {
      const std::wstring k_th(cfg.assocl_key(i).vstr());
      if (!thread_proxy::thnchk(k_th)) { continue; }

      cref_t<cch_thread> _rth; if (!_rth.create0(1)) { b = false; break; }
      const hashx<unity, cref_t<cch_thread> >::entry* e_th = 0;
        if (rses.hg_threads.insert(k_th, &e_th) != 1) { b = false; break; }
      bmdx_str::words::swap_bytes(e_th->v, _rth);

      const unity& th_cfg = cfg.assocl_c(i);
      e_th->v->cb_disp = th_cfg.u_has(kth_disp, 6) && th_cfg[kth_disp].isBoolTrue();
      if (dispatcher_mt::thread_proxy::_s_add_slots_nl(rses, k_th, th_cfg, hsubs_ins, hsubs_outs) != 1) { b = false; break; }
    }
    if (!b) { break; }

    if (rses.__thm_lmsc > 0)
    {
      if (!rses.lmsc.create4(1, rses.name_pr, _lmsc_nb_qum, _lmsc_nb_qinfo, _lmsc_peers)) { b = false; break; }
      rses.lmsc->clt_setparam_chsbin(_lmsc_chsbin);
      rses.lmsc->det_init(); // 1st-time init., not critical if fails: it is periodically repeated by th_lmsc_impl, when necessary
    }
    rses.__set_ses_state(-1); // setting session state to "late initialization", to be able to handle subscription requests from IPC
    if (rses.__thm_lqsd > 0) { if (!rses.th_lqsd.start_auto<thread_proxy::th_lqsd_impl>(pdisp->_r_ths)) { b = false; break; } }
    if (rses.__thm_lmsc > 0) { if (!rses.th_lmsc.start_auto<thread_proxy::th_lmsc_impl>(pdisp->_r_ths)) { b = false; break; } }

      // NOTE the following updates qs subscribers' input lists both locally and non-locally
    s_long resfl = dispatcher_mt::thread_proxy::_s_update_subs_lists(pdisp->_r_ths, hsubs_ins, 16|8);
    resfl |= dispatcher_mt::thread_proxy::_s_update_subs_lists(pdisp->_r_ths, hsubs_outs, 4|8);
      resfl &= ~s_long(0x10); // "failed to update non-local subscription" is not an error in here
    if (resfl != 0 && !!(flags_ctor & dispatcher_mt_flags::ensure_local_subs)) { b = false; break; }

    break;
  } } catch (...) { b = false; }

  if (b) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_nprx); if (sizeof(__lock)) {} rses.__set_ses_state(1); } // starting the dispatcher session
  else
  {
    rses.__set_ses_state(0); // dispatcher session ends due to initialization error
    if (1) { critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_nprx); if (sizeof(__lock)) {} rses.nprx -= 1; }
    _s_thh_signal_stop(rses); while (_s_thh_active(rses)) { sleep_mcs(100, 1); }
    pdisp->_r_ths.clear();
  }
}
void dispatcher_mt::thread_proxy::_s_disp_dtor(dispatcher_mt* pdisp) __bmdx_noex
{
  if (!pdisp) { return; }
  if (pdisp->_r_ths)
  {
    cch_session& rses = *pdisp->_r_ths._pnonc_u();
    int e = rses.exitmode;
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_nprx); if (sizeof(__lock)) {}
    rses.nprx -= 1; if (e > 0 || rses.nprx <= 0) { rses.__set_ses_state(0); _s_thh_signal_stop(rses); } // dispatcher session ends due to ~dispatcher_mt()
  }
  if (pdisp->_r_ths)
  {
    cch_session& rses = *pdisp->_r_ths._pnonc_u();
    int e = rses.exitmode;
    if (e == 2) { while (rses.nprx > 0 || _s_thh_active(rses)) { sleep_mcs(100, 1); } } // NOTE this loop exits only when all clients session references are released (i.e. may wait while any client thread is busy or sleeping, or infinitely if it's been terminated by force).
    pdisp->_r_ths.clear();
  }
}
  // Ret. vals. are same as _s_proxy_init().
  // Ret. vals. are compatible with _s_request().
  //    1 - success.
  //    -2 - failed.
  //    -3 - disp. session is closed.
  //    -5 - invalid thread name.
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_proxy_new(const cref_t<dispatcher_mt::cch_session>& _r_ths, unity& retval, arrayref_t<wchar_t> _name_th) __bmdx_noex
{
  unity temp; retval.swap(temp);
  if (!_r_ths) { return -2; } if (_r_ths.ref().ses_state != 1) { return -3; }
  if (!_name_th.is_valid()) { return -2; }

  dispatcher_mt::thread_proxy* p(0); try { p = new dispatcher_mt::thread_proxy(); } catch (...) {}
    if (!p) { return -2; }
  s_long res = dispatcher_mt::thread_proxy::_s_proxy_init(*p, _r_ths, _name_th);
    if (res < 0) { try { delete p; } catch (...) {} return res; }
  try { retval.set_obj<thread_proxy, o_interfaces<thread_proxy, i_dispatcher_mt> >(*p, true); } catch (...) { try { delete p; } catch (...) {} return -2; }
  return 1;
}
  //    1 - success.
  //    -2 - failed.
  //    -3 - disp. session is closed.
  //    -5 - invalid thread name.
  //    -7 - thread does not exist.
s_long dispatcher_mt::thread_proxy::_s_proxy_init(thread_proxy& x, const cref_t<dispatcher_mt::cch_session>& _r_ths, arrayref_t<wchar_t> thread_name) __bmdx_noex
{
  if (!thnchk(thread_name)) { return -5; }
  if (!_r_ths) { return -2; }
  try {
    std::wstring process_name;
    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths->lkd_disp_ths); if (sizeof(__lock)) {}
      if (_r_ths.ref().ses_state != 1) { return -3; }
      process_name = _r_ths.ref().name_pr;
      if (!_r_ths.ref().hg_threads.find(thread_name)) { return -7; }
    }
    x._name_th.assign(thread_name.pd(), _t_wz(thread_name.n()));
    x._name_pr = process_name;

    if (!x.mtrk_htracking.create_info(false)) { return -2; }
    cref_t<t_htracking_proxy> htr; if (!htr.create0(1)) { return -2; }
    x.mtrk_htracking.set_ref(htr);

    if (1)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths->lkd_disp_nprx); if (sizeof(__lock)) {}
      x._r_ths = _r_ths;
      x._r_ths->nprx += 1;
    }
    return 1;
  } catch (...) { return -2; }
}



dispatcher_mt::dispatcher_mt(arrayref_t<char> process_name, const unity& cfg, s_long flags_ctor) __bmdx_noex       { try { dispatcher_mt::thread_proxy::_s_disp_ctor(this, process_name.is_valid() ? bsToWs(process_name.pd(), process_name.n()) : L"", cfg, flags_ctor); } catch (...) {} }
dispatcher_mt::dispatcher_mt(arrayref_t<wchar_t> process_name, const unity& cfg, s_long flags_ctor) __bmdx_noex        { dispatcher_mt::thread_proxy::_s_disp_ctor(this, process_name, cfg, flags_ctor); }
dispatcher_mt::~dispatcher_mt() __bmdx_noex        { dispatcher_mt::thread_proxy::_s_disp_dtor(this); }
bool dispatcher_mt::is_valid() const __bmdx_noex        { return _r_ths; }
bool dispatcher_mt::has_session() const __bmdx_noex        { return _r_ths && _r_ths.ref().ses_state == 1; }
void dispatcher_mt::end_session() __bmdx_noex
{  if (_r_ths)  {
    cch_session& rses = *_r_ths._pnonc_u();
    critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkd_disp_nprx); if (sizeof(__lock)) {}
    rses.__set_ses_state(0); dispatcher_mt::thread_proxy::_s_thh_signal_stop(rses); // dispatcher session ends because dispatcher_mt::end_session() is called y client
} }
bool dispatcher_mt::frqperm(s_long& f, bool b_get, bool b_set) const __bmdx_noex
{
  if (!has_session()) { return false; }
  critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths->lkd_disp_ths); if (sizeof(__lock)) {}
  volatile s_long& x = _r_ths->frqperm; int x0 = x; if (b_set) { x = f; } if (b_get) { f = x0; }
  return true;
}
s_long dispatcher_mt::new_proxy(unity& dest, arrayref_t<wchar_t> _name_th) const __bmdx_noex { return thread_proxy::_s_proxy_new(_r_ths, dest, _name_th); }
s_long dispatcher_mt::new_proxy(unity& dest, arrayref_t<char> _name_th) const __bmdx_noex { if (!_name_th.is_valid()) { return -2; } try { return thread_proxy::_s_proxy_new(_r_ths, dest, bsToWs(_name_th.pd(), _name_th.n())); } catch (...) {} return -2; }

s_long dispatcher_mt::request(s_long rt, unity& retval, const unity& args, s_long flags) __bmdx_noex
  { try { return thread_proxy::_s_request(0, _r_ths, rt, retval, args, -1, flags); } catch (...) { return -2; } }

} // bmdx ns

#else // !bmdx_part_dispatcher_mt
  s_long cv_ff::cv_mget::Fmget_set_retvals(unity* phm, const cref_t<t_stringref>* patt, unity* pretmsg, cref_t<t_stringref>* retatt, s_long proc_flags) { return -2; }
#endif // bmdx_part_dispatcher_mt

#ifdef _MSC_VER
  #pragma warning(pop)
#endif
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif // #ifndef __bmdx_main_impl_skip
