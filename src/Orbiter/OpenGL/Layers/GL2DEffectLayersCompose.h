#ifndef GL2DEffectLayersCompose_H_
#define GL2DEffectLayersCompose_H_

#include "../Effects/gl2deffectfactory.h"
#include "../Texture/TextureManager.h"
#include <map>

#include "GL2DEffectLayer.h"

namespace GLEffect2D 
{

	enum GL2DEffectDefaultLayers {
		DesktopOffScreen = 2,
		DesktopOnScreen = 3
	};


/**
 *	Singleton class that defines a layered based rendering
 */
class LayersCompose
{
	int Width, Height;
	std::map <int, GL2DEffectLayer*> LayerList;

	int CurrentLayer;

	TextureManager* TextureMan;

	static LayersCompose* Instance;
	LayersCompose();
public:
	static LayersCompose* GetInstance();
	virtual ~LayersCompose(void);

	void Setup(int Width, int Height);
	void CleanUp();

	OpenGLTexture GetNewScreen();

	OpenGLTexture GetOldScreen();

	void SetUpNextDisplay();

	FloatRect GetDefaultFullScreenUVMapping();

	/**
	 *	Creates a custom layer that match the level. 
	 *	If the layer level already exits, it returns the old layer
	 */
	GL2DEffectLayer* CreateLayer(int LayerLevel);

	/**
	 *	Returns if exist one layer at choosen layer level
	 */
	bool ExistsLayerLevel(int LayerLevel);

	/**
	 *	Remove a layer from drawing
	 *	returns false if layer previously doesn't exist;
	 */
	bool RemoveLayer(int LayerLevel);

	/**
	 *	Paint all layers
	 */
	void Paint();

	/*
	 *	Test if there are pending effects in any of layers
	 */
	bool HasEffects();

	/**
	 *	Creates a layer inside the chosed layer
	 */
	GL2DEffect* CreateEffect(int LayerLevel, int IDEffect, int TimeForComplete);

	/**
	 *	Creates a widget inside the chosed layer
	 */
	TBaseWidget* CreateWidget(int LayerLevel, int WidgetType, int Top, 
		int Left, int Width, int Height, char* Text);

	/**
	 * Clear the openGL screen using an RGB based color
	 * @param Red Red component of the clear color
	 * @param Green Green component of the clear color
	 * @param Blue Blue component of the clear color
	 */
	void ClearScreen(unsigned char Red, unsigned char Green, unsigned char Blue);

	void PaintScreen3D();

	/**
	 *	TODO: For Ciprian, to treat the cases of window resize
	 *	Support for resize screen 
	 */
	void Resize(int Width, int Height);

	OpenGLTexture GetRenderedScreen();
};

}

#endif
