/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/PciHostBridgeStubLib.h>
#include <Library/PciSegmentStubLib.h>
#include <Library/PciSegmentLib.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#include "PciBus.h"

#define UNIT_TEST_NAME        L"PciBus Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

VOID
EFIAPI
ProcessLibraryConstructorList (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
PciBusEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

#define gPciRootBridge(Segment) \
  { \
    { \
      ACPI_DEVICE_PATH, \
      ACPI_DP, \
      { \
        (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), \
        (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8) \
      }, \
    }, \
    EISA_PNP_ID (0x0A03), \
    (Segment) \
  }

#define gPci(Device, Function) \
  { \
    { \
      HARDWARE_DEVICE_PATH, \
      HW_PCI_DP, \
      { \
        (UINT8) (sizeof (PCI_DEVICE_PATH)), \
        (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8) \
      } \
    }, \
    (Function), \
    (Device) \
  }

#define gEndEntire \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { END_DEVICE_PATH_LENGTH, 0 } \
  }

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} TEST_PCI_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciBridge;
  EFI_DEVICE_PATH_PROTOCOL  End;
} TEST_PCI_BRIDGE_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciBridge;
  PCI_DEVICE_PATH           PciDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} TEST_PCI2_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciBridge;
  PCI_DEVICE_PATH           PciBridge2;
  EFI_DEVICE_PATH_PROTOCOL  End;
} TEST_PCI_BRIDGE2_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciBridge;
  PCI_DEVICE_PATH           PciBridge2;
  PCI_DEVICE_PATH           PciDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} TEST_PCI3_DEVICE_PATH;

TEST_PCI_DEVICE_PATH         gTestPciDevicePath = {gPciRootBridge(0), gPci(0, 0), gEndEntire};
TEST_PCI_BRIDGE_DEVICE_PATH  gTestPciBridgeDevicePath = {gPciRootBridge(0), gPci(29, 0), gEndEntire};
TEST_PCI2_DEVICE_PATH        gTestPci2DevicePath = {gPciRootBridge(0), gPci(29, 0), gPci(0, 0), gEndEntire};
TEST_PCI_BRIDGE2_DEVICE_PATH gTestPciBridge2DevicePath = {gPciRootBridge(0), gPci(29, 0), gPci(1, 0), gEndEntire};
TEST_PCI3_DEVICE_PATH        gTestPci3DevicePath = {gPciRootBridge(0), gPci(29, 0), gPci(1, 0), gPci(0, 0), gEndEntire};

REGISTER_PCI_DEVICE_STRUCT   mTestPciDevices[] = {
  {
    .DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&gTestPciDevicePath,
    .Address = {0, 0, 0, 0, 0, 0},
    .PciId = {0x8086, 0xABCD, 0x00, HEADER_TYPE_DEVICE, {0x00, PCI_CLASS_BRIDGE_HOST, PCI_CLASS_BRIDGE}},
    .PciConfig = {
       .DeviceConfig = {
         .Bar = {
           {PCI_BAR_TYPE_32BIT_MEM, 0x10000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_64BIT_MEM, 0x100000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_IO, 0x100},
           {PCI_BAR_TYPE_INVALID, 0},
         },
         .ExpansionRomBar = {PCI_BAR_TYPE_INVALID, 0},
       },
    },
  },
  {
    .DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&gTestPciBridgeDevicePath,
    .Address = {0, 0, 29, 0, 0, 0},
    .PciId = {0x8086, 0xABCE, 0x00, HEADER_TYPE_PCI_TO_PCI_BRIDGE, {PCI_IF_BRIDGE_P2P, PCI_CLASS_BRIDGE_P2P, PCI_CLASS_BRIDGE}},
    .PciConfig = {
       .BridgeConfig = {
         .Bar = {
           {PCI_BAR_TYPE_32BIT_MEM, 0x10000},
           {PCI_BAR_TYPE_INVALID, 0},
         },
         .IoBaseLimit = {PCI_BASE_LIMIT_TYPE_32BIT_IO},
         .MemBaseLimit = {PCI_BASE_LIMIT_TYPE_MEM},
         .PMemBaseLimit = {PCI_BASE_LIMIT_TYPE_64BIT_PMEM},
         .ExpansionRomBar = {PCI_BAR_TYPE_INVALID, 0},
       },
    },
  },
  {
    .DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&gTestPci2DevicePath,
    .Address = {0, 0, 0, 0, 0, 0},
    .PciId = {0x8086, 0xABDE, 0x00, HEADER_TYPE_DEVICE, {0x00, PCI_CLASS_MASS_STORAGE_OTHER, PCI_CLASS_MASS_STORAGE}},
    .PciConfig = {
       .DeviceConfig = {
         .Bar = {
           {PCI_BAR_TYPE_32BIT_MEM, 0x1000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_64BIT_MEM, 0x10000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_IO, 0x10},
           {PCI_BAR_TYPE_INVALID, 0},
         },
         .ExpansionRomBar = {PCI_BAR_TYPE_INVALID, 0},
       },
    },
  },
  {
    .DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&gTestPciBridge2DevicePath,
    .Address = {0, 0, 1, 0, 0, 0},
    .PciId = {0x8086, 0xABCF, 0x00, HEADER_TYPE_PCI_TO_PCI_BRIDGE, {PCI_IF_BRIDGE_P2P, PCI_CLASS_BRIDGE_P2P, PCI_CLASS_BRIDGE}},
    .PciConfig = {
       .BridgeConfig = {
         .Bar = {
           {PCI_BAR_TYPE_IO, 0x100},
           {PCI_BAR_TYPE_INVALID, 0},
         },
         .IoBaseLimit = {PCI_BASE_LIMIT_TYPE_16BIT_IO},
         .MemBaseLimit = {PCI_BASE_LIMIT_TYPE_MEM},
         .PMemBaseLimit = {PCI_BASE_LIMIT_TYPE_32BIT_PMEM},
         .ExpansionRomBar = {PCI_BAR_TYPE_INVALID, 0},
       },
    },
  },
  {
    .DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&gTestPci3DevicePath,
    .Address = {0, 0, 0, 0, 0, 0},
    .PciId = {0x8086, 0xABEE, 0x00, HEADER_TYPE_DEVICE, {0x00, PCI_CLASS_MASS_STORAGE_OTHER, PCI_CLASS_MASS_STORAGE}},
    .PciConfig = {
       .DeviceConfig = {
         .Bar = {
           {PCI_BAR_TYPE_32BIT_MEM, 0x1000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_64BIT_MEM, 0x10000},
           {PCI_BAR_TYPE_INVALID, 0},
           {PCI_BAR_TYPE_IO, 0x10},
           {PCI_BAR_TYPE_INVALID, 0},
         },
         .ExpansionRomBar = {PCI_BAR_TYPE_INVALID, 0},
       },
    },
  },
};

