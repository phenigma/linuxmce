#include "PopupCollection.h"

#include "Orbiter.h"

#include "Mesh/MeshBuilder.h"
#include "Mesh/MeshFrame.h"
#include "Texture/TextureManager.h"
#include "OpenGL3DEngine.h"

PopupCollection::PopupCollection(OpenGL3DEngine* Engine)
: Scene(NULL)
{
	Current = "";
	this->Engine = Engine;
}

PopupCollection::~PopupCollection(void)
{
	std::map<std::string, PopupDescription* >::iterator Item = Popups.begin(),
		End = Popups.end();
	for(; Item != End; ++Item)
	{
		delete (Item->second);
	}
	Popups.clear();
}


void PopupCollection::HidePopups()
{
	Current = "";
}

void PopupCollection::HidePopup(std::string ID)
{
	g_pPlutoLogger->Write(LV_STATUS, "PopupCollection::HidePopup %s", ID.c_str());

	PopupDescription* Item;
	if (!Exists(ID))
	{
	}
	Item = Popups[ID];
	if(Item)
		Item->Hide();
}

void PopupCollection::PaintPopup(std::string ID, PlutoPopup *Popup)
{
	PopupDescription* Item;
	if(Current == ID)
		return;

	if(Current!= "")
	{
		Item = Popups[Current];
		if(Item)
			Item->Hide();
	}		

	Current = ID;
	
	if (Exists(ID))
	{
		Item = Popups[ID];
		if(Item)
			Item->Show();
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "Added in map popup %s, now size is %d", ID.c_str(), Popups.size());

		
		Engine->StartPopupDrawing();

		Popup->m_pObj->RenderObject(Popup->m_pObj, Popup->m_Position); 
		Item = new PopupDescription(Engine, ID);
		Popups[ID] = Item;
	}
	
}

bool PopupCollection::Exists(std::string ID)
{
	bool Result = Popups.find(ID) != Popups.end();
	g_pPlutoLogger->Write(LV_WARNING, "Searching in map (size = %d): %s and the result is: %d ", 
		Popups.size(),
		ID.c_str(), 
		Result);
	return Result; 
}

void PopupCollection::ChangeScene(MeshFrame* Scene)
{
	HidePopups();
	this->Scene = Scene;
}
