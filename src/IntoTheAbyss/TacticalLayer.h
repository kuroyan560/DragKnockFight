#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/NavigationAI.h"
#include"../IntoTheAbyss/BehavioralLayer.h"
#include<memory>
/// <summary>
/// ��p�w
/// </summary>

/// <summary>
/// �p�X���t�H���[����
/// </summary>
class FollowPath
{
public:
	FollowPath(const std::shared_ptr<MovingBetweenTwoPoints> &OPERATE);

	void Init(const std::vector<WayPointData> &ROUTE);

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
	std::shared_ptr<MovingBetweenTwoPoints> operateFollow;
	std::vector<WayPointData> route;
	Vec2<float>startPos, endPos;
	int routeHandle;

	bool goalFlag;

};


/// <summary>
/// ���w�܂Ō�����
/// </summary>
class MoveToOwnGround
{
public:
	MoveToOwnGround(const std::shared_ptr<FollowPath> &OPERATE);

	void Init();

	/// <summary>
	/// ���s
	/// </summary>
	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

	WayPointData startPoint, endPoint;
	std::vector<WayPointData>route;
private:
	std::shared_ptr<FollowPath>followPath;
	std::shared_ptr<SearchWayPoint> searchPoint;

	Vec2<int>prevStartHandle, prevEndHandle;
	int timer, timeOver;
};