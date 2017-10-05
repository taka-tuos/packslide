#include "ximage.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ft_gl.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//extern int ftgl_dotn;

int xi_dotn=16;
GLuint xi_texture_id;

void x_memcpy(void *dst, void *src, int size, int alpha)
{
	int i;
	for (i = 0; i < size; i++) {
		if (((int *)src)[i] != alpha) ((int *)dst)[i] = ((int *)src)[i];
	}
}

void x_memset(void *dst, int data, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		((int *)dst)[i] = data;
	}
}

void cut8(int *al2)
{
	if (*al2 > 255) *al2 = 255;
	if (*al2 < 0) *al2 = 0;
}

void x_dalpha(unsigned char *al, int sx, int sy)
{
	int x, y;
	int *al2 = (int *)malloc(sx*sy * 4);

	for (y = 0; y < sy; y++) {
		for (x = 0; x < sx; x++) {
			al2[y * sx + x] = al[y * sx + x];
		}
	}

	for (y = 0; y < sy; y++) {
		for (x = 0; x < sx; x++) {
			int a, a8, ad;

			a = al2[y * sx + x];

			a8 = (((a >> 7) << 0) | ((a >> 7) << 1) | ((a >> 7) << 2) | ((a >> 7) << 3) | ((a >> 7) << 4) | ((a >> 7) << 5) | ((a >> 7) << 6) | ((a >> 7) << 7));

			ad = a - a8;

			al2[y * sx + x] = a8;

			if ((x + 1) < sx) {
				al2[(y + 0) * sx + (x + 1)] += ad * 7 / 16;
				cut8(&al2[(y + 0) * sx + (x + 1)]);
			}

			if ((x + 1) < sx && (y + 1) < sy) {
				al2[(y + 1) * sx + (x + 1)] += ad * 1 / 16;
				cut8(&al2[(y + 1) * sx + (x + 1)]);
			}

			if ((y + 1) < sy) {
				al2[(y + 1) * sx + (x + 0)] += ad * 5 / 16;
				cut8(&al2[(y + 1) * sx + (x + 0)]);
			}

			if ((x - 1) >= 0 && (y + 1) < sy) {
				al2[(y + 1) * sx + (x - 1)] += ad * 3 / 16;
				cut8(&al2[(y + 1) * sx + (x - 1)]);
			}

			al[y * sx + x] = al2[y * sx + x];
		}
	}
}

