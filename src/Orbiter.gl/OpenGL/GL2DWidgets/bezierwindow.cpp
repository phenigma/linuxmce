#include "bezierwindow.h"

#include <GL/gl.h>
#include "../OpenGLTextureConverter.h"

#include "../Tests/MeshBuilder.h"
#include "../Tests/MeshFrame.h"

// Generates A Display List Based On The Data In The Patch
// And The Number Of Divisions
GLuint genBezier(BEZIER_PATCH patch, int divs, FloatRect TextureWrapper2D, ColorRGB Background,
					OpenGLTexture BackgroundTexture, MeshTransform& Transform) {
	int		u = 0, v;
	double		py, px, pyold; 
	GLuint		drawlist = glGenLists(1);			// Make The Display List
	POINT_3D	temp[4];
	POINT_3D	*last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs+1));

	float CoordU, CoordV;
	// Array Of Points To Mark The First Line Of Polys


	temp[0] = patch.anchors[0][3];					// The First Derived Curve (Along X-Axis)
	temp[1] = patch.anchors[1][3];
	temp[2] = patch.anchors[2][3];
	temp[3] = patch.anchors[3][3];

	for (v=0;v<=divs;v++) {						// Create The First Line Of Points
		px = ((double)v)/((double)divs);				// Percent Along Y-Axis
		// Use The 4 Points From The Derived Curve To Calculate The Points Along That Curve
		last[v] = MathUtils::Bernstein((float)px, temp);
	}


	for (u=1;u<=divs;u++) {
		py    = ((double)u)/((double)divs);			// Percent Along Y-Axis
		pyold = ((double)u-1.0)/((double)divs);			// Percent Along Old Y Axis

		temp[0] = MathUtils::Bernstein((float)py, patch.anchors[0]);		// Calculate New Bezier Points
		temp[1] = MathUtils::Bernstein((float)py, patch.anchors[1]);
		temp[2] = MathUtils::Bernstein((float)py, patch.anchors[2]);
		temp[3] = MathUtils::Bernstein((float)py, patch.anchors[3]);

		MeshContainer* Container;
		MeshBuilder MB;

		MB.SetColor(1.0f, 1.0f, 1.0f);
		MB.SetAlpha(Background.Alpha);
		MB.SetTexture(BackgroundTexture);

		MB.Begin(MBMODE_TRIANGLE_STRIP);

		for (v=0;v<=divs;v++) {
			px = ((float)v)/((float)divs);			// Percent Along The X-Axis

			// Begin A New Triangle Strip

			CoordU = (float)MathUtils::InterpolateValues(TextureWrapper2D.Left, 
				TextureWrapper2D.Left+TextureWrapper2D.Width, 
				1.0f-(float)px);
			CoordV = (float)MathUtils::InterpolateValues(TextureWrapper2D.Top, 
				TextureWrapper2D.Top+TextureWrapper2D.Height, 
				1-(float)pyold);

			MB.SetTexture2D(CoordU, CoordV); 
			MB.AddVertexFloat(last[v].x, last[v].y, last[v].z);

			last[v] = MathUtils::Bernstein((float)px, temp);			// Generate New Point

			CoordU = (float)MathUtils::InterpolateValues(TextureWrapper2D.Left, 
				TextureWrapper2D.Left+TextureWrapper2D.Width, 
				1-(float)px);
			CoordV = (float)MathUtils::InterpolateValues(TextureWrapper2D.Top, 
				TextureWrapper2D.Top+TextureWrapper2D.Height, 
				1-(float)py);
			MB.SetTexture2D(CoordU, CoordV); 
			MB.AddVertexFloat(last[v].x, last[v].y, last[v].z);
		}

		Container = MB.End();

		MeshFrame* Frame = new MeshFrame();

		Frame->SetMeshContainer(Container);
		Frame->Paint(Transform);
		delete Container;
		Frame->SetMeshContainer(NULL);
		delete Frame;
	}
	
#define DEBUG_BEZIER_SURFACE
#ifdef DEBUG_BEZIER_SURFACE
	int i,j;
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f,1.0f,0.0f, 0.5f); //Color of control lines for bezier patch
	for(i=0;i<4;i++) {								// draw the horizontal lines
		glBegin(GL_LINE_STRIP);
		for(j=0;j<4;j++)
			glVertex3d(patch.anchors[i][j].x, patch.anchors[i][j].y, patch.anchors[i][j].z);
		glEnd();
	}
	for(i=0;i<4;i++) {								// draw the vertical lines
		glBegin(GL_LINE_STRIP);
		for(j=0;j<4;j++)
			glVertex3d(patch.anchors[j][i].x, patch.anchors[j][i].y, patch.anchors[j][i].z);
		glEnd();
	}
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
#endif
	free(last);							// Free The Old Vertices Array
	return drawlist;						// Return The Display List
}



TBezierWindow::TBezierWindow(int Left, int Top, int Width, int Height, char* Text)
	: TBaseWidget(Left, Top, Width, Height, Text) {
	BezierDefinition.Divisions = 15;
}

TBezierWindow::~TBezierWindow() {
}


void TBezierWindow::Paint(MeshTransform& Transform)
{
	if (!Visible)
		return;
		
	TBaseWidget::Paint(Transform);
	
	//glBindTexture(GL_TEXTURE_2D, BackgroundTex);
	//glBegin(GL_TRIANGLE_STRIP);				// Begin A New Triangle Strip

	//glColor4f(Background.Red, Background.Green, Background.Blue, Background.Alpha);
	genBezier(BezierDefinition, BezierDefinition.Divisions, TextureWrapper2D, Background, 
		BackgroundTex, Transform);

	//glEnd();
}

bool TBezierWindow::SetBackgroundImage(OpenGLTexture Background)
{
	// if wants to be set an invalid texture, the function will return false
	if(Background<0)
		return false;
		
	// set up the texture as texture background
	BackgroundTex = Background;
	
	return true;
}

void TBezierWindow::SetUpTextureFromSurface(PlutoGraphic *Picture)
{
	if(Picture == NULL)
		return;
	
	if(BackgroundTex != 0)
		glDeleteTextures(1, &BackgroundTex);
		
	SetTextureWraping(0.0, 0.0, 
		((float)Picture->Width)/MathUtils::MinPowerOf2(Picture->Width), 
		((float)Picture->Height)/MathUtils::MinPowerOf2(Picture->Height));	
	
	BackgroundTex = OpenGLTextureConverter::GenerateTexture(Picture);
	BezierDefinition.texture = BackgroundTex;
}

void TBezierWindow::SetRectCoordinates(FloatRect Coordinates)
{
	SetLeft((int)Coordinates.Left);
	SetTop((int)Coordinates.Top);
	SetWidth((int)Coordinates.Width);
	SetHeight((int)Coordinates.Height);

	for(int x = 0; x<4; x++)
		for(int y = 0; y<4; y++)
		{
			BezierDefinition.anchors[x][y].x = MathUtils::InterpolateValues(float(Left), 
					float(Left + Width), x/3.0f);
			BezierDefinition.anchors[x][y].y = MathUtils::InterpolateValues(float(Top), 
				float(Top + Height), y/3.0f);
			BezierDefinition.anchors[x][y].z = -250.0f;
		}
}

