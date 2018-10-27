#include <common.h>
#include <init.h>
#include <errno.h>
#include <block.h>
#include <disks.h>
#include <malloc.h>

#define CR      0x0
#define SC      0x4
#define SA      0x8
#define CP      0xC
#define BF      0x00080000

#define INIT    0x20
#define FIN     0x10
#define START   0x01

#define WRITE   0x05
#define READ    0x01


void __iomem* regs;

static int eco32_disk_read(struct block_device *blk, void *buffer,
                         int block, int num_blocks)
{
    while (num_blocks > 0) {
        unsigned int n;
        if (num_blocks > 8) {
            n = 8;
            num_blocks-=8;
        } else {
            n = num_blocks;
            num_blocks = 0;
        }

        writel(n, regs + SC);
        writel(block, regs + SA);
        writel(READ, regs + CR);

        while (!(readl(regs + CR) & FIN));

        for (unsigned int i = 0; i < n * SECTOR_SIZE; i += 4){
            *((unsigned int*)buffer) = readl(regs + BF + i);
            buffer += 4;
        }

        block += n;
    }
    return 0;
}

#ifdef CONFIG_BLOCK_WRITE
static int eco32_disk_write(struct block_device *blk, const void *buffer,
                          int block, int num_blocks)
{
    while (num_blocks > 0) {
        unsigned int n;
        if (num_blocks > 8) {
            n = 8;
            num_blocks-=8;
        } else {
            n = num_blocks;
            num_blocks = 0;
        }

        writel(n, regs + SC);
        writel(block, regs + SA);
        writel(WRITE, regs + CR);

        for (unsigned int i = 0; i < n * SECTOR_SIZE; i += 4){
            writel(*((unsigned int*)buffer), regs + BF + i);
            buffer += 4;
        }

        while (!(readl(regs + CR) & FIN));

        block += n;
    }
    return 0;
}
#endif

static struct block_device_ops eco32_disk_ops = {
    .read = eco32_disk_read,
#ifdef CONFIG_BLOCK_WRITE
    .write = eco32_disk_write,
#endif
};

static int eco32_disk_probe(struct device_d *dev)
{
    int rc;
    struct resource* iores;
    struct block_device* blk;

    iores = dev_request_mem_resource(dev, 0);
    if (IS_ERR(iores))
        return PTR_ERR(iores);
    regs = (void*)((unsigned int)IOMEM(iores->start) | 0xC0000000);


    while (!(readl(regs + CR) & INIT));

    if (readl(regs + CP) == 0) {
        dev_err(dev, "No disk present\n");
        return -ENODEV;
    }

    blk = xzalloc(sizeof(struct block_device));

    blk->dev = dev;
    blk->ops = &eco32_disk_ops;
    blk->num_blocks = readl(regs + CP);

    rc = cdev_find_free_index("disk");
    blk->cdev.name = basprintf("disk%d", rc);
    blk->blockbits = SECTOR_SHIFT;

    rc = blockdevice_register(blk);
    if (rc) {
        dev_err(dev, "could not register eco32 disk\n");
        free(blk);
        return rc;
    }

    rc = parse_partition_table(blk);
    if (rc != 0) {
        dev_warn(dev, "No partition table found\n");
    }

    return 0;
}

static __maybe_unused struct of_device_id eco32_disk_dt_ids[] = {
    {
        .compatible = "thm,eco32-disk",
    }, {
        /* sentinel */
    }
};

static struct driver_d eco32_disk_driver = {
    .name   = "eco32_disk",
    .probe  = eco32_disk_probe,
    .of_compatible = DRV_OF_COMPAT(eco32_disk_dt_ids),
};
device_platform_driver(eco32_disk_driver);
