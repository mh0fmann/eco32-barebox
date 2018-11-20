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
    void (*kernel)(const char*, void*, unsigned int, unsigned int);

    const char* cmdline;
    int ret;
    void* fdt;
    unsigned int initrd_start = 0, initrd_end = 0;

    /* get cmdline */
    ret = bootm_load_os(data, data->os_address);
    if (ret)
        return ret;

    cmdline = linux_bootargs_get();
    if (IS_ERR(cmdline))
        return 1;

    /* get device tree */
    fdt = bootm_get_devicetree(data);
    if (IS_ERR(fdt))
        return 1;

    /* get initrd */
    if (bootm_has_initrd(data)) {
        if (data->initrd_address == UIMAGE_INVALID_ADDRESS) {
            return 1;
        }

        ret = bootm_load_initrd(data, data->initrd_address);
        if (ret)
            return ret;

        initrd_start = data->initrd_res->start;
        initrd_end = data->initrd_res->end;
    }

    if (bootm_verbose(data)) {
        pr_info("cmdline:%s\n", cmdline);
        pr_info("fdt@0x%08x\n", (unsigned int)fdt);
        pr_info("initrd_start@0x%08x\n", initrd_start);
        pr_info("initrd_end@0x%08x\n", initrd_end);
    }

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

    kernel(cmdline, fdt, initrd_start, initrd_end);

    /* not reached */
    return -1;
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
