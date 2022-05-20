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

	void Init();

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

	int stanTimer;
};


//�_�b�V��
class OperateDash
{
public:
	OperateDash();

	//�_�b�V�����J�n����
	void Init(const Vec2<float> &DASH_SPEED);

	//���̃X�s�[�h����ǂꂭ�炢�_�b�V�����邩
	const Vec2<float> &Update();
private:
	float rate;
	Vec2<float> dashSpeed;
	float timer;
};


/// <summary>
/// �U��񂵂̖���
/// </summary>
class OperateSwing
{
public:
	OperateSwing();

	void Init(int SWING_COOL_TIME);

	/// <summary>
	/// ���v���ɐU���
	/// </summary>
	AiResult SwingClockWise();

	/// <summary>
	/// �����v���ɐU���
	/// </summary>
	AiResult SwingCounterClockWise();

	/// <summary>
	/// �G�����ړ��ł��钷���ŐU���
	/// </summary>
	AiResult SwingLongDisntnce();

	void Update();

private:
	int swingTimer;
	int swingCoolTime;
	int prevSwingTimer;
	int prevSwingCoolTimer;
	bool enableToSwingFlag;
};