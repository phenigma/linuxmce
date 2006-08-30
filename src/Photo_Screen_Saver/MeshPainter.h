#ifndef MESHPAINTER_H_
#define MESHPAINTER_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

class ExtensionManager;

class MeshPainter
{
	static MeshPainter* Instance_;
	ExtensionManager *ExtensionManager_;
	
	MeshPainter();
	
public:
	void PaintContainer(MeshContainer& Container, MeshTransform& Transform);
	
	static MeshPainter* Instance();

	void Setup(ExtensionManager *ExtensionManager);
	void CleanUp();
};

#endif /*MESHPAINTER_H_*/
