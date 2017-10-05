#include "ft_gl.h"
#include "unicode.h"

FT_Error face_requester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface)
{
	*aface = (FT_Face)request_data;
	return 0;
}

ftgl_fhndl *ftgl_init(char *ttf)
{
	ftgl_fhndl *obj = (ftgl_fhndl *)malloc(sizeof(ftgl_fhndl));

	FT_Init_FreeType(&obj->library);
	FT_New_Face(obj->library, ttf, 0, &obj->face);

	FTC_Manager_New(obj->library, 0, 0, 0, face_requester, obj->face, &obj->cache_man);
	obj->font_type.face_id = (FTC_FaceID)1;
	obj->font_type.width = (short)0;
	obj->font_type.height = (short)0;
	obj->font_type.flags = FT_LOAD_RENDER | FT_LOAD_TARGET_MONO;
	FTC_ImageCache_New(obj->cache_man, &obj->image_cache);

	return obj;
}

void ftgl_dots(ftgl_fhndl *obj, int dots)
{
	obj->font_type.width = (short)0;
	obj->font_type.height = (short)((float)dots / 0.75f);
	obj->ftgl_dotn = dots;
}

int ftgl_dchr(ftgl_fhndl *obj, ftgl_surf *sdl_screen, int dx, int dy, int chr, int c)
{
	FT_Glyph glyph;
	FT_BitmapGlyph glyph_bitmap;
	FT_UInt glyph_index = FT_Get_Char_Index(obj->face, chr);
	FTC_ImageCache_Lookup(obj->image_cache, &obj->font_type, glyph_index, &glyph, NULL);
	glyph_bitmap = (FT_BitmapGlyph)glyph;

	int baseline = obj->face->ascender * obj->face->size->metrics.y_ppem / obj->face->units_per_EM;

	int x, y;
	for (y = 0; y < glyph_bitmap->bitmap.rows; y++) {
		for (x = 0; x < glyph_bitmap->bitmap.width; x += 8) {
			int vy = y + dy - glyph_bitmap->top + baseline;
			int vx = x + dx + glyph_bitmap->left;
			unsigned int *p, d;
			int i;
			p = (unsigned int *)sdl_screen->pixels + vy * sdl_screen->p + vx;
			d = glyph_bitmap->bitmap.buffer[y*glyph_bitmap->bitmap.pitch + (x >> 3)];
			if (vy >= 0 && vy < sdl_screen->h) for (i = 0; i < 8; i++) if (vx + i >= 0 && vx + i < sdl_screen->w) if ((d & (0x80 >> i)) != 0) { p[i] = c; }
		}
	}

	return glyph->advance.x >> 6 >> 4;
}

int ftgl_strlen(ftgl_fhndl *obj, char *sz)
{
	FT_Glyph glyph;

	unsigned char *s = (unsigned char *)sz;
	int ps = 0;
	int pc = 0;

	int x = 0;

	for (; *s != 0; s++) {
		if (ps == 0) {
			if ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)) {
				ps = *s;
			}
			else {
				FT_UInt glyph_index = FT_Get_Char_Index(obj->face, unicode_table_sb[*s]);
				FTC_ImageCache_Lookup(obj->image_cache, &obj->font_type, glyph_index, &glyph, NULL);
				int ax = glyph->advance.x >> 6 >> 4;
				FT_Vector delta;
				FT_Get_Kerning(obj->face, pc, unicode_table_sb[*s], FT_KERNING_DEFAULT, &delta);
				x += delta.x >> 6;
				x += ax >> 6;
				pc = unicode_table_sb[*s];
			}
		}
		else {
			int k, t;
			if (0x81 <= ps && ps <= 0x9f) {
				k = (ps - 0x81) * 2;
			}
			else {
				k = (ps - 0xe0) * 2 + 62;
			}
			if (0x40 <= *s && *s <= 0x7e) {
				t = *s - 0x40;
			}
			else if (0x80 <= *s && *s <= 0x9e) {
				t = *s - 0x80 + 63;
			}
			else {
				t = *s - 0x9f;
				k++;
			}
			FT_UInt glyph_index = FT_Get_Char_Index(obj->face, unicode_table_db[k * 94 + t]);
			FTC_ImageCache_Lookup(obj->image_cache, &obj->font_type, glyph_index, &glyph, NULL);
			int ax = glyph->advance.x >> 6 >> 4;
			FT_Vector delta;
			FT_Get_Kerning(obj->face, pc, unicode_table_db[k * 94 + t], FT_KERNING_DEFAULT, &delta);
			x += delta.x >> 6;
			x += ax >> 6;
			pc = unicode_table_db[k * 94 + t];
			ps = 0;
		}
	}

	return x;
}

void ftgl_dstr(ftgl_fhndl *obj, ftgl_surf *sdl_screen, int x, int y, char *sz, int c)
{
	return;
	
	int ps = 0;
	int pc = 0;

	unsigned char *s = (unsigned char *)sz;

	for (; *s != 0; s++) {
		if (ps == 0) {
			if ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc)) {
				ps = *s;
			}
			else {
				int ax = ftgl_dchr(obj, sdl_screen, x, y, unicode_table_sb[*s], c);
				FT_Vector delta;
				FT_Get_Kerning(obj->face, pc, unicode_table_sb[*s], FT_KERNING_DEFAULT, &delta);
				x += delta.x >> 6;
				x += ax >> 6;
				pc = unicode_table_sb[*s];
			}
		}
		else {
			int k, t;
			if (0x81 <= ps && ps <= 0x9f) {
				k = (ps - 0x81) * 2;
			}
			else {
				k = (ps - 0xe0) * 2 + 62;
			}
			if (0x40 <= *s && *s <= 0x7e) {
				t = *s - 0x40;
			}
			else if (0x80 <= *s && *s <= 0x9e) {
				t = *s - 0x80 + 63;
			}
			else {
				t = *s - 0x9f;
				k++;
			}
			int ax = ftgl_dchr(obj, sdl_screen, x, y, unicode_table_db[k * 94 + t], c);
			FT_Vector delta;
			FT_Get_Kerning(obj->face, pc, unicode_table_db[k * 94 + t], FT_KERNING_DEFAULT, &delta);
			x += delta.x >> 6;
			x += ax >> 6;
			pc = unicode_table_db[k * 94 + t];
			ps = 0;
		}
	}
}

void ftgl_quit(ftgl_fhndl *obj)
{
	FTC_Manager_Done(obj->cache_man);
	FT_Done_FreeType(obj->library);
}
