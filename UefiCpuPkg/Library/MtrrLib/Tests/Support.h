/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MTRR_SUPPORT_H_
#define _MTRR_SUPPORT_H_

#include <Uefi.h>
#include <Base.h>
#include <Register/ArchitecturalMsr.h>
#include <Register/Cpuid.h>
#include <Register/Msr.h>
#include <Library/MtrrLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MtrrStubLib.h>

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define TestIteration  10

#define TEST_VariableMtrrCount  10
#define TEST_NumberOfReservedVariableMtrrs 0
#define NotSpecifiedWithCurrentMtrrSetting 8
#define SCRATCH_BUFFER_SIZE           (4 * SIZE_4KB)

#define TEST_PhysicalAddressBits 36
extern MTRR_MEMORY_CACHE_TYPE TEST_DefaultCacheType;

extern MTRR_MEMORY_RANGE *mActualRanges;
extern UINT32 mActualRangesCount;
extern UINT32 mActualMtrrsCount;

extern MTRR_VARIABLE_SETTING *mTestMtrrSetting;
extern MTRR_MEMORY_RANGE *mRawMtrrRanges;
extern MTRR_MEMORY_RANGE *mExpectedEffectiveMemoryRanges;
extern UINT32 mExpectedEffectiveMtrrMemoryRangesCount;
extern UINT32 mExpectedVariableMtrrUsageCount;

extern UINT32 MtrrCacheTypeValues[5];

VOID
GenerateValidAndConfigurableMtrrPairs (
  IN OUT MTRR_VARIABLE_SETTING **VariableMtrrSettings,
  IN OUT MTRR_MEMORY_RANGE **RawMtrrMemoryRanges,
  IN UINT32 UcCount,
  IN UINT32 WtCount,
  IN UINT32 WbCount,
  IN UINT32 WpCount,
  IN UINT32 WcCount
  );

VOID
GenerateInvalidMemoryLayout (
  IN OUT MTRR_MEMORY_RANGE **EffectiveMemoryRanges,
  IN UINT32 RangesCount
  );

VOID
GetEffectiveMemoryRanges (
  IN MTRR_MEMORY_RANGE *RawMtrrMemoryRanges,
  IN UINT32 RawMtrrMemoryRangeCount,
  OUT MTRR_MEMORY_RANGE **EffectiveMtrrMemoryRanges,
  OUT UINT32 *EffectiveMtrrMemoryRangeCount
  );

VOID
DumpMtrrOrRanges (
  IN MTRR_VARIABLE_SETTING *MtrrPairs,
  IN UINT32 TotalMtrrCount,
  IN MTRR_MEMORY_RANGE *Ranges,
  IN UINT32 TotalRangesCount
  );

VOID
DumpAllRangePiecesEndPoints (
  IN UINT64 *AllRangePiecesEndPoints,
  IN UINT32 AllRangePiecesEndPointCount
  );

VOID
CollectRawMtrrRangesEndpointsAndSortAndRemoveDuplicates (
  IN UINT64 *AllEndPointsInclusive,
  IN UINT32 *pAllEndPointsCount,
  IN MTRR_MEMORY_RANGE *RawMtrrMemoryRanges,
  IN UINT32 RawMtrrMemoryRangesCount
  );

VOID
DumpEffectiveRangesCArrayStyle (
  IN MTRR_MEMORY_RANGE *EffectiveRanges,
  IN UINT32 EffectiveRangesCount
  );

VOID
DumpTestInput (
  VOID
  );


INT32
CompareFuncUint64 (
  const void * a,
  const void * b
  );

INT32
CompareFuncInt32 (
  const void * a,
  const void * b
  );

VOID
CollectTestResult (
  VOID
  );

#endif
