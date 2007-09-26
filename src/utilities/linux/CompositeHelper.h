#ifndef __COMPOSITE_HELPER_H__
#define __COMPOSITE_HELPER_H__
//---------------------------------------------------------------------------------------------------------------
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>
//---------------------------------------------------------------------------------------------------------------
class CompositeHelper
{
private:

	Display *m_display;
	static CompositeHelper m_Instance;

	CompositeHelper();
	void FillTextProperty(XTextProperty* p, const char* text);

public:

	~CompositeHelper();
	static CompositeHelper& GetInstance() { return m_Instance; }
	Display *GetDisplay() { return m_display; }

	XVisualInfo *GetVisualForComposite();
	bool RegisterReplacementWindowForSDL(XVisualInfo *visinfo, int nWidth, int nHeight);
	bool UnregisterReplacementWindowForSDL();
};
//---------------------------------------------------------------------------------------------------------------
#endif //__COMPOSITE_HELPER_H__
