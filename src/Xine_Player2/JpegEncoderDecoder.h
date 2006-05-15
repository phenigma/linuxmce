//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JPEGENCODERDECODER_H
#define JPEGENCODERDECODER_H

/**
*/
class JpegEncoderDecoder
{
private:
	unsigned char *workingBuffer;
	int workingBufferSize;
	int finalBufferSize;

public:
    JpegEncoderDecoder();

    ~JpegEncoderDecoder();

	bool encodeIntoBuffer(char *buffer, int width, int height, int bytesPerPixel, char* &compressedBuffer, int &compressedSize);

	bool allocateBuffer(int width, int height, int bpp);

	unsigned char *getAllocatedBuffer();

	int getBufferSize();

	void setCompressedSize(int finalSize);
};

#endif
