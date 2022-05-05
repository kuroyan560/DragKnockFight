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
	/// <summary>
	/// ��������ہA��_�Ԃ̍��W��n��
	/// </summary>
	/// <param name="POS">�Ώۂ̃L�����N�^�[�̍��W</param>
	/// <param name="VELOCITY">�ړ���</param>
	MovingBetweenTwoPoints(const std::shared_ptr<OperateMove> &OPERATION);

	void Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS);

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
	std::shared_ptr<OperateMove> operateMove;
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
	SearchWayPoint(const std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> &WAY_POINTS);

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
	std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> wayPoints;
	Vec2<float>startPos;
	
};