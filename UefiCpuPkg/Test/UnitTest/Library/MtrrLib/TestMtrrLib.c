/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <string.h>

#include <malloc.h>
#include <time.h>
#include <stdlib.h>

#include "Support.h"

#define UNIT_TEST_NAME        L"MtrrLib Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

#define MAX_STRING_SIZE  1025

extern CHAR8 *CacheTypeFullNames[9];

UINT64 mFailureCount_OneByOne = 0;
UINT64 mFailureCount_AllAtOnce = 0;

MTRR_VARIABLE_SETTING *mTestMtrrSetting = NULL;
MTRR_MEMORY_RANGE *mRawMtrrRanges = NULL;
MTRR_MEMORY_RANGE *mExpectedEffectiveMemoryRanges = NULL;
UINT32 mExpectedEffectiveMtrrMemoryRangesCount = 0;
UINT32 mExpectedVariableMtrrUsageCount = 0 ;

BOOLEAN
VerifyResult (
  IN MTRR_MEMORY_RANGE *ExpectedEffectiveMemoryRanges,
  IN UINT32 ExpectedEffectiveMtrrMemoryRangeCount,
  IN UINT32 ExpectedVariableMtrrUsageCount,
  IN MTRR_MEMORY_RANGE *ActualRanges,
  IN UINT32 ActualRangesCount,
  IN UINT32 ActualMtrrsCount
  )
{
    UINT32 i;

    BOOLEAN PassSoFar = TRUE;


    // Verify effective ranges count.
    if (ExpectedEffectiveMtrrMemoryRangeCount != ActualRangesCount)
    {
        DEBUG((DEBUG_ERROR, "[Fail]: Ranges Count Not Match!\n"));
        DEBUG((DEBUG_ERROR, "Expected: [%02d] Actual: [%02d]\n", ExpectedEffectiveMtrrMemoryRangeCount, ActualRangesCount));
        PassSoFar = FALSE;
    }
    else
    {
        // Verify each effective range
        for (i = 0; i < ActualRangesCount; i++)
        {
            MTRR_MEMORY_RANGE Expected = ExpectedEffectiveMemoryRanges[i];
            MTRR_MEMORY_RANGE Actual = ActualRanges[i];
            if (!(Expected.BaseAddress == Actual.BaseAddress && Expected.Length == Actual.Length && Expected.Type == Actual.Type))
            {
                DEBUG((DEBUG_ERROR, "[Fail]: Range %02d Not Match!\n", i));
                PassSoFar = FALSE;
            }
        }
    }

    if (PassSoFar)
    {
        // Verify variable MTRR usage
        if (ExpectedVariableMtrrUsageCount < ActualMtrrsCount)
        {
            DEBUG((DEBUG_ERROR, "[Fail]: Worse Usage!\n"));
            DEBUG((DEBUG_ERROR, "Expected: [%02d] Actual: [%02d]\n", ExpectedVariableMtrrUsageCount, ActualMtrrsCount));
            PassSoFar = FALSE;
        }
        else if (ExpectedVariableMtrrUsageCount == ActualMtrrsCount)
        {
            DEBUG((DEBUG_ERROR, "[Pass]: No Better\n"));
            //DEBUG((DEBUG_ERROR, "Expected: [%02d] Actual: [%02d]\n", ExpectedVariableMtrrUsageCount, ActualMtrrsCount));
        }
        else
        {
            DEBUG((DEBUG_ERROR, "[Pass]\n"));
            DEBUG((DEBUG_ERROR, "Actual Usage: [%02d]\n", ActualMtrrsCount));
        }
    }

    return PassSoFar;
}

