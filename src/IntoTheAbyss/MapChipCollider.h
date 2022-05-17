#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "ScrollMgr.h"
#include "Intersected.h"
#include <vector>

using namespace std;

// マップサイズ
const float MAP_CHIP_HALF_SIZE = 25.0f;
const float MAP_CHIP_SIZE = 50.0f;

class MapChipCollider : public Singleton<MapChipCollider> {


	/*===== メンバ関数 =====*/

private:
	// 線分と線分の当たり判定
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// 線分と線分の交点を求める処理
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

	// 当たり判定関数内でデータを受け渡しする際に使用する構造体。
	struct HitData {

		Vec2<float> hitPos;
		INTERSECTED_LINE hitLine;
		Vec2<int> hitChipIndex;

	};


public:
	// マップチップとプレイヤーの当たり判定 絶対に貫通させないバージョン
	INTERSECTED_LINE CheckHitMapChipBasedOnTheVel(Vec2<float>& pos, const Vec2<float>& prevFramePos, const Vec2<float>& vel, const Vec2<float>& size, const vector<vector<int>>& mapChipData, Vec2<int>& hitChipIndex);

	// マップチップとプレイヤーの当たり判定 絶対にめり込ませないバージョン
	INTERSECTED_LINE CheckHitMapChipBasedOnTheScale(Vec2<float>& pos, const Vec2<float>& size, const vector<vector<int>>& mapChipData, const INTERSECTED_LINE& direction, Vec2<int>& hitChipIndex);


};