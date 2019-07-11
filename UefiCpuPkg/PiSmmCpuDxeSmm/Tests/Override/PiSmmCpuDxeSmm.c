/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PiSmmCpuDxeSmm.h"

BOOLEAN                             mXdSupported = TRUE;
UINT64                              mAddressEncMask = 0;

VOID  *gGlobalPageTableAllocatedMem;
VOID  *gGlobalPageTableAlignedMem;
UINTN gGlobalPageTableAlignedMemSize;

VOID *
AllocatePageTableMemory (
  IN UINTN           Pages
  )
{
  //DEBUG ((DEBUG_ERROR, "AllocatePageTableMemory(%x)\n", Pages));

  if (gGlobalPageTableAllocatedMem == NULL) {
    gGlobalPageTableAllocatedMem = AllocatePool (SIZE_8MB + SIZE_4KB);
    ASSERT (gGlobalPageTableAllocatedMem != NULL);
    gGlobalPageTableAlignedMem = (VOID *)(((UINTN)gGlobalPageTableAllocatedMem + SIZE_4KB - 1) & ~(SIZE_4KB - 1));
    gGlobalPageTableAlignedMemSize = SIZE_8MB;
    DEBUG ((DEBUG_ERROR, "gGlobalPageTableAlignedMem(0x%lx)\n", (UINT64)(UINTN)gGlobalPageTableAlignedMem));
  }

  if (EFI_PAGES_TO_SIZE(Pages) > gGlobalPageTableAlignedMemSize) {
    DEBUG ((DEBUG_ERROR, "Out of resource\n"));
    ASSERT (FALSE);
    return NULL;
  }

  gGlobalPageTableAlignedMemSize -= EFI_PAGES_TO_SIZE(Pages);
  //DEBUG ((DEBUG_ERROR, "-- Addr (%lx)\n", (UINT64)((UINTN)gGlobalPageTableAlignedMem + gGlobalPageTableAlignedMemSize)));
  return (VOID *)((UINTN)gGlobalPageTableAlignedMem + gGlobalPageTableAlignedMemSize);
}

VOID
FreePageTableMemory (
  VOID
  )
{
  FreePool (gGlobalPageTableAllocatedMem);
  gGlobalPageTableAllocatedMem = NULL;
}
