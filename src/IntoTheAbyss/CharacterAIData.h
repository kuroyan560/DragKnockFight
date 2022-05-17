#pragma once
#include<memory>
#include"../Common/Singleton.h"
#include"NavigationAI.h"
/// <summary>
/// �e�S�[���̐i��
/// </summary>
enum class AiResult :int
{
	OPERATE_FAIL = -1,
	OPERATE_INPROCESS,
	OPERATE_SUCCESS
};

struct CommonParameter
{
	float stamineGauge;	//�X�^�~�i�̊���
	float gaugeValue;	//�D���Q�[�W�̊���
	int swingStamina;	//�U��񂵎��̃X�^�~�i����
	int dashStamina;	//�_�b�V�����̃X�^�~�i����
};

/// <summary>
/// AI�����s����̂ɔ��f�ɕK�v�ȃp�����[�^�[�Q
/// </summary>
class CharacterAIData :public Singleton<CharacterAIData>
{
public:
	CommonParameter playerData, bossData;
	std::vector<std::vector<WayPointData>> wayPoints;
	float distance;	//�v���C���[�ƓG�Ƃ̋���
	float position;	//���ݒn

	bool swingFlag;//�U��񂵓���
	bool dashFlag;//�_�b�V������
	int dashCount;
	int dashTimer;

	float swingClockwiseDistance;
	float swingCounterClockwiseDistance;

	const int EVALUATION_MAX_VALUE = 10;

	Vec2<float>nowPos,prevPos;

	bool releaseSwingFlag;
	bool prevSwingFlag;
};

class CharacterAIOrder :public Singleton<CharacterAIOrder>
{
public:
	Vec2<float> vel;//�ړ���
	bool dashFlag;
	bool swingClockWiseFlag;
	bool swingCounterClockWiseFlag;
	bool startAiFlag;
};
