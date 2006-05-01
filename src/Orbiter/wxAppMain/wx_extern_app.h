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
#include "wx_extern_helpers.h"

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

void Extern_Task_Listener();
void Extern_Task_Response(Extern_Task_Data *pExtern_Task_Data);

#endif
// _WX_EXTERN_APP_H_
