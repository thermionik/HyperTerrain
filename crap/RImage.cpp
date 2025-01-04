//-------------------------------------------------------------------
//  File:     RImage.h
//  Created:  08/27/00 2:09:AM
//  Author:   Aaron Hilton
//  Comments: Basic image data container.
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
#include "std.h"
#include "RImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RImage::RImage()
{
  m_nBPP = 0;
  m_nWidth = m_nHeight = m_nStride = 0;
  m_nData = 0;
  m_pData = 0;
}

RImage::~RImage()
{
  Toast();
}

// Create using an image loader.
// Usage:   myTexture.Create( "Nifty.bmp", imgLoadBMP );
bool RImage::Create( char *szImageName, fncImageLoader imgLoader)
{
  return imgLoader( szImageName, this );
}

// Create using basic parameters to allocate an image buffer.
// Usage:   myImage.Create( 128, 256, 24 );  // Creates a 128x256x24bpp image.
bool RImage::Create( UInt32 width, UInt32 height, UInt32 bpp )
{
  Toast(); // Nuke any old image data.

  if( width==0 || height==0 || bpp==0 )
    return false;

  m_nWidth = width;
  m_nStride = width * ((bpp+7)/8);
  m_nHeight = height;
  m_nBPP = bpp;

  m_nData = m_nStride * m_nHeight;
  m_pData = new BYTE[m_nData];
  if( m_pData == 0 )
    return false;

  return true;
}


void RImage::Toast()
{
  if( m_pData )
  {
		delete [] (BYTE *) m_pData;
    m_pData = 0;
    m_nData = 0;
    m_nBPP = 0;
    m_nWidth = m_nHeight = m_nStride = 0;
  }
}
