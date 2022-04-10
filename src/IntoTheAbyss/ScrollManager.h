#pragma once
#include "Vec.h"
#include "Singleton.h"
#include<vector>
#include<array>
#include"ShakeMgr.h"
#include"Camera.h"
#include"KuroMath.h"

class ScrollManager : public Singleton<ScrollManager>
{
public:
	ScrollManager() :zoom(1.0f)
	{};
	void Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE, const Vec2<float>&ADJ);
	void Update();
	void CalucurateScroll(const Vec2<float> &VEL, const Vec2<float> &PLAYER_POS);
	Vec2<float>Affect(const Vec2<float> &Pos);//�X�N���[���ƃY�[����K�p������

	void Warp(const Vec2<float> POS);

	Vec2<float> honraiScrollAmount;	//�{���X�N���[����
	Vec2<float> scrollAmount;		//�X�N���[����
	float zoom;						//�Y�[���{��
private:

	Vec2<int>windowSize;
	Vec2<float>windowHalfSize;
	Vec2<float>adjLine;
	Vec2<float> mapSize;
	bool initFlag;

	Vec2<float> CaluStartScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(0.0f, 0.0f);
		Vec2<float> startPos;

		startPos.x = (distance.x + SIZE.x);
		startPos.y = (distance.y + SIZE.y);
		return startPos;
	};


	Vec2<float> CaluEndScrollLine(const Vec2<float> &SIZE)
	{
		Vec2<float> distance(mapSize.x, mapSize.y);
		Vec2<float> endPos;

		endPos.x = (distance.x - SIZE.x);
		endPos.y = (distance.y - SIZE.y);
		return endPos;
	};
};

