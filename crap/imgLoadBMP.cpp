//-------------------------------------------------------------------
//  File:   imgLoadBMP.cpp
//  Created:  07/28/00 2:26:PM
//  Author:   Aaron Hilton
//  Comments: Load a windows BMP file into an RImage object.
//-------------------------------------------------------------------
#include "std.h"
#include "imgLoadBMP.h"
#include "RImage.h"
#include "IOFile.h"

typedef struct bmpFileHeader_tag_
{
  UInt8  B, M;
  UInt32 nSize;
  UInt32 nReserved;
  UInt32 nOffsetToBits;
} BMPFileHeader;

typedef struct bmpImageHeader_tag_
{
  UInt32 nSize;
  Int32  nWidth;
  Int32  nHeight;
  UInt16 nPlanes;
  UInt16 nBPP;
  UInt32 nCompression;
  UInt32 nImageSize;
  Int32  nWidthPPM;
  Int32  nHeightPPM;
  UInt32 nColorUsed;
  UInt32 nColorImportant;
} BMPImageHeader;

// This is our actual BMP image loading function.
bool imgLoadBMP(char* szImageName, RImage *img)
{
  BMPFileHeader bfh;
  BMPImageHeader bih;
  UInt8 *pBMPData=0;

  IOFile filebmp; // File of BMP image.

  // Make sure we have an image object!
  if( img == 0 )
      return false;

  // Create a read-only file object.
  if( filebmp.Create( szImageName, "rb" ) == false )
      return false;

  // Make sure we toast the file object and delete the image data,
  // at the bottom under __finally{}.

  // Individually load the data, so we can validate and swizzle as we move.
  // (Swizzling is required for endian type swapping for other platforms.)
  // Just so ya know, intel uses the legacy little endian format which is
  // little end first, or like Day-Month-Year. Instead of the more metric
  // form of Year-Month-Day.  So, bytes are ordered with the most significant
  // parts first.
  filebmp >> bfh.B; filebmp >> bfh.M;
  filebmp >> bfh.nSize;
  filebmp >> bfh.nReserved;
  filebmp >> bfh.nOffsetToBits;

  // Make sure we're loading a BMP file.
  if(( bfh.B != 'B' ) || (bfh.M != 'M') || filebmp.IsEOF())
  {
    filebmp.Toast();
    return false;
  }

  // Finish loading in the actual image header.
  filebmp >> bih.nSize;
  filebmp >> bih.nWidth;
  filebmp >> bih.nHeight;
  filebmp >> bih.nPlanes;
  filebmp >> bih.nBPP;
  filebmp >> bih.nCompression;
  filebmp >> bih.nImageSize;
  filebmp >> bih.nWidthPPM;
  filebmp >> bih.nHeightPPM;
  filebmp >> bih.nColorUsed;
  filebmp >> bih.nColorImportant;

  // Validate the image header results.
  if( (bih.nCompression != 0) ||
      (bih.nHeight == 0) ||
      (bih.nWidth == 0) )
  {
    filebmp.Toast();
    return false;
  }

  // We must have either an 8, 24, or 32bit image.
  if( (bih.nBPP != 8) && (bih.nBPP != 24) && (bih.nBPP != 32) )
  {
    filebmp.Toast();
    return false;
  }

  // Jump to the image data.
  filebmp.SeekFromStart( bfh.nOffsetToBits );

  // Prep the image data for loading.
  UInt32 nStride = (bih.nWidth * (bih.nBPP/8) + 1) & (~1); // Stride is the BMP's bytes per row.
  bih.nImageSize = nStride * bih.nHeight;
  pBMPData = new UInt8[bih.nImageSize];
  if( pBMPData == 0 )
  {
    filebmp.Toast();
    return false;
  }

  // Load it up.. we want the image data raw.
//  for (int i=bih.nHeight-1; i; i--)
  for (signed int i=bih.nHeight-1; i>=0; i--)
  {
	  if( filebmp.Read( pBMPData+i*nStride, nStride ) == false )
	  {
		filebmp.Toast();
		delete pBMPData;
		return false; // In case we hit an IOFile error by now.
	  }
  }

/*  if( filebmp.Read( pBMPData, bih.nImageSize ) == false )
  {
    filebmp.Toast();
    delete pBMPData;
    return false; // In case we hit an IOFile error by now.
  }*/

  // Create our target Image.
  img->Create( bih.nWidth, bih.nHeight, bih.nBPP );

    // Typically our Image stride is shorter than Microsoft's BMP stride.
    //  We don't bother with byte aligning the image data, even
    //  though that COULD be a great speed boost for odd size images.

  // Swizzle, and process the BMP image data to GL compatible RGBA data.
  bool bFlipY;
  if( bih.nHeight < 0 )  // Negative height is top to bottom, right side up.
  {
    bih.nHeight = -bih.nHeight;
    bFlipY = false;
  }
  else // Positive height is bottom to top, or upside down.
    bFlipY = true;

  UInt8 *pDest=(UInt8*)img->GetData(), *pSrc;
  for( UInt32 y=0; y<(UInt32)bih.nHeight; y++ )
  {
    if( bFlipY ) // Handle pointer calculations for yFlip situations.
      pSrc = pBMPData + bih.nImageSize - (y * nStride) - nStride;
    else
      pSrc = pBMPData + (y * nStride);

    // Perform row copy of image data, and in 24 & 32bit cases,
    // swizzle those blue and red bytes around.
    switch( bih.nBPP )
    {
      case 8:
      {
        memcpy( pDest, pSrc, img->GetStride() );
      }
      break;

      case 24:  // from BGR order to RGB. So swap blue and red bytes.
      {
        for( UInt32 x=0; x<img->GetStride(); x+=3 )
        {
          pDest[x] = pSrc[x+2];
          pDest[x+1] = pSrc[x+1];
          pDest[x+2] = pSrc[x];
        }
      }
      break;

      case 32:  // 32bit is just like 24bit, but we go from BGRA to RGBA,
      {         // so green and alpha are left alone.
        for( UInt32 x=0; x<img->GetStride(); x+=4 )
        {
          pDest[x] = pSrc[x+2];
          pDest[x+1] = pSrc[x+1];
          pDest[x+2] = pSrc[x];
          pDest[x+3] = pSrc[x+3];
        }
      }
      break;

      default: // Only supporting 8, 24, and 32 image formats.
      {
        filebmp.Toast();
        delete pBMPData;
        img->Toast();
        return false;
      }
    }

    pDest += img->GetStride();
  }

  // Done loading image!
  filebmp.Toast();
  delete pBMPData;
  return true;
}


//-------------------------------------------------------------------
//  History:
//  $Log:$
//-------------------------------------------------------------------
