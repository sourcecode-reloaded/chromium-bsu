/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute 
 * it and/or use it and/or modify it under the terms of the 
 * "Artistic License" 
 */
#ifdef AUDIO_SDLMIXER
 
#include "AudioSDLMixer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _WIN32

#include "extern.h"
#include "define.h"
#include "Global.h"

//====================================================================
AudioSDLMixer::AudioSDLMixer()
	: Audio()
{    
	//UNCLEAN - if initSound fails, Global::audio_enabled will be set to false
	if(Global::audio_enabled == true)
		initSound();
}

AudioSDLMixer::~AudioSDLMixer()
{
	if(Global::audio_enabled) 
	{
    	for (int i = 0; i < NumSoundTypes; i++)
        	Mix_FreeChunk (sounds[i]);

    	Mix_CloseAudio ();
	}
}

/** 
 * open audio device and load sounds
 */
//----------------------------------------------------------
void	AudioSDLMixer::initSound()
{
    if ( Mix_OpenAudio (22050, AUDIO_S16, 2, 512) < 0 )
	{
		fprintf(stderr, "ERROR initializing audio - AudioSDLMixer::initSound()\n");
        Global::audio_enabled = false;
	}
	else
	{
		for (int i = 0; i < NumSoundTypes; i++)
    		sounds[i] = Mix_LoadWAV ( dataLoc ( fileNames[i] ) );

		Mix_ReserveChannels (1); // channel 0 is for music

		atexit (Mix_CloseAudio);
	}
}

/** 
 * play sound
 */
//----------------------------------------------------------
void	AudioSDLMixer::playSound(SoundType type, float pos[3], int)
{
	if (Global::audio_enabled) 
	{
		Mix_PlayChannel (-1, sounds[type], 0);
    }
}

/**
 * pause music channel. If CDROM enabled, call Audio::pauseGameMusic();
 */
//----------------------------------------------------------
void AudioSDLMixer::pauseGameMusic(bool status)
{
	if (Global::audio_enabled) 
	{
		if(cdrom)
		{
			Audio::pauseGameMusic(status);
		}
		else
		{
	    	if (status)
				Mix_PauseMusic();
			else
				Mix_ResumeMusic();
		}
	}
}

/**
 * stop music channel. If CDROM enabled, call Audio::stopMusic();
 */
//----------------------------------------------------------
void	AudioSDLMixer::stopMusic()
{
    if (Global::audio_enabled) 
	{
		Audio::stopMusic();
		Mix_HaltChannel (0);
	}
}

//----------------------------------------------------------
void	AudioSDLMixer::setMusicMode(SoundType mode)
{
    if (Global::audio_enabled)
	{
		Audio::setMusicMode(mode);
		switch(mode)
		{
			default:
			case MusicGame:
				if(cdrom)
					Mix_HaltChannel (0);
				else
					Mix_PlayChannel (0, sounds[mode], -1);
				break;
			case MusicMenu:
				Mix_PlayChannel (0, sounds[mode], -1);
				break;
		}
	}
}

/**
 * set volume for music channel 
 */
//----------------------------------------------------------
void	AudioSDLMixer::setMusicVolume(float value)
{
    if (Global::audio_enabled)
	{
		Mix_Volume (0, (int)(MIX_MAX_VOLUME*value) );
	}
}

/**
 * set volume for music channel 
 */
//----------------------------------------------------------
void	AudioSDLMixer::setSoundVolume(float value)
{
    if (Global::audio_enabled)
	{
		for (int i = 1; i < MIX_CHANNELS; i++)
			Mix_Volume (i, (int)(MIX_MAX_VOLUME*value) );
	}
}

#endif // AUDIO_SDLMIXER

