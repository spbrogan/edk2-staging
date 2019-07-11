/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Uefi.h>
#include <IndustryStandard/Mbr.h>
#include <Uefi/UefiGpt.h>
#include <Guid/Gpt.h>
#include "FatLitePeim.h"

#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#define TOTAL_SIZE   (512 * 1024)
#define BLOCK_SIZE   (512)
#define IO_ALIGN     (1)

#define UNIT_TEST_NAME        L"PeiGpt Unit Test Application"
#define UNIT_TEST_VERSION     L"0.1"
#define MAX_STRING_SIZE  1025


/**
Simple test.
**/
BOOLEAN
FatFindGptPartitions(
	IN  PEI_FAT_PRIVATE_DATA *PrivateData,
	IN  UINTN                ParentBlockDevNo
	);

VOID
FixBuffer (
	UINT8                   *TestBuffer,
	UINTN                   BufferSize
	)
{
	EFI_PARTITION_TABLE_HEADER   *PrimaryHeader;
	EFI_PARTITION_TABLE_HEADER   *BackupHeader;
	UINT32                       BlockSize;
	EFI_LBA                      LastBlock;
	UINT8                        *Ptr;

	PrimaryHeader = NULL;
	BackupHeader = NULL;

	BlockSize = BLOCK_SIZE;
	LastBlock = (BufferSize + BlockSize - 1) / BlockSize - 1;

	PrimaryHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize));
	if (MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize) + MultU64x32(PrimaryHeader->NumberOfPartitionEntries, PrimaryHeader->SizeOfPartitionEntry) < BufferSize) {
		Ptr = TestBuffer + MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize);
		PrimaryHeader->PartitionEntryArrayCRC32 = CalculateCrc32(Ptr, (UINTN)MultU64x32(PrimaryHeader->NumberOfPartitionEntries, PrimaryHeader->SizeOfPartitionEntry));
	}

	if (PrimaryHeader->Header.HeaderSize + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize) < BufferSize) {
		PrimaryHeader->Header.CRC32 = 0;
		PrimaryHeader->Header.CRC32 = CalculateCrc32(PrimaryHeader, PrimaryHeader->Header.HeaderSize);
	}


	BackupHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(LastBlock, BlockSize));
	if (MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize) + MultU64x32(BackupHeader->NumberOfPartitionEntries, BackupHeader->SizeOfPartitionEntry) < BufferSize) {
		Ptr = TestBuffer + MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize);
		BackupHeader->PartitionEntryArrayCRC32 = CalculateCrc32(Ptr, (UINTN)MultU64x32(BackupHeader->NumberOfPartitionEntries, BackupHeader->SizeOfPartitionEntry));
	}

	if (BackupHeader->Header.HeaderSize + MultU64x32(LastBlock, BlockSize) < BufferSize) {
		BackupHeader->Header.CRC32 = 0;
		BackupHeader->Header.CRC32 = CalculateCrc32(BackupHeader, BackupHeader->Header.HeaderSize);
	}
}

VOID
FixBufferNotFixCRC (
	UINT8                   *TestBuffer,
	UINTN                   BufferSize
	)
{
	EFI_PARTITION_TABLE_HEADER   *PrimaryHeader;
	EFI_PARTITION_TABLE_HEADER   *BackupHeader;
	UINT32                       BlockSize;
	EFI_LBA                      LastBlock;
	UINT8                        *Ptr;

	PrimaryHeader = NULL;
	BackupHeader = NULL;

	BlockSize = BLOCK_SIZE;
	LastBlock = (BufferSize + BlockSize - 1) / BlockSize - 1;

	PrimaryHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize));
	if (MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize) + MultU64x32(PrimaryHeader->NumberOfPartitionEntries, PrimaryHeader->SizeOfPartitionEntry) < BufferSize) {
		Ptr = TestBuffer + MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize);
	}

	BackupHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(LastBlock, BlockSize));
	if (MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize) + MultU64x32(BackupHeader->NumberOfPartitionEntries, BackupHeader->SizeOfPartitionEntry) < BufferSize) {
		Ptr = TestBuffer + MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize);
	}
}


EFI_STATUS
CreatePrivateData(
	UINT8                   *TestBuffer,
	UINTN                   BufferSize,
	UINTN                   ParentBlockDevNo,
	PEI_FAT_PRIVATE_DATA    **PrivateData
	)
{
	PEI_FAT_PRIVATE_DATA    *TestPrivateData;
	TestPrivateData = malloc (sizeof(PEI_FAT_PRIVATE_DATA));
	if (TestPrivateData == NULL) {
		return EFI_OUT_OF_RESOURCES;
	}
	memset (TestPrivateData, 0, sizeof(PEI_FAT_PRIVATE_DATA));

	TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize = BLOCK_SIZE;
	TestPrivateData->BlockDevice[ParentBlockDevNo].LastBlock = (BufferSize + TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize - 1) / TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize - 1;
	TestPrivateData->BlockDevice[ParentBlockDevNo].StartingPos = (UINTN)TestBuffer;
	TestPrivateData->BlockDevice[ParentBlockDevNo].ParentDevNo = ParentBlockDevNo;
	TestPrivateData->BlockDeviceCount = 1;

	*PrivateData = TestPrivateData;
	return EFI_SUCCESS;
}

