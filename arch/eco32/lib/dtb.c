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
#include <init.h>
#include <of.h>
#include <memory.h>

extern char __dtb_start[];


void of_add_memory_bank(struct device_node *node, bool dump,
                        int r, u64 base, u64 size)
{
    char s[5];

    sprintf(s, "ram%d", r);
    barebox_add_memory_bank(s, base | 0xC0000000, size);

    if (dump)
        pr_info("%s: %s: 0x%08llx@0x%08llx\n", node->name, s, size, base);
}

static int eco32_of_init(void)
{
    struct device_node *root;

    root = of_get_root_node();
    if (root)
        return 0;

    root = of_unflatten_dtb(__dtb_start);
    if (!IS_ERR(root)) {
        pr_debug("using internal DTB\n");
        of_set_root_node(root);
        if (IS_ENABLED(CONFIG_OFDEVICE))
            of_probe();
        return 0;
    }

    return 1;
}
core_initcall(eco32_of_init);
