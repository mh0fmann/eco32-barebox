/*
 *  Copyright (C) 2013 Boris BREZILLON <b.brezillon@overkiz.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <common.h>
#include <clock.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/clk/at91_pmc.h>
#include <mfd/syscon.h>
#include <regmap.h>

#include "pmc.h"

#define PERIPHERAL_ID_MIN	2
#define PERIPHERAL_ID_MAX	31
#define PERIPHERAL_MASK(id)	(1 << ((id) & PERIPHERAL_ID_MAX))

#define PERIPHERAL_RSHIFT_MASK	0x3
#define PERIPHERAL_RSHIFT(val)	(((val) >> 16) & PERIPHERAL_RSHIFT_MASK)

#define PERIPHERAL_MAX_SHIFT	3

struct clk_peripheral {
	struct clk clk;
	struct regmap *regmap;
	u32 id;
	const char *parent;
};

#define to_clk_peripheral(clk) container_of(clk, struct clk_peripheral, clk)

struct clk_sam9x5_peripheral {
	struct clk clk;
	struct regmap *regmap;
	struct clk_range range;
	u32 id;
	u32 div;
	bool auto_div;
	const char *parent;
};

#define to_clk_sam9x5_peripheral(clk) \
	container_of(clk, struct clk_sam9x5_peripheral, clk)

static int clk_peripheral_enable(struct clk *clk)
{
	struct clk_peripheral *periph = to_clk_peripheral(clk);
	int offset = AT91_PMC_PCER;
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return 0;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCER1;
	regmap_write(periph->regmap, offset, PERIPHERAL_MASK(id));

	return 0;
}

static void clk_peripheral_disable(struct clk *clk)
{
	struct clk_peripheral *periph = to_clk_peripheral(clk);
	int offset = AT91_PMC_PCDR;
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCDR1;
	regmap_write(periph->regmap, offset, PERIPHERAL_MASK(id));
}

static int clk_peripheral_is_enabled(struct clk *clk)
{
	struct clk_peripheral *periph = to_clk_peripheral(clk);
	int offset = AT91_PMC_PCSR;
	unsigned int status;
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return 1;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCSR1;
	regmap_read(periph->regmap, offset, &status);

	return status & PERIPHERAL_MASK(id) ? 1 : 0;
}

static const struct clk_ops peripheral_ops = {
	.enable = clk_peripheral_enable,
	.disable = clk_peripheral_disable,
	.is_enabled = clk_peripheral_is_enabled,
};

struct clk *
at91_clk_register_peripheral(struct regmap *regmap, const char *name,
			     const char *parent_name, u32 id)
{
	int ret;
	struct clk_peripheral *periph;

	if (!name || !parent_name || id > PERIPHERAL_ID_MAX)
		return ERR_PTR(-EINVAL);

	periph = xzalloc(sizeof(*periph));

	periph->clk.name = name;
	periph->clk.ops = &peripheral_ops;

	if (parent_name) {
		periph->parent = parent_name;
		periph->clk.parent_names = &periph->parent;
		periph->clk.num_parents = 1;
	}

	periph->id = id;
	periph->regmap = regmap;

	ret = clk_register(&periph->clk);
	if (ret) {
		kfree(periph);
		return ERR_PTR(ret);
	}

	return &periph->clk;
}

static void clk_sam9x5_peripheral_autodiv(struct clk_sam9x5_peripheral *periph)
{
	struct clk *parent;
	unsigned long parent_rate;
	int shift = 0;

	if (!periph->auto_div)
		return;

	if (periph->range.max) {
		parent = clk_get_parent(&periph->clk);
		parent_rate = clk_get_rate(parent);
		if (!parent_rate)
			return;

		for (; shift < PERIPHERAL_MAX_SHIFT; shift++) {
			if (parent_rate >> shift <= periph->range.max)
				break;
		}
	}

	periph->auto_div = false;
	periph->div = shift;
}

static int clk_sam9x5_peripheral_enable(struct clk *clk)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);

	if (periph->id < PERIPHERAL_ID_MIN)
		return 0;

	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_write_bits(periph->regmap, AT91_PMC_PCR,
			  AT91_PMC_PCR_DIV_MASK | AT91_PMC_PCR_CMD |
			  AT91_PMC_PCR_EN,
			  AT91_PMC_PCR_DIV(periph->div) |
			  AT91_PMC_PCR_CMD |
			  AT91_PMC_PCR_EN);

	return 0;
}

static void clk_sam9x5_peripheral_disable(struct clk *clk)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);

	if (periph->id < PERIPHERAL_ID_MIN)
		return;

	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_write_bits(periph->regmap, AT91_PMC_PCR,
			  AT91_PMC_PCR_EN | AT91_PMC_PCR_CMD,
			  AT91_PMC_PCR_CMD);
}

static int clk_sam9x5_peripheral_is_enabled(struct clk *clk)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);
	unsigned int status;

	if (periph->id < PERIPHERAL_ID_MIN)
		return 1;

	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_read(periph->regmap, AT91_PMC_PCR, &status);

	return status & AT91_PMC_PCR_EN ? 1 : 0;
}

static unsigned long
clk_sam9x5_peripheral_recalc_rate(struct clk *clk,
				  unsigned long parent_rate)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);
	unsigned int status;

	if (periph->id < PERIPHERAL_ID_MIN)
		return parent_rate;

	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_read(periph->regmap, AT91_PMC_PCR, &status);

	if (status & AT91_PMC_PCR_EN) {
		periph->div = PERIPHERAL_RSHIFT(status);
		periph->auto_div = false;
	} else {
		clk_sam9x5_peripheral_autodiv(periph);
	}

	return parent_rate >> periph->div;
}

static long clk_sam9x5_peripheral_round_rate(struct clk *clk,
					     unsigned long rate,
					     unsigned long *parent_rate)
{
	int shift = 0;
	unsigned long best_rate;
	unsigned long best_diff;
	unsigned long cur_rate = *parent_rate;
	unsigned long cur_diff;
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);

	if (periph->id < PERIPHERAL_ID_MIN || !periph->range.max)
		return *parent_rate;

	if (periph->range.max) {
		for (; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
			cur_rate = *parent_rate >> shift;
			if (cur_rate <= periph->range.max)
				break;
		}
	}

	if (rate >= cur_rate)
		return cur_rate;

	best_diff = cur_rate - rate;
	best_rate = cur_rate;
	for (; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
		cur_rate = *parent_rate >> shift;
		if (cur_rate < rate)
			cur_diff = rate - cur_rate;
		else
			cur_diff = cur_rate - rate;

		if (cur_diff < best_diff) {
			best_diff = cur_diff;
			best_rate = cur_rate;
		}

		if (!best_diff || cur_rate < rate)
			break;
	}

	return best_rate;
}

static int clk_sam9x5_peripheral_set_rate(struct clk *clk,
					  unsigned long rate,
					  unsigned long parent_rate)
{
	int shift;
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(clk);
	if (periph->id < PERIPHERAL_ID_MIN || !periph->range.max) {
		if (parent_rate == rate)
			return 0;
		else
			return -EINVAL;
	}

	if (periph->range.max && rate > periph->range.max)
		return -EINVAL;

	for (shift = 0; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
		if (parent_rate >> shift == rate) {
			periph->auto_div = false;
			periph->div = shift;
			return 0;
		}
	}

	return -EINVAL;
}

static const struct clk_ops sam9x5_peripheral_ops = {
	.enable = clk_sam9x5_peripheral_enable,
	.disable = clk_sam9x5_peripheral_disable,
	.is_enabled = clk_sam9x5_peripheral_is_enabled,
	.recalc_rate = clk_sam9x5_peripheral_recalc_rate,
	.round_rate = clk_sam9x5_peripheral_round_rate,
	.set_rate = clk_sam9x5_peripheral_set_rate,
};

struct clk *
at91_clk_register_sam9x5_peripheral(struct regmap *regmap,
				    const char *name, const char *parent_name,
				    u32 id, const struct clk_range *range)
{
	int ret;
	struct clk_sam9x5_peripheral *periph;

	if (!name || !parent_name)
		return ERR_PTR(-EINVAL);

	periph = xzalloc(sizeof(*periph));

	periph->clk.name = name;
	periph->clk.ops = &sam9x5_peripheral_ops;

	if (parent_name) {
		periph->parent = parent_name;
		periph->clk.parent_names = &periph->parent;
		periph->clk.num_parents = 1;
	}

	periph->id = id;
	periph->div = 0;
	periph->regmap = regmap;
	periph->auto_div = true;
	periph->range = *range;

	ret = clk_register(&periph->clk);
	if (ret) {
		kfree(periph);
		return ERR_PTR(ret);
	}

	clk_sam9x5_peripheral_autodiv(periph);

	return &periph->clk;
}
