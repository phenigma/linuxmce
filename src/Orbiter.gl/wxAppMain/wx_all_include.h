//
// Author : Remus C.
//
// Changed by : ...
//

// DO NOT CHANGE THIS FILE
// included automatically before other .h files
// see the corresponding .cpp file

#ifndef _WX_ALL_INCLUDE_H_
#define _WX_ALL_INCLUDE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx_all_include.h"
#endif

#include "wx_lib_compatibility.h"
#include "wx_config.h"
#include "define_cond.h"
#include "define_logger.h"
#include "wx_logger.h"
#include "wx_helpers.h"
#include "wx_main.h"
#include "wx_debugging.h"

#include "wx/dynarray.h"
#include <string>
#include <map>
#include <list>
using namespace std;

#ifdef WX_SDL_DEMO
#include "SDL/SDL.h"
#endif // WX_SDL_DEMO

#include "wx_extern_helpers.h"

#endif
// _WX_ALL_INCLUDE_H_
