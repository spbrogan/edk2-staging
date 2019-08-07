/** @file
  Application for Cryptographic Primitives Validation.

Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "TestBaseCryptLib.h"

SUITE_DESC  mSuiteDesc[] = {
    //
    // -----Title--------------------------Package------------Sup---Tdn-------TestNum------------TestDesc
    //
    {L"EKU verify tests",            L"CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs7EkuTestNum,       mPkcs7EkuTest},
    {L"HASH verify tests",           L"CryptoPkg.BaseCryptLib", NULL, NULL, &mHashTestNum,           mHashTest},
    {L"HMAC verify tests",           L"CryptoPkg.BaseCryptLib", NULL, NULL, &mHmacTestNum,           mHmacTest},
    {L"BlockCipher verify tests",    L"CryptoPkg.BaseCryptLib", NULL, NULL, &mBlockCipherTestNum,    mBlockCipherTest},
    {L"RSA verify tests",            L"CryptoPkg.BaseCryptLib", NULL, NULL, &mRsaTestNum,            mRsaTest},
    {L"RSACert verify tests",        L"CryptoPkg.BaseCryptLib", NULL, NULL, &mRsaCertTestNum,        mRsaCertTest},
    {L"PKCS7 verify tests",          L"CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs7TestNum,          mPkcs7Test},
    {L"PKCS5 verify tests",          L"CryptoPkg.BaseCryptLib", NULL, NULL, &mPkcs5TestNum,          mPkcs5Test},
    {L"Authenticode verify tests",   L"CryptoPkg.BaseCryptLib", NULL, NULL, &mAuthenticodeTestNum,   mAuthenticodeTest},
    {L"ImageTimestamp verify tests", L"CryptoPkg.BaseCryptLib", NULL, NULL, &mImageTimestampTestNum, mImageTimestampTest},
    {L"DH verify tests",             L"CryptoPkg.BaseCryptLib", NULL, NULL, &mDhTestNum,             mDhTest},
    {L"PRNG verify tests",           L"CryptoPkg.BaseCryptLib", NULL, NULL, &mPrngTestNum,           mPrngTest},
    {L"OAEP encrypt verify tests",   L"CryptoPkg.BaseCryptLib", NULL, NULL, &mOaepTestNum,           mOaepTest},
};


///================================================================================================
///================================================================================================
///
/// TEST ENGINE
///
///================================================================================================
///================================================================================================

int main ()
{
  UINTN                     SuiteIndex;
  UINTN                     TestIndex;
  EFI_STATUS                Status;
  UNIT_TEST_FRAMEWORK       *Fw = NULL;
  CHAR16                    ShortName[100];

  RandomSeed (NULL, 0);

  ShortName[0] = L'\0';

  UnicodeSPrint (&ShortName[0], sizeof(ShortName), L"%a", gEfiCallerBaseName); 
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    UNIT_TEST_SUITE *Suite = NULL;
    Status = CreateUnitTestSuite (&Suite, Fw, mSuiteDesc[SuiteIndex].Title, mSuiteDesc[SuiteIndex].Package, mSuiteDesc[SuiteIndex].Sup, mSuiteDesc[SuiteIndex].Tdn);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    for (TestIndex = 0; TestIndex < *mSuiteDesc[SuiteIndex].TestNum; TestIndex++) {
      AddTestCase (Suite, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Description, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->ClassName, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Func, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->PreReq, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->CleanUp, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Context);
    }
  }

  Status = RunAllTestSuites (Fw);

EXIT:
  if (Fw != NULL) {
    FreeUnitTestFramework (Fw);
  }

  return Status;
}
