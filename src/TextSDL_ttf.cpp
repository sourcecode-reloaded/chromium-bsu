/*
 * Copyright 2010 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#ifdef TEXT_SDLTTF

#include "gettext.h"

#include "TextSDL_ttf.h"

#include <sys/stat.h>
#include <SDL/SDL_ttf.h>

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || (defined(HAVE_OPENGL_GL_H) && defined(HAVE_OPENGL_GLU_H))
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif

using namespace std;

static int power_of_two(int input)
{
        int value = 1;

        while ( value < input ) {
                value <<= 1;
        }
        return value;
}

//====================================================================
TextSDL_ttf::TextSDL_ttf()
{
	if (-1 == TTF_Init()) 
	{
		fprintf(stderr, _("SDL_ttf: error initialising SDL_ttf: %s\n"), TTF_GetError());
		throw _("SDL_ttf: error initialising SDL_ttf");
	}
	
	fontFile = findFont();
	font = TTF_OpenFont(fontFile, 24);
	if(NULL == font)
	{
		fprintf(stderr, _("SDL_ttf: error loading font: %s\n"), fontFile);
		free((void*)fontFile); fontFile = NULL;
		throw _("SDL_ttf: error loading font");
	}
	free((void*)fontFile); fontFile = NULL;
}

TextSDL_ttf::~TextSDL_ttf()
{
	TTF_CloseFont(font); font = NULL;
	free((void*)fontFile); fontFile = NULL;
	TTF_Quit();
}

void TextSDL_ttf::Render(const char* str, const int len)
{
	SDL_Surface *image;
	GLuint texture;
	SDL_Rect area;
        Uint32 saved_flags;
        Uint8  saved_alpha;
	SDL_Color color = {255,255,255,255};
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Surface *text;
	SDL_Rect dstrect;
	int w, h;
	GLfloat texcoord[4];
	        GLfloat texMinX, texMinY;
        GLfloat texMaxX, texMaxY;

	//char save;
	//if(len >= 0){ save = str[len]; ((char*)str)[len] = '\0'; }
	text = TTF_RenderUTF8_Blended(font, str, color);
	w = (text->w);
	h = (text->h);
	texcoord[0] = 0.0f;                     /* Min X */
	texcoord[3] = 0.0f;                     /* Min Y */
	texcoord[2] = (GLfloat)text->w / w;  /* Max X */
	texcoord[1] = (GLfloat)text->h / h;  /* Max Y */
	image = SDL_CreateRGBSurface(
				SDL_SWSURFACE,
				w, h,
				32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
				0x000000FF, 
				0x0000FF00, 
				0x00FF0000, 
				0xFF000000
#else
				0xFF000000,
				0x00FF0000, 
				0x0000FF00, 
				0x000000FF
#endif
			   );
        saved_flags = text->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
#if SDL_VERSION_ATLEAST(1, 3, 0)
        SDL_GetSurfaceAlphaMod(text, &saved_alpha);
        SDL_SetSurfaceAlphaMod(text, 0xFF);
#else
        saved_alpha = text->format->alpha;
        if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
                SDL_SetAlpha(text, 0, 0);
        }
#endif
        area.x = 0;
        area.y = 0;
        area.w = text->w;
        area.h = text->h;
        SDL_BlitSurface(text, &area, image, &area);
#if SDL_VERSION_ATLEAST(1, 3, 0)
        SDL_SetSurfaceAlphaMod(text, saved_alpha);
#else
        if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
                SDL_SetAlpha(text, saved_flags, saved_alpha);
        }
#endif
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     w, h,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image->pixels);
        SDL_FreeSurface(image); /* No longer needed */
		SDL_FreeSurface(text);
		
		        texMinX = texcoord[0];
        texMinY = texcoord[1];
        texMaxX = texcoord[2];
        texMaxY = texcoord[3];

		                glBindTexture(GL_TEXTURE_2D, texture);
                glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f(texMinX, texMinY); glVertex2i(0,   0  );
                glTexCoord2f(texMaxX, texMinY); glVertex2i(w, 0  );
                glTexCoord2f(texMinX, texMaxY); glVertex2i(0,   h);
                glTexCoord2f(texMaxX, texMaxY); glVertex2i(w, h);
                glEnd();


	//if(len >= 0) ((char*)str)[len] = save;
}

