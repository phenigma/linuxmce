#ifndef ConfigureCommons_H_
#define ConfigureCommons_H_

#ifdef WIN32
#define DEFAULT_FONT_NAME "c:\\pluto\\src\\avwizard\\wizardgui\\StartUpWizard\\Fonts\\Zurchbc.ttf"
#else
//#define DEFAULT_FONT_NAME "/usr/share/fonts/truetype/kochi/kochi-gothic.ttf"
#define DEFAULT_FONT_NAME "/usr/share/fonts/truetype/msttcorefonts/Georgia.ttf"
#endif

#define COMMAND_SET_RESOLUTION "UpdateResolution.sh"
#define COMMAND_SET_RESOLUTION_PLUS  COMMAND_SET_RESOLUTION " resolution+"
#define COMMAND_SET_RESOLUTION_MINUS COMMAND_SET_RESOLUTION " resolution-"

#define COMMAND_SET_REFRESH "UpdateResolution.sh"
#define COMMAND_SET_REFRESH_PLUS  COMMAND_SET_REFRESH " refresh+"
#define COMMAND_SET_REFRESH_MINUS COMMAND_SET_REFRESH " refresh-"


#endif
