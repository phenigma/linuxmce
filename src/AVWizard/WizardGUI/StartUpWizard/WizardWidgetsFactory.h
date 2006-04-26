/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------
#ifndef WizardWidgetsFactoryH
#define WizardWidgetsFactoryH
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#define WIDGET_PAGE			1
#define WIDGET_IMAGE		2
#define WIDGET_LISTBOX		3
#define WIDGET_LABEL		4
#define WIDGET_BUTTON		5
#define WIDGET_CONTAINER	6
//---------------------------------------------------------------------------
#include <map>
#include <string>
//---------------------------------------------------------------------------
#include "SettingsDictionaryTree.h"
//---------------------------------------------------------------------------
class WizardWidgetsFactory
{
protected:
	static WizardWidgetsFactory* Instance;
	std::map<std::string, int> WidgetTypes;

	SDLFrontEnd* FrontEnd;


    // Private constructor to make a singleton class
	WizardWidgetsFactory();
public:
	/**
	 *	Factory method
	 */
	WizardWidgetBase* CreateWidget(int ID, std::string Name);

	/**
	 *	XML based dialog
	 */
	WizardWidgetBase* CreateWidget(SettingsDictionaryTree* DialogSource);


	/**
	 *	Get the singleton instance
	 */
	static WizardWidgetsFactory* GetInstance();

	/**
	 *	Destroy attached resources to the singleton
	 */
	void CleanUp();

	/**
	 *	Get based it's type ID from specified TypeName
	 */
	int GetWidgetType(std::string TypeName);

	/**
	 *	Set the SDL FrontEnd that will have drawing routines to screen
	 */
	void SetSDLFrontEnd(SDLFrontEnd* FrontEnd);
};
//---------------------------------------------------------------------------
#endif
