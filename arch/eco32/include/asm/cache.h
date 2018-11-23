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

#ifndef __ASM_ECO32_CACHE_H_
#define __ASM_ECO32_CACHE_H_

#define flush_caches()      __asm__("cctl 7")
#define flush_dcache()      __asm__("cctl 4")
#define flush_icache()      __asm__("cctl 3")

#endif /* __ASM_ECO32_CACHE_H_ */
