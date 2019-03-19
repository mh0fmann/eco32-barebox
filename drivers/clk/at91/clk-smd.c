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
#include <io.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/clk/at91_pmc.h>
#include <mfd/syscon.h>
#include <regmap.h>

#include "pmc.h"

#define SMD_SOURCE_MAX		2

#define SMD_DIV_SHIFT		8
#define SMD_MAX_DIV		0xf

struct at91sam9x5_clk_smd {
	struct clk clk;
	struct regmap *regmap;
	const char *parent_names[SMD_SOURCE_MAX];
};

#define to_at91sam9x5_clk_smd(clk) \
	container_of(clk, struct at91sam9x5_clk_smd, clk)

static unsigned long at91sam9x5_clk_smd_recalc_rate(struct clk *clk,
						    unsigned long parent_rate)
{
	struct at91sam9x5_clk_smd *smd = to_at91sam9x5_clk_smd(clk);
	unsigned int smdr;
	u8 smddiv;

	regmap_read(smd->regmap, AT91_PMC_SMD, &smdr);
	smddiv = (smdr & AT91_PMC_SMD_DIV) >> SMD_DIV_SHIFT;

	return parent_rate / (smddiv + 1);
}

static long at91sam9x5_clk_smd_round_rate(struct clk *clk, unsigned long rate,
					  unsigned long *parent_rate)
{
	unsigned long div;
	unsigned long bestrate;
	unsigned long tmp;

	if (rate >= *parent_rate)
		return *parent_rate;

	div = *parent_rate / rate;
	if (div > SMD_MAX_DIV)
		return *parent_rate / (SMD_MAX_DIV + 1);

	bestrate = *parent_rate / div;
	tmp = *parent_rate / (div + 1);
	if (bestrate - rate > rate - tmp)
		bestrate = tmp;

	return bestrate;
}

static int at91sam9x5_clk_smd_set_parent(struct clk *clk, u8 index)
{
	struct at91sam9x5_clk_smd *smd = to_at91sam9x5_clk_smd(clk);

	if (index > 1)
		return -EINVAL;

	regmap_write_bits(smd->regmap, AT91_PMC_SMD, AT91_PMC_SMDS,
			  index ? AT91_PMC_SMDS : 0);

	return 0;
}

static int at91sam9x5_clk_smd_get_parent(struct clk *clk)
{
	struct at91sam9x5_clk_smd *smd = to_at91sam9x5_clk_smd(clk);
	unsigned int smdr;

	regmap_read(smd->regmap, AT91_PMC_SMD, &smdr);

	return smdr & AT91_PMC_SMDS;
}

static int at91sam9x5_clk_smd_set_rate(struct clk *clk, unsigned long rate,
				       unsigned long parent_rate)
{
	struct at91sam9x5_clk_smd *smd = to_at91sam9x5_clk_smd(clk);
	unsigned long div = parent_rate / rate;

	if (parent_rate % rate || div < 1 || div > (SMD_MAX_DIV + 1))
		return -EINVAL;

	regmap_write_bits(smd->regmap, AT91_PMC_SMD, AT91_PMC_SMD_DIV,
			  (div - 1) << SMD_DIV_SHIFT);

	return 0;
}

static const struct clk_ops at91sam9x5_smd_ops = {
	.recalc_rate = at91sam9x5_clk_smd_recalc_rate,
	.round_rate = at91sam9x5_clk_smd_round_rate,
	.get_parent = at91sam9x5_clk_smd_get_parent,
	.set_parent = at91sam9x5_clk_smd_set_parent,
	.set_rate = at91sam9x5_clk_smd_set_rate,
};

struct clk *
at91sam9x5_clk_register_smd(struct regmap *regmap, const char *name,
			    const char **parent_names, u8 num_parents)
{
	struct at91sam9x5_clk_smd *smd;
	int ret;

	smd = xzalloc(sizeof(*smd));
	smd->clk.name = name;
	smd->clk.ops = &at91sam9x5_smd_ops;
	memcpy(smd->parent_names, parent_names,
	       num_parents * sizeof(smd->parent_names[0]));
	smd->clk.parent_names = smd->parent_names;
	smd->clk.num_parents = num_parents;
	/* init.flags = CLK_SET_RATE_GATE | CLK_SET_PARENT_GATE; */
	smd->regmap = regmap;

	ret = clk_register(&smd->clk);
	if (ret) {
		kfree(smd);
		return ERR_PTR(ret);
	}

	return &smd->clk;
}
