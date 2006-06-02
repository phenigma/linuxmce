#include "GL2DEffectLayersCompose.h"

#include "../Orbiter3DCommons.h"

#include "../../SDL/SDLGraphic.h"

#include <iostream>

#include "../Tests/OpenGLGraphic.h"

namespace GLEffect2D 
{

LayersCompose* LayersCompose::Instance = NULL;

LayersCompose::LayersCompose()
{
	TextureMan = TextureManager::Instance();
}

/*virtual*/ LayersCompose::~LayersCompose(void)
{
}

/*static*/ LayersCompose* LayersCompose::GetInstance()
{
	if(Instance == NULL)
		Instance = new LayersCompose();
	return Instance;
}

void LayersCompose::Setup(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	TextureMan = TextureManager::Instance();
}

void LayersCompose::CleanUp()
{
	RemoveLayer(2);
}

OpenGLTexture LayersCompose::GetNewScreen()
{
	return NULL;
	//return TextureManager->GetNextScreen();
}

OpenGLTexture LayersCompose::GetOldScreen()
{
	//return TextureManager->GetLastScreen();
	return 0;
}

void LayersCompose::SetUpNextDisplay()
{
//	if(TextureMan != NULL)
//		TextureMan->SetUpNextDisplay();
}

void LayersCompose::ConfigureNextScreen(PlutoGraphic* NextScreen)
{
	SDLGraphic* TextureScreen = dynamic_cast<SDLGraphic*>(NextScreen);
	TextureMan->ConfigureNextScreen(TextureScreen->m_pSDL_Surface);
}

void LayersCompose::ConfigureOldScreen(PlutoGraphic* LastScreen)
{
//	if(TextureMan != NULL)
//		TextureMan->ConfigureOldScreen(LastScreen);
}

bool LayersCompose::ExistsLayerLevel(int LayerLevel)
{
	return (LayerList.find(LayerLevel) != LayerList.end());
}

GL2DEffectLayer* LayersCompose::CreateLayer(int LayerLevel)
{
	GL2DEffectLayer* Result = new GL2DEffectLayer(Width, Height);
	LayerList[LayerLevel] = Result;
	return Result;
}

bool LayersCompose::RemoveLayer(int LayerLevel)
{
	if(!ExistsLayerLevel(LayerLevel))
		return false;

	std::map <int, GL2DEffectLayer*>::iterator Item = LayerList.begin();
	while(Item != LayerList.end())
	{
		if(Item->first == LayerLevel)
		{
			delete (Item->second);
			Item = LayerList.erase(Item);
		}
		else
			++Item;
	}

	return true;
}

FloatRect LayersCompose::GetDefaultFullScreenUVMapping()
{
	FloatRect Result;
	return Result;
}

void LayersCompose::Paint()
{
	//TextureMan->SetUpNextDisplay();
	ClearScreen(0, 0, 0);
		
	//Count the layers with effect
	int NoLayers = 0;
	std::map <int, GL2DEffectLayer*>::iterator Item;
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

	//TextureManager->ResetRendering();
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
		{
			NoLayers--;
			Item->second->Paint();

//			if(NoLayers)
//				TextureManager->CopyRenderSceneToTexture();
		}
	}

}

bool LayersCompose::HasEffects()
{
	std::map <int, GL2DEffectLayer*>::iterator Item;
	for(Item = LayerList.begin(); Item != LayerList.end(); ++Item)
	{
		if ((Item->second)->HasEffects())
			return true;
	}	
	return false;
}

GL2DEffect* LayersCompose::CreateEffect(int LayerLevel, int IDEffect, 
	int TimeForComplete)
{
	return LayerList[LayerLevel]->CreateEffect(IDEffect, TimeForComplete);

}

TBaseWidget* LayersCompose::CreateWidget(int LayerLevel, int WidgetType, int Top, 
	int Left, int Width, int Height, char* Text)
{
	return LayerList[LayerLevel]->CreateWidget(WidgetType, Top, Left, Width, 
		Height, Text);
}

void LayersCompose::ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	glClearColor(Red/255.0f, Green/255.0f, Blue/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void LayersCompose::PaintScreen3D()
{
	DrawingWidgetsEngine* Engine = new DrawingWidgetsEngine(Width, Height);
	TBaseWidget* Screen3D = Commons3D::Instance().GetScreen3D();
	TBaseWidget* Selected = Commons3D::Instance().GetSelected();
	TBaseWidget* Highlight = Commons3D::Instance().GetHighLight();

	

	Engine->AddWidget(Screen3D);
	Engine->AddWidget(Selected);
	Engine->AddWidget(Highlight);

	Screen3D->SetVisible(true);
	
	Screen3D->SetTexture(TextureMan->NewScreen->Texture);
	Selected->SetVisible(true);
	Highlight->SetVisible(true);

	MeshTransform Transform;
	Engine->Paint(Transform);
	Engine->RemoveWidgetFromList(Highlight);
	Engine->RemoveWidgetFromList(Selected);
	Engine->RemoveWidgetFromList(Screen3D);

	Screen3D->SetVisible(false);
	Selected->SetVisible(false);
	Highlight->SetVisible(false);
	delete Engine;
}

OpenGLTexture LayersCompose::GetRenderedScreen()
{
	if(!TextureMan)
		return 0;
	return 0; //TextureMan->GetRenderedScreen();
}

}
