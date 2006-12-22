#define __NO_VERSION__
#include <linux/config.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
#include "keywest_old.c"
#else
#include "../alsa-kernel/ppc/keywest.c"
#endif
