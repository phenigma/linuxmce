#ifndef ShapesColor_h
#define ShapesColor_h

#include "SerializeClass/SerializeClass.h"

class PlutoColor : public SerializeClass
{
public:
	PlutoColor() {}
	PlutoColor(long Value) { m_Value=Value; }
	PlutoColor(unsigned char R,unsigned char G,unsigned char B) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=255; };
	PlutoColor(unsigned char R,unsigned char G,unsigned char B, unsigned char A) { unsigned char *ucp = (unsigned char *) &m_Value;  ucp[0]=B; ucp[1]=G; ucp[2]=R; ucp[3]=A; };
	PlutoColor(const PlutoColor &C) {m_Value=C.m_Value;  }

	static PlutoColor Black() { return PlutoColor(0,0,0); }
	static PlutoColor Red() { return PlutoColor(255,0,0); }
	static PlutoColor Green() { return PlutoColor(0,255,0); }
	static PlutoColor Blue() { return PlutoColor(0,0,255); }
	static PlutoColor White() { return PlutoColor(255,255,255); }
	long m_Value;
	long AsARGB() { return m_Value; };
	int A() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[3]; };
	int R() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[2]; };
	int G() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[1]; };
	int B() { unsigned char *ucp = (unsigned char *) &m_Value;  return ucp[0]; };

	void A(unsigned char A) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[3]=A; };
	void R(unsigned char R) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[2]=R; };
	void G(unsigned char G) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[1]=G; };
	void B(unsigned char B) { unsigned char *ucp = (unsigned char *) &m_Value; ucp[0]=B; };
	virtual string SerializeClassClassName() { return "PlutoColor"; }
	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_Value;
	}
};

class PlutoPoint : public SerializeClass
{
public:
	int X,Y;
	PlutoPoint() { X=0; Y=0;  }
	PlutoPoint(PlutoPoint *p) { X=p->X; Y=p->Y;  };
	PlutoPoint(int x,int y) {X=x; Y=y;  }
	PlutoPoint(const PlutoPoint &p) {X=p.X; Y=p.Y;  }

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + X + Y;
	}
	virtual string SerializeClassClassName() { return "PlutoPoint"; }

	PlutoPoint operator+ (PlutoPoint &p) { return PlutoPoint(X + p.X,Y + p.Y); }
};

class PlutoSize : public SerializeClass
{
public:
	int Width,Height;
	PlutoSize() {Width=0; Height=0; };
	PlutoSize(const PlutoSize &s) {Width=s.Width; Height=s.Height; };
	PlutoSize(PlutoPoint &pt)
	{
		Width=pt.X;
		Height=pt.Y;
		
	}
	PlutoSize(PlutoPoint *pt)
	{
		Width=pt->X;
		Height=pt->Y;
		
	}
	PlutoSize(int W,int H) { Width=W; Height=H;  }

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + Width + Height;
	}
	virtual string SerializeClassClassName() { return "PlutoSize"; }
};

class PlutoRectangle : public SerializeClass
// Define our own rect class since there's no ANSI standard
// for rectangles.
{
public:
	PlutoRectangle() {X=Y=Width=Height=0; };
	PlutoRectangle(int dX, int dY, int dWidth, int dHeight) { X=dX; Y=dY; Width=dWidth; Height=dHeight;  } ;
	PlutoRectangle(int dX, int dY) { X=dX; Y=dY;  } ;
	PlutoRectangle(PlutoPoint pt,PlutoSize sz) { X=pt.X; Y=pt.Y; Width=sz.Width; Height=sz.Height; }
	PlutoRectangle(PlutoPoint origin,PlutoPoint corner) { X=origin.X; Y=origin.Y; Width=corner.X-origin.X; Height=corner.Y-origin.Y; }
	PlutoRectangle(const PlutoRectangle &Rect) { X = Rect.X; Y=Rect.Y; Width=Rect.Width; Height=Rect.Height; }
	
	int Top() { return Y; }
	int Bottom() { return Y + Height; }
	int Left() { return X; }
	int Right() { return X + Width; }

	void Bottom(int Bottom) { Height = Bottom-Y; }
	void Right(int Right) { Width = Right-X; }

	PlutoPoint Location() { return PlutoPoint(X,Y); }
	PlutoPoint Point() { return PlutoPoint(X,Y); }
	void Location(PlutoPoint p) { X=p.X; Y=p.Y; }
	PlutoSize Size() { return PlutoSize(Width,Height); }
	void Size(PlutoSize s) { Width=s.Width; Height=s.Height; }
	bool Contains(int dX, int dY) { return (dX >= X && dY >=Y && dX < (X+Width) && dY < (Y+Height)); };
	PlutoRectangle& operator=(const PlutoRectangle& Rect) { X = Rect.X; Y=Rect.Y; Width=Rect.Width; Height=Rect.Height;return *this; };
	PlutoRectangle operator+ (PlutoPoint &p) { return PlutoRectangle(X + p.X,Y + p.Y,Width,Height); }
	int X2() {return X+Width;};
	int Y2() {return Y+Height;};

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + X + Y + Width + Height;
	}

	bool IntersectsWith(PlutoRectangle &r)
	{
		if (((r.X >= X && r.X <= X2()) || (r.X2() >= X && r.X2() <= X2())) &&
			((r.Y >= Y && r.Y <= Y2()) || (r.Y2() >= Y && r.Y2() <= Y2())))
			return true;
		
		return false;
	}
	void Union(PlutoRectangle &RectToUnite)
	{
		PlutoRectangle NewRect;

		if (RectToUnite.X < X)
			NewRect.X = RectToUnite.X;
		else
			NewRect.X = X;

		if (RectToUnite.Y < Y)
			NewRect.X = RectToUnite.X;
		else
			NewRect.Y = Y;

		if (RectToUnite.X2() > X2())
			NewRect.Width = RectToUnite.X2() - NewRect.X;
		else
			NewRect.Width = X2() - NewRect.X;

		if (RectToUnite.Y2() > Y2())
			NewRect.Height = RectToUnite.Y2() - NewRect.Y;
		else
			NewRect.Height= Y2() - NewRect.Y;

		*this = NewRect;
	}

	static PlutoRectangle PlutoRectangle::Union(PlutoRectangle &r1,PlutoRectangle &r2)
	{
		return PlutoRectangle(
			PlutoPoint(min(r1.X,r2.X),min(r1.Y,r2.Y)),
			PlutoPoint(max(r1.Right(),r2.Right()),max(r1.Bottom(),r2.Bottom())) );
	}

	int X, Y, Width, Height;
	virtual string SerializeClassClassName() { return "PlutoRectangle"; }
};

#endif
