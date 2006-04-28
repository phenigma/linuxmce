#pragma once
//--------------------------------------------------------------------------------------------------------------
#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
using namespace cpp;
using namespace cpp::Threading;
//--------------------------------------------------------------------------------------------------------------
#include "CallBackTypes.h"
#include "PopupAnimationData.h"
#include "dialog_types.h"
//--------------------------------------------------------------------------------------------------------------
//forward declarations
class IPopupDialog;
class PopupCallBackData;
//--------------------------------------------------------------------------------------------------------------
class PopupManagerImpl
{
private:
	/*
	*	Returns a pointer to popup
	*/
	IPopupDialog *GetPopup(const unsigned long ulPopupID);

	//data
	unsigned long m_ulPopupCounter;
	map<unsigned long, E_DIALOG_TYPE> mapPopups;

public:
	PopupManagerImpl(void);
	~PopupManagerImpl(void);

	/*
	 *	Creates a popup of type 'aDialogType'
	 */
	bool CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData);
	bool CreatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData, unsigned long& ulPopupID);

	/*
	 *	Refreshes custom data for a popup
	 */
	bool RefreshPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData);
	bool RefreshPopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData);

	/*
	 *	Makes popup visible
	 */
	bool ShowPopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData = NULL);
	bool ShowPopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData = NULL);

	/*
	 *	Makes popup hidden
	 */
	bool HidePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData = NULL);
	bool HidePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData = NULL);

	/*
	 *	Closes the popup
	 */
	bool ClosePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData = NULL);
	bool ClosePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData = NULL);

	/*
	 *	Creates a custom animation with the popup
	 */
	bool AnimatePopup(E_DIALOG_TYPE aDialogType, PopupCallBackData *pCallBackData, 
		AnimationType aAnimationType, PopupAnimationData *pAnimationData);
	bool AnimatePopup(const unsigned long ulPopupID, PopupCallBackData *pCallBackData, 
		AnimationType aAnimationType, PopupAnimationData *pAnimationData);
};
//--------------------------------------------------------------------------------------------------------------
typedef Singleton<PopupManagerImpl, CreateUsingNew<PopupManagerImpl>, DefaultLifetime, MultiThreaded> PopupManager;
//--------------------------------------------------------------------------------------------------------------