void ximage_init(void)
{
	//ftgl_init("ipag.ttf");
	//ftgl_dots(12);
	
	int iw,ih,dummy;
	
	GLubyte *pix = stbi_load("resource/16x16.png", &iw, &ih, &dummy, 4);
	
	glGenTextures(1, &xi_texture_id);
	
	glBindTexture(GL_TEXTURE_2D, xi_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iw, ih, 0, GL_BGRA, GL_UNSIGNED_BYTE, pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	
	GLubyte *pix = (GLubyte *)malloc(iw * ih * 4);

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

	return obj;
}

ximage *ximage_load(char *f)
{
	int w,h,dummy;
	int i;

	unsigned int *pixels = (unsigned int *)stbi_load(f, &w, &h, &dummy, 4);

	return ximage_create(w,h,w,pixels);
}

void Gtexture_DefaultVS(float u, float v, float x, float y, float w, float h, void *ctx, void *ext)
{
	glTexCoord2f(0, 0); glVertex2f(x	, y		);
	glTexCoord2f(0, v);	glVertex2f(x	, y + h);
	glTexCoord2f(u, v);	glVertex2f(x + w, y + h);
	glTexCoord2f(u, 0);	glVertex2f(x + w, y		);
}

void ximage_bitblt(ximage *dst, ximage *src, int x, int y)
{
	/*int i;
	int ox = 0, oy = 0;
	int ex = 0, ey = 0;

	if (x < 0) ox = (0 - x);
	if (y < 0) oy = (0 - y);

	if (dst->w < (x + src->w)) ex = ((x + src->w) - dst->w) - ox;
	if (dst->h < (y + src->h)) ey = ((y + src->h) - dst->h) - oy;

	for (i = y + oy; i < y + src->h - oy - ey; i++) {
		x_memcpy(dst->pixels + (i * dst->p + (x + ox)), src->pixels + (((i - y) - oy) * src->p + (ox)), src->w - ox - ex, 0x00000000);
	}*/
	int w,h,iw,ih;
	float texture_sz[4];
	
	iw = src->w;
	ih = src->h;
	
	w = src->p >> 16;
	h = src->p & 0xffff;
	
	texture_sz[0] = (float)w / (float)iw;
	texture_sz[1] = (float)h / (float)ih;
	texture_sz[2] = (float)w;
	texture_sz[3] = (float)h;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, (GLuint)src->pixels);
	glBegin(GL_QUADS);
	Gtexture_DefaultVS(texture_sz[0], texture_sz[1], x, y, texture_sz[2], texture_sz[3], NULL, NULL);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

ximage_fhndl *ximage_initft(char *ttf)
{
	ximage_fhndl *obj = (ximage_fhndl *)malloc(sizeof(int));

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
		ox = strlen(sz)*od/2, oy = od;
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
		glTexCoord2f(u0, v0); glVertex2f(x	, y		);
		glTexCoord2f(u0, v1);	glVertex2f(x	, y + h);
		glTexCoord2f(u1, v1);	glVertex2f(x + w, y + h);
		glTexCoord2f(u1, v0);	glVertex2f(x + w, y		);
		sz++;
		x+=od;
	}
		
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glColor4f(1,1,1,1); 
	
	/*return;
	int c1 = c;
	c &= 0xff000000;
	c |= (((c1 >> 16) & 0xff) << 0);
	c |= (((c1 >> 8) & 0xff) << 8);
	c |= (((c1 >> 0) & 0xff) << 16);

	int ox, oy;
	int od = 96 * ((ftgl_fhndl *)hndl)->ftgl_dotn / 72 / 2;

	switch (mode) {
	case 0:
		ox = 0, oy = 0;
		break;
	case 1:
		ox = (ftgl_strlen((ftgl_fhndl *)hndl, sz) / 2), oy = od;
		break;
	case 2:
		ox = (ftgl_strlen((ftgl_fhndl *)hndl, sz)), oy = 0;
		break;
	}

	ftgl_dstr((ftgl_fhndl *)hndl, (ftgl_surf *)dst, x - ox, y - oy, sz, c | 0xff000000);*/
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
	//ftgl_dots((ftgl_fhndl *)hndl, size);
	*hndl = (float)size / 0.75f;
}

void ximage_boxfill(ximage *dst, int x0, int y0, int x1, int y1, int c)
{
	/*int c1 = c;
	c &= 0xff000000;
	c |= (((c1 >> 16) & 0xff) << 0);
	c |= (((c1 >> 8) & 0xff) << 8);
	c |= (((c1 >> 0) & 0xff) << 16);

	int i;
	int ox = 0, oy = 0;
	int ex = 0, ey = 0;

	if (x0 < 0) ox = (0 - x0);
	if (y0 < 0) oy = (0 - y0);

	if (dst->w < x1) ex = (x1 - dst->w) - ox;
	if (dst->h < y1) ey = (y1 - dst->h) - oy;

	for (i = y0 + oy; i < y1 - oy - ey; i++) {
		x_memset(dst->pixels + (i * dst->p + (x0 + ox)), c | 0xff000000, (x1 - x0) - ox - ex);
	}*/
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
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;){
		if (x0 >= 0 && y0 >= 0 && x0 < dst->w && y0 < dst->h)((int *)dst->pixels)[y0 * dst->p + x0] = c;
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void ximage_quitft(ximage_fhndl *hndl)
{
	ftgl_quit((ftgl_fhndl *)hndl);
}

void ximage_delete(ximage *dst)
{
	free(dst);
}
