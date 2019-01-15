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

#include <common.h>
#include <malloc.h>
#include <init.h>
#include <memory.h>
#include <asm-generic/memory_layout.h>

void __noreturn eco32_start_barebox(void);

void __noreturn eco32_start_barebox(void)
{
    mem_malloc_init((void *)(MALLOC_BASE),
                    (void *)(MALLOC_BASE + MALLOC_SIZE - 1));

    start_barebox();
}
