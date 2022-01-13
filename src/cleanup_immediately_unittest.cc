#include <googlelog/logging.h>
#include <googlelog/raw_logging.h>

#include "base/commandlineflags.h"
#include "googletest.h"

#ifdef HAVE_LIB_GFLAGS
#include <gflags/gflags.h>
using namespace GFLAGS_NAMESPACE;
#endif

#ifdef HAVE_LIB_GMOCK
#include <gmock/gmock.h>

#include "mock-log.h"
// Introduce several symbols from gmock.
using GOOGLE_NAMESPACE::googlelog_testing::ScopedMockLog;
using testing::_;
using testing::AllOf;
using testing::AnyNumber;
using testing::HasSubstr;
using testing::InitGoogleMock;
using testing::StrictMock;
using testing::StrNe;
#endif

using namespace GOOGLE_NAMESPACE;

TEST(CleanImmediately, logging) {
  google::SetLogFilenameExtension(".foobar");
  google::EnableLogCleaner(0);

  for (unsigned i = 0; i < 1000; ++i) {
    LOG(INFO) << "cleanup test";
  }

  google::DisableLogCleaner();
}

int main(int argc, char **argv) {
  FLAGS_colorlogtostderr = false;
  FLAGS_timestamp_in_logfile_name = true;
#ifdef HAVE_LIB_GFLAGS
  ParseCommandLineFlags(&argc, &argv, true);
#endif
  // Make sure stderr is not buffered as stderr seems to be buffered
  // on recent windows.
  setbuf(stderr, NULL);

  // Test some basics before InitGoogleLogging:
  CaptureTestStderr();
  const string early_stderr = GetCapturedTestStderr();

  EXPECT_FALSE(IsGoogleLoggingInitialized());

  InitGoogleLogging(argv[0]);

  EXPECT_TRUE(IsGoogleLoggingInitialized());

  InitGoogleTest(&argc, argv);
#ifdef HAVE_LIB_GMOCK
  InitGoogleMock(&argc, argv);
#endif

  // so that death tests run before we use threads
  CHECK_EQ(RUN_ALL_TESTS(), 0);
}