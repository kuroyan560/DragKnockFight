#pragma once
#include"../KuroEngine.h"
#include"StrategicLayer.h"
#include"TacticalLayer.h"
#include"BehavioralLayer.h"
#include"OperatingLayer.h"
#include<memory>
#include"BehaviorPredection.h"

enum AiStrategy
{
	STRATEGY_NONE = -1,
	//STRATEGY_RESTORE_STAMINA,
	STRATEGY_SWING_DASH,
	STRATEGY_SWING_CLOCKWISE,
	STRATEGY_SWING_COUNTERCLOCKWISE,
	STRATEGY_SWING_3TIMES_CLOCKWISE,
	STRATEGY_SWING_3TIMES_COUNTERCLOCKWISE,
	STRATEGY_MAX
};

/// <summary>
/// キャラクターAIの意思決定
/// 階層型ゴール指向プランニングに基づいて設計する
/// </summary>
class CharacterAI
{
public:
	CharacterAI();

	void Init();
	void Finalize();
	void Update();
	void Draw();

	void UpdateGauge();

	std::vector<WayPointData> shortestData;

	std::array<std::unique_ptr<IStrategicLayer>, STRATEGY_MAX> strategyArray;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData>route;
	Vec2<float>vel;

	int strategyOfChoice,prevStrategyOfChoice;

	int startTimer;
	bool initFlag;
	bool useAiFlag;


	bool startDashFlag;

	std::unique_ptr<BehaviorPredection> behaviorGauge;
};
