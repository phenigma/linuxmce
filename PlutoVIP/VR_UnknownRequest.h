#ifndef VR_UnknownRequest_H
#define VR_UnknownRequest_H

#include "RA/RA_Request.h"

class VR_UnknownRequest : public RA_Request
{
public:
	VR_UnknownRequest(long data,const char *size) : RA_Request() {}

};
#endif

