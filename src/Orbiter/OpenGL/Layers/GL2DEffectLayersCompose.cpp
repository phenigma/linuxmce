#include "GL2DEffectLayersCompose.h"

#include <iostream>

#include "../OpenGLGraphic.h"
#include "../Mesh/MeshPainter.h"


namespace GLEffect2D 
{

LayersCompose* LayersCompose::Instance_ = NULL;

LayersCompose::LayersCompose() : 
	CurrentLayer(NULL), 
	OldLayer(NULL),
	TextureMan(NULL),
	OldScreen(NULL),
	NewScreen(NULL)
{
}

/*virtual*/ LayersCompose::~LayersCompose(void)
{
}

/*static*/ LayersCompose* LayersCompose::Instance()
{
	if(Instance_ == NULL)
		Instance_ = new LayersCompose();
	return Instance_;
}

void LayersCompose::Setup(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	TextureMan = TextureManager::Instance();
	OldScreen = new GLRenderScreenToGraphic(Width, Height);
	NewScreen = new GLRenderScreenToGraphic(Width, Height);
	CreateLayer(2);
	CreateLayer(3);
}

void LayersCompose::CleanUp()
{
	delete OldScreen;
	OldScreen = NULL;
	delete NewScreen;
	NewScreen = NULL;

	this->RemoveLayer(3);
	this->RemoveLayer(2);
}

bool LayersCompose::ExistsLayerLevel(int LayerLevel)
{
	return (LayerList.find(LayerLevel) != LayerList.end());
}

Layer* LayersCompose::CreateLayer(int LayerLevel)
{
	Layer* Result = new Layer(Width, Height);
	LayerList[LayerLevel] = Result;
	return Result;
}

bool LayersCompose::RemoveLayer(int LayerLevel)
{
	if(!ExistsLayerLevel(LayerLevel))
		return false;

	std::map <int, Layer*>::iterator Item = LayerList.begin();
	while(Item != LayerList.end())
	{
		if(Item->first == LayerLevel)
		{
			delete (Item->second); 
			LayerList.erase(Item);
			return true;
		}
		else
			++Item;
	}

	return false;
}

FloatRect LayersCompose::GetDefaultFullScreenUVMapping()
{
	FloatRect Result;
	return Result;
}

void LayersCompose::Paint()
{
	//Count the layers with effect
	int NoLayers = 0;
	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
			NoLayers++;
	}

	//NoLayers
	if(NoLayers == 0)
	{
		this->PaintScreen3D();
		
		return;
	}

	
	MeshTransform Transform;
	OldScreen->RenderFrameToGraphic(Transform);
	//NewScreen->RenderFrame();

	//TextureManager->ResetRendering();
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
		{
			NoLayers--;
			Item->second->Paint();
		}
	}
}

bool LayersCompose::HasEffects()
{
	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
			return true;
	}	
	return false;
}

Effect* LayersCompose::CreateEffect(int LayerLevel, int IDEffect, 
	int StartAfter, int TimeForComplete)
{
	return LayerList[LayerLevel]->CreateEffect(IDEffect, StartAfter, TimeForComplete);
}

TBaseWidget* LayersCompose::CreateWidget(int LayerLevel, int WidgetType, int Top, 
	int Left, int Width, int Height, char* Text)
{
	return LayerList[LayerLevel]->CreateWidget(WidgetType, Top, Left, Width, 
		Height, Text);
}

void LayersCompose::PaintScreen3D()
{
	//CurrentLayer->Paint(Transform);
	//LayerList[3]->SetRenderFrame(CurrentLayer);
	//LayerList[3]->RenderFrameToGraphic(Transform);
	MeshTransform Transform;
	Transform.ApplyTranslate(-Width/2.0f, -Height/2.0f,1.2f* Height/2.0f);
	CurrentLayer->Paint(Transform);
}

void LayersCompose::Resize(int Width, int Height)
{
	std::map <int, Layer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
		Item->second->Resize(Width, Height);
}

void LayersCompose::UpdateLayers(MeshFrame* CurrentLayer, MeshFrame* OldLayer)
{
	this->CurrentLayer = CurrentLayer;
	this->OldLayer = OldLayer;
}

MeshFrame* LayersCompose::GetCurrentLayer()
{
	return CurrentLayer;
}

MeshFrame* LayersCompose::GetOldLayer()
{
	return OldLayer;
}

OpenGLGraphic* LayersCompose::GetOldScreen()
{
	return NULL;
}

OpenGLGraphic* LayersCompose::GetNewScreen()
{
	return NULL;
}

}
