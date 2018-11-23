/*
 * Copyright (c) 2018 Martin Hofmann <martin.hofmann@mni.thm.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <config.h>
#include <asm-generic/memory_layout.h>

#define PSW             0
#define TLB_INDEX       1
#define TLB_ENTRY_HI    2
#define TLB_ENTRY_LO    3
#define TLB_ENTRIES     32

    .global _start

    .section ".text_entry","ax"

_start:
    # init PSW
    # disable interrupts
    # interrupt vector points to ROM
    mvts    $0,PSW

    # setup stack pointer
    addi    $29,$0,STACK_BASE + STACK_SIZE

    # invalidate TLB
    # fill with direct-mapped virtual addresses
    mvts    $0,TLB_ENTRY_LO
    ldhi    $8,0xC0000000
    mvts    $8,TLB_ENTRY_HI
    addi    $8,$0,0
    addi    $9,$0,TLB_ENTRIES
tlb_inv_loop:
    mvts    $8,TLB_INDEX
    tbwi
    addi    $8,$8,1
    bne     $8,$9,tlb_inv_loop

    # clear BSS
    addi    $8,$0,__bss_start
    addi    $9,$0,__bss_stop
    j       bss_clr_test
bss_clr_loop:
    stw $0,$8,0
    addi    $8,$8,4
bss_clr_test:
    bltu    $8,$9,bss_clr_loop

    # jump to the main routine
    j       eco32_start_barebox

    # catch if we somehow return
loop:
    j       loop

