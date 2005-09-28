#ifndef __xxProxy_Orbiter_H__
#define __xxProxy_Orbiter_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 

#include <SDL.h>
//-----------------------------------------------------------------------------------------------------
#include "../SDL/OrbiterSDL.h"
//-----------------------------------------------------------------------------------------------------

class BDCommandProcessor;

//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class xxProxy_Orbiter : public OrbiterSDL
	{
		int   m_ImageQuality;

		BDCommandProcessor *m_pBDCommandProcessor;

	public:
		xxProxy_Orbiter(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress);
		virtual ~xxProxy_Orbiter();

		// Public virtual methods
		virtual void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);

		virtual void BeginPaint();
		virtual void EndPaint();

		virtual void SimulateMouseClick(int x, int y);
		virtual void SimulateKeyPress(long key);

		virtual void OnReload();
		virtual bool GetConfig();

		virtual void SetImageQuality(unsigned long nImageQuality);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif //__xxProxy_Orbiter_H__ 
