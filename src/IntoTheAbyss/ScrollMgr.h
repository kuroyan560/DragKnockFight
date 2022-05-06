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
	ScrollMgr() :zoom(1.0f)
	{};
	void Init(const Vec2<float> POS, const Vec2<float>& MAP_MAX_SIZE, const Vec2<float>& ADJ);
	void Update(const Vec2<float>& LineCenterPos);
	void CalucurateScroll(const Vec2<float>& VEL, const Vec2<float>& PLAYER_POS);
	Vec2<float>Affect(const Vec2<float>& Pos);//スクロールとズームを適用させるk

	void Warp(const Vec2<float> POS);
	void Reset();

	Vec2<float> honraiScrollAmount;	//本来スクロール量
	Vec2<float> scrollAmount;		//スクロール量
	float zoom;						//ズーム倍率

	Vec2<float> lineCenterOffset;	// 紐の中心を画面の中心に持ってくるためのオフセット 中心からずれていたらこの値を使って中心に持ってくる。

private:

	Vec2<int>windowSize;
	Vec2<float>windowHalfSize;
	Vec2<float>adjLine;
	Vec2<float> mapSize;
	bool warpFlag;

	Vec2<float>resetAmount;
public:
	Vec2<float> CaluStartScrollLine(const Vec2<float>& SIZE)
	{
		Vec2<float> distance(0.0f, 0.0f);
		Vec2<float> startPos;

		startPos.x = (distance.x + SIZE.x);
		startPos.y = (distance.y + SIZE.y);
		return startPos;
	};


	Vec2<float> CaluEndScrollLine(const Vec2<float>& SIZE)
	{
		Vec2<float> distance(mapSize.x, mapSize.y);
		Vec2<float> endPos;

		endPos.x = (distance.x - SIZE.x);
		endPos.y = (distance.y - SIZE.y);
		return endPos;
	};
};