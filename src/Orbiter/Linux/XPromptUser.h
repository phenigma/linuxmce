#ifndef __XPROMPTUSER_H
#define __XPROMPTUSER_H

#include "X3DWindow.h"
#include "X3DButton.h"

#include <string>
#include <map>
#include <list>

using namespace std;
/**
@author Ioan Marius Curelariu
*/
namespace DCE {

class XPromptUser: public X3DWindow
{
public:
	enum BtnPlacement {BTN_HORIZ, BTN_VERT};
	
protected:
	string m_sPrompt;
	map<int, string> *m_pMapPrompts;
	
	Font m_textFont, m_btnFont;
	int m_nTextPosX, m_nTextPosY, m_nTextHeight;
	map<int, X3DButton* > m_mapButtons;

	int m_nUserOption;
	time_t m_tTimeout;

private:
	const int m_nSpaceBetweenLines;
	bool m_bFreeTextFont, m_bFreeBtnFont;
	BtnPlacement m_btnPlacement;
	
	void UnloadFonts();

	bool EventLoop();

public:
	string m_wndName;
	XPromptUser(string sPrompt, map<int, string> *pMapPrompts=NULL);
	virtual ~XPromptUser();

	inline void SetFonts(Font textFont, Font btnFont);
	inline bool GetPromptTextSizeHint(string sText, int &width, int &height);
	inline bool GetBtnTextSizeHint(string sText, int &widht, int &height);
	int GetUserOption(){ return m_nUserOption; }

	int CreateWindow(Display *pDisplay, int nScreen, Window wndParent, int x, int y, int cx, int cy);
	bool ShowWindow(bool bShow=true);
	bool DrawWindow();
	
	void SetButtonPlacement(BtnPlacement placement);

	void Init();
	int RunModal();
	void DeInit();

private:
	list<string> SplitTextInLines(string sText);
	bool GetTextSizeHint(string sText, Font font, int &width, int &height);
};

}	// namespace DCE

#endif	//__XPROMPTUSER_H

