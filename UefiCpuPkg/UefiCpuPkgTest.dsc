## @file UefiHostUnitTestCasePkg.dsc
# 
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiCpuPkg
  PLATFORM_GUID                  = 27BD7BAE-606F-4BDE-9216-CD1826F44CA9
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiCpuPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  DEFINE UNIT_TEST_FRAMEWORK_MODE = HOST
  DEFINE UNIT_TEST_XML_MODE = FALSE

[PcdsFixedAtBuild]
  !if $(UNIT_TEST_XML_MODE)
    gUefiHostUnitTestPkgTokenSpaceGuid.HostUnitTestMode|0x1
  !endif

[LibraryClasses]
  CmockaLib|CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
#  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
#  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf

  BaseLib|UefiHostTestPkg/Library/BaseLibHost/BaseLibHost.inf
  BaseMemoryLib|UefiHostTestPkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  CpuLib|UefiHostTestPkg/Library/BaseCpuLibHost/BaseCpuLibHost.inf
  DebugLib|UefiHostTestPkg/Library/DebugLibHost/DebugLibHost.inf
  DevicePathLib|UefiHostTestPkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  DxeServicesTableLib|UefiHostTestPkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  HobLib|UefiHostTestPkg/Library/HobLibHost/HobLibHost.inf
  MemoryAllocationLib|UefiHostTestPkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  MtrrStubLib|UefiHostTestPkg/Library/MtrrStubLib/MtrrStubLib.inf
  OsServiceLib|UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf
#  PeiServicesTablePointerLib|UefiHostTestPkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf
  SmmServicesTableLib|UefiHostTestPkg/Library/SmmServicesTableLibHost/SmmServicesTableLibHost.inf
  TimerLib|UefiHostTestPkg/Library/BaseTimerLibHost/BaseTimerLibHost.inf
  UefiBootServicesTableLib|UefiHostTestPkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  UefiDriverEntryPoint|UefiHostTestPkg/Library/UefiDriverEntryPointHost/UefiDriverEntryPointHost.inf
  UefiRuntimeServicesTableLib|UefiHostTestPkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  UnitTestAssertLib|UefiHostUnitTestPkg/Library/UnitTestAssertLibcmocka/UnitTestAssertLibcmocka.inf
  UnitTestLib|UefiHostUnitTestPkg/Library/UnitTestLibcmocka/UnitTestLibcmocka.inf

  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf

[Components]

  CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf {
  <BuildOptions>
    MSFT:*_*_*_CC_FLAGS      ==  /c /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1 -DHAVE_VSNPRINTF -DHAVE_SNPRINTF

    GCC:*_*_IA32_CC_FLAGS    == -m32 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
    GCC:*_*_X64_CC_FLAGS     == -m64 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
  }

  UefiCpuPkg/Library/MtrrLib/Tests/TestMtrrLib.inf {
  <LibraryClasses>
    NULL|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  }

[Components.IA32]
  # BUGBUG: Need change SmmInitPageTable() to return UINTN.
  # BUGBUG: Need enable allocate <4G memory in X64 build.
  UefiCpuPkg/PiSmmCpuDxeSmm/Tests/TestSmmCpuMemoryManagement.inf {
  <LibraryClasses>
    NULL|UefiCpuPkg/PiSmmCpuDxeSmm/Tests/Override/PiSmmCpuDxeSmm.inf
    NULL|UefiHostTestPkg/Library/BaseLibNullCpuid/BaseLibNullCpuid.inf
    NULL|UefiHostTestPkg/Library/BaseLibNullMsr/BaseLibNullMsr.inf
    MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
    IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
    PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
    PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
    LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
    UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
    SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
    CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
    SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
  }

!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc