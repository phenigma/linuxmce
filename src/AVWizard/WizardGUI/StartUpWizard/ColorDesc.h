#ifndef COLOR_DESC_H
#define COLOR_DESC_H

class TColorDesc
{
	unsigned char R, G, B;
public:
	TColorDesc(unsigned char Red, unsigned char Green, unsigned char Blue);
	~TColorDesc(void);

	unsigned char GetRed() {
		return R;
	}

	unsigned char GetGreen() {
		return G;
	}

	unsigned char GetBlue() {
		return B;
	}

};

#endif
