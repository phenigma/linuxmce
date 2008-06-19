#include "LZO.h"
#include <string.h>

// TODO: make this thread safe
#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);

// memory must be freed by called using 'delete []'
char * LZO::Compress(lzo_byte *data, lzo_uint length, lzo_uint *compressed_size)
{
	char *pCompressedData = new char[length * 3];

	lzo1x_1_compress(data, length, (lzo_byte *)(pCompressedData + sizeof(int)), compressed_size, wrkmem);
	*((int *)pCompressedData) = (int)length;
	*compressed_size += sizeof(int);

	return pCompressedData;
}

// memory must be freed by called using 'delete []'
char * LZO::Decompress(lzo_byte *data, lzo_uint length, lzo_uint *decompressed_size)
{
	int nUncompressDataSize = *(reinterpret_cast<int *>(data));
	char *pUncompressedData = new char[nUncompressDataSize + 1];
	memset(pUncompressedData, 0, nUncompressDataSize + 1);

	lzo1x_decompress(data + sizeof(int), length - sizeof(int), (lzo_byte *)pUncompressedData, decompressed_size, wrkmem);

	return pUncompressedData;
}
