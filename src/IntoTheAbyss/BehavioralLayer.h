#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/OperatingLayer.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include<memory>
#include"BulletCollision.h"
#include"../IntoTheAbyss/NavigationAI.h"
/// <summary>
/// 振る舞い層
/// </summary>

/// <summary>
/// 二点間を移動
/// </summary>
class MovingBetweenTwoPoints
{
public:
	MovingBetweenTwoPoints();

	void Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS, bool STACK_FLAG = false);

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
/// 現在いる地点から最も近いウェイポイントの検索
/// </summary>
class SearchWayPoint
{
public:
	SearchWayPoint();

	void Init(const Vec2<float> &START_POS);

	/// <summary>
	/// 実行,最も近いウェイポイントのハンドルを返す
	/// </summary>
	const WayPointData &Update();

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();
private:
	std::vector<std::vector<WayPointData>> wayPoints;
	Vec2<float>startPos;
	
};

/// <summary>
/// スタックしない場所を探す
/// </summary>
class SearchingToNotGetStuck
{
public:
	SearchingToNotGetStuck();

	const WayPointData &Update(const Vec2<float> START_POS);

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();
private:
	SearchWayPoint search;
	std::vector<std::vector<WayPointData>> wayPoints;
};

/// <summary>
/// スタックしない場所に向かう
/// </summary>
class MoveToNotStack
{
public:
	MoveToNotStack();

	/// <summary>
	/// スタート地点を設定し、スタックしない場所まで移動する
	/// </summary>
	/// <param name="START_POS">スタート地点</param>
	void Init(const Vec2<float> &START_POS);

	void Update();

	/// <summary>
	/// 現在実行している処理の進捗
	/// </summary>
	/// <returns>FAIL...失敗,INPROCESS...実行中,SUCCESS...成功</returns>
	AiResult CurrentProgress();

private:
	SearchingToNotGetStuck searchToNotStack;
	MovingBetweenTwoPoints operateFollow;
};