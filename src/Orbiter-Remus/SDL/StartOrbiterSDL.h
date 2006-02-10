#if !defined(__START_ORBITER_SDL_H__)
#define __START_ORBITER_SDL_H__

#include "OrbiterSDL.h"

struct keyboardState
{
  keyboardState()
    : bShiftDown(false)
      , bControlDown(false)
      , bAltDown(false)
      , bRepeat(false)
      , bCapsLock(false)
      , cKeyDown(0)
    {
    }
  bool bShiftDown;
  bool bControlDown;
  bool bAltDown;
  bool bRepeat;
  bool bCapsLock;
  clock_t cKeyDown;
};

struct SDL_Event_Loop_Data
{
  OrbiterSDL * pOrbiter;
  SDL_Event event;
  Orbiter::Event orbiterEvent;
  keyboardState kbdState;
};

OrbiterSDL * CreateOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode, int Width, int Height, bool bFullScreen);

bool SDL_Event_Process(SDL_Event_Loop_Data &sdl_event_loop_data);
bool SDL_Event_Loop_End(SDL_Event_Loop_Data &sdl_event_loop_data);

#endif
// __START_ORBITER_SDL_H__
