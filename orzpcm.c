#include "orzpcm.h"

void _orzpcm_callback(void *unused, Uint8 *stream, int len);

SDL_AudioSpec Desired;
SDL_AudioSpec Obtained;

int _orzpcm_flag[ORZPCM_CH];
int _orzpcm_ptr[ORZPCM_CH];
int _orzpcm_limit[ORZPCM_CH];
uint8_t *_orzpcm_dats[ORZPCM_CH];
int _orzpcm_loop[ORZPCM_CH];

struct RiffHeader {
	uint32_t riff;
	int32_t  size;
	uint32_t type;
};

struct FormatChunk {
	uint32_t id;
	int32_t size;
	int16_t format;
	uint16_t channels;
	uint32_t samplerate;
	uint32_t bytepersec;
	uint16_t blockalign;
	uint16_t bitswidth;
};

struct DataChunk {
	uint32_t id;
	int32_t size;
};

void _orzpcm_init(void)
{
	memset(_orzpcm_flag, 0, sizeof(_orzpcm_flag));
	memset(_orzpcm_ptr, 0, sizeof(_orzpcm_ptr));
	memset(_orzpcm_limit, 0, sizeof(_orzpcm_limit));
	memset(_orzpcm_loop, 0, sizeof(_orzpcm_loop));

	Desired.freq = 44100;
	Desired.format = AUDIO_S16LSB;
	Desired.channels = 2;
	Desired.samples = 441;
	Desired.callback = _orzpcm_callback;
	Desired.userdata = NULL;

	SDL_OpenAudio(&Desired, &Obtained);
	SDL_PauseAudio(0);
}

void _orzpcm_callback(void *unused, Uint8 *stream, int len)
{
	int i;
	static int err = 0;
	static unsigned int step = 0;
	Sint16 *frames = (Sint16 *)stream;
	int framesize = len / 2;
	for (i = 0; i < framesize; i++, step++) {
		int dat = 0;
		int j;
		//int c = 0;
		for (j = 0; j < ORZPCM_CH; j++) {
			if (_orzpcm_ptr[j] < _orzpcm_limit[j] && _orzpcm_flag[j] != 0) {
				_orzpcm_ptr[j]++;
				int d = _orzpcm_dats[j][_orzpcm_ptr[j] >> 2];
				int d2 = (d << 8) | d;
				dat += (d2 - 32767);
				//c++;
				if (_orzpcm_ptr[j] >= _orzpcm_limit[j] && _orzpcm_loop[j]) _orzpcm_ptr[j] = 0;
				else if (_orzpcm_ptr[j] >= _orzpcm_limit[j]) _orzpcm_flag[j] = 0;
			}
		}
		if (dat > 32767 || dat < -32767) {
			float dif = 32767.0f / abs(dat);
			dat = (int)((float)dat * dif);
		}
		frames[i] = dat;
	}
}

_orzpcm_chunk _orzpcm_loadfile(char *filename)
{
	int a, b, c;
	uint8_t *d;

	FILE *fp = fopen(filename, "rb");

	struct RiffHeader riff;
	struct FormatChunk fmt;
	struct DataChunk data;

	fread(&riff, 12, 1, fp);
	fread(&fmt, 24, 1, fp);
	fread(&data, 8, 1, fp);

	d = (uint8_t *)malloc(data.size);
	c = data.size << 2;

	fread(d, 1, data.size, fp);

	fclose(fp);

	_orzpcm_chunk chunk;

	chunk.d = d;
	chunk.fs = c;

	return chunk;
}

int _orzpcm_play(_orzpcm_chunk chunk, int loop)
{
	int n;

	for (n = 0; n < ORZPCM_CH; n++) if (_orzpcm_flag[n] == 0) break;

	_orzpcm_limit[n] = chunk.fs;
	_orzpcm_ptr[n] = 0;
	_orzpcm_dats[n] = chunk.d;
	_orzpcm_flag[n] = 1;
	_orzpcm_loop[n] = loop;

	return n;
}

void _orzpcm_stop(int ch)
{
	_orzpcm_flag[ch] = 0;
}

void _orzpcm_resume(int ch)
{
	_orzpcm_flag[ch] = 1;
}