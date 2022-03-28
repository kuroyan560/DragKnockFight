#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include<vector>

struct PlayerDebugParameterData
{
	float ADD_GRAVITY;				// プレイヤーにかける重力
	float MAX_GRAVITY;			// プレイヤーにかける重力の最大量
	float RECOIL_AMOUNT;			// 弾を撃った際の反動
	float FIRST_RECOIL_AMOUNT;		// 弾を撃った際の反動
	float MAX_RECOIL_AMOUNT;		// 弾を撃った際の反動の最大値
	int RAPID_FIRE_TIMER;			// 連射タイマー

	PlayerDebugParameterData(const float &ADD_GRAVITY, const float &MAX_GRAVITY, const float &RECOIL_AMOUNT,
		const float &FIRST_RECOIL_AMOUNT, const float &MAX_RECOIL_AMOUNT, const float &RAPID_FIRE_TIMER):
		ADD_GRAVITY(ADD_GRAVITY), MAX_GRAVITY(MAX_GRAVITY), RECOIL_AMOUNT(RECOIL_AMOUNT), FIRST_RECOIL_AMOUNT(FIRST_RECOIL_AMOUNT),
		MAX_RECOIL_AMOUNT(MAX_RECOIL_AMOUNT), RAPID_FIRE_TIMER(RAPID_FIRE_TIMER)
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
};

