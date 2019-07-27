How to run cmocka for UEFI code.
=========================
1) install cmocka
  Cmocka repository was added as one submodule of HBFA project.
  The user can use the following commands to clone both main HBFA repo and Cmocka submodule:
  Add the "--recursive" flag to the git clone command:
  $ git clone --recursive <HBFA_REPO_URL>
or
  Manually initialize and update the submodules after the clone operation on main project:
  $ git clone <HBFA_REPO_URL>
  $ git submodule update --init --recursive

  And use the following combined commands to pull the remote submodule updates
(e.g. Updating the new supported Cmocka release tag):
  $ git pull --recurse-submodules && git submodule update --recursive

2) run sample code (Basic Mode)
Part A: Build in Linux
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA
    <...>/Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

Part B: Build in Windows
    build -p UefiHostUnitTestCasePkg\UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA
    <...>\Build\UefiHostUnitTestCasePkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe  

You may see below. Have fun
==============================
Int Safe Lib Unit Test Application v0.1
[==========] Running 71 test(s).
[ RUN      ] Test SafeInt8ToUint8
[       OK ] Test SafeInt8ToUint8
[ RUN      ] Test SafeInt8ToUint16
[       OK ] Test SafeInt8ToUint16
[ RUN      ] Test SafeInt8ToUint32
[       OK ] Test SafeInt8ToUint32
[ RUN      ] Test SafeInt8ToUintn
[       OK ] Test SafeInt8ToUintn
[ RUN      ] Test SafeInt8ToUint64
[       OK ] Test SafeInt8ToUint64
...
[==========] 71 test(s) run.
[  PASSED  ] 67 test(s).
[  FAILED  ] 4 test(s), listed below:
[  FAILED  ] Test SafeInt16ToChar8
[  FAILED  ] Test SafeInt32ToChar8
[  FAILED  ] Test SafeIntnToChar8
[  FAILED  ] Test SafeInt64ToChar8
[==========] Running 10 test(s).
...
[ RUN      ] Test SafeInt64Mult
[       OK ] Test SafeInt64Mult
[==========] 10 test(s) run.
[  PASSED  ] 10 test(s).
==============================

3) run sample code (XML report)
  set environment variable: CMOCKA_XML_FILE=<test.xml>

  Build in Linux
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t GCC5 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA -DUNIT_TEST_XML_MODE
    export CMOCKA_XML_FILE=TestBaseSafeIntLib.X64.xml
    ./Build/UefiHostUnitTestCasePkg/DEBUG_GCC5/X64/TestBaseSafeIntLib

  Build in Windows
    build -p UefiHostUnitTestCasePkg/UefiHostUnitTestCasePkg.dsc -a X64 -t VS2015x86 -DUNIT_TEST_FRAMEWORK_MODE=CMOCKA -DUNIT_TEST_XML_MODE
    set CMOCKA_XML_FILE=TestBaseSafeIntLib.X64.xml
    <...>\Build\UefiHostUnitTestCasePkg\DEBUG_VS2015x86\X64\TestBaseSafeIntLib.exe

  report is <test.xml>

4) get code coverage
See HBFA/UefiHostUnitTestPkg/ReadMe-Coverage.txt


