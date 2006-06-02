#include "MeshFrame.h"

#include "MeshPainter.h"

MeshFrame::MeshFrame(MeshContainer* Mesh)
{
	Visible = true;
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

void MeshFrame::AddChildren(MeshFrame* Frame)
{
	Cildren.push_back(Frame);
}

void MeshFrame::RemoveChildren(MeshFrame* Frame)
{
	std::vector<MeshFrame*>::iterator Child;
	for(Child = Cildren.begin(); Child!=Cildren.end(); Child++)
		if((*Child) == Frame)
			Cildren.erase(Child);
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	if(!Visible)
		return;
	ChildTransform.ApplyTransform(Transform);
	MeshPainter* Painter = MeshPainter::Instance();
	if(Mesh!= NULL)
		Painter->PaintContainer(*Mesh, ChildTransform);

			
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

/*virtual*/ void MeshFrame::SetVisible(bool Visible)
{
	this->Visible = Visible;
}
