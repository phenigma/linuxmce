#include "GenerateWizardConfigDefaults.h"

#include "GUIWizardUtils.h"

#include "ConfigureCommons.h"

#include "Wizard.h"

#include <iostream>

GenerateWizardConfigDefaults::GenerateWizardConfigDefaults()
{
	FontText = DEFAULT_FONT_TEXT;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontText"))
		FontText = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontText");

	FontTitle = DEFAULT_FONT_STEP;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontTitle"))
		FontTitle = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontTitle");

	FontMiniTitle = DEFAULT_FONT_MINITITLE;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontMiniTitle"))
		FontMiniTitle = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontMiniTitle");


	FontName = FontText;
}

GenerateWizardConfigDefaults::~GenerateWizardConfigDefaults()
{
}

void GenerateWizardConfigDefaults::GenerateDefaults(std::string FolderToSave)
{
	if(FolderToSave.length())
	{
#ifdef WIN32
		if(FolderToSave[FolderToSave.length()-1] != '\\')
			FolderToSave += "\\";
#else
		if(FolderToSave[FolderToSave.length()-1] != '/')
			FolderToSave += "/";
#endif
	}

	SettingsDictionaryTree* Settings, *Options;
	SettingsDictionaryTree* PageDescription;
	SettingsDictionary* Dictionary;
	Settings = new SettingsDictionaryTree();

	Settings->GetDictionary()->SetName("AVWizard");
	Settings->GetDictionary()->SetType("Config_File");
	Settings->GetDictionary()->Set("Version", "0.0.1 alpha");

	PageNames[0] = "Welcome.xml";
	PageNames[1] = "VideoRatio.xml";
	PageNames[2] = "VideoOutput.xml";
	PageNames[3] = "VideoResolution.xml";
	PageNames[4] = "AudioConnector.xml";
	PageNames[5] = "AudioVolume.xml";
	PageNames[6] = "DolbyTest.xml";
	PageNames[7] = "DTSTest.xml";
	PageNames[8] = "FinalSelections.xml";

	std::string TmpPrefix = "/tmp/";
	for(int i = 0; i < 9; i++)
		PageNames[i] = TmpPrefix + PageNames[i];

	Options = new SettingsDictionaryTree();

	for(int i = 0; i<8; i++)
	{
		PageDescription = new SettingsDictionaryTree();
		Dictionary = PageDescription->GetDictionary();

		Dictionary->SetName("ScreenStep"+Utils::Int32ToString(i+1));
		Dictionary->SetType("Screen");
		Dictionary->Set("FileName", PageNames[i].c_str());
		Dictionary->Set("Index", i);

		Options->AddChild(PageDescription);
	}

	Dictionary = Options->GetDictionary();
	Dictionary->SetName("Screens");
	Dictionary->SetType("Pages");

	Settings->AddChild(Options);

	delete Settings;
}


void GenerateWizardConfigDefaults::GenerateDefaultPages(std::string FolderToSave, std::string ImageFolder, std::string FontFolder)
{
	if(FolderToSave.length())
	{
#ifdef WIN32
		if(FolderToSave[FolderToSave.length()-1] != '\\')
			FolderToSave += "\\";
#else
		if(FolderToSave[FolderToSave.length()-1] != '/')
			FolderToSave += "/";
#endif
	}

	if(ImageFolder.length())
	{
#ifdef WIN32
		if(ImageFolder[ImageFolder.length()-1] != '\\')
			ImageFolder += "\\";
#else
		if(ImageFolder[ImageFolder.length()-1] != '/')
			ImageFolder += "/";
#endif
	}

	
	if(FontFolder.length())
	{
#ifdef WIN32
		if(FontFolder[FontFolder.length()-1] != '\\')
			FontFolder += "\\";
#else
		if(FontFolder[FontFolder.length()-1] != '/')
			FontFolder += "/";
#endif
	}

	GeneratePage0(FolderToSave, ImageFolder, FontFolder);
	GeneratePage1(FolderToSave, ImageFolder, FontFolder);
	GeneratePage2(FolderToSave, ImageFolder, FontFolder);
	GeneratePage3(FolderToSave, ImageFolder, FontFolder);
	GeneratePage4(FolderToSave, ImageFolder, FontFolder);
	GeneratePage5(FolderToSave, ImageFolder, FontFolder);
	GeneratePage6(FolderToSave, ImageFolder, FontFolder);
	GeneratePage7(FolderToSave, ImageFolder, FontFolder);
	GeneratePage8(FolderToSave, ImageFolder, FontFolder);
}


