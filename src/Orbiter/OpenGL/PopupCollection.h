#ifndef PopupCollection_H_
#define PopupCollection_H_

#include "PopupDescription.h"

#include <map>
#include <string>

#include "Orbiter.h"
#include "Mesh/MeshFrame.h"

class OpenGL3DEngine;

class PopupCollection
{
	std::map <std::string, PopupDescription* > Popups;
	std::string Current;
	MeshFrame* Scene;
	OpenGL3DEngine* Engine;

public:
	PopupCollection(OpenGL3DEngine* Engine);
	~PopupCollection();

	void PaintPopup(std::string ID, std::string ObjectHash, PlutoPopup *Popup, int EffectID);
	void HidePopup(std::string ID, std::string ObjectHash);

	void Reset();

	bool Exists(std::string ID);
};

#endif
