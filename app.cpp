
/**************************************
*                                     *
*   Jeff Molofee's Basecode Example   *
*   SDL porting by Fabio Franchello   *
*          nehe.gamedev.net           *
*                2001                 *
*                                     *
**************************************/

// Includes
#ifdef WIN32													// If We're Under MSVC
#include <windows.h>											// Include The Windows Header
#else															// Otherwise
#include <stdio.h>												// Include The Standar IO Header
#include <stdlib.h>												// And The Standard Lib (for exit())
#endif															// Then...

#include <math.h>												// We Require The Math Lib For Sin and Cos
#include <GL/gl.h>												// And Obviously The OpenGL Header
#include <GL/glu.h>												// And The GLu Heander

#include "SDL/SDL.h"												// Finally: The SDL Header!
#include "SDL/SDL_opengl.h"
#include <assert.h>

#include "types.h"
#include "rawvolume.h"
#include "dpatch.h"
#include "wtimer.h"
#include "voxel.h"
#include "volume.h"
#include "glFontRenderer.h"
#include "skybox.h"


#include "main.h"												// We're Including theHeader Where Defs And Prototypes Are




#ifdef WIN32													// If We're Under MSVC
#pragma comment( lib, "OpenGL32.lib" )							// We Can Tell The Linker To Look For OpenGl32.lib ...
#pragma comment( lib, "GLu32.lib" )								// ...GLu32.lib ...
#pragma comment( lib, "SDLmain.lib" )							// ...SDLmain.lib ...
#pragma comment( lib, "SDL.lib" )								// And SDL.lib At Link Time
#endif															// For Other Platforms, Such As LINUX, The Link Flags Are Defined in The Makefile

int GenTexture(void);


extern S_AppStatus AppStatus;									// We're Using This Struct As A Repository For The Application State (Visible, Focus, ecc)

// User Defined Variables
float		angle;												// Used To Rotate The Triangles
float		cnt1, cnt2;											// Used To Move The Object On The Screen
float		Lx, Ly;												// We use all this variables to hold
Sint16		Ix, Iy;												// Values used for moving/rotating figures
int			rot1, rot2;											// Counter Variables
Sint16		IyOff, IxOff;										// Counter Variavles
SDL_Surface *NeHe_Logo;											// We have 3 logo: an NeHe Logo...
SDL_Surface *SDL_Logo;											// ...an SDL Logo...
SDL_Surface *SD_Logo;											// ...and a SD Logo
int t = 0;

DPatch dpatch;
Camera viewpoint;
RawVolume vol1;
FontRenderer output;
WTimer timer, water_animation;

int numframes = 0;
double fps = 0.0;
double elapsedtime = 0.0;
float Heading = 0.0;

bool normals;

float g_Lighting = 0.8;
int gDrawMode = DRAW_USE_FILL_ONLY;
bool gMoved = true;
bool paused = false;

void SetDrawModeContext();
bool gAnimating;
GLfloat light_position[] = {51002.0, 50.0, 512.0, 0.0 };
TextureMap SkyTextures[6]; 
SkyBox     WorldSky;

// Our function pointers for the ARB multitexturing functions
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
//glActiveTextureARB		= 
//glMultiTexCoord2fARB	= 
S3DTGA detail, base, water;



// Code
bool InitGL(SDL_Surface *S)										// Any OpenGL Initialization Code Goes Here
{
	glClearColor(0.0f,0.0f,0.0f,0.5f);							// Black Background
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing (Less Or Equal)
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel(GL_SMOOTH);									// Select Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	return true;												// Return TRUE (Initialization Successful)
}