VOID
DumpTestInput (
  VOID
  )
{
    // Dump MTRR
    DEBUG((DEBUG_ERROR, "---- Test MTRR Settings ----\n"));
    DumpMtrrOrRanges(mTestMtrrSetting, mExpectedVariableMtrrUsageCount, NULL, 0);

    // Dump raw MTRR ranges
    DEBUG((DEBUG_ERROR, "---- Raw MTRR Ranges ----\n"));
    DumpMtrrOrRanges(NULL, 0, mRawMtrrRanges, mExpectedVariableMtrrUsageCount);

    
    DEBUG((DEBUG_ERROR, "---- Raw MTRR Range Endpoints Sorted ----\n"));
    UINT32 AllEndPointsCount = mExpectedVariableMtrrUsageCount << 1;
    UINT64 *AllEndPointsInclusive = calloc(AllEndPointsCount, sizeof(UINT64));
    CollectRawMtrrRangesEndpointsAndSortAndRemoveDuplicates(AllEndPointsInclusive, &AllEndPointsCount, mRawMtrrRanges, mExpectedVariableMtrrUsageCount);
    DumpAllRangePiecesEndPoints(AllEndPointsInclusive, AllEndPointsCount);
    free(AllEndPointsInclusive);

    // Dump effective ranges in C-Array style
    DEBUG((DEBUG_ERROR, "---- Effective Ranges ----\n"));
    DumpMtrrOrRanges(NULL, 0, mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);

    // Dump effective ranges in C-Array style
    DEBUG((DEBUG_ERROR, "---- Effective Ranges in C-Array Style Start----\n"));
    DumpEffectiveRangesCArrayStyle(mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);
    DEBUG((DEBUG_ERROR, "---- Effective Ranges in C-Array Style Finish ----\n"));
}


