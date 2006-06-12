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
	for(Child = Children.begin(); Child!=Children.end(); Child++)
	{
		(*Child)->CleanUp();
		delete (*Child);
	}
	Children.clear();
}

void MeshFrame::SetMeshContainer(MeshContainer* Mesh)
{
	this->Mesh = Mesh;
}

void MeshFrame::AddChild(MeshFrame* Frame)
{
	Children.push_back(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); Child++)
		if((*Child) == Frame)
			Child = Children.erase(Child);
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	ChildTransform.ApplyTransform(Transform);
	MeshPainter* Painter = MeshPainter::Instance();
	if(Mesh!= NULL)
		Painter->PaintContainer(*Mesh, ChildTransform);

			
	/*
 	std::vector<MeshFrame*>::reverse_iterator Child;
	for(Child = Children.rbegin(); Child!=Children.rend(); Child++)
		(*Child)->Paint(ChildTransform);
	*/

	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); Child++)
		(*Child)->Paint(ChildTransform);
}

/*virtual*/ void MeshFrame::SetTransform(MeshTransform& Transform)
{
	this->Transform.CopyFrom(Transform);
}

/*virtual*/ void MeshFrame::ApplyTransform(MeshTransform& Transform)
{
	this->Transform.ApplyTransform(Transform);
}