bool Initialize(void)											// Any Application & User Initialization Code Goes Here
{
// Our function pointers for the ARB multitexturing functions

//glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)	wglGetProcAddress("glMultiTexCoord2fARB");
//glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2fARB");
glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTextureARB");

if(!glMultiTexCoord2fARB)
{
printf("Error!");
exit(1);
}

	LoadSkyTextures();

	AppStatus.Visible		= true;								// At The Beginning, Our App Is Visible
	AppStatus.MouseFocus	= true;								// And Have Both Mouse
	AppStatus.KeyboardFocus = true;								// And Input Focus

	/*	Came with NeheSDL Example...
	// Start Of User Initialization
	angle		= 0.0f;											// Set The Starting Angle To Zero
	cnt1		= 0.0f;											// Set The Cos(for the X axis) Counter To Zero
	cnt2		= 0.0f;											// Set The Sin(for the Y axis) Counter To Zero
	Lx			= 0.0f;
	Ly			= 0.0f;
	Iy			= 0;
	Ix			= 0;
	IyOff		= 1;
	IxOff		= 1;

	if(!(NeHe_Logo = SDL_LoadBMP("NeHe_Title.bmp")) ||			// We try to load the first...
		!(SDL_Logo = SDL_LoadBMP("SDL_Title.bmp")) ||			// ...the second...
		!(SD_Logo = SDL_LoadBMP("SD_Title.bmp")) )				// ..and the third Logo
	{
		Log("Cannot load graphic: %s\n", SDL_GetError() );
		return false;
	}

	SDL_SetColorKey(NeHe_Logo, SDL_SRCCOLORKEY|SDL_RLEACCEL,	// Now: we're setting the key color for
		SDL_MapRGB(NeHe_Logo->format, 0, 0, 0) );				// the logo surfaces. That is the color that
	SDL_SetColorKey(SDL_Logo, SDL_SRCCOLORKEY|SDL_RLEACCEL,		// we want to result traslucent when the
		SDL_MapRGB(SDL_Logo->format, 0, 0, 0) );				// surface is blit. Refer on the SDL docs
	SDL_SetColorKey(SD_Logo, SDL_SRCCOLORKEY|SDL_RLEACCEL,		// for more info about this
		SDL_MapRGB(SD_Logo->format, 0, 0, 0) );					// topic.
	*/ // End- Came with NeheSDL Example...
	    double left, right, bottom, top, nearp, farp, fovangle;
    
    nearp = 1.0f;
    farp = 4500.0f;
    fovangle = 90.0f;
    right = nearp*tan((fovangle / 2)* (M_PI/180));
    left = -right;
    top = right / ((GLfloat) kWindowWidth / (GLfloat) kWindowHeight);
    bottom = -top;
    
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
        	glEnable(GL_CULL_FACE);		// Cull back-facing triangle
    
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing 
	//glShadeModel(GL_SMOOTH);		// Enables Smooth Color Shading

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();							// Reset The Projection Matrix
        glFrustum(left, right, bottom, top, nearp, farp);                                                                                                                                                                                                                                // Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
	

	// ----------- LIGHTING SETUP -----------
	// Light values and coordinates
	GLfloat  whiteLight[]    = { 0.45f,  0.45f, 0.45f, 1.0f };
	GLfloat  ambientLight[]  = { 0.5f,  0.5f, 0.5f, 1.0f };
	GLfloat  diffuseLight[]  = { 0.25f,  0.25f, 0.25f, 1.0f };
	//GLfloat	 lightPos[]      = { .00f,300.00f, 0.00f, 0.0f };

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = {50.0};

	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);

	//glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set the color for the landscape
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, whiteLight );

	// ----------- TEXTURE SETUP -----------
	// Use Generated Texture Coordinates
	// Fog stuff...
    /*
	glEnable(GL_FOG);
    {
	   GLfloat density = 0.001;
	   GLfloat fogColor[4] = {0.5, 0.5, 1.0, 0.3};
	   GLint fogMode;
	   fogMode = GL_LINEAR; //GL_LINEAR, GL_EXP, GL_EXP2
	   glFogi(GL_FOG_MODE, fogMode);
	   glFogfv(GL_FOG_COLOR, fogColor);
	   glFogf(GL_FOG_DENSITY, density);
	  // glHint(GL_FOG_HINT, GL_DONT_CARE);
	   glHint(GL_FOG_HINT, GL_NICEST);
//	   	glEnable(GL_FOG);						// Turn on fog
//	glFogi(GL_FOG_MODE, GL_LINEAR);			// Set the fog mode to LINEAR (Important)
//	glFogfv(GL_FOG_COLOR, fogColor);		// Give OpenGL our fog color
	glFogf(GL_FOG_START, 0.0);				// Set the start position for the depth at 0
	glFogf(GL_FOG_END, 50.0);				// Set the end position for the detph at 50

    } */ /* this is a comment */
	//===============================
	// Old DScape main init stuff...
	//===============================
