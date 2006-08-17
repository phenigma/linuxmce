#include "DataGridRenderer_OpenGL.h"

#include "OpenGL3DEngine.h"
#include "OrbiterRenderer_OpenGL.h"

#include "SDL.h"

DataGridRenderer_OpenGL::DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner)
: DataGridRenderer(pOwner), RenderFrame(NULL)
{
	Orbiter* pOrbiter = pOwner->m_pOrbiter;
	OrbiterRenderer_OpenGL* pRendererGL = dynamic_cast<OrbiterRenderer_OpenGL*>(pOrbiter->Renderer());
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
	}
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject Engine->StartFrameDrawing");

	Engine->StartDatagridDrawing();

	DataGridRenderer::RenderObject(pObj_Screen, point);

	RenderFrame = Engine->EndDatagridDrawing();

	if(StartAnimation>0)
	{
		Engine->CubeAnimateDatagridFrames("",
			BeforeDataGrid, RenderFrame, 800, iPK_Direction, GetAlphaLevel() / 255.0f);
		StartAnimation = 0;
	}
	else
	{
		Engine->RemoveMeshFrameFromDesktop(BeforeDataGrid);
		//delete BeforeDataGrid;
		//BeforeDataGrid = NULL;
	}
}
