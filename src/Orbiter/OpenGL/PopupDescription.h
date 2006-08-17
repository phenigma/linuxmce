#ifndef PopupDescription_H_
#define PopupDescription_H_

#include <string>
using namespace std;

class MeshFrame;
class OpenGL3DEngine;
class PlutoRectangle;

class PopupDescription
{
    MeshFrame *PopupFrame;
	string ObjectID;
	string ObjectHash;
	OpenGL3DEngine* Engine;
	PlutoRectangle* HighLight;

public:
	PopupDescription(OpenGL3DEngine* Engine, string ID, string ObjHash, PlutoRectangle* HighLight);
	virtual ~PopupDescription(void);

	/**
	 *	Removes the popup from screne tree
	 */
	void Hide();
	/**
	 *	Adds to scene the current popup
	 */
	void Show();
};

#endif
