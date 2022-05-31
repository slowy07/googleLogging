#ifndef GOOGLELOG_PLATFORM_H
#define GOOGLELOG_PLATFORM_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define GOOGLELOG_OS_WINDOWS
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define GOOGLELOG_OS_CYGWIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
#ifndef GOOGLELOG_OS_LINUX
#define GOOGLELOG_OS_LINUX
#endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define GOOGLELOG_OS_MACOSX
#elif defined(__FreeBSD__)
#define GOOGLELOG_OS_FREEBSD
#elif defined(__NetBSD__)
#define GOOGLELOG_OS_NETBSD
#elif defined(__OpenBSD__)
#define GOOGLELOG_OS_OPENBSD
#else
// TODO(hamaji): Add other platforms.
#error Platform not supported by googlelog. Please consider to contribute platform information by submitting a pull request on Github.
#endif

#endif