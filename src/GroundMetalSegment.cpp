/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "GroundMetalSegment.h"

#include <cmath>

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GLES/gl.h>
#endif
//#define GL_EXT_
//#include <GL/glext.h>

#include "Config.h"
#include "Global.h"
#include "Ground.h"

//====================================================================
GroundMetalSegment::GroundMetalSegment(float p[3], float s[2], Ground *prnt)
	: GroundSegment(p, s, prnt)
{
}

GroundMetalSegment::~GroundMetalSegment()
{
}

//----------------------------------------------------------
void GroundMetalSegment::drawGL()
{
	Config *config = Config::instance();
	static float c0_clr[4] = { 0.65, 0.62, 0.53, 1.0 };
	static float c1_clr[4] = { 0.79, 0.82, 0.69, 1.0 };
	static float r0_clr[4]  = { 0.07, 0.07, 0.13, 1.0 };
	static float r1_clr[4]  = { 0.31, 0.30, 0.30, 1.0 };
	static float r2_clr[4]  = { 0.31, 0.30, 0.30, 1.0 };
	float S;
	float tmp;
	float rep;
	float tilt;
	float clr_sin;
	bool blipMirrorT = false;

	age += 1.0;

	clr_sin = 0.5*sin(parent->game->gameFrame*0.001);
	r1_clr[0] = 0.15+clr_sin;
	r2_clr[0] = 0.15+clr_sin;

	clr_sin = 0.2*sin(parent->game->gameFrame*0.0005);
	c0_clr[0] = 0.28+clr_sin;
	c0_clr[1] = 0.25+clr_sin;
	c0_clr[2] = 0.16+clr_sin;
	c1_clr[0] = 0.42+clr_sin;
	c1_clr[1] = 0.45+clr_sin;
	c1_clr[2] = 0.34+clr_sin;

	switch(parent->variation)
	{
		default:
		case 0:
			rep = 0.26;
			tilt = 0.1;
			S = -parent->game->frame*0.001;
			blipMirrorT = false;
			break;
		case 1:
			rep = 0.4;
			tilt = 0.2;
			S = -parent->game->frame*0.001;
			blipMirrorT = true;
			break;
		case 2:
			S = -parent->game->frame*0.005;
			tmp = sin(S);
			rep = 0.7+tmp;
			tilt = 0.5+tmp;
			blipMirrorT = true;
			break;
	}

#ifdef EXPERIMENTAL
	drawMultiTex(rep, S, tilt, blipMirrorT,
					c0_clr,
					c1_clr,
					r0_clr,
					r1_clr,
					r2_clr);
#else // EXPERIMENTAL
	if(config->gfxLevel() > 1)
	{
		drawBlip(rep, S, tilt, blipMirrorT);
	}
	if(true)
	{
		drawSurface(c0_clr,
					c1_clr,
					r0_clr,
					r1_clr,
					r2_clr);
	}
#endif // EXPERIMENTAL

}

//----------------------------------------------------------
void GroundMetalSegment::drawBlip(float rep, float S, float tilt, bool blipMirrorT)
{
	float repA = 0.0;
	float repB = rep;
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Blip]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	#define N 4
	GLfloat texcoords[N][6] = {
		{rep, repA+S+tilt, 0.0, repA+S,      0.0, repB+S     },
		{rep, repA+S+tilt, 0.0, repB+S,      rep, repB+S+tilt},
		{0.0, repA+S,      rep, repA+S+tilt, rep, repB+S+tilt},
		{0.0, repA+S,      rep, repB+S+tilt, 0.0, repB+S     }};
	GLfloat vertices[N][9] = {
		{
			         pos[0],          pos[1], pos[2],
			-size[0]+pos[0],          pos[1], pos[2],
			-size[0]+pos[0], -size[1]+pos[1], pos[2]
		},{
			         pos[0],          pos[1], pos[2],
			-size[0]+pos[0], -size[1]+pos[1], pos[2],
			         pos[0], -size[1]+pos[1], pos[2]
		},{
			size[0]+pos[0],          pos[1], pos[2],
			        pos[0],          pos[1], pos[2],
			        pos[0], -size[1]+pos[1], pos[2]
		},{
			size[0]+pos[0],          pos[1], pos[2],
			        pos[0], -size[1]+pos[1], pos[2],
			size[0]+pos[0], -size[1]+pos[1], pos[2]
		}};
	for(int i = 0; i < N; i++){
		glVertexPointer(3, GL_FLOAT, 0, &(vertices[i]));
		glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords[i]));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	if(blipMirrorT) { repA = rep; repB = 0.0; }

	GLfloat texcoords2[N][6] = {
		{0.0, repA+S,      rep, repA+S+tilt, rep, repB+S+tilt},
		{0.0, repA+S,      rep, repB+S+tilt, 0.0, repB+S     },
		{rep, repA+S+tilt, 0.0, repA+S,      0.0, repB+S     },
		{rep, repA+S+tilt, 0.0, repB+S,      rep, repB+S+tilt}};
	GLfloat vertices2[N][9] = {
		{
			size[0]+pos[0], size[1]+pos[1], pos[2],
			        pos[0], size[1]+pos[1], pos[2],
			        pos[0],         pos[1], pos[2]
		},{
			size[0]+pos[0], size[1]+pos[1], pos[2],
			        pos[0],         pos[1], pos[2],
			size[0]+pos[0],         pos[1], pos[2]
		},{
			         pos[0],  size[1]+pos[1], pos[2],
			-size[0]+pos[0],  size[1]+pos[1], pos[2],
			-size[0]+pos[0],          pos[1], pos[2]
		},{
			         pos[0],  size[1]+pos[1], pos[2],
			-size[0]+pos[0],          pos[1], pos[2],
			         pos[0],          pos[1], pos[2]
		}};
	for(int i = 0; i < N; i++){
		glVertexPointer(3, GL_FLOAT, 0, &(vertices2[i]));
		glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords2[i]));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//----------------------------------------------------------
