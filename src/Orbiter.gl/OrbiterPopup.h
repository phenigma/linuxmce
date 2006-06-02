#pragma once
//--------------------------------------------------------------------------------------------------------------
#include "CallBackTypes.h"
#include "dialog_types.h"
#include "../SerializeClass/ShapesColors.h"
//--------------------------------------------------------------------------------------------------------------
class PopupCallBackData;
class Task;
//--------------------------------------------------------------------------------------------------------------
class OrbiterPopup
{
public:
	OrbiterPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData);
	virtual ~OrbiterPopup(void);

	virtual Task *GetTask();

protected:
	E_DIALOG_TYPE m_aDialogType;
	unsigned long m_ulPopupID;
	PlutoRectangle m_rect;
};
//--------------------------------------------------------------------------------------------------------------
