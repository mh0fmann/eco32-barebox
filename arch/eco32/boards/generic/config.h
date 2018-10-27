#ifndef _GENERIC_NAMES_H_
#define _GENERIC_NAMES_H_

#define CONFIG_SYS_CLK_FREQ         20000000

#define ECO32_TIMER_FREQ            50000000

#define ECO32_SOPC_MEMORY_BASE      0xC0000000
#define ECO32_SOPC_MEMORY_SIZE      0x02000000

/* We reserve 1M for barebox */
#define BAREBOX_RESERVED_SIZE       0x00100000

/* Devices */
#define ECO32_SOPC_UART_BASE        0xF0300000
#define ECO32_SOPC_DISK_BASE        0xF0400000

/* Where Barebox will be in the memory */
#define ECO32_SOPC_TEXT_BASE        CONFIG_ARCH_TEXT_BASE

#endif
