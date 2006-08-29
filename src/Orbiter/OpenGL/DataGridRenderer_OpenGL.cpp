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

//////////////////////////////////////////////////////////////////////////
//temp
static bubu = 1;
if(bubu)
{
	if(m_pObj_Owner->m_ObjectID.find("5086") != string::npos)
		m_pObj_Owner->m_vectObj_TiedToUs.push_back(m_pObj_Owner->m_pOrbiter->FindObject(4949));

	bubu = 0;
}
//////////////////////////////////////////////////////////////////////////

	string DatagridFrameID = "datagrid " + m_pObj_Owner->GenerateObjectHash(point, false);


	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject");
	MeshFrame * BeforeDataGrid = RenderFrame;

	MeshFrame *BeforeDataGridClone = NULL;
	if(StartAnimation > 0)
	{
		TextureManager::Instance()->SuspendTextureRelease();
		BeforeDataGridClone = BeforeDataGrid->Clone();

		Engine->RemoveMeshFrameFromDesktop(BeforeDataGrid);
		//TextureManager::Instance()->InvalidateItem(m_pObj_Owner->GenerateObjectHash(point));
		//BeforeDataGrid->CleanUp();
		//delete BeforeDataGrid;
		//BeforeDataGrid = NULL;
	}

	Engine->StartDatagridDrawing(DatagridFrameID);

	DataGridRenderer::RenderObject(pObj_Screen, point);

	RenderFrame = Engine->EndDatagridDrawing(DatagridFrameID);

	if(0 != StartAnimation)
	{
		//Engine->BeginModifyGeometry();

		if(Engine->IsCubeAnimatedDatagrid(DatagridFrameID))
		{
			Engine->StopDatagridAnimations();
		}

		//Engine->EndModifyGeometry();

		vector<string> Dependencies;
		BuildDependencies(Dependencies);
		g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::StartAnimation");
		Engine->CubeAnimateDatagridFrames(DatagridFrameID, BeforeDataGridClone, RenderFrame, m_AnimationSpeed, iPK_Direction, 
			GetAlphaLevel() / 255.0f, Dependencies);
		Engine->AddMeshFrameToDesktop("", BeforeDataGridClone);
		StartAnimation = 0;
	}
}

void DataGridRenderer_OpenGL::m_AnimationSpeed_set(int AnimationSpeed)
{
	m_AnimationSpeed=AnimationSpeed;
	for( vector<DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.begin();it!=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.end();++it )
	{
		DesignObj_Orbiter *pObj = *it;
		if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			DataGridRenderer_OpenGL *pDataGridRenderer_OpenGL = (DataGridRenderer_OpenGL *) pObj->Renderer();
			pDataGridRenderer_OpenGL->m_AnimationSpeed_set(AnimationSpeed);
		}
	}
}

void DataGridRenderer_OpenGL::BuildDependencies(vector<string> & Dependencies)
{
	Dependencies.clear();
	for( vector<DesignObj_Orbiter *>::iterator it=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.begin();it!=m_pObj_Owner_DataGrid->m_vectObj_TiedToUs.end();++it )
	{
		DesignObj_Orbiter *pObj = *it;
		Dependencies.push_back("datagrid " + pObj->GenerateObjectHash(pObj->m_pPopupPoint, false));
	}
}

