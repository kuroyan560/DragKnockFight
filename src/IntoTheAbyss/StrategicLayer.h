#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include"../IntoTheAbyss/TacticalLayer.h"
#include"../IntoTheAbyss/BehavioralLayer.h"
#include"../IntoTheAbyss/NavigationAI.h"
#include"../IntoTheAbyss/OperatingLayer.h"
#include"../Engine/DrawFunc.h"
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
	/// デバック用
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 評価値の算出
	/// </summary>
	/// <returns>評価値</returns>
	virtual float EvaluationFunction() = 0;

	WayPointData startPoint, endPoint;
	std::vector<WayPointData> route;
	bool startFlag;

public:

	float GetGaugeStatus();

protected:
	int timer;
	int timeOver;
	//アイテム探索
	struct SearchData
	{
		float distance;
		int itemIndex;
	};
	SearchData SearchItem(const SphereCollision &DATA);
	static const float SEARCH_RADIUS;
	//場所によってアイテム探索範囲を変える用の変数
	SphereCollision searchCollision;
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

	void Draw()override;

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

	bool getFlag;
	int prevItemIndex;
	//アイテム探索--------------------------

	//成否--------------------------
	float staminaGauge;
	static const float SUCCEED_GAIN_STAMINA_VALUE;//どれぐらいの値

};

/// <summary>
/// 自陣に向かう
/// </summary>
class SwingClockWise :public IStrategicLayer
{
public:
	SwingClockWise();

	void Init()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Update()override;

	void Draw()override {};

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction();

	MoveToOwnGround moveToOnwGround;
private:
	bool goToTheFieldFlag;
	OperateSwing operateSwing;
	bool finishFlag;

	int SWING_MAX_COOL_TIME = 30;//振り回しのクールタイム
};



/// <summary>
/// 優勢ゲージを獲得する
/// </summary>
class SwingThreeTimesCounterClockWise :public IStrategicLayer
{
public:
	SwingThreeTimesCounterClockWise();

	void Init()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Update()override;

	void Draw()override {};

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction()override;

private:
	static const float SUCCEED_GAUGE_VALUE;//どこまで回復したら成功とするか
	float nowGauge;							//戦略開始時の優勢ゲージ
	//クラッシュさせる--------------------------
	bool crashEnemyFlag;
	int SWING_MAX_COOL_TIME = 30;//振り回しのクールタイム

	OperateSwing operateSwing;
	MoveToOwnGround moveToOnwGround;

	//クラッシュされないようにする--------------------------
	bool dontCrashFlag;

	int countSwingNum;
	bool swingingFlag;
	bool finishFlag;
};

class Dash :public IStrategicLayer
{
public:
	Dash();

	void Init()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Update()override;

	void Draw()override {};

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction()override;
	MoveToOwnGround moveToOnwGround;
private:
	OperateDash dash;
	bool finishFlag;

};

class SwingClockWiseThreeTimes :public IStrategicLayer
{
public:
	SwingClockWiseThreeTimes();

	void Init()override;
	
	void Update()override;
	
	void Draw()override {};

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction()override;
private:
	//クラッシュさせる--------------------------
	bool crashEnemyFlag;
	int SWING_MAX_COOL_TIME = 30;//振り回しのクールタイム

	OperateSwing operateSwing;
	MoveToOwnGround moveToOnwGround;

	//クラッシュされないようにする--------------------------
	bool dontCrashFlag;

	int countSwingNum;
	bool swingingFlag;
	bool finishFlag;
};

class SwingCounterClockWise :public IStrategicLayer
{
public:
	SwingCounterClockWise();

	void Init()override;

	void Update()override;

	void Draw()override {};

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction()override;
private:
	//クラッシュさせる--------------------------
	bool crashEnemyFlag;
	int SWING_MAX_COOL_TIME = 30;//振り回しのクールタイム

	OperateSwing operateSwing;
	MoveToOwnGround moveToOnwGround;

	//クラッシュされないようにする--------------------------
	bool dontCrashFlag;

	int countSwingNum;
	bool swingingFlag;
	bool finishFlag;
};