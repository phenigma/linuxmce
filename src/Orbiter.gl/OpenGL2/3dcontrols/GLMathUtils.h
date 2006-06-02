#ifndef GLMathUtils_H_
#define GLMathUtils_H_

class GLMathUtils
{
public:
	static const float _PI;

	GLMathUtils(void);
	~GLMathUtils(void);
	static float RadianToDegree(float Angle);
	static float DegreeToRadian(float Angle);
	
	static bool IsPowOf2(int Value);
};

#endif