void GroundMetalSegment::drawSurface(float *c0_clr, float *c1_clr,
									 float *r0_clr, float *r1_clr, float *r2_clr)
{
	Config *config = Config::instance();
	float rep = 1.0;
	glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Base]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	#undef N
	#define N 8

	if(config->gfxLevel() > 0)
	{
		GLfloat texcoords[N][6] = {
			{rep, rep, rep, 0.0, 0.0, 0.0},
			{rep, rep, 0.0, 0.0, 0.0, rep},
			{0.0, rep, 0.0, 0.0, rep, 0.0},
			{0.0, rep, rep, 0.0, rep, rep},
			{0.0, 0.0, 0.0, rep, rep, rep},
			{0.0, 0.0, rep, rep, rep, 0.0},
			{rep, 0.0, rep, rep, 0.0, rep},
			{rep, 0.0, 0.0, rep, 0.0, 0.0}};
		GLfloat vertices[N][9] = {
			{
				         pos[0],    -size[1]+pos[1], pos[2],
				         pos[0],             pos[1], pos[2],
				-size[0]+pos[0],             pos[1], pos[2]
			},{
				         pos[0],    -size[1]+pos[1], pos[2],
				-size[0]+pos[0],             pos[1], pos[2],
				-size[0]+pos[0],    -size[1]+pos[1], pos[2]
			},{
				 size[0]+pos[0],    -size[1]+pos[1], pos[2],
				 size[0]+pos[0],             pos[1], pos[2],
				         pos[0],             pos[1], pos[2]
			},{
				 size[0]+pos[0],    -size[1]+pos[1], pos[2],
				         pos[0],             pos[1], pos[2],
				         pos[0],    -size[1]+pos[1], pos[2]
			},{
				 size[0]+pos[0],             pos[1], pos[2],
				 size[0]+pos[0], size[1]+pos[1]+0.1, pos[2],
				         pos[0], size[1]+pos[1]+0.1, pos[2]
			},{
				 size[0]+pos[0],             pos[1], pos[2],
				         pos[0], size[1]+pos[1]+0.1, pos[2],
				         pos[0],             pos[1], pos[2]
			},{
				         pos[0],             pos[1], pos[2],
				         pos[0], size[1]+pos[1]+0.1, pos[2],
				-size[0]+pos[0], size[1]+pos[1]+0.1, pos[2]
			},{
				         pos[0],             pos[1], pos[2],
				-size[0]+pos[0], size[1]+pos[1]+0.1, pos[2],
				-size[0]+pos[0],             pos[1], pos[2],
				}};
		#define c(a) a[0], a[1], a[2], a[3]
		GLfloat colors[N][12] = {
			{ c(c0_clr), c(c1_clr), c(r2_clr) },
			{ c(c0_clr), c(r2_clr), c(r0_clr) },
			{ c(r1_clr), c(r0_clr), c(c1_clr) },
			{ c(r1_clr), c(c1_clr), c(c0_clr) },
			{ c(r0_clr), c(r1_clr), c(c0_clr) },
			{ c(r0_clr), c(c0_clr), c(c1_clr) },
			{ c(c1_clr), c(c0_clr), c(r0_clr) },
			{ c(c1_clr), c(r0_clr), c(r2_clr) }};
		glEnableClientState(GL_COLOR_ARRAY);
		for(int i = 0; i < N; i++){
			glColorPointer(4, GL_FLOAT, 0, &(colors[i]));
			glVertexPointer(3, GL_FLOAT, 0, &(vertices[i]));
			glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords[i]));
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		glDisableClientState(GL_COLOR_ARRAY);
	}
	else
	{
		float b = -0.1;
		glColor4f(c0_clr[0]-b, c0_clr[1]-b, c0_clr[2]-b, c0_clr[3] );
		GLfloat texcoords[N][6] = {
			{rep, rep, rep, 0.0, 0.0, 0.0},
			{rep, rep, 0.0, 0.0, 0.0, rep},
			{0.0, rep, 0.0, 0.0, rep, 0.0},
			{0.0, rep, rep, 0.0, rep, rep},
			{0.0, 0.0, 0.0, rep, rep, rep},
			{0.0, 0.0, rep, rep, rep, 0.0},
			{rep, 0.0, rep, rep, 0.0, rep},
			{rep, 0.0, 0.0, rep, 0.0, 0.0}};
		GLfloat vertices[N][9] = {
			{
				         pos[0], -size[1]+pos[1], pos[2],
				         pos[0],          pos[1], pos[2],
				-size[0]+pos[0],          pos[1], pos[2]
			},{
				         pos[0], -size[1]+pos[1], pos[2],
				-size[0]+pos[0],          pos[1], pos[2],
				-size[0]+pos[0], -size[1]+pos[1], pos[2]
			},{
				 size[0]+pos[0], -size[1]+pos[1], pos[2],
				 size[0]+pos[0],          pos[1], pos[2],
				         pos[0],          pos[1], pos[2]
			},{
				 size[0]+pos[0], -size[1]+pos[1], pos[2],
				         pos[0],          pos[1], pos[2],
				         pos[0], -size[1]+pos[1], pos[2]
			},{
				 size[0]+pos[0],          pos[1], pos[2],
				 size[0]+pos[0],  size[1]+pos[1], pos[2],
				         pos[0],  size[1]+pos[1], pos[2]
			},{
				 size[0]+pos[0],          pos[1], pos[2],
				         pos[0],  size[1]+pos[1], pos[2],
				         pos[0],          pos[1], pos[2]
			},{
				         pos[0],          pos[1], pos[2],
				         pos[0],  size[1]+pos[1], pos[2],
				-size[0]+pos[0],  size[1]+pos[1], pos[2]
			},{
				         pos[0],          pos[1], pos[2],
				-size[0]+pos[0],  size[1]+pos[1], pos[2],
				-size[0]+pos[0],          pos[1], pos[2],
			}};
		for(int i = 0; i < N; i++){
			glVertexPointer(3, GL_FLOAT, 0, &(vertices[i]));
			glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords[i]));
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

