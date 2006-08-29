/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef COLOR_DESC_H
#define COLOR_DESC_H

class TColorDesc
{
	unsigned char R, G, B;
public:
	TColorDesc(unsigned char Red, unsigned char Green, unsigned char Blue);
	virtual ~TColorDesc(void);

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
