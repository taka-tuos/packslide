#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef __ERASE__
#define __ERASE__

#define WIDTH	12		// フィールド幅
#define HEIGHT	12		// フィールド高
#define VANISH	4		// 消去に必要な個数
#define COLOR	5		// 色数

// ブロック管理用構造体
struct BLOCK
{
	int nColor;		   // 色
	int nVanish;	// 消去フラグ
};

// フィールド管理用構造体
struct FIELD
{
	struct BLOCK stBlock[WIDTH][HEIGHT];	// ブロック構造体配列
};


int Check(struct FIELD* pField);
int Vanish(struct FIELD* pField);
int Slide(struct FIELD* pField);
void Count(struct FIELD* pField, int nX, int nY, int* pCount);

#endif
