/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//--------------------------------------------------------------
#ifndef WizardWidgetScrollList_H_
#define WizardWidgetScrollList_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
#include "ColorDesc.h"
//---------------------------------------------------------------------------
#include<string>
#include<vector>
//---------------------------------------------------------------------------
class WizardWidgetScrollList : public WizardWidgetBase
{
protected:	
	TColorDesc* BackColor;
	TColorDesc* BackFocusedColor;

	TColorDesc* HighBackColor;
	TColorDesc* HighBackFocusedColor;
	bool Focused;

	std::vector<std::string> ItemNames;
	std::vector<std::string> ItemValues;

	void WriteItem(std::string Name, bool Selected,
		int Index, int TopItemIndex, int BottomItemIndex, 
		int ItemHeight);
	int ItemIndex;
protected:
	WizardWidgetPage* GetContainerPage();
public:
	WizardWidgetScrollList(SDLFrontEnd* FrontEnd, std::string Name);
	~WizardWidgetScrollList();

	int GetItemIndex();
	void SetItemIndex(int ItemIndex);
	void SetItemIndex(std::string TextToSelect);
	
	void SetFocus(bool IsFocused);
	bool GetFocus();

	std::string GetSelectedValue();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

	/**
	 */
	virtual void AddItem(std::string ItemName, std::string ItemValue);
};

#endif
