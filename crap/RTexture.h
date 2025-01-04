//-------------------------------------------------------------------
//	File:		  Texture.h
//	Created:	05/26/99 2:30:AM
//	Author:		Aaron Hilton
//	Comments:	Create and manage OpenGL textures.
//-------------------------------------------------------------------
#ifndef __Texture_h_
#define __Texture_h_

#include "RImage.h"

class RTexture
{
public:
	RTexture();
	~RTexture();

  // Create our texture with an image loader.
  // Usage:   myTexture.Create( "Nifty.bmp", imgLoadBMP );
  bool Create( char *szImageName, fncImageLoader imgLoader);

	// Create an OpenGL Texture, with our Image class, for input.
  // When using pAlpha, it must be 8bits and pImage must be 24bits.
  // Usage:   myTexture.Create( myImage );
  // or:      myTexture.Create( myImage, my8bitAlphaMap );
	bool Create(RImage *pImage, RImage *pAlpha=0);
	void Toast();

	void Use();

protected:
	unsigned int m_nID;
};

#endif // __Texture_h_
//-------------------------------------------------------------------
//	History:
//	$Log:$
//-------------------------------------------------------------------
