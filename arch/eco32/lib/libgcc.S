
    .global __muldi3
    .global __ashldi3
    .global __lshrdi3
    .global __ashrdi3

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

__lshrdi3:
    subi    $29,$29,0x0014
    stw     $28,$29,0x0000
    addi    $28,$29,0x0004
    add     $11,$4,$0
    ldw     $8,$5,0x0000
    ldw     $9,$5,0x0004
    addi    $10,$0,0x0000
    beq     $6,$10,__lshrdi3_2
    addi    $12,$0,0x0020
    sub     $12,$12,$6
    ble     $12,$10,__lshrdi3_3
    slr     $10,$8,$6
    stw     $10,$28,0x0000
    slr     $9,$9,$6
    sll     $8,$8,$12
    or      $8,$9,$8
    stw     $8,$28,0x0004
    ldw     $8,$28,0x0000
    stw     $8,$11,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$11,0x0004
__lshrdi3_1:
    add     $2,$11,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
__lshrdi3_2:
    jr      $31
    stw     $8,$4,0x0000
    stw     $9,$4,0x0004
    add     $2,$11,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
    jr      $31
__lshrdi3_3:
    stw     $10,$28,0x0000
    sub     $10,$10,$12
    slr     $8,$8,$10
    stw     $8,$28,0x0004
    ldw     $8,$28,0x0000
    stw     $8,$11,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$11,0x0004
    j       __lshrdi3_1


__ashldi3:
    subi    $29,$29,0x0014
    stw     $28,$29,0x0000
    addi    $28,$29,0x0004
    add     $11,$4,$0
    ldw     $9,$5,0x0000
    ldw     $8,$5,0x0004
    addi    $10,$0,0x0000
    beq     $6,$10,__ashldi3_2
    addi    $12,$0,0x0020
    sub     $12,$12,$6
    ble     $12,$10,__ashldi3_3
    sll     $10,$8,$6
    stw     $10,$28,0x0004
    sll     $9,$9,$6
    slr     $8,$8,$12
    or      $8,$9,$8
    stw     $8,$28,0x0000
    ldw     $8,$28,0x0000
    stw     $8,$11,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$11,0x0004
__ashldi3_1:
    add     $2,$11,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
    jr      $31
__ashldi3_2:
    stw     $9,$4,0x0000
    stw     $8,$4,0x0004
    add     $2,$11,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
    jr      $31
__ashldi3_3:
    stw     $10,$28,0x0004
    sub     $10,$10,$12
    sll     $8,$8,$10
    stw     $8,$28,0x0000
    ldw     $8,$28,0x0000
    stw     $8,$11,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$11,0x0004
    j       __ashldi3_1

__ashrdi3:
    subi    $29,$29,0x0014
    stw     $28,$29,0x0000
    addi    $28,$29,0x0004
    add     $10,$4,$0
    ldw     $8,$5,0x0000
    ldw     $9,$5,0x0004
    addi    $11,$0,0x0000
    beq     $6,$11,__ashrdi3_2
    addi    $12,$0,0x0020
    sub     $12,$12,$6
    ble     $12,$11,__ashrdi3_3
    sar     $11,$8,$6
    stw     $11,$28,0x0000
    slr     $9,$9,$6
    sll     $8,$8,$12
    or      $8,$9,$8
    stw     $8,$28,0x0004
    ldw     $8,$28,0x0000
    stw     $8,$10,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$10,0x0004
__ashrdi3_1:
    add     $2,$10,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
    jr      $31
__ashrdi3_2:
    stw     $8,$4,0x0000
    stw     $9,$4,0x0004
    add     $2,$10,$0
    ldw     $28,$29,0x0000
    addi    $29,$29,0x0014
    jr      $31
__ashrdi3_3:
    sari    $9,$8,0x001f
    stw     $9,$28,0x0000
    sub     $11,$11,$12
    sar     $8,$8,$11
    stw     $8,$28,0x0004
    ldw     $8,$28,0x0000
    stw     $8,$10,0x0000
    ldw     $8,$28,0x0004
    stw     $8,$10,0x0004
    j       __ashrdi3_1
