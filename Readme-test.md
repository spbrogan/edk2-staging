## Introduction

This branch(edk2-host-test) is used for edk2 unit tests that can be running in OS environment. 
Owner is: Jiewen Yao <jiewen.yao@intel.com>


edk2-host-test are separated from [https://github.com/tianocore/edk2-staging/tree/HBFA](https://github.com/tianocore/edk2-staging/tree/HBFA). It contains a framework for running host based unit test in OS environment and all unit test cases. 

Here are some advantages for using it:

* Lightweight unit testing

  It can run directly in host os instead of running sct in uefi emulator. So it's lightweight and simple to use.

* Easy to debug (for unit tests)

  In past it was hard to do source code debug for UEFI unit testing because in UEFI environment gdb need to be configure appropriately. But now we run unit tests as OS application, so it is very easy to do source code debug same as normal applications.

* Easy for continuous integration.

  Running UEFI unit test as OS application is more easy for integration than UEFI SCT test.

## Suppored Features

* Running test cases in OS environment.
* Supported windows and linux environment.
* Use [cmocka](https://cmocka.org/) as unit testing framework.
* Generate code coverage report

## How to use edk2-host-test

Quick start:

0. ref: [https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
1. ```mkdir your-workspace``` or ```cd your-workspace``` 
2. ```git clone -b edk2-host-test https://github.com/xiaoyuxlu/edk2-staging.git edk2-host-test```
3. ```cd edk2-host-test```
4. run ```edksetup.bat``` or ```source edksetup.sh```
5. ```build -p MdePkg/MdePkgTest.dsc -a X64 -t VS2017 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA``` or ```build -p MdePkg/MdePkgTest.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA```
6. run [DestDir]\TestBaseSafeIntLib.exe

REF: [UefiHostUnitTestPkg/ReadMe-cmocka.txt](UefiHostUnitTestPkg/ReadMe-cmocka.txt)

## Timeline

| Time | Event | Related Module|
|---|---| --- |
| 2019 July | Initial Version | UefiHostTestPkg UefiHostUnitTestPkg CmockaHostUnitTestPkg and Port BaseSafeIntLib unit test |

## Related Materials

* [HBFA](https://github.com/tianocore/edk2-staging/tree/HBFA)