#include <common.h>
#include <driver.h>
#include <memory.h>
#include <envfs.h>
#include <init.h>


static int eco32_core_init(void)
{
    barebox_set_model("eco32-generic");
    barebox_set_hostname("eco32");

    default_environment_path_set(NULL);

    return 0;
}
core_initcall(eco32_core_init);
