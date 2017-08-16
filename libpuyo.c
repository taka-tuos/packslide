#include "libpuyo.h"

// 関数のプロトタイプ宣言

// 全ブロックの同色で隣接している個数をチェックする
int Check(struct FIELD* pField)
{
	int nX = 0;
	int nY = 0;
	int nReturn = 0;
	struct FIELD stDummy;

	for (nX = 0; nX < WIDTH; ++nX)
	{
		for (nY = 0; nY < HEIGHT; ++nY)
		{
			// 空白＝０としているので、０の場合は何もしない
			if (pField->stBlock[nX][nY].nColor == 0)
			{
				continue;
			}

			// 消去フラグをクリアしておく
			pField->stBlock[nX][nY].nVanish = 0;

			// フィールド構造体をコピーして、検査用の構造体領域を作成する
			memcpy(&stDummy, pField, sizeof(struct FIELD));

			// 同色で隣接している個数をカウントする
			Count(&stDummy, nX, nY, &pField->stBlock[nX][nY].nVanish);

			// 同色で隣接している個数がVANISH定数を超えていたら、戻り値を１にする
			nReturn |= (pField->stBlock[nX][nY].nVanish >= VANISH);
		}
	}

	// １箇所でも消去される箇所があれば１、なければ０が返る
	return nReturn;
}

// VANISH定数よりたくさん同色で隣接していたら消去する
int Vanish(struct FIELD* pField)
{
	int nX = 0;
	int nY = 0;

	int en = 0;

	for (nX = 0; nX < WIDTH; ++nX)
	{
		for (nY = 0; nY < HEIGHT; ++nY)
		{
			// 同色での隣接数がVANISH定数以上か？
			if (pField->stBlock[nX][nY].nVanish >= VANISH)
			{
				// ブロック情報をクリアする
				memset(&pField->stBlock[nX][nY], 0, sizeof(struct BLOCK));
				en++;
			}
		}
	}

	return en;
}

// 消去されて空いた隙間を１段だけ詰める
int Slide(struct FIELD* pField)
{
	int nX = 0;
	int nY = 0;
	int nReturn = 0;

	for (nY = HEIGHT - 1; nY >= 1; --nY)
	{
		for (nX = 0; nX < WIDTH; ++nX)
		{
			// 検査箇所が空白で、１つ上が空白ではない場合
			if (pField->stBlock[nX][nY].nColor == 0 &&
				pField->stBlock[nX][nY - 1].nColor != 0)
			{
				// 検査箇所へ１つ上のブロック情報をコピーする
				memcpy(&pField->stBlock[nX][nY], &pField->stBlock[nX][nY - 1], sizeof(struct BLOCK));

				// １つ上のブロック情報をクリアする
				memset(&pField->stBlock[nX][nY - 1], 0, sizeof(struct BLOCK));

				// 動いた箇所があったので、戻り値を１にする
				nReturn = 1;
			}
		}
	}

	// 動いた箇所があった場合は１、そうでない場合は０が返る
	return nReturn;
}

// 同色で隣接している個数を数える（再帰関数）
void Count(struct FIELD* pField, int nX, int nY, int* pCount)
{
	// 検査箇所の色を保持する
	int nColor = pField->stBlock[nX][nY].nColor;

	// ダブって検査しないように、検査した箇所のブロック情報をクリアしておく
	memset(&pField->stBlock[nX][nY], 0, sizeof(struct BLOCK));

	// カウンタをインクリメントする
	++(*pCount);

	// １つ上のブロックを検査
	if (nY - 1 >= 0 && nColor == pField->stBlock[nX][nY - 1].nColor)
	{
		Count(pField, nX, nY - 1, pCount);
	}

	// １つ下のブロックを検査
	if (nY + 1 < HEIGHT && nColor == pField->stBlock[nX][nY + 1].nColor)
	{
		Count(pField, nX, nY + 1, pCount);
	}

	// １つ左のブロックを検査
	if (nX - 1 >= 0 && nColor == pField->stBlock[nX - 1][nY].nColor)
	{
		Count(pField, nX - 1, nY, pCount);
	}

	// １つ右のブロックを検査
	if (nX + 1 < WIDTH && nColor == pField->stBlock[nX + 1][nY].nColor)
	{
		Count(pField, nX + 1, nY, pCount);
	}
}