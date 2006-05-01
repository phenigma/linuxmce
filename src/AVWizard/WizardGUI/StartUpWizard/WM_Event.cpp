#include "WM_Event.h"

void WM_Event::Quit()
{
	this->Type = WMET_QUIT;
}

void WM_Event::LeftKey()
{
	this->Type = WMET_LEFT_KEY;
}

void WM_Event::RightKey()
{
	this->Type = WMET_RIGHT_KEY;
}

void WM_Event::UpKey()
{
	this->Type = WMET_UP_KEY;	
}
	
void WM_Event::DownKey()
{
	this->Type = WMET_DOWN_KEY;	
}		

void WM_Event::EnterKey()
{
	this->Type = WMET_ENTER_KEY;	
}

void WM_Event::EscapeKey()
{
	this->Type = WMET_ESCAPE_KEY;
}

void WM_Event::Save()
{
	this->Type = WMET_SAVE;
}


void WM_Event::ConvertFromSDLEvent(SDL_Event& Event)
{
	switch(Event.type)
	{
		case SDL_QUIT:
			Quit();
			break;
		case SDL_KEYUP:
			switch(Event.key.keysym.sym)
			{
				case SDLK_UP:
					UpKey();
					break;
				case SDLK_DOWN:
					DownKey();
					break;
				case SDLK_LEFT:
					LeftKey();
					break;
				case SDLK_RIGHT:
					RightKey();
					break;
				case SDLK_ESCAPE:
					EscapeKey();
					break;
				case SDLK_RETURN:
					EnterKey();
					break;
				default:
					Type = 0;
			}
			break;
		default:
			Type = 0;
	}
}
