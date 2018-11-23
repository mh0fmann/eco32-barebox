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

    .global __muldi3

__muldi3:
    subi    $29,$29,0x001c
    stw     $28,$29,0x0000
    addi    $28,$29,0x0004
    add     $12,$4,$0
    ldw     $10,$5,0x0004
    ldw     $8,$6,0x0000
    ldw     $13,$6,0x0004
    ldw     $9,$5,0x0000
    stw     $9,$28,0x0010
    stw     $8,$28,0x0008
    andi    $9,$10,0xffff
    slri    $11,$10,0x0010
    andi    $14,$13,0xffff
    slri    $8,$13,0x0010
    mul     $15,$9,$14
    mul     $14,$11,$14
    mul     $11,$11,$8
    mul     $8,$9,$8
    add     $8,$8,$14
    slri    $9,$15,0x0010
    add     $8,$9,$8
    bleu    $14,$8,__muldi3_1
    ldhi    $1,0x00010000
    ori     $1,$1,0x0000
    add     $11,$11,$1
__muldi3_1:
    slri    $9,$8,0x0010
    slli    $8,$8,0x0010
    andi    $15,$15,0xffff
    add     $8,$8,$15
    add     $11,$9,$11
    ldw     $9,$28,0x0008
    mul     $9,$10,$9
    ldw     $10,$28,0x0010
    mul     $10,$13,$10
    add     $10,$9,$10
    add     $10,$11,$10
    stw     $10,$12,0x0000
    stw     $8,$12,0x0004
    add     $2,$12,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x001c
    jr      $31