float TextSDL_ttf::Advance(const char* str, const int len)
{
	int width = 0;
	//char save;
	//if(len >= 0){ save = str[len]; ((char*)str)[len] = '\0'; }
	TTF_SizeUTF8(font, str, &width, NULL);
	//if(len >= 0) ((char*)str)[len] = save;
	return (float)width;
}

float TextSDL_ttf::LineHeight(const char* str, const int len)
{
	int height = 0;
	char save;
	//if(len >= 0){ save = str[len]; ((char*)str)[len] = '\0'; }
	TTF_SizeUTF8(font, str, NULL, &height);
	//if(len >= 0) ((char*)str)[len] = save;
	return (float)height;
}

const char* TextSDL_ttf::findFont()
{
	struct stat statbuf;
	const char* font = NULL;
	const char* path = NULL;

	#define CHECK_FONT_PATH(filename) \
	{ \
		path = filename; \
		if( !font && path && 0 == stat(path, &statbuf) ) \
			font = strdup(path); \
	}

	// Get user-specified font path
	CHECK_FONT_PATH(getenv("CHROMIUM_BSU_FONT"))

#ifdef FONT_PATH
	// Get distro-specified font path
	CHECK_FONT_PATH(FONT_PATH)
#endif

#ifdef HAVE_FONTCONFIG
	// Get default font via fontconfig
	if( !font && FcInit() )
	{
		FcResult result;
		FcFontSet *fs;
		FcPattern* pat;
		FcPattern *match;

		/*
		TRANSLATORS: If using the SDL_ttf backend, this should be the font
		name of a font that contains all the Unicode characters in use in
		your translation.
		*/
		pat = FcNameParse((FcChar8 *)_("Gothic Uralic"));
		FcConfigSubstitute(0, pat, FcMatchPattern);

		FcPatternDel(pat, FC_WEIGHT);
		FcPatternAddInteger(pat, FC_WEIGHT, FC_WEIGHT_BOLD);

		FcDefaultSubstitute(pat);
		fs = FcFontSetCreate();
		match = FcFontMatch(0, pat, &result);

		if (match) FcFontSetAdd(fs, match);
		if (pat) FcPatternDestroy(pat);
		if(fs){
			FcChar8* file;
			if( FcPatternGetString (fs->fonts[0], FC_FILE, 0, &file) == FcResultMatch ){
				CHECK_FONT_PATH((const char*)file)
			}
			FcFontSetDestroy(fs);
		}
		FcFini();
	}
#endif

	// Check a couple of common paths for Gothic Uralic/bold as a last resort
	// Debian
	/*
	TRANSLATORS: If using the SDL_ttf backend, this should be the path of a bold
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Debian it should be the Debian path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/truetype/uralic/gothub__.ttf"))
	/*
	TRANSLATORS: If using the SDL_ttf backend, this should be the path of a
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Debian it should be the Debian path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/truetype/uralic/gothu___.ttf"))
	// Mandrake
	/*
	TRANSLATORS: If using the SDL_ttf backend, this should be the path of a bold
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Mandrake it should be the Mandrake path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF"))
	/*
	TRANSLATORS: If using the SDL_ttf backend, this should be the path of a
	font that contains all the Unicode characters in use in	your translation.
	If the font is available in Mandrake it should be the Mandrake path.
	*/
	CHECK_FONT_PATH(_("/usr/share/fonts/TTF/uralic/GOTHU___.TTF"))

	// Check the non-translated versions of the above
	CHECK_FONT_PATH("/usr/share/fonts/truetype/uralic/gothub__.ttf")
	CHECK_FONT_PATH("/usr/share/fonts/truetype/uralic/gothu___.ttf")
	CHECK_FONT_PATH("/usr/share/fonts/TTF/uralic/GOTHUB__.TTF")
	CHECK_FONT_PATH("/usr/share/fonts/TTF/uralic/GOTHU___.TTF")

	return font;
}

#endif // TEXT_SDLTTF
