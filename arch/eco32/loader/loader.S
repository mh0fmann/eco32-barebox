/*
 * Copyright (c) 2018 Martin Hofmann <martin.hofmann@mni.thm.de>
 * Copyright (c) 2018 Hellwig Geisse <hellwig.geisse@mni.thm.de>
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

#
# loader.S - The Barebox-Loader
#

# Runtime environment:
#
# This code must be loaded and started at 0xC0010000.
# It allocates a stack from 0xC0011000 downwards. So
# it must run within 4K (code + data + stack).
#
# This code expects the disk number of the boot disk
# in $16, the start sector of the disk or partition
# to be booted in $17 and its size in $18.
#
# If Barebox will be used as stage 2 bootloader this
# will be the stage 1 that loads barebox.
# Otherwise this will be the partition boot record
# which will be loaded by the original ECO32 stage 2
# bootloader.
# Either way this will load Barebox to memory and
# execute it


    .global start

.section .text
    # load the image and start it
start:
    addi    $29,$0,0xC0011000               # setup stack
    addi    $4,$0,strtmsg                   # say what is going on
    jal     msgout
    # $19 = sector number
    # $20 = load address
    # $21 = sectors left
    bgtu    $17,$0,no_mboot                 # if we are the stage 2 adjust
    addi    $17,$0,1                        # $17 to load from sector 2
no_mboot:
    addi    $19,$0,1                        # start loading with sector 1
    addi    $20,$0,CONFIG_ARCH_TEXT_BASE    # where to load the image
    addi    $21,$0,BAREBOX_SIZE_IN_SECTORS  # how many sectors to load
start1:
    addi    $4,$0,'.'
    jal     chrout
    add     $4,$0,$19
    add     $5,$0,$20
    andi    $5,$5,0x3FFFFFFF
    addi    $6,$0,256                       # load in pieces of 256 sectors
    jal     rdsct
    addi    $19,$19,256
    addi    $20,$20,256*512
    subi    $21,$21,256
    bgt     $21,$0,start1
    cctl    7                               # sync caches
    addi    $8,$0,CONFIG_ARCH_TEXT_BASE     # start executing barebox
    jr  $8

    # read disk sectors
    #   $4 start sector number (disk or partition relative)
    #   $5 transfer address
    #   $6 number of sectors
rdsct:
    subi    $29,$29,32
    stw     $31,$29,20
    stw     $6,$29,16           # sector count
    add     $7,$5,$0            # transfer address
    add     $6,$4,$17           # relative sector -> absolute
    addi    $5,$0,'r'           # command
    add     $4,$0,$16           # disk number
    addi    $8,$0,0xC0000028
    jalr    $8
    bne     $2,$0,rderr         # error?
    ldw     $31,$29,20
    addi    $29,$29,32
    jr      $31

    # disk read error
rderr:
    addi    $4,$0,dremsg
    jal     msgout
    j       halt

    # output message
    #   $4 pointer to string
msgout:
    subi    $29,$29,8
    stw     $31,$29,4
    stw     $16,$29,0
    addi    $16,$4,0            # $16: pointer to string
msgout1:
    ldbu    $4,$16,0            # get character
    beq     $4,$0,msgout2       # done?
    jal     chrout              # output character
    addi    $16,$16,1           # bump pointer
    j       msgout1             # continue
msgout2:
    ldw     $16,$29,0
    ldw     $31,$29,4
    addi    $29,$29,8
    jr      $31

    # output character
    #   $4 character
chrout:
    subi    $29,$29,4
    stw     $31,$29,0
    addi    $8,$0,0xC0000020
    jalr    $8
    ldw     $31,$29,0
    addi    $29,$29,4
    jr      $31

    # halt execution by looping
halt:
    addi    $4,$0,hltmsg
    jal     msgout
halt1:
    j       halt1

    # messages
strtmsg:
    .string "Loading Barebox "
dremsg:
    .string "Disk read error\r\n"
hltmsg:
    .string "Bootstrap halted\r\n"

# boot record signature
.section .data
    .byte   0x55, 0xAA
