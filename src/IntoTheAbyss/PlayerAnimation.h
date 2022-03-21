#pragma once
#include<vector>
#include"Vec.h"

static const enum PLAYER_ANIM
{
	ON_GROUND_WAIT,
	ON_GROUND_DASH,
	ON_AIR_DASH_X,
	ON_AIR_DASH_Y,
	ON_WALL_WAIT,
	ON_WALL_DASH,
	PLAYER_ANIM_NUM
};

class PlayerAnimation
{
	PLAYER_ANIM status = ON_GROUND_WAIT;
	struct Anim
	{
		std::vector<int> graph;
		int interval;
		Vec2<int>size;
		bool loop;
		Vec2<float>handOffsetLeft = { 0,0 };
		Vec2<float>handOffsetRight = { 0,0 };
	};
	Anim animations[PLAYER_ANIM_NUM];

	int idx;	//画像インデックス
	int timer;	//時間計測

public:
	PlayerAnimation();
	void Init(const PLAYER_ANIM& AnimStatus = ON_GROUND_WAIT)
	{
		status = AnimStatus;
		timer = 0;
		idx = 0;
	}

	void Update();

	void ChangeAnim(const PLAYER_ANIM& AnimStatus)
	{
		if (status == AnimStatus)return;
		status = AnimStatus;
		timer = 0;
		idx = 0;
	}

	const int& GetGraphHandle()
	{
		return animations[status].graph[idx];
	}

	const Vec2<int>GetGraphSize()&
	{
		return animations[status].size;
	}
	
	const Vec2<float>GetHandOffsetLeft()&
	{
		return animations[status].handOffsetLeft;
	}
	const Vec2<float>GetHandOffsetRight()&
	{
		return animations[status].handOffsetRight;
	}
};

