#include "MeshFrame.h"

#include "MeshPainter.h"

#include "DCE/Logger.h"

MeshFrame::MeshFrame(MeshContainer* Mesh) 
	: Visible_(true),
	TextureTransform(),
	Transform()
{
	this->Mesh = Mesh;
	
}

MeshFrame::~MeshFrame(void)
{
}

/*virtual*/ void MeshFrame::CleanUp()
{
	DCE::g_pPlutoLogger->Write(LV_CRITICAL, "xxxxx MeshFrame::CleanUp: %p", this);	

	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); Child++)
	{
		MeshFrame *pMeshFrame = *Child;
	
		pMeshFrame->CleanUp();
		delete pMeshFrame;
		pMeshFrame = NULL;
	}
	Children.clear();

	delete Mesh;
	Mesh = NULL;
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

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::AddChild: Added %p to %p", Frame, this);	
	Children.push_back(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	if(Frame == NULL)
	{
		//DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::RemoveChild: Frame is NULL!!!");
		return;
	}
		
	//DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::RemoveChild %p, is leaf %d", Frame, Frame->Children.size() == 0);

	std::vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); )
	{
		MeshFrame *pChildFrame = *Child;
		pChildFrame->RemoveChild(Frame);

		if((*Child) == Frame)
			Child = Children.erase(Child);
		else
			++Child;
	}
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	if(!Visible_)
	{
		//DCE::g_pPlutoLogger->Write(LV_STATUS, "NOT Painting %p. It's invisible.", this);	
		return; 
	}
	
	MeshTransform Transform;
	Transform.ApplyTransform(this->Transform);
	Transform.ApplyTransform(ChildTransform);
	MeshPainter* Painter = MeshPainter::Instance();
	if(Mesh!= NULL)
		Painter->PaintContainer(*Mesh, Transform, TextureTransform);

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "xxxxx Painting %p with %d children: ", this, Children.size());

	std::vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->Paint(Transform);
	}

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "xxxxx Painting %p END ", this);
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

/*virtual*/ void MeshFrame::Paint()
{
    MeshTransform Transform;
	Paint(Transform);
}

/*virtual*/ void MeshFrame::SetTextureTransform(MeshTransform& TextureTransform)
{
	this->TextureTransform = TextureTransform;
}

MeshContainer* MeshFrame::GetMeshContainer()
{
	return Mesh;
}

/*virtual*/ void MeshFrame::SetAlpha(float Alpha)
{
	if(Mesh)
		Mesh->SetAlpha(Alpha);

	std::vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetAlpha(Alpha);
	}
}

/*virtual*/ void MeshFrame::SetColor(Point3D& Color)
{
	if(Mesh)
		Mesh->SetColor(Color);
	std::vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetColor(Color);
	}
}
