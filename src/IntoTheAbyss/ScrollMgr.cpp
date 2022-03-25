#include "ScrollMgr.h"
#include "ViewPort.h"

void ScrollMgr::DetectMapChipForScroll(Vec2<float> PLAYER_POS, Vec2<float> SIZE)
{

	//プレイヤーの座標を基準に上下左右に検知用の座標を設定する　←　カメラ画角と一緒
	std::array<Vec2<float>, 4> screenPos;//画角
	Vec2<float> upVec, sideVec;											//プレイヤーからどれくらい離れた所に検知用の座標を置くか

	Vec2<float> win = SIZE;

	screenPos[UP] = { PLAYER_POS.x,PLAYER_POS.y - win.y };
	screenPos[DOWN] = { PLAYER_POS.x,PLAYER_POS.y + win.y };
	screenPos[LEFT] = { PLAYER_POS.x - win.x,PLAYER_POS.y };
	screenPos[RIGHT] = { PLAYER_POS.x + win.x,PLAYER_POS.y };



	const std::vector<std::vector<int>> chipData = *mapChipPtr;



	float chipSize = 50.0f;
	int count = 0;
	std::array<Vec2<float>, 4>tmpDetect;
	//スクロール範囲を検知する
	for (int y = 0; y < chipData.size(); ++y)
	{
		for (int x = 0; x < chipData[y].size(); ++x)
		{
			if (chipData[y][x] == 2)
			{
				tmpDetect[count] = { x * chipSize,y * chipSize };
				++count;
			}
		}
	}

	{
		int leftData = 0;
		int rightData = 0;
		int upData = 0;
		int downData = 0;
		//上下左右どちらにあるのかを整理する
		for (int i = 0; i < 4; ++i)
		{
			//どこが一番上か調べる
			if (tmpDetect[i].y < tmpDetect[upData].y)
			{
				upData = i;
			}
			//どこが一番下か調べる
			if (tmpDetect[downData].y < tmpDetect[i].y)
			{
				downData = i;
			}
			//どこが一番右か調べる
			if (tmpDetect[rightData].x < tmpDetect[i].x)
			{
				rightData = i;
			}
			//どこが一番左か調べる
			if (tmpDetect[i].x < tmpDetect[leftData].x)
			{
				leftData = i;
			}
		}

		//上下左右に仕分けする
		detectPos[UP] = tmpDetect[upData];
		detectPos[DOWN] = tmpDetect[downData];
		detectPos[LEFT] = tmpDetect[leftData];
		detectPos[RIGHT] = tmpDetect[rightData];
	}





	//どこからどこまでスクロールしていいか決める
	//上
	if (screenPos[UP].y < detectPos[UP].y)
	{
		hitData[UP] = false;

	}
	else
	{
		hitData[UP] = true;
	}


	//下
	if (detectPos[DOWN].y < screenPos[DOWN].y)
	{
		hitData[DOWN] = false;

	}
	else
	{
		hitData[DOWN] = true;
	}


	//左
	if (screenPos[LEFT].x < detectPos[LEFT].x)
	{
		hitData[LEFT] = false;

	}
	else
	{
		hitData[LEFT] = true;
	}


	//右
	if (detectPos[RIGHT].x < screenPos[RIGHT].x)
	{
		hitData[RIGHT] = false;

	}
	else
	{
		hitData[RIGHT] = true;
	}


	//スクロールの最大値を求める計算式-----------------------
	Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
	Vec2<float> startPos, endPos;

	float adj = chipSize / 2.0f;

	startPos.x = (detectPos[LEFT].x + win.x);
	endPos.x = (distance.x - win.x);

	startPos.y = (detectPos[UP].y + win.y);
	endPos.y = (distance.y - win.y);

	scrollMaxValue = endPos - startPos;
	//スクロールの最大値を求める計算式-----------------------


	//ブロックサイズ半分ズレいているので調整
	scrollMaxValue.x += adj;
	scrollMaxValue.y += adj;
	scrollMinValue = { -adj,-adj };
}

void ScrollMgr::CalucurateScroll(Vec2<float> SCROLL_DATA)
{
	if (!warpFlag)
	{
		//上下でスクロール管理用のチップを検知したらスクロールしない
		//if (hitData[UP] && hitData[DOWN])
		{
			honraiScrollAmount.y -= SCROLL_DATA.y;
		}
		//左右でスクロール管理用のチップを検知したらスクロールしない
		//if (hitData[LEFT] && hitData[RIGHT])
		{
			honraiScrollAmount.x -= SCROLL_DATA.x;
		}

		if (!hitData[UP])
		{
			honraiScrollAmount.y = scrollMinValue.y;
		}
		if (!hitData[DOWN])
		{
			honraiScrollAmount.y = scrollMaxValue.y;
		}

		if (!hitData[LEFT])
		{
			honraiScrollAmount.x = scrollMinValue.x;
		}
		if (!hitData[RIGHT])
		{
			honraiScrollAmount.x = scrollMaxValue.x;
		}


		for (int i = 0; i < 4; ++i)
		{
			oldHitData[i] = hitData[i];
		}
		oldScrollData = SCROLL_DATA;
	}
	else
	{
		warpFlag = false;
	}
}

