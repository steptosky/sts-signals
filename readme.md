# Cross-platform headers only C++ library for working with signals-slots pattern.


| CI        | master | develop | last commit |
| --------- |:------:|:-------:|:-----------:|
| appveyor  |[![Build status](https://ci.appveyor.com/api/projects/status/ttnd36rho34b3qqt/branch/master?svg=true)](https://ci.appveyor.com/project/steptosky/sts-signals/branch/master)|[![Build status](https://ci.appveyor.com/api/projects/status/ttnd36rho34b3qqt/branch/develop?svg=true)](https://ci.appveyor.com/project/steptosky/sts-signals/branch/develop)|[![Build status](https://ci.appveyor.com/api/projects/status/ttnd36rho34b3qqt?svg=true)](https://ci.appveyor.com/project/steptosky/sts-signals)|
| travis    |[![Build Status](https://travis-ci.org/steptosky/sts-signals.svg?branch=master)](https://travis-ci.org/steptosky/sts-signals)|[![Build Status](https://travis-ci.org/steptosky/sts-signals.svg?branch=develop)](https://travis-ci.org/steptosky/sts-signals)|[![Build Status](https://travis-ci.org/steptosky/sts-signals.svg)](https://travis-ci.org/steptosky/sts-signals)|

---


#### common
- The library is distributed under 
[BSD (3-Clause)](http://opensource.org/licenses/BSD-3-Clause) 
license for more information read the [license](license.txt) file.
- The library uses [SemVer](http://semver.org/).
- The library requires C++ 11 or higher.
- The library's versions from the ```master``` branch are available in our 
  [[bintray]](https://bintray.com/steptosky/conan-open-source/sts-signals:steptosky) conan remote.  
  ```sts-signals/X.Y.Z@steptosky/stable```   

---


#### notes
- In this library some _dirty hacks_ are used to make slots calling as fast as possible.   
  Author of those hacks doesn't exactly know when those hacks can lead to problems, in theory never but it isn't guaranteed. 
  For some controversial situations the tests are written.  
  The definition ```STS_SIGNALS_SAFE_DELEGATE``` turns off those hacks. 
  You can find more information about this definition in the ```sts::signals::Signal``` class description in source file.
- This implementation isn't thread safe.
- This implementation doesn't work with return value. Author thinks it isn't necessary,
  you can use reference parameter in signal to make needed behavior.
- This implementation has auto-disconnect implementation for objects with slots, see ```sts::signals::AutoDisconnect``` class.
- This implementation doesn't support _std::function_ and lambdas.


#### warning 
- As the library hasn't got the major version 
  the library's interface and logic are not stable and can be changed at any time.

---

## How to use example
```  C++
class Sender { 
public:
    // declaration of a signal.
    sts::signals::Signal<bool, int> mSignal;
};

// If you want the slots to be auto-disconnected when your Receiver object is being deleted
// you just have to inherit your Receiver from sts::signals::AutoDisconnect and nothing more.
// Auto-disconnect doesn't work for static functions.
class Receiver : public sts::signals::AutoDisconnect {
public:
    static void slotStaticFunction(bool, int){}
    void slotMethod(bool, int){}
};

Sender senderObj;
Receiver receiverObj;

// Connecting slots to the signal.
senderObj.mSignal.connect(&Receiver::slotStaticFunction);       // static function
senderObj.mSignal.connect(&receiverObj, &Receiver::slotMethod); // class method

// invoke signal.
senderObj.mSignal(true, 5);

// Disconnecting slots from the signal
sender.mSignal.disconnect(&Receiver::slotStaticFunctiond);      // static function
sender.mSignal.disconnect(&receiverObj, &Receiver::slotMethod); // class method
// or
sender.mSignal.disconnect(&receiverObj);                        // all class methods of given pointer
```

---

## Build

#### dependencies
- [Cmake 3.7+](https://cmake.org) - build tool.
- [Conan 1.5+](https://www.conan.io) - dependency tool.
- [Python 2 or 3](https://www.python.org) - is needed for the Conan.
- [Doxygen](http://www.stack.nl/~dimitri/doxygen) - if you want to generate the documentation.
- [Gtest and Gmock 1.8](https://github.com/google/googletest) - testing (used through the Conan).


#### documentation
Run from the root folder ```doxygen doxyfile``` the result will be in the ```doc-generated``` folder.  
The ```doxygen``` has to be accessible through your ```PATH``` environment variable.


#### conan evironment variables
- **CONAN_TESTING_REPORT_DIR**=(string path) - A path for storing tests results. Default value is specified in the cmake script.
- **CONAN_BUILD_TESTING**=(0 or 1) - Enables/disables building and running the tests.  
  If you set ```BUILD_TESTING=ON``` as a parameter while running ```cmake``` command it will auto-set ```CONAN_BUILD_TESTING=1```.


#### cmake variables
- **BUILD_TESTING**=(ON/OFF) - Enables/disables building test projects. It is standard cmake variable.
- **TESTING_REPORT_DIR**=(string path) - You can specify the directory for the tests reports, it can be useful for CI.

Sometimes you will need to delete the file ```cmake/conan.cmake``` then the newer version of this file will be downloaded from the Internet while running ```cmake``` command.  
This file is responsible for cmake and conan interaction.


### Build scripts examples
These are just examples, 
probably you will need to adjust them for your purposes.


##### Windows (.bat) For Visual Studio 2017
``` batch
::==========================================================
@echo off
:: Fix problems with some symbols
REM change CHCP to UTF-8
CHCP 1252
CLS
::==========================================================
set dir="msvc"
if not exist %dir% mkdir %dir%
cd %dir%
::==========================================================
::conan user userName -r remote -p password
::==========================================================
call cmake -G "Visual Studio 15 Win64" ../ ^
    -DCMAKE_INSTALL_PREFIX=../output ^
    -DBUILD_TESTING=ON
:: Keep it commented if you want to generate VS project only
:: (without building), otherwise uncomment it.
::call cmake --build . --target install --config Release
::call cmake --build . --target install --config Debug
::==========================================================
cd ../
pause
::==========================================================
```


##### Linux (.sh)
``` bash
#===========================================================
dir_name="build-release"
#===========================================================
rm -r ${dir_name}
mkdir ${dir_name}
cd ${dir_name}
#===========================================================
#conan user userName -r remote -p password
#===========================================================
cmake -G"Unix Makefiles" ../ \
    -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX=../output \
    -DBUILD_TESTING=ON
cmake --build . --target install
cd ../
#===========================================================
```


##### Mac OS (.sh)
``` bash
#===========================================================
dir_name="build-release"
#===========================================================
rm -r ${dir_name}
mkdir ${dir_name}
cd ${dir_name}
#===========================================================
#conan user userName -r remote -p password
#===========================================================
cmake -G"Unix Makefiles" ../ \
    -DCMAKE_BUILD_TYPE="Release" \
    -DCMAKE_INSTALL_PREFIX=../output \
    -DBUILD_TESTING=ON
cmake --build . --target install
cd ../
#===========================================================
```


#### Build with the conan 'create' and 'conan package tools'.
These are just examples for Windows, 
probably you will need to adjust them for your purposes.


##### Windows (.bat) For Visual Studio 2017 x64

``` batch
::==========================================================
::conan user userName -r remote -p password
::==========================================================
call conan create . steptosky/develop ^
     -s compiler="Visual Studio" ^
     -s compiler.version=15 ^
     -s compiler.runtime=MD ^
     -s build_type=Release ^
     -o sts-signals:shared=True ^
     -e CONAN_TESTING_REPORT_DIR="report/conan-test" ^
     -e CONAN_BUILD_TESTING=1 ^
    --build=sts-signals ^
    --build=outdated
    
call conan create . steptosky/develop ^
     -s compiler="Visual Studio" ^
     -s compiler.version=15 ^
     -s compiler.runtime=MDd ^
     -s build_type=Debug ^
     -o sts-signals:shared=True ^
     -e CONAN_TESTING_REPORT_DIR="report/conan-test" ^
     -e CONAN_BUILD_TESTING=1 ^
    --build=sts-signals ^
    --build=outdated

pause
:: remove build dir
rd /s/q "test_package/build"
```


##### Windows (.bat) building with the conan package tools
``` batch
::==========================================================
::conan user userName -r remote -p password
::==========================================================
:: https://github.com/conan-io/conan-package-tools

set CONAN_USERNAME=steptosky
set CONAN_CHANNEL=testing
set CONAN_BUILD_TESTING=1
set CONAN_TESTING_REPORT_DIR=report
set CONAN_BUILD_POLICY=missing
set CONAN_STABLE_BRANCH_PATTERN=master
set CONAN_UPLOAD_ONLY_WHEN_STABLE=1
set CONAN_VISUAL_VERSIONS=15
set CTEST_OUTPUT_ON_FAILURE=1

call python build.py

:: uncomment it if you want to
:: remove all packages from all versions 
:: from testing channel
:: conan remove sts-signals/*@steptosky/testing -f

pause

:: remove build dir
rd /s/q "test_package/build"
```
After building the package will be available in the conan local cache,
so you can use it in your other projects on local PC.   

---


## For the library developers
- [release-checklist](release-checklist.md) see this file when you are making the release.
- [changelog](changelog.md) this file has to be filled during the release process and contains information about changes.
- [conan-package-tools](https://github.com/conan-io/conan-package-tools).
- [lasote docker hub](https://hub.docker.com/u/lasote/).

---


## Copyright
Copyright (c) 2018, StepToSky team. All rights reserved.  
[www.steptosky.com](http://www.steptosky.com/)