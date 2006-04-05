#include "MouseBehavior_Linux.h"

#include "SDL.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

//-----------------------------------------------------------------------------------------------------

MouseBehavior_Linux::MouseBehavior_Linux(Orbiter *pOrbiter)
        : MouseBehavior(pOrbiter)
{
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
	MouseBehavior::SetMousePosition(X,Y);
    SDL_WarpMouse(X,Y);
}