void GenerateWizardConfigDefaults::GeneratePage0(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionary* Dictionary;

	Page = CreateControlPage("WelcomeStep", 640, 480, true, "AVWizard Welcome");
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Page->AddChild(GenerateTabContainer(-1, ImageFolder, FontFolder));


//	Page->AddChild(CreateControlImage("BackImage", ImageFolder+"background.png", 0, 52));
/*
	std::string LabelCaption = "WELCOME TO SYSTEM";

	SetFontStyle(24, "000000", "Regular");
	Page->AddChild(CreateControlLabel(
		"PageDesc", 
		LabelCaption,
		194, 102,
		"Left"
		));

	LabelCaption = "CONFIGURATION WIZARD";

	SetFontStyle(24, "000000", "Regular");
	Page->AddChild(CreateControlLabel(
		"PageDesc2", 
		LabelCaption,
		194, 128,
		"Left"
		));
*/
	std::string StringList[4];
	StringList[0] = "From here you can set up the audio & video for your home";
	StringList[1] = "For selections use \"UP/DOWN/LEFT/RIGHT\" arrows";
	StringList[2] = "For validation use \"Enter\"";
	StringList[3] = "IF you want to go back one step user \"Escape\"";

	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<4; i++)
	{
		Page->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			200+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlButton(
		"MainBtn",
		" Continue ",
		555,
		440,
		true
	));



	Page->SaveToXMLFile(PageNames[0]);
}

void GenerateWizardConfigDefaults::GeneratePage1(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = CreateControlPage("ScreenStep1", 640, 480, true, "AVWizard Configurator");
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Container = GenerateTabContainer(0, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create an image control that shows a perfect square
	Container->AddChild(CreateControlImage("SquareImage", ImageFolder+"square.png", 175, 170));

	//Create an image control that shows a perfect circle
	Container->AddChild(CreateControlImage("CircleImage", ImageFolder+"circle.png", 320, 170));

	//Create text area
	std::string StringList[6];

	StringList[0] = "If the above objects are perfect square and circle, your TV has the tradi-";
	StringList[1] = "tional aspect ration, meaning 4:3 proportions.";
	StringList[2] = "If the above objects are wide and than they are high, namely a wide rectangle and";
	StringList[3] = "a wide oval, then you likely have a newer widescreen TV with a 16:9 proportions.";
	StringList[4] = "What type of TV do you have?";
	StringList[5] = "Press the cursor left for 4:3, and right for 16:9";


	SetFontStyle(14, "000000", "Regular");
	for(int i = 0; i<4; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				300+i*15,
				"Center"
			));
	}

	SetFontStyle(14, "000000", "Bold");
	Container->AddChild(CreateControlLabel(
		"QuestionText",
		StringList[4],
		320, 375,
		"Center"
		));

	SetFontStyle(13, "000000", "Regular");
	Container->AddChild(CreateControlLabel(
		"QuestionText",
		StringList[5],
		320, 391,
		"Center"
		));

	Page->AddChild(CreateControlButton(
		"Btn1",
		" 4:3 ",
		65,
		440,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		" 16:9 ",
		575,
		440,
		true
		));


	Page->SaveToXMLFile(PageNames[1]);
}

void GenerateWizardConfigDefaults::GeneratePage2(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Dictionary->Set("Picture", ImageFolder+"button_tex.png");


	Container = GenerateTabContainer(1, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[5];
	StringList[0] = "I have tried to automatically detect which video cable your TV is conected to.";
	StringList[1] = "If I got it right press the Enter key. Otherwise sue the left and right arrow to";
	StringList[2] = "select the correct video cable and connector.";
	StringList[3] = "Since you have a widescreen TV, it is recomended to";
	StringList[4] = "use component, VGA or DVI connectors.";

	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<3; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			200+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlButton(
			"Btn1",
			"Composite",
			100, 285,
			true
		));

	Page->AddChild(CreateControlButton(
			"Btn2",
			"S-Video",
			230, 285,
			true
		));

	Page->AddChild(CreateControlButton(
			"Btn3",
			"Component",
			355, 285,
			true
		));

	Page->AddChild(CreateControlButton(
			"Btn4",
			"VGA",
			468, 285,
			true
		));

	Page->AddChild(CreateControlButton(
			"Btn5",
			"DVI",
			540, 285,
			true
		));


	for(int i = 3; i<5; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				380+i*15-45,
				"Center"
			));
	}

	Page->SaveToXMLFile(PageNames[2]);
}

void GenerateWizardConfigDefaults::GeneratePage3(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(2, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[10];
	StringList[0] = "Here are the appropiate resolutions for your TV. The lowest resolution is on";
	StringList[1] = "the left, and the highest resolution is on the right. Use the left and the right arrows";
	StringList[2] = "to select the resolution you want to use. The higher the resolution, in other";
	StringList[3] = "words the futher right, the sharper the picture. However if you move to the";
	StringList[4] = "right and the picture dissapears, that probably means your TV is not able to";
	StringList[5] = "support that high of the resolution, and you should move to the left to select a";
	StringList[6] = "lower resolution again.";
	StringList[7] = "If you are an advanced user you can use the up and down arrow to adjust the";
	StringList[8] = "refresh rate. Use the left and right arrows to adjust the resoltion, and press";
	StringList[9] = "enter when you're satisfied with the results.";

	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<10; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				175+i*15,
				"Center"
			));
	}

	Page->AddChild(CreateControlListBox(
			"ListBox1",
			"640x480",
			320, 350,
			110, 335,
			false
		));

	Page->AddChild(CreateControlListBox(
		"ListBox2",
		"75 Hz",
		320, 390,
		110, 335,
		false
		));

	Page->SaveToXMLFile(PageNames[3]);
}

void GenerateWizardConfigDefaults::GeneratePage4(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(3, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[1];
	StringList[0] = "Which connector did you use?";

	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<1; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}


	Page->AddChild(CreateControlButton(
		"Btn1",
		"Analog Stereo",
		142, 285,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"SPDIF Coaxial",
		315, 285,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn3",
		"SPDIF Optical",
		490, 285,
		true
		));

	Page->SaveToXMLFile(PageNames[4]);
}

void GenerateWizardConfigDefaults::GeneratePage5(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(4, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[2];
	StringList[0] = "I am playing some music, press enter when you confirm that you can hear it";
	StringList[1] = "and the volume is adjusted at a confortable level.";

	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<2; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270,
		250
		));

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "left_on.png",
		220, 270
		));

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "right_on.png",
		410,
		270
		));


	SetFontStyle(40, "002900", "Regular");
	Page->AddChild(CreateControlLabel(
		"SpeakerVolumeTextShadow",
		"50%",
		321,
		351,
		"Center"
		));
	SetFontStyle(40, "009900", "Regular");
	Page->AddChild(CreateControlLabel(
		"SpeakerVolumeText",
		"50%",
		320,
		350,
		"Center"
		));

	Page->SaveToXMLFile(PageNames[5]);
}

void GenerateWizardConfigDefaults::GeneratePage6(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(5, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[3];
	StringList[0] = "Can your stereo play Dolby Digital?";
	StringList[1] = "I am playing some music in Dolby Digital 5.1. If you're able to hear the music,";
	StringList[2] = "press the right arrow, otherwise press the left arrow.";
	
	SetFontStyle(13, "000000", "Regular");
	for(int i = 0; i<1; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270,
		250
		));

	Page->AddChild(CreateControlButton(
		"Btn1",
		"No, I cannot hear it",
		155, 310,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"Yes, I can hear it",
		495, 310,
		true
		));

	Page->SaveToXMLFile(PageNames[6]);
}

void GenerateWizardConfigDefaults::GeneratePage7(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;


	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(6, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[3];
	StringList[0] = "Can your stereo play DTS?";
	StringList[1] = "I am playing some music in DTS. If you're able to hear the music, press the";
	StringList[2] = "right arrow, otherwise press the left arrow.";

	SetFontStyle(13, "000000", "Bold");
	Container->AddChild(CreateControlLabel(
		"DescribeText0",
		StringList[0],
		320, 175,
		"Center"
		));
	SetFontStyle(13, "000000", "Regular");
	for(int i = 1; i<3; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270,
		250
		));


	Page->AddChild(CreateControlButton(
		"Btn1",
		"No, I cannot hear it",
		155, 310,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"Yes, I can hear it",
		495, 310,
		true
		));

	Page->SaveToXMLFile(PageNames[7]);
}

