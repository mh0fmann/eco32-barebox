#include <common.h>
#include <init.h>
#include <restart.h>

static void __noreturn eco32_restart_cpu(struct restart_handler *rst)
{
    /* we do not restart atm */
    hang();
}

static int restart_register_feature(void)
{
    return restart_handler_register_fn(eco32_restart_cpu);
}
coredevice_initcall(restart_register_feature);
