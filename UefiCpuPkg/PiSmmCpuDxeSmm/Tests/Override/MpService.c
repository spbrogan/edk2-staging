/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PiSmmCpuDxeSmm.h"

EFI_STATUS
EFIAPI
SmmBlockingStartupThisAp (
  IN      EFI_AP_PROCEDURE          Procedure,
  IN      UINTN                     CpuIndex,
  IN OUT  VOID                      *ProcArguments OPTIONAL
  )
{
  return EFI_SUCCESS;
}

/**
  Create 4G PageTable in SMRAM.

  @param[in]      Is32BitPageTable Whether the page table is 32-bit PAE
  @return         PageTable Address

**/
UINTN
Gen4GPageTable (
  IN      BOOLEAN                   Is32BitPageTable
  )
{
  VOID    *PageTable;
  UINTN   Index;
  UINT64  *Pte;
  UINT64  *Pdpte;

  //
  // Allocate the page table
  //
  PageTable = AllocatePageTableMemory (5);
  ASSERT (PageTable != NULL);

  PageTable = (VOID *)((UINTN)PageTable);
  Pte = (UINT64*)PageTable;

  //
  // Zero out all page table entries first
  //
  ZeroMem (Pte, EFI_PAGES_TO_SIZE (1));

  //
  // Set Page Directory Pointers
  //
  for (Index = 0; Index < 4; Index++) {
    Pte[Index] = ((UINTN)PageTable + EFI_PAGE_SIZE * (Index + 1)) | mAddressEncMask |
                   (Is32BitPageTable ? IA32_PAE_PDPTE_ATTRIBUTE_BITS : PAGE_ATTRIBUTE_BITS);
  }
  Pte += EFI_PAGE_SIZE / sizeof (*Pte);

  //
  // Fill in Page Directory Entries
  //
  for (Index = 0; Index < EFI_PAGE_SIZE * 4 / sizeof (*Pte); Index++) {
    Pte[Index] = (Index << 21) | mAddressEncMask | IA32_PG_PS | PAGE_ATTRIBUTE_BITS;
  }

  Pdpte = (UINT64*)PageTable;

  return (UINTN)PageTable;
}
