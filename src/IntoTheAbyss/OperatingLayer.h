#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include<memory>
/// <summary>
/// ����w�̃N���X��Z�߂���
/// </summary>

/// <summary>
/// �ړ����߂��o���N���X
/// </summary>
class OperateMove
{
public:
	OperateMove();

	/// <summary>
	/// ���s
	/// </summary>
	void Update(const Vec2<float> &VELOCITY);

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();
private:
	bool initFlag;
	Vec2<float>oldPos;
};