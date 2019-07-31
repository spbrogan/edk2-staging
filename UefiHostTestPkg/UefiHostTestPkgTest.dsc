## @file UefiHostUnitTestPkgTest.dsc
# 
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostTestPkg
  PLATFORM_GUID                  = AED3ADB9-2773-4796-83A7-41862BD8A1C1
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostTestPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = UefiHostTestPkg/UefiHostTestPkgTest.fdf

  DEFINE UNIT_TEST_FRAMEWORK_MODE = HOST
  DEFINE UNIT_TEST_XML_MODE = FALSE

[PcdsFixedAtBuild]
  !if $(UNIT_TEST_XML_MODE)
    gUefiHostUnitTestPkgTokenSpaceGuid.HostUnitTestMode|0x1
  !endif

[LibraryClasses]
  CmockaLib|CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf

  BaseLib|MdePkg/Library/BaseLibHost/BaseLibHost.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  DebugLib|MdePkg/Library/DebugLibHost/DebugLibHost.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  MemoryAllocationLib|MdePkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  OsServiceLib|UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLibHost/SmmServicesTableLibHost.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  UnitTestAssertLib|CmockaHostUnitTestPkg/Library/UnitTestAssertLibcmocka/UnitTestAssertLibcmocka.inf
  UnitTestLib|CmockaHostUnitTestPkg/Library/UnitTestLibcmocka/UnitTestLibcmocka.inf

[Components]

  CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf {
  <BuildOptions>
    MSFT:*_*_*_CC_FLAGS      ==  /c /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1 -DHAVE_VSNPRINTF -DHAVE_SNPRINTF

    GCC:*_*_IA32_CC_FLAGS    == -m32 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
    GCC:*_*_X64_CC_FLAGS     == -m64 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
  }

  MdePkg/Library/DxeServicesTableLibHost/Tests/TestDxeServicesTableLib.inf
  MdePkg/Library/SmmServicesTableLibHost/Tests/TestSmmServicesTableLib.inf
  MdePkg/Library/PeiServicesLib/Tests/TestPeiServicesLib.inf
  MdePkg/Library/UefiBootServicesTableLibHost/Tests/TestUefiBootServicesTableLib.inf
  MdePkg/Library/MemoryAllocationLibHost/Tests/TestMemoryAllocationLib.inf
  MdePkg/Library/UefiRuntimeServicesTableLibHost/Tests/TestUefiRuntimeServicesTableLib.inf

  MdePkg/Library/BasePcdLibHost/Tests/TestPcdLibStatic.inf {
  <LibraryClasses>
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  <PcdsPatchableInModule>
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidStrAsc|"StrAsc"|VOID*|12
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidStrUni|L"StrUni"|VOID*|28
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidBufAll0|{0x0, 0x0, 0x0}|VOID*|5
    gTestCasePkgTokenSpaceGuid.PcdTestPatchableInModuleVoidBufAll1|{0xFF, 0xFF, 0xFF}|VOID*|5
  }
  MdePkg/Library/BasePcdLibHost/Tests/TestPcdLibDynamic.inf {
  <LibraryClasses>
    PcdLib|MdePkg/Library/BasePcdLibHost/BasePcdLibHost.inf
  }


!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc