#ifndef ConfigureCommons_H_
#define ConfigureCommons_H_

#ifdef WIN32
#define DEFAULT_FONT_NAME "c:\\pluto\\src\\avwizard\\wizardgui\\StartUpWizard\\Fonts\\Zurchbc.ttf"
#else
//#define DEFAULT_FONT_NAME "/usr/share/fonts/truetype/kochi/kochi-gothic.ttf"
#define DEFAULT_FONT_STEP "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
#define DEFAULT_FONT_MINITITLE "/usr/share/fonts/truetype/msttcorefonts/Trebuchet_MS.ttf"
#define DEFAULT_FONT_TEXT "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf"
#endif

#define COMMAND_SET_RESOLUTION "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
#define COMMAND_SET_RESOLUTION_PLUS  COMMAND_SET_RESOLUTION " resolution+"
#define COMMAND_SET_RESOLUTION_MINUS COMMAND_SET_RESOLUTION " resolution-"

#define COMMAND_SET_REFRESH "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
#define COMMAND_SET_REFRESH_PLUS  COMMAND_SET_REFRESH " refresh+"
#define COMMAND_SET_REFRESH_MINUS COMMAND_SET_REFRESH " refresh-"

#define COMMAND_SET_RATIO "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
#define COMMAND_SET_RATIO_4_3  COMMAND_SET_RATIO " 4_3"
#define COMMAND_SET_RATIO_16_9 COMMAND_SET_RATIO " 16_9"

#define COMMAND_SET_AUDIO_CONNECTOR "/usr/pluto/bin/AVWizard_AudioConnector.sh"


#ifdef WIN32
#define PATH_PIXMAPS "wiz_pixmaps"
#else
#define PATH_PIXMAPS "/usr/pluto/wiz_pixmaps"
#endif

//#define PATH_PIXMAPS "/home/ciplogic/SVN/src/AVWizard/WizardGUI/StartUpWizard/wiz_pixmaps"

#endif
