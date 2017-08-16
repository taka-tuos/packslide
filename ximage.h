#include <stdlib.h>
#include <string.h>

#ifndef __XIMAGE__
#define __XIMAGE__

typedef struct {
	unsigned int *pixels;
	int w,h,p;
} ximage;

typedef void ximage_fhndl;

void ximage_init(void);
ximage *ximage_create(int w, int h, int p, void *pixels);
ximage *ximage_load(char *f);
void ximage_bitblt(ximage *dst, ximage *src, int x, int y);
ximage_fhndl *ximage_initft(char *ttf);
void ximage_textout(ximage_fhndl *hndl, ximage *dst, int x, int y, int c, char *sz, int mode);
void ximage_textoutf(ximage_fhndl *hndl, ximage *dst, int x, int y, int c, int mode, char *fmt, ...);
void ximage_textsize(ximage_fhndl *hndl, int size);
void ximage_boxfill(ximage *dst, int x0, int y0, int x1, int y1, int c);
void ximage_lineto(ximage *dst, int x0, int y0, int x1, int y1, int c);
void ximage_boxnfill(ximage *dst, int x0, int y0, int x1, int y1, int c, int t);
void ximage_quitft(ximage_fhndl *hndl);
void ximage_delete(ximage *dst);

#endif