UNIT_TEST_STATUS
TheRunningManForValidAndConfigurableLayout (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context,
  IN UINT32 UcCount,
  IN UINT32 WtCount,
  IN UINT32 WbCount,
  IN UINT32 WpCount,
  IN UINT32 WcCount
  )
{
    BOOLEAN DEBUG_OVERRIDE = FALSE; // If we provide overriding DebugRanges as below, we need to set this to TRUE.
    mTestMtrrSetting = NULL;
    mRawMtrrRanges = NULL;
    mExpectedEffectiveMemoryRanges = NULL;
    mExpectedEffectiveMtrrMemoryRangesCount = 0;
    mExpectedVariableMtrrUsageCount = UcCount + WtCount + WbCount + WpCount + WcCount;

    GenerateValidAndConfigurableMtrrPairs(&mTestMtrrSetting, &mRawMtrrRanges, UcCount, WtCount, WbCount, WpCount, WcCount); // Total Mtrr count must <= TEST_VariableMtrrCount - TEST_NumberOfReservedVariableMtrrs
    
    GetEffectiveMemoryRanges(mRawMtrrRanges, mExpectedVariableMtrrUsageCount, &mExpectedEffectiveMemoryRanges, &mExpectedEffectiveMtrrMemoryRangesCount);

    BOOLEAN OneByOnePass = FALSE;
    RETURN_STATUS OneByOneStatus = EFI_SUCCESS;

    mActualRanges = NULL;
    mActualRangesCount = 0;
    mActualMtrrsCount = 0;

    UINT32 i = 0;
    InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits); // Must reset the HAL for each iteration!
    for (i = 0; i < mExpectedEffectiveMtrrMemoryRangesCount; i++)
    {
       
        OneByOneStatus = MtrrSetMemoryAttribute(mExpectedEffectiveMemoryRanges[i].BaseAddress, mExpectedEffectiveMemoryRanges[i].Length, mExpectedEffectiveMemoryRanges[i].Type);
        free(mActualRanges);
        mActualRanges = NULL;
        if (OneByOneStatus != EFI_SUCCESS)
        {
            break;
        }
    }

    free(mActualRanges);
    mActualRanges = NULL;

    // Verify the result of MtrrSetMemoryAttribute()
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttribute(): "));
    if (OneByOneStatus == EFI_SUCCESS)
    {
        MtrrDebugPrintAllMtrrs();
        CollectTestResult ();
        
        MTRR_MEMORY_RANGE *ActualRanges_MtrrSetMemoryAttribute = mActualRanges;
        UINT32 ActualRangesCount_MtrrSetMemoryAttribute = mActualRangesCount;
        UINT32 ActualMtrrsCount_MtrrSetMemoryAttribute = mActualMtrrsCount;
        OneByOnePass = VerifyResult(mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount, mExpectedVariableMtrrUsageCount,
            ActualRanges_MtrrSetMemoryAttribute, ActualRangesCount_MtrrSetMemoryAttribute, ActualMtrrsCount_MtrrSetMemoryAttribute);
    }
    else
    {
        DEBUG((DEBUG_ERROR, "[Fail]: Status = %08x\n", OneByOneStatus));
        DEBUG((DEBUG_ERROR, "Failed to set effective memory range of index: [%02d]\n", i));
    }
    free(mActualRanges);

    // Test MtrrSetMemoryAttributesInMtrrSettings() by setting expected ranges All At Once.
    BOOLEAN AllAtOncePass = FALSE;
    RETURN_STATUS AllAtOnceStatus = EFI_SUCCESS;
    mActualRanges = NULL;
    mActualRangesCount = 0;
    mActualMtrrsCount = 0;

    InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits);

    UINT8 *Scratch;
    UINTN ScratchSize = SCRATCH_BUFFER_SIZE;
    Scratch = calloc(ScratchSize, sizeof(UINT8));
    AllAtOnceStatus = MtrrSetMemoryAttributesInMtrrSettings(NULL, Scratch, &ScratchSize, mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);
    if (!RETURN_ERROR (AllAtOnceStatus)) {
      CollectTestResult ();
    }
    free(mActualRanges); 
    mActualRanges = NULL;

    if (AllAtOnceStatus == RETURN_BUFFER_TOO_SMALL)
    {
        Scratch = realloc(Scratch, ScratchSize);
        AllAtOnceStatus = MtrrSetMemoryAttributesInMtrrSettings(NULL, Scratch, &ScratchSize, mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);
        if (!RETURN_ERROR (AllAtOnceStatus)) {
          CollectTestResult ();
        }
        free(mActualRanges);
        mActualRanges = NULL;
    }
    free(Scratch);



    // Verify result of MtrrSetMemoryAttributesInMtrrSettings()
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttributesInMtrrSettings(): "));
    if (AllAtOnceStatus == EFI_SUCCESS)
    {
        MtrrDebugPrintAllMtrrs();
        CollectTestResult ();
        
        MTRR_MEMORY_RANGE *ActualRanges_SetMemoryAttributesInMtrrSettings = mActualRanges;
        UINT32 ActualRangesCount_SetMemoryAttributesInMtrrSettings = mActualRangesCount;
        UINT32 ActualMtrrsCount_SetMemoryAttributesInMtrrSettings = mActualMtrrsCount;
        AllAtOncePass = VerifyResult(mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount, mExpectedVariableMtrrUsageCount,
            ActualRanges_SetMemoryAttributesInMtrrSettings, ActualRangesCount_SetMemoryAttributesInMtrrSettings, ActualMtrrsCount_SetMemoryAttributesInMtrrSettings);
    }
    else
    {
        DEBUG((DEBUG_ERROR, "[Fail]: Status = %08x\n", AllAtOnceStatus));
    }
    free(mActualRanges); 
    mActualRanges = NULL;

    if (OneByOnePass && AllAtOncePass)
    {
        //DEBUG((DEBUG_ERROR, "Double Pass!\n"));
    }
    else
    {
        //DumpMtrrAndRawRanges(mTestMtrrSetting, mExpectedEffectiveMemoryRanges, mExpectedVariableMtrrUsageCount);
        DumpTestInput();

        if (!OneByOnePass && !AllAtOncePass)
        {
            DEBUG((DEBUG_ERROR, "Double Failure!\n"));
            mFailureCount_OneByOne++;
            mFailureCount_AllAtOnce++;
        }
        else
        {
            DEBUG((DEBUG_ERROR, "[Fail]: Single Fail!\n"));
            if (!OneByOnePass)
            {
                DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttribute() : %a\n", "FAIL"));
                DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttributesInMtrrSettings() : %a\n", "PASS"));
                mFailureCount_OneByOne++;
            }
            if (!AllAtOncePass)
            {
                DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttribute() : %a\n", "PASS"));
                DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttributesInMtrrSettings() : %a\n", "FAIL"));
                mFailureCount_AllAtOnce++;
            }
        }
    }

    // clean up
    free(mTestMtrrSetting);
    if (!DEBUG_OVERRIDE)
        free(mRawMtrrRanges);
    free(mExpectedEffectiveMemoryRanges);

    UT_ASSERT_TRUE(OneByOnePass);
    UT_ASSERT_TRUE(AllAtOncePass);

    return UNIT_TEST_PASSED;
}

