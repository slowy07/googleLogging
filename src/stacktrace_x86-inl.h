// Produce stack trace

#include <stdint.h>   // for uintptr_t

#include "utilities.h"   // for OS_* macros

#if !defined(GOOGLELOG_OS_WINDOWS)
#include <unistd.h>
#include <sys/mman.h>
#endif

#include <cstdio>  // for NULL
#include "stacktrace.h"

_START_GOOGLE_NAMESPACE_

// Given a pointer to a stack frame, locate and return the calling
// stackframe, or return NULL if no stackframe can be found. Perform sanity
// checks (the strictness of which is controlled by the boolean parameter
// "STRICT_UNWINDING") to reduce the chance that a bad pointer is returned.
template<bool STRICT_UNWINDING>
static void **NextStackFrame(void **old_sp) {
  void **new_sp = static_cast<void **>(*old_sp);

  // Check that the transition from frame pointer old_sp to frame
  // pointer new_sp isn't clearly bogus
  if (STRICT_UNWINDING) {
    // With the stack growing downwards, older stack frame must be
    // at a greater address that the current one.
    if (new_sp <= old_sp) return NULL;
    // Assume stack frames larger than 100,000 bytes are bogus.
    if ((uintptr_t)new_sp - (uintptr_t)old_sp > 100000) return NULL;
  } else {
    // In the non-strict mode, allow discontiguous stack frames.
    // (alternate-signal-stacks for example).
    if (new_sp == old_sp) return NULL;
    // And allow frames upto about 1MB.
    if ((new_sp > old_sp)
        && ((uintptr_t)new_sp - (uintptr_t)old_sp > 1000000)) return NULL;
  }
  if ((uintptr_t)new_sp & (sizeof(void *) - 1)) return NULL;
#ifdef __i386__
  // On 64-bit machines, the stack pointer can be very close to
  // 0xffffffff, so we explicitly check for a pointer into the
  // last two pages in the address space
  if ((uintptr_t)new_sp >= 0xffffe000) return NULL;
#endif
#if !defined(GOOGLELOG_OS_WINDOWS)
  if (!STRICT_UNWINDING) {
    // Lax sanity checks cause a crash in 32-bit tcmalloc/crash_reason_test
    // on AMD-based machines with VDSO-enabled kernels.
    // Make an extra sanity check to insure new_sp is readable.
    // Note: NextStackFrame<false>() is only called while the program
    //       is already on its last leg, so it's ok to be slow here.
    static int page_size = getpagesize();
    void *new_sp_aligned = (void *)((uintptr_t)new_sp & ~(page_size - 1));
    if (msync(new_sp_aligned, page_size, MS_ASYNC) == -1) {
      return NULL;
    }
  }
#endif
  return new_sp;
}

// If you change this function, also change GetStackFrames below.
int GetStackTrace(void** result, int max_depth, int skip_count) {
  void **sp;

#ifdef __GNUC__
#if __GNUC__ * 100 + __GNUC_MINOR__ >= 402
#define USE_BUILTIN_FRAME_ADDRESS
#endif
#endif

#ifdef USE_BUILTIN_FRAME_ADDRESS
  sp = reinterpret_cast<void**>(__builtin_frame_address(0));
#elif defined(__i386__)
  // Stack frame format:
  //    sp[0]   pointer to previous frame
  //    sp[1]   caller address
  //    sp[2]   first argument
  //    ...
  sp = (void **)&result - 2;
#elif defined(__x86_64__)
  // __builtin_frame_address(0) can return the wrong address on gcc-4.1.0-k8
  unsigned long rbp;
  // Move the value of the register %rbp into the local variable rbp.
  // We need 'volatile' to prevent this instruction from getting moved
  // around during optimization to before function prologue is done.
  // An alternative way to achieve this
  // would be (before this __asm__ instruction) to call Noop() defined as
  //   static void Noop() __attribute__ ((noinline));  // prevent inlining
  //   static void Noop() { asm(""); }  // prevent optimizing-away
  __asm__ volatile ("mov %%rbp, %0" : "=r" (rbp));
  // Arguments are passed in registers on x86-64, so we can't just
  // offset from &result
  sp = (void **) rbp;
#endif

  int n = 0;
  while (sp && n < max_depth) {
    if (*(sp+1) == (void *)0) {
      // In 64-bit code, we often see a frame that
      // points to itself and has a return address of 0.
      break;
    }
    if (skip_count > 0) {
      skip_count--;
    } else {
      result[n++] = *(sp+1);
    }
    // Use strict unwinding rules.
    sp = NextStackFrame<true>(sp);
  }
  return n;
}

_END_GOOGLE_NAMESPACE_