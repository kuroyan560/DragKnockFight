#pragma once
#include"Singleton.h"
#include<vector>
#include"CharacterInterFace.h"
static const enum FACE_STATUS { DEFAULT, DAMAGE, BREAK, DEAD, FACE_STATUS_NUM };

class FaceIcon : public Singleton<FaceIcon>
{
	friend class Singleton<FaceIcon>;
	FaceIcon();


	//アニメーション情報
	struct Anim
	{
		std::vector<int>graph;
		int interval;
	};

	Anim animasions[PLAYABLE_CHARACTER_NUM][FACE_STATUS_NUM];

	//アニメーションのための時間計測
	int timer[TEAM_NUM];
	int graphIdx[TEAM_NUM];

	//顔背景
	int backGraph;

	//状態
	FACE_STATUS status[TEAM_NUM];

	//キャラクター
	PLAYABLE_CHARACTER_NAME character[TEAM_NUM];

public:
	void Init(const PLAYABLE_CHARACTER_NAME& Left, const PLAYABLE_CHARACTER_NAME& Right);
	void Update();
	void Draw();

	void Change(const WHICH_TEAM& WhichTeam, const FACE_STATUS& Status);
};