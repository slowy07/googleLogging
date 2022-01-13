// Tests the ScopedMockLog class.

#include "mock-log.h"

#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using GOOGLE_NAMESPACE::GOOGLELOG_INFO;
using GOOGLE_NAMESPACE::GOOGLELOG_WARNING;
using GOOGLE_NAMESPACE::GOOGLELOG_ERROR;
using GOOGLE_NAMESPACE::googlelog_testing::ScopedMockLog;
using std::string;
using testing::_;
using testing::HasSubstr;
using testing::InSequence;
using testing::InvokeWithoutArgs;

// Tests that ScopedMockLog intercepts LOG()s when it's alive.
TEST(ScopedMockLogTest, InterceptsLog) {
  ScopedMockLog log;

  InSequence s;
  EXPECT_CALL(log, Log(GOOGLELOG_WARNING, HasSubstr("/mock-log_unittest.cc"), "Fishy."));
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, _, "Working..."))
      .Times(2);
  EXPECT_CALL(log, Log(GOOGLELOG_ERROR, _, "Bad!!"));

  LOG(WARNING) << "Fishy.";
  LOG(INFO) << "Working...";
  LOG(INFO) << "Working...";
  LOG(ERROR) << "Bad!!";
}

void LogBranch() {
  LOG(INFO) << "Logging a branch...";
}

void LogTree() {
  LOG(INFO) << "Logging the whole tree...";
}

void LogForest() {
  LOG(INFO) << "Logging the entire forest.";
  LOG(INFO) << "Logging the entire forest..";
  LOG(INFO) << "Logging the entire forest...";
}

// The purpose of the following test is to verify that intercepting logging
// continues to work properly if a LOG statement is executed within the scope
// of a mocked call.
TEST(ScopedMockLogTest, LogDuringIntercept) {
  ScopedMockLog log;
  InSequence s;
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, __FILE__, "Logging a branch..."))
      .WillOnce(InvokeWithoutArgs(LogTree));
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, __FILE__, "Logging the whole tree..."))
      .WillOnce(InvokeWithoutArgs(LogForest));
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, __FILE__, "Logging the entire forest."));
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, __FILE__, "Logging the entire forest.."));
  EXPECT_CALL(log, Log(GOOGLELOG_INFO, __FILE__, "Logging the entire forest..."));
  LogBranch();
}

}  // namespace

int main(int argc, char **argv) {
  GOOGLE_NAMESPACE::InitGoogleLogging(argv[0]);
  testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}