VOID
GenerateMemoryTypeCombination (
  OUT UINT32 *UcCount,
  OUT UINT32 *WtCount,
  OUT UINT32 *WbCount,
  OUT UINT32 *WpCount,
  OUT UINT32 *WcCount
  )
{
    UINT32 MaxMtrrTypes = 5;
    UINT32 TotalMtrrCountMax = GetFirmwareVariableMtrrCount();
    UINT32 TotalMtrrCountToUse = (rand() % TotalMtrrCountMax) + 1;

    UINT32 TypeBucket[5] = { 0, 0, 0, 0, 0 };
    UINT32 i;
    for (i = 0; i < TotalMtrrCountToUse; i++)
    {
        UINT32 BucketIndex = rand() % MaxMtrrTypes;
        TypeBucket[BucketIndex]++;
    }

    *UcCount= TypeBucket[0];
    *WtCount= TypeBucket[1];
    *WbCount= TypeBucket[2];
    *WpCount= TypeBucket[3];
    *WcCount= TypeBucket[4];
}


CHAR8 mAnimation[4] = { '-', '\\', '|', '/' };

UNIT_TEST_STATUS
EFIAPI
TestGeneratorForValidAndConfigurableMemoryLayouts (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
    UINT32 UcCount;
    UINT32 WtCount;
    UINT32 WbCount;
    UINT32 WpCount;
    UINT32 WcCount;
    UINT32 i = 0;
    DEBUG((DEBUG_ERROR, "Test for valid and configurable layouts started.\n\n"));
    mFailureCount_OneByOne = 0;
    mFailureCount_AllAtOnce = 0;
    UINT32 AnimationCount = 0;
    while (i < TestIteration)
    {
        DEBUG((DEBUG_ERROR, "[Iteration %02d]\n", i + 1));
        DEBUG((DEBUG_INFO, "\r[#%10d/%d] :  ", i+1, TestIteration));
        if (i % 200 == 0)
        {
            AnimationCount++;
            DEBUG((DEBUG_INFO, "%c  ", mAnimation[AnimationCount % 4]));
        }
        
        // Default cache type is randomized for each iteration. And each iteration will test 2 MtrrLib APIs.
        TEST_DefaultCacheType = (MTRR_MEMORY_CACHE_TYPE)MtrrCacheTypeValues[rand() % 5];
        DEBUG((DEBUG_ERROR, "Default cache type: %a\n", CacheTypeFullNames[TEST_DefaultCacheType]));

        InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits);
        GenerateMemoryTypeCombination(&UcCount, &WtCount, &WbCount, &WpCount, &WcCount);
        DEBUG((DEBUG_ERROR, "Expected Total Usage: %d\n", UcCount + WtCount + WbCount + WpCount + WcCount));
        DEBUG((DEBUG_ERROR, "UC=%02d, WT=%02d, WB=%02d, WP=%02d, WC=%02d\n", UcCount, WtCount, WbCount, WpCount, WcCount));
        TheRunningManForValidAndConfigurableLayout(Framework, Context, UcCount, WtCount, WbCount, WpCount, WcCount);
        i++;
        DEBUG((DEBUG_ERROR, "%03lld | %03lld\n\n", mFailureCount_OneByOne, mFailureCount_AllAtOnce)); // In case the test crash, we can save some statistics so far.
    }
    DEBUG((DEBUG_ERROR, "\n"));
    DEBUG((DEBUG_ERROR, "=========================== Failure Statistics ===========================\n"));
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttribute()                 =   %03lld\n", mFailureCount_OneByOne));
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttributesInMtrrSettings()  =   %03lld\n", mFailureCount_AllAtOnce));
    DEBUG((DEBUG_ERROR, "==================================================================\n"));
    DEBUG((DEBUG_ERROR, "\nTest finished.\n"));

    return UNIT_TEST_PASSED;
}

