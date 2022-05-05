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
	/// <summary>
	/// 生成する際、二点間の座標を渡す
	/// </summary>
	/// <param name="POS">対象のキャラクターの座標</param>
	/// <param name="VELOCITY">移動量</param>
	MovingBetweenTwoPoints(const std::shared_ptr<OperateMove> &OPERATION);

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
/// 現在いる地点から最も近いウェイポイントの検索
/// </summary>
class SearchWayPoint
{
public:
	SearchWayPoint(const std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> &WAY_POINTS);

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
	std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> wayPoints;
	Vec2<float>startPos;
	
};