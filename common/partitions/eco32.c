/*
 * Copyright 2018 wiggum <martin.hofmann@mni.thm.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <common.h>
#include <disks.h>
#include <init.h>
#include <asm/unaligned.h>
#include <dma.h>
#include <linux/err.h>
#include <string.h>

#include "parser.h"


#define NPE             (SECTOR_SIZE / sizeof(PartEntry))
#define DESCR_SIZE      20
#define PART_MAGIC      0xF5A5F2F9


typedef struct {
    unsigned int type;
    unsigned int start;
    unsigned int size;
    char descr[DESCR_SIZE];
} PartEntry;

/**
 * Check if a ECO32 like partition describes this block device
 * @param blk Block device to register to
 * @param pd Where to store the partition information
 *
 * It seems at least on ARM this routine cannot use temp. stack space for the
 * sector. So, keep the malloc/free.
 */
static void eco32_partition(void *buf, struct block_device *blk,
              struct partition_desc *pd)
{
    PartEntry* table = (buf + SECTOR_SIZE);
    pd->used_entries = 0;

    if (table[NPE-1].type == PART_MAGIC &&
        table[NPE-1].start == ~PART_MAGIC && 
        table[NPE-1].size == PART_MAGIC)
    {
        for (int i = 0; i < NPE-2 && pd->used_entries < MAX_PARTITION; i++) {
            if (table[i].type != 0) {
                struct partition* p = &pd->parts[pd->used_entries];
                
                p->first_sec = table[i].start;
                p->size = table[i].size;
                memcpy(p->name, table[i].descr, DESCR_SIZE);
                
                pd->used_entries++;
            }
        }
    }
}

static struct partition_parser eco32 = {
    .parse = eco32_partition,
    .type = filetype_eco32,
};

static int eco32_partition_init(void)
{
    return partition_parser_register(&eco32);
}
postconsole_initcall(eco32_partition_init);
