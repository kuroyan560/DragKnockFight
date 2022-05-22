#pragma once
#include<memory>
#include"../Common/Singleton.h"
#include"NavigationAI.h"
/// <summary>
/// 各ゴールの進捗
/// </summary>
enum class AiResult :int
{
	OPERATE_FAIL = -1,
	OPERATE_INPROCESS,
	OPERATE_SUCCESS
};

struct CommonParameter
{
	float stamineGauge;	//スタミナの割合
	float gaugeValue;	//優勢ゲージの割合
	int swingStamina;	//振り回し時のスタミナ消費
	int dashStamina;	//ダッシュ時のスタミナ消費
};

/// <summary>
/// AIを実行するのに判断に必要なパラメーター群
/// </summary>
class CharacterAIData :public Singleton<CharacterAIData>
{
public:
	CommonParameter playerData, bossData;
	std::vector<std::vector<WayPointData>> wayPoints;
	float distance;	//プレイヤーと敵との距離
	float position;	//現在地

	bool swingFlag;//振り回し入力
	bool dashFlag;//ダッシュ入力
	int dashCount;
	int dashTimer;

	float swingClockwiseDistance;
	float swingCounterClockwiseDistance;

	const int EVALUATION_MAX_VALUE = 10;

	Vec2<float>nowPos,prevPos;

	bool releaseSwingFlag;
	bool prevSwingFlag;

	int crashCount;
};

class CharacterAIOrder :public Singleton<CharacterAIOrder>
{
public:
	Vec2<float> vel;//移動量
	bool dashFlag;
	bool swingClockWiseFlag;
	bool swingCounterClockWiseFlag;
	bool swingThreeTimesFlag;
	bool stopFlag;

	bool startAiFlag;
	bool prevSwingFlag;//予備動作 

	float prevRate;

	void Init()
	{
		dashFlag = false;
		swingClockWiseFlag = false;
		swingCounterClockWiseFlag = false;
		startAiFlag = false;
		prevSwingFlag = false;
		stopFlag = false;
		prevRate = 0.0f;
	};
};