//	GenTexture();
    output.Init("Art/Font.tga");

    TGA_Load(&base, "Art/terrain-texture.tga", GL_LINEAR, GL_LINEAR);    
    TGA_Load(&detail, "Art/terrain-detail.tga", GL_LINEAR, GL_LINEAR);    
	TGA_Load(&water, "Art/water.tga", GL_LINEAR, GL_LINEAR);

    dpatch.SetTextureIDs(base.ID, detail.ID, water.ID);
    normals = false;


    vol1.Load("Data/heightmap.raw");
    dpatch.Initialize(&vol1, 0, 0); 
    dpatch.SetPriority(0.00125);

    Point3d loc;
    loc.x = 0.0;
    loc.z = 0.0;
	//  loc.y = vol1.GetHeight(loc.x, loc.z) + 15;
    loc.y = vol1.GetHeight(loc.x, loc.z) + 1.8;
    
    viewpoint.SetLocation(loc);

    timer.Start();
	water_animation.Start();
	return true;												// Return TRUE (Initialization Successful)
}


void Deinitialize(void)											// Any User Deinitialization Goes Here
{
	return;														// We Have Nothing To Deinit Now
}


void Update(Uint32 Milliseconds, Uint8 *Keys, Uint16 MouseX, Uint16 MouseY)	// Perform Motion Updates Here
{
	Point3d p;
    float Speed = 40.0;
    float MovePerFrame = 1 / fps;
    p = viewpoint.GetLocation();


	// Set The Mouse Position To The Center Of The Window	( Add )
	Heading += (float)(5 - MouseX)/100 * 15;
	viewpoint.gCameraRotation[ROTATE_PITCH] += (float)(5 - MouseY)/100 * 15;
	viewpoint.gCameraRotation[ROTATE_YAW] = 360.0f - Heading;
	gMoved = true;

	if(Keys)													// If We're Sent A Key Event With The Update
	{
		if(Keys[SDLK_ESCAPE])									// And If The Key Pressed Was ESC
		{
			TerminateApplication();								// Terminate The Application
		}

		if(Keys[SDLK_F1])										// If The Key Pressed Was F1
		{
			ToggleFullscreen();									// Use SDL Function To Toggle Fullscreen Mode (But Not In Windows :) )
		}
	
	
		// Key Bindings
		if(Keys[SDLK_e])										// If The Key Pressed Was F1
			{  
				float xmov = Speed*MovePerFrame * sin( viewpoint.gCameraRotation[ROTATE_YAW] * 3.14 / 180.0f );
				float zmov = -Speed*MovePerFrame * cos( viewpoint.gCameraRotation[ROTATE_YAW] * 3.14 / 180.0f);
				float height = (vol1.GetHeight(p.x, p.z) + 10.8f);

				if( (p.x+=xmov) >= 0 && (p.z+=zmov) >= 0 && (p.x+=xmov) < 1024 && (p.z+=zmov) < 1024 )
					viewpoint.Move(xmov, height, zmov);
   
				gMoved = true;
			}
		if (Keys[SDLK_d])
		{   
				float xmov = -Speed *MovePerFrame* sin( viewpoint.gCameraRotation[ROTATE_YAW] * 3.14 / 180.0f );
				float zmov = Speed *MovePerFrame * cos( viewpoint.gCameraRotation[ROTATE_YAW] * 3.14 / 180.0f );
				float height = // 8.0f * cos ( viewpoint.gCameraRotation[0] * 3.14 / 180.0f );//
					(vol1.GetHeight(p.x, p.z) + 10.8f);

				if( (p.x+=xmov) >= 0 && (p.z+=zmov) >= 0 && (p.x+=xmov) < 1024 && (p.z+=zmov) < 1024 )
					viewpoint.Move(xmov, height, zmov);
				gMoved = true;
		}
		
		if (Keys[SDLK_f])
		{
				float xmov = -Speed *MovePerFrame* sin( (viewpoint.gCameraRotation[ROTATE_YAW] - 90)* 3.14 / 180.0f );
				float zmov = Speed *MovePerFrame * cos( (viewpoint.gCameraRotation[ROTATE_YAW] - 90)* 3.14 / 180.0f );
				float height = // 8.0f * cos ( viewpoint.gCameraRotation[0] * 3.14 / 180.0f );//
					(vol1.GetHeight(p.x, p.z) + 10.8f);

				if( (p.x+=xmov) >= 0 && (p.z+=zmov) >= 0 && (p.x+=xmov) < 1024 && (p.z+=zmov) < 1024 )
					viewpoint.Move(xmov, height, zmov);
				gMoved = true;
		}

		if (Keys[SDLK_s])
		{
				float xmov = -Speed *MovePerFrame* sin( (viewpoint.gCameraRotation[ROTATE_YAW] + 90)* 3.14 / 180.0f );
				float zmov = Speed *MovePerFrame * cos( (viewpoint.gCameraRotation[ROTATE_YAW] + 90)* 3.14 / 180.0f );
				float height = // 8.0f * cos ( viewpoint.gCameraRotation[0] * 3.14 / 180.0f );//
					(vol1.GetHeight(p.x, p.z) + 10.8f);

				if( (p.x+=xmov) >= 0 && (p.z+=zmov) >= 0 && (p.x+=xmov) < 1024 && (p.z+=zmov) < 1024 )
					viewpoint.Move(xmov, height, zmov);
				gMoved = true;
		}
		if (Keys[SDLK_z])
		{
				viewpoint.gCameraRotation[0] += 3.0f;
				if (viewpoint.gCameraRotation[0] < 0)
					viewpoint.gCameraRotation[0] += 360;
				if (viewpoint.gCameraRotation[0] > 360)
					viewpoint.gCameraRotation[0] -= 360;
 
				gMoved = true;
		}
		if (Keys[SDLK_a])
		{
				viewpoint.gCameraRotation[0] -= 3.0f;
				if (viewpoint.gCameraRotation[0] < 0)
					viewpoint.gCameraRotation[0] += 360;
				if (viewpoint.gCameraRotation[0] > 360)
					viewpoint.gCameraRotation[0] -= 360;
            
				gMoved = true;
		}
		if (Keys[SDLK_i])
		{
			float prior = dpatch.GetPriority();
			prior -= 0.0001;
			if (prior < 0 )
				prior = 0;
			dpatch.SetPriority(prior);
			gMoved = true;
		}
		if (Keys[SDLK_n])
		{	
			g_Lighting += 0.01;
			if ( g_Lighting > 1.0 )
				g_Lighting = 1.0;

		  }
		if (Keys[SDLK_m])
		{
			g_Lighting -= 0.01;
			if ( g_Lighting < 0.0 )
				g_Lighting = 0.0;    
		}
		if (Keys[SDLK_g])
		{
			float prior = dpatch.GetPriority();
			prior += 0.0001;
			dpatch.SetPriority(prior);
			gMoved = true;
		}        
		if (Keys[SDLK_t])
			{
				gDrawMode++;
				if (gDrawMode > DRAW_USE_WIREFRAME)
					gDrawMode = DRAW_USE_TEXTURE;
				SetDrawModeContext();
			}
		if (Keys[SDLK_c])
		{
			float height = (vol1.GetHeight(0.0, 0.0) + 8.0f);
			viewpoint.position.x = viewpoint.position.z = 0.0;
			gMoved = true;
	//        cout << endl<<"X : "<<viewpoint.position.x<<" Y : "<<viewpoint.position.z;
		}

		if (Keys[SDLK_q])
		{
		//	dpatch.~DPatch();
			output.Shutdown();
			exit(0);
		}
		if (Keys[SDLK_o])
		{
			if (gAnimating)
				gAnimating = false;
			else
				gAnimating = true;
		}
		if (Keys[SDLK_p])
		{
			if ( paused )
				paused = false;
			else
				paused = true;

		}
		if (Keys[SDLK_b])
			dpatch.ToggleSplitOnly();
		
	}

	angle += (float)(Milliseconds) / 5.0f;						// Update Angle Based On The Clock
	cnt1  += 0.010f;											// Update the Cos Counter Based On The Clock
	cnt2  += 0.008f;											// And The Same Thing For The Sin Counter
	Lx    += 0.035f;
	Ly    += 0.030f;

	return;														// We Always Make Functions Return
}

