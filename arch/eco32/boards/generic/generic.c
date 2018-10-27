#include <common.h>
#include <driver.h>
#include <memory.h>
#include <init.h>

static int eco32_mem_init(void)
{
    barebox_add_memory_bank("ram0", ECO32_SOPC_MEMORY_BASE, ECO32_SOPC_MEMORY_SIZE);

    return 0;
}
mem_initcall(eco32_mem_init);

static int eco32_console_init(void)
{
    add_generic_device("eco32_serial", DEVICE_ID_DYNAMIC, NULL,
                        ECO32_SOPC_UART_BASE, 0x20, IORESOURCE_MEM, NULL);
    add_generic_device("eco32_disk", DEVICE_ID_DYNAMIC, NULL,
                        ECO32_SOPC_DISK_BASE, 0x00080000, IORESOURCE_MEM, NULL);
    return 0;
}
console_initcall(eco32_console_init);

static int eco32_core_init(void)
{
    barebox_set_model("eco32-generic");
    barebox_set_hostname("eco32");

    return 0;
}
core_initcall(eco32_core_init);