VOID
ReadSeedBuffer (
	IN CHAR8 *FileName,
	IN VOID  *Buffer,
	IN UINTN MaxBufferSize,
	OUT UINTN *BufferSize
	)
{
	FILE *f = fopen(FileName, "rb");
	if (f==NULL) {
		fputs ("File error",stderr);
		exit (1);
	}
	fseek(f, 0, SEEK_END);

	UINTN fsize = ftell(f);
	rewind(f);

	fsize = fsize > MaxBufferSize ? MaxBufferSize : fsize;
	size_t bytes_read = fread((void *)Buffer, 1, fsize, f);
	if ((UINTN)bytes_read!=fsize) {
		fputs ("File error",stderr);
		exit (1);
	}
	fclose(f);
	if (BufferSize != NULL) {
		*BufferSize = fsize;
	}
}

/**
Using Normal Gpt.bin file,return should be True.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsNormal(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);
	
	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/Gpt.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(1, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;
}

/**
Modify Primary Header break up,leave Backup Header unchange,return should be True.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsPrimaryBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptPrimaryBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(1, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify Primary Header and Backup Header break up,return should be Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsPrimaryBackupBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptPrimaryBackupBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify BlockNo break up,return should be Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsBlockNo(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptBlockNo.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBufferNotFixCRC (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify the PartEntryArrayCRC value ,return should be Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsPartEntryArrayCRC(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptPartEntryArrayCRC.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBufferNotFixCRC (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify the PartitionsCRC value ,return should be Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsCRC(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptCRC.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBufferNotFixCRC (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Change the SizeOfPartitionEntry less than 128, check should be Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsSizeOfPartitionEntry(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptSizeOfPartEntry.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify the header version is not 0x00010000,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsRevisionBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptRevisionBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify the HeaderSize is less than 92,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsHeaderSizeBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptHeaderSizeBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify Reserved value is not 0,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsReservedBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptReservedBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify MyLBA value,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsMyLBABreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptMyLBABreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify AlternateLBA value,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsAlternateLBABreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptAlternateLBABreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify FirstUsableLBA value,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsFirstUsableLBABreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptFirstUsableLBABreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify LastUsableLBA value,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsLastUsableLBABreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptLastUsableLBABreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify DiskGuid value,the return should True
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsDiskGUIDBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptDiskGuidBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(1, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify PartitionEntryLBA value,the return should Fail
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsPartitionEntryLBABreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptPartitionEntryLBABreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify Last Reserved value,the return should be True,No crash.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsLastReservedBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptLastReservedBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(1, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Modify the Buffersize of PartitionEntry,the return should Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsPartitionEntrySizeOverflow(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
  {
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptPartitionEntrySizeOverflow.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(0, Result);

	free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Set the Backup Header signature not "EFI PART",return should be True.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsBackupBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
	PEI_FAT_PRIVATE_DATA *PrivateData;
	UINTN                ParentBlockDevNo;
	BOOLEAN              Result;
	VOID                 *TestBuffer;
	UINTN                TestBufferSize;

	//
	// If the result of addition doesn't overflow MAX_UINT64, then it's addition
	//
	TestBuffer = malloc (TOTAL_SIZE);
	if (TestBuffer == NULL) {
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	}
	memset (TestBuffer, 0, TOTAL_SIZE);

	ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptBackupBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
	FixBuffer (TestBuffer, TestBufferSize);

	ParentBlockDevNo = 0;
	PrivateData = NULL;
	if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
		return UNIT_TEST_ERROR_PREREQ_NOT_MET;
	Result = 0;
	Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
	UT_ASSERT_EQUAL(1, Result);

	free(TestBuffer);
    return UNIT_TEST_PASSED;
}

/**
Modify the value of BlockDeviceCount,the return should not be hang or crashed.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsBlockDeviceCountOverflow(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
    PEI_FAT_PRIVATE_DATA *PrivateData;
    UINTN                ParentBlockDevNo;
    BOOLEAN              Result;
    VOID                 *TestBuffer;
    UINTN                TestBufferSize;

    //
    // If the result of addition doesn't overflow MAX_UINT64, then it's addition
    //
    TestBuffer = malloc (TOTAL_SIZE);
    if (TestBuffer == NULL) {
        return UNIT_TEST_ERROR_PREREQ_NOT_MET;
    }
    memset (TestBuffer, 0, TOTAL_SIZE);

    ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/Gpt.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
    FixBuffer (TestBuffer, TestBufferSize);

    ParentBlockDevNo = 0;
    PrivateData = NULL;
    if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
        return UNIT_TEST_ERROR_PREREQ_NOT_MET;
    PrivateData->BlockDeviceCount = 65;
    Result = 0;
    Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
    UT_ASSERT_EQUAL(0, Result);

    free(TestBuffer);
	return UNIT_TEST_PASSED;

}

/**
Break the Protective MBR Header,the return should be Fail.
**/
UNIT_TEST_STATUS
EFIAPI
TestFatFindGptPartitionsMBRBreak(
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
    PEI_FAT_PRIVATE_DATA *PrivateData;
    UINTN                ParentBlockDevNo;
    BOOLEAN              Result;
    VOID                 *TestBuffer;
    UINTN                TestBufferSize;

    //
    // If the result of addition doesn't overflow MAX_UINT64, then it's addition
    //
    TestBuffer = malloc (TOTAL_SIZE);
    if (TestBuffer == NULL) {
        return UNIT_TEST_ERROR_PREREQ_NOT_MET;
    }
    memset (TestBuffer, 0, TOTAL_SIZE);

    ReadSeedBuffer ("../../../../UefiHostUnitTestCasePkg/TestCase/FatPkg/FatPei/GptTestBin/GptMBRBreak.bin", TestBuffer, TOTAL_SIZE, &TestBufferSize);
    FixBuffer (TestBuffer, TestBufferSize);

    ParentBlockDevNo = 0;
    PrivateData = NULL;
    if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, ParentBlockDevNo, &PrivateData)))
        return UNIT_TEST_ERROR_PREREQ_NOT_MET;
    Result = 0;
    Result = FatFindGptPartitions(PrivateData, ParentBlockDevNo);
    UT_ASSERT_EQUAL(1, Result);

    free(TestBuffer);
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

  Status = CreateUnitTestSuite (&TestSuite, Fw, L"FatPei GPT Test Suite", L"Common.FatPei.Gpt", NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Failed in CreateUnitTestSuite for FatPei Gpt Test Suite\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase(TestSuite, L"Test Normal", L"Common.FatPei.Gpt.Normal", TestFatFindGptPartitionsNormal, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PrimaryBreak", L"Common.FatPei.PrimaryBreak", TestFatFindGptPartitionsPrimaryBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PrimaryBackupBreak", L"Common.FatPei.PrimaryBackupBreak", TestFatFindGptPartitionsPrimaryBackupBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test BackupBreak", L"Common.FatPei.BackupBreak", TestFatFindGptPartitionsBackupBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test RevisionBreak", L"Common.FatPei.RevisionBreak", TestFatFindGptPartitionsRevisionBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test HeaderSizeBreak", L"Common.FatPei.HeaderSizeBreak", TestFatFindGptPartitionsHeaderSizeBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test ReservedBreak", L"Common.FatPei.ReservedBreak", TestFatFindGptPartitionsReservedBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test MyLBABreak", L"Common.FatPei.MyLBABreak", TestFatFindGptPartitionsMyLBABreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test AlternateLBABreak", L"Common.FatPei.AlternateLBABreak", TestFatFindGptPartitionsAlternateLBABreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test FirstUsableLBABreak", L"Common.FatPei.FirstUsableLBABreak", TestFatFindGptPartitionsFirstUsableLBABreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test LastUsableLBABreak", L"Common.FatPei.LastUsableLBABreak", TestFatFindGptPartitionsLastUsableLBABreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test DiskGUIDBreak", L"Common.FatPei.DiskGUIDBreak", TestFatFindGptPartitionsDiskGUIDBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PartitionEntryLBABreak", L"Common.FatPei.PartitionEntryLBABreak", TestFatFindGptPartitionsPartitionEntryLBABreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PartEntryArrayCRC", L"Common.FatPei.PartEntryArrayCRC", TestFatFindGptPartitionsPartEntryArrayCRC, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test BlockNo", L"Common.FatPei.BlockNo", TestFatFindGptPartitionsBlockNo, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test CRC", L"Common.FatPei.CRC", TestFatFindGptPartitionsCRC, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test SizeOfPartitionEntry", L"Common.FatPei.SizeOfPartitionEntry", TestFatFindGptPartitionsSizeOfPartitionEntry, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test LastReservedBreak", L"Common.FatPei.LastReservedBreak", TestFatFindGptPartitionsLastReservedBreak, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test PartitionEntrySizeOverflow", L"Common.FatPei.PartitionEntrySizeOverflow", TestFatFindGptPartitionsPartitionEntrySizeOverflow, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test BlockDeviceCountOverflow", L"Common.FatPei.BlockDeviceCountOverflow", TestFatFindGptPartitionsBlockDeviceCountOverflow, NULL, NULL, NULL);
  AddTestCase(TestSuite, L"Test MBRBreak", L"Common.FatPei.MBRBreak", TestFatFindGptPartitionsMBRBreak, NULL, NULL, NULL);

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
