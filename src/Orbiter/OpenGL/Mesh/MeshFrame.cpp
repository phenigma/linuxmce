#include "MeshFrame.h"

#include "MeshPainter.h"

#include "DCE/Logger.h"

MeshFrame::MeshFrame(MeshContainer* Mesh) : Visible_(true)
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
		MeshFrame *pMeshFrame = *Child;
	
		pMeshFrame->CleanUp();
		delete pMeshFrame;
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
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::AddChild: Frame is NULL!!!");	
		return;
	}
	
	DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::AddChild: Added %p to %p", Frame, this);	
	Children.push_back(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	if(Frame == NULL)
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::RemoveChild: Frame is NULL!!!");
		return;
	}
		
	DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::RemoveChild %p, is leaf %d", Frame, Frame->Children.size() == 0);
		
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
	{
		DCE::g_pPlutoLogger->Write(LV_STATUS, "NOT Painting %p. It's invisible.", this);	
		return;
	}
	
	MeshTransform Transform;
	Transform.ApplyTransform(this->Transform);
	Transform.ApplyTransform(ChildTransform);
	MeshPainter* Painter = MeshPainter::Instance();
	if(Mesh!= NULL)
		Painter->PaintContainer(*Mesh, Transform);

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "Painting %p with %d children: ", this, Children.size());

	std::vector<MeshFrame*>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
		(*Child)->Paint(Transform);

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "Painting %p END ", this);
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
