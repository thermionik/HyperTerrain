//-------------------------------------------------------------------
//  File:     RImage.h
//  Created:  08/27/00 2:09:AM
//  Author:   Aaron Hilton
//  Comments: Basic image data container.
//            Image Loaders can use RImage as a data container.
//            Look for imgLoadBMP for an example of an image loader.
// Usage:
//  // On initialization of program.
//  RImage img;
//  img.Create( 128, 128, 8 );
//
//  // Middle of Render Loop.
//  char *pTmp = (char*)img.GetData();
//  DrawPlasmaField( 128, 128, pTmp );
//  // You could combine with RLiveTexture, to translate 8bit
//  // colors into 32bit, and throw that to the screen.
//
//  // On shutdown of program.
//  img.Toast();
//-------------------------------------------------------------------
#ifndef __RImage_h_
#define __RImage_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RImage;
typedef bool (*fncImageLoader)(char* szImageName, RImage *img);

class RImage
{
public:
	RImage();
	virtual ~RImage();

  // Create using an image loader.
  // Usage:   myTexture.Create( "Nifty.bmp", imgLoadBMP );
  bool Create( char *szImageName, fncImageLoader imgLoader);

  // Create using basic parameters to allocate an image buffer.
  // Usage:   myImage.Create( 128, 256, 24 );  // Creates a 128x256x24bpp image.
  bool Create( UInt32 width, UInt32 height, UInt32 bpp );
  virtual void Toast();

  int     GetBPP()    {return m_nBPP;}    // Bits per pixel. Valid types are 8(intensity), 24(RGB), 32(RGBA)
  UInt32  GetWidth()  {return m_nWidth;}  // Width of image.
  UInt32  GetStride() {return m_nStride;} // Number of bytes in each horizontal scan line.
  UInt32  GetHeight() {return m_nHeight;} // Height of image.

  UInt32  GetImageSize() {return m_nData;}// Allocated pixel data space used by image.
  void*   GetData()   {return m_pData;}   // Get a pointer to the pixel data.

protected:
  int     m_nBPP;
  UInt32  m_nWidth, m_nHeight, m_nStride;
  UInt32  m_nData;
  void*   m_pData;
};

#endif // __RImage_h_
//-------------------------------------------------------------------
//  History:
//  $Log:$
//-------------------------------------------------------------------
