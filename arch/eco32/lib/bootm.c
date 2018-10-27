#include <boot.h>
#include <bootm.h>
#include <common.h>
#include <libfile.h>
#include <malloc.h>
#include <init.h>
#include <fs.h>
#include <errno.h>
#include <binfmt.h>
#include <restart.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/cache.h>

static int do_bootm_elf(struct image_data *data)
{
    void (*entry)(void* fdt);
    struct elf_image* elf;
    void* buf, *fdt;
    
    buf = read_file(data->os_file, NULL);
    if (!buf)
        return -EINVAL;

    elf = elf_load_image(buf);
    if (IS_ERR(elf))
        return PTR_ERR(elf);
    
    fdt = bootm_get_devicetree(data);
    if (IS_ERR(fdt)) {
        goto bootm_elf_done;
    }

    pr_info("Starting application at 0x%08lx\n", elf->entry);

    if (data->dryrun)
        goto bootm_elf_done;

    shutdown_barebox();

    entry = (void*)elf->entry;

    flush_caches();

    entry(fdt);

bootm_elf_done:
    elf_release_image(elf);
    free(buf);

    return 1;
}

static struct image_handler elf_handler = {
    .name = "ELF",
    .bootm = do_bootm_elf,
    .filetype = filetype_elf,
};

static struct binfmt_hook binfmt_elf_hook = {
    .type = filetype_elf,
    .exec = "bootm",
};

static int eco32_register_image_handler(void)
{
    register_image_handler(&elf_handler);
    binfmt_register(&binfmt_elf_hook);

    return 0;
}
late_initcall(eco32_register_image_handler);
