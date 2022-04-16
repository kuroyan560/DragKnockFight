#include "SuperiorityGauge.h"
#include<math.h>
#include"Engine/ImguiApp.h"
#include"../Engine/DrawFunc_FillTex.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"
#include"DebugImGuiManager.h"

float SuperiorityGauge::GAUGE_MAX_VALUE = 100.0f;
int SuperiorityGauge::STACK_MAX_TIMER = 120;

SuperiorityGauge::SuperiorityGauge()
{
	playerGaugeData = std::make_unique<GaugeData>();
	enemyGaugeData = std::make_unique<GaugeData>();
	gaugeGraphHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_flame.png");
	gaugeVarGraphPlayer = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_player.png");
	gaugeVarGraphEnemy = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_enemy.png");

	imguiHandle = DebugImGuiManager::Instance()->Add("Gauge");
}

void SuperiorityGauge::AddPlayerGauge(const float &VALUE)
{
	float value = fabs(VALUE);
	playerGaugeData->gaugeValue += value;
	enemyGaugeData->gaugeValue += -value;

	LimitGauge();
}

void SuperiorityGauge::AddEnemyGauge(const float &VALUE)
{
	float value = fabs(VALUE);
	playerGaugeData->gaugeValue += -value;
	enemyGaugeData->gaugeValue += value;

	LimitGauge();
}

const std::unique_ptr<SuperiorityGauge::GaugeData> &SuperiorityGauge::GetPlayerGaugeData()
{
	return playerGaugeData;
}

const std::unique_ptr<SuperiorityGauge::GaugeData> &SuperiorityGauge::GetEnemyGaugeData()
{
	return enemyGaugeData;
}

const bool &SuperiorityGauge::IsStacking()
{
	return isStackingFlag;
}

void SuperiorityGauge::Init()
{
	playerGaugeData->Init(GAUGE_MAX_VALUE);
	enemyGaugeData->Init(GAUGE_MAX_VALUE);
}

void SuperiorityGauge::Update()
{
	//デバック用
	if (GAUGE_MAX_VALUE != oldMaxGauge)
	{
		playerGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
		enemyGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
	}
	oldMaxGauge = GAUGE_MAX_VALUE;

	//前フレームの振り切りフラグを保存。
	playerGaugeData->prevOverGaugeFlag = playerGaugeData->overGaugeFlag;
	enemyGaugeData->prevOverGaugeFlag = enemyGaugeData->overGaugeFlag;

	LimitGauge();

	//割合計算
	playerGaugeData->gaugeDivValue = playerGaugeData->gaugeValue / GAUGE_MAX_VALUE;
	enemyGaugeData->gaugeDivValue = enemyGaugeData->gaugeValue / GAUGE_MAX_VALUE;


	//拮抗しているかどうか
	if (playerGaugeData->gaugeDivValue == enemyGaugeData->gaugeDivValue)
	{
		++stackTimer;
	}
	else
	{
		stackTimer = 0.0f;
	}

	if (STACK_MAX_TIMER <= stackTimer)
	{
		isStackingFlag = true;
	}
	else
	{
		isStackingFlag = false;
	}
}

#include"D3D12App.h"
void SuperiorityGauge::Draw()
{
	static const Vec2<float>OFFSET = { 0.0f,23.0f };

	Vec2<float>gaguePos(1280.0f / 2.0f, 65.0f);
	DrawFunc::DrawRotaGraph2D(gaguePos + OFFSET, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(gaugeGraphHandle));

	Vec2<float> gaugeVarPos = { gaguePos.x,101.0f };
	DrawFunc_FillTex::DrawRotaGraph2D(gaugeVarPos + OFFSET, { 1.0f,1.0f }, 0.0f, TexHandleMgr::GetTexBuffer(gaugeVarGraphPlayer),
		TexHandleMgr::GetTexBuffer(gaugeVarGraphEnemy), 1.0f, { 0.5f,0.5f }, { false,false }, Vec2<float>(playerGaugeData->gaugeDivValue, 0.0f));
}

void SuperiorityGauge::DebugValue(float *ADD_VALUE)
{
	if (DebugImGuiManager::Instance()->DrawFlag(imguiHandle))
	{
		ImGui::Begin("Gauge");
		ImGui::Text("Q...AddPlayerGaugeValue,W...AddEnemyGagueValue");
		ImGui::Text("PlayerGauge:%f EnemyGauge:%f", playerGaugeData->gaugeValue, enemyGaugeData->gaugeValue);
		ImGui::Text("PlayerDivGauge:%f EnemyDivGauge:%f", playerGaugeData->gaugeDivValue, enemyGaugeData->gaugeDivValue);
		ImGui::Text("PlayerOver:%d EnemyOver:%d", playerGaugeData->overGaugeFlag, enemyGaugeData->overGaugeFlag);
		ImGui::Text("StackTimer:%d", stackTimer);
		ImGui::Text("StackFlag:%d", isStackingFlag);
		ImGui::InputFloat("GAUGE_MAX_VALUE", &GAUGE_MAX_VALUE);
		ImGui::InputFloat("ADD_VALUE", ADD_VALUE);
		ImGui::InputInt("STACK_MAX_TIMER", &STACK_MAX_TIMER);
		ImGui::End();
	}
}

void SuperiorityGauge::LimitGauge()
{
	playerGaugeData->overGaugeFlag = false;
	enemyGaugeData->overGaugeFlag = false;

	//プレイヤーの最小制限
	if (playerGaugeData->gaugeValue <= 0.0f)
	{
		playerGaugeData->gaugeValue = 0.0f;
	}
	//敵の最小制限
	if (enemyGaugeData->gaugeValue <= 0.0f)
	{
		enemyGaugeData->gaugeValue = 0.0f;
	}
	//プレイヤーが振り切った判定
	if (GAUGE_MAX_VALUE <= playerGaugeData->gaugeValue)
	{
		playerGaugeData->gaugeValue = GAUGE_MAX_VALUE;
		playerGaugeData->overGaugeFlag = true;
	}
	//敵が振り切った判定
	if (GAUGE_MAX_VALUE <= enemyGaugeData->gaugeValue)
	{
		enemyGaugeData->gaugeValue = GAUGE_MAX_VALUE;
		enemyGaugeData->overGaugeFlag = true;
	}
}
