/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define UNIT_TEST_NAME        L"Static PCD Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

UNIT_TEST_STATUS
EFIAPI
TestStaticPcdFeature (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  BOOLEAN  Feature;

  Feature = FeaturePcdGet(PcdTestFeaturePlagTrue);
  UT_ASSERT_TRUE(Feature);

  Feature = FeaturePcdGet(PcdTestFeaturePlagFalse);
  UT_ASSERT_FALSE(Feature);

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestStaticPcdFixedAtBuild (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  BOOLEAN  DataBool;
  UINT8    DataUint8;
  UINT16   DataUint16;
  UINT32   DataUint32;
  UINT64   DataUint64;
  VOID     *DataPtr;
  UINTN    Size;
  UINT8    BufAll0[] = {0x0, 0x0, 0x0};
  UINT8    BufAll1[] = {0xFF, 0xFF, 0xFF};

  DataBool = FixedPcdGetBool(PcdTestFixedAtBuildBoolTrue);
  UT_ASSERT_TRUE(DataBool);

  DataBool = FixedPcdGetBool(PcdTestFixedAtBuildBoolFalse);
  UT_ASSERT_FALSE(DataBool);

  DataUint8 = FixedPcdGet8(PcdTestFixedAtBuildUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  DataUint8 = FixedPcdGet8(PcdTestFixedAtBuildUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  DataUint16 = FixedPcdGet16(PcdTestFixedAtBuildUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0x0);

  DataUint16 = FixedPcdGet16(PcdTestFixedAtBuildUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  DataUint32 = FixedPcdGet32(PcdTestFixedAtBuildUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0x0);

  DataUint32 = FixedPcdGet32(PcdTestFixedAtBuildUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  DataUint64 = FixedPcdGet64(PcdTestFixedAtBuildUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0x0);

  DataUint64 = FixedPcdGet64(PcdTestFixedAtBuildUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Size = FixedPcdGetSize(PcdTestFixedAtBuildVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StrAsc"));

  DataPtr = FixedPcdGetPtr(PcdTestFixedAtBuildVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StrAsc", sizeof("StrAsc"));

  Size = FixedPcdGetSize(PcdTestFixedAtBuildVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StrUni"));

  DataPtr = FixedPcdGetPtr(PcdTestFixedAtBuildVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StrUni", sizeof(L"StrUni"));

  Size = FixedPcdGetSize(PcdTestFixedAtBuildVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = FixedPcdGetPtr(PcdTestFixedAtBuildVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = FixedPcdGetSize(PcdTestFixedAtBuildVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = FixedPcdGetPtr(PcdTestFixedAtBuildVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestStaticPcdPatchableInModuleGet (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  BOOLEAN  DataBool;
  UINT8    DataUint8;
  UINT16   DataUint16;
  UINT32   DataUint32;
  UINT64   DataUint64;
  VOID     *DataPtr;
  UINTN    Size;
  UINT8    BufAll0[] = {0x0, 0x0, 0x0};
  UINT8    BufAll1[] = {0xFF, 0xFF, 0xFF};

  DataBool = PatchPcdGetBool(PcdTestPatchableInModuleBoolTrue);
  UT_ASSERT_TRUE(DataBool);

  DataBool = PatchPcdGetBool(PcdTestPatchableInModuleBoolFalse);
  UT_ASSERT_FALSE(DataBool);

  DataUint8 = PatchPcdGet8(PcdTestPatchableInModuleUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  DataUint8 = PatchPcdGet8(PcdTestPatchableInModuleUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  DataUint16 = PatchPcdGet16(PcdTestPatchableInModuleUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0x0);

  DataUint16 = PatchPcdGet16(PcdTestPatchableInModuleUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  DataUint32 = PatchPcdGet32(PcdTestPatchableInModuleUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0x0);

  DataUint32 = PatchPcdGet32(PcdTestPatchableInModuleUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  DataUint64 = PatchPcdGet64(PcdTestPatchableInModuleUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0x0);

  DataUint64 = PatchPcdGet64(PcdTestPatchableInModuleUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Size = PatchPcdGetSize(PcdTestPatchableInModuleVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StrAsc"));

  DataPtr = PatchPcdGetPtr(PcdTestPatchableInModuleVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StrAsc", sizeof("StrAsc"));

  Size = PatchPcdGetSize(PcdTestPatchableInModuleVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StrUni"));

  DataPtr = PatchPcdGetPtr(PcdTestPatchableInModuleVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StrUni", sizeof(L"StrUni"));

  Size = PatchPcdGetSize(PcdTestPatchableInModuleVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PatchPcdGetPtr(PcdTestPatchableInModuleVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = PatchPcdGetSize(PcdTestPatchableInModuleVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PatchPcdGetPtr(PcdTestPatchableInModuleVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestStaticPcdPatchableInModuleSet (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  EFI_STATUS Status;
  BOOLEAN  DataBool;
  UINT8    DataUint8;
  UINT16   DataUint16;
  UINT32   DataUint32;
  UINT64   DataUint64;
  VOID     *DataPtr;
  UINTN    Size;
  UINT8    BufAll0[] = {0x0, 0x0, 0x0, 0x0, 0x0};
  UINT8    BufAll1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  Status = PcdSetBoolS(PcdTestPatchableInModuleBoolTrue, FALSE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetBool(PcdTestPatchableInModuleBoolTrue);
  UT_ASSERT_FALSE(DataBool);

  Status = PcdSetBoolS(PcdTestPatchableInModuleBoolFalse, TRUE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetBool(PcdTestPatchableInModuleBoolFalse);
  UT_ASSERT_TRUE(DataBool);

  Status = PcdSet8S(PcdTestPatchableInModuleUint8All0, 0xFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGet8(PcdTestPatchableInModuleUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  Status = PcdSet8S(PcdTestPatchableInModuleUint8All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGet8(PcdTestPatchableInModuleUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  Status = PcdSet16S(PcdTestPatchableInModuleUint16All0, 0xFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGet16(PcdTestPatchableInModuleUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  Status = PcdSet16S(PcdTestPatchableInModuleUint16All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGet16(PcdTestPatchableInModuleUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0x0);
  
  Status = PcdSet32S(PcdTestPatchableInModuleUint32All0, 0xFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PcdGet32(PcdTestPatchableInModuleUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  Status = PcdSet32S(PcdTestPatchableInModuleUint32All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PatchPcdGet32(PcdTestPatchableInModuleUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0x0);
  
  Status = PcdSet64S(PcdTestPatchableInModuleUint64All0, 0xFFFFFFFFFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGet64(PcdTestPatchableInModuleUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Status = PcdSet64S(PcdTestPatchableInModuleUint64All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGet64(PcdTestPatchableInModuleUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0x0);
  
  Size = sizeof("StringAscii");
  Status = PcdSetPtrS(PcdTestPatchableInModuleVoidStrAsc, &Size, "StringAscii");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestPatchableInModuleVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StringAscii"));

  DataPtr = PcdGetPtr(PcdTestPatchableInModuleVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StringAscii", sizeof("StringAscii"));
  
  Size = sizeof(L"StringUnicode");
  Status = PcdSetPtrS(PcdTestPatchableInModuleVoidStrUni, &Size, L"StringUnicode");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestPatchableInModuleVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StringUnicode"));

  DataPtr = PcdGetPtr(PcdTestPatchableInModuleVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StringUnicode", sizeof(L"StringUnicode"));

  Size = sizeof(BufAll0);
  Status = PcdSetPtrS(PcdTestPatchableInModuleVoidBufAll0, &Size, BufAll0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestPatchableInModuleVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PcdGetPtr(PcdTestPatchableInModuleVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = sizeof(BufAll1);
  Status = PcdSetPtrS(PcdTestPatchableInModuleVoidBufAll1, &Size, BufAll1);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestPatchableInModuleVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PcdGetPtr(PcdTestPatchableInModuleVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"Static PCD Test Suite", L"Common.PCD.Static", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for Static PCD Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test Feature PCD", L"Common.PCD.Static.Feature", TestStaticPcdFeature, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test FixedAtBuild PCD", L"Common.PCD.Static.FixedAtBuild", TestStaticPcdFixedAtBuild, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PatchableInModule PCD Get", L"Common.PCD.Static.PatchableInModuleGet", TestStaticPcdPatchableInModuleGet, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PatchableInModule PCD Set", L"Common.PCD.Static.PatchableInModuleSet", TestStaticPcdPatchableInModuleSet, NULL, NULL, NULL);

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
