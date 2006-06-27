#ifndef MeshFrame_H_
#define MeshFrame_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <vector>

class MeshFrame
{
	bool Visible_;

	
public:

//MOVE ME
	MeshContainer* Mesh;
	MeshTransform Transform;
	MeshTransform TextureTransform;
	std::vector<MeshFrame*> Children;

	MeshFrame(MeshContainer* Mesh = NULL);
	virtual ~MeshFrame(void);

	virtual void CleanUp();

	void SetMeshContainer(MeshContainer* Mesh);

	void AddChild(MeshFrame* Frame);
	void RemoveChild(MeshFrame* Frame);

	virtual void Paint(MeshTransform ChildTransform);
	
	virtual void SetTransform(MeshTransform& Transform);
	virtual void ApplyTransform(MeshTransform& Transform);

	virtual void SetTextureTransform(MeshTransform& TextureTransform);
	virtual void SetVisible(bool Visible);

	virtual void Paint();
};

#endif