void GenerateWizardConfigDefaults::GeneratePage8(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep8");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(7, ImageFolder, FontFolder);
	Page->AddChild(Container);


	//Create text area
	std::string StringList[5];
	StringList[0] = "You're all done. Here are your selections:";
	StringList[1] = "If you're satisfied with these settings, press enter and I begin creating a";
	StringList[2] = "new user experience for you that is optimized for your television and stereo.";
	StringList[3] = "This process can take up to 15 minutes, please be patient. If you want to";
	StringList[4] = "change the settings, press the menu button to start again.";

	SetFontStyle(16, "000000", "Italic");
	Container->AddChild(CreateControlLabel(
		"DescribeText0",
		StringList[0],
		320,
		180,
		"Center"
		));

	SetFontStyle(13, "000000", "Regular");
	for(int i = 1; i<5; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			335+i*15,
			"Center"
			));
	}

	SetFontStyle(15, "000000", "Regular");
	for(int i = 1; i<=8; i++)
	Container->AddChild(CreateControlLabel(
		"OptionText"+Utils::Int32ToString(i),
		Utils::Int32ToString(i) + ".",
		190,
		110+(i+5)*17,
		"Left"
		));

	Page->AddChild(CreateControlButton(
		"Btn1",
		"I do not agree",
		120,
		450,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"I agree",
		555,
		450,
		true
		));

	Page->SaveToXMLFile(PageNames[8]);
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::GenerateTabContainer(int NoSelectedTab, 
																		   std::string ImageFolder,
																		   std::string FontFolder)
{
	int i;
	char Buffer[1024];
	SettingsDictionaryTree* Result;
	SettingsDictionaryTree* Container;
	SettingsDictionaryTree* Control;
	SettingsDictionary* Dictionary = NULL;

	Result = new SettingsDictionaryTree();

	Container = new SettingsDictionaryTree();
	Dictionary = Container->GetDictionary();

	Dictionary->SetName("HeadProgress");
	Dictionary->SetType("Container");
	Dictionary->Set("Visible", 0);

	for(i = 0; i<8; i++)
	{
		Control = new SettingsDictionaryTree();
		Dictionary = Control->GetDictionary();
		

		if(i <= NoSelectedTab)
			snprintf(Buffer, sizeof(Buffer), "Step%dOn", i);
		else
			snprintf(Buffer, sizeof(Buffer), "Step%dOff", i);

		std::string ControlImageName = Buffer;

		std::string FileImageName;
		if(i <= NoSelectedTab)
			FileImageName = ImageFolder + "button_on.png";
		else
			FileImageName = ImageFolder + "button_off.png";

		Container->AddChild( CreateControlImage(ControlImageName, 
			FileImageName,
			5+i*79, 0
		));
	}

	Dictionary = Result->GetDictionary();
	Dictionary->SetType("Container");
	Dictionary->SetName("BaseFrame");
	Result->AddChild(Container);


	Result->AddChild(CreateControlImage("BackImage", ImageFolder+"background.png", 0, 52));


	std::string LabelCaption;
	switch(NoSelectedTab)
	{
		case 0:
			LabelCaption = "VIDEO RATIO";
			break;
		case 1:
			LabelCaption = "OUTPUT CONNECTOR";
			break;
		case 2:
			LabelCaption = "RESOLUTION AND REFRESH";
			break;
		case 3:
			LabelCaption = "AUDIO OUTPUT CONNECTOR";
			break;
		case 4:
			LabelCaption = "AUDIO VOLUME";
			break;
		case 5:
			LabelCaption = "DOLBY AUDIO TEST";
			break;
		case 6:
			LabelCaption = "DTS AUDIO TEST";
			break;
		case 7:
			LabelCaption = "FINAL SETUP";
			break;
		default:
			LabelCaption = "WELCOME TO AVWIZARD";
	};
	SetFontStyle(24, "000000", "Regular");
	Result->AddChild(CreateControlLabel(
		"PageDesc" + Utils::Int32ToString(i+1), 
		LabelCaption,
		194, 102,
		"Left"
		));

	LabelCaption = "STEP " + Utils::Int32ToString(NoSelectedTab+1);

	SetFontStyle(18, "009900", "Regular");
	Result->AddChild(CreateControlLabel(
		"PageDesc" + Utils::Int32ToString(i+1), 
		LabelCaption,
		194,
		133,
		"Left"
		)
	);

	FontName = FontTitle;

	for (i = 0; i< 8; i++)
	{
		std::string FontColor, Caption;
		Caption = "STEP " + Utils::Int32ToString(i+1);

		if (i == NoSelectedTab)
			FontColor = "000000";
		else
		if(i <= NoSelectedTab)
			FontColor = "AfCfAf";
		else
			FontColor = "A0C0A0";

		SetFontStyle(19, "808080", "Regular");
		Result->AddChild(CreateControlLabel(
				"PageTab" + Utils::Int32ToString(i+1),
				Caption, 
				46+i*79,
				7,
				"Center"
				));
		SetFontStyle(19, FontColor, "Regular");

		Result->AddChild(CreateControlLabel(
				"PageTab" + Utils::Int32ToString(i+1),
				Caption, 
				45+i*79,
				6,
				"Center"
				));
		
	}

	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"video_settings.png", 6, 35));
	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"audio_settings.png", 242, 35));
	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"final.png", 558, 35));

	FontName = FontMiniTitle;
	SetFontStyle(14, "FFFFFF", "BOLD");
	Result->AddChild(CreateControlLabel(
		"StepDefineVideo_INFO" + Utils::Int32ToString(i+1),
		"VIDEO SETTINGS", 
		130,
		37,
		"Center"
		));

	Result->AddChild(CreateControlLabel(
		"StepDefineAudio_INFO" + Utils::Int32ToString(i+1),
		"AUDIO SETTINGS", 
		400,
		37,
		"Center"
		));

	Result->AddChild(CreateControlLabel(
		"FINAL_INFO",
		"FINAL", 
		600,
		37,
		"Center"
		));

	FontName = FontText;

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlImage(std::string ControlName, std::string Picture, int Left, int Top)
{
	//Create an image control that shows a perfect square
	SettingsDictionaryTree* Result= new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Image");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Picture", Picture);

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlLabel(std::string ControlName, 
										   std::string Caption,
										   int Left, 
										   int Top,
										   std::string Align)
{
	SettingsDictionaryTree* Result= new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();
	Dictionary->SetName(ControlName);
	Dictionary->SetType("Label");

	Dictionary->Set("Caption", Caption); 

	Dictionary->Set("FontName", FontName); 
	Dictionary->Set("FontColor", Color);
	Dictionary->Set("FontHeight", FontHeight);
	Dictionary->Set("Style", Style);

	Dictionary->Set("Top", Top);
	Dictionary->Set("Left", Left);
	Dictionary->Set("Align", Align);

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlPage(std::string ControlName, 
	int Width,
	int Height,
	bool FullScreen,
	std::string Caption)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Page");
	Dictionary->Set("Width", Width);
	Dictionary->Set("Height", Height);
	if(FullScreen)
		Dictionary->Set("Fullscreen", 1);
	else
		Dictionary->Set("Fullscreen", 0);
	Dictionary->Set("Caption", Caption);

	return Result;
}

