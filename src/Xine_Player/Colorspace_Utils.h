#ifndef COLORSPACE_UTILS_H
#define COLORSPACE_UTILS_H

#include <inttypes.h>
#include <malloc.h>

class Colorspace_Utils
{
public:
	static void yuy2toyv12 ( uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height );
	static uint8_t * yv12torgb ( uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height );
};

#endif
