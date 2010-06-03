/*
 * Copyright 2010 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef TEXT_SDLTTF

#include "gettext.h"

#include "TextSDL_ttf.h"

#include <sys/stat.h>
#include <SDL/SDL_ttf.h>

#ifdef HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
#endif

using namespace std;

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
	SDL_Color color = {255,255,255,255};
	SDL_Surface *text;
	//char save;
	//if(len >= 0){ save = str[len]; ((char*)str)[len] = '\0'; }
	text = TTF_RenderUTF8_Blended(font, str, color);
	SDL_BlitSurface(text, NULL, screen, &dstrect);
	SDL_FreeSurface(text);
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
