/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 */

#ifndef _ASM_BITOPS_H_
#define _ASM_BITOPS_H_

#include <asm-generic/bitops/__ffs.h>
#include <asm-generic/bitops/__fls.h>
#include <asm-generic/bitops/ffs.h>
#include <asm-generic/bitops/fls.h>
#include <asm-generic/bitops/ffz.h>
#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/find.h>
#include <asm-generic/bitops/ops.h>

#define set_bit(x, y)               __set_bit(x, y)
#define clear_bit(x, y)             __clear_bit(x, y)
#define change_bit(x, y)            __change_bit(x, y)
#define test_and_set_bit(x, y)      __test_and_set_bit(x, y)
#define test_and_clear_bit(x, y)    __test_and_clear_bit(x, y)
#define test_and_change_bit(x, y)   __test_and_change_bit(x, y)

#endif /* _ASM_BITOPS_H_ */
