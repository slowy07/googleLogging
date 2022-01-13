#include <cstdio>
#include <string>
#include <iosfwd>
#include <glog/logging.h>
#include "base/commandlineflags.h"
#include "config.h"

DECLARE_bool(logtostderr);
GLOG_DEFINE_bool(check_mode, false, "Prints 'opt' or 'dbg'");

using std::string;
using namespace GOOGLE_NAMESPACE;

int CheckNoReturn(bool b) {
  string s;
  if (b) {
    LOG(FATAL) << "Fatal";
  } else {
    return 0;
  }
}

struct A { };
std::ostream &operator<<(std::ostream &str, const A&) {return str;}

int main(int, char* argv[]) {
  FLAGS_logtostderr = true;
  InitGoogleLogging(argv[0]);
  if (FLAGS_check_mode) {
    printf("%s\n", DEBUG_MODE ? "dbg" : "opt");
    return 0;
  }
  LOG(INFO) << "TESTMESSAGE INFO";
  LOG(WARNING) << 2 << "something" << "TESTMESSAGE WARNING"
               << 1 << 'c' << A() << std::endl;
  LOG(ERROR) << "TESTMESSAGE ERROR";
  bool flag = true;
  (flag ? LOG(INFO) : LOG(ERROR)) << "TESTMESSAGE COND";
  LOG(FATAL) << "TESTMESSAGE FATAL";
}