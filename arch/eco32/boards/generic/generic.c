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
#include <driver.h>
#include <memory.h>
#include <envfs.h>
#include <init.h>


static int eco32_core_init(void)
{
    barebox_set_model("eco32-generic");
    barebox_set_hostname("eco32");

    default_environment_path_set(NULL);

    return 0;
}
core_initcall(eco32_core_init);