void ScrollMgr::WarpScroll(Vec2<float> PLAYER_POS, Vec2<float> SIZE)
{
	//プレイヤーの座標を基準に上下左右に検知用の座標を設定する　←　カメラ画角と一緒
	std::array<Vec2<float>, 4> screenPos;//画角
	Vec2<float> win = SIZE;

	screenPos[UP] = { PLAYER_POS.x,PLAYER_POS.y - win.y };
	screenPos[DOWN] = { PLAYER_POS.x,PLAYER_POS.y + win.y };
	screenPos[LEFT] = { PLAYER_POS.x - win.x,PLAYER_POS.y };
	screenPos[RIGHT] = { PLAYER_POS.x + win.x,PLAYER_POS.y };

	//どこからどこまでスクロールしていいか決める
	//上
	if (screenPos[UP].y < detectPos[UP].y)
	{
		hitData[UP] = false;
	}
	else
	{
		hitData[UP] = true;
	}



	//下
	if (detectPos[DOWN].y < screenPos[DOWN].y)
	{
		hitData[DOWN] = false;
	}
	else
	{
		hitData[DOWN] = true;
	}


	//左

	if (screenPos[LEFT].x < detectPos[LEFT].x)
	{
		hitData[LEFT] = false;
	}
	else
	{
		hitData[LEFT] = true;
	}

	//右
	if (detectPos[RIGHT].x < screenPos[RIGHT].x)
	{
		hitData[RIGHT] = false;
	}
	else
	{
		hitData[RIGHT] = true;
	}

	//スクロールの最大値を求める計算式-----------------------
	const std::vector<std::vector<int>> chipData = *mapChipPtr;

	float chipSize = 50.0f;
	Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
	Vec2<float> startPos, endPos;

	float adj = chipSize / 2.0f;

	startPos.x = (detectPos[LEFT].x + win.x);
	endPos.x = (distance.x - win.x);

	startPos.y = (detectPos[UP].y + win.y);
	endPos.y = (distance.y - win.y);
	//スクロールの最大値を求める計算式-----------------------


	//下、左、右のワープ時のスクロールの値を修正-----------------------
	if (hitData[DOWN] && !oldHitData[DOWN])
	{
		float tmp = endPos.x - PLAYER_POS.x;
		float tmp2 = endPos.y - PLAYER_POS.y;
		honraiScrollAmount.x = scrollMaxValue.x - tmp;
		honraiScrollAmount.y = scrollMaxValue.y - tmp2;
		warpFlag = true;
	}
	if (hitData[LEFT] && !oldHitData[LEFT])
	{
		honraiScrollAmount.x = PLAYER_POS.x - startPos.x;
		honraiScrollAmount.y = PLAYER_POS.y - startPos.y;
		warpFlag = true;
	}
	if (hitData[RIGHT] && !oldHitData[RIGHT])
	{
		float tmp = endPos.x - PLAYER_POS.x;
		float tmp2 = endPos.y - PLAYER_POS.y;
		honraiScrollAmount.x = scrollMaxValue.x - tmp;
		honraiScrollAmount.y = scrollMaxValue.y - tmp2;
		warpFlag = true;
	}
	//下、左、右のワープ時のスクロールの値を修正-----------------------

}

void ScrollMgr::Restart()
{
	scrollAmount = honraiScrollAmount;
	warpFlag = true;
}

void ScrollMgr::AlimentScrollAmount()
{
	if (honraiScrollAmount.x <= scrollMinValue.x)
	{
		honraiScrollAmount.x = scrollMinValue.x;
	}
	if (honraiScrollAmount.y <= scrollMinValue.y)
	{
		honraiScrollAmount.y = scrollMinValue.y;
	}


	if (scrollMaxValue.x <= honraiScrollAmount.x)
	{
		honraiScrollAmount.x = scrollMaxValue.x;
	}
	if (scrollMaxValue.y <= honraiScrollAmount.y)
	{
		honraiScrollAmount.y = scrollMaxValue.y;
	}

}

void ScrollMgr::StopScroll(const int &DIR)
{
	stopScrollData[DIR] = true;
}
