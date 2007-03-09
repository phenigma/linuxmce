/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "PopupDescription.h"

#include "Orbiter.h"

#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

PopupDescription::PopupDescription(OpenGL3DEngine* Engine, string ID, string ObjHash, PlutoRectangle* HighLight)
{
	this->Engine = Engine;
	ObjectID = ID;
	ObjectHash = ObjHash;
	PopupFrame = Engine->EndFrameDrawing(ObjHash);
}

PopupDescription::~PopupDescription(void)
{
	Engine->RemoveMeshFrameFromDesktop(PopupFrame);

	PopupFrame->CleanUp(true);
	delete PopupFrame;
	PopupFrame = NULL;
}

void PopupDescription::Hide()
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Remove popup: %s", ObjectHash.c_str());

	Engine->RemoveMeshFrameFromDesktopForID(ObjectHash);
}

void PopupDescription::Show()
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Add popup: %s to scene", ObjectHash.c_str());
	Engine->AddMeshFrameToDesktop("", PopupFrame);
}
