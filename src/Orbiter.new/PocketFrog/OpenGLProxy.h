#ifndef __OPENGL_PROXY__
#define __OPENGL_PROXY__

#include <WINDOWS.H>
#include <memory>
using namespace std;
//-----------------------------------------------------------------------------------------------------------
class OpenGLProxy
{
public:
	void Attach(HWND hWnd);
	void Detach();

	static OpenGLProxy *GetInstance();
	~OpenGLProxy();

private:
	void Reset();

	OpenGLProxy();

	HWND mhWnd;
	HDC mhDC;
	HGLRC mhRC;

	static auto_ptr<OpenGLProxy> m_psInstance;
};
//-----------------------------------------------------------------------------------------------------------
#endif
