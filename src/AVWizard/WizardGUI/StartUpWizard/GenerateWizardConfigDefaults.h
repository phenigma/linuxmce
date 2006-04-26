#ifndef GenerateWizardConfigDefaults_H_
#define GenerateWizardConfigDefaults_H_

#include <string>

#include "SettingsDictionaryTree.h"

class GenerateWizardConfigDefaults
{
	/**
	 *	Path to font name
	 */
	std::string FontName; 
	int FontHeight;
	std::string Color; 
	std::string Style;
	/**
	 *	File of XML files that stores the default dialogs
	 */
	std::string PageNames[8];
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage1(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 2 of the wizard: VideoRatio.xml
	 */
	void GeneratePage2(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage3(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage4(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage5(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage6(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage7(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage8(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate container with tabs and highlight the specified item
	 *	Too generates text items
	 */
	SettingsDictionaryTree* GenerateTabContainer(int NoSelectedTab, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate image control with minimum parameters
	 */
	SettingsDictionaryTree* CreateControlImage(std::string ControlName, 
		std::string Picture, 
		int Left, 
		int Top);
	/**
	 *	Generate label control with minimum parameters
	 */
	SettingsDictionaryTree* CreateControlLabel(std::string ControlName, 
		std::string Caption, 
		int Left, 
		int Top,
		std::string Align);

	SettingsDictionaryTree* CreateControlPage(std::string ControlName, 
		int Width, int Height,
		bool FullScreen,
		std::string Caption);

	/**
	 *	Adds to a specified page's dictionary default images for buttons
	 */
	void SetDefaultBtnImages(SettingsDictionary* Dictionary, std::string ImageFolder);


	/**
	 *	Setup the current font, is used to set up current font style for controls
	 */
	void SetFontStyle(
		int FontHeight, 
		std::string Color,
		std::string Style
		);

	/**
	 *	Setup the name of the current font
	 */
	void SetFontName(std::string FontName);

	/**
	 *	Creates a definition for one button
	 */
	SettingsDictionaryTree* CreateControlButton(
		std::string ControlName, 
		std::string Caption, 
		int Left, int Top,
		bool Expands);

	SettingsDictionaryTree* CreateControlListBox(
		std::string ControlName, 
		std::string Caption, 
		int Left, int Top,
		int Width, int Height,
		bool Expands);
public:
	GenerateWizardConfigDefaults(void);
	virtual ~GenerateWizardConfigDefaults(void);

	void GenerateDefaults(std::string FolderToSave = "");

	void GenerateDefaultPages(
		std::string FolderToSave = "", 
		std::string ImageFolder = "", 
		std::string FontFolder = "");
};

#endif
