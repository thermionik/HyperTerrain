
/***********************************************
*                                              *
*    Jeff Molofee's Revised OpenGL Basecode    *
*  Huge Thanks To Maxwell Sayles & Peter Puck  *
*   SDL Port By Fabio 'SnowDruid' Franchello   *
*            http://nehe.gamedev.net           *
*                     2001                     *
*                                              *
***********************************************/

/* NOTES:	This is a portable version of the great NeHeGL Framework  made using
			the awesome SDL Library by Sam Lantinga (http://www.libsdl.org).

			The ASK_FULLSCREEN flag only work with MSVC presently.
			The F1 key to toggle fullscreen only work with Linux and BeOS, since
			SDL only support the function under those two OSes.

			Fabio 'SnowDruid' Franchello (snowdruid@tiscalinet.it)
*/

// Includes
#ifdef WIN32																// If We're Under MSVC
#include <windows.h>														// We Need The Windows Header
#else																		// Otherwhise
#include <stdio.h>															// We're Including The Standard IO Header
#include <stdlib.h>															// And The Standard Lib Header
#include <string.h>															// And The String Lib Header
#endif																		// Then...

#include <GL/gl.h>															// We're Including The OpenGL Header
#include <GL/glu.h>															// And The GLu Header
#include "SDL/SDL.h"															// And Of Course The SDL Header

#include "main.h"															// Header File For The NeHeGL Basecode

#ifdef WIN32																// If We're Under MSVC
#pragma comment(lib, "OpenGL32.lib")										// We're Telling The Linker To Look For The OpenGL32.lib
#pragma comment(lib, "GLu32.lib")											// The GLu32.lib Library...
#pragma comment(lib, "SDLmain.lib")											// The SDLmain.lib And
#pragma comment(lib, "SDL.lib")												// The SDL.lib Libraries
#endif																		// Then...

// Globals
bool isProgramLooping;														// We're Using This One To Know If The Program Must Go On In The Main Loop
S_AppStatus AppStatus;														// The Struct That Holds The Runtime Data Of The Application
SDL_Surface *Screen;


// Constants -----------------------------------------------------------------


// Function Prototypes -------------------------------------------------------



// Code
bool InitTimers(Uint32 *C)													// This Is Used To Init All The Timers In Our Application
{
	*C = SDL_GetTicks();													// Hold The Value Of SDL_GetTicks At The Program Init

	return true;															// Return TRUE (Initialization Successful)
}

void TerminateApplication(void)												// Terminate The Application
{
	static SDL_Event Q;														// We're Sending A SDL_QUIT Event

	Q.type = SDL_QUIT;														// To The SDL Event Queue

	if(SDL_PushEvent(&Q) == -1)												// Try Send The Event
	{
		Log("SDL_QUIT event can't be pushed: %s\n", SDL_GetError() );		// And Eventually Report Errors
		exit(1);															// And Exit
	}

	return;																	// We're Always Making Our Funtions Return
}

void ToggleFullscreen(void)													// Toggle Fullscreen/Windowed (Works On Linux/BeOS Only)
{
	SDL_Surface *S;															// A Surface To Point The Screen

	S = SDL_GetVideoSurface();												// Get The Video Surface

	if(!S || (SDL_WM_ToggleFullScreen(S)!=1))								// If SDL_GetVideoSurface Failed, Or We Can't Toggle To Fullscreen
	{
		Log("Unable to toggle fullscreen: %s\n", SDL_GetError() );			// We're Reporting The Error, But We're Not Exiting
	}
	
	return;																	// Always Return
}

void ReshapeGL(int width, int height)										// Reshape The Window When It's Moved Or Resized
{
	glViewport(0,0,(GLsizei)(width),(GLsizei)(height));						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix */

	gluPerspective(45.0f,(GLfloat)(width)/(GLfloat)(height),1.0f,100.0f);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glLoadIdentity();														// Reset The Modelview Matrix

	return;																	// Always Return, We're Standard :)
}

bool CreateWindowGL(int W, int H, int B, Uint32 F)							// This Code Creates Our OpenGL Window
{
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );								// In order to use SDL_OPENGLBLIT we have to
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );							// set GL attributes first
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );							// colors and doublebuffering

	if(!(Screen = SDL_SetVideoMode(W, H, B, F)))							// We're Using SDL_SetVideoMode To Create The Window
	{
		return false;														// If It Fails, We're Returning False
	}

	SDL_FillRect(Screen, NULL, SDL_MapRGBA(Screen->format,0,0,0,0));		// A key event! God knows all the time I've spent just to
																			// figure out this basic thing. We have to open the Screen Alpha Channel!

	ReshapeGL(SCREEN_W, SCREEN_H);											// We're Calling Reshape As The Window Is Created

	return true;															// Return TRUE (Initialization Successful)
}

