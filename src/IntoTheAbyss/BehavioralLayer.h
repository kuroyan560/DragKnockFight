#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/OperatingLayer.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include<memory>
#include"BulletCollision.h"
#include"../IntoTheAbyss/NavigationAI.h"
/// <summary>
/// �U�镑���w
/// </summary>

/// <summary>
/// ��_�Ԃ��ړ�
/// </summary>
class MovingBetweenTwoPoints
{
public:
	MovingBetweenTwoPoints();

	void Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS, bool STACK_FLAG = false);

	/// <summary>
	/// ���s
	/// </summary>
	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

private:
	OperateMove operateMove;
	OperateDash operateDash;
	bool dashFlag;
	int dashTimer;
	Vec2<float>startPos;
	Vec2<float>endPos;
	Vec2<float>vel;
	int timer;
	int timeOver;

	SphereCollision startColision,endColision;
	bool initFlag;
};


/// <summary>
/// ���݂���n�_����ł��߂��E�F�C�|�C���g�̌���
/// </summary>
class SearchWayPoint
{
public:
	SearchWayPoint();

	void Init(const Vec2<float> &START_POS);

	/// <summary>
	/// ���s,�ł��߂��E�F�C�|�C���g�̃n���h����Ԃ�
	/// </summary>
	const WayPointData &Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();
private:
	std::vector<std::vector<WayPointData>> wayPoints;
	Vec2<float>startPos;
	
};

/// <summary>
/// �X�^�b�N���Ȃ��ꏊ��T��
/// </summary>
class SearchingToNotGetStuck
{
public:
	SearchingToNotGetStuck();

	const WayPointData &Update(const Vec2<float> START_POS);

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();
private:
	SearchWayPoint search;
	std::vector<std::vector<WayPointData>> wayPoints;
};

/// <summary>
/// �X�^�b�N���Ȃ��ꏊ�Ɍ�����
/// </summary>
class MoveToNotStack
{
public:
	MoveToNotStack();

	/// <summary>
	/// �X�^�[�g�n�_��ݒ肵�A�X�^�b�N���Ȃ��ꏊ�܂ňړ�����
	/// </summary>
	/// <param name="START_POS">�X�^�[�g�n�_</param>
	void Init(const Vec2<float> &START_POS);

	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

private:
	SearchingToNotGetStuck searchToNotStack;
	MovingBetweenTwoPoints operateFollow;
};