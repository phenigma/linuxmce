#ifndef PLUTOBTGRIDENGINE_H
#define PLUTOBTGRIDENGINE_H

#include <akngrid.h> // CAknGrid


class CPlutoBTGridEngine : public CAknGrid
	{
public: 

	void ConstructL(TInt aGridResource, TInt aIconFileResource);

private: 

	void SizeChanged();
	
private: 
	void SetupGrid();
	void SetupGridIconsL(TInt aIconFileResource);

	
	};

#endif	

// End of File
