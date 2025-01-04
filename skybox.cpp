/* skybox.cpp - By Lucas McLane          */
/* for Travis McLane's DScape 2002, 2003 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>										// Header File For Standard Input/Output
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>									// Header File For The GLu32 Library
#include "gl/glaux.h"									// Header File For The Glaux Library

#include "skybox.h"

AUX_RGBImageRec *LoadBMP(char *Filename);


GLuint	texture[6];			// Storage For 3 Textures

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}



void SkyBox::Render( TextureMap *SkyBox )
{
	TextureMap *SkyBoxPtr = SkyBox;
	
/*	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
*/
        glDisable(GL_LIGHTING);
        
	glColor3f(1.0f, 1.0f, 1.0f);

	float SkyPoints[8][3] =	   //The skybox vertices...
	{						   //(created this way to avoid gaps between sides)
		{-1.0f, -1.0f, -1.0f}, // 0
		{ 1.0f, -1.0f, -1.0f}, // 1
		{ 1.0f,  1.0f, -1.0f}, // 2
		{-1.0f,  1.0f, -1.0f}, // 3
		{-1.0f, -1.0f,  1.0f}, // 4
		{ 1.0f, -1.0f,  1.0f}, // 5
		{ 1.0f,  1.0f,  1.0f}, // 6
		{-1.0f,  1.0f,  1.0f}, // 7
	};	

	glVertexPointer(3, GL_FLOAT, 0, SkyPoints);
//	glScalef(0.5f, 0.5f, 0.5f);

	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID);
	glBegin(GL_QUADS);
		glNormal3f( -1.0, 0.0, 0.0);
		glTexCoord2f(1.0f, 0.0f); glArrayElement(1); //Right face
		glTexCoord2f(1.0f, 1.0f); glArrayElement(2);
		glTexCoord2f(0.0f, 1.0f); glArrayElement(6);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(5);
	glEnd();

	SkyBoxPtr++;
	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID);
	glBegin(GL_QUADS);
		glNormal3f( 0.0, 0.0, 1.0);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(0); //Left face
		glTexCoord2f(1.0f, 0.0f); glArrayElement(4);
		glTexCoord2f(1.0f, 1.0f); glArrayElement(7);
		glTexCoord2f(0.0f, 1.0f); glArrayElement(3);
	glEnd();
	
	SkyBoxPtr++;
	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID );
	glBegin(GL_QUADS);
		glNormal3f( 0.0, 1.0, 0.0);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(4); //Bottom face
		glTexCoord2f(1.0f, 0.0f); glArrayElement(5);
		glTexCoord2f(1.0f, 1.0f); glArrayElement(6);
		glTexCoord2f(0.0f, 1.0f); glArrayElement(7);
	glEnd();

	SkyBoxPtr++;
	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID);
	glBegin(GL_QUADS);
		glNormal3f( 0.0, -1.0, 0.0);
		glTexCoord2f(1.0f, 0.0f); glArrayElement(0); //Top face
		glTexCoord2f(1.0f, 1.0f); glArrayElement(3);
		glTexCoord2f(0.0f, 1.0f); glArrayElement(2);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(1);
	glEnd();

	SkyBoxPtr++;
	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID);
	glBegin(GL_QUADS);			
		glNormal3f( 0.0, 0.0, -1.0);
		glTexCoord2f(1.0f, 0.0f); glArrayElement(3); //Back face
		glTexCoord2f(1.0f, 1.0f); glArrayElement(7);
		glTexCoord2f(0.0f, 1.0f); glArrayElement(6);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(2);
	glEnd();

	SkyBoxPtr++;
	glBindTexture(GL_TEXTURE_2D, SkyBoxPtr->ID);
	glBegin(GL_QUADS);		
		glNormal3f( 1.0, 0.0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glArrayElement(0); //Front face
		glTexCoord2f(0.0f, 1.0f); glArrayElement(1);
		glTexCoord2f(0.0f, 0.0f); glArrayElement(5);
		glTexCoord2f(1.0f, 0.0f); glArrayElement(4);
	glEnd();

	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisable(GL_BLEND);

//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_FOG);

}

