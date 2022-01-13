#include "utilities.h"

#if defined(HAVE_PTHREAD)
# include <pthread.h>
#endif
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <googlelog/logging.h>

#ifdef HAVE_LIB_GFLAGS
#include <gflags/gflags.h>
using namespace GFLAGS_NAMESPACE;
#endif

using namespace GOOGLE_NAMESPACE;

static void* DieInThread(void*) {
  // We assume pthread_t is an integral number or a pointer, rather
  // than a complex struct.  In some environments, pthread_self()
  // returns an uint64 but in some other environments pthread_self()
  // returns a pointer.
  fprintf(
      stderr, "0x%px is dying\n",
      static_cast<const void*>(reinterpret_cast<const char*>(pthread_self())));
  // Use volatile to prevent from these to be optimized away.
  volatile int a = 0;
  volatile int b = 1 / a;
  fprintf(stderr, "We should have died: b=%d\n", b);
  return NULL;
}

static void WriteToStdout(const char* data, size_t size) {
  if (write(STDOUT_FILENO, data, size) < 0) {
    // Ignore errors.
  }
}

int main(int argc, char **argv) {
#if defined(HAVE_STACKTRACE) && defined(HAVE_SYMBOLIZE)
  InitGoogleLogging(argv[0]);
#ifdef HAVE_LIB_GFLAGS
  ParseCommandLineFlags(&argc, &argv, true);
#endif
  InstallFailureSignalHandler();
  const std::string command = argc > 1 ? argv[1] : "none";
  if (command == "segv") {
    // We'll check if this is outputted.
    LOG(INFO) << "create the log file";
    LOG(INFO) << "a message before segv";
    // We assume 0xDEAD is not writable.
    int *a = (int*)0xDEAD;
    *a = 0;
  } else if (command == "loop") {
    fprintf(stderr, "looping\n");
    while (true);
  } else if (command == "die_in_thread") {
#if defined(HAVE_PTHREAD)
    pthread_t thread;
    pthread_create(&thread, NULL, &DieInThread, NULL);
    pthread_join(thread, NULL);
#else
    fprintf(stderr, "no pthread\n");
    return 1;
#endif
  } else if (command == "dump_to_stdout") {
    InstallFailureWriter(WriteToStdout);
    abort();
  } else if (command == "installed") {
    fprintf(stderr, "signal handler installed: %s\n",
        IsFailureSignalHandlerInstalled() ? "true" : "false");
  } else {
    // Tell the shell script
    puts("OK");
  }
#endif
  return 0;
}