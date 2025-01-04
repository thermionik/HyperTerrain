/* skybox.h - By Lucas McLane            */
/* for Travis McLane's DScape 2002, 2003 */

#ifdef WIN32
#include <windows.h>									// Header File For Windows
#include <dinput.h>										// Direct Input Functions ( Add )
#include <math.h>										// Math Library Header File
#include <stdio.h>										// Header File For Standard Input/Output
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include "gl\glaux.h"					     			// Header File For The Glaux Library
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE	0x812F
#endif

int DrawSkyBox(GLvoid);
int LoadSkyTextures(GLvoid);

class TextureMap
{
	public:

	GLubyte* data;			// image data
	unsigned int bpp;		// bits per pixel
	unsigned int width;		
	unsigned int height;	
	unsigned int ID;		

	bool LoadTGA(char* filename, GLfloat minFilter, GLfloat maxFilter);

    TextureMap() { }
   ~TextureMap()
	{
	    if(data)
		{
			glDeleteTextures(1, &ID);
			delete data;
		}	
	}
};

class SkyBox
{
	public:
	
    SkyBox() { }
   ~SkyBox() { }   	
	void Render( TextureMap *SkyTexture );
};