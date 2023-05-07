#ifndef PRE_COMPILE_DEF_H
#define PRE_COMPILE_DEF_H

#include "commonDef.h"

// standard
#include <fcntl.h>
#include <cstdio>
#include <cassert>
#include <time.h>
#include <cfloat>
#ifdef FC_OS_WIN32
#define _USE_MATH_DEFINES
#endif // FC_OS_WIN32
#include <cmath>
#include <climits>
#include <cstring>
#ifdef FC_OS_WIN32
#include <direct.h>

#ifndef 	WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#ifndef 	NOMINMAX
#	define NOMINMAX
#endif 
#endif 

#include <windows.h>
#include <crtdbg.h>
#include <shellapi.h>
#include <Rpc.h>
#endif

#include "commonType.h"

#endif // PRE_COMPILE_DEF_H

