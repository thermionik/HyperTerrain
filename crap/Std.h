//-------------------------------------------------------------------
//  File:   Std.h
//  Created:  02/12/00 9:01:PM
//  Author:   Aaron Hilton
//  Comments: Standard header file to include all source files.
//            (Precompiled header)
//-------------------------------------------------------------------
#ifndef __STD_h_
#define __STD_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "GLEXT.h"

#include "PTypes.h"

#endif // _STD_h_