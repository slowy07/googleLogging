#include "config.h"
#include "port.h"
#include "stacktrace.h"
#include <dbghelp.h>

_START_GOOGLE_NAMESPACE_

int GetStackTrace(void** result, int max_depth, int skip_count) {
  if (max_depth > 64) {
    max_depth = 64;
  }
  skip_count++;  // we want to skip the current frame as well
  // This API is thread-safe (moreover it walks only the current thread).
  return CaptureStackBackTrace(static_cast<DWORD>(skip_count), static_cast<DWORD>(max_depth), result, NULL);
}

_END_GOOGLE_NAMESPACE_