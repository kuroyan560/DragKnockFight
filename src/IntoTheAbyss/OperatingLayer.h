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
	/// <summary>
	/// ��������ہA���W�ƈړ��ʂ�n��
	/// </summary>
	/// <param name="POS">�Ώۂ̃L�����N�^�[�̍��W</param>
	/// <param name="VELOCITY">�ړ���</param>
	OperateMove(const std::shared_ptr<Vec2<float>> &POS);

	/// <summary>
	/// ���s
	/// </summary>
	void Update(const Vec2<float> &VELOCITY);

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

	std::shared_ptr<Vec2<float>>pos;
	Vec2<float> vel;
private:

	Vec2<float>oldPos;

};