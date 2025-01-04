#ifndef GL_FONTRENDERER_H
#define GL_FONTRENDERER_H

#ifdef WIN32
	#include <windows.h>		// Header File For Windows
	#include <gl\gl.h>			// Header File For The OpenGL32 Library
	#include <gl\glu.h>			// Header File For The GLu32 Library
	#include <windowsx.h>	//Windows header with some kewl extras
	#include <mmsystem.h>
	#include <iostream>			
	#include <conio.h>
	#include <io.h>
	#include <malloc.h>
#else
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#endif

#include <stdio.h> 
#include <stdarg.h>
#include <stdlib.h>

#include <memory.h>
#include <string.h>
#include <math.h>

#include <fcntl.h>
#include <time.h>


//TGA image structure

typedef struct S3DTGA_TYP
{
	GLuint	 ID;				//Texture ID used to select a texture
	GLubyte* data;				//Image data (up to 32 bits)
	GLuint	 bpp;				//Image color depth in bits per pixel
	GLuint	 width;				//Image width
	GLuint	 height;			//Image height
} S3DTGA, *S3DTGA_PTR;	

bool TGA_Load(S3DTGA_PTR, char*,
			  GLfloat, GLfloat);

//Quad structure, that contains the four vertex's
typedef struct QUAD_TYP
	{
	GLfloat ulvertex[3];
	GLfloat urvertex[3];
	GLfloat llvertex[3];
	GLfloat lrvertex[3];
	} QUAD, *QUAD_PTR;	


class FontRenderer{
	public:
		FontRenderer();
		~FontRenderer();
		void Init(char*);
		void Shutdown();
		void Printf(int x, int y, const char *string, ...);
	private:
		GLuint base;
		S3DTGA font_texture;
};


#endif