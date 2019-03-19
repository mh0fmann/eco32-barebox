// SPDX-License-Identifier: GPL-2.0+
/* Author: Alexander Shiyan <shc_work@mail.ru> */

#include <common.h>
#include <clock.h>
#include <io.h>
#include <init.h>

#include <linux/clk.h>
#include <linux/err.h>

static __iomem void *clps711x_timer_base;

static uint64_t clps711x_cs_read(void)
{
	return ~readw(clps711x_timer_base);
}

static struct clocksource clps711x_cs = {
	.read = clps711x_cs_read,
	.mask = CLOCKSOURCE_MASK(16),
};

static int clps711x_cs_probe(struct device_d *dev)
{
	struct resource *iores;
	u32 rate;
	struct clk *timer_clk;

	timer_clk = clk_get(dev, NULL);
	if (IS_ERR(timer_clk))
		return PTR_ERR(timer_clk);

	rate = clk_get_rate(timer_clk);
	iores = dev_request_mem_resource(dev, 0);
	if (IS_ERR(iores)) {
		clk_put(timer_clk);
		return PTR_ERR(iores);
	}
	clps711x_timer_base = IOMEM(iores->start);

	clocks_calc_mult_shift(&clps711x_cs.mult, &clps711x_cs.shift, rate,
			       NSEC_PER_SEC, 10);

	return init_clock(&clps711x_cs);
}

static __maybe_unused struct of_device_id clps711x_timer_dt_ids[] = {
	{ .compatible = "cirrus,ep7209-timer", },
	{ }
};

static struct driver_d clps711x_cs_driver = {
	.name = "clps711x-cs",
	.probe = clps711x_cs_probe,
	.of_compatible = DRV_OF_COMPAT(clps711x_timer_dt_ids),
};
coredevice_platform_driver(clps711x_cs_driver);
