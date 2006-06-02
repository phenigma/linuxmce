#include "basewidget.h"

#include <stdlib.h>

/**
 * That class defines one GL Widget
 * You may asociate size of it as top-left point and size (in pixels)
 * @param ParamLeft - left coordinate (of top-left most point)
 * @param ParamTop - top coordinate (of top-left most point of the widget)
 * @param ParamWidth - width of the widget
 * @param ParamHeight - height of the widget
 * @param ParamText - the asociated text of the widget
 * @return 
 */
TBaseWidget::TBaseWidget(int ParamLeft, int ParamTop, int ParamWidth, int ParamHeight, char* ParamText)
	: TextureWrapper2D(0.0f, 0.0f, 1.0f, 1.0f),
	  BackgroundTex(0),
	  Background(1.0f, 1.0f, 1.0f, 1.0f),
	  Left(ParamLeft),
	  Top(ParamTop),  
	  Width(ParamWidth), 
	  Height(ParamHeight), 
	  Parent(NULL),
	  Text(ParamText),
	  Transform(),
	  Visible(false) {
}

TBaseWidget::~TBaseWidget()
{
	std::vector <TBaseWidget*>::iterator Widget; 
	
	for(Widget = Childs.begin(); Widget < Childs.end(); Widget++)
		delete(*Widget);
		
	Childs.clear();	
}

void TBaseWidget::Paint(MeshTransform& Transform)
{
	// if the actual window is not visible, will not be painted
	if (!Visible)
		return;
	std::vector <TBaseWidget*>::iterator Widget; 
	
	for(Widget = Childs.begin(); Widget < Childs.end(); Widget++)
		(*Widget)->Paint(Transform);
}

FloatRect TBaseWidget::GetTextureCoordinates()
{
	return TextureWrapper2D;
}

bool TBaseWidget::GetVisible(){
	return Visible;
}

TBaseWidget* TBaseWidget::GetParent() {
	return Parent;
}

int TBaseWidget::GetNoChild() {
	return int(Childs.size());
}

TBaseWidget* TBaseWidget::GetChild(unsigned int NoChild) {
	if (NoChild>= Childs.size())
		return NULL;
	return Childs[NoChild];
}

/*virtual*/ std::string TBaseWidget::GetName()
{
	std::string Result;
	if (Text == NULL)
		Result = "";
	else
		Result = Text;
	return Result;
}
