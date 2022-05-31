// Copyright (c) 1999, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define _GNU_SOURCE 1 // needed for O_NOFOLLOW and pread()/pwrite()

#include "utilities.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <string>
#ifdef HAVE_UNISTD_H
# include <unistd.h>  // For _exit.
#endif
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>  // For uname.
#endif
#include <ctime>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <cstdarg>
#include <cstdlib>
#ifdef HAVE_PWD_H
# include <pwd.h>
#endif
#ifdef HAVE_SYSLOG_H
# include <syslog.h>
#endif
#include <vector>
#include <cerrno>                   // for errno
#include <sstream>
#ifdef GOOGLELOG_OS_WINDOWS
#include "windows/dirent.h"
#else
#include <dirent.h> // for automatic removal of old logs
#endif
#include "base/commandlineflags.h"        // to get the program name
#include <googlelog/logging.h>
#include <googlelog/raw_logging.h>
#include "base/googleinit.h"

#ifdef HAVE_STACKTRACE
# include "stacktrace.h"
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

using std::string;
using std::vector;
using std::setw;
using std::setfill;
using std::hex;
using std::dec;
using std::min;
using std::ostream;
using std::ostringstream;

using std::FILE;
using std::fwrite;
using std::fclose;
using std::fflush;
using std::fprintf;
using std::perror;

#ifdef __QNX__
using std::fdopen;
#endif

#ifdef _WIN32
#define fdopen _fdopen
#endif

// There is no thread annotation support.
#define EXCLUSIVE_LOCKS_REQUIRED(mu)

static bool BoolFromEnv(const char *varname, bool defval) {
  const char* const valstr = getenv(varname);
  if (!valstr) {
    return defval;
  }
  return memchr("tTyY1\0", valstr[0], 6) != NULL;
}

GOOGLELOG_DEFINE_bool(timestamp_in_logfile_name,
                 BoolFromEnv("GOOGLE_TIMESTAMP_IN_LOGFILE_NAME", true),
                 "put a timestamp at the end of the log file name");
GOOGLELOG_DEFINE_bool(logtostderr, BoolFromEnv("GOOGLE_LOGTOSTDERR", false),
                 "log messages go to stderr instead of logfiles");
GOOGLELOG_DEFINE_bool(alsologtostderr, BoolFromEnv("GOOGLE_ALSOLOGTOSTDERR", false),
                 "log messages go to stderr in addition to logfiles");
GOOGLELOG_DEFINE_bool(colorlogtostderr, false,
                 "color messages logged to stderr (if supported by terminal)");
GOOGLELOG_DEFINE_bool(colorlogtostdout, false,
                 "color messages logged to stdout (if supported by terminal)");
GOOGLELOG_DEFINE_bool(logtostdout, BoolFromEnv("GOOGLE_LOGTOSTDOUT", false),
                 "log messages go to stdout instead of logfiles");
#ifdef GOOGLELOG_OS_LINUX
GOOGLELOG_DEFINE_bool(drop_log_memory, true, "Drop in-memory buffers of log contents. "
                 "Logs can grow very quickly and they are rarely read before they "
                 "need to be evicted from memory. Instead, drop them from memory "
                 "as soon as they are flushed to disk.");