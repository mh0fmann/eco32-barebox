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


int do_bootm_linux(struct image_data* data)
{
    void (*kernel)(char*, void*, unsigned int, unsigned int);

    const char* cmdline = linux_bootargs_get();
    int ret;
    void* fdt;

    ret = bootm_load_os(data, data->os_address);
    if (ret)
        return ret;

    fdt = bootm_get_devicetree(data);
    if (IS_ERR(fdt))
        return 1;

    if (data->dryrun)
        return 0;

    kernel = (void*)(data->os_address + data->os_entry);

    /* kernel parameter passing
     * $4 : cmdline
     * $5 : fdt
     * $6 : initrd start
     * $7 : initrd end
     */

    flush_caches();

    kernel(cmdline, NULL, 0, 0);
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

static struct image_handler linux_handler = {
    .name = "ECO32 Linux",
    .bootm = do_bootm_linux,
    .filetype = filetype_uimage,
    .ih_os = IH_OS_LINUX,
};

static int eco32_register_image_handler(void)
{
    int ret;

    ret = register_image_handler(&elf_handler);
    binfmt_register(&binfmt_elf_hook);

    ret = register_image_handler(&linux_handler);

    return ret;
}
late_initcall(eco32_register_image_handler);