BOOLEAN
IsValidLayout (
  IN MTRR_MEMORY_RANGE *EffectiveMemoryRanges,
  IN UINT32 RangesCount
  )
{
    return FALSE;
}

UNIT_TEST_STATUS
TheRunningManForInvalidLayout (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
    UINT32 TotalMtrrCountMax = GetFirmwareVariableMtrrCount();
    UINT32 RangesCount = (rand() % (TotalMtrrCountMax << 2)) + 1;
    mExpectedEffectiveMtrrMemoryRangesCount = RangesCount;

    GenerateInvalidMemoryLayout(&mExpectedEffectiveMemoryRanges, RangesCount);
    while (IsValidLayout(mExpectedEffectiveMemoryRanges, RangesCount))
    {
        GenerateInvalidMemoryLayout(&mExpectedEffectiveMemoryRanges, RangesCount);
    }

    //MtrrSetMemoryAttribute() test
    UINT32 i = 0;
    RETURN_STATUS OneByOneStatus = EFI_SUCCESS;
    InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits);
    for (i = 0; i < RangesCount; i++)
    {
        OneByOneStatus = MtrrSetMemoryAttribute(mExpectedEffectiveMemoryRanges[i].BaseAddress, mExpectedEffectiveMemoryRanges[i].Length, mExpectedEffectiveMemoryRanges[i].Type);
        free(mActualRanges);
        mActualRanges = NULL;
        if (OneByOneStatus != EFI_SUCCESS)
            break;
    }

    free(mActualRanges);
    mActualRanges = NULL;

    if (OneByOneStatus != EFI_SUCCESS)
    {
        mFailureCount_OneByOne++; // We expect failure here.
        //DumpTestInput();
    }
    else
    {
        DumpTestInput(); // Unexpected success, we need to dump the input.
    }



    //MtrrSetMemoryAttributeMtrrSetMemoryAttributesInMtrrSettings() test
    InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits); // Must reset the HAL for each iteration!

    RETURN_STATUS AllAtOnceStatus = EFI_SUCCESS;
    UINT8 *Scratch;
    UINTN ScratchSize = SCRATCH_BUFFER_SIZE;
    Scratch = calloc(ScratchSize, sizeof(UINT8));
    AllAtOnceStatus = MtrrSetMemoryAttributesInMtrrSettings(NULL, Scratch, &ScratchSize, mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);
    if (!RETURN_ERROR (AllAtOnceStatus)) {
      CollectTestResult ();
    }
    free(mActualRanges);
    mActualRanges = NULL;

    if (AllAtOnceStatus == RETURN_BUFFER_TOO_SMALL)
    {
        Scratch = realloc(Scratch, ScratchSize);
        AllAtOnceStatus = MtrrSetMemoryAttributesInMtrrSettings(NULL, Scratch, &ScratchSize, mExpectedEffectiveMemoryRanges, mExpectedEffectiveMtrrMemoryRangesCount);
        if (!RETURN_ERROR (AllAtOnceStatus)) {
          CollectTestResult ();
        }
        free(mActualRanges);
        mActualRanges = NULL;
    }
    free(Scratch);

    if (AllAtOnceStatus != EFI_SUCCESS)
    {
        mFailureCount_AllAtOnce++;
        //DumpTestInput();
    }
    else
    {
        DumpTestInput(); // Unexpected success, we need to dump the input.
    }
    

    free(mExpectedEffectiveMemoryRanges);

    UT_ASSERT_NOT_EQUAL(OneByOneStatus, EFI_SUCCESS);
    UT_ASSERT_NOT_EQUAL(AllAtOnceStatus, EFI_SUCCESS);

    return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
