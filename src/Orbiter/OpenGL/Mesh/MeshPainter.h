#ifndef MESHPAINTER_H_
#define MESHPAINTER_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

#include <pthread.h>

class ExtensionManager;

class MeshPainter
{
	pthread_mutex_t SafetyPaintMutex;
	static MeshPainter* _Instance;
	ExtensionManager *ExtensionManager_;
	
	MeshPainter();
	
public:
	void PaintContainer(MeshContainer& Container, MeshTransform& Transform,
		MeshTransform& TextureTransform);
	
	static MeshPainter* Instance();

	void Setup(ExtensionManager *ExtensionManager);
	void CleanUp();
	
};

#endif /*MESHPAINTER_H_*/
