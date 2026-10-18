#ifndef __GL_COMPAT_H
#define __GL_COMPAT_H

/* Platform-specific OpenGL headers */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#endif /* __GL_COMPAT_H */
