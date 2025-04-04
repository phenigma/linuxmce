#ifndef MPLAYER_MPBSWAP_H
#define MPLAYER_MPBSWAP_H

#include <sys/types.h>
#include "libavutil/bswap.h"
#include "config.h"

#ifndef HAVE_SWAB
void swab(const void *from, void *to, ssize_t n);
#endif

#endif /* MPLAYER_MPBSWAP_H */
