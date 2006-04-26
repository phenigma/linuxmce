/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WM_EVENT_H_
#define WM_EVENT_H_

#include <SDL.h>

/**
 * WMET = Window Message Event Type
 * Types that may generate one window, is used 
 * for future kind of events and maybe for multiple devices
 */
 
#define WMET_APPLICATION		1000
#define WMET_QUIT				1001

#define WMET_KEYS				2000
#define WMET_LEFT_KEY 			2001
#define WMET_RIGHT_KEY			2002
#define WMET_UP_KEY				2003
#define WMET_DOWN_KEY			2004

#define WMET_ENTER_KEY			2010
#define WMET_ESCAPE_KEY			3000


/**
 *	Helper class that converts SDL events 
 *	to generic WM_Event
 */
class WM_Event
{
public:
	int Type;

	/**
	 *	Set the current event as Quit Event
	 */
	void Quit();
	
	/**
	 *	Set the current event as left key event
	 */
	void LeftKey();

	/**
	 *	Set the current event as right key event
	 */
	void RightKey();	

	/**
	 *	Set the current event as Up key event
	 */
	void UpKey();	

	/**
	 *	Set the current event as Down key event
	 */
	void DownKey();	

	/**
	 *	Set the current event as Enter key event
	 */
	void EnterKey();	

	/**
	 *	Set the current event as Escape key event
	 */
	void EscapeKey();	

	/**
	 *	Get the event information based on SDL_Event
	 *	given as parameter
	 */
	void ConvertFromSDLEvent(SDL_Event& Event);	
};

#endif /*WM_EVENT_H_*/
