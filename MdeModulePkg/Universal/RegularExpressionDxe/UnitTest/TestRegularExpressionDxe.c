/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/RegularExpressionProtocol.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"RegularExpression Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

EFI_STATUS
EFIAPI
RegularExpressionDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

UNIT_TEST_STATUS
EFIAPI
TestRegularExpressionMatchString (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                       Status;
  EFI_REGULAR_EXPRESSION_PROTOCOL  *RegularExpression;
  BOOLEAN                          Result;
  UINTN                            CapturesCount;
  EFI_REGEX_CAPTURE                *Captures;

  Status = gBS->LocateProtocol (&gEfiRegularExpressionProtocolGuid, NULL, &RegularExpression);
  if (EFI_ERROR(Status)) {
    return UNIT_TEST_PASSED;
  }

  CapturesCount = 0;
  Status = RegularExpression->MatchString (RegularExpression, L"Test", L"T*", NULL, &Result, &Captures, &CapturesCount);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  UT_ASSERT_TRUE (Result);
  UT_ASSERT_EQUAL (CapturesCount, 1);
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestRegularExpressionGetInfo (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                       Status;
  EFI_REGULAR_EXPRESSION_PROTOCOL  *RegularExpression;
  UINTN                            RegExSyntaxTypeListSize;
  EFI_REGEX_SYNTAX_TYPE            RegExSyntaxTypeList[3];

  Status = gBS->LocateProtocol (&gEfiRegularExpressionProtocolGuid, NULL, &RegularExpression);
  if (EFI_ERROR(Status)) {
    return UNIT_TEST_PASSED;
  }

  RegExSyntaxTypeListSize = sizeof(RegExSyntaxTypeList);
  Status = RegularExpression->GetInfo (RegularExpression, &RegExSyntaxTypeListSize, RegExSyntaxTypeList);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  UT_ASSERT_EQUAL (RegExSyntaxTypeListSize, sizeof(RegExSyntaxTypeList[1]) * 2);
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

  RegularExpressionDxeEntry (gImageHandle, gST);

  Fw = NULL;
  TestSuite = NULL;

  AsciiStrToUnicodeStrS (gEfiCallerBaseName, ShortName, sizeof(ShortName)/sizeof(ShortName[0]));
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"RegularExpression Basic Test Suite", L"Common.RegularExpression.Basic", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for RegularExpression Basic Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test RegularExpressionMatchString", L"Common.RegularExpression.Basic.RegularExpressionMatchString", TestRegularExpressionMatchString, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test RegularExpressionGetInfo", L"Common.RegularExpression.Basic.RegularExpressionGetInfo", TestRegularExpressionGetInfo, NULL, NULL, NULL);

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
