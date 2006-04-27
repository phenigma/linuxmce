#ifndef __MouseBehavior_Linux_H__
#define __MouseBehavior_Linux_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "../MouseBehavior.h"
using namespace DCE;
#include "OrbiterLinux.h"

namespace DCE
{
    //-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior in a Linux environment
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior_Linux : public MouseBehavior
	{
	public:
		MouseBehavior_Linux(Orbiter *pOrbiter);
		virtual void SetMousePosition(int X,int Y);
		virtual void ShowMouse(bool bShow);
    protected:
        class OrbiterLinux *m_pOrbiterLinux;
	};
}
//-----------------------------------------------------------------------------------------------------

#endif
// __MouseBehavior_Linux_H__
