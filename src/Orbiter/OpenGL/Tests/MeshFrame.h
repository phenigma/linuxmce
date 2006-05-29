#ifndef MeshFrame_H_
#define MeshFrame_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <vector>

class MeshFrame
{
	MeshContainer* Mesh;
	MeshTransform Transform;

	std::vector<MeshFrame*> Cildren;
	
public:
	MeshFrame(MeshContainer* Mesh = NULL);
	virtual ~MeshFrame(void);

	virtual void CleanUp();

	void SetMeshContainer(MeshContainer* Mesh);

	void AddChildren(MeshFrame* Frame);
	void RemoveChildren(MeshFrame* Frame);

	void Paint(MeshTransform ChildTransform);
	
	virtual void SetTransform(MeshTransform& Transform);
	virtual void ApplyTransform(MeshTransform& Transform);
};

#endif
