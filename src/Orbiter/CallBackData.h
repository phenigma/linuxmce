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
#endif