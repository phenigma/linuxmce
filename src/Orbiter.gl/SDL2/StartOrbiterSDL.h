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

// initialize OS stuff
// application-specific code
// exit if this fails
bool Init_System();

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

class SDL_App_Object
{
public:
    SDL_App_Object(int argc, char *argv[]);
    ~SDL_App_Object();

    bool Run(); // load-config, create, event-loop, destroy

    bool LoadConfig();

    bool Create();
    bool EventProcess();
    void Destroy();

    int GetExitCode() const;
    void SetExitCode(int nExitCode);

    SDL_Event_Loop_Data *m_pSDL_Event_Loop_Data;
protected:
    int argc;
    char **argv;
    int m_nExitCode;
};

#if (! USE_WX_LIB)
int main(int argc, char *argv[]);
#endif // (! USE_WX_LIB)

#endif
// __START_ORBITER_SDL_H__