int main(int argc, char **argv)												// Our Main Funcion!
{
	SDL_Event	E;															// And Event Used In The Polling Process
	Uint8		*Keys;														// A Pointer To An Array That Will Contain The Keyboard Snapshot
	Uint16		MouseX;
	Uint16		MouseY;
	Uint32		Vflags;														// Our Video Flags
	Uint32		TickCount;													// Used For The Tick Counter
	Uint32		LastCount;													// Used For The Tick Counter
	
	Screen = NULL;															// We're Standard, We're Initializing Every Variable We Have
	Keys = NULL;															// We Compilers Won't Complain
	MouseX = NULL;
	MouseY = NULL;
	Vflags = SDL_HWSURFACE|SDL_OPENGLBLIT;									// We Want A Hardware Surface And Special OpenGLBlit Mode
																			// So We Can Even Blit 2D Graphics In our OpenGL Scene

	InitErrorLog();															// Init The Error Log

	if(SDL_Init(SDL_INIT_VIDEO)<0)											// Init The SDL Library, The VIDEO Subsystem
	{
		Log("Unable to open SDL: %s\n", SDL_GetError() );					// If SDL Can't Be Initialized
		exit(1);															// Get Out Of Here. Sorry.
	}

	atexit(SDL_Quit);														// SDL's Been init, Now We're Making Sure Thet SDL_Quit Will Be Called In Case of exit()

#if defined FULLSCREEN_ASK													// We're Choosing Compile Time If We Want The Application To Ask For Fullscreen (WIN32 Only)

	if(MessageBox(HWND_DESKTOP, "Want to use fullscreen mode?",				// With a MessageBox Call
		"Fullscreen?", MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		Vflags|=SDL_FULLSCREEN;												// If Yes, Add The Fullscreen Flag To Our Init
	}

#elif defined FULLSCREEN													// Now, We Can Decide To Always Launch Out Application Fullscreen

	Vflags|=SDL_FULLSCREEN;													// If So, We Always Need The Fullscreen Video Init Flag

#endif																		// If Neither FULLSCREEN_ASK nor FULLSCREEN Were Specified At Compile Time, We're
																			// Launching Our Application in Windowed Mode

	SetUpIcon("NeHe_Icon.bmp");												// We're setting up a fancy icon

	if(!CreateWindowGL(SCREEN_W, SCREEN_H, SCREEN_BPP, Vflags))				// Our Video Flags Are Set, We're Creating The Window
	{
		Log("Unable to open screen surface: %s\n", SDL_GetError() );		// If Something's Gone Wrong, Report
		exit(1);															// And Exit
	}

	SDL_WM_SetCaption(APP_NAME, NULL);										// We're Setting The Window Caption

	if(!InitTimers(&LastCount))												// We Call The Timers Init Function
	{
		Log("Can't init the timers: %s\n", SDL_GetError() );				// If It Can't Init, Report
		exit(1);															// And Exit
	}

	if(!InitGL(Screen))														// We're Calling The OpenGL Init Function
	{
		Log("Can't init GL: %s\n", SDL_GetError() );						// If Something's Gone Wrong, Report
		exit(1);															// And Guess What? Exit
	}

	if(!Initialize())														// Now We're Initting The Application
	{
		Log("App init failed: %s\n", SDL_GetError() );						// Blah Blah Blah, Blah
		exit(1);															// And Blah
	}

	isProgramLooping = true;												// Ok, Make Our Program Loop

	while(isProgramLooping)													// And While It's looping
	{

		if(SDL_PollEvent(&E))												// We're Fetching The First Event Of The Queue
		{
			switch(E.type)													// And Processing It
			{
				
			case SDL_QUIT:													// It's a QUIT Event?
				{
					isProgramLooping = false;								// If Yes, Make The Program Stop Looping
					break;													// And Break
				}

			case SDL_VIDEORESIZE:											// It's a RESIZE Event?
				{
					ReshapeGL(E.resize.w, E.resize.h);						// If Yes, Recalculate The OpenGL Scene Data For The New Window
					break;													// And Break
				}

			case SDL_ACTIVEEVENT:											// It's an ACTIVE Event?
				{
					if(E.active.state & SDL_APPACTIVE)						// Activity Level Changed? (IE: Iconified?)
					{
						if(E.active.gain)									// Activity's Been Gained?
						{
							AppStatus.Visible = true;						// If Yes, Set AppStatus.Visible
						}
						else												// Otherwhise
						{
							AppStatus.Visible = false;						// Reset AppStatus.Visible
						}
					}
					
					if(E.active.state & SDL_APPMOUSEFOCUS)					// The Mouse Cursor Has Left/Entered The Window Space?
					{
						if(E.active.gain)									// Entered?
						{
							AppStatus.MouseFocus = true;						// Report It Setting AppStatus.MouseFocus
						}
						else												// Otherwhise
						{
							AppStatus.MouseFocus = false;					// The Cursor Has Left, Reset AppStatus.MouseFocus
						}
					}

					if(E.active.state & SDL_APPINPUTFOCUS)					// The Window Has Gained/Lost Input Focus?
					{
						if(E.active.gain)									// Gained?
						{
							AppStatus.KeyboardFocus = true;					// Report It Where You Know (You Always Report, You're A Spy, Aren't You?!)
						}
						else												// Otherwhise
						{
							AppStatus.KeyboardFocus = false;				// Reset AppStatus.KeyboardFocus
						}
					}
					
					break;													// And Break
				}

			case SDL_MOUSEBUTTONDOWN: 
			case SDL_KEYDOWN:												// Someone Has Pressed A Key?
				{
					Keys = SDL_GetKeyState(NULL);							// Is It's So, Take A SnapShot Of The Keyboard For The Update() Func To Use
					break;													// And Break;
				}
			case SDL_MOUSEMOTION:
				{
					MouseX = E.motion.x;
					MouseY = E.motion.y;
					break;
				}
			}
		}
		else																// No Events To Poll? (SDL_PollEvent()==0?)
		{
			if(!AppStatus.Visible)											// If The Application Is Not Visible
			{
				SDL_WaitEvent(NULL);										// Leave The CPU Alone, Don't Waste Time, Simply Wait For An Event
			}
			else															// Otherwhise
			{
				TickCount = SDL_GetTicks();									// Get Present Ticks
				SDL_WarpMouse(5,5);	
				Update(TickCount-LastCount, Keys, MouseX, MouseY);							// And Update The Motions And Data
				LastCount = TickCount;										// Save The Present Tick Probing
				Draw(Screen);												// Do The Drawings!
				SDL_GL_SwapBuffers();										// And Swap The Buffers (We're Double-Buffering, Remember?)
			}
		}
	}

	Deinitialize();															// The Program Stopped Looping, We Have To Close And Go Home
																			// First, The Application Data Deinitialization
	CloseErrorLog();														// Then, The Error Log
	exit(0);																// And Finally We're Out, exit() Will Call SDL_Quit

	return 0;																// We're Standard: The main() Must Return A Value
}

SDL_Surface *SetUpIcon(char *File)											// Icon SDL setup routine
{
	SDL_Surface *Icon;														// The icon itself
	Uint8       *Pixels, *Mask;												// We need a mask and a pointer for the raw image
	int         i, mlen;													// Iterator and a store variable

	if( (Icon = SDL_LoadBMP(File)) == NULL)									// We load the bmp file
	{
		Log("Unable to load icon: %s\n", SDL_GetError() );
		return NULL;
	}

	if( (Icon->w%8) != 0)													// Check the bmp size
	{
		Log("Icon width must be a multiple of 8\n" );
		SDL_FreeSurface(Icon);
		return NULL;
	}

	if( !Icon->format->palette )											// Check if the file is palettized
	{
		Log("Icon file must have a palette.\n" );
		SDL_FreeSurface(Icon);
		return NULL;
	}

	SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, *((Uint8 *)Icon->pixels));		// Setup the key color

	Pixels =(Uint8 *)Icon->pixels;											// Point the raw data
	mlen   = Icon->w*Icon->h;												// And calculate the total size

	if( !(Mask = (Uint8 *)malloc(mlen/8)) )									// Ask the OS for memory to store the mask
	{
		Log("malloc() failed while loading icon: %s\n", SDL_GetError() );
		SDL_FreeSurface(Icon);
		return NULL;
	}

	memset(Mask, 0, mlen/8);												// Zero the memory

	for(i=0;i<mlen;)														// Do the icon masking
	{
		if(Pixels[i] != *Pixels)											// More info on this
		{
			Mask[i/8] |= 0x01;												// on the SDL docs
		}

		++i;

		if((i%8)!=0)
		{
			Mask[i/8] <<= 1;
		}
	}

	SDL_WM_SetIcon(Icon, Mask);												// Having our data prepared, finally setup the icon

	return Icon;															// And return
}

