#include "game.h"
#include "libpuyo.h"
#include "orzpcm.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define PGP 12.5f

#define GLUpdate glFlush

#define dprogress() \
	ximage_bitblt(__disp,__img,0,0); \
	 \
	ximage_boxnfill(__disp,320-128-4,240-8-2,320+128+3,240+8+2,0xffffff,1); \
	 \
	ximage_boxfill(__disp,320-128-2,240-8,320-128+(pg / 100.0f * 256.0f),240+8,0x802000); \
	 \
	/*SDL_UpdateRect(sdl_screen,0,0,0,0);*/ \
	GLUpdate(); \

void gfxSetView2D(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void gfxInitialize(int width, int height)
{
	gfxSetView2D(width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void sysQuitProgram(int code)
{
	SDL_Quit();

	exit(code);
}

int main(int argc, char *argv[])
{
	SDL_Event sdl_event;
	SDL_Surface *sdl_screen;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
	
	const SDL_VideoInfo* info = NULL;
	int width = 0;
	int height = 0;
	int bpp = 0;
	int flags = 0;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		/* Failed, exit. */
		fprintf(stderr, "Video initialization failed: %s\n",
			 SDL_GetError());
		sysQuitProgram(1);
	}

	info = SDL_GetVideoInfo();

	if(!info) {
		fprintf(stderr, "Video query failed: %s\n",
			 SDL_GetError());
		sysQuitProgram(1);
	}

	width = 640;
	height = 480;
	bpp = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

	flags = SDL_OPENGL /*| SDL_FULLSCREEN*/;

	if(SDL_SetVideoMode(width, height, bpp, flags) == 0) {
		fprintf(stderr, "Video mode set failed: %s\n",
			 SDL_GetError());
		sysQuitProgram(1);
	}

	gfxInitialize(width, height);

	sdl_screen = SDL_GetVideoSurface();//SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE/* | SDL_FULLSCREEN*/);

	SDL_ShowCursor(0);

	ximage *__disp = NULL;//ximage_create(640, 480, sdl_screen->pitch >> 2, sdl_screen->pixels);

	float pg = 0;

	ximage *__img = ximage_load("resource/back.png");

	//ximage *__img2 = ximage_load("resource/splash.png");

	pg += PGP;
	dprogress();

	SDL_Joystick *joy = SDL_JoystickOpen(0);

	int fps = 0;

	_orzpcm_init();

	_orzpcm_chunk ogg[5];

	//ximage_bitblt(__disp,__img2,0,0);

	//SDL_UpdateRect(sdl_screen,0,0,0,0);

	ogg[0] = _orzpcm_loadfile("resource/game_maoudamashii_1_battle15.wav");
	pg += PGP;
	dprogress();

	ogg[1] = _orzpcm_loadfile("resource/game_maoudamashii_7_event37.wav");
	pg += PGP;
	dprogress();

	ogg[2] = _orzpcm_loadfile("resource/btn07.wav");
	pg += PGP;
	dprogress();

	ogg[3] = _orzpcm_loadfile("resource/btn11.wav");
	pg += PGP;
	dprogress();

	ogg[4] = _orzpcm_loadfile("resource/game_maoudamashii_9_jingle02.wav");
	pg += PGP;
	dprogress();

	ximage_init();

	ximage_fhndl *tft = ximage_initft("resource/splafont.ttf");
	ximage_textsize(tft, 64);
	pg += PGP;
	dprogress();

	ximage_fhndl *sft = ximage_initft("resource/ipa-mona/ipag-mona.ttf");
	ximage_textsize(sft, 12);
	pg += PGP;
	dprogress();

	SDL_Delay(100);

	int ftim = SDL_GetTicks();

	int hgn;
	FILE *hg = fopen("hg.dat", "r+");

	fread(&hgn, 4, 1, hg);

	fclose(hg);

	ximage *__chip[5];
	int i, j;

	for (i = 0; i < 5; i++) {
		char f[64];
		sprintf(f, "resource/chip%d.png", i + 1);
		__chip[i] = ximage_load(f);
	}

	ximage_fhndl *cft = ximage_initft("resource/ipa-mona/ipag-mona.ttf");
	ximage_textsize(cft, 12);

	ximage_fhndl *nft = ximage_initft("resource/ipa-mona/ipag-mona.ttf");
	ximage_textsize(nft, 20);

	char fpss[16];
	fpss[0] = 0;

__reset:
	;
	int tti = _orzpcm_play(ogg[1], 1);

	while (1) {
		fps_adjust();
		
		glColor4f(1, 1, 1, 1.0);

		Uint8 *key = SDL_GetKeyState(NULL);

		ximage_bitblt(__disp, __img, 0, 0);

		ximage_textout(tft, __disp, 320 + 0, 187 + 0, 0xffffff, "PACK SLIDE", 1);


		if (fps % 30 < 15) ximage_textout(sft, __disp, 320, 287, 0xffffff, "HIT RETURN KEY", 1);

		ximage_textoutf(nft, __disp, 320, 480 - 32 - 64, (fps % 10 < 5) ? (0xffffff) : (0x0000ff), 1, "high %08d", hgn);

		ximage_textout(sft, __disp, 320, 480 - 32, 0xffffff, "(c)2016 TSH-TECH HIGISCHOOL D-KEN", 1);
		ximage_textout(sft, __disp, 320, 480 - 32 - 16, 0xffffff, "Powered by SDL and freetype", 1);

		if (SDL_GetTicks() - ftim >= 1000) {
			sprintf(fpss, "%d fps", fps);
			fps = 0;
			ftim = SDL_GetTicks();
		}

		ximage_textout(sft, __disp, 2, 2, 0xffffff, fpss, 0);

		//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
		GLUpdate();

		poll_event(&sdl_event);

		if (key[SDLK_RETURN]) break;
		if (key[SDLK_p]) {
			SDL_Quit();
			exit(0);
		}

		fps++;
	}

	ftim = SDL_GetTicks();
	fps = 0;

	struct FIELD stField;

	_orzpcm_stop(tti);

__tnt:

	tti = _orzpcm_play(ogg[0], 1);

	for (i = 0; i < 12; i++) {
		for (j = 0; j < 12; j++) {
			stField.stBlock[i][j].nColor = rand() % 5 + 1;
			stField.stBlock[i][j].nVanish = 0;
		}
	}

	int ny = 0;

	int kf[] = { 1, 1, 1, 1, 1, 1 };

	int score = 0;
	int level = 0;

	int ltime = 2700;

	int vtime = 0;

	for (i = 0; i < 90; i++) {
		fps_adjust();

		ximage_bitblt(__disp, __img, 0, 0);

		int n = 3 - i / 30;

		ximage_textoutf(tft, __disp, 320, 240, 0xffffff, 1, "%d", n);

		//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
		GLUpdate();

		poll_event(&sdl_event);
	}

	while (1) {
		fps_adjust();
		Uint8 *key = SDL_GetKeyState(NULL);

		ximage_bitblt(__disp, __img, 0, 0);

		if (SDL_GetTicks() - ftim >= 1000) {
			sprintf(fpss, "%d fps", fps);
			fps = 0;
			ftim = SDL_GetTicks();
		}

		for (i = 0; i < 12; i++) {
			for (j = 0; j < 12; j++) {
				if (stField.stBlock[j][i].nColor != 0) ximage_bitblt(__disp, __chip[stField.stBlock[j][i].nColor - 1], j * 32 + 128, i * 32 + 48);
			}
		}

		if (key[SDLK_UP]) {
			if (kf[0] == 0) {
				if (ny > 0) ny--;
				kf[0] = 1;
			}
		}
		else {
			kf[0] = 0;
		}

		if (key[SDLK_DOWN]) {
			if (kf[1] == 0) {
				if (ny < 11) ny++;
				kf[1] = 1;
			}
		}
		else {
			kf[1] = 0;
		}

		if (key[SDLK_LEFT]) {
			if (kf[2] == 0) {
				struct BLOCK nl[12];
				for (i = 0; i < 12; i++) {
					int n = i + 1;
					if (n >= 12) n = n - 12;
					nl[i] = stField.stBlock[n][ny];
				}
				for (i = 0; i < 12; i++) {
					stField.stBlock[i][ny] = nl[i];
				}
				kf[2] = 1;
			}
		}
		else {
			kf[2] = 0;
		}

		if (key[SDLK_RIGHT]) {
			if (kf[3] == 0) {
				struct BLOCK nl[12];
				for (i = 0; i < 12; i++) {
					int n = i - 1;
					if (n < 0) n = 12 + n;
					nl[i] = stField.stBlock[n][ny];
				}
				for (i = 0; i < 12; i++) {
					stField.stBlock[i][ny] = nl[i];
				}
				kf[3] = 1;
			}
		}
		else {
			kf[3] = 0;
		}

		int ln = 1;

		while (Check(&stField) != 0) {
			int n = Vanish(&stField) * 200;
			score += n * ln;
			ln++;
		}

		if (ln != 1) _orzpcm_play(ogg[3], 0);

		if (score > hgn) {
			hgn = score;
			FILE *hg = fopen("hg.dat", "r+");

			fwrite(&hgn, 4, 1, hg);

			fclose(hg);
		}

		Slide(&stField);

		if (key[SDLK_r]) {
			if (kf[4] == 0 && vtime == 0) {
				for (i = 0; i < 12; i++) {
					for (j = 0; j < 12; j++) {
						stField.stBlock[i][j].nColor = rand() % 5 + 1;
						stField.stBlock[i][j].nVanish = 0;
					}
				}
				level++;
				vtime = 450;
				kf[4] = 1;
			}
		}
		else {
			kf[4] = 0;
		}


		ximage_boxfill(__disp, 192, 8, 192 + (int)((float)vtime / 450.0*256.0), 32, 0x802000);
		ximage_textoutf(cft, __disp, 320, 20, 0xffffff, 1, "%02d", vtime / 30);


		ximage_boxnfill(__disp, 128 - 1, 48 + 32 * ny, 128 + 384 + 1, 48 + 32 + 1 + ny * 32, (fps % 10 < 5) ? (0xffffff) : (0xffff00), 2);

		ximage_textout(cft, __disp, 2, 2, 0xffffff, fpss, 0);

		ximage_textoutf(cft, __disp, 2, 18, 0xffffff, 0, "%08d pt", score);
		ximage_textoutf(cft, __disp, 2, 34, 0xffffff, 0, "high %08d", hgn);
		ximage_textoutf(cft, __disp, 2, 50, 0xffffff, 0, "level %d", level);

		int m, s, t;

		m = ltime / (30 * 60);
		s = (ltime / 30) % 60;
		t = (ltime % 30) * 3;

		int nc = 0x00ff00;
		if (ltime < 1800) nc = 0x00ffff;
		if (ltime < 900) nc = 0x0000ff;

		char tts[256];
		tts[0] = 0;

		int nc2 = 0x00ff00;
		if (ltime < 1800 + 30 && ltime > 1800 - 90) nc = 0x00ffff;
		if (ltime < 900 + 30 && ltime > 900 - 90) if (ltime < 900) nc = 0x0000ff;

		if (ltime < 1800 + 30 && ltime > 1800 - 90) strcpy(tts, "Žc‚è 1•ª");
		if (ltime < 900 + 30 && ltime > 900 - 90) strcpy(tts, "Žc‚è 30•b");

		if (ltime == 1800 + 30) _orzpcm_play(ogg[2], 0);
		if (ltime == 900 + 30) _orzpcm_play(ogg[2], 0);

		if (fps % 30 < 15) {
			for (i = -1; i < 2; i++) {
				for (j = -1; j < 2; j++) {
					ximage_textoutf(sft, __disp, 320 + i, 20 + j, 0, 1, "%s", tts);
				}
			}
			ximage_textoutf(sft, __disp, 320, 20, nc2, 1, "%s", tts);
		}

		ximage_textoutf(nft, __disp, 640 - 135, 2, nc, 0, "%02d:%02d.%02d", m, s, t);

		if (key[SDLK_m] && key[SDLK_LCTRL]) {
			_orzpcm_stop(tti);

			if (kf[5] == 0) {
				for (i = -2; i < 3; i++) {
					for (j = -2; j < 3; j++) {
						ximage_textout(sft, __disp, 320 + j, 480 - 32 - 16 - 16 - 8 - 16 + i, 0x000000, "ƒQ[ƒ€‚ð‘±‚¯‚é : C", 1);
						ximage_textout(sft, __disp, 320 + j, 480 - 32 - 16 - 8 + 2 - 16 + i, 0x000000, "‚â‚è‚È‚¨‚µ : I", 1);
						ximage_textout(sft, __disp, 320 + j, 480 - 32 - 16 - 8 + 2 + i, 0x000000, "ƒQ[ƒ€‚ð‚â‚ß‚é : Q", 1);
					}
				}

				ximage_textout(sft, __disp, 320, 480 - 32 - 16 - 16 - 8 - 16, 0xffffff, "ƒQ[ƒ€‚ð‘±‚¯‚é : C", 1);
				ximage_textout(sft, __disp, 320, 480 - 32 - 16 - 8 + 2 - 16, 0xffffff, "‚â‚è‚È‚¨‚µ : I", 1);
				ximage_textout(sft, __disp, 320, 480 - 32 - 16 - 8 + 2, 0xffffff, "ƒQ[ƒ€‚ð‚â‚ß‚é : Q", 1);

				//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
				GLUpdate();

				while (1) {
					fps_adjust();

					Uint8 *key = SDL_GetKeyState(NULL);

					poll_event(&sdl_event);

					if (key[SDLK_c]) break;
					if (key[SDLK_i]) {
						_orzpcm_stop(tti);
						goto __tnt;
					}
					if (key[SDLK_q])  {
						goto __reset;
						return 0;
					}
				}

				kf[5] = 1;
			}
			_orzpcm_resume(tti);
		}
		else {
			kf[5] = 0;
		}

		if (ltime == 0) {
			int i;

			_orzpcm_stop(tti);

			_orzpcm_play(ogg[4], 0);

			ximage_textout(tft, __disp, 320, 240, 0xffffff, "TIME UP", 1);

			//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
			GLUpdate();

			for (i = 0; i < 60; i++) {
				fps_adjust();

				poll_event(&sdl_event);
			}

			for (i = -2; i < 3; i++) {
				for (j = -2; j < 3; j++) {
					ximage_textout(sft, __disp, 320 + j, 480 - 32 - 16 - 16 - 8 + i, 0x000000, "ƒQ[ƒ€‚ð‘±‚¯‚é : C", 1);
					ximage_textout(sft, __disp, 320 + j, 480 - 32 - 16 - 8 + 2 + i, 0x000000, "ƒQ[ƒ€‚ð‚â‚ß‚é : Q", 1);
				}
			}


			ximage_textout(sft, __disp, 320, 480 - 32 - 16 - 16 - 8, 0xffffff, "ƒQ[ƒ€‚ð‘±‚¯‚é : C", 1);
			ximage_textout(sft, __disp, 320, 480 - 32 - 16 - 8 + 2, 0xffffff, "ƒQ[ƒ€‚ð‚â‚ß‚é : Q", 1);

			//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
			GLUpdate();

			while (1) {
				fps_adjust();

				poll_event(&sdl_event);

				if (key[SDLK_c]) {
					goto __tnt;
				}
				if (key[SDLK_q]) {
					goto __reset;
					return 0;
				}
			}

			ltime = 2700;

			score = 0;

			for (i = 0; i < 12; i++) {
				for (j = 0; j < 12; j++) {
					stField.stBlock[i][j].nColor = rand() % 5 + 1;
					stField.stBlock[i][j].nVanish = 0;
				}
			}

			level = 0;

			vtime = 0;

			kf[4] = 1;
		}

		//SDL_UpdateRect(sdl_screen, 0, 0, 0, 0);
		GLUpdate();

		poll_event(&sdl_event);

		fps++;

		ltime--;
		if (vtime > 0) vtime--;
	}

	SDL_JoystickClose(joy);

	SDL_Quit();

	return 0;
}

void fps_adjust(void)
{
	static unsigned long maetime = 0;
	static int frame = 0;
	long sleeptime;
	if (!maetime) maetime = SDL_GetTicks();
	frame++;
	sleeptime = (frame < FPS) ?
		(maetime + (long)((double)frame*(1000.0 / FPS)) - SDL_GetTicks()) :
		(maetime + 1000 - SDL_GetTicks());
	if (sleeptime > 0)SDL_Delay(sleeptime);
	if (frame >= FPS) {
		frame = 0;
		maetime = SDL_GetTicks();
	}
}

void poll_event(SDL_Event *sdl_event)
{
	if (SDL_PollEvent(sdl_event)) {
		switch (sdl_event->type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
}
