#ifndef WM_EVENT_H_
#define WM_EVENT_H_

#include <SDL.h>

/**
 * WMET = Window Message Event Type
 */
 
#define WMET_APPLICATION		1000
#define WMET_QUIT				1001

#define WMET_KEYS				2000
#define WMET_LEFT_KEY 			2001
#define WMET_RIGHT_KEY			2002
#define WMET_UP_KEY				2003
#define WMET_DOWN_KEY			2004

class WM_Event
{
public:
	int Type;

	void Quit();
	
	void LeftKey();	
	void RightKey();	
	void UpKey();	
	void DownKey();	
	
	void ConvertFromSDLEvent(SDL_Event& Event);	
};

#endif /*WM_EVENT_H_*/
