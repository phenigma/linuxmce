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
}

void PopupDescription::Hide()
{
	g_pPlutoLogger->Write(LV_WARNING, "Remove popup: %s", ObjectHash.c_str());
	
	Engine->RemoveMeshFrameFromDesktopForID(ObjectHash);
}

void PopupDescription::Show()
{
	g_pPlutoLogger->Write(LV_WARNING, "Add popup: %s to scene", ObjectHash.c_str());
	Engine->AddMeshFrameToDesktop("", PopupFrame);
}
