#include "MeshFrame.h"

#include "MeshPainter.h"

#include "DCE/Logger.h"

MeshFrame::MeshFrame(MeshContainer* Mesh)
{
	this->Mesh = Mesh;
}

MeshFrame::~MeshFrame(void)
{
	CleanUp();
}

/*virtual*/ void MeshFrame::CleanUp()
{
	delete Mesh;
	Mesh = NULL;
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
	if(Frame == NULL)
		return;
	Children.push_back(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	if(Frame == NULL)
		return;
	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); )
		if((*Child) == Frame)
			Child = Children.erase(Child);
		else
			++Child;
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	if(!Visible_)
		return;
	MeshTransform Transform;
	Transform.ApplyTransform(this->Transform);
	Transform.ApplyTransform(ChildTransform);
	MeshPainter* Painter = MeshPainter::Instance();
	if(Mesh!= NULL)
		Painter->PaintContainer(*Mesh, Transform);

	DCE::g_pPlutoLogger->Write(LV_STATUS, "Painting %p with %d children: ", this, Children.size());

	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); Child++)
		(*Child)->Paint(Transform);

	DCE::g_pPlutoLogger->Write(LV_STATUS, "Painting %p END ", this);
}

/*virtual*/ void MeshFrame::SetTransform(MeshTransform& Transform)
{
	this->Transform.CopyFrom(Transform);
}

/*virtual*/ void MeshFrame::ApplyTransform(MeshTransform& Transform)
{
	this->Transform.ApplyTransform(Transform);
}

/*virtual*/ void MeshFrame::SetVisible(bool Visible)
{
	Visible_ = Visible;
}
