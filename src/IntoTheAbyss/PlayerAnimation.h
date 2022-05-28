#pragma once
#include<vector>
#include"Vec.h"

struct Anim
{
	std::vector<int> graph;
	int interval;
	bool loop;
};

class PlayerAnimation
{
	int status = 0;

	std::vector<Anim>animations;

	int idx = 0;	//画像インデックス
	float timer = 0;	//時間計測

public:
	PlayerAnimation(const std::vector<Anim>& Anims);
	void Init(const int& AnimStatus)
	{
		status = AnimStatus;
		timer = 0;
		idx = 0;
	}

	void Update();

	void ChangeAnim(const int& AnimStatus, const bool& AnimReset = false)
	{
		if (status == AnimStatus)
		{
			if (AnimReset)
			{
				timer = 0;
				idx = 0;
			}
			return;
		}
		timer = 0;
		idx = 0;
		status = AnimStatus;
	}

	const int& GetGraphHandle()
	{
		return animations[status].graph[idx];
	}

	Vec2<int>GetGraphSize();
	
	const int& GetNowAnim() { return status; }

	//どれかひとつでも当てはまったらtrueを返す
	bool Compare(const std::vector<int>& Status);

	const bool& FinishNowAnim() { return animations[status].graph.size() - 1 == idx; }
};

