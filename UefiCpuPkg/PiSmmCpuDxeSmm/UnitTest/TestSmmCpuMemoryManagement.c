/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiSmm.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmMemoryAttribute.h>

#include "PiSmmCpuDxeSmm.h"

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"PiSmmCpu Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

UINTN  gTestCr3;
VOID
SetPageTableBase (
  IN UINTN   Cr3
  );

VOID
FreePageTableMemory (
  VOID
  );

EFI_STATUS
SetShadowStack (
  IN  UINTN                                      Cr3,
  IN  EFI_PHYSICAL_ADDRESS                       BaseAddress,
  IN  UINT64                                     Length
  );

VOID *
GetPageTableEntry (
  IN  PHYSICAL_ADDRESS                  Address,
  OUT PAGE_ATTRIBUTE                    *PageAttribute
  );

EFI_STATUS
EFIAPI
EdkiiSmmSetMemoryAttributes (
  IN  EDKII_SMM_MEMORY_ATTRIBUTE_PROTOCOL   *This,
  IN  EFI_PHYSICAL_ADDRESS                  BaseAddress,
  IN  UINT64                                Length,
  IN  UINT64                                Attributes
  );

EFI_STATUS
EFIAPI
EdkiiSmmClearMemoryAttributes (
  IN  EDKII_SMM_MEMORY_ATTRIBUTE_PROTOCOL   *This,
  IN  EFI_PHYSICAL_ADDRESS                  BaseAddress,
  IN  UINT64                                Length,
  IN  UINT64                                Attributes
  );

EFI_STATUS
EFIAPI
EdkiiSmmGetMemoryAttributes (
  IN  EDKII_SMM_MEMORY_ATTRIBUTE_PROTOCOL   *This,
  IN  EFI_PHYSICAL_ADDRESS                  BaseAddress,
  IN  UINT64                                Length,
  IN  UINT64                                *Attributes
  );

VOID
EFIAPI
SmmCpuMemoryManagementSuiteSetup (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  )
{
  gTestCr3 = SmmInitPageTable ();
  AsmWriteCr3 (gTestCr3);
  return ;
}

VOID
EFIAPI
SmmCpuMemoryManagementSuiteClean (
  UNIT_TEST_FRAMEWORK_HANDLE  Framework
  )
{
  FreePageTableMemory ();
  return ;
}

UNIT_TEST_STATUS
EFIAPI
TestSmmSetMemoryAttributes (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS  Status;
  UINT64      Attributes;

  Status = EdkiiSmmSetMemoryAttributes (NULL, 0x9000, 0x1000, EFI_MEMORY_RP);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = EdkiiSmmGetMemoryAttributes (NULL, 0x9000, 0x1000, &Attributes);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  UT_ASSERT_EQUAL(Attributes, EFI_MEMORY_RP);
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestSmmShadowStackPage (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                        Status;
  UINT64                            *PageEntry;
  PAGE_ATTRIBUTE                    PageAttribute;

  Status = SetShadowStack (gTestCr3, 0xA000, 0x1000);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  PageEntry = GetPageTableEntry (0xA000, &PageAttribute);
  UT_ASSERT_EQUAL((*PageEntry & IA32_PG_RW), 0);
  UT_ASSERT_EQUAL((*PageEntry & IA32_PG_D), IA32_PG_D);
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestSmmReadOnlyPage (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                        Status;
  UINT64                            *PageEntry;
  PAGE_ATTRIBUTE                    PageAttribute;

  Status = EdkiiSmmSetMemoryAttributes (NULL, 0xB000, 0x1000, EFI_MEMORY_RO);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  PageEntry = GetPageTableEntry (0xB000, &PageAttribute);
  UT_ASSERT_EQUAL((*PageEntry & IA32_PG_RW), 0);
  UT_ASSERT_EQUAL((*PageEntry & IA32_PG_D), 0);
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestSmmClearMemoryAttributes (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS  Status;
  UINT64      Attributes;

  Status = EdkiiSmmClearMemoryAttributes (NULL, 0x9000, 0x1000, EFI_MEMORY_RP);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = EdkiiSmmGetMemoryAttributes (NULL, 0x9000, 0x1000, &Attributes);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  UT_ASSERT_NOT_EQUAL(Attributes, EFI_MEMORY_RP);
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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"SmmCpuMemoryManagement PageTable Test Suite", L"Common.SmmCpuMemoryManagement.PageTable", SmmCpuMemoryManagementSuiteSetup, SmmCpuMemoryManagementSuiteClean);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for SmmCpuMemoryManagement PageTable Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test SmmSetMemoryAttributes", L"Common.SmmCpuMemoryManagement.PageTable.SmmSetMemoryAttributes", TestSmmSetMemoryAttributes, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test SmmShadowStackPage", L"Common.SmmCpuMemoryManagement.PageTable.SmmShadowStackPage", TestSmmShadowStackPage, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test SmmReadOnlyPage", L"Common.SmmCpuMemoryManagement.PageTable.SmmReadOnlyPage", TestSmmReadOnlyPage, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test SmmClearMemoryAttributes", L"Common.SmmCpuMemoryManagement.PageTable.SmmClearMemoryAttributes", TestSmmClearMemoryAttributes, NULL, NULL, NULL);

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
