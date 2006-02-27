//
// C++ Interface: math3dutils
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MATH3DUTILS_H
#define MATH3DUTILS_H

/**
@author ciplogic
*/

#include "../../SerializeClass/ShapesColors.h"

/**
 * That class defines one rectangle structure which is defined as top,left coordinate
 * and it's width and height
 * That structure is used where Rect is not suitable for actual needs (because works with 
 * integer based types
 */
class FloatRect
{
public:
	float Top;
	float Left;
	float Width;
	float Height;
	
	FloatRect()
		: Top(0),
		  Left(0),
		  Width(1),
		  Height(1)
	{
	}
	
	FloatRect(float TopValue, float LeftValue, float WidthValue, float HeightValue) 
		: Top(TopValue),
		  Left(LeftValue),
		  Width(WidthValue),
		  Height(HeightValue) {
	}
	
	FloatRect(PlutoRectangle Rectangle)
		: Top((float)Rectangle.Y),
		  Left((float)Rectangle.X),  
		  Width((float)Rectangle.Width),
		  Height((float)Rectangle.Height) {
	}
	
	/**
	 * That function say ain intermediar rectangle from source rectangle
	 * and one another dest point using a given value 
	 * for Value = 0 the result is the source rectangle
	 * for Value = 1 the result is destination rectangle
	 * for Value = 0.5 the result is the middle of both rectangles
	 */
	FloatRect Interpolate(FloatRect DestRect, float Value);	
	
};

class FloatPoint
{
public:
	float X;
	float Y;
	FloatPoint(float ValueX, float ValueY) 
		: X(ValueX),
		  Y(ValueY)
	{
	}
	
	FloatPoint() 
		: X(0),
		  Y(0)
	{
	}
	/**
	 * That function say ain intermediar point from source point
	 * and one another dest point using a given value 
	 * for Value = 0 the result is the source point
	 * for Value = 1 the result is destination point
	 * for Value = 0.5 the result is the middle of both points
	 */
	
	FloatPoint Interpolate(FloatPoint DestPoint, float Value)
	{
		FloatPoint Result;
		
		Result.X = X*(1-Value)+DestPoint.X*Value;
		Result.Y = Y*(1-Value)+DestPoint.Y*Value;
		
		return Result;
	}
};

class T3DMatrix;

class T3DVertex {
public:
	/**
	 * 3D coordinate of one vertex
	 */	 
	float X, Y, Z;
public:
	/**
	 *        Moves a vector with a specified size
	 * @param ValueX The X axis size
	 * @param ValueY The Y axis size
	 * @param ValueZ The Z axis size
	 */
	void Translate(float ValueX, float ValueY, float ValueZ);
	
	/**
	 *        Scales (over the origin) one 3D Point
	 * @param ValueX The X axis size
	 * @param ValueY The X axis size
	 * @param ValueZ The X axis size
	 */
	void Scale(float ValueX, float ValueY, float ValueZ);
	friend class T3DMatrix;
	
	/**
	 *        Apply a custom transform (given by a transform matrix) to a point
	 * @param Matrix 
	 * @return 
	 */
	T3DVertex Transform(T3DMatrix& Matrix);
};

class T3DMatrix {
	
public:
	//keept public for performance reasons
	/**
	 * Matrix[4][4] stored in liniar way. Standard for 3D transforms
	 */
	 
	float Values[16];
	
	/**
	 * Default constructor that init the current matrix with 
	 * unit matrix (coresponing of No-Transform 3D matrix)
	 */
	T3DMatrix();
	~T3DMatrix();
	
	
	/**
	 *        Set the default transform (no transform) for 
	 *  current matrix
	 */
	void SetIdentity();
	
	/**
	 *        Set the curent matrix as translate matrix
	 * @param ValueX The size of X axis size translation
	 * @param ValueY The size of Y axis size translation
	 * @param ValueZ The size of Z axis size translation
	 */
	void SetTranslate(float ValueX, float ValueY, float ValueZ);
	
	/**
	 *        Set the curent matrix as a scale matrix
	 * @param ValueX The size of scaling over X axis
	 * @param ValueY The size of scaling over Y axis
	 * @param ValueZ The size of scaling over Z axis
	 */
	void SetScale(float ValueX, float ValueY, float ValueZ);
	
	/**
	 *        Set the curent matrix as a rotate over X axis
	 * @param Angle 
	 */
	void SetRotateX(float Angle);
	/**
	 *        Set the curent matrix as a rotate over Y axis
	 * @param Angle 
	 */
	void SetRotateY(float Angle);
	/**
	 *        Set the curent matrix as a rotate over Z axis
	 * @param Angle 
	 */
	void SetRotateZ(float Angle);
	
	/**
	 *        Apply the transforms of other matrixes on current matrix
	 * @param Matrix 
	 * @return 
	 */
	T3DMatrix Multiply(T3DMatrix Matrix);
	
	
	/**
	 *        Set up the current matrix as another matrix
	 * @param Matrix 
	 * @return 
	 */
	T3DMatrix operator = (T3DMatrix& Matrix);
};

/**
* Structure for a 3-dimensional point 
*/
typedef struct point_3d {						
	float x, y, z;
} POINT_3D;


class MathUtils
{
	public:
		/**
		* That function the minimum value greater or equal than given value which is power of two
		* @param Value 
		* @return 
		*/
		static int MinPowerOf2(int Value);
		static float InterpolateValues(float Source, float Dest, float Theta);
		static float InterpolateSinus(float Source, float Dest, float Theta);
		static const float _PI;

		static POINT_3D pointAdd(POINT_3D p, POINT_3D q);
		static POINT_3D pointTimes(float c, POINT_3D p);
		static POINT_3D makePoint(float a, float b, float c);

		static POINT_3D Bernstein(float u, POINT_3D *p);

};

#endif