VOID
EFIAPI
PciBusSuiteSetup (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  )
{
  UINT16  Data16;

  RegisterPciDevices (ARRAY_SIZE(mTestPciDevices), mTestPciDevices);
  Data16 = PciSegmentRead16 (0);

  ProcessLibraryConstructorList (gImageHandle, gST);

  InitializePciHostBridge (gImageHandle, gST);

  PciBusEntryPoint (gImageHandle, gST);
  
  return ;
}

VOID
EFIAPI
PciBusSuiteTeardown (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  )
{
  // TBD
  // Uninstall all the protocol
  // cleanup GCD
  return ;
}

UNIT_TEST_STATUS
EFIAPI
TestPciBusDxe (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *Handles;
  UINTN                           Index;
  UINTN                           HandleNum;
  EFI_HANDLE                      Controller;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &HandleNum,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return UNIT_TEST_PASSED;
  }

  for (Index = 0; Index < HandleNum; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiPciRootBridgeIoProtocolGuid, (VOID **)&PciRootBridgeIo);
    if (EFI_ERROR (Status)) {
      continue;
    }
    Controller = Handles[Index];

    if (gFullEnumeration) {
      Status = PciEnumerator (Controller, PciRootBridgeIo->ParentHandle);
    } else {
      //Status = PciEnumeratorLight (Controller);
    }
    StartPciDevices (Controller);
  }
  return UNIT_TEST_PASSED;
}

/**
  The main() function for setting up and running the tests.

  @retval EFI_SUCCESS on successful running.
  @retval Other error code on failure.
**/
int main()
{
  EFI_STATUS                Status;
  UNIT_TEST_FRAMEWORK       *Fw;
  UNIT_TEST_SUITE           *TestSuite;
  CHAR16                    ShortName[MAX_STRING_SIZE];

  Fw = NULL;
  TestSuite = NULL;

  AsciiStrToUnicodeStrS (gEfiCallerBaseName, ShortName, sizeof(ShortName)/sizeof(ShortName[0]));
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"PciBus Basic Test Suite", L"Common.PciBus.Basic", PciBusSuiteSetup, PciBusSuiteTeardown);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for PciBus Basic Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test PciBus", L"Common.PciBus.Basic.ResourceAllocation", TestPciBusDxe, NULL, NULL, NULL);

  //
  // Execute the tests.
  //
  Status = RunAllTestSuites(Fw);

EXIT:
  if (Fw != NULL) {
    FreeUnitTestFramework(Fw);
  }

  return Status;
}
