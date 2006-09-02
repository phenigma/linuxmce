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
	Reset();
}

void PopupCollection::Reset()
{
	Current = "";
	map<string, PopupDescription* >::iterator Item = Popups.begin(),
		End = Popups.end();
	for(; Item != End; ++Item)
	{
		delete (Item->second);
	}
	Popups.clear();
}


void PopupCollection::HidePopup(string ID, string ObjectHash)
{
	g_pPlutoLogger->Write(LV_CRITICAL, "PopupCollection::HidePopup %s", ObjectHash.c_str());

	PopupDescription* Item;
	Item = Popups[ObjectHash];
	if(Item)
	{
		Item->Hide();
		if(ObjectHash == Current)
			Current = "";
	}
}

void PopupCollection::PaintPopup(string ID, string ObjectHash, PlutoPopup *Popup, int EffectID)
{
	g_pPlutoLogger->Write(LV_CRITICAL, "PopupCollection::PaintPopup %s", ObjectHash.c_str());

	PopupDescription* Item;
	if(Current == ObjectHash)
		return;

	Current = ObjectHash;
	if (Exists(ObjectHash))
	{
		Item = Popups[ObjectHash];
		if(Item)
			Item->Show();
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "Added in map popup %s, now size is %d", ID.c_str(), Popups.size());

		
		Engine->StartFrameDrawing(ObjectHash);

		Popup->m_pObj->RenderObject(Popup->m_pObj, Popup->m_Position); 
		Item = new PopupDescription(Engine, ID, ObjectHash, NULL);
		if(EffectID)
		{
			
			PlutoRectangle HighlightRectangle(
				Popup->m_pObj->m_rPosition.X + Popup->m_Position.X,
				Popup->m_pObj->m_rPosition.Y + Popup->m_Position.Y,
				Popup->m_pObj->m_rPosition.Width, 
				Popup->m_pObj->m_rPosition.Height);

			//Engine->ShowHighlightRectangle(Popup->m_pObj->m_rPosition);
		}
		Popups[ObjectHash] = Item;
	}
	
}

bool PopupCollection::Exists(string ID)
{
	bool Result = Popups.find(ID) != Popups.end();
	g_pPlutoLogger->Write(LV_WARNING, "Searching in map (size = %d): %s and the result is: %d ", 
		Popups.size(),
		ID.c_str(), 
		Result);
	return Result; 
}

void PopupCollection::ResetObjectInPopup(std::string PopupID, DesignObj_Orbiter *pObj)
{
	std::map <std::string, PopupDescription* >::iterator it = Popups.find(PopupID);

	if(it != Popups.end())
	{
		if( it->second==NULL )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"PopupCollection::ResetObjectInPopup frame is null");
			return;
		}
		MeshFrame *pPopupFrame = it->second->Frame();
		pObj->m_GraphicToDisplay = GRAPHIC_HIGHLIGHTED;
		MeshFrame* pOldFrame = TextureManager::Instance()->GetCacheItem(pObj->GenerateObjectHash(pObj->m_pPopupPoint));
		pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
		MeshFrame* pNewFrame = TextureManager::Instance()->GetCacheItem(pObj->GenerateObjectHash(pObj->m_pPopupPoint));
		
		if(NULL != pPopupFrame && NULL != pOldFrame && NULL != pNewFrame)
		{
			pPopupFrame->ReplaceChild(pOldFrame, pNewFrame);
		}
	}
}
