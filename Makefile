TARGET		= packslide
OBJS_TARGET	= core.o orzpcm.o ximage.o libpuyo.o ft_gl.o

CFLAGS += -O2 -g -std=gnu99 `sdl-config --cflags` `pkg-config freetype2 --cflags` -fpermissive
LIBS += `pkg-config freetype2 --libs` `sdl-config --libs` -lm -lc -lGL -lGLU -lglut -lstdc++ 

include Makefile.in
