/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#include "MainSDL.h"

#ifdef USE_SDL

#include <stdlib.h>
#include <math.h>
#include <SDL.h>

#include "compatibility.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "extern.h"
#include "Global.h"
#include "HeroAircraft.h"
#include "Audio.h"
#include "MainGL.h"


//====================================================================
MainSDL::MainSDL(int, char **)
{
	mouseToggle = Global::mouseActive;
	fire = 0;
	xjoy = yjoy = xjNow = yjNow = 0;
	adjCount = 0;
	
	Uint32 initOpts;
	
	initOpts = SDL_INIT_VIDEO;
	
#ifdef WITH_JOYSTICK
	initOpts = initOpts|SDL_INIT_JOYSTICK;
#endif
#ifdef NO_PARACHUTE
	initOpts = initOpts|SDL_INIT_NOPARACHUTE;
#endif
	if(Global::use_cdrom)
		initOpts = initOpts|SDL_INIT_CDROM;
		
	if( SDL_Init( initOpts ) < 0 ) 
	{
		fprintf(stderr,"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit( 1 );
	}
	fprintf(stderr, "SDL initialized.\n");

#ifdef WITH_JOYSTICK
	int nj = SDL_NumJoysticks();
	if(nj > 0)
	{
		fprintf(stderr, "num joysticks = %d\n", nj);
		joystick = SDL_JoystickOpen(0);
		fprintf(stderr, "   joystick 0 = %p\n", joystick);
		if(joystick)
			SDL_JoystickEventState(SDL_ENABLE);
	}
	else
	{
		//fprintf(stderr, "no joysticks found\n");
		joystick = 0;
	}
#else
	joystick = 0;
#endif

	setVideoMode();

	fprintf(stderr, "-OpenGL---------------------------------------\n");
	fprintf(stderr, "Vendor     : %s\n", glGetString( GL_VENDOR ) );
	fprintf(stderr, "Renderer   : %s\n", glGetString( GL_RENDERER ) );
	fprintf(stderr, "Version    : %s\n", glGetString( GL_VERSION ) );
//	fprintf(stderr, "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
	fprintf(stderr, "----------------------------------------------\n");

	//-- Set the window manager title bar
	SDL_WM_SetCaption( "chromium B.S.U.", "chromium B.S.U." );

	//-- Create game
	Global::createGame();
}

MainSDL::~MainSDL()
{
}

//----------------------------------------------------------
bool MainSDL::run()
{
	float targetAdj		= 1.0;
	Uint32 start_time	= 0; 
	Uint32 now_time		= 0; 
	Uint32 last_time	= 0;
	int done = 0;
	int frames;

	//-- enter main loop...
	start_time = SDL_GetTicks();
	frames = 0;
	while( !done ) 
	{
		SDL_Event event;
		
		//-- Draw our scene...
		Global::mainGL->drawGL();
		
		SDL_GL_SwapBuffers( );
		
		#ifdef CHECK_ERRORS
		checkErrors();
		#endif// CHECK_ERRORS

		//-- Delay
		SDL_Delay( 32-(int)(24.0*Global::gameSpeed) );
//		SDL_Delay( 8 );
//		SDL_Delay( 16 );
//		SDL_Delay( 32 );
//		SDL_Delay( 66 );

//		//-- cheezy, partially functional record mechanism
//		bool write = false;
//		SDL_Event *fileEvent;
//		if( !write && Global::gameMode == Global::Game)
//		{
//			while( (fileEvent = getEvent(Global::eventFile)) ) 
//				done = this->process(fileEvent);
//		}
		
		/* Check if there's a pending event. */
		while( SDL_PollEvent( &event ) ) 
		{
//			if(write)
//				saveEvent(&event);
			done = this->process(&event);
		}
		this->joystickMove();
		++frames;
		
		Global::frame++;
		if( !(Global::gameFrame%10) )
		{
			now_time = SDL_GetTicks();
			if(last_time)
			{
				Global::fps = (10.0/(now_time-last_time))*1000.0;
			}
			last_time = now_time;
			
			if(Global::gameMode != Global::Menu)
			{
				if(Global::gameFrame < 400)
				{
					if(Global::fps < 48.0 && Global::gameSpeed < 1.0)
					{
						Global::gameSpeed += 0.02;
						fprintf(stdout, "init----> %3.2ffps gameSpeed = %g\n", Global::fps, Global::gameSpeed);
					}
					else if(Global::gameFrame > 20)
					{
						float tmp = 50.0/Global::fps;
						tmp = 0.8*targetAdj + 0.2*tmp;
						targetAdj = floor(100.0*(tmp+0.005))/100.0;
						fprintf(stdout, "init----> %3.2ffps targetAdj = %g, tmp = %g\n", Global::fps, targetAdj, tmp);
					}
				}
				else if( Global::auto_speed && (Global::fps > 30.0 && Global::fps < 100.0))  // discount any wacky fps from pausing
				{
					//Global::speedAdj = targetAdj;
					// Everything was originally based on 50fps - attempt to adjust
					// if we're outside a reasonable range
					float tmp = 50.0/Global::fps;
					if( fabs(targetAdj-tmp) > 0.1)
					{
						adjCount++;
						Global::speedAdj = tmp;
						fprintf(stdout, "adjust--> %3.2f targetAdj = %g -- Global::speedAdj = %g\n", Global::fps, targetAdj, Global::speedAdj);
					}
					else
						Global::speedAdj = targetAdj;
				}
				else
					Global::speedAdj = targetAdj;
					
//				if( !(frames%500) )
//					fprintf(stdout, "fps = %g speedAdj = %g\n", Global::fps, Global::speedAdj);
			}
			
		}
	}
	fflush(stdout);
	
	//-- Delete game objects
	Global::deleteGame();
	
	if(adjCount > 20)
	{
		fprintf(stderr, "%d speed adjustments required.\n", adjCount);
		fprintf(stderr, "NOTE: The game was not able to maintain a steady 50 frames per\n");
		fprintf(stderr, "      second. You should consider reducing your screen resolution\n");
		fprintf(stderr, "      or lowering the graphics detail setting.\n");
		fprintf(stderr, "      -OR-\n");
		fprintf(stderr, "      make sure that you aren't running any system monitoring\n");
		fprintf(stderr, "      tools (like \'top\', \'xosview\', etc.) These kinds of tools\n");
		fprintf(stderr, "      can make it difficult to maintain a steady frame rate.\n");
	}
	
	//-- Destroy our GL context, etc.
	fprintf(stderr, "exit.\n");
	SDL_Quit();
	
	return false;
}

//----------------------------------------------------------
bool MainSDL::checkErrors()
{
	bool retVal = false;
	GLenum	gl_error;
	char*	sdl_error;
	
	//-- Check for GL errors
	gl_error = glGetError( );
	if( gl_error != GL_NO_ERROR ) 
	{
		fprintf(stderr, "ERROR!!! OpenGL error: %s\n", gluErrorString(gl_error) );
		retVal = true;
	}

	//-- Check for SDL errors
	sdl_error = SDL_GetError( );
	if( sdl_error[0] != '\0' ) 
	{
		fprintf(stderr, "ERROR!!! SDL error '%s'\n", sdl_error);
		SDL_ClearError();
		retVal = true;
	}
	
	return retVal;
}

//----------------------------------------------------------
void MainSDL::setVideoMode() 
{
	int w;
	int h;
	Uint32 video_flags;
	SDL_Surface *glSurface = 0;
	
	//-- Set the flags we want to use for setting the video mode
	video_flags = SDL_OPENGL;
	if(Global::full_screen)
		video_flags |= SDL_FULLSCREEN;
	
	switch(Global::screenSize)
	{
		case 0:
			w = Global::screenW = 512;
			h = Global::screenH = 384;
			break;
		case 1:
			w = Global::screenW = 640;
			h = Global::screenH = 480;
			break;
		case 2:
			w = Global::screenW = 800;
			h = Global::screenH = 600;
			break;
		case 3:
			w = Global::screenW = 1024;
			h = Global::screenH = 768;
			break;
		case 4:
			w = Global::screenW = 1280;
			h = Global::screenH = 960;
			break;
		default:
			w = Global::screenW = 640;
			h = Global::screenH = 480;
			break;
	}
	
	int rs, gs, bs, ds;
	int bpp;
	if(Global::true_color)
	{
		//-- 24 bit color
		bpp = 24;
		rs = gs = bs = 8;
		ds = 16;
	}
	else
	{
		//-- 16 bit color
		bpp = 16;
		rs = gs = bs = 5;
		ds = 16;
	}
	
	//-- Initialize display
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,	rs );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,	gs );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,	bs );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,	ds );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	if ( (glSurface = SDL_SetVideoMode( w, h, bpp, video_flags )) == NULL ) 
	{
		fprintf(stderr, "Couldn't set GL mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	else
	{
		fprintf(stderr, "video mode set ");
	}
	
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, 	&rs);
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE,	&gs);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE,	&bs);
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE,	&ds);
	fprintf(stderr, "(bpp=%d RGB=%d%d%d depth=%d)\n", glSurface->format->BitsPerPixel, rs, gs, bs, ds);

	if(Global::mainGL)
		Global::mainGL->initGL();
}

#endif // USE_SDL
