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


	//�A�j���[�V�������
	struct Anim
	{
		std::vector<int>graph;
		int interval;
	};

	Anim animasions[CHARACTER_NUM][FACE_STATUS_NUM];

	//�A�j���[�V�����̂��߂̎��Ԍv��
	int timer[FACE_NUM];
	int graphIdx[FACE_NUM];

	//��w�i
	int backGraph;

	//���
	FACE_STATUS status[FACE_NUM];

	//�L�����N�^�[
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