bool TextureMap::LoadTGA(char* filename, GLfloat minFilter, GLfloat maxFilter)	
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	//Uncompressed TGA header
	GLubyte		TGAcompare[12];					//Used to compare TGA header
	GLubyte		header[6];						//The first six useful bytes from the header
	GLuint		bytesPerPixel;					//Holds the bpp of the TGA
	GLuint		imageSize;						//Used to store image size while in RAM
	GLuint		temp;							//Temp variable
	GLuint		type=GL_RGBA;					//Set the default OpenGL mode to RBGA (32 BPP)

	FILE* file = fopen(filename, "rb");			// Open The TGA File

	if(file==NULL													   ||	// Does File Even Exist?
	   fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
	   memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0			   ||	// Does The Header Match What We Want?
	   fread(header,1,sizeof(header),file)!=sizeof(header))					// If So Read Next 6 Header Bytes
	{
		if(file==NULL)							// Did The File Even Exist? *Added Jim Strong*
		{
			return false;							
		}
		else
		{
			fclose(file);						// If anything failed, close the file
			return false;						
		}
	}
	width = header[1] * 256 + header[0];		// Determine The TGA Width	(highbyte*256+lowbyte)
	height= header[3] * 256 + header[2];		// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(width	<=0	||							// Is The Width Less Than Or Equal To Zero
	   height<=0	||							// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))		// Is The TGA 24 or 32 Bit?
	{
		fclose(file);							// If Anything Failed, Close The File
		return false;							
	}

	bpp	 = header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel= bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize	 = width*height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	data= new GLubyte [imageSize];				// Reserve Memory To Hold The TGA Data

	if(data==NULL ||							// Does The Storage Memory Exist?
	   fread(data, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(data!=NULL)							// Was Image Data Loaded
			free(data);							// If So, Release The Image Data
				
		fclose(file);							// Close The File
		return false;							// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{										// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp	 =data[i];						// Temporarily Store The Value At Image Data 'i'
		data[i]	 = data[i + 2];					// Set The 1st Byte To The Value Of The 3rd Byte
		data[i+2]= temp;						// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);								//Close the file

	glGenTextures(1, &ID);						//Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, ID);			//Bind the texture to a texture object 
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, 
					  height, GL_RGB, GL_UNSIGNED_BYTE, data);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);	//Filtering for if texture is bigger than should be
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);	//Filtering for if texture is smaller than it should be
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(bpp==24)									//Was the TGA 24 bpp?
		type=GL_RGB;							

//	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);

	return true;
}

int LoadSkyTextures()									// Load Bitmaps And Convert To Textures
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[6];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	TextureImage[0]=LoadBMP("Art/skybox/Back.bmp");
	TextureImage[1]=LoadBMP("Art/skybox/Front.bmp");
	TextureImage[2]=LoadBMP("Art/skybox/Bottom.bmp");
	TextureImage[3]=LoadBMP("Art/skybox/Top.bmp");
	TextureImage[4]=LoadBMP("Art/skybox/Right.bmp");
	TextureImage[5]=LoadBMP("Art/skybox/Left.bmp");

	for(int count=0; count < 6; count++)
	{
	
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, &texture[count]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[count]);
//		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[count]->sizeX, TextureImage[count]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[count]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[count]->sizeX, 
					  TextureImage[count]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[count]->data);

	

		if (TextureImage[count])									// If Texture Exists
		{
			if (TextureImage[count]->data)							// If Texture Image Exists
			{
				free(TextureImage[count]->data);					// Free The Texture Image Memory
			}

			free(TextureImage[count]);								// Free The Image Structure
		}
	}


	return Status;										// Return The Status
}


int DrawSkyBox(GLvoid)									// Here's Where We Do All The Drawing
{
	
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
//	glLoadIdentity();									// Reset The View
//	glTranslatef(0.0f,0.0f,z);

	glScalef(2000.0, 2000.0, 2000.0);
	float x, y, z;
	x = 1.0f;
	y = 1.0f;
	z = 1.0f;

	glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f( x, -y, -z);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f( x,  y, -z);
        glTexCoord2f(0.0f, 1.0f);        glVertex3f(-x, y, -z);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(-x, -y, -z);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f(-x, -y, z);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f(-x, y, z);
        glTexCoord2f(0.0f, 1.0f);        glVertex3f(x,  y, z);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x,  -y, z);
    glEnd();
     

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f(-x, -y, -z);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f(-x, -y, z);
        glTexCoord2f(0.0f, 1.0f);        glVertex3f(x,  -y, z);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x,  -y, -z);
    glEnd();
      

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);        glVertex3f(x, y, -z);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x, y, z);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f(-x,y, z);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f(-x,y, -z);
    glEnd();
       

    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f(-x, y, -z);
        glTexCoord2f(0.0f, 1.0f);        glVertex3f(-x, y, z);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(-x, -y, z);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f(-x, -y, -z);
    glEnd();
        

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x, -y, -z);
        glTexCoord2f(1.0f, 0.0f);        glVertex3f(x, -y, z);
        glTexCoord2f(1.0f, 1.0f);        glVertex3f(x, y,  z);
		glTexCoord2f(0.0f, 1.0f);        glVertex3f(x, y, -z);
    glEnd();
	glScalef(0.0005, 0.0005, 0.0005);
	return TRUE;										// Keep Going
}


