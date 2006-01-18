
#ifndef UUENCODE_H
#define UUENCODE_H

int Ns_HtuuEncode(unsigned char *input, unsigned int len, unsigned char *output);

int Ns_HtuuDecode(unsigned char *input, unsigned char *output, int outputlen);

/* The docs say:
 * Side effects:
 *      Encoded characters are placed in output which must be
 *	large enough for the result, i.e., (1 + (len * 4) / 3)
 But I pass in a data block of 38284 and get back 51048 when 51046 should be the max!!!!
 Adding an extra 100 bytes just to be safe
 */

#define MaxEncodedSize(Size) (1 + ( (Size+10) * 4) / 3)
#define MaxDecodedSize(Size) (3 + ( (Size+10) * 3) / 4)

#endif

