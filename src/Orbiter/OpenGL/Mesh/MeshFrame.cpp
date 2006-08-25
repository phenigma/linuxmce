#include "MeshFrame.h"

#include "MeshPainter.h"

#include "DCE/Logger.h"

MeshFrame::MeshFrame(string Name, MeshContainer* Mesh) 
	: Visible_(true),
	TextureTransform(),
	Transform(),
	Parent(NULL)
{
	this->Mesh = Mesh;
	this->Name_ = Name;
	
}

MeshFrame::~MeshFrame(void)
{
}

/*virtual*/ void MeshFrame::CleanUp()
{
	DCE::g_pPlutoLogger->Write(LV_CRITICAL, "xxxxx MeshFrame::CleanUp: %p", this);	

	vector<MeshFrame*>::iterator Child;
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

	if(NULL != Frame->Parent)
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::AddChild: Frame %p/%s already has a parent %p/%s!",
			Frame, Frame->Name_.c_str(), Parent, NULL != Parent ? Parent->Name_.c_str() : "no parent");	

		//throw "Frame already has a parent";
	}

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::AddChild: Added %p to %p", Frame, this);	
	Children.push_back(Frame);
	Frame->Parent = this;
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	if(Frame == NULL)
	{
		//DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::RemoveChild: Frame is NULL!!!");
		return;
	}

	if(NULL != Frame->Parent)
	{
		vector<MeshFrame*>::iterator Child = 
			find(Frame->Parent->Children.begin(), Frame->Parent->Children.end(), Frame);
		if(Child == Frame->Parent->Children.end())
		{
			DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::RemoveChild: Got a parent, but doesn't have us as child!");
			//throw "Got a parent, but doesn't have us as child";
		}
		else
		{
			Frame->Parent->Children.erase(Child);

			DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::RemoveChild %p/%s from parent %p/%s", 
				Frame, Frame->Name_.c_str(), Frame->Parent, Frame->Parent->Name_.c_str());

			Frame->Parent = NULL;
		}
	}
	else
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::RemoveChild: Got no parent! :(");
		//throw "Got no parent! :( I'm all alone!";
	}
}

MeshFrame* MeshFrame::ReplaceChild(MeshFrame* OldFrame, MeshFrame* NewFrame)
{
	if(OldFrame == NULL)
	{
		//DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::ReplaceChild: Frame is NULL!!!");
		return NULL;
	}

	if(NULL != OldFrame->Parent)
	{
		vector<MeshFrame*>::iterator Child = find(OldFrame->Parent->Children.begin(), 
			OldFrame->Parent->Children.end(), OldFrame);

		if(Child == OldFrame->Parent->Children.end())
		{
			DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::ReplaceChild: Got a parent, but doesn't have us as child!");
			//throw "Got a parent, but doesn't have us as child";
		}
		else
		{
			*Child = NewFrame;
			NewFrame->Parent = this;

			DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::ReplaceChild %p/%s from parent %p/%s with %p/%s", 
				OldFrame, OldFrame->Name_.c_str(), OldFrame->Parent, OldFrame->Parent->Name_.c_str(),
				NewFrame, NewFrame->Name_.c_str());

			return NewFrame;
		}
	}
	else
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::ReplaceChild: Got no parent! :(");
		//throw "Got no parent! :( I'm all alone!";
	}

	return OldFrame;
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

	vector<MeshFrame *>::iterator Child, EndChild;
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

/*virtual*/ void MeshFrame::SetAlpha(float Alpha, string ExcludePattern /* = "" */)
{
	//we don't want to setup alpha for Frames that have their name starting with exclude pattern
	if(this->Name_.find(ExcludePattern) == 0)
		return;
	if(Mesh)
		Mesh->SetAlpha(Alpha);

	vector<MeshFrame *>::iterator Child, EndChild;
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
	vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetColor(Color);
	}
}

MeshFrame *MeshFrame::Clone()
{
	MeshFrame *Result = new MeshFrame(Name_ + " clone");
	Result->Visible_ = Visible_;

	if(NULL != Mesh)
		Result->Mesh = Mesh->Clone();

	Result->Transform = Transform;
	Result->TextureTransform = TextureTransform;

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
	{
		Result->AddChild((*it)->Clone());
	}
	return Result;
}

void MeshFrame::Print(string sIndent)
{
	if(Name_.find("rectangle") != string::npos)
		return;

	if(Name_.find("text") != string::npos)
		return;

	DCE::g_pPlutoLogger->Write(LV_STATUS, "%s%s '%s' (frame) %p", sIndent.c_str(), 
		Children.size() ? "+ " : "- ", Name_.c_str(), this);

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
	{
		(*it)->Print(sIndent + "`-");
	}
}

MeshFrame *MeshFrame::FindChild(string Name)
{
	MeshFrame *ChildMesh = NULL;

	if(Name_ == Name)
		ChildMesh = this;
	else
	{
		for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
		{
			ChildMesh = (*it)->FindChild(Name);

			if(NULL != ChildMesh)
				return ChildMesh;
		}
	}

	return ChildMesh;
}