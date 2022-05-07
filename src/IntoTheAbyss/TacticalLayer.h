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
	FollowPath();

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
	MovingBetweenTwoPoints operateFollow;
	MoveToNotStack dontStack;
	std::vector<WayPointData> route;
	Vec2<float>startPos, endPos;
	int routeHandle;

	bool goalFlag;

	bool stackFlag;


	bool initFlag;
};


/// <summary>
/// ���w�܂Ō�����
/// </summary>
class MoveToOwnGround
{
public:
	MoveToOwnGround();

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
	FollowPath followPath;
	SearchWayPoint searchPoint;

	Vec2<int>prevStartHandle, prevEndHandle;
	int timer, timeOver;
};