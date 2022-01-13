#ifndef BASE_COMMANDLINEFLAGS_H__
#define BASE_COMMANDLINEFLAGS_H__

#include "config.h"
#include <cstdlib>               // for getenv
#include <cstring>               // for memchr
#include <string>

#ifdef HAVE_LIB_GFLAGS

#include <gflags/gflags.h>

#else

#include <googlelog/logging.h>

#define DECLARE_VARIABLE(type, shorttype, name, tn)                     \
  namespace fL##shorttype {                                             \
    extern GOOGLE_GOOGLELOG_DLL_DECL type FLAGS_##name;                      \
  }                                                                     \
  using fL##shorttype::FLAGS_##name
#define DEFINE_VARIABLE(type, shorttype, name, value, meaning, tn)      \
  namespace fL##shorttype {                                             \
    GOOGLE_GOOGLELOG_DLL_DECL type FLAGS_##name(value);                      \
    char FLAGS_no##name;                                                \
  }                                                                     \
  using fL##shorttype::FLAGS_##name

// bool specialization
#define DECLARE_bool(name) \
  DECLARE_VARIABLE(bool, B, name, bool)
#define DEFINE_bool(name, value, meaning) \
  DEFINE_VARIABLE(bool, B, name, value, meaning, bool)

// int32 specialization
#define DECLARE_int32(name) \
  DECLARE_VARIABLE(GOOGLE_NAMESPACE::int32, I, name, int32)
#define DEFINE_int32(name, value, meaning) \
  DEFINE_VARIABLE(GOOGLE_NAMESPACE::int32, I, name, value, meaning, int32)

// uint32 specialization
#ifndef DECLARE_uint32
#define DECLARE_uint32(name) \
  DECLARE_VARIABLE(GOOGLE_NAMESPACE::uint32, U, name, uint32)
#endif // DECLARE_uint64
#define DEFINE_uint32(name, value, meaning) \
  DEFINE_VARIABLE(GOOGLE_NAMESPACE::uint32, U, name, value, meaning, uint32)

// Special case for string, because we have to specify the namespace
// std::string, which doesn't play nicely with our FLAG__namespace hackery.
#define DECLARE_string(name)                                            \
  namespace fLS {                                                       \
    extern GOOGLE_GOOGLELOG_DLL_DECL std::string& FLAGS_##name;              \
  }                                                                     \
  using fLS::FLAGS_##name
#define DEFINE_string(name, value, meaning)                             \
  namespace fLS {                                                       \
    std::string FLAGS_##name##_buf(value);                              \
    GOOGLE_GOOGLELOG_DLL_DECL std::string& FLAGS_##name = FLAGS_##name##_buf; \
    char FLAGS_no##name;                                                \
  }                                                                     \
  using fLS::FLAGS_##name

#endif  // HAVE_LIB_GFLAGS

// Define GOOGLELOG_DEFINE_* using DEFINE_* . By using these macros, we
// have GOOGLELOG_* environ variables even if we have gflags installed.
//
// If both an environment variable and a flag are specified, the value
// specified by a flag wins. E.g., if GOOGLELOG_v=0 and --v=1, the
// verbosity will be 1, not 0.

#define GOOGLELOG_DEFINE_bool(name, value, meaning) \
  DEFINE_bool(name, EnvToBool("GOOGLELOG_" #name, value), meaning)

#define GOOGLELOG_DEFINE_int32(name, value, meaning) \
  DEFINE_int32(name, EnvToInt("GOOGLELOG_" #name, value), meaning)

#define GOOGLELOG_DEFINE_uint32(name, value, meaning) \
  DEFINE_uint32(name, EnvToUInt("GOOGLELOG_" #name, value), meaning)

#define GOOGLELOG_DEFINE_string(name, value, meaning) \
  DEFINE_string(name, EnvToString("GOOGLELOG_" #name, value), meaning)

// These macros (could be functions, but I don't want to bother with a .cc
// file), make it easier to initialize flags from the environment.

#define EnvToString(envname, dflt)   \
  (!getenv(envname) ? (dflt) : getenv(envname))

#define EnvToBool(envname, dflt)   \
  (!getenv(envname) ? (dflt) : memchr("tTyY1\0", getenv(envname)[0], 6) != NULL)

#define EnvToInt(envname, dflt)  \
  (!getenv(envname) ? (dflt) : strtol(getenv(envname), NULL, 10))

#define EnvToUInt(envname, dflt)  \
  (!getenv(envname) ? (dflt) : strtoul(getenv(envname), NULL, 10))

#endif  // BASE_COMMANDLINEFLAGS_H__