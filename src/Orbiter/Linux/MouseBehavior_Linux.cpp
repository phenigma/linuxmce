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
    m_pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
}

void MouseBehavior_Linux::SetMousePosition(int X,int Y)
{
	MouseBehavior::SetMousePosition(X,Y);
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_LockDisplay();
	// this is crashing with: Xlib: unexpected async reply (sequence 0x273)!
    // should not crash with X-locking code, not yet tested
    //SDL_WarpMouse(X,Y);
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_UnlockDisplay();
}

void MouseBehavior_Linux::ShowMouse(bool bShow)
{
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_LockDisplay();
	SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
    if (m_pOrbiterLinux)
        m_pOrbiterLinux->X_UnlockDisplay();
}
