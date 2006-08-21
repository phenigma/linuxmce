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
TBaseWidget::TBaseWidget(
						 MeshFrame* ParentContext,
						 int Left, 
						 int Top, 
						 int Width, 
						 int Height, 
						 string Text)
	: TextureWrapper2D(0.0f, 0.0f, 1.0f, 1.0f),
	  BackgroundTex(0),
	  Background(1.0f, 1.0f, 1.0f, 1.0f),
	  Parent(NULL),
	  Visible(false),
	  Context(NULL),
	  Container(NULL)
{
	this->Left = Left;
	this->Top = Top;
	this->Width = Width;
	this->Height = Height;
	this->Text = Text;

	Context = ParentContext;
}

TBaseWidget::~TBaseWidget()
{
	vector <TBaseWidget*>::iterator Widget; 
	
	for(Widget = Childs.begin(); Widget < Childs.end(); Widget++)
		delete(*Widget);
		
	Childs.clear();	
}

void TBaseWidget::Paint()
{
	// if the actual window is not visible, will not be painted
	if (!Visible)
		return;
	vector <TBaseWidget*>::iterator Widget; 
	
	for(Widget = Childs.begin(); Widget < Childs.end(); Widget++)
		(*Widget)->Paint();
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

/*virtual*/ string TBaseWidget::GetName()
{
	return Text;
}

MeshFrame* TBaseWidget::GetContext()
{
	return Context;
}
