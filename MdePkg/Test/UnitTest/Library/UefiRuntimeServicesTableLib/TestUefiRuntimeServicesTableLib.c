/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"UefiRuntimeServices Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

#define  TEST_VAR_NAME L"TestVar"
EFI_GUID gTestVariableGuid = {0x82f9e117, 0x543f, 0x4638, {0xab, 0x7d, 0xc4, 0xe8, 0x5f, 0xde, 0x86, 0x8c}};

UNIT_TEST_STATUS
EFIAPI
TestVariable (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  UINT8       Data;
  UINT32      Attribute;

  Data = 0x5A;
  Size = sizeof(Data);
  Status = gRT->SetVariable (
                  TEST_VAR_NAME,
                  &gTestVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  Size,
                  &Data
                  );
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = gRT->GetVariable (
                  TEST_VAR_NAME,
                  &gTestVariableGuid,
                  &Attribute,
                  &Size,
                  &Data
                  );
  UT_ASSERT_NOT_EFI_ERROR(Status);
  UT_ASSERT_EQUAL(Attribute, EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS);
  UT_ASSERT_EQUAL(Size, sizeof(Data));
  UT_ASSERT_EQUAL(Data, 0x5A);
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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"UefiRuntimeServices Variable Test Suite", L"Common.UefiRuntimeServices.Variable", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for UefiRuntimeServices Variable Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test Variable", L"Common.UefiBootServices.Variable.GetSecVar", TestVariable, NULL, NULL, NULL);

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
