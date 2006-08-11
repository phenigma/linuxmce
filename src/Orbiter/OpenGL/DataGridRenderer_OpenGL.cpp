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
DesignObj_DataGrid *pDataGridOwner = dynamic_cast<DesignObj_DataGrid *>(pObj_Screen);
if( pDataGridOwner )
g_pPlutoLogger->Write(LV_ACTION, "DataGridRenderer_OpenGL::RenderObject-start %s start row %d",pDataGridOwner->m_sGridID.c_str(),pDataGridOwner->m_GridCurRow);
else
g_pPlutoLogger->Write(LV_ACTION, "DataGridRenderer_OpenGL::RenderObject-NULL");
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject");
	MeshFrame * BeforeDataGrid = RenderFrame;
	if(RenderFrame)
	{
		g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject release old frame");
	}
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject Engine->StartFrameDrawing");

	Engine->StartDatagridDrawing();

if( pDataGridOwner )
g_pPlutoLogger->Write(LV_ACTION, "DataGridRenderer::RenderObject-start %s start row %d",pDataGridOwner->m_sGridID.c_str(),pDataGridOwner->m_GridCurRow);
	DataGridRenderer::RenderObject(pObj_Screen, point);
if( pDataGridOwner )
g_pPlutoLogger->Write(LV_ACTION, "DataGridRenderer::RenderObject-stop %s start row %d",pDataGridOwner->m_sGridID.c_str(),pDataGridOwner->m_GridCurRow);

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
if( pDataGridOwner )
g_pPlutoLogger->Write(LV_ACTION, "DataGridRenderer_OpenGL::RenderObject-stop %s start row %d",pDataGridOwner->m_sGridID.c_str(),pDataGridOwner->m_GridCurRow);
}

bool DataGridRenderer_OpenGL::Scroll_Grid(string sRelative_Level, int iPK_Direction,bool bMoveOneLineIfCannotPage)
{
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::Scroll_Grid");
	this->iPK_Direction = iPK_Direction;
	bool bResult = DataGridRenderer::Scroll_Grid(sRelative_Level, iPK_Direction, bMoveOneLineIfCannotPage);
	StartAnimation = 1;
	return bResult;
}