TestGeneratorForInvalidMemoryLayouts (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
    mTestMtrrSetting = NULL;
    mRawMtrrRanges = NULL;

    mFailureCount_OneByOne = 0;
    mFailureCount_AllAtOnce = 0;

    UINT32 i = 0;
    DEBUG((DEBUG_ERROR, "Test for invalid layouts started.\n\n"));
    UINT32 AnimationCount = 0;
    while (i < TestIteration)
    {
        DEBUG((DEBUG_ERROR, "[Iteration %02d]\n", i + 1));
        DEBUG((DEBUG_INFO, "\r[#%10d/%d] :  ", i + 1, TestIteration));
        if (i % 200 == 0)
        {
            AnimationCount++;
            DEBUG((DEBUG_INFO, "%c  ", mAnimation[AnimationCount % 4]));
        }

        // Default cache type is randomized for each iteration. And each iteration will test 2 MtrrLib APIs.
        TEST_DefaultCacheType = (MTRR_MEMORY_CACHE_TYPE)MtrrCacheTypeValues[rand() % 5];
        DEBUG((DEBUG_ERROR, "Default cache type: %a\n", CacheTypeFullNames[TEST_DefaultCacheType]));
        InitializeMtrrRegs(TEST_DefaultCacheType, TEST_VariableMtrrCount, TEST_PhysicalAddressBits);// This initialization is necessary because we need to initialize the MtrrCount.
        TheRunningManForInvalidLayout(Framework, Context);
        i++;
        DEBUG((DEBUG_ERROR, "%03lld | %03lld\n\n", mFailureCount_OneByOne, mFailureCount_AllAtOnce));
    }


    DEBUG((DEBUG_ERROR, "\n"));
    DEBUG((DEBUG_ERROR, "=========================== Unexpected Success Statistics ===========================\n"));
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttribute()                 =   %03lld\n", TestIteration - mFailureCount_OneByOne));
    DEBUG((DEBUG_ERROR, "MtrrSetMemoryAttributesInMtrrSettings()  =   %03lld\n", TestIteration - mFailureCount_AllAtOnce));
    DEBUG((DEBUG_ERROR, "==================================================================\n"));
    DEBUG((DEBUG_ERROR, "\nTest finished.\n"));
    
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

  srand((unsigned int)time(NULL));

  Fw = NULL;
  TestSuite = NULL;

  AsciiStrToUnicodeStrS (gEfiCallerBaseName, ShortName, sizeof(ShortName)/sizeof(ShortName[0]));
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"MtrrLib Basic Test Suite", L"Common.MtrrLib.Basic", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for MtrrLib Basic Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test ValidAndConfigurableMemoryLayouts", L"Common.MtrrLib.Basic.ValidAndConfigurableMemoryLayouts", TestGeneratorForValidAndConfigurableMemoryLayouts, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test InvalidMemoryLayouts", L"Common.MtrrLib.Basic.InvalidMemoryLayouts", TestGeneratorForInvalidMemoryLayouts, NULL, NULL, NULL);

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