void Draw(SDL_Surface *Screen)									// Our Drawing Code
{
	assert(Screen);												// We won't go on if the Screen pointer is invalid

//	Draw3D(Screen);												// We split our drawing code into two pieces:
//	Draw2D(Screen);												// First draw the 3d stuff, then blit 2d surfaces onto it

	    int numDrawn = 0;
//    GLUquadricObj *D;

    numframes++;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    
    glLoadIdentity();						// Reset The View
    viewpoint.Display();
 
   // glPushMatrix();
    //glScalef( 1024.0, 1024.0, 1024.0 );
   // glScalef( 1024.0, 5000.0, 5000.0 );
   //WorldSky.Render(&SkyTextures[0]);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPolygonMode(GL_FRONT, GL_FILL);

	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);
	glPushMatrix();
	glLoadIdentity();
    glRotatef(viewpoint.gCameraRotation[ROTATE_PITCH], 1.f, 0.f, 0.f);
    glRotatef(viewpoint.gCameraRotation[ROTATE_YAW],   0.f, 1.f, 0.f);
	glTranslatef( viewpoint.GetLocation().x, viewpoint.GetLocation().y, viewpoint.GetLocation().z );
	DrawSkyBox();
	glPopMatrix();

   // glPopMatrix();

   // glLoadIdentity();
   // viewpoint.Display();
    
    if ( !paused )
    if ( gMoved )
    {
		if (t == 0) {
			dpatch.Tesselate(&viewpoint);
			t = 1;
		}
        gMoved = false;
    }
    
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);

