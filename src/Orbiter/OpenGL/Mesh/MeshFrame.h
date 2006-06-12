#ifndef MeshFrame_H_
#define MeshFrame_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <vector>

class MeshFrame
{
	MeshContainer* Mesh;
	MeshTransform Transform;

	
public:

//MOVE ME
std::vector<MeshFrame*> Children;

	MeshFrame(MeshContainer* Mesh = NULL);
	virtual ~MeshFrame(void);

	virtual void CleanUp();

	void SetMeshContainer(MeshContainer* Mesh);

	void AddChild(MeshFrame* Frame);
	void RemoveChild(MeshFrame* Frame);

	void Paint(MeshTransform ChildTransform);
	
	virtual void SetTransform(MeshTransform& Transform);
	virtual void ApplyTransform(MeshTransform& Transform);
};

#endif
