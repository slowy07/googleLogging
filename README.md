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

## cmake

google log also support cmake that can be used to build the project on a wide range platforms. if you don't have cmake installed already, you can donwload it from cmake's [official website](http://www.cmake.org)

cmake works by generating native makefiles or build projects that can be used in the compiler environment of your choice. you can either build google logging with cmake as a standalone project or it can be incorporated int an existing cmake build for another project.

### build google logging with CMake

when building google logging as standalone project, on unix-like system with GNU make as build tool, the typical workflow is:

1. get the source code and change it.
   
   ```
    git clone https://github.com/slowy07/googleLogging
    cd googleLogging
    ```
2. run cmake to configure the build tree
    
    ```
    cmake -S . -B build -G "Unix Makefiles"
    ```
    cmake provides differents generators, and by default will pick the most relevant one to your environtment. if you need a specific version of visual studio, use ``cmake . -G <generator-name>``, and see ``cmake --help`` for the available generators. also see ``-T <toolset-name>``, wich can be used to request the native x64 toolchain with ``-T host=x64``.
3. afterwards, generated files can be use to compile the project
   
   ```
    cmake --build build
    ```
4. test the build software (optional)

    ```
    cmake --build build --target test
    ```
5. install the build files (optional)

    ```
    cmake --build build --target install
    ```


## setting flags

severals flags influence google log output behavior. if the [google gflags library](https://gflags.github.io/gflags/) is installed in your machine, the build system will automatically detect and use it, alowing you to pass flgas on the command line. for example, if you want to turn the flag ``--loghoster`` on, you can start application with the following command line:

```
./your-application --loglostderr=1
```

if the google gflags library isn't installed, you set flags vie environment variables, prefixing the flag name with ``GOOGLELOG_``, eg:

```
GOOGLELOG_loglostderr=1 ./your-application
```