//    dpatch.Update();
    glDisable(GL_BLEND);
    numDrawn = dpatch.Render(&viewpoint, &water_animation);





    //Get in Ortho View
    glMatrixMode(GL_PROJECTION);					//Select the projection matrix
    glPushMatrix();									//Store the projection matrix
    glLoadIdentity();								//Reset the projection matrix
    glOrtho(0,640,0,480,0,1);	//Set up an ortho screen
    glMatrixMode(GL_MODELVIEW);						//Select the modelview matrix

    glColor4f(1,0,0,1);
    
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT, GL_FILL);
 //   output.Printf(0, 80, "Triangles Tesselated : %d", dpatch.NumTri() );
 //   output.Printf(0, 60, "Triangles Drawn      : %d", numDrawn );
    output.Printf(0, 40, "Frames Per Second    : %7.2f", fps );
  //  output.Printf(0, 20, "Total Vertices       : %d", dpatch.NumVertices() );
//	output.Printf(0, 0, "xpos:  %f  zpos:  %f", viewpoint.position.x, viewpoint.position.z);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);
 
	SetDrawModeContext();

    
    		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
                
    glMatrixMode(GL_PROJECTION);					//Select the projection matrix
    glPopMatrix();							//Restore the old projection matrix
    glMatrixMode(GL_MODELVIEW);						//Select the modelview matrix
                        
    timer.Stop();
    elapsedtime = timer.GetElapsed();
    if ( elapsedtime > 1.0f )
    {
        fps = numframes / elapsedtime;
        elapsedtime = 0.0;
        numframes = 0;
        timer.Start();
    }
        
    
	return;
}

void Draw3D(SDL_Surface *S)										// OpenGL drawing code here
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity();											// Reset The Modelview Matrix
	glTranslatef(float(cos(cnt1)), float(sin(cnt2)), -6.0f);	// Translate 6 Units Into The Screen And Use cnt1 And cnt2 To Move The Object
	glRotatef(angle, 0.0f, 1.0f, 0.0f);							// Rotate On The Y-Axis By angle

	for (rot1=0; rot1<2; rot1++)								// 2 Passes
	{
		glRotatef(90.0f,0.0f,1.0f,0.0f);						// Rotate 90 Degrees On The Y-Axis
		glRotatef(180.0f,1.0f,0.0f,0.0f);						// Rotate 180 Degrees On The X-Axis

		for (rot2=0; rot2<2; rot2++)							// 2 Passes
		{
			glRotatef(180.0f,0.0f,1.0f,0.0f);					// Rotate 180 Degrees On The Y-Axis
			glBegin(GL_TRIANGLES);								// Begin Drawing Triangles
				glColor3f (1.f, 0.f, 0.f);						// 1st Color (Red) ...
					glVertex3f( 0.0f, 1.0f, 0.0f);				// ...For The 1st Vertex
				glColor3f (0.f, 1.f, 0.f);						// 2nd Color (Green) ...
					glVertex3f(-1.0f,-1.0f, 1.0f);				// ...For The 2nd Vertex
				glColor3f (0.f, 0.f, 1.f);						// And 3rd Color (Blue) ...
					glVertex3f( 1.0f,-1.0f, 1.0f);				// ...For The 3rd Vertex
			glEnd();											// Done Drawing Triangles
		}
	}

	glFlush();													// Flush The GL Rendering Pipelines

	return;
}

