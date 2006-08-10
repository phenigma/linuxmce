#include "DataGridRenderer_OpenGL.h"

#include "OpenGL3DEngine.h"
#include "OrbiterRenderer_OpenGL.h"

#include "SDL.h"

DataGridRenderer_OpenGL::DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner)
: DataGridRenderer(pOwner), RenderFrame(NULL)
{
	Orbiter* pOrbiter = pOwner->m_pOrbiter;
	OrbiterRenderer_OpenGL* pRendererGL = dynamic_cast<OrbiterRenderer_OpenGL*>(pOrbiter->Renderer());
	StartAnimation = 0;
	if(NULL == pRendererGL)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot get a reference to the OrbiterRenderer!... quiting");
		Engine = NULL;
		return;
	}
	this->Engine = pRendererGL->Engine;

}

DataGridRenderer_OpenGL::~DataGridRenderer_OpenGL(void)
{

}


/*virtual*/ void DataGridRenderer_OpenGL::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject");
	MeshFrame * BeforeDataGrid = RenderFrame;
	if(RenderFrame)
	{
		g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject release old frame");
		//Engine->RemoveMeshFrameFromDesktop(RenderFrame);

		//delete RenderFrame;
		//RenderFrame = NULL;
	}
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject Engine->StartFrameDrawing");

	Engine->StartDatagridDrawing();

	DataGridRenderer::RenderObject(pObj_Screen, point);

	RenderFrame = Engine->EndDatagridDrawing();
	/*
	MeshTransform Transform;
	Transform.ApplyTranslate(0, -Engine->Compose->Height, 0);
	Transform.ApplyRotateX(15*(0.5-DoubleRender));
	Transform.ApplyTranslate(0, Engine->Compose->Height, 0);
	RenderFrame->SetTransform(Transform);
	//Engine->RemoveMeshFrameFromDesktop(RenderFrame);
*/
	if(StartAnimation>0)
	{
		
		Engine->CubeAnimateDatagridFrames(BeforeDataGrid, RenderFrame, 1200, iPK_Direction);
		StartAnimation = 0;
	}
}

void DataGridRenderer_OpenGL::Scroll_Grid(string sRelative_Level, int iPK_Direction)
{
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::Scroll_Grid");
	this->iPK_Direction = iPK_Direction;
	DataGridRenderer::Scroll_Grid(sRelative_Level, iPK_Direction);
	StartAnimation = 1;
	
}
