#ifndef PLUTO_UTILS_LZO_H
#define PLUTO_UTILS_LZO_H

#include "minilzo.h"

namespace LZO
{
	char * Compress(lzo_byte *data, lzo_uint length, lzo_uint *compressed_size);
	char * Decompress(lzo_byte *data, lzo_uint length, lzo_uint *decompressed_size);
}

#endif /*PLUTO_UTILS_LZO_H*/
