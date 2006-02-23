#ifndef CONTROLPOINT_H_
#define CONTROLPOINT_H_
#include "main.h"
class ControlPoint
{
public:
	ControlPoint();
	virtual ~ControlPoint();
protected:
	static char *GetFirstDocumentItem( IXML_Document * doc,
                                 const char *item );
};

#endif /*CONTROLPOINT_H_*/
