#ifndef UTILITIES_H__
#define UTILITIES_H__

// printf macros for size_t, in the style of inttypes.h
#ifdef _LP64
#define __PRIS_PREFIX "z"
#else
#define __PRIS_PREFIX
#endif

// Use these macros after a % in a printf format string
// to get correct 32/64 bit behavior, like this:
// size_t size = records.size();
// printf("%"PRIuS"\n", size);

#define PRIdS __PRIS_PREFIX "d"
#define PRIxS __PRIS_PREFIX "x"
#define PRIuS __PRIS_PREFIX "u"
#define PRIXS __PRIS_PREFIX "X"
#define PRIoS __PRIS_PREFIX "o"

#include "base/mutex.h"  // This must go first so we get _XOPEN_SOURCE

#include <string>

#include <googlelog/logging.h>

#if defined(GOOGLELOG_OS_WINDOWS)
# include "port.h"
#endif

#include "config.h"

// There are three different ways we can try to get the stack trace:
//
// 1) The libunwind library.  This is still in development, and as a
//    separate library adds a new dependency, but doesn't need a frame
//    pointer.  It also doesn't call malloc.
//
// 2) Our hand-coded stack-unwinder.  This depends on a certain stack
//    layout, which is used by gcc (and those systems using a
//    gcc-compatible ABI) on x86 systems, at least since gcc 2.95.
//    It uses the frame pointer to do its work.
//
// 3) The gdb unwinder -- also the one used by the c++ exception code.
//    It's obviously well-tested, but has a fatal flaw: it can call
//    malloc() from the unwinder.  This is a problem because we're
//    trying to use the unwinder to instrument malloc().
//
// 4) The Windows API CaptureStackTrace.
//
// Note: if you add a new implementation here, make sure it works
// correctly when GetStackTrace() is called with max_depth == 0.
// Some code may do that.

#if defined(HAVE_LIB_UNWIND)
# define STACKTRACE_H "stacktrace_libunwind-inl.h"
#elif defined(HAVE__UNWIND_BACKTRACE)
# define STACKTRACE_H "stacktrace_unwind-inl.h"
#elif !defined(NO_FRAME_POINTER)
# if defined(__i386__) && __GNUC__ >= 2
#  define STACKTRACE_H "stacktrace_x86-inl.h"
# elif (defined(__ppc__) || defined(__PPC__)) && __GNUC__ >= 2
#  define STACKTRACE_H "stacktrace_powerpc-inl.h"
# elif defined(GOOGLELOG_OS_WINDOWS)
#  define STACKTRACE_H "stacktrace_windows-inl.h"
# endif
#endif

#if !defined(STACKTRACE_H) && defined(HAVE_EXECINFO_H)
# define STACKTRACE_H "stacktrace_generic-inl.h"
#endif

#if defined(STACKTRACE_H)
# define HAVE_STACKTRACE
#endif

#ifndef GOOGLELOG_NO_SYMBOLIZE_DETECTION
#ifndef HAVE_SYMBOLIZE
// defined by gcc
#if defined(__ELF__) && defined(GOOGLELOG_OS_LINUX)
# define HAVE_SYMBOLIZE
#elif defined(GOOGLELOG_OS_MACOSX) && defined(HAVE_DLADDR)
// Use dladdr to symbolize.
# define HAVE_SYMBOLIZE
#elif defined(GOOGLELOG_OS_WINDOWS)
// Use DbgHelp to symbolize
# define HAVE_SYMBOLIZE
#endif
#endif // !defined(HAVE_SYMBOLIZE)
#endif // !defined(GOOGLELOG_NO_SYMBOLIZE_DETECTION)

#ifndef ARRAYSIZE
// There is a better way, but this is good enough for our purpose.
# define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))
#endif

_START_GOOGLE_NAMESPACE_

namespace googlelog_internal_namespace_ {

#ifdef HAVE___ATTRIBUTE__
# define ATTRIBUTE_NOINLINE __attribute__ ((noinline))
# define HAVE_ATTRIBUTE_NOINLINE
#elif defined(GOOGLELOG_OS_WINDOWS)
# define ATTRIBUTE_NOINLINE __declspec(noinline)
# define HAVE_ATTRIBUTE_NOINLINE
#else
# define ATTRIBUTE_NOINLINE
#endif

const char* ProgramInvocationShortName();

int64 CycleClock_Now();

int64 UsecToCycles(int64 usec);
WallTime WallTime_Now();

int32 GetMainThreadPid();
bool PidHasChanged();

pid_t GetTID();

const std::string& MyUserName();

// Get the part of filepath after the last path separator.
// (Doesn't modify filepath, contrary to basename() in libgen.h.)
const char* const_basename(const char* filepath);

// Wrapper of __sync_val_compare_and_swap. If the GCC extension isn't
// defined, we try the CPU specific logics (we only support x86 and
// x86_64 for now) first, then use a naive implementation, which has a
// race condition.
template<typename T>
inline T sync_val_compare_and_swap(T* ptr, T oldval, T newval) {
#if defined(HAVE___SYNC_VAL_COMPARE_AND_SWAP)
  return __sync_val_compare_and_swap(ptr, oldval, newval);
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
  T ret;
  __asm__ __volatile__("lock; cmpxchg %1, (%2);"
                       :"=a"(ret)
                        // GCC may produces %sil or %dil for
                        // constraint "r", but some of apple's gas
                        // dosn't know the 8 bit registers.
                        // We use "q" to avoid these registers.
                       :"q"(newval), "q"(ptr), "a"(oldval)
                       :"memory", "cc");
  return ret;
#else
  T ret = *ptr;
  if (ret == oldval) {
    *ptr = newval;
  }
  return ret;
#endif
}

void DumpStackTraceToString(std::string* stacktrace);

struct CrashReason {
  CrashReason() : filename(0), line_number(0), message(0), depth(0) {}

  const char* filename;
  int line_number;
  const char* message;

  // We'll also store a bit of stack trace context at the time of crash as
  // it may not be available later on.
  void* stack[32];
  int depth;
};

void SetCrashReason(const CrashReason* r);

void InitGoogleLoggingUtilities(const char* argv0);
void ShutdownGoogleLoggingUtilities();

}  // namespace googlelog_internal_namespace_

_END_GOOGLE_NAMESPACE_

using namespace GOOGLE_NAMESPACE::googlelog_internal_namespace_;

#endif  // UTILITIES_H__