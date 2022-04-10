#pragma once
#include"Singleton.h"
#include<vector>

static const enum WHICH_FACE { LEFT_FACE, RIGHT_FACE, FACE_NUM };
static const enum FACE_STATUS { DEFAULT, DAMAGE, BREAK, DEAD, FACE_STATUS_NUM };
static const enum CHARACTER { LUNA, CHARACTER_NUM, DEFAULT_CHARA = LUNA };

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

	Anim animasions[CHARACTER_NUM][FACE_STATUS_NUM];

	//アニメーションのための時間計測
	int timer[FACE_NUM];
	int graphIdx[FACE_NUM];

	//顔背景
	int backGraph;

	//状態
	FACE_STATUS status[FACE_NUM];

	//キャラクター
	CHARACTER character[FACE_NUM];

public:
	void Init(const CHARACTER& Left, const CHARACTER& Right);
	void Init()
	{
		Init(LUNA, LUNA);
	}
	void Update();
	void Draw();

	void Change(const WHICH_FACE& Which, const FACE_STATUS& Status);
};