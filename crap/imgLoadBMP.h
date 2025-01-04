//-------------------------------------------------------------------
//  File:   imgLoadBMP.h
//  Created:  07/28/00 2:23:PM
//  Author:   Aaron Hilton
//  Comments: Load a windows BMP file into an RImage object.
//-------------------------------------------------------------------
#ifndef __imgLoadBMP_h_
#define __imgLoadBMP_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RImage;

bool imgLoadBMP(char* szImageName, RImage *img);

#endif // __imgLoadBMP_h_
//-------------------------------------------------------------------
//  History:
//  $Log:$
//-------------------------------------------------------------------
