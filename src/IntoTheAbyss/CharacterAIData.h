#pragma once
#include<memory>
#include"../Common/Singleton.h"
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
	bool nearByWallFlag;//壁と近いかどうか
	int staminaMaxValue;//現在のスタミナ量
	int staminaValue;	//最大のスタミナ量
	int gaugeMaxValue;	//現在のゲージ量
	int gaugeValue;		//最大のゲージ量
	int positionGauge;	//現在の位置

};

/// <summary>
/// AIを実行するのに判断に必要なパラメーター群
/// </summary>
class CharacterAIData :public Singleton<CharacterAIData>
{
public:
	CommonParameter playerData, bossData;
	float distance;	//プレイヤーと敵との距離
	float position;	//現在地
};