#ifdef EXPERIMENTAL

#define glTexCoord2f_M0(a, b) glMultiTexCoord2fARB(GL_TEXTURE0_ARB, a, b)
#define glTexCoord2f_M1(a, b) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, a, b)

//--------------------------------------------------------------------
void GroundMetalSegment::drawMultiTex(float rep, float S, float tilt, bool blipMirrorT,
						float *c0_clr,
						float *c1_clr,
						float *r0_clr,
						float *r1_clr,
						float *r2_clr)
{
	float repA = 0.0;
	float repB = rep;

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Blip]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, parent->tex[Ground::Base]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_TRIANGLES); //-- use triangles to prevent color popping on Utah

		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r2_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f(-size[0]+pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f(-size[0]+pos[0], -size[1]+pos[1], pos[2]);

		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f(-size[0]+pos[0], -size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0], -size[1]+pos[1], pos[2]);

		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f( size[0]+pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0], -size[1]+pos[1], pos[2]);

		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f( size[0]+pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0], -size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r1_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f( size[0]+pos[0], -size[1]+pos[1], pos[2]);

		if(blipMirrorT) { repA = rep; repB = 0.0; }

		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r1_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f( size[0]+pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);

		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r1_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f( size[0]+pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f( size[0]+pos[0],		   pos[1], pos[2]);
		//--
		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repA+S);      glColor4fv(r0_clr); glTexCoord2f_M1( 0.0,  1.0); glVertex3f(-size[0]+pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r2_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f(-size[0]+pos[0],		   pos[1], pos[2]);

		glTexCoord2f_M0( rep,  repA+S+tilt); glColor4fv(c0_clr); glTexCoord2f_M1( 1.0,  1.0); glVertex3f(		  pos[0],  size[1]+pos[1], pos[2]);
		glTexCoord2f_M0( 0.0,  repB+S);      glColor4fv(r2_clr); glTexCoord2f_M1( 0.0,  0.0); glVertex3f(-size[0]+pos[0],		   pos[1], pos[2]);
		glTexCoord2f_M0( rep,  repB+S+tilt); glColor4fv(c1_clr); glTexCoord2f_M1( 1.0,  0.0); glVertex3f(		  pos[0],		   pos[1], pos[2]);

	glEnd();

	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

#endif // EXPERIMENTAL
