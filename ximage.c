#include "ximage.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ft_gl.h"

extern int ftgl_dotn;

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
}

ximage *ximage_load(char *f)
{
	ximage *obj = (ximage *)malloc(sizeof(ximage));
	int dummy;
	int i;

	obj->pixels = (unsigned int *)stbi_load(f, &obj->w, &obj->h, &dummy, 4);

	for (i = 0; i < obj->w*obj->h; i++) {
		int c = obj->pixels[i];
		obj->pixels[i] &= 0xff000000;
		obj->pixels[i] |= (((c >> 16) & 0xff) << 0);
		obj->pixels[i] |= (((c >> 8) & 0xff) << 8);
		obj->pixels[i] |= (((c >> 0) & 0xff) << 16);
	}

	unsigned char *al = (unsigned char *)malloc(obj->w*obj->h);
	for (i = 0; i < obj->w*obj->h; i++) {
		al[i] = (obj->pixels[i] >> 24);
		int nx = i % obj->w;
		int ny = i / obj->w;
		if ((nx == 31 || nx == 0) && (ny == 31 || ny == 0) && obj->w == 32 && obj->h == 32) al[i] = 0;
	}
	x_dalpha(al, obj->w, obj->h);

	for (i = 0; i < obj->w*obj->h; i++) {
		if (al[i] != 0) obj->pixels[i] &= 0xffffff;
		else obj->pixels[i] = 0;
		obj->pixels[i] |= (al[i] << 24);
	}

	obj->p = obj->w;

	return obj;
}

ximage *ximage_create(int w, int h, int p, void *pixels)
{
	ximage *obj = (ximage *)malloc(sizeof(ximage));

	obj->pixels = (uint32_t *)pixels;

	obj->w = w;
	obj->h = h;
	obj->p = p;

	return obj;
}

void ximage_bitblt(ximage *dst, ximage *src, int x, int y)
{
	int i;
	int ox = 0, oy = 0;
	int ex = 0, ey = 0;

	if (x < 0) ox = (0 - x);
	if (y < 0) oy = (0 - y);

	if (dst->w < (x + src->w)) ex = ((x + src->w) - dst->w) - ox;
	if (dst->h < (y + src->h)) ey = ((y + src->h) - dst->h) - oy;

	for (i = y + oy; i < y + src->h - oy - ey; i++) {
		x_memcpy(dst->pixels + (i * dst->p + (x + ox)), src->pixels + (((i - y) - oy) * src->p + (ox)), src->w - ox - ex, 0x00000000);
	}
}

ximage_fhndl *ximage_initft(char *ttf)
{
	ximage_fhndl *obj = (ximage_fhndl *)ftgl_init(ttf);
	ftgl_dots((ftgl_fhndl *)obj, 12);

	return obj;
}

void ximage_textout(ximage_fhndl *hndl, ximage *dst, int x, int y, int c, char *sz, int mode)
{
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

	ftgl_dstr((ftgl_fhndl *)hndl, (ftgl_surf *)dst, x - ox, y - oy, sz, c | 0xff000000);
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
	ftgl_dots((ftgl_fhndl *)hndl, size);
}

void ximage_boxfill(ximage *dst, int x0, int y0, int x1, int y1, int c)
{
	int c1 = c;
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
	}
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
