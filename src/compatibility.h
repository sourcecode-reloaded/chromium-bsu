/*
 * Copyright (c) 2000 Mark B. Allan. All rights reserved.
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */
#ifndef compatibility_h
#define compatibility_h

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifdef _WIN32
	#include <windows.h>
	/* disable silly 'const double to float truncation' warning */
	#pragma warning (disable:4305)
	/* disable 'double to float truncation' warning */
	#pragma warning (disable:4244)
	/* disable 'const int to char truncation'  warning */
	#pragma warning (disable:4309)
	/* disable 'int to bool'  warning */
	#pragma warning (disable:4800)
#endif

#ifndef GL_QUADS
#define GL_QUADS GL_TRIANGLE_STRIP
#endif

#ifndef HAVE_GLUPERSPECTIVE
mat4 frustum(float left, float right, float bottom,
             float top, float near, float far);

mat4 perspective(float fov_y, float width,
                 float height, float near, float far);
#endif

#endif // compatibility_h
