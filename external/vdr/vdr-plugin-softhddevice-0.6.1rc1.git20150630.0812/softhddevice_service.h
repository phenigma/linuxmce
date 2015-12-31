///
///	@file softhddev_service.h @brief software HD device service header file.
///
///	Copyright (c) 2012 by durchflieger.  All Rights Reserved.
///
///	Contributor(s):
///
///	License: AGPLv3
///
///	This program is free software: you can redistribute it and/or modify
///	it under the terms of the GNU Affero General Public License as
///	published by the Free Software Foundation, either version 3 of the
///	License.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU Affero General Public License for more details.
///
///	$Id$
//////////////////////////////////////////////////////////////////////////////

#pragma once

#define ATMO_GRAB_SERVICE	"SoftHDDevice-AtmoGrabService-v1.0"
#define ATMO1_GRAB_SERVICE	"SoftHDDevice-AtmoGrabService-v1.1"
#define OSD_3DMODE_SERVICE	"SoftHDDevice-Osd3DModeService-v1.0"

enum
{ GRAB_IMG_RGBA_FORMAT_B8G8R8A8 };

typedef struct
{
    int structSize;

    // request data
    int analyseSize;
    int clippedOverscan;

    // reply data
    int imgType;
    int imgSize;
    int width;
    int height;
    void *img;
} SoftHDDevice_AtmoGrabService_v1_0_t;

typedef struct
{
    int Mode;
} SoftHDDevice_Osd3DModeService_v1_0_t;

typedef struct
{
    // request/reply data

    int width;
    int height;

    // reply data

    int size;

    void *img;
} SoftHDDevice_AtmoGrabService_v1_1_t;
