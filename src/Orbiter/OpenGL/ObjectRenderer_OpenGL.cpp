#include "ObjectRenderer_OpenGL.h"

#include "Orbiter.h"
#include "OrbiterRenderer.h"

using namespace DCE;

ObjectRenderer_OpenGL::ObjectRenderer_OpenGL(DesignObj_Orbiter *pOwner) : ObjectRenderer(pOwner)
{
	
}

/*virtual*/ ObjectRenderer_OpenGL::~ObjectRenderer_OpenGL(void)
{

}

/*virtual*/ void ObjectRenderer_OpenGL::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	//we have nothing to render
	if(m_pOwner->m_vectGraphic.size() == 0)
		return;

	PlutoGraphic *pPlutoGraphic = m_pOwner->m_vectGraphic[0];

	string sFileName = "";
	if(
		pPlutoGraphic->IsEmpty() && NULL != GetCacheImageManager() && pPlutoGraphic->m_Filename.length() &&
		GetCacheImageManager()->IsImageInCache(pPlutoGraphic->m_Filename, m_pOwner->m_Priority)
		)
	{
		//if we have the file in cache
		sFileName = GetCacheImageManager()->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(pPlutoGraphic->IsEmpty() && GetLocalDirectory().length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = GetLocalDirectory() + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(pPlutoGraphic->IsEmpty() && sFileName.empty())
	{
		// Request our config info
		char *pGraphicFile=NULL;
		int iSizeGraphicFile=0;

		DCE::CMD_Request_File CMD_Request_File(
			m_pOwner->m_pOrbiter->m_dwPK_Device,m_pOwner->m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			"orbiter/C" + StringUtils::itos(m_pOwner->m_pOrbiter->m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename,
			&pGraphicFile,&iSizeGraphicFile);
		m_pOwner->m_pOrbiter->SendCommand(CMD_Request_File);

		if (!iSizeGraphicFile)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());
			return;
		}

		//save the image in cache
		if(NULL != GetCacheImageManager()) //cache manager is enabled ?
		{
			GetCacheImageManager()->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, m_pOwner->m_Priority);
			
			sFileName = GetCacheImageManager()->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		if( (sFileName.empty() || iSizeGraphicFile) && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
		{
			delete pGraphicFile;
			pGraphicFile = NULL;
			return;
		}

		delete pGraphicFile;
		pGraphicFile = NULL;
	}

	if(pPlutoGraphic->IsEmpty() && !sFileName.empty())
	{
		if(!FileUtils::FileExists(sFileName))
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read file %s", (sFileName).c_str());
			return;
		}

		switch(pPlutoGraphic->m_GraphicFormat)
		{
		case GR_JPG:
		case GR_GIF:
		case GR_TIF:
		case GR_PNG:
		case GR_BMP:
			{
				size_t size = 0;
				char *pData = FileUtils::ReadFileIntoBuffer(sFileName.c_str(), size);

				if(!size)
					return;

				if(!pPlutoGraphic->LoadGraphic(pData, size))
					return;

				delete [] pData;
			}
			break;

		default:
			;
		}
	}


	if(!pPlutoGraphic->IsEmpty())
		m_pOwner->m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
#ifdef DEBUG
	else
		g_pPlutoLogger->Write(LV_STATUS, "No graphic to render for object %s", m_pOwner->m_ObjectID.c_str());
#endif

	//TODO:
	//m_pOwner->m_GraphicToDisplay == GRAPHIC_SELECTED)
	//m_pOwner->m_pOrbiter->SelectObject(m_pOwner, point);
}


