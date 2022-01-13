# googlelogging

google logging is a c++ 98 lib that implements application-level logging. the library provides logging APIs base on c++ style streams and various helper macros.

## getting started

you can log messaage by simply straming this to log
```cpp
#include <googlelog/logging.h>

int main(int argc, char* argv[]){
    google::InitGoogleLogging(argv[0]);
    
    //..
    LOG(INFO) << "Found "<< num_cookies << " cookies";
}
```
