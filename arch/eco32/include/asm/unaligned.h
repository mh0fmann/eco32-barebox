#ifndef __ASM_ECO32_UNALIGNED_H
#define __ASM_ECO32_UNALIGNED_H

#include <asm/byteorder.h>

#if defined(__BIG_ENDIAN)
# include <linux/unaligned/be_memmove.h>
# include <linux/unaligned/le_byteshift.h>
# include <linux/unaligned/generic.h>
# define get_unaligned  __get_unaligned_be
# define put_unaligned  __put_unaligned_be
#else
# error need to define endianess
#endif

#endif /* __ASM_ECO32_UNALIGNED_H */
