#ifndef MESHPAINTER_H_
#define MESHPAINTER_H_

#include "MeshContainer.h"
#include "MeshTransform.h"

class MeshPainter
{
public:
	virtual void PaintContainer(MeshContainer& Container, MeshTransform& Transform);
};

#endif /*MESHPAINTER_H_*/
