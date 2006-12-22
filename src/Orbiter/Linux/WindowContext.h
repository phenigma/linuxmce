#ifndef __WINDOW_CONTEXT_H__
#define __WINDOW_CONTEXT_H__

#include "SerializeClass/ShapesColors.h"
#include "utilities/linux/window_manager/WMController/WindowLayer.h"
#include <map>
//-----------------------------------------------------------------------------------------------------
class WindowContext
{
public:

	WindowContext(string sWindowName);
	~WindowContext();

	WindowLayer Layer();
	void Layer(WindowLayer layer);

	bool IsMaximized();
	void Maximize(bool bValue);

	bool IsFullScreen();
	void FullScreen(bool bValue);

	bool IsVisible();
	void Visible(bool bValue);

	bool IsActivated();
	void Activate(bool bValue);

	PlutoRectangle Position();
	void Position(PlutoRectangle rectangle);

	string ToString();

	WindowContext& operator=(const WindowContext& context);
	friend bool operator==(const WindowContext& context1, const WindowContext& context2);
	friend bool operator!=(const WindowContext& context1, const WindowContext& context2);

private:

	WindowContext() {}

	string m_sWindowName;
	WindowLayer m_WindowLayer;
	bool m_bMaximized;
	bool m_bFullScreen;
	bool m_bVisible;
	bool m_bActivated;
	PlutoRectangle m_rectPosition;
};
//-----------------------------------------------------------------------------------------------------
#endif //__WINDOW_CONTEXT_H__
