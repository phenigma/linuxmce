
#ifndef UUENCODE_H
#define UUENCODE_H

int Ns_HtuuEncode(unsigned char *input, unsigned int len, unsigned char *output);

int Ns_HtuuDecode(unsigned char *input, unsigned char *output, int outputlen);

#define MaxEncodedSize(Size) (1 + (Size * 4) / 3)
#define MaxDecodedSize(Size) (3 + (Size * 3) / 4)

#endif

