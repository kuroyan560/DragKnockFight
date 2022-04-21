#pragma once
#include"Singleton.h"
#include<vector>
#include"CharacterInterFace.h"
static const enum FACE_STATUS { DEFAULT, DAMAGE, BREAK, DEAD, FACE_STATUS_NUM };

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

	Anim animasions[PLAYABLE_CHARACTER_NUM][FACE_STATUS_NUM];

	//�A�j���[�V�����̂��߂̎��Ԍv��
	int timer[TEAM_NUM];
	int graphIdx[TEAM_NUM];

	//��w�i
	int backGraph;

	//���
	FACE_STATUS status[TEAM_NUM];

	//�L�����N�^�[
	PLAYABLE_CHARACTER_NAME character[TEAM_NUM];

public:
	void Init(const PLAYABLE_CHARACTER_NAME& Left, const PLAYABLE_CHARACTER_NAME& Right);
	void Update();
	void Draw();

	void Change(const WHICH_TEAM& WhichTeam, const FACE_STATUS& Status);
};