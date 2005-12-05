#ifndef __CALLBACK_DATA_H__
#define __CALLBACK_DATA_H__

#include "SerializeClass/ShapesColors.h"
//-----------------------------------------------------------------------------------------------------
class CallBackData
{
public:
	CallBackData() {}
	virtual ~CallBackData() {}
};
//-----------------------------------------------------------------------------------------------------
class PositionCallBackData : public CallBackData
{
public:
	PlutoRectangle m_rectPosition;
};
//-----------------------------------------------------------------------------------------------------
class KeyCallBackData : public CallBackData
{
public:
	int m_nPlutoKey;
};
//-----------------------------------------------------------------------------------------------------
class MouseClickCallBackData : public CallBackData
{
public:
	int m_x, m_y;
};
//-----------------------------------------------------------------------------------------------------
#endif