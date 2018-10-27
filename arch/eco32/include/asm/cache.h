#ifndef __ASM_ECO32_CACHE_H_
#define __ASM_ECO32_CACHE_H_

#define flush_caches()      __asm__("cctl 7")
#define flush_dcache()      __asm__("cctl 4")
#define flush_icache()      __asm__("cctl 3")

#endif /* __ASM_ECO32_CACHE_H_ */
