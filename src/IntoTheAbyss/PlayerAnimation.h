#pragma once
#include<vector>
#include"Vec.h"
#include"CharacterInterFace.h"

static const enum PLAYER_ANIM
{
	DEFAULT_FRONT,
	DEFAULT_BACK,
	PULL_FRONT,
	PULL_BACK,
	HOLD,
	SWINGED,
	TIRED,
	KNOCK_OUT,
	NON_PILOT,
	PLAYER_ANIM_NUM
};

class PlayerAnimation
{
	PLAYER_ANIM status = DEFAULT_FRONT;
	struct Anim
	{
		std::vector<int> graph;
		int interval;
		bool loop;
		Vec2<float>handCenterOffset = { 0,0 };
	};
	Anim animations[PLAYER_ANIM_NUM];

	int idx;	//�摜�C���f�b�N�X
	float timer;	//���Ԍv��

public:
	PlayerAnimation(const PLAYABLE_CHARACTER_NAME& CharacterName);
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
		if (status == KNOCK_OUT)return;
		status = AnimStatus;
		timer = 0;
		idx = 0;
	}

	const int& GetGraphHandle()
	{
		return animations[status].graph[idx];
	}

	Vec2<int>GetGraphSize();
	
	const Vec2<float>GetHandCenterOffset()&
	{
		return animations[status].handCenterOffset;
	}

	const PLAYER_ANIM& GetNowAnim() { return status; }

	//�ǂꂩ�ЂƂł����Ă͂܂�����true��Ԃ�
	bool Compare(const std::vector<PLAYER_ANIM>& Status);
};

