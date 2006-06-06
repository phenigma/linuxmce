#include "DataGridRenderer_Proxy.h"
#include "Proxy_Orbiter.h"

#include "../DCE/Logger.h"
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/OrbiterRenderer.h"
#include "../Orbiter/DesignObj_DataGrid.h"
#include "../DCE/DataGrid.h"

DataGridRenderer_Proxy::DataGridRenderer_Proxy(DesignObj_Orbiter *pOwner) : DataGridRenderer(pOwner)
{
}

DataGridRenderer_Proxy::~DataGridRenderer_Proxy(void)
{
}

bool DataGridRenderer_Proxy::RenderCell(DataGridTable *pT, DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point)
{
	bool bRetValue = DataGridRenderer::RenderCell(pT, pCell, j, i, iGraphicToDisplay, point);

	Proxy_Orbiter *pProxy_Orbiter = dynamic_cast<Proxy_Orbiter *>(m_pOwner->m_pOrbiter);
	if(NULL == pProxy_Orbiter)
		return bRetValue;

	int x, y, w, h;
	GetGridCellDimensions(pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );

	string sX1 = StringUtils::ltos(x);
	string sY1 = StringUtils::ltos(y);
	string sX2 = StringUtils::ltos(x + w);
	string sY2 = StringUtils::ltos(y + h);

	string sTouchX = StringUtils::ltos(x + w / 2);
	string sTouchY = StringUtils::ltos(y + h / 2);

	string sXMLItem = 
		"<MenuItem>\r\n"
		"\t<Name>Button</Name>\r\n"
		"\t<URL>" + pProxy_Orbiter->m_sRequestUrl + "x=" + sTouchX + "&amp;" + "y=" + sTouchY + "</URL>\r\n"
		"\t<TouchArea X1=\"" + sX1 + "\" Y1=\"" + sY1 + "\" X2=\"" + sX2 + "\" Y2=\"" + sY2 + "\"/>\r\n"
		"</MenuItem>\r\n";

	pProxy_Orbiter->m_dequeCellXMLItems.push_front(sXMLItem);
	return bRetValue;
}
