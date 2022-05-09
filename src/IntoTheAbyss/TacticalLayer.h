#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/NavigationAI.h"
#include"../IntoTheAbyss/BehavioralLayer.h"
#include<memory>
/// <summary>
/// 戦術層
/// </summary>

/// <summary>
/// パスをフォローする
/// </summary>
class FollowPath
{
public:
	FollowPath();

	void Init(const std::vector<WayPointData> &ROUTE);

	/// <summary>
	/// 実行
	/// </summary>
	void Update();

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
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
/// 自陣まで向かう
/// </summary>
class MoveToOwnGround
{
public:
	MoveToOwnGround();

	void Init();

	/// <summary>
	/// 実行
	/// </summary>
	void Update();

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();

	WayPointData startPoint, endPoint;
	std::vector<WayPointData>route;
private:
	FollowPath followPath;
	SearchWayPoint searchPoint;

	Vec2<int>prevStartHandle, prevEndHandle;
	int timer, timeOver;
};