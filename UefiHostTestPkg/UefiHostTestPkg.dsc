## @file UefiHostTestPkg.dsc
# 
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostTestPkg
  PLATFORM_GUID                  = C2A9A6F1-57DC-4397-ACB6-BCF5DF454BC0
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostTestPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLibHost/BaseLibHost.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  MemoryAllocationLib|MdePkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  DebugLib|MdePkg/Library/DebugLibHost/DebugLibHost.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  HobLib|MdePkg/Library/HobLibHost/HobLibHost.inf
  SmmMemLib|MdePkg/Library/SmmMemLibHost/SmmMemLibHost.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf

[LibraryClasses.common.USER_DEFINED]

[Components]
  MdePkg/Library/BaseLibHost/BaseLibHost.inf
  UefiHostTestPkg/Library/BaseLibNullCpuid/BaseLibNullCpuid.inf
  UefiHostTestPkg/Library/BaseLibNullMsr/BaseLibNullMsr.inf
  MdePkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  MdePkg/Library/BaseCpuLibHost/BaseCpuLibHost.inf
  MdePkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  MdePkg/Library/BaseTimerLibHost/BaseTimerLibHost.inf
  MdePkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  MdePkg/Library/DebugLibHost/DebugLibHost.inf
  MdePkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  MdePkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  MdePkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  MdePkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf
  MdePkg/Library/HobLibHost/HobLibHost.inf
  MdePkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  MdePkg/Library/SmmMemLibHost/SmmMemLibHost.inf
  MdePkg/Library/UefiLibHost/UefiLibHost.inf
  MdePkg/Library/PeimEntryPointHost/PeimEntryPointHost.inf
  MdePkg/Library/UefiDriverEntryPointHost/UefiDriverEntryPointHost.inf
  UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf

!include UefiHostUnitTestPkg/UefiHostUnitTestBuildOption.dsc
