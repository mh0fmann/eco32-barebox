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
#include <restart.h>

static void __noreturn eco32_restart_cpu(struct restart_handler *rst)
{
    /* we do not restart atm */
    hang();
}

static int restart_register_feature(void)
{
    return restart_handler_register_fn(eco32_restart_cpu);
}
coredevice_initcall(restart_register_feature);
