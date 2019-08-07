/** @file
  UEFI RNG (Random Number Generator) Protocol test application.

Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/        

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/Rng.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"RngDxe Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

EFI_STATUS
EFIAPI
RngDriverEntry (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  );

UNIT_TEST_STATUS
EFIAPI
TestRngGetInfo (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS         Status;
  EFI_RNG_PROTOCOL   *Rng;
  UINTN              RngAlgListSize;
  EFI_RNG_ALGORITHM  RngAlgList[10];

  Status = gBS->LocateProtocol (&gEfiRngProtocolGuid, NULL, (VOID **)&Rng);
  if (EFI_ERROR(Status)) {
    return UNIT_TEST_PASSED;
  }

  //-----------------------------------------
  // Rng->GetInfo() interface test.
  //-----------------------------------------
  
  RngAlgListSize = 0;
  Status = Rng->GetInfo (Rng, &RngAlgListSize, NULL);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_BUFFER_TOO_SMALL);

  //
  // Print out the supported RNG algorithm GUIDs
  //
  UT_ASSERT_EQUAL (RngAlgListSize, 2 * sizeof (EFI_RNG_ALGORITHM));

  Status = Rng->GetInfo (Rng, &RngAlgListSize, RngAlgList);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);

  UT_ASSERT_MEM_EQUAL (&RngAlgList[0], &gEfiRngAlgorithmSp80090Ctr256Guid, sizeof(EFI_GUID));
  UT_ASSERT_MEM_EQUAL (&RngAlgList[1], &gEfiRngAlgorithmRaw, sizeof(EFI_GUID));

  return UNIT_TEST_PASSED;
}
  
UNIT_TEST_STATUS
EFIAPI
TestRngGetRNG (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS         Status;
  EFI_RNG_PROTOCOL   *Rng;
  UINT8              Rand[32];
  UINTN              RandSize = sizeof(Rand);
  UINTN              Index;

  Status = gBS->LocateProtocol (&gEfiRngProtocolGuid, NULL, (VOID **)&Rng);
  if (EFI_ERROR(Status)) {
    return UNIT_TEST_PASSED;
  }


  //
  // RNG with default algorithm
  //
  Status = Rng->GetRNG (Rng, NULL, RandSize, Rand);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  
  //
  // RNG with SP800-90-HMAC-256
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmSp80090Hmac256Guid, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //
  // RNG with SP800-90-HASH-256
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmSp80090Hash256Guid, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //
  // RNG with SP800-90-CTR-256
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmSp80090Ctr256Guid, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //
  // RNG with X9.31-3DES
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmX9313DesGuid, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //
  // RNG with X9.31-AES
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmX931AesGuid, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //
  // RNG with RAW Entropy
  //
  Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmRaw, RandSize, Rand);
  UT_ASSERT_TRUE ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED));

  //-----------------------------------------
  // Random Number Generator test.
  //-----------------------------------------

  RandSize = 1;
  for (Index = 0; Index < 20; Index++) {
    Status = Rng->GetRNG (Rng, NULL, RandSize, Rand);
    UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
    RandSize +=1;
  }

  //-----------------------------------------
  // Random Number Generator test.
  //-----------------------------------------
  RandSize = 32;
  for (Index = 0; Index < 20; Index++) {
    Status = Rng->GetRNG (Rng, &gEfiRngAlgorithmRaw, RandSize, Rand);
    UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
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
  
  RngDriverEntry (gImageHandle, gST);

  Fw = NULL;
  TestSuite = NULL;

  AsciiStrToUnicodeStrS (gEfiCallerBaseName, ShortName, sizeof(ShortName)/sizeof(ShortName[0]));
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"RngDxe Basic Test Suite", L"Common.Rng.Basic", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for RngDxe Basic Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test Rng GetInfo", L"Common.Rng.Basic.GetInfo", TestRngGetInfo, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test Rng GetRNG",  L"Common.Rng.Basic.GetRNG",  TestRngGetRNG, NULL, NULL, NULL);

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
