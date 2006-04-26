//---------------------------------------------------------------------------

#ifndef GUIWizardUtilsH
#define GUIWizardUtilsH
//---------------------------------------------------------------------------
#include <string>
#include "ColorDesc.h"

#ifdef WIN32
#define snprintf _snprintf 
#define snscanf _snscanf
#endif

class Utils
{
	public:
		static int StringToInt32(const char* String);
		static int StringToInt32(std::string String);
		static std::string Int32ToString(int Value);
		static char* CloneStr(const char* String);

		static char* CopyStr(const char* String, int StartPos, int Length);
		static char* AllTrim(const char* String);

		static TColorDesc* StringToColorDesc(std::string String);
};
//---------------------------------------------------------------------------
#endif
