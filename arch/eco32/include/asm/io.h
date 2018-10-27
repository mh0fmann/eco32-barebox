#ifndef __ASM_ECO32_IO_H
#define __ASM_ECO32_IO_H

#define IO_SPACE_LIMIT      0x0

#include <asm/byteorder.h>

#define __raw_writeb(v, a)       (*(volatile unsigned char  *)(a) = (v))
#define __raw_writew(v, a)       (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v, a)       (*(volatile unsigned int   *)(a) = (v))

#define __raw_readb(a)          (*(volatile unsigned char  *)(a))
#define __raw_readw(a)          (*(volatile unsigned short *)(a))
#define __raw_readl(a)          (*(volatile unsigned int   *)(a))

#define readb(addr)             __raw_readb(addr)
#define readw(addr)             __raw_readw(addr)
#define readl(addr)             __raw_readl(addr)

#define writeb(val, addr)       __raw_writeb(val, addr)
#define writew(val, addr)       __raw_writew(val, addr)
#define writel(val, addr)       __raw_writel(val, addr)

#endif
