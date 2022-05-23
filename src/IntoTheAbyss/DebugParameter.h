#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include<vector>
#include"DebugImGuiManager.h"

struct PlayerDebugParameterData
{
	float playerSpeed;
	float damage;//ÉQÅ[ÉWó 

	PlayerDebugParameterData(float SPEED = 15.0f) :playerSpeed(SPEED), damage(17.0f)
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

	GaugeDebugParameterData() :playerBulletAddGuaugeValue(4.0f), enemyBulletAddGuaugeValue(2.0f),
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
	float staminaHealAmount;
	float swingAngle;
	float swingMax;
	bool enableToDashAfterSwingFlag;

	float damage;
	BossDebugParameterData() :
		vel(14.0f), coolTime(30), staminaMax(6), staminaDash(1), staminaSwing(2), enableToDashAfterSwingFlag(false),
		swingAngle(0.02f), swingMax(0.11f), staminaHealAmount(1.5f), damage(10.0f)
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
	int playerParamImguiHandle;

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


	bool aiFlag;

};

