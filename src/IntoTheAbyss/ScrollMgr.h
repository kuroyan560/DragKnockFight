#pragma once
#include "Vec.h"
#include "Singleton.h"
#include<vector>
#include<array>
#include"ShakeMgr.h"
#include"Camera.h"
#include"KuroMath.h"

class ScrollMgr : public Singleton<ScrollMgr> {

public:
	/*===== メンバ変数 =====*/
	Vec2<float> honraiScrollAmount;

	Vec2<float> scrollAmount;		//スクロール量
	float zoom;			// カメラのズーム量

	const std::vector<std::vector<int>> *mapChipPtr;//マップチップ情報のポインタ
	std::array<bool, 4> hitData;			//上下左右、どの部分にスクロール管理用のマップチップに引っかかったか
	std::array<bool, 4> oldHitData;			//上下左右、どの部分にスクロール管理用のマップチップに引っかかったかの前フレーム情報
	std::array<bool, 4> stopScrollData;		//上下左右、どの方向に行けなくさせるか
	Vec2<float> oldScrollData;

	std::array<Vec2<float>, 4> detectPos;//スクロール検知用の座標

	Vec2<float> scrollMaxValue;
	Vec2<float> scrollMinValue;


public:
	ScrollMgr();

	/*===== メンバ関数 =====*/
	//初期化
	inline void Init(const std::vector<std::vector<int>> *CHIP_DATA)
	{
		//scrollAmount = Vec2<float>(WIN_WIDTH_HALF, WIN_HEIGHT_HALF);
		//honraiScrollAmount = scrollAmount;

		mapChipPtr = CHIP_DATA;
		oldHitData[0] = true;
		oldHitData[1] = true;
		oldHitData[2] = true;
		oldHitData[3] = true;
	};

	void Update();


	void DetectMapChipForScroll(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE = Vec2<float>(1280 / 2.0f, 720 / 2.0f));

	void CalucurateScroll(const Vec2<float> &SCROLL_DATA);


	void WarpScroll(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE = Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	void Restart();
	void AlimentScrollAmount();


	enum { UP, DOWN, LEFT, RIGHT };

	bool warpFlag;

	Vec2<float> CaluStartScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
		Vec2<float> startPos;

		float adj = 50.0f / 2.0f;

		startPos.x = (detectPos[LEFT].x + SIZE.x);
		startPos.y = (detectPos[UP].y + SIZE.y);

		return startPos;
	};


	Vec2<float> CaluEndScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(detectPos[RIGHT].x, detectPos[DOWN].y);
		Vec2<float> endPos;

		float adj = 50.0f / 2.0f;

		endPos.x = (distance.x - SIZE.x);
		endPos.y = (distance.y - SIZE.y);

		return endPos;
	};

	void StopScroll(const int &DIR);

	Vec2<float>Affect(const Vec2<float>&Pos)	//スクロールとズームを適用させる
	{
		Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
		scrollShakeZoom.x *= zoom;
		scrollShakeZoom.y *= zoom;
		return Pos * zoom - scrollShakeZoom;
	}
};