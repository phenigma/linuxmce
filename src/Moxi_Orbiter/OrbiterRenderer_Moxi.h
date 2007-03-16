/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __ORBITER_RENDERER_MOXI_H__
#define __ORBITER_RENDERER_MOXI_H__

#include "../Orbiter/SDL/OrbiterRenderer_SDL.h"

class OrbiterRendererFactory;

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_Moxi : public OrbiterRenderer_SDL
	{
	private:
		friend class ::OrbiterRendererFactory;
		OrbiterRenderer_Moxi(Orbiter *pOrbiter);

		bool DisplayProgress(string sMessage, int nProgress);
		bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
		int PromptUser(string sPrompt,int iTimeoutSeconds=10, map<int,string> *p_mapPrompts = NULL);
		int PromptFor(string sToken);

	public:

		~OrbiterRenderer_Moxi();

		void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
		void BeginPaint();
		void EndPaint();
	};
}

#endif // __ORBITER_RENDERER_MOXI_H__