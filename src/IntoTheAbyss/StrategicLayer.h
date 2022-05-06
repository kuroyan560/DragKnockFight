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
/// 戦略層の抽象クラス
/// </summary>
class IStrategicLayer
{
public:
	~IStrategicLayer() {};

	virtual void Init() = 0;

	/// <summary>
	/// 実行
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	virtual AiResult CurrentProgress() = 0;

	/// <summary>
	/// 評価値の算出
	/// </summary>
	/// <returns>評価値</returns>
	virtual float EvaluationFunction() = 0;

	WayPointData startPoint, endPoint;
	std::vector<WayPointData> route;
	bool startFlag;

protected:
	int timer;
	int timeOver;
};




/// <summary>
/// スタミナの回復
/// </summary>
class RestoreStamina :public IStrategicLayer
{
public:
	RestoreStamina();

	void Init()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Update()override;

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;


	float EvaluationFunction()override;


private:
	FollowPath followPath;
	SearchWayPoint searchStartPoint, searchGoalPoint;
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
	static const int SUCCEED_GAIN_STAMINA_VALUE;//どれぐらいの値

};

/// <summary>
/// 自陣に向かう
/// </summary>
class GoToTheField :public IStrategicLayer
{
public:
	GoToTheField();

	void Init()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Update()override;

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction();

	MoveToOwnGround moveToOnwGround;
private:
	bool goToTheFieldFlag;

};



/// <summary>
/// 優勢ゲージを獲得する
/// </summary>
class AcquireASuperiorityGauge :public IStrategicLayer
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

	float EvaluationFunction();

private:
	static const float SUCCEED_GAUGE_VALUE;//どこまで回復したら成功とするか
	float nowGauge;//戦略開始時の優勢ゲージ
	//クラッシュさせる--------------------------
	bool crashEnemyFlag;



	//クラッシュされないようにする--------------------------
	bool dontCrashFlag;
};