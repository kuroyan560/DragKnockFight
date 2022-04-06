#include "SuperiorityGauge.h"
#include<math.h>
#include"Engine/ImguiApp.h"
#include"../Engine/DrawFunc_Color.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"

float SuperiorityGauge::GAUGE_MAX_VALUE = 100.0f;
int SuperiorityGauge::STACK_MAX_TIMER = 120;

SuperiorityGauge::SuperiorityGauge()
{
	playerGaugeData = std::make_unique<GaugeData>();
	enemyGaugeData = std::make_unique<GaugeData>();
	gaugeGraphHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/gauge_flame.png");
	gaugeVarGraphHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/gauge.png");

	Vec2<float>winHalf(1280.0f / 2.0f, 720.0f / 2.0f);
	gaguePos = winHalf;
	gagueVarPos = winHalf;

	gagueVarPos.x -= 1135.0f / 2.0f;
	gagueVarPos.y -= 25.0f / 2.0f;
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
	playerGaugeData.reset();
	enemyGaugeData.reset();
	playerGaugeData = std::make_unique<GaugeData>();
	enemyGaugeData = std::make_unique<GaugeData>();

	playerGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
	enemyGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
}

void SuperiorityGauge::Update()
{
	//ƒfƒoƒbƒN—p
	if (GAUGE_MAX_VALUE != oldMaxGauge)
	{
		playerGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
		enemyGaugeData->gaugeValue = GAUGE_MAX_VALUE / 2.0f;
	}
	oldMaxGauge = GAUGE_MAX_VALUE;

	LimitGauge();

	//Š„‡ŒvZ
	playerGaugeData->gaugeDivValue = playerGaugeData->gaugeValue / GAUGE_MAX_VALUE;
	enemyGaugeData->gaugeDivValue = enemyGaugeData->gaugeValue / GAUGE_MAX_VALUE;


	//hR‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
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

void SuperiorityGauge::Draw()
{
	DrawFunc::DrawRotaGraph2D(gaguePos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(gaugeGraphHandle));
	DrawFunc_Color::DrawGraph(gagueVarPos, TexHandleMgr::GetTexBuffer(gaugeVarGraphHandle), Color(255, 126, 204, 255), { false,false }, Vec2<float>(enemyGaugeData->gaugeDivValue, 0.0f), Vec2<float>(1.0f, 1.0f));
}

void SuperiorityGauge::DebugValue(float *ADD_VALUE)
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

void SuperiorityGauge::LimitGauge()
{
	playerGaugeData->overGaugeFlag = false;
	enemyGaugeData->overGaugeFlag = false;

	//ƒvƒŒƒCƒ„[‚ÌÅ¬§ŒÀ
	if (playerGaugeData->gaugeValue <= 0.0f)
	{
		playerGaugeData->gaugeValue = 0.0f;
	}
	//“G‚ÌÅ¬§ŒÀ
	if (enemyGaugeData->gaugeValue <= 0.0f)
	{
		enemyGaugeData->gaugeValue = 0.0f;
	}
	//ƒvƒŒƒCƒ„[‚ªU‚èØ‚Á‚½”»’è
	if (GAUGE_MAX_VALUE <= playerGaugeData->gaugeValue)
	{
		playerGaugeData->gaugeValue = GAUGE_MAX_VALUE;
		playerGaugeData->overGaugeFlag = true;
	}
	//“G‚ªU‚èØ‚Á‚½”»’è
	if (GAUGE_MAX_VALUE <= enemyGaugeData->gaugeValue)
	{
		enemyGaugeData->gaugeValue = GAUGE_MAX_VALUE;
		enemyGaugeData->overGaugeFlag = true;
	}
}
