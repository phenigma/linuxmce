#ifndef Frame_H_
#define Frame_H_

#include "Container.h"
#include "Transform.h"

#include <vector>

using namespace std;

class Frame
{
	bool Visible_;

	Container* pMesh;
	Frame* Parent;
	string Name_;

public:
//MOVE ME
	Transform pTransform;
	vector<Frame*> Children;

	Frame(string Name = "unknown", Container*  = NULL);
	virtual ~Frame(void);

	virtual void CleanUp();

	void SetContainer(Container* );
	Container* GetContainer();

	void AddChild(Frame* Frame);
	void RemoveChild(Frame* Frame);

	virtual void Paint(Transform ChildTransform);
	
	virtual void SetTransform(Transform& pTransform);
	virtual void ApplyTransform(Transform& pTransform);

	virtual void SetVisible(bool Visible);

	virtual void Paint();

	virtual void SetAlpha(float Alpha, string ExcludePattern = "");
	virtual void SetColor(Point3D& Color);

	Frame *GetParent() { return Parent; }
	void ResetParent() { Parent = NULL; }
	Frame *FindChild(string Name);

	void Print(string sIndent);
	string Name() { return Name_; }
};

#endif
