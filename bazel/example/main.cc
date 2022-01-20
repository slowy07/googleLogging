#include <gflags/gflags.h>
#include <googlelog/logging.h>
#include <googlelog/stl_logging.h>

int main(int argc, char* argv[]){
  // initialize googlelog library
  google::InitGoogleLogging(argv[0]);

  gflags::ParseCommandLineFlag(&argc, &argv, true);

  LOG(INFO) << "test hello";

  // googlelog/stl_logging llow logging STL containers
  
  std::vector<int> x;
  x.push_back(1);
  x.push_back(2);
  x.push_back(3);
  
  LOG(INFO) << "ABC it's easy as" << x;

  return 0;
}
