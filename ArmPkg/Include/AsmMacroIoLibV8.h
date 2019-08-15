/** @file
  Macros to work around lack of Clang support for LDR register, =expr

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Portions copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2016, Linaro Ltd. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef __MACRO_IO_LIBV8_H__
#define __MACRO_IO_LIBV8_H__

// MU_CHANGE BEGIN - Supply VS versions of macros.
// This is gross but GCC doesn't follow the C++ spec and is using a '#' in macro definitions.
#define CATSTR2(x,y) x##y
#define CATSTR(x,y) CATSTR2(x,y)
#define NUM(x) CATSTR(HASH,x)
#define HASH #

// CurrentEL : 0xC = EL3; 8 = EL2; 4 = EL1
// This only selects between EL1 and EL2, else we die.
// Provide the Macro with a safe temp xreg to use.
#if !defined(_MSC_VER)
#define EL1_OR_EL2(SAFE_XREG)        \
        mrs    SAFE_XREG, CurrentEL ;\
        cmp    SAFE_XREG, #0x8      ;\
        b.gt   .                    ;\
        b.eq   2f                   ;\
        cbnz   SAFE_XREG, 1f        ;\
        b      .                    ;// We should never get here
#else
#define EL1_OR_EL2(SAFE_XREG)        \
        mrs    SAFE_XREG, CurrentEL __CR__\
        cmp    SAFE_XREG, NUM(0x8)  __CR__\
6
        bgt    %b6                  __CR__\
        beq    %f2                  __CR__\
        cbnz   SAFE_XREG, NUM(0x4)  __CR__\
5                                   __CR__\
        bne    %b5                 // We should never get here
#endif

// CurrentEL : 0xC = EL3; 8 = EL2; 4 = EL1
// This only selects between EL1 and EL2 and EL3, else we die.
// Provide the Macro with a safe temp xreg to use.
#if !defined(_MSC_VER)
#define EL1_OR_EL2_OR_EL3(SAFE_XREG) \
        mrs    SAFE_XREG, CurrentEL ;\
        cmp    SAFE_XREG, #0x8      ;\
        b.gt   3f                   ;\
        b.eq   2f                   ;\
        cbnz   SAFE_XREG, 1f        ;\
        b      .                    ;// We should never get here
#else
#define EL1_OR_EL2_OR_EL3(SAFE_XREG) \
        mrs    SAFE_XREG, CurrentEL  __CR__\
        cmp    SAFE_XREG, NUM(0x8)   __CR__\
        bgt    %f3                   __CR__\
        beq    %f2                   __CR__\
        cbnz   SAFE_XREG, %f1        __CR__\
5                                    __CR__\
        bne    %b5                // We should never get here
#endif


#if defined(_MSC_VER)
#define LoadConstantToReg(Data, Reg) \
  ldr Reg, =Data

#endif
// MU_CHANGE END

// MU_CHANGE BEGIN - Remove non-VS versions of macros.
#if !defined(_MSC_VER)

#define _ASM_FUNC(Name, Section)    \
  .global   Name                  ; \
  .section  #Section, "ax"        ; \
  .type     Name, %function       ; \
  Name:

#define ASM_FUNC(Name)            _ASM_FUNC(ASM_PFX(Name), .text. ## Name)

#define MOV32(Reg, Val)                   \
  movz      Reg, (Val) >> 16, lsl #16   ; \
  movk      Reg, (Val) & 0xffff

#define MOV64(Reg, Val)                             \
  movz      Reg, (Val) >> 48, lsl #48             ; \
  movk      Reg, ((Val) >> 32) & 0xffff, lsl #32  ; \
  movk      Reg, ((Val) >> 16) & 0xffff, lsl #16  ; \
  movk      Reg, (Val) & 0xffff

#endif
// MU_CHANGE END

#endif // __MACRO_IO_LIBV8_H__
