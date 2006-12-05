/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "MeshFrame.h"

#include "MeshPainter.h"

#include "DCE/Logger.h"

MeshFrame::MeshFrame(string Name, MeshContainer* Mesh) 
	: Visible_(true),
	TextureTransform(),
	Transform(),
	Parent(NULL),
	Volatile_(false)
{
	this->Mesh = Mesh;
	this->Name_ = Name;
}

MeshFrame::~MeshFrame(void)
{
	if(IsVolatile() && NULL != Mesh)
		Mesh->DisposeTextures();

	delete Mesh;
	Mesh = NULL;

	if(!Volatile_)
		TextureManager::Instance()->InvalidateItem(this);

#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame destructor %p/%s, volatile %d", this, this->Name_.c_str(), Volatile_);
#endif
}

void MeshFrame::MarkAsVolatile() 
{ 
	Volatile_ = true; 
}

/*virtual*/ void MeshFrame::CleanUp(bool VolatilesOnly/* = false*/)
{
#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::CleanUp: %p/%s", this, Name_.c_str());	
#endif

	vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); )
	{
		MeshFrame *pMeshFrame = *Child;

		if(!VolatilesOnly || pMeshFrame->IsVolatile())
		{
			pMeshFrame->CleanUp();
			delete pMeshFrame;
			pMeshFrame = NULL;

			if(VolatilesOnly)
			{
				Child = Children.erase(Child);
			}
			else
				++Child;
		}
		else
		{
			pMeshFrame->ResetParent();
			++Child;
		}
	}	

	if(!VolatilesOnly)
		Children.clear();

	Volatile_ = false;
}

void MeshFrame::SetMeshContainer(MeshContainer* Mesh)
{
	this->Mesh = Mesh;
}

void MeshFrame::AddChild(MeshFrame* Frame)
{
	if(Frame == NULL)
	{
		DCE::g_pPlutoLogger->Write(LV_WARNING, "MeshFrame::AddChild: Frame is NULL!!!");	
		return;
	}

	if(NULL != Frame->Parent)
	{
		DCE::g_pPlutoLogger->Write(LV_WARNING, "MeshFrame::AddChild: Frame %p/%s already has a parent %p!",
			Frame, Frame->Name_.c_str(), Frame->Parent);	

		//throw "Frame already has a parent";
	}

#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::AddChild: Added %p/%s to %p/%s", Frame, Frame->Name_.c_str(),
		this, Name_.c_str());	
#endif

	Children.push_back(Frame);
	Frame->Parent = this;

	CheckIntegrity(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame)
{
	//CheckIntegrity(Frame);

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

#ifdef DEBUG
			DCE::g_pPlutoLogger->Write(LV_STATUS, "MeshFrame::RemoveChild %p/%s from parent %p/%s", 
				Frame, Frame->Name_.c_str(), Frame->Parent, Frame->Parent->Name_.c_str());
#endif

			Frame->Parent = NULL;

			if(Frame->IsVolatile())
			{
				Frame->CleanUp(true);
				delete Frame;
				Frame = NULL;
			}
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
	CheckIntegrity(OldFrame);

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
			NewFrame->Parent = OldFrame->Parent;
			*Child = NewFrame;

#ifdef DEBUG
			DCE::g_pPlutoLogger->Write(LV_STATUS, "ttt MeshFrame::ReplaceChild %p/%s from parent %p/%s with %p/%s", 
				OldFrame, OldFrame->Name_.c_str(), OldFrame->Parent, OldFrame->Parent->Name_.c_str(),
				NewFrame, NewFrame->Name_.c_str());
#endif

			if(NewFrame != OldFrame && NULL != OldFrame && OldFrame->IsVolatile())
			{
				//DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::ReplaceChild: replaced a volatile %p/%s",
				//	OldFrame, OldFrame->Name().c_str());

				OldFrame->CleanUp(true);
				delete OldFrame;
				OldFrame = NULL;
			}

			CheckIntegrity(NewFrame);

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
	if(!ExcludePattern.empty())
	{
		vector<string> vectPatterns;
		StringUtils::Tokenize(ExcludePattern, "|", vectPatterns);

		for(vector<string>::iterator it = vectPatterns.begin(); it != vectPatterns.end(); ++it)
		{
			string sPattern = *it;
			if(this->Name_.find(sPattern) == 0)
				return;
		}
	}

	if(Mesh)
		Mesh->SetAlpha(Alpha);

	vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetAlpha(Alpha, ExcludePattern);
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
	Result->MarkAsVolatile();
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
	//if(Name_.find("rectangle") != string::npos)
	//	return;

	//if(Name_.find("text") != string::npos)
	//	return;

	DCE::g_pPlutoLogger->Write(LV_STATUS, "%s%s '%s' %s %p", sIndent.c_str(), 
		Children.size() ? "+ " : "- ", Name_.c_str(), this->Volatile_ ? "(v)" : "", this);

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

void MeshFrame::Stealth()
{	
	Name_ = Name_ + " stealth";

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
		(*it)->Stealth();
}

bool MeshFrame::CheckIntegrity(MeshFrame *Frame)
{
	bool Result = false;

	if(NULL != Frame && NULL != Frame->Parent)
	{
		vector<MeshFrame*>::iterator Child = find(Frame->Parent->Children.begin(), 
			Frame->Parent->Children.end(), Frame);

        Result = Child != Frame->Parent->Children.end();	
	}

	if(!Result)
	{
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "MeshFrame::CheckIntegrity failed for %p/%s",
			Frame, NULL != Frame ? Frame->Name().c_str() : "null frame");
	}

	return Result;
}