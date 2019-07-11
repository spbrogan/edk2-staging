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

#define UNIT_TEST_NAME        L"Dynamic PCD Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicPreReq (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  RETURN_STATUS Status;
  UINTN    Size;
  UINT8    BufAll0[] = {0x0, 0x0, 0x0};
  UINT8    BufAll1[] = {0xFF, 0xFF, 0xFF};

  Status = PcdSetBoolS(PcdTestDynamicBoolTrue, TRUE);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetBoolS(PcdTestDynamicBoolFalse, FALSE);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet8S(PcdTestDynamicUint8All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet8S(PcdTestDynamicUint8All1, 0xFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet16S(PcdTestDynamicUint16All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet16S(PcdTestDynamicUint16All1, 0xFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet32S(PcdTestDynamicUint32All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet32S(PcdTestDynamicUint32All1, 0xFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet64S(PcdTestDynamicUint64All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSet64S(PcdTestDynamicUint64All1, 0xFFFFFFFFFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof("StrAsc");
  Status = PcdSetPtrS(PcdTestDynamicVoidStrAsc, &Size, "StrAsc");
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(L"StrUni");
  Status = PcdSetPtrS(PcdTestDynamicVoidStrUni, &Size, L"StrUni");
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(BufAll0);
  Status = PcdSetPtrS(PcdTestDynamicVoidBufAll0, &Size, BufAll0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(BufAll1);
  Status = PcdSetPtrS(PcdTestDynamicVoidBufAll1, &Size, BufAll1);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicGet (
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

  DataBool = PcdGetBool(PcdTestDynamicBoolTrue);
  UT_ASSERT_TRUE(DataBool);

  DataBool = PcdGetBool(PcdTestDynamicBoolFalse);
  UT_ASSERT_FALSE(DataBool);

  DataUint8 = PcdGet8(PcdTestDynamicUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  DataUint8 = PcdGet8(PcdTestDynamicUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  DataUint16 = PcdGet16(PcdTestDynamicUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0x0);

  DataUint16 = PcdGet16(PcdTestDynamicUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  DataUint32 = PcdGet32(PcdTestDynamicUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0x0);

  DataUint32 = PcdGet32(PcdTestDynamicUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  DataUint64 = PcdGet64(PcdTestDynamicUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0x0);

  DataUint64 = PcdGet64(PcdTestDynamicUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Size = PcdGetSize(PcdTestDynamicVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StrAsc"));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StrAsc", sizeof("StrAsc"));

  Size = PcdGetSize(PcdTestDynamicVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StrUni"));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StrUni", sizeof(L"StrUni"));

  Size = PcdGetSize(PcdTestDynamicVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = PcdGetSize(PcdTestDynamicVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicSet (
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

  Status = PcdSetBoolS(PcdTestDynamicBoolTrue, FALSE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetBool(PcdTestDynamicBoolTrue);
  UT_ASSERT_FALSE(DataBool);

  Status = PcdSetBoolS(PcdTestDynamicBoolFalse, TRUE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetBool(PcdTestDynamicBoolFalse);
  UT_ASSERT_TRUE(DataBool);

  Status = PcdSet8S(PcdTestDynamicUint8All0, 0xFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGet8(PcdTestDynamicUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  Status = PcdSet8S(PcdTestDynamicUint8All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGet8(PcdTestDynamicUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  Status = PcdSet16S(PcdTestDynamicUint16All0, 0xFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGet16(PcdTestDynamicUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  Status = PcdSet16S(PcdTestDynamicUint16All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGet16(PcdTestDynamicUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0x0);
  
  Status = PcdSet32S(PcdTestDynamicUint32All0, 0xFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PcdGet32(PcdTestDynamicUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  Status = PcdSet32S(PcdTestDynamicUint32All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PcdGet32(PcdTestDynamicUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0x0);
  
  Status = PcdSet64S(PcdTestDynamicUint64All0, 0xFFFFFFFFFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGet64(PcdTestDynamicUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Status = PcdSet64S(PcdTestDynamicUint64All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGet64(PcdTestDynamicUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0x0);
  
  Size = sizeof("StringAscii");
  Status = PcdSetPtrS(PcdTestDynamicVoidStrAsc, &Size, "StringAscii");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestDynamicVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StringAscii"));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StringAscii", sizeof("StringAscii"));
  
  Size = sizeof(L"StringUnicode");
  Status = PcdSetPtrS(PcdTestDynamicVoidStrUni, &Size, L"StringUnicode");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestDynamicVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StringUnicode"));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StringUnicode", sizeof(L"StringUnicode"));

  Size = sizeof(BufAll0);
  Status = PcdSetPtrS(PcdTestDynamicVoidBufAll0, &Size, BufAll0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestDynamicVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = sizeof(BufAll1);
  Status = PcdSetPtrS(PcdTestDynamicVoidBufAll1, &Size, BufAll1);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetSize(PcdTestDynamicVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PcdGetPtr(PcdTestDynamicVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicExPreReq (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  RETURN_STATUS Status;
  UINTN    Size;
  UINT8    BufAll0[] = {0x0, 0x0, 0x0};
  UINT8    BufAll1[] = {0xFF, 0xFF, 0xFF};

  Status = PcdSetExBoolS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolTrue, TRUE);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetExBoolS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolFalse, FALSE);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx8S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx8S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All1, 0xFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx16S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx16S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All1, 0xFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx32S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx32S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All1, 0xFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx64S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All0, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Status = PcdSetEx64S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All1, 0xFFFFFFFFFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof("StrAsc");
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc, &Size, "StrAsc");
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(L"StrUni");
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni, &Size, L"StrUni");
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(BufAll0);
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0, &Size, BufAll0);
  UT_ASSERT_NOT_EFI_ERROR(Status);

  Size = sizeof(BufAll1);
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1, &Size, BufAll1);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicExGet (
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

  DataBool = PcdGetExBool(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolTrue);
  UT_ASSERT_TRUE(DataBool);

  DataBool = PcdGetExBool(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolFalse);
  UT_ASSERT_FALSE(DataBool);

  DataUint8 = PcdGetEx8(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  DataUint8 = PcdGetEx8(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  DataUint16 = PcdGetEx16(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0x0);

  DataUint16 = PcdGetEx16(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  DataUint32 = PcdGetEx32(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0x0);

  DataUint32 = PcdGetEx32(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  DataUint64 = PcdGetEx64(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0x0);

  DataUint64 = PcdGetEx64(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StrAsc"));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StrAsc", sizeof("StrAsc"));

  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StrUni"));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StrUni", sizeof(L"StrUni"));

  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll1, sizeof(BufAll1));

  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestDynamicPcdDynamicExSet (
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

  Status = PcdSetExBoolS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolTrue, FALSE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetExBool(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolTrue);
  UT_ASSERT_FALSE(DataBool);

  Status = PcdSetExBoolS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolFalse, TRUE);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataBool = PcdGetExBool(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExBoolFalse);
  UT_ASSERT_TRUE(DataBool);

  Status = PcdSetEx8S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All0, 0xFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGetEx8(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All0);
  UT_ASSERT_EQUAL(DataUint8, 0xFF);

  Status = PcdSetEx8S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint8 = PcdGetEx8(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint8All1);
  UT_ASSERT_EQUAL(DataUint8, 0x0);

  Status = PcdSetEx16S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All0, 0xFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGetEx16(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All0);
  UT_ASSERT_EQUAL(DataUint16, 0xFFFF);

  Status = PcdSetEx16S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint16 = PcdGetEx16(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint16All1);
  UT_ASSERT_EQUAL(DataUint16, 0x0);
  
  Status = PcdSetEx32S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All0, 0xFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PcdGetEx32(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All0);
  UT_ASSERT_EQUAL(DataUint32, 0xFFFFFFFF);

  Status = PcdSetEx32S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint32 = PcdGetEx32(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint32All1);
  UT_ASSERT_EQUAL(DataUint32, 0x0);
  
  Status = PcdSetEx64S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All0, 0xFFFFFFFFFFFFFFFF);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGetEx64(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All0);
  UT_ASSERT_EQUAL(DataUint64, 0xFFFFFFFFFFFFFFFF);

  Status = PcdSetEx64S(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All1, 0x0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  DataUint64 = PcdGetEx64(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExUint64All1);
  UT_ASSERT_EQUAL(DataUint64, 0x0);
  
  Size = sizeof("StringAscii");
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc, &Size, "StringAscii");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc);
  UT_ASSERT_EQUAL(Size, sizeof("StringAscii"));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrAsc);
  UT_ASSERT_MEM_EQUAL(DataPtr, "StringAscii", sizeof("StringAscii"));
  
  Size = sizeof(L"StringUnicode");
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni, &Size, L"StringUnicode");
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni);
  UT_ASSERT_EQUAL(Size, sizeof(L"StringUnicode"));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidStrUni);
  UT_ASSERT_MEM_EQUAL(DataPtr, L"StringUnicode", sizeof(L"StringUnicode"));

  Size = sizeof(BufAll0);
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0, &Size, BufAll0);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll0));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll0);
  UT_ASSERT_MEM_EQUAL(DataPtr, BufAll0, sizeof(BufAll0));

  Size = sizeof(BufAll1);
  Status = PcdSetExPtrS(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1, &Size, BufAll1);
  UT_ASSERT_NOT_EFI_ERROR(Status);
  Size = PcdGetExSize(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1);
  UT_ASSERT_EQUAL(Size, sizeof(BufAll1));

  DataPtr = PcdGetExPtr(&gTestCasePkgTokenSpaceGuid, PcdTestDynamicExVoidBufAll1);
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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"Dynamic PCD Test Suite", L"Common.PCD.Dynamic", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for Dynamic PCD Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test Dynamic PCD Get", L"Common.PCD.Dynamic.DynamicGet", TestDynamicPcdDynamicGet, TestDynamicPcdDynamicPreReq, NULL, NULL);
  AddTestCase(TestSuite, L"Test Dynamic PCD Set", L"Common.PCD.Dynamic.DynamicSet", TestDynamicPcdDynamicSet, TestDynamicPcdDynamicPreReq, NULL, NULL);
  AddTestCase(TestSuite, L"Test DynamicEx PCD Get", L"Common.PCD.Dynamic.DynamicExGet", TestDynamicPcdDynamicExGet, TestDynamicPcdDynamicExPreReq, NULL, NULL);
  AddTestCase(TestSuite, L"Test DynamicEx PCD Set", L"Common.PCD.Dynamic.DynamicExSet", TestDynamicPcdDynamicExSet, TestDynamicPcdDynamicExPreReq, NULL, NULL);

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
