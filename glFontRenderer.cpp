#include "glFontRenderer.h"

FontRenderer::FontRenderer(){};
FontRenderer::~FontRenderer(){
	Shutdown();
};

//------------------------------------------------------------------//
//- GLvoid SHINING3D::Font_Init(GLvoid) ----------------------------//
//------------------------------------------------------------------//
//- Description: This function initiates the font engine. Just put -//
//-              a call to this function in the initiation part of -//
//-              your program.									   -//
//------------------------------------------------------------------//
//- Sample use:													   -//
//-             Font_Init();									   -//
//------------------------------------------------------------------//
void FontRenderer::Init(char* filename)	
	{
	int loop;
	float cx=0;
	float cy=0;

	TGA_Load(&font_texture, filename, 
			 GL_LINEAR, GL_LINEAR);

	base=glGenLists(95);						//Creating a display list that can hold 95
	glBindTexture(GL_TEXTURE_2D, font_texture.ID);//Bind our font texture
	for(loop=0; loop<95; loop++)				//Loop through the display list
		{
		cx=float(loop%16)/16.0f;				//X position of current character
		cy=float(loop/16)/8.0f;					//Y position of current character

		glNewList(base+loop,GL_COMPILE);		//Start building a list
		glBegin(GL_TRIANGLE_STRIP);			//Use a quad for each character
			//Top right (tex/vertex) coordinates
			glTexCoord2f((float)(cx+0.0815f), (float)(1.0-cy));		
			glVertex2i(16,16);
			//Top left (tex/vertex) coordinates
			glTexCoord2f((float)(cx),		  (float)(1.0-cy));	
			glVertex2i(0,16);
			//Bottom right (tex/vertex) coordinates
			glTexCoord2f((float)(cx+0.0815f), (float)(1.0-cy-0.120)); 
			glVertex2i(16,0);
			//Bottom left (tex/vertex) coordinates
			glTexCoord2f((float)(cx),		  (float)(1.0-cy-0.120)); 
			glVertex2i(0,0);
		glEnd();							//Done Building Our Quad (Character)
		glTranslated(12,0,0);				//Move To The Right Of The Character
	glEndList();							//Done Building The Display List
	}															// Loop Until All 256 Are Built
}

//------------------------------------------------------------------//
//- GLvoid SHINING3D:: Font_Shutdown(GLvoid) -----------------------//
//------------------------------------------------------------------//
//- Description: This function shuts down the font engine.  Put a  -//
//-              call to this in the shutdown part of your program.-//
//------------------------------------------------------------------//
//- Sample use:													   -//
//-             Font_Shutdown();								   -//
//------------------------------------------------------------------//
void FontRenderer::Shutdown(){
	glDeleteLists(base, 96); 
}


//------------------------------------------------------------------//
//- GLvoid SHINING3D::glPrint(GLint, GLint, const char*, ...) ------//
//------------------------------------------------------------------//
//- Description: This function prints a string of text onto the    -//
//-				 the screen.  You *must* be in ortho view to use   -//
//-              this function, but thats not a bad thing, it makes-//
//-              positioning the text easier.  Also, this function -//
//-              functions (hehehe) almost exactly like printf(...)-//
//-              in dos.										   -//
//------------------------------------------------------------------//
//- Sample use:													   -//
//-             glPrint(120, 120, "Hi, I am %d years old", 16);	   -//
//------------------------------------------------------------------//
void FontRenderer::Printf(int x, int y, const char *string, ...)
{
	char		text[256];										// Holds Our String
	va_list		va;												// Pointer To List Of Arguments
	int len;
        
	if(string==NULL)											// If There's No Text
		return;													// Do Nothing

	va_start(va, string);										// Parses The String For Variables
	    vsprintf(text, string, va);								// And Converts Symbols To Actual Numbers
	va_end(va);													// Results Are Stored In Text
//	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, font_texture.ID);			// Select Our Font Texture
	glPushMatrix();												// Store The Modelview Matrix
	glLoadIdentity();											// Reset The Modelview Matrix
	glTranslated(x,y,0);										// Position The Text (0,0 - Bottom Left)
	glListBase(base-32);										// Choose The Font Set
	len = strlen(text);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// Draws The Display List Text
	glPopMatrix();												// Restore The Old Projection Matrix
//        glEnable(GL_FOG);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

}