void GenerateWizardConfigDefaults::SetFontStyle(
		int FontHeight, 
		std::string Color,
		std::string Style
	)
{
	this->FontHeight = FontHeight;
	this->Style = Style;
	this->Color = Color;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlButton(
	std::string ControlName, 
	std::string Caption, 
	int Left, int Top,
	bool Expands)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Button");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);

	if(Expands)
		Dictionary->Set("Expands", 1);
	else
		Dictionary->Set("Expands", 0);
	
	Dictionary->Set("Caption", Caption);

	return Result;   
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlListBox(
	std::string ControlName, 
	std::string Caption, 
	int Left, int Top,
	int Width, int Height,
	bool Expands)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("ListBox");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Width", Width);
	Dictionary->Set("Height", Height);

	if(Expands)
		Dictionary->Set("Expands", 1);
	else
		Dictionary->Set("Expands", 0);

	Dictionary->Set("Caption", Caption);

	return Result;   
}



void GenerateWizardConfigDefaults::SetDefaultBtnImages(SettingsDictionary* Dictionary, std::string ImageFolder)
{
	Dictionary->Set("FontColor", "ffffff");
	Dictionary->Set("FontHeight", 17);
	Dictionary->Set("FontName", FontName); 
	Dictionary->Set("Picture", ImageFolder+"button_tex.png");
	Dictionary->Set("PictureLeft", ImageFolder+"button_left.png");
	Dictionary->Set("PictureRight", ImageFolder+"button_right.png");
	Dictionary->Set("PictureHigh ", ImageFolder+"button_high_tex.png");
	Dictionary->Set("PictureHighLeft", ImageFolder+"button_high_left.png");
	Dictionary->Set("PictureHighRight", ImageFolder+"button_high_right.png");

	std::cout<<ImageFolder+"button_high_right.png"<<std::endl;

	Dictionary->Set("PictureArrowLeft", ImageFolder+"left_off.png");
	Dictionary->Set("PictureArrowRight", ImageFolder+"right_off.png");
	Dictionary->Set("PictureHighArrowLeft", ImageFolder+"left_on.png");
	Dictionary->Set("PictureHighArrowRight", ImageFolder+"right_on.png");

}
