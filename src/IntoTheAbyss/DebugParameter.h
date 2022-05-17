#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include<vector>
#include"DebugImGuiManager.h"

struct PlayerDebugParameterData
{
	float ADD_GRAVITY;				// プレイヤーにかける重力
	float MAX_GRAVITY;			// プレイヤーにかける重力の最大量
	float RECOIL_AMOUNT;			// 弾を撃った際の反動
	float FIRST_RECOIL_AMOUNT;		// 弾を撃った際の反動
	float MAX_RECOIL_AMOUNT;		// 弾を撃った際の反動の最大値
	int RAPID_FIRE_TIMER;			// 連射タイマー

	PlayerDebugParameterData(const float &ADD_GRAVITY, const float &MAX_GRAVITY, const float &RECOIL_AMOUNT,
		const float &FIRST_RECOIL_AMOUNT, const float &MAX_RECOIL_AMOUNT, const float &RAPID_FIRE_TIMER) :
		ADD_GRAVITY(ADD_GRAVITY), MAX_GRAVITY(MAX_GRAVITY), RECOIL_AMOUNT(RECOIL_AMOUNT), FIRST_RECOIL_AMOUNT(FIRST_RECOIL_AMOUNT),
		MAX_RECOIL_AMOUNT(MAX_RECOIL_AMOUNT), RAPID_FIRE_TIMER(RAPID_FIRE_TIMER)
	{
	}
};


struct GaugeDebugParameterData
{
	float playerBulletAddGuaugeValue;
	float enemyBulletAddGuaugeValue;
	float playerClashDamageValue;
	float enemyClashDamageValue;
	float swingDamageValue;

	GaugeDebugParameterData() :playerBulletAddGuaugeValue(4.0f), enemyBulletAddGuaugeValue(4.0f),
		playerClashDamageValue(10.0f), enemyClashDamageValue(10.0f),
		swingDamageValue(5.0f)
	{
	}
};

struct RoundDebugData
{
	float maxTimer;
	float maskMaxTimer;
	float sizeMaxTimer;
	RoundDebugData() :maxTimer(120.0f), maskMaxTimer(120.0f), sizeMaxTimer(120.0f)
	{
	}
};

struct RoundParameterData
{
	RoundDebugData roundData, readyData, fightData, numberData, nextNumberData;

	RoundParameterData()
	{

	}
};


struct BossDebugParameterData
{
	float vel;
	int coolTime;
	int staminaMax;
	int staminaDash;
	int staminaSwing;
	float swingAngle;
	float swingMax;
	bool enableToDashAfterSwingFlag;
	BossDebugParameterData() :
		vel(14.0f), coolTime(30), staminaMax(6), staminaDash(1), staminaSwing(2), enableToDashAfterSwingFlag(true),
		swingAngle(0.02f), swingMax(0.13f)
	{
	}
};

struct MaskData
{
	Vec2<float>maskPos;
	Vec2<float>maskSize;
	Vec2<float>pos;
};

class DebugParameter :public Singleton<DebugParameter>
{
public:
	DebugParameter();

	void Update();
	void DrawImGui();

	std::vector<PlayerDebugParameterData> playerData;
	std::shared_ptr<PlayerDebugParameterData> nowData;
	bool loadPresetFlag;
	int selectNum;

	std::shared_ptr<RoundParameterData> roundData;
	std::shared_ptr<GaugeDebugParameterData> gaugeData;

	int gaugeParamImguiHandle;
	int roundParamImguiHandle;
	int bossParamImguiHandle;

	MaskData maskData;

	int bossStageNum;
	std::vector<BossDebugParameterData> bossDebugData;
	const BossDebugParameterData &GetBossData();

	std::string GetStatus(int NUM)
	{
		std::string getName;
		switch (NUM)
		{
		case 0:
			getName = "MOVE";
			return getName;
			break;

		case 1:
			getName = "ATTACK";
			return getName;
			break;

		case 2:
			getName = "SWING";
			return getName;
			break;
		}
	}

};

