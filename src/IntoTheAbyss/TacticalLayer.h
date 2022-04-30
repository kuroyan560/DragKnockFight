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
	FollowPath(const std::shared_ptr<MovingBetweenTwoPoints> &OPERATE);

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
	std::shared_ptr<MovingBetweenTwoPoints> operateFollow;
	std::vector<WayPointData> route;
	Vec2<float>startPos, endPos;
	int routeHandle;

	bool goalFlag;

};