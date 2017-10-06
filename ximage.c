#include "ximage.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glew.h>
#include <GL/glut.h>

GLuint xi_texture_id;
GLuint xfbo;
int xi_w, xi_h;

void ximage_init(int w, int h)
{
	int iw,ih,dummy;
	
	xi_w = w;
	xi_h = h;
	
	GLubyte *pix = stbi_load("resource/16x16.png", &iw, &ih, &dummy, 4);
	
	glGenFramebuffersEXT(1, &xfbo);
	
	glGenTextures(1, &xi_texture_id);
	
	glBindTexture(GL_TEXTURE_2D, xi_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iw, ih, 0, GL_BGRA, GL_UNSIGNED_BYTE, pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

ximage *ximage_fbo(int w, int h)
{
	return ximage_create(w, h, w, NULL);
}

ximage *ximage_update(ximage *fbo)
{
	ximage_bitblt(NULL, fbo, 0, 0);
}

void ximage_fboenable(ximage *fbo)
{
	if(!fbo) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	} else {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, xfbo);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, (GLuint)fbo->p, 0);
	}
}

GLuint __pow2_list[] = {
	1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536
};

GLuint __return_pow2(int n)
{
	int i;
	for (i = 1; i < 16; i++) {
		if (__pow2_list[i] >= n) return __pow2_list[i];
	}
	return 0;
}

ximage *ximage_create(int w, int h, int p, void *pixels)
{
	ximage *obj = (ximage *)malloc(sizeof(ximage));
	GLuint texture_id;
	
	int iw = __return_pow2(w);
	int ih = __return_pow2(h);
	
	GLubyte *pix = NULL;
	
	if(pixels) {
		pix = (GLubyte *)malloc(iw * ih * 4);

		memset(pix, 0, iw * ih * 4);
		
		int x,y;
		
		GLubyte *bit = (GLubyte *)pixels;

		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				pix[(y * iw + x) * 4 + 0] = bit[(y * w + x) * 4 + 0];
				pix[(y * iw + x) * 4 + 1] = bit[(y * w + x) * 4 + 1];
				pix[(y * iw + x) * 4 + 2] = bit[(y * w + x) * 4 + 2];
				pix[(y * iw + x) * 4 + 3] = bit[(y * w + x) * 4 + 3];
			}
		}
	}
	
	glGenTextures(1, &texture_id);
	
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iw, ih, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	obj->pixels = (uint32_t *)texture_id;

	obj->w = iw;
	obj->h = ih;
	obj->p = (w << 16) | h;
	obj->f = pixels ? 1 : 0;

	return obj;
}

ximage *ximage_load(char *f)
{
	int w,h,dummy;
	int i;

	unsigned int *pixels = (unsigned int *)stbi_load(f, &w, &h, &dummy, 4);

	return ximage_create(w,h,w,pixels);
}

void ximage_bitblt(ximage *dst, ximage *src, int x, int y)
{
	int w,h,iw,ih;
	float u,v;
	
	iw = src->w;
	ih = src->h;
	
	w = src->p >> 16;
	h = src->p & 0xffff;
	
	u = (float)w / (float)iw;
	v = (float)h / (float)ih;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (GLuint)src->pixels);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(x	, y		);
	glTexCoord2f(0, v);	glVertex2f(x	, y + h);
	glTexCoord2f(u, v);	glVertex2f(x + w, y + h);
	glTexCoord2f(u, 0);	glVertex2f(x + w, y		);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

ximage_fhndl *ximage_initft()
{
	ximage_fhndl *obj = (ximage_fhndl *)malloc(sizeof(ximage_fhndl));

	*obj=16;

	return obj;
}

void ximage_textout(ximage_fhndl *hndl, ximage *dst, int x, int y, int c, char *sz, int mode)
{
	glColor4f(((c >> 0) & 0xff)/255.0f,((c >> 8) & 0xff)/255.0f,((c >> 16) & 0xff)/255.0f,1); 
	
	int ox, oy;
	int od = *hndl;
	
	const float oneC = (1.0f / 16.0f);
	const float oneD = 0;
	
	switch (mode) {
	case 0:
		ox = 0, oy = 0;
		break;
	case 1:
		ox = strlen(sz)*od/2, oy = od/2;
		break;
	case 2:
		ox = strlen(sz)*od, oy = 0;
		break;
	}
	
	x -= ox;
	y -= oy;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, xi_texture_id);
	
	glBegin(GL_QUADS);
	
	while(*sz) {
		int ch = *sz;
		float u0 = oneC * (float)(ch & 15) + oneD;
		float v0 = oneC * (float)(ch >> 4) + oneD;
		float u1 = oneC * (float)(ch & 15) + oneC + oneD;
		float v1 = oneC * (float)(ch >> 4) + oneC + oneD;
		float w = od, h = od;
		glTexCoord2f(u0, v0);	glVertex2f(x	, y		);
		glTexCoord2f(u0, v1);	glVertex2f(x	, y + h);
		glTexCoord2f(u1, v1);	glVertex2f(x + w, y + h);
		glTexCoord2f(u1, v0);	glVertex2f(x + w, y		);
		sz++;
		x+=od;
	}
		
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glColor4f(1,1,1,1); 
}

void ximage_textoutf(ximage_fhndl *hndl, ximage *dst, int x, int y, int c, int mode, char *fmt, ...)
{
	char buf[1024];

	va_list argptr;
	va_start(argptr, fmt);

	vsnprintf(buf, sizeof(buf), fmt, argptr);

	va_end(argptr);

	ximage_textout(hndl, dst, x, y, c, buf, mode);
}

void ximage_textsize(ximage_fhndl *hndl, int size)
{
	*hndl = (float)size / 0.75f;
}

void ximage_boxfill(ximage *dst, int x0, int y0, int x1, int y1, int c)
{
	glColor4f(((c >> 0) & 0xff)/255.0f,((c >> 8) & 0xff)/255.0f,((c >> 16) & 0xff)/255.0f,1); 
	glBegin(GL_QUADS);
	glVertex2f(x0, y0);
	glVertex2f(x0, y1);
	glVertex2f(x1, y1);
	glVertex2f(x1, y0);
	glEnd();
	glColor4f(1,1,1,1); 
}

void ximage_boxnfill(ximage *dst, int x0, int y0, int x1, int y1, int c, int t)
{
	ximage_boxfill(dst, x0 + 0, y0 + 0, x1 + 0, y0 + t, c);
	ximage_boxfill(dst, x0 + 0, y1 - t, x1 + 0, y1 + 0, c);

	ximage_boxfill(dst, x0 + 0, y0 + 0, x0 + t, y1 + 0, c);
	ximage_boxfill(dst, x1 - t, y0 + 0, x1 + 0, y1 + 0, c);
}


void ximage_lineto(ximage *dst, int x0, int y0, int x1, int y1, int c)
{
	glColor4f(((c >> 0) & 0xff)/255.0f,((c >> 8) & 0xff)/255.0f,((c >> 16) & 0xff)/255.0f,1); 
	glBegin(GL_LINES);
	glVertex2f(x0, y0);
	glVertex2f(x1, y1);
	glEnd();
	glColor4f(1,1,1,1); 
}

void ximage_quitft(ximage_fhndl *hndl)
{
	
}

void ximage_delete(ximage *dst)
{
	free(dst);
}
