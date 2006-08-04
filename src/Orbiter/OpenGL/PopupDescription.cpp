#include "PopupDescription.h"

#include "Orbiter.h"

#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

PopupDescription::PopupDescription(OpenGL3DEngine* Engine, string ID)
{
	this->Engine = Engine;
	ObjectID = ID;
	PopupFrame = Engine->EndPopupMode();
}

PopupDescription::~PopupDescription(void)
{
}

void PopupDescription::Hide()
{
	g_pPlutoLogger->Write(LV_WARNING, "Remove popup: %s", ObjectID.c_str());
	Engine->RemoveMeshFrameFromDesktop(PopupFrame);
}

void PopupDescription::Show()
{
	g_pPlutoLogger->Write(LV_WARNING, "Add the popup: %s", ObjectID.c_str());
	Engine->AddMeshFrameToDesktop(ObjectID, PopupFrame);
}
