/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"ImageVerification Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

EFI_STATUS
EFIAPI
DxeImageVerificationHandler (
  IN  UINT32                           AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer,
  IN  UINTN                            FileSize,
  IN  BOOLEAN                          BootPolicy
  );

UINT8  mFileBuffer[] = {
  0xAA, 0xBB, 0xCC, 0xDD,
};

UNIT_TEST_STATUS
EFIAPI
TestImageVerification (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS                 Status;
  VOID                       *FileBuffer;
  UINTN                      FileSize;

  FileBuffer = mFileBuffer;
  FileSize = sizeof(mFileBuffer);
  Status = DxeImageVerificationHandler (
             0,
             NULL, // File : EFI_DEVICE_PATH_PROTOCOL
             FileBuffer,
             FileSize,
             FALSE
             );
  UT_ASSERT_NOT_EFI_ERROR(Status);
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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"ImageVerification Basic Test Suite", L"Common.ImageVerification.Basic", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for ImageVerification Basic Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test ImageVerification", L"Common.ImageVerification.Basic.Basic", TestImageVerification, NULL, NULL, NULL);

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
