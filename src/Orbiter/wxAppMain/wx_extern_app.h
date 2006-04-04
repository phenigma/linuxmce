//
// Author : C Remus
//
// Changed by : ...
//

#ifndef _WX_EXTERN_APP_H_
#define _WX_EXTERN_APP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_extern_app.h"
#endif

#include "wx_thread_cmd.h"

#ifdef USE_RELEASE_CODE
class SDL_App_Object;
#endif // USE_RELEASE_CODE

class ExternApp : public wxThread_Cmd
{
public:
    ExternApp(int argc, char *argv[]);
    ~ExternApp();

    void Run();

    bool Create();
    bool EventLoop();
    void Destroy();

protected:
    int argc;
    char **argv;

#ifdef USE_RELEASE_CODE
public:
    SDL_App_Object *v_pSDL_App_Object;
#endif // USE_RELEASE_CODE
};

struct Extern_Event_Data
{
    Extern_Event_Data()
            : nEventId(0)
            , nButtonId(0)
        {
        }
    unsigned int nEventId;
    int nButtonId;
};

void Extern_Event_Listener();
void Extern_Event_Response(Extern_Event_Data *pExtern_Event_Data);

#endif
// _WX_EXTERN_APP_H_
