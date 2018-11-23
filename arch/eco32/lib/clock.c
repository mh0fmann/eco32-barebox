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
#include <clock.h>
#include <init.h>
#include <io.h>

#define TI_CT       0xF0000000
#define TI_DI       0xF0000004
#define TI_CN       0xF0000008


static uint64_t eco32_clocksource_read(void)
{
    return (uint64_t)readl(TI_CN);
}

static struct clocksource eco32_cs = {
    .read   = eco32_clocksource_read,
    .mask   = 0xFFFFFFFF,
    .shift  = 10,
};

static int eco32_clocksource_init(void)
{
    eco32_cs.mult = clocksource_hz2mult(ECO32_TIMER_FREQ, eco32_cs.shift);

    writel(0, TI_CT);
    writel(0xFFFFFFFF, TI_DI);

    return init_clock(&eco32_cs);
}
core_initcall(eco32_clocksource_init);
