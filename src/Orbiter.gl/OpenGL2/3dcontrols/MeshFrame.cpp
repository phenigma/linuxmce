#include "MeshFrame.h"

#include "MeshPainter.h"

MeshFrame::MeshFrame(MeshContainer* Mesh)
{
	this->Mesh = Mesh;
}

MeshFrame::~MeshFrame(void)
{
	
}

/*virtual*/ void MeshFrame::CleanUp()
{
	delete Mesh;
	std::vector<MeshFrame*>::iterator Child;
	for(Child = Cildren.begin(); Child!=Cildren.end(); Child++)
	{
		(*Child)->CleanUp();
		delete (*Child);
	}
	Cildren.clear();
}

void MeshFrame::SetMeshContainer(MeshContainer* Mesh)
{
	this->Mesh = Mesh;
}

void MeshFrame::AddChild(MeshFrame* Frame)
{
	Cildren.push_back(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	std::vector<MeshFrame*>::iterator Child;
	for(Child = Cildren.begin(); Child!=Cildren.end(); Child++)
		if((*Child) == Frame)
			Cildren.erase(Child);
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	ChildTransform.ApplyTransform(Transform);
	MeshPainter Painter;
	if(Mesh!= NULL)
		Painter.PaintContainer(*Mesh, ChildTransform);

			
 	std::vector<MeshFrame*>::iterator Child;
	for(Child = Cildren.begin(); Child!=Cildren.end(); Child++)
		(*Child)->Paint(ChildTransform);
}

/*virtual*/ void MeshFrame::SetTransform(MeshTransform& Transform)
{
	Transform.CopyFrom(Transform);
}

/*virtual*/ void MeshFrame::ApplyTransform(MeshTransform& Transform)
{
	Transform.ApplyTransform(Transform);
}

