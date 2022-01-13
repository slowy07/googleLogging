#ifndef BASE_LOG_SEVERITY_H__
#define BASE_LOG_SEVERITY_H__

// The recommended semantics of the log levels are as follows:
//
// INFO:
//   Use for state changes or other major events, or to aid debugging.
// WARNING:
//   Use for undesired but relatively expected events, which may indicate a
//   problem
// ERROR:
//   Use for undesired and unexpected events that the program can recover from.
//   All ERRORs should be actionable - it should be appropriate to file a bug
//   whenever an ERROR occurs in production.
// FATAL:
//   Use for undesired and unexpected events that the program cannot recover
//   from. 

// Annoying stuff for windows -- makes sure clients can import these functions
#ifndef GOOGLE_GOOGLELOG_DLL_DECL
# if defined(_WIN32) && !defined(__CYGWIN__)
#   define GOOGLE_GOOGLELOG_DLL_DECL  __declspec(dllimport)
# else
#   define GOOGLE_GOOGLELOG_DLL_DECL
# endif
#endif

// Variables of type LogSeverity are widely taken to lie in the range
// [0, NUM_SEVERITIES-1].  Be careful to preserve this assumption if
// you ever need to change their values or add a new severity.
typedef int LogSeverity;

const int GOOGLELOG_INFO = 0, GOOGLELOG_WARNING = 1, GOOGLELOG_ERROR = 2, GOOGLELOG_FATAL = 3,
  NUM_SEVERITIES = 4;
#ifndef GOOGLELOG_NO_ABBREVIATED_SEVERITIES
# ifdef ERROR
#  error ERROR macro is defined. Define GOOGLELOG_NO_ABBREVIATED_SEVERITIES before including logging.h. See the document for detail.
# endif
const int INFO = GOOGLELOG_INFO, WARNING = GOOGLELOG_WARNING,
  ERROR = GOOGLELOG_ERROR, FATAL = GOOGLELOG_FATAL;
#endif

// DFATAL is FATAL in debug mode, ERROR in normal mode
#ifdef NDEBUG
#define DFATAL_LEVEL ERROR
#else
#define DFATAL_LEVEL FATAL
#endif

extern GOOGLE_GOOGLELOG_DLL_DECL const char* const LogSeverityNames[NUM_SEVERITIES];

// NDEBUG usage helpers related to (RAW_)DCHECK:
//
// DEBUG_MODE is for small !NDEBUG uses like
//   if (DEBUG_MODE) foo.CheckThatFoo();
// instead of substantially more verbose
//   #ifndef NDEBUG
//     foo.CheckThatFoo();
//   #endif
//
// IF_DEBUG_MODE is for small !NDEBUG uses like
//   IF_DEBUG_MODE( string error; )
//   DCHECK(Foo(&error)) << error;
// instead of substantially more verbose
//   #ifndef NDEBUG
//     string error;
//     DCHECK(Foo(&error)) << error;
//   #endif
//
#ifdef NDEBUG
enum { DEBUG_MODE = 0 };
#define IF_DEBUG_MODE(x)
#else
enum { DEBUG_MODE = 1 };
#define IF_DEBUG_MODE(x) x
#endif

#endif  // BASE_LOG_SEVERITY_H__