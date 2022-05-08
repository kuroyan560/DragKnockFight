#pragma once
#include"../KuroEngine.h"
#include"StrategicLayer.h"
#include"TacticalLayer.h"
#include"BehavioralLayer.h"
#include"OperatingLayer.h"
#include<memory>

enum AiStrategy
{
	STRATEGY_NONE = -1,
	STRATEGY_RESTORE_STAMINA,
	STRATEGY_GO_TO_THE_FIELD,
	STRATEGY_ACQUIRE_A_SUPERIORITY_GAUGE,
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
	void Update();
	void Draw();

	std::vector<WayPointData> shortestData;

	std::array<std::unique_ptr<IStrategicLayer>, STRATEGY_MAX> strategyArray;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData>route;
	Vec2<float>vel;

	int strategyOfChoice,prevStrategyOfChoice;

	int startTimer;
	bool initFlag;
};