void Draw2D(SDL_Surface *S)										// SDL drawing code here
{
	static SDL_Rect src1={0,0,0,0},								// We're blitting 3 rectangles,
					src2={0,0,0,0},								// so we have to prepare them
					src3={0,0,0,0};

	SDL_FillRect(S, &src1, SDL_MapRGBA(S->format,0,0,0,0));
																// That's an issue many people is having!
																// We set up our Alpha Channel first!
																
	src1.x = (Sint16)((SCREEN_W+NeHe_Logo->w)/2+(cos(Lx)*170)-SCREEN_W/6);
																// src1 is the first Logo
	src1.y = (Iy+=IyOff);

	if(Iy>SCREEN_H-NeHe_Logo->h || Iy<0)						// We calculate the position of the next frame
	{
		IyOff = -IyOff;
	}
	
	src1.w = NeHe_Logo->w;										// Fill the rect structure
	src1.h = NeHe_Logo->h;

	SDL_BlitSurface(NeHe_Logo, NULL, S, &src1);			// And finally blit and update
	SDL_UpdateRects(S, 1, &src1);

	SDL_FillRect(S, &src2, SDL_MapRGBA(S->format,0,0,0,0));
																// The same goes for the logo 2 and 3. Alpha Channel...

	src2.x = (Ix+=IxOff);
	src2.y = (Sint16)((SCREEN_H+SDL_Logo->h)/2+(sin(Ly)*170)-SCREEN_H/6);

	if(Ix>SCREEN_W-SDL_Logo->w || Ix<0)							// Calculations...
	{
		IxOff = - IxOff;
	}

	src2.w = SDL_Logo->w;
	src2.h = SDL_Logo->h;

	SDL_BlitSurface(SDL_Logo, NULL, S, &src2);				// Blit and Update
	SDL_UpdateRects(S, 1, &src2);

	SDL_FillRect(S, &src3, SDL_MapRGBA(S->format,0,0,0,0));
																// Guess what?

	src3.x = (Sint16)((SCREEN_W+SD_Logo->w)/2+(sin(Lx)*170)-SCREEN_W/6);
	src3.y = (Sint16)((SCREEN_H+SD_Logo->h)/2+(cos(Ly)*170)-SCREEN_H/6);

	src3.w = SD_Logo->w;
	src3.h = SD_Logo->h;

	SDL_BlitSurface(SD_Logo, NULL, S, &src3);
	SDL_UpdateRects(S, 1, &src3);

	return;														// We're Always Making Functions Return
}

void SetDrawModeContext()
{
	switch (gDrawMode)
	{
	case DRAW_USE_TEXTURE:
//                glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case DRAW_USE_LIGHTING:
//		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case DRAW_USE_FILL_ONLY:
	//	glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT, GL_FILL);
                glShadeModel(GL_SMOOTH);
		break;

        default:	
	case DRAW_USE_WIREFRAME:
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	}
}


// Texture
GLuint gTextureID=1;
// ---------------------------------------------------------------------
// Initialize the ROAM implementation
//
int GenTexture()
{
// ----------- TEXTURE INITIALIZATION -----------
	glBindTexture(GL_TEXTURE_2D, gTextureID);
	
	unsigned char *pTexture = (unsigned char *)malloc(TEXTURE_SIZE*TEXTURE_SIZE*3);
	unsigned char *pTexWalk = pTexture;

	if ( !pTexture )
		exit(0);

	// Create a random stipple pattern for the texture.  Only use the Green channel.
	// This could easily be modified to load in a bitmap or other texture source.
	for ( int x = 0; x < TEXTURE_SIZE; x++ )
		for ( int y = 0; y < TEXTURE_SIZE; y++ )
		{
			int color = (int)(128.0+(40.0 * rand())/10);
			if ( color > 255 )  color = 255;
			if ( color < 0 )    color = 0;
			*(pTexWalk++) = 0;
			*(pTexWalk++) = color;	// Only use the Green chanel.
			*(pTexWalk++) = 0;
		}

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pTexture);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
       // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	free(pTexture);

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	return 0;
}