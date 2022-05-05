#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include"../IntoTheAbyss/TacticalLayer.h"
#include"../IntoTheAbyss/BehavioralLayer.h"
#include"../IntoTheAbyss/NavigationAI.h"
/// <summary>
/// 戦術層
/// </summary>


/// <summary>
/// スタミナの回復
/// </summary>
class RestoreStamina
{
public:
	RestoreStamina(
		const std::shared_ptr<FollowPath> &FOLLOW_PATH,
		const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS,
		const const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS
	);

	void Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS);

	/// <summary>
	/// 実行
	/// </summary>
	void Update();

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();


	float EvaluationFunction();


	Vec2<float>startPos, endPos;
	Vec2<int>startHandle, endHandle;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData> route;
private:
	std::shared_ptr<FollowPath> followPath;
	std::unique_ptr<SearchWayPoint> searchStartPoint, searchGoalPoint;
	std::unique_ptr<MoveToOwnGround> moveToOnwGround;

	bool readyToFollowPathFlag;
	bool readyToGoToGoalFlag;

	std::vector<Vec2<float>>itemList;
	float serachItemRadius;

	bool initRouteFlag;


	SphereCollision searchArea;
	int searchItemIndex;
	bool seachItemFlag;
	bool initFlag;
	Vec2<int>prevStartHandle;
	static const float SEARCH_RADIUS;

	bool getFlag;
	int prevItemIndex;
	struct SearchData
	{
		float distance;
		int itemIndex;
	};
	RestoreStamina::SearchData SearchItem(const SphereCollision &DATA);
};

/// <summary>
/// 自陣に向かう
/// </summary>
class GoToTheField
{
public:
	GoToTheField();

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
};



/// <summary>
/// 優勢ゲージを獲得する
/// </summary>
class AcquireASuperiorityGauge
{
public:
	AcquireASuperiorityGauge();

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
};