#pragma once
#include<vector>
#include"Vec.h"

static const enum PLAYER_ANIM
{
	DEFAULT_FRONT,
	DEFAULT_BACK,
	PLAYER_ANIM_NUM
};

class PlayerAnimation
{
	PLAYER_ANIM status = DEFAULT_FRONT;
	struct Anim
	{
		std::vector<int> graph;
		int interval;
		Vec2<int>size;
		bool loop;
		Vec2<float>handCenterOffset = { 0,0 };
	};
	Anim animations[PLAYER_ANIM_NUM];

	int idx;	//画像インデックス
	int timer;	//時間計測

public:
	PlayerAnimation();
	void Init(const PLAYER_ANIM& AnimStatus = DEFAULT_FRONT)
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
	
	const Vec2<float>GetHandCenterOffset()&
	{
		return animations[status].handCenterOffset;
	}
};

