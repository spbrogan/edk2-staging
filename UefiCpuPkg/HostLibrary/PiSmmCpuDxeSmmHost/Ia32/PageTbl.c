/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PiSmmCpuDxeSmm.h"

UINT8                               mPhysicalAddressBits = 32;

/**
  Create PageTable for SMM use.

  @return     PageTable Address

**/
UINT32
SmmInitPageTable (
  VOID
  )
{
  return Gen4GPageTable (TRUE);
}