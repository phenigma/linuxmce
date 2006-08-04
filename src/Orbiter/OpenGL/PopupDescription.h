#ifndef PopupDescription_H_
#define PopupDescription_H_


#include <string>
using namespace std;

class MeshFrame;
class OpenGL3DEngine;

class PopupDescription
{
    MeshFrame *PopupFrame;
	string ObjectID;
	OpenGL3DEngine* Engine;

public:
	PopupDescription(OpenGL3DEngine* Engine, string ID);
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
