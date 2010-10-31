/*
 * Copyright 2010 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Artistic License"
 */
#ifndef TextSDL_ttf_h
#define TextSDL_ttf_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#ifdef TEXT_SDLTTF

#include <SDL/SDL_ttf.h>

#include "Text.h"

/**
 * Use SDL_ttf for rendering text in OpenGL
 */
//====================================================================
class TextSDL_ttf : public Text
{
public:

	TextSDL_ttf();
	virtual ~TextSDL_ttf();

	virtual void Render(const char*, const int = -1);
	virtual float Advance(const char*, const int = -1);

	virtual float LineHeight(const char*, const int = -1);

private:
	TTF_Font *font;
	const char* fontFile;

	const char* findFont();

};

#endif // TEXT_SDLTTF

#endif // TextSDL_ttf_h
