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

	std::array<std::unique_ptr<IStrategicLayer>, 3> strategyArray;

	bool initFlag = false;
	bool staminaInit = false;

	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData>route;
	Vec2<float>vel;

	AiStrategy strategyOfChoice,prevStrategyOfChoice;
};
