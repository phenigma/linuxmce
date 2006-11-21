/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "DataGridRenderer_OpenGL.h"

#include "OpenGL3DEngine.h"
#include "OrbiterRenderer_OpenGL.h"
#include "DatagridAnimationManager.h"

#include "SDL.h"

DataGridRenderer_OpenGL::DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner)
: DataGridRenderer(pOwner), m_pRenderFrame(NULL)
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
	PLUTO_SAFETY_LOCK(cm, m_pObj_Owner->m_pOrbiter->m_ScreenMutex);

	string DatagridFrameID = "datagrid " + m_pObj_Owner->GenerateObjectHash(point, false);

	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject");
	MeshFrame * BeforeDataGrid = m_pRenderFrame;

	MeshFrame *BeforeDataGridClone = NULL;
	if(0 != StartAnimation && BeforeDataGrid != NULL)
	{
		TextureManager::Instance()->SuspendTextureRelease();

		//create a clone
		BeforeDataGridClone = BeforeDataGrid->Clone();

		//disconnect from parent
		BeforeDataGridClone->ResetParent();

		//replace old datagrid with the clone in the animation
		Engine->ReplaceMeshInAnimations(BeforeDataGrid, BeforeDataGridClone);

		//old datagrid already deleted when it was replaced with its clone
		BeforeDataGrid = NULL;
	}

	Engine->EndModifyGeometry();
	Engine->StartDatagridDrawing(DatagridFrameID);

	DataGridRenderer::RenderObject(pObj_Screen, point);

	m_pRenderFrame = Engine->EndDatagridDrawing(DatagridFrameID);
	Engine->BeginModifyGeometry();

	if(0 != StartAnimation && NULL != BeforeDataGridClone)
	{
		vector<string> Dependencies;
		BuildDependencies(Dependencies);
		g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::StartAnimation");
		Engine->GetDatagridAnimationManager()->PrepareForAnimation(
			DatagridFrameID, BeforeDataGridClone, m_pRenderFrame, m_AnimationSpeed, iPK_Direction, 
			GetAlphaLevel() / 255.0f, Dependencies);
		StartAnimation = 0;
	}
	else
		Engine->AddMeshFrameToDesktop("", m_pRenderFrame);

	g_pPlutoLogger->Write(LV_WARNING, "DataGridRenderer_OpenGL::RenderObject ENDED");
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

	if(NULL != m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo)
	{
		//also the parent
		Dependencies.push_back("datagrid " + 
			m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo->GenerateObjectHash(
				m_pObj_Owner_DataGrid->m_pDesignObj_Orbiter_TiedTo->m_pPopupPoint, false)
		);
	}	
}

