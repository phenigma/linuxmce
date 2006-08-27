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
	m_AnimationSpeed = 100;

}

DataGridRenderer_OpenGL::~DataGridRenderer_OpenGL(void)
{

}

#include "DCE/DataGrid.h"
/*virtual*/ void DataGridRenderer_OpenGL::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{

string sValue;
DataGridTable *pDataGridTable = m_pObj_Owner_DataGrid->DataGridTable_Get();
if( pDataGridTable )
{
for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
{
DataGridCell *pDataGridCell = it->second;
if( pDataGridCell->GetText() )
sValue += pDataGridCell->GetText() + string(",");
}
}
else
{
DataGridTable *pDataGridTable = m_pObj_Owner_DataGrid->DataGridTable_Get();
}
g_pPlutoLogger->Write(LV_EVENTHANDLER,"DataGridRenderer_OpenGL::RenderObject datagrid %s to row %d col %d value %s",
					  m_pObj_Owner_DataGrid->m_sGridID.c_str(),m_pObj_Owner_DataGrid->m_GridCurRow,m_pObj_Owner_DataGrid->m_GridCurCol,sValue.c_str());

	string DatagridFrameID = "datagrid " + m_pObj_Owner->GenerateObjectHash(point, false);
	if(Engine->IsCubeAnimatedDatagrid(DatagridFrameID))
		return;

	Engine->BeginModifyGeometry();
	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject");
	MeshFrame * BeforeDataGrid = RenderFrame;

	MeshFrame *BeforeDataGridClone = NULL;
	if(StartAnimation > 0)
	{
		TextureManager::Instance()->SuspendTextureRelease();
		BeforeDataGridClone = BeforeDataGrid->Clone();
		Engine->RemoveMeshFrameFromDesktop(BeforeDataGrid);
	}

	Engine->StartDatagridDrawing(DatagridFrameID);

	DataGridRenderer::RenderObject(pObj_Screen, point);

	RenderFrame = Engine->EndDatagridDrawing(DatagridFrameID);

	if(0 != StartAnimation)
	{
		g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::StartAnimation");
		Engine->CubeAnimateDatagridFrames(DatagridFrameID, BeforeDataGridClone, RenderFrame, m_AnimationSpeed, iPK_Direction, 
			GetAlphaLevel() / 255.0f);
		Engine->AddMeshFrameToDesktop("", BeforeDataGridClone);
		StartAnimation = 0;
	}
	Engine->EndModifyGeometry();

}
