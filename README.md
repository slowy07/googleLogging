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

## building from source

google log supports multiple build systems for compiling the project from source

### cmake

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

### consuming google log in a cmake project

if you have googlelog installed in your system, you can use the CMake command ``find_package`` to build against google log in your CMake project as follows:

```
cmake_minimum_required (VERSION 3.16)
project (myproj VERSION 1.0)

find_package (glog 0.6.0 REQUIRED)

add_executable (myapp main.cpp)
target_link_libraries (myapp googlelog::googlelog)
```

compile definitions and options will be added automatically to your target asa needed

### incorporating googlelog into a CMake project

you can also use the CMake command ``add_subdirectory`` to include googlelog directly from a subdirectory of your project by replacing the ``find_package`` call from the previous exmple by ``add_subdirectory``. the ``googlelog:googlelog`` target is in this case an ``ALIAS`` library for the ``glog`` library target.


## setting flags

severals flags influence google log output behavior. if the [google gflags library](https://gflags.github.io/gflags/) is installed in your machine, the build system will automatically detect and use it, alowing you to pass flgas on the command line. for example, if you want to turn the flag ``--loghoster`` on, you can start application with the following command line:

```
./your-application --loglostderr=1
```

if the google gflags library isn't installed, you set flags vie environment variables, prefixing the flag name with ``GOOGLELOG_``, eg:

```
GOOGLELOG_loglostderr=1 ./your-application
```

## user guide

googleLog define a series of macros that simplify many common logging task.yoi can log messages by severity level, control logging behavior from the command line, log base on conditionals abort the program when expected conditions are not met, introduce your own verbose logging levels, customize the prefix attached to log message, and more.

following section describe the functionality supported by googleLogging. please note this description may not be complete but limited to the most useful ones. if you want to find less common features, please check header files under ``src/googlelog`` directory.

## severity levels

you can specify one of the following severity levels (in increasing order of severity): ``INFO``, ``WARNING``, ``ERROR``, and ``FATAL``. logging a ``FATAL`` message terminates the program (after the message is logged). Note that message of a given sverity are logged not only in the logfile for that severity, but also in all logfiles of lower sverity. E.g., a message of severity ``FATAL`` will be logged to the logfiles of severity ``FATAL``, ``ERROR``, ``WARNING``, and ``INFO``.

the ``DFATAL`` severity logs a ``FATAL`` error in debug mode (i.e., there is no ``NDEBUG`` macro defined), but avoids halting program in production by automatically reducing the severity to ``ERROR``.

Unless otherwise sspecified, googlelog writes to the filename ``/tmp/\<program name\>.\<hostname\>.\<user name\>.log.\<severity level\>.\<date\>-\<time\>.\<pid\>`` (e.g ``/tmp/hello_world.example.com.hamaji.log.INFO.20080709-222411.10474``). By default, googlelog copies the log messages of severity leve ``ERROR`` or ``FATAL`` to standard error (``stderr``) in addition to log files.

## installation notes for 64 bit linux systems

the glibc built-in stack-unwinder on 64 bit sytem has some problem with google log. ( in particular, if you areusing ``InstallFailureSignalHandler()``, the isgnal may be raised in the middle of malloc, holding some malloc-related locks when they invoke the stack unwinder. The built-in stck unwinder may cll malloc recusively, which my require the thread to acquire a lock it already holds: deadlock.)

for that reason. if you use 64 bit system and yoi need install ``InstallFailureSignalHandler()``, we strongly recommended you install ``libunwind`` berfore you trying to configure or install google log libunwind can be found [here](https://www.nongnu.org/libunwind/download.html).

even if you already intall ``libunwind`` installed, you probably still need to intall from the snapshot yo get the latest version.

caution: if you instll libunwind from url above, be aware that you my have trouble if you can tray staticallu link your binary with googlelog: thaht is,if you link with ``gcc -static -lgcc_eh ...``. This because both ``libunwind`` and ``libgcc`` implemen the same C++ exception handling API, but they implement them differently on some platforms. this is not likely to be a problem on ia64. but my be on x86-64.

also, if you link binaries statically, mke sure that you add ``-wl, --eh-frame-hdr`` to you linker options. this is required so that ``libundiwnd`` can find the information generated by the compiler required for stack unwinding.

using ``-static`` is rare, though, so unless you know this will affect you probably won't.

if you cannot or do not wish to install libunwind. you can still try to use two kinds of stack-unwinder: 1. glibc built-in-stack-unwinder and 2. frame pointer based stack-unwinder

1. as already mentioned, glibc's unwinder has a deadlock issue. however, if you don't use. ``InstallFailureSignalHandler()`` ot you don't worry about thath rare possibilities of deadlocks, you can use this stack-unwinder. if you specify no options and ``libuwind`` isn't detected on your sytem, the configure script chooses this unwinder by default.

2. the frame pointer based stack unwinder requires that your application, the googlelog library, and ystem libraries like libc, all be compiled with a frame pointer. thi is not the default for _x86-64_
