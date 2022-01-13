// Unit tests for functions in demangle.c.

#include "utilities.h"

#include <iostream>
#include <fstream>
#include <string>
#include <googlelog/logging.h>
#include "demangle.h"
#include "googletest.h"
#include "config.h"

#ifdef HAVE_LIB_GFLAGS
#include <gflags/gflags.h>
using namespace GFLAGS_NAMESPACE;
#endif

GOOGLELOG_DEFINE_bool(demangle_filter, false,
                 "Run demangle_unittest in filter mode");

using namespace std;
using namespace GOOGLE_NAMESPACE;

// A wrapper function for Demangle() to make the unit test simple.
static const char *DemangleIt(const char * const mangled) {
  static char demangled[4096];
  if (Demangle(mangled, demangled, sizeof(demangled))) {
    return demangled;
  } else {
    return mangled;
  }
}

#if defined(GOOGLELOG_OS_WINDOWS)

#if defined(HAVE_DBGHELP) && !defined(NDEBUG)
TEST(Demangle, Windows) {
  EXPECT_STREQ(
    "public: static void __cdecl Foo::func(int)",
    DemangleIt("?func@Foo@@SAXH@Z"));
  EXPECT_STREQ(
    "public: static void __cdecl Foo::func(int)",
    DemangleIt("@ILT+1105(?func@Foo@@SAXH@Z)"));
  EXPECT_STREQ(
    "int __cdecl foobarArray(int * const)",
    DemangleIt("?foobarArray@@YAHQAH@Z"));
}
#endif

#else

// Test corner cases of bounary conditions.
TEST(Demangle, CornerCases) {
  const size_t size = 10;
  char tmp[size] = { 0 };
  const char *demangled = "foobar()";
  const char *mangled = "_Z6foobarv";
  EXPECT_TRUE(Demangle(mangled, tmp, sizeof(tmp)));
  // sizeof("foobar()") == size - 1
  EXPECT_STREQ(demangled, tmp);
  EXPECT_TRUE(Demangle(mangled, tmp, size - 1));
  EXPECT_STREQ(demangled, tmp);
  EXPECT_FALSE(Demangle(mangled, tmp, size - 2));  // Not enough.
  EXPECT_FALSE(Demangle(mangled, tmp, 1));
  EXPECT_FALSE(Demangle(mangled, tmp, 0));
  EXPECT_FALSE(Demangle(mangled, NULL, 0));  // Should not cause SEGV.
}

// Test handling of functions suffixed with .clone.N, which is used by GCC
// 4.5.x, and .constprop.N and .isra.N, which are used by GCC 4.6.x.  These
// suffixes are used to indicate functions which have been cloned during
// optimization.  We ignore these suffixes.
TEST(Demangle, Clones) {
  char tmp[20];
  EXPECT_TRUE(Demangle("_ZL3Foov", tmp, sizeof(tmp)));
  EXPECT_STREQ("Foo()", tmp);
  EXPECT_TRUE(Demangle("_ZL3Foov.clone.3", tmp, sizeof(tmp)));
  EXPECT_STREQ("Foo()", tmp);
  EXPECT_TRUE(Demangle("_ZL3Foov.constprop.80", tmp, sizeof(tmp)));
  EXPECT_STREQ("Foo()", tmp);
  EXPECT_TRUE(Demangle("_ZL3Foov.isra.18", tmp, sizeof(tmp)));
  EXPECT_STREQ("Foo()", tmp);
  EXPECT_TRUE(Demangle("_ZL3Foov.isra.2.constprop.18", tmp, sizeof(tmp)));
  EXPECT_STREQ("Foo()", tmp);
  // Invalid (truncated), should not demangle.
  EXPECT_FALSE(Demangle("_ZL3Foov.clo", tmp, sizeof(tmp)));
  // Invalid (.clone. not followed by number), should not demangle.
  EXPECT_FALSE(Demangle("_ZL3Foov.clone.", tmp, sizeof(tmp)));
  // Invalid (.clone. followed by non-number), should not demangle.
  EXPECT_FALSE(Demangle("_ZL3Foov.clone.foo", tmp, sizeof(tmp)));
  // Invalid (.constprop. not followed by number), should not demangle.
  EXPECT_FALSE(Demangle("_ZL3Foov.isra.2.constprop.", tmp, sizeof(tmp)));
}

TEST(Demangle, FromFile) {
  string test_file = FLAGS_test_srcdir + "/src/demangle_unittest.txt";
  ifstream f(test_file.c_str());  // The file should exist.
  EXPECT_FALSE(f.fail());

  string line;
  while (getline(f, line)) {
    // Lines start with '#' are considered as comments.
    if (line.empty() || line[0] == '#') {
      continue;
    }
    // Each line should contain a mangled name and a demangled name
    // separated by '\t'.  Example: "_Z3foo\tfoo"
    string::size_type tab_pos = line.find('\t');
    EXPECT_NE(string::npos, tab_pos);
    string mangled = line.substr(0, tab_pos);
    string demangled = line.substr(tab_pos + 1);
    EXPECT_EQ(demangled, DemangleIt(mangled.c_str()));
  }
}

#endif

int main(int argc, char **argv) {
#ifdef HAVE_LIB_GFLAGS
  ParseCommandLineFlags(&argc, &argv, true);
#endif
  InitGoogleTest(&argc, argv);

  FLAGS_logtostderr = true;
  InitGoogleLogging(argv[0]);
  if (FLAGS_demangle_filter) {
    // Read from cin and write to cout.
    string line;
    while (getline(cin, line, '\n')) {
      cout << DemangleIt(line.c_str()) << endl;
    }
    return 0;
  } else if (argc > 1) {
    cout << DemangleIt(argv[1]) << endl;
    return 0;
  } else {
    return RUN_ALL_TESTS();
  }
}