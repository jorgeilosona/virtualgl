/* Copyright (C)2004 Landmark Graphics
 *
 * This library is free software and may be redistributed and/or modified under
 * the terms of the wxWindows Library License, Version 3 or (at your option)
 * any later version.  The full license is in the LICENSE.txt file included
 * with this distribution.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * wxWindows Library License for more details.
 */

#include "faker-sym.h"
#include "faker-macros.h"

// This file contains stubs for GLX functions which are faked simply by remapping
// their display handle and/or X Visual handle to the server's display


// This attempts to look up a visual in the hash or match it using 2D functions
// if (for some reason) it wasn't obtained with glXChooseVisual()

#define _DefaultScreen(d) ((!fconfig.glp && d) ? DefaultScreen(d):0)

GLXFBConfig _MatchConfig(Display *dpy, XVisualInfo *vis)
{
	GLXFBConfig c=0, *configs=NULL;  int n=0;
	if(!dpy || !vis) return 0;
	if(!(c=vish.getpbconfig(dpy, vis)))
	{
		int defattribs[]={GLX_DOUBLEBUFFER, 1, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8, GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE,
			GLX_PBUFFER_BIT, None};
		int rgbattribs[]={GLX_DOUBLEBUFFER, 1, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8, GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE,
			GLX_PBUFFER_BIT, None};
		int ciattribs[]={GLX_DOUBLEBUFFER, 1, GLX_BUFFER_SIZE, 8,
			GLX_RENDER_TYPE, GLX_COLOR_INDEX_BIT, GLX_DRAWABLE_TYPE,
			GLX_PBUFFER_BIT, None};
		int *attribs=rgbattribs;
		if(vis->c_class!=TrueColor && vis->c_class!=DirectColor)
		{
			attribs=ciattribs;
			attribs[3]=vis->depth;
		}
		else
		{
			if(vis->depth<8) attribs[3]=attribs[5]=attribs[7]=1;
			else if(vis->depth<16) attribs[3]=attribs[5]=attribs[7]=2;
			else if(vis->depth<24) attribs[3]=attribs[5]=attribs[7]=4;
		}
		if(((configs=glXChooseFBConfig(_localdpy, _DefaultScreen(_localdpy), attribs, &n))==NULL || n<1)
			&& ((configs=glXChooseFBConfig(_localdpy, _DefaultScreen(_localdpy), defattribs, &n))==NULL || n<1))
			return 0;
		c=configs[0];
		if(c) vish.add(dpy, vis, c);
	}
	return c;
}

extern "C" {

GLXFBConfig *glXChooseFBConfig(Display *dpy, int screen, const int *attrib_list, int *nelements)
{
	#ifdef USEGLP
	if(fconfig.glp)
	{
		// Argh!
		int glpattribs[257], j=0;
		for(int i=0; attrib_list[i]!=None && i<=254; i+=2)
		{
			if(attrib_list[i]!=GLX_DRAWABLE_TYPE)
			{
				glpattribs[j++]=attrib_list[i];  glpattribs[j++]=attrib_list[i+1];
			}
		}
		glpattribs[j]=None;
		return glPChooseFBConfig(_localdev, glpattribs, nelements);
	}
	else
	#endif
	return _glXChooseFBConfig(_localdpy, screen, attrib_list, nelements);
}

GLXFBConfigSGIX *glXChooseFBConfigSGIX (Display *dpy, int screen, const int *attrib_list, int *nelements)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPChooseFBConfig(_localdev, attrib_list, nelements);
	else
	#endif
	return _glXChooseFBConfigSGIX(_localdpy, screen, attrib_list, nelements);
}

#ifdef sun
void glXCopyContext(Display *dpy, GLXContext src, GLXContext dst, unsigned int mask)
#else
void glXCopyContext(Display *dpy, GLXContext src, GLXContext dst, unsigned long mask)
#endif
{
	#ifdef USEGLP
	if(fconfig.glp) glPCopyContext(src, dst, mask);
	else
	#endif
	_glXCopyContext(_localdpy, src, dst, mask);
}

GLXPbuffer glXCreatePbuffer(Display *dpy, GLXFBConfig config, const int *attrib_list)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPCreateBuffer(config, attrib_list);
	else
	#endif
	return _glXCreatePbuffer(_localdpy, config, attrib_list);
}

GLXPbuffer glXCreateGLXPbufferSGIX(Display *dpy, GLXFBConfigSGIX config, unsigned int width, unsigned int height, int *attrib_list)
{
	#ifdef USEGLP
	if(fconfig.glp)
	{
		int glpattribs[257], j=0;
		for(int i=0; attrib_list[i]!=None && i<=254; i+=2)
		{
			glpattribs[j++]=attrib_list[i];  glpattribs[j++]=attrib_list[i+1];
		}
		glpattribs[j++]=GLP_PBUFFER_WIDTH;  glpattribs[j++]=width;
		glpattribs[j++]=GLP_PBUFFER_HEIGHT;  glpattribs[j++]=height;
		glpattribs[j]=None;
		return glPCreateBuffer(config, glpattribs);
	}
	#endif
	return _glXCreateGLXPbufferSGIX(_localdpy, config, width, height, attrib_list);
}

void glXDestroyGLXPbufferSGIX(Display *dpy, GLXPbuffer pbuf)
{
	#ifdef USEGLP
	if(fconfig.glp) glPDestroyBuffer(pbuf);
	else
	#endif
	_glXDestroyGLXPbufferSGIX(_localdpy, pbuf);
}

void glXDestroyPbuffer(Display *dpy, GLXPbuffer pbuf)
{
	#ifdef USEGLP
	if(fconfig.glp) glPDestroyBuffer(pbuf);
	else
	#endif
	_glXDestroyPbuffer(_localdpy, pbuf);
}

void glXFreeContextEXT(Display *dpy, GLXContext ctx)
{
	#ifdef USEGLP
	// Tell me about the rabbits, George ...
	if(fconfig.glp) return;
	else
	#endif
	_glXFreeContextEXT(_localdpy, ctx);
}

const char *glXGetClientString(Display *dpy, int name)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPGetLibraryString(name);
	else
	#endif
	return _glXGetClientString(_localdpy, name);
}

int glXGetConfig(Display *dpy, XVisualInfo *vis, int attrib, int *value)
{
	TRY();
	GLXFBConfig c;  int glxvalue, err;
	errifnot(c=_MatchConfig(dpy, vis));
	switch(attrib)
	{
		case GLX_USE_GL:
			if(value) {*value=1;  return 0;}
			else return GLX_BAD_VALUE;
		case GLX_RGBA:
			if((err=glXGetFBConfigAttrib(_localdpy, c, GLX_RENDER_TYPE, &glxvalue))!=0)
				return err;
			*value=glxvalue==GLX_RGBA_BIT? 1:0;  return 0;
		default:
			return glXGetFBConfigAttrib(_localdpy, c, attrib, value);
	}
	CATCH();
	return GLX_BAD_ATTRIBUTE;
}

#ifdef USEGLP
GLXContext glXGetCurrentContext(void)
{
	if(fconfig.glp) return glPGetCurrentContext();
	else
	return _glXGetCurrentContext();
}

Display *glXGetCurrentDisplay(void)
{
	if(fconfig.glp)
	{
		Display *dpy=NULL;  pbwin *pb=NULL;
		if((pb=winh.findpb(glXGetCurrentDrawable()))!=NULL)
			dpy=pb->getwindpy();
		return dpy;
	}
	else
	return _glXGetCurrentDisplay();
}

GLXDrawable glXGetCurrentDrawable(void)
{
	if(fconfig.glp) return glPGetCurrentBuffer();
	else
	return _glXGetCurrentDrawable();
}

GLXDrawable glXGetCurrentReadDrawable(void)
{
	if(fconfig.glp) return glPGetCurrentReadBuffer();
	else
	return _glXGetCurrentReadDrawable();
}
#endif

int glXGetFBConfigAttrib(Display *dpy, GLXFBConfig config, int attribute, int *value)
{
	#ifdef USEGLP
	if(fconfig.glp)
	{
		if(attribute==GLX_X_VISUAL_TYPE) {*value=GLX_TRUE_COLOR;  return 0;}
		else return glPGetFBConfigAttrib(config, attribute, value);
	}
	else
	#endif
	return _glXGetFBConfigAttrib(_localdpy, config, attribute, value);
}

int glXGetFBConfigAttribSGIX(Display *dpy, GLXFBConfigSGIX config, int attribute, int *value_return)
{
	return glXGetFBConfigAttrib(dpy, config, attribute, value_return);
}

GLXFBConfigSGIX glXGetFBConfigFromVisualSGIX(Display *dpy, XVisualInfo *vis)
{
	return _MatchConfig(dpy, vis);
}

GLXFBConfig *glXGetFBConfigs(Display *dpy, int screen, int *nelements)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPGetFBConfigs(_localdev, nelements);
	else
	#endif
	return _glXGetFBConfigs(_localdpy, screen, nelements);
}

void glXGetSelectedEvent(Display *dpy, GLXDrawable draw, unsigned long *event_mask)
{
	#ifdef USEGLP
	if(fconfig.glp) return;
	else
	#endif
	_glXGetSelectedEvent(_localdpy, draw, event_mask);
}

void glXGetSelectedEventSGIX(Display *dpy, GLXDrawable drawable, unsigned long *mask)
{
	#ifdef USEGLP
	if(fconfig.glp) return;
	else
	#endif
	_glXGetSelectedEventSGIX(_localdpy, drawable, mask);
}

