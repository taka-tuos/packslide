#include <SDL/SDL.h>

#ifndef __ORZPCM__
#define __ORZPCM__

#define ORZPCM_CH 16

typedef struct {
	int fs;
	uint8_t *d;
} _orzpcm_chunk;

void _orzpcm_init(void);
_orzpcm_chunk _orzpcm_loadfile(char *filename);
int _orzpcm_play(_orzpcm_chunk chunk, int loop);
void _orzpcm_stop(int ch);
void _orzpcm_resume(int ch);

#endif
