#include "ObjectFactory.h"

#include "DesignObj_DataGrid.h"
#include "DesignObj_Orbiter.h"

DesignObj_Orbiter *ObjectFactory::CreateObject(unsigned long ulType, Orbiter *pOrbiter)
{
	switch(ulType)
	{
		case DESIGNOBJTYPE_Datagrid_CONST:	
			return new DesignObj_DataGrid(pOrbiter);
	}

	return new DesignObj_Orbiter(pOrbiter);
}