GLXContext glXImportContextEXT(Display *dpy, GLXContextID contextID)
{
	#ifdef USEGLP
	if(fconfig.glp) return 0;
	else
	#endif
	return _glXImportContextEXT(_localdpy, contextID);
}

Bool glXIsDirect (Display *dpy, GLXContext ctx)
{
	#ifdef USEGLP
	if(fconfig.glp) return True;
	else
	#endif
	return _glXIsDirect(_localdpy, ctx);
}

int glXQueryContext(Display *dpy, GLXContext ctx, int attribute, int *value)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPQueryContext(ctx, attribute, value);
	else
	#endif
	return _glXQueryContext(_localdpy, ctx, attribute, value);
}

int glXQueryContextInfoEXT(Display *dpy, GLXContext ctx, int attribute, int *value)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPQueryContext(ctx, attribute, value);
	else
	#endif
	return _glXQueryContextInfoEXT(_localdpy, ctx, attribute, value);
}

void glXQueryDrawable(Display *dpy, GLXDrawable draw, int attribute, unsigned int *value)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPQueryBuffer(draw, attribute, value);
	else
	#endif
	return _glXQueryDrawable(_localdpy, draw, attribute, value);
}

Bool glXQueryExtension(Display *dpy, int *error_base, int *event_base)
{
	#ifdef USEGLP
	if(fconfig.glp)
		{if(error_base) *error_base=0;  if(event_base) *event_base=0;  return True;}
	else
	#endif
	return _glXQueryExtension(_localdpy, error_base, event_base);
}

const char *glXQueryExtensionsString(Display *dpy, int screen)
{
	#ifdef USEGLP
	if(fconfig.glp) return "GLX_SGIX_fbconfig GLX_SGIX_pbuffer";
	else
	#endif
	return _glXQueryExtensionsString(_localdpy, screen);
}

const char *glXQueryServerString(Display *dpy, int screen, int name)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPGetDeviceString(_localdev, name);
	else
	#endif
	return _glXQueryServerString(_localdpy, screen, name);
}

void glXQueryGLXPbufferSGIX(Display *dpy, GLXPbuffer pbuf, int attribute, unsigned int *value)
{
	#ifdef USEGLP
	if(fconfig.glp) glPQueryBuffer(pbuf, attribute, value);
	else
	#endif
	_glXQueryGLXPbufferSGIX(_localdpy, pbuf, attribute, value);
}

Bool glXQueryVersion(Display *dpy, int *major, int *minor)
{
	#ifdef USEGLP
	if(fconfig.glp) return glPQueryVersion(major, minor);
	else
	#endif
	return _glXQueryVersion(_localdpy, major, minor);
}

void glXSelectEvent(Display *dpy, GLXDrawable draw, unsigned long event_mask)
{
	#ifdef USEGLP
	if(fconfig.glp) return;
	else
	#endif
	_glXSelectEvent(_localdpy, draw, event_mask);
}

void glXSelectEventSGIX(Display *dpy, GLXDrawable drawable, unsigned long mask)
{
	#ifdef USEGLP
	if(fconfig.glp) return;
	else
	#endif
	_glXSelectEventSGIX(_localdpy, drawable, mask);
}

#ifdef sun
int glXVideoResizeSUN(Display *display, GLXDrawable window, float factor)
{
	#ifdef USEGLP
	if(fconfig.glp) return 0;
	else
	#endif
	return _glXVideoResizeSUN(_localdpy, window, factor);
}

int glXGetVideoResizeSUN(Display *display, GLXDrawable window, float *factor)
{
	#ifdef USEGLP
	if(fconfig.glp) return 0;
	else
	#endif
	return _glXGetVideoResizeSUN(_localdpy, window, factor);
}

int glXDisableXineramaSUN(Display *dpy)
{
	#ifdef USEGLP
	if(fconfig.glp) return 0;
	else
	#endif
	return _glXDisableXineramaSUN(_localdpy);
}
#endif

shimfuncdpy3(Bool, glXJoinSwapGroupNV, Display*, dpy, GLXDrawable, drawable, GLuint, group, return );
shimfuncdpy3(Bool, glXBindSwapBarrierNV, Display*, dpy, GLuint, group, GLuint, barrier, return );
shimfuncdpy4(Bool, glXQuerySwapGroupNV, Display*, dpy, GLXDrawable, drawable, GLuint*, group, GLuint*, barrier, return );
shimfuncdpy4(Bool, glXQueryMaxSwapGroupsNV, Display*, dpy, int, screen, GLuint*, maxGroups, GLuint*, maxBarriers, return );
shimfuncdpy3(Bool, glXQueryFrameCountNV, Display*, dpy, int, screen, GLuint*, count, return );
shimfuncdpy2(Bool, glXResetFrameCountNV, Display*, dpy, int, screen, return );

}
