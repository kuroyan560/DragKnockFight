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
	RestoreStamina();

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


	float EvaluationFunction();


	Vec2<float>startPos, endPos;
	Vec2<int>startHandle,endHandle;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData> route;
private:
	FollowPath followPath;
	SearchWayPoint searchStartPoint,searchGoalPoint;
	MoveToOwnGround moveToOnwGround;

	std::vector<Vec2<float>>itemList;
	bool initRouteFlag;



	//アイテム探索--------------------------
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
	//アイテム探索--------------------------

	//成否--------------------------
	int staminaGauge;
	int timer;
	int timeOver;
	static const int SUCCEED_GAIN_STAMINA_VALUE;//どれぐらいの値

};

/// <summary>
/// 自陣に向かう
/// </summary>
class GoToTheField
{
public:
	GoToTheField();

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
	bool startFlag;
	std::vector<WayPointData>route;
	MoveToOwnGround moveToOnwGround;
private:
	int timer;
	int timeOver;

	bool goToTheFieldFlag;

};



/// <summary>
/// 優勢ゲージを獲得する
/// </summary>
class AcquireASuperiorityGauge
{
public:
	AcquireASuperiorityGauge();

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

private:
	static const float SUCCEED_GAUGE_VALUE;//どこまで回復したら成功とするか
	float nowGauge;//戦略開始時の優勢ゲージ
	int timer;
	int timeOver;

	//クラッシュさせる--------------------------
	bool crashEnemyFlag;



	//クラッシュされないようにする--------------------------
	bool dontCrashFlag;
};