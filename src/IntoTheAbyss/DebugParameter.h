#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include<vector>

struct PlayerDebugParameterData
{
	float ADD_GRAVITY;				// �v���C���[�ɂ�����d��
	float MAX_GRAVITY;			// �v���C���[�ɂ�����d�͂̍ő��
	float RECOIL_AMOUNT;			// �e���������ۂ̔���
	float FIRST_RECOIL_AMOUNT;		// �e���������ۂ̔���
	float MAX_RECOIL_AMOUNT;		// �e���������ۂ̔����̍ő�l
	int RAPID_FIRE_TIMER;			// �A�˃^�C�}�[

	PlayerDebugParameterData(const float &ADD_GRAVITY, const float &MAX_GRAVITY, const float &RECOIL_AMOUNT,
		const float &FIRST_RECOIL_AMOUNT, const float &MAX_RECOIL_AMOUNT, const float &RAPID_FIRE_TIMER):
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

	GaugeDebugParameterData() :playerBulletAddGuaugeValue(10.0f), enemyBulletAddGuaugeValue(10.0f), 
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


	std::shared_ptr<GaugeDebugParameterData> gaugeData;

	std::shared_ptr<RoundParameterData> data;
};

