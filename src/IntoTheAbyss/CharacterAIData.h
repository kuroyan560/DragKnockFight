#pragma once
#include<memory>
#include"../Common/Singleton.h"
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
	bool nearByWallFlag;//�ǂƋ߂����ǂ���
	int staminaMaxValue;//���݂̃X�^�~�i��
	int staminaValue;	//�ő�̃X�^�~�i��
	int gaugeMaxValue;	//���݂̃Q�[�W��
	int gaugeValue;		//�ő�̃Q�[�W��
	int positionGauge;	//���݂̈ʒu

};

/// <summary>
/// AI�����s����̂ɔ��f�ɕK�v�ȃp�����[�^�[�Q
/// </summary>
class CharacterAIData :public Singleton<CharacterAIData>
{
public:
	CommonParameter playerData, bossData;
	float distance;	//�v���C���[�ƓG�Ƃ̋���
	float position;	//���ݒn
};
