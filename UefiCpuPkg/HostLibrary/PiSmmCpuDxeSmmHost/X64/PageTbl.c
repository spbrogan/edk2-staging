/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PiSmmCpuDxeSmm.h"

BOOLEAN                             m1GPageTableSupport = TRUE;
BOOLEAN                             mCpuSmmStaticPageTable = TRUE;

UINT8                               mPhysicalAddressBits = 48;
UINT64                              gPhyMask = ((1ull << 48) - 1) & ((1ull << 48) - EFI_PAGE_SIZE);

/**
  Set static page table.

  @param[in] PageTable     Address of page table.
**/
VOID
SetStaticPageTable (
  IN UINTN               PageTable
  )
{
  UINT64                                        PageAddress;
  UINTN                                         NumberOfPml4EntriesNeeded;
  UINTN                                         NumberOfPdpEntriesNeeded;
  UINTN                                         IndexOfPml4Entries;
  UINTN                                         IndexOfPdpEntries;
  UINTN                                         IndexOfPageDirectoryEntries;
  UINT64                                        *PageMapLevel4Entry;
  UINT64                                        *PageMap;
  UINT64                                        *PageDirectoryPointerEntry;
  UINT64                                        *PageDirectory1GEntry;
  UINT64                                        *PageDirectoryEntry;

  if (mPhysicalAddressBits <= 39 ) {
    NumberOfPml4EntriesNeeded = 1;
    NumberOfPdpEntriesNeeded = (UINT32)LShiftU64 (1, (mPhysicalAddressBits - 30));
  } else {
    NumberOfPml4EntriesNeeded = (UINT32)LShiftU64 (1, (mPhysicalAddressBits - 39));
    NumberOfPdpEntriesNeeded = 512;
  }

  //
  // By architecture only one PageMapLevel4 exists - so lets allocate storage for it.
  //
  PageMap         = (VOID *) PageTable;

  PageMapLevel4Entry = PageMap;
  PageAddress        = 0;
  for (IndexOfPml4Entries = 0; IndexOfPml4Entries < NumberOfPml4EntriesNeeded; IndexOfPml4Entries++, PageMapLevel4Entry++) {
    //
    // Each PML4 entry points to a page of Page Directory Pointer entries.
    //
    PageDirectoryPointerEntry = (UINT64 *) ((*PageMapLevel4Entry) & ~mAddressEncMask & gPhyMask);
    if (PageDirectoryPointerEntry == NULL) {
      PageDirectoryPointerEntry = AllocatePageTableMemory (1);
      ASSERT(PageDirectoryPointerEntry != NULL);
      ZeroMem (PageDirectoryPointerEntry, EFI_PAGES_TO_SIZE(1));

      *PageMapLevel4Entry = (UINT64)(UINTN)PageDirectoryPointerEntry | mAddressEncMask | PAGE_ATTRIBUTE_BITS;
    }

    if (m1GPageTableSupport) {
      PageDirectory1GEntry = PageDirectoryPointerEntry;
      for (IndexOfPageDirectoryEntries = 0; IndexOfPageDirectoryEntries < 512; IndexOfPageDirectoryEntries++, PageDirectory1GEntry++, PageAddress += SIZE_1GB) {
        if (IndexOfPml4Entries == 0 && IndexOfPageDirectoryEntries < 4) {
          //
          // Skip the < 4G entries
          //
          continue;
        }
        //
        // Fill in the Page Directory entries
        //
        *PageDirectory1GEntry = PageAddress | mAddressEncMask | IA32_PG_PS | PAGE_ATTRIBUTE_BITS;
      }
    } else {
      PageAddress = BASE_4GB;
      for (IndexOfPdpEntries = 0; IndexOfPdpEntries < NumberOfPdpEntriesNeeded; IndexOfPdpEntries++, PageDirectoryPointerEntry++) {
        if (IndexOfPml4Entries == 0 && IndexOfPdpEntries < 4) {
          //
          // Skip the < 4G entries
          //
          continue;
        }
        //
        // Each Directory Pointer entries points to a page of Page Directory entires.
        // So allocate space for them and fill them in in the IndexOfPageDirectoryEntries loop.
        //
        PageDirectoryEntry = (UINT64 *) ((*PageDirectoryPointerEntry) & ~mAddressEncMask & gPhyMask);
        if (PageDirectoryEntry == NULL) {
          PageDirectoryEntry = AllocatePageTableMemory (1);
          ASSERT(PageDirectoryEntry != NULL);
          ZeroMem (PageDirectoryEntry, EFI_PAGES_TO_SIZE(1));

          //
          // Fill in a Page Directory Pointer Entries
          //
          *PageDirectoryPointerEntry = (UINT64)(UINTN)PageDirectoryEntry | mAddressEncMask | PAGE_ATTRIBUTE_BITS;
        }

        for (IndexOfPageDirectoryEntries = 0; IndexOfPageDirectoryEntries < 512; IndexOfPageDirectoryEntries++, PageDirectoryEntry++, PageAddress += SIZE_2MB) {
          //
          // Fill in the Page Directory entries
          //
          *PageDirectoryEntry = PageAddress | mAddressEncMask | IA32_PG_PS | PAGE_ATTRIBUTE_BITS;
        }
      }
    }
  }
}

/**
  Create PageTable for SMM use.

  @return The address of PML4 (to set CR3).

**/
UINT32
SmmInitPageTable (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS              Pages;
  UINT64                            *PTEntry;

  //
  // Generate PAE page table for the first 4GB memory space
  //
  Pages = Gen4GPageTable (FALSE);

  //
  // Fill Page-Table-Level4 (PML4) entry
  //
  PTEntry = (UINT64*)AllocatePageTableMemory (1);
  ASSERT (PTEntry != NULL);
  *PTEntry = Pages | mAddressEncMask | PAGE_ATTRIBUTE_BITS;
  ZeroMem (PTEntry + 1, EFI_PAGE_SIZE - sizeof (*PTEntry));

  if (mCpuSmmStaticPageTable) {
    SetStaticPageTable ((UINTN)PTEntry);
  }

  //
  // Return the address of PML4 (to set CR3)
  //
  return (UINT32)(UINTN)PTEntry;
}