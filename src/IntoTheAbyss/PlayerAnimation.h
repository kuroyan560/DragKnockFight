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
	int status;

	std::vector<Anim>animations;

	int idx;	//�摜�C���f�b�N�X
	float timer;	//���Ԍv��

public:
	PlayerAnimation(const std::vector<Anim>& Anims);
	void Init(const int& AnimStatus)
	{
		status = AnimStatus;
		timer = 0;
		idx = 0;
	}

	void Update();

	void ChangeAnim(const int& AnimStatus)
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

	Vec2<int>GetGraphSize();
	
	const int& GetNowAnim() { return status; }

	//�ǂꂩ�ЂƂł����Ă͂܂�����true��Ԃ�
	bool Compare(const std::vector<int>& Status);
};

