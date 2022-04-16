#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "Intersected.h"

// 当たり判定クラス
class Collider : public Singleton<Collider> {

public:

	/*===== メンバ関数 =====*/

	// オブジェクトとオブジェクトの移動量線分を使った当たり判定。
	INTERSECTED_LINE CheckHitVel(Vec2<float>& pushBackPosA, const Vec2<float> prevFramePosA, const Vec2<float>& velA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB);

	// オブジェクトとオブジェクトのサイズを使った当たり判定。
	INTERSECTED_LINE CheckHitSize(Vec2<float>& posA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB, const INTERSECTED_LINE& dir);


public:

	// 線分と線分の当たり判定
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// 線分と線分の交点を求める処理
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

};