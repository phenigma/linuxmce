#ifndef GLFONTRENDERER_H_
#define GLFONTRENDERER_H_

#include<string>

class GLFontRenderer
{
	
	std::string FontName;
public:
	/**
	 * Utility class that render a font and create a mesh to be drawn.
	 */
	GLFontRenderer();
	virtual ~GLFontRenderer();
	
	
	void TextOut(int X, int Y, std::string Text);
	
};

#endif /*GLFONTRENDERER_H_*/