//------------------------------------------------------------------//
//- BOOL TGA_Load(TGA_PTR, char*) ----------------------------------//
//------------------------------------------------------------------//
//- Description: This function loads a TGA image.  You must first  -//
//-              create a S3DTGA object, and then pass it's address-//
//-              as this function's first argument.  Then you must -//
//-              provide the tga's filename.  Last, but not least, -//
//-              you need to provide the texture's filter (using   -//
//-              the regular OpenGL filter constants). The function-//
//-              returns true if everything went ok. The function  -//
//-              also writes a status to the error log to tell you -//
//-              exactly what went wrong, or if nothing did go     -//
//-              wrong, exactly what file was loaded.			   -//
//------------------------------------------------------------------//
//- Sample use:													   -//
//-              S3DTGA tga;									   -//
//-              TGA_Load(&tga, "Art/mytga.tga",				   -//
//-                       GL_LINEAR, GL_LINEAR);                   -//
//------------------------------------------------------------------//
bool TGA_Load(S3DTGA_PTR image, char* filename,
			  GLfloat minfilter, GLfloat maxfilter)	
	{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	//Uncompressed TGA header
	GLubyte		TGAcompare[12];								//Used to compare TGA header
	GLubyte		header[6];									//The first six useful bytes from the header
	GLuint		bytesPerPixel;								//Holds the bpp of the TGA
	GLuint		imageSize;									//Used to store image size while in RAM
	GLuint		temp;										//Temp variable
	GLuint		type=GL_RGBA;								//Set the default OpenGL mode to RBGA (32 BPP)
        GLuint		texture = 0;
        
	FILE* file = fopen(filename, "rb");						// Open The TGA File

	if(file==NULL													   ||	// Does File Even Exist?
	   fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
	   memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0			   ||	// Does The Header Match What We Want?
	   fread(header,1,sizeof(header),file)!=sizeof(header))					// If So Read Next 6 Header Bytes
		{
		if(file==NULL)									// Did The File Even Exist? *Added Jim Strong*
			{
			printf("%s does not exist.", filename);
			return false;							
			}
		else
			{
			fclose(file);						// If anything failed, close the file
			printf("Could not load %s correctly, general failure.", filename);
			return false;						
			}
		}

	image->width  = header[1] * 256 + header[0];		// Determine The TGA Width	(highbyte*256+lowbyte)
	image->height = header[3] * 256 + header[2];		// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(image->width	<=0	||								// Is The Width Less Than Or Equal To Zero
	   image->height<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))				// Is The TGA 24 or 32 Bit?
		{
		fclose(file);									// If Anything Failed, Close The File
		printf("%s's height or width is less than zero, or the TGA is not 24 or 32 bits.", filename);
		return false;							
		}

	image->bpp		= header[4];						// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= image->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= image->width * image->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	image->data=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(image->data==NULL ||							// Does The Storage Memory Exist?
	   fread(image->data, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
		{
		if(image->data!=NULL)						// Was Image Data Loaded
			free(image->data);						// If So, Release The Image Data
		
		printf("Storage memory for %s does not exist or is corrupted.", filename);
		
		fclose(file);								// Close The File
		return false;								// Return False
		}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
		{										// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=image->data[i];					// Temporarily Store The Value At Image Data 'i'
		image->data[i] = image->data[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		image->data[i + 2] = temp;				// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
		}

	fclose (file);								//Close the file

	// Build A Texture From The Data

//        GLuint *ppid;
//        ppid = &image->ID;
//	glGenTextures(1, ppid);				//Generate OpenGL texture IDs
//        image->ID = *ppid;

	glGenTextures(1, &image->ID);				//Generate OpenGL texture IDs

        glBindTexture(GL_TEXTURE_2D, image->ID);			//Bind the texture to a texture object 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);	//Filtering for if texture is bigger than should be
//  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxfilter);	//Filtering for if texture is smaller than it should be
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
//  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
 // Upload the texture, and finish our work.
 //   gluBuild2DMipmaps(GL_TEXTURE_2D, 2, imgFinal.GetWidth(), imgFinal.GetHeight(),
 //                     GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, imgFinal.GetData());
	

	if(image->bpp==24)							//Was the TGA 24 bpp?
		type=GL_RGB;	
	
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image->width, image->height,
                      type, GL_UNSIGNED_BYTE, image->data);	

	//glTexImage2D(GL_TEXTURE_2D, 0, type, image->width, image->height, 0, type, GL_UNSIGNED_BYTE, image->data);

	printf("Loaded %s correctly.", filename);
	return true;
	}



