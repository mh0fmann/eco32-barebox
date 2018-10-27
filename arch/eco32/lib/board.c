#include <common.h>
#include <malloc.h>
#include <init.h>
#include <memory.h>
#include <asm-generic/memory_layout.h>

void __noreturn eco32_start_barebox(void)
{
    mem_malloc_init((void *)(MALLOC_BASE),
                    (void *)(MALLOC_BASE + MALLOC_SIZE - 1));

    start_barebox();
}
