#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include <SDL/SDL.h>

typedef struct {
	unsigned int *pixels;
	int w,h,p;
} ftgl_surf;

typedef struct {
	FT_Library library;
	FT_Face face;

	int ftgl_dotn;

	FTC_Manager cache_man;
	FTC_ImageCache image_cache;
	FTC_CMapCache cmap_cache;
	FTC_ImageTypeRec font_type;
} ftgl_fhndl;

ftgl_fhndl *ftgl_init(char *ttf);
void ftgl_dots(ftgl_fhndl *obj, int dots);
int ftgl_dchr(ftgl_fhndl *obj, ftgl_surf *sdl_screen, int dx, int dy, int chr, int c);
void ftgl_dstr(ftgl_fhndl *obj, ftgl_surf *sdl_screen, int x, int y, char *sz, int c);
int ftgl_strlen(ftgl_fhndl *obj, char *sz);
void ftgl_quit(ftgl_fhndl *obj);
