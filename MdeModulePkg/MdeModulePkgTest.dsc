## @file MdeModulePkgTest.dsc
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = MdeModulePkg
  PLATFORM_GUID                  = 8AD872E8-AA24-4ADF-AC25-562DD777A624
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/MdeModulePkg
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
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf

  BaseLib|MdePkg/HostLibrary/BaseLibHost/BaseLibHost.inf
  BaseMemoryLib|MdePkg/HostLibrary/BaseMemoryLibHost/BaseMemoryLibHost.inf
  DebugLib|MdePkg/HostLibrary/DebugLibHost/DebugLibHost.inf
  DevicePathLib|MdePkg/HostLibrary/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  DxeServicesTableLib|MdePkg/HostLibrary/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  MemoryAllocationLib|MdePkg/HostLibrary/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  UefiBootServicesTableLib|MdePkg/HostLibrary/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  UefiDriverEntryPoint|MdePkg/HostLibrary/UefiDriverEntryPointHost/UefiDriverEntryPointHost.inf
  UefiRuntimeServicesTableLib|MdePkg/HostLibrary/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf

  OsServiceLib|UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf
  UnitTestAssertLib|CmockaHostUnitTestPkg/Library/UnitTestAssertLibcmocka/UnitTestAssertLibcmocka.inf
  UnitTestLib|CmockaHostUnitTestPkg/Library/UnitTestLibcmocka/UnitTestLibcmocka.inf

  PciHostBridgeStubLib|UefiHostTestPkg/Helpers/PciHostBridgeStubLib/PciHostBridgeStubLib.inf
  PciSegmentLib|UefiHostTestPkg/Helpers/PciSegmentStubLib/PciSegmentStubLib.inf
  PciSegmentStubLib|UefiHostTestPkg/Helpers/PciSegmentStubLib/PciSegmentStubLib.inf
[Components]

  CmockaHostUnitTestPkg/Library/CmockaLib/CmockaLib.inf {
  <BuildOptions>
    MSFT:*_*_*_CC_FLAGS      ==  /c /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1 -DHAVE_VSNPRINTF -DHAVE_SNPRINTF

    GCC:*_*_IA32_CC_FLAGS    == -m32 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
    GCC:*_*_X64_CC_FLAGS     == -m64 -O0 -g -fprofile-arcs -ftest-coverage -std=gnu99 -Wpedantic -Wall -Wshadow -Wmissing-prototypes -Wcast-align -Werror=address -Wstrict-prototypes -Werror=strict-prototypes -Wwrite-strings -Werror=write-strings -Werror-implicit-function-declaration -Wpointer-arith -Werror=pointer-arith -Wdeclaration-after-statement -Werror=declaration-after-statement -Wreturn-type -Werror=return-type -Wuninitialized -Werror=uninitialized -Werror=strict-overflow -Wstrict-overflow=2 -Wno-format-zero-length -Wmissing-field-initializers -Wformat-security -Werror=format-security -fno-common -Wformat -fno-common -fstack-protector-strong -DHAVE_SIGNAL_H
  }

  MdeModulePkg/Bus/Pci/PciBusDxe/UnitTest/TestPciBusDxe.inf {
  <LibraryClasses>
    NULL|MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  }

  MdeModulePkg/Universal/RegularExpressionDxe/UnitTest/TestRegularExpressionDxe.inf {
    <LibraryClasses>
    NULL|MdeModulePkg/Universal/RegularExpressionDxe/RegularExpressionDxe.inf
  }

  MdeModulePkg/Universal/RegularExpressionDxe/RegularExpressionDxe.inf {
  <BuildOptions>
    # Enable STDARG for variable arguments
    *_*_*_CC_FLAGS = -DHAVE_STDARG_H -Dsprintf_s=SPRINTF_S

    # Override MSFT build option to remove /Oi and /GL
    MSFT:*_*_*_CC_FLAGS          = /GL-
    INTEL:*_*_*_CC_FLAGS         = /Oi-

    # Oniguruma: potentially uninitialized local variable used
    MSFT:*_*_*_CC_FLAGS = /wd4701 /wd4703

    # Oniguruma: intrinsic function not declared
    MSFT:*_*_*_CC_FLAGS = /wd4164

    # Oniguruma: old style declaration in st.c
    MSFT:*_*_*_CC_FLAGS = /wd4131

    # Oniguruma: 'type cast' : truncation from 'OnigUChar *' to 'unsigned int'
    MSFT:*_*_*_CC_FLAGS = /wd4305 /wd4306

    # Oniguruma: nameless union declared in regparse.h
    MSFT:*_*_*_CC_FLAGS = /wd4201

    # Oniguruma: 'type cast' : "int" to "OnigUChar", function pointer to "void *"
    MSFT:*_*_*_CC_FLAGS = /wd4244 /wd4054

    # Oniguruma: previous local declaration
    MSFT:*_*_*_CC_FLAGS = /wd4456

    # Oniguruma: signed and unsigned mismatch/cast
    MSFT:*_*_*_CC_FLAGS = /wd4018 /wd4245 /wd4389

    # Oniguruma: tag_end in parse_callout_of_name
    GCC:*_*_*_CC_FLAGS = -Wno-error=maybe-uninitialized

    # Not add -Wno-error=maybe-uninitialized option for XCODE
    # XCODE doesn't know this option
    XCODE:*_*_*_CC_FLAGS =
  }

  MdeModulePkg/Library/UefiVariablePolicyLib/UnitTest/UefiVariablePolicyUnitTest.inf {
    <LibraryClasses>
      UefiVariablePolicyLib|MdeModulePkg/Library/UefiVariablePolicyLib/UefiVariablePolicyLib.inf
      SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  }

!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc