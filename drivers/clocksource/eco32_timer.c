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

#define TI_CT       0
#define TI_DI       4
#define TI_CN       8


static __iomem void *timer_base = NULL;

static uint64_t eco32_cs_read(void)
{
    return (uint64_t)readl(timer_base + TI_CN);
}

static struct clocksource eco32_cs = {
    .read   = eco32_cs_read,
    .mask   = CLOCKSOURCE_MASK(32),
    .shift  = 10,
};

int eco32_cs_probe(struct device_d *dev)
{
    struct resource *iores;
    uint32_t val;

    if (timer_base != NULL) {
        pr_debug("cs already registered. skipping\n");
        return 0;
    }

    iores = dev_request_mem_resource(dev, 0);
    if (IS_ERR(iores))
        return PTR_ERR(iores);

    timer_base = (void*)((unsigned int)IOMEM(iores->start)|0xF0000000);

    eco32_cs.mult = clocksource_hz2mult(ECO32_TIMER_FREQ, eco32_cs.shift);

    writel(0, timer_base + TI_CT);
    writel(0xFFFFFFFF, timer_base + TI_DI);

    return 0;
}


static __maybe_unused struct of_device_id eco32_cs_dt_ids[] = {
    {
        .compatible = "thm,eco32-timer",
    }, {
        /* sentinel */
    }
};

static struct driver_d eco32_cs_driver = {
    .name = "eco32-cs",
    .probe = eco32_cs_probe,
    .of_compatible = DRV_OF_COMPAT(eco32_cs_dt_ids),
};

static int eco32_cs_init(void)
{
    return platform_driver_register(&eco32_cs_driver);
}
core_initcall(eco32_cs_init);
