#ifndef PAINTER_H_
#define PAINTER_H_

#include "Container.h"
#include "Transform.h"

class ExtensionManager;

class Painter
{
	static Painter* Instance_;
	ExtensionManager *ExtensionManager_;
	
	Painter();
	
public:
	void PaintContainer(Container& Container, Transform& Transform);
	
	static Painter* Instance();
	static void Destroy();

	void Setup(ExtensionManager *ExtensionManager);
	void CleanUp();
};

#endif /*PAINTER_H_*/
