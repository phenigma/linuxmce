#include "PopupDescription.h"

#include "Orbiter.h"

#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

PopupDescription::PopupDescription(OpenGL3DEngine* Engine, string ID, PlutoRectangle* HighLight)
{
	this->Engine = Engine;
	ObjectID = ID;
	PopupFrame = Engine->EndFrameDrawing();
}

PopupDescription::~PopupDescription(void)
{
}

void PopupDescription::Hide()
{
	g_pPlutoLogger->Write(LV_WARNING, "Remove popup: %s", ObjectID.c_str());
	
	//This might look like a hack! Need to re-think naming for popups.
	Engine->RemoveMeshFrameFromDesktop(PopupFrame);
	Engine->RemoveMeshFrameFromDesktopFromID(ObjectID);
}

void PopupDescription::Show()
{
	g_pPlutoLogger->Write(LV_WARNING, "Add popup: %s to scene", ObjectID.c_str());
	Engine->AddMeshFrameToDesktop("", ObjectID, PopupFrame);
}
