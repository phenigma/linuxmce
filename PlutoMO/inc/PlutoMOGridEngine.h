#ifndef PLUTOMOGRIDENGINE_H
#define PLUTOMOGRIDENGINE_H

#include <akngrid.h> // CAknGrid


class CPlutoMOGridEngine : public CAknGrid
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
