#include  "config.h"
#ifdef HAVE_USING_OPERATOR

#include <functional>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#ifdef __GNUC__

# if defined(_LIBCPP_VERSION)
#  ifndef GOOGLELOG_STL_LOGGING_FOR_UNORDERED
#  define GOOGLELOG_STL_LOGGING_FOR_UNORDERED
#  endif
# else
#  ifndef GOOGLELOG_STL_LOGGING_FOR_EXT_HASH
#  define GOOGLELOG_STL_LOGGING_FOR_EXT_HASH
#  endif
#  ifndef GOOGLELOG_STL_LOGGING_FOR_EXT_SLIST
#  define GOOGLELOG_STL_LOGGING_FOR_EXT_SLIST
#  endif
#  ifndef GOOGLELOG_STL_LOGGING_FOR_TR1_UNORDERED
#  define GOOGLELOG_STL_LOGGING_FOR_TR1_UNORDERED
#  endif
# endif
#endif

#include <googlelog/logging.h>
#include <googlelog/stl_logging.h>
#include "googletest.h"

using namespace std;
#ifdef GOOGLELOG_STL_LOGGING_FOR_EXT_HASH
using namespace __gnu_cxx;
#endif

struct user_hash {
  size_t operator()(int x) const { return static_cast<size_t>(x); }
};

static void TestSTLLogging() {
  {
    // Test a sequence.
    vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    ostringstream ss;
    ss << v;
    EXPECT_EQ(ss.str(), "10 20 30");
    vector<int> copied_v(v);
    CHECK_EQ(v, copied_v);  // This must compile.
  }

  {
    // Test a sorted pair associative container.
    map< int, string > m;
    m[20] = "twenty";
    m[10] = "ten";
    m[30] = "thirty";
    ostringstream ss;
    ss << m;
    EXPECT_EQ(ss.str(), "(10, ten) (20, twenty) (30, thirty)");
    map< int, string > copied_m(m);
    CHECK_EQ(m, copied_m);  // This must compile.
  }

#ifdef GOOGELOG_STL_LOGGING_FOR_EXT_HASH
  {
    // Test a hashed simple associative container.
    hash_set<int> hs;
    hs.insert(10);
    hs.insert(20);
    hs.insert(30);
    ostringstream ss;
    ss << hs;
    EXPECT_EQ(ss.str().size(), 8);
    EXPECT_TRUE(ss.str().find("10") != string::npos);
    EXPECT_TRUE(ss.str().find("20") != string::npos);
    EXPECT_TRUE(ss.str().find("30") != string::npos);
    hash_set<int> copied_hs(hs);
    CHECK_EQ(hs, copied_hs);  // This must compile.
  }
#endif

#ifdef GOOGLELOG_STL_LOGGING_FOR_EXT_HASH
  {
    // Test a hashed pair associative container.
    hash_map<int, string> hm;
    hm[10] = "ten";
    hm[20] = "twenty";
    hm[30] = "thirty";
    ostringstream ss;
    ss << hm;
    EXPECT_EQ(ss.str().size(), 35);
    EXPECT_TRUE(ss.str().find("(10, ten)") != string::npos);
    EXPECT_TRUE(ss.str().find("(20, twenty)") != string::npos);
    EXPECT_TRUE(ss.str().find("(30, thirty)") != string::npos);
    hash_map<int, string> copied_hm(hm);
    CHECK_EQ(hm, copied_hm);  // this must compile
  }
#endif

  {
    // Test a long sequence.
    vector<int> v;
    string expected;
    for (int i = 0; i < 100; i++) {
      v.push_back(i);
      if (i > 0) expected += ' ';
      const size_t buf_size = 256;
      char buf[buf_size];
      snprintf(buf, buf_size, "%d", i);
      expected += buf;
    }
    v.push_back(100);
    expected += " ...";
    ostringstream ss;
    ss << v;
    CHECK_EQ(ss.str(), expected.c_str());
  }

  {
    // Test a sorted pair associative container.
    // Use a non-default comparison functor.
    map< int, string, greater<int> > m;
    m[20] = "twenty";
    m[10] = "ten";
    m[30] = "thirty";
    ostringstream ss;
    ss << m;
    EXPECT_EQ(ss.str(), "(30, thirty) (20, twenty) (10, ten)");
    map< int, string, greater<int> > copied_m(m);
    CHECK_EQ(m, copied_m);  // This must compile.
  }

#ifdef GOOGLELOG_STL_LOGGING_FOR_EXT_HASH
  {
    // Test a hashed simple associative container.
    // Use a user defined hash function.
    hash_set<int, user_hash> hs;
    hs.insert(10);
    hs.insert(20);
    hs.insert(30);
    ostringstream ss;
    ss << hs;
    EXPECT_EQ(ss.str().size(), 8);
    EXPECT_TRUE(ss.str().find("10") != string::npos);
    EXPECT_TRUE(ss.str().find("20") != string::npos);
    EXPECT_TRUE(ss.str().find("30") != string::npos);
    hash_set<int, user_hash> copied_hs(hs);
    CHECK_EQ(hs, copied_hs);  // This must compile.
  }
#endif
}

int main(int, char**) {
  TestSTLLogging();
  std::cout << "PASS\n";
  return 0;
}

#else

#include <iostream>

int main(int, char**) {
  std::cout << "We don't support stl_logging for this compiler.\n"
            << "(we need compiler support of 'using ::operator<<' "
            << "for this feature.)\n";
  return 0;
}

#endif  // HAVE_USING_OPERATOR
