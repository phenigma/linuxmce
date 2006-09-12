#ifndef MeshFrame_H_
#define MeshFrame_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <vector>

using namespace std;

class MeshFrame
{
	bool Visible_;

	MeshContainer* Mesh;
	MeshFrame* Parent;
	string Name_;
	bool Volatile_;

public:
	//MOVE ME
	MeshTransform Transform;
	MeshTransform TextureTransform;
	vector<MeshFrame*> Children;

	MeshFrame(string Name = "unknown", MeshContainer* Mesh = NULL);
	virtual ~MeshFrame(void);

	virtual void CleanUp(bool VolatilesOnly = false);

	void SetMeshContainer(MeshContainer* Mesh);
	MeshContainer* GetMeshContainer();

	void AddChild(MeshFrame* Frame);
	void RemoveChild(MeshFrame* Frame);
	MeshFrame* ReplaceChild(MeshFrame* OldFrame, MeshFrame* NewFrame);

	virtual void Paint(MeshTransform ChildTransform);

	virtual void SetTransform(MeshTransform& Transform);
	virtual void ApplyTransform(MeshTransform& Transform);

	virtual void SetTextureTransform(MeshTransform& TextureTransform);
	virtual void SetVisible(bool Visible);

	virtual void Paint();

	virtual void SetAlpha(float Alpha, string ExcludePattern = "");
	virtual void SetColor(Point3D& Color);

	MeshFrame *Clone();
	void Stealth();
	MeshFrame *GetParent() { return Parent; }
	void ResetParent() { Parent = NULL; }
	MeshFrame *FindChild(string Name);

	void Print(string sIndent);
	string Name() { return Name_; }

	bool CheckIntegrity(MeshFrame *Frame);

	void MarkAsVolatile() { Volatile_ = true; }
	bool IsVolatile() { return Volatile_; }
};

#endif
