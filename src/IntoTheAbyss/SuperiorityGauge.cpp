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
	gaugeGraphHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_flame.png");
	gaugeVarGraphPlayer = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_player.png");
	gaugeVarGraphEnemy = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/gauge_enemy.png");

	imguiHandle = DebugImGuiManager::Instance()->Add("Gauge");
}

void SuperiorityGauge::AddGauge(const WHICH_TEAM& Team, const float& Value)
{
	gaugeData[Team].gaugeValue += Value;
	gaugeData[TEAM_NUM - Team - 1].gaugeValue -= Value;
}

const SuperiorityGauge::GaugeData& SuperiorityGauge::GetGaugeData(const WHICH_TEAM& Team)
{
	return gaugeData[Team];
}

void SuperiorityGauge::Init()
{
	gaugeData[LEFT_TEAM].Init(GAUGE_MAX_VALUE);
	gaugeData[RIGHT_TEAM].Init(GAUGE_MAX_VALUE);
}

void SuperiorityGauge::Update()
{
	//デバック用
	if (GAUGE_MAX_VALUE != oldMaxGauge)
	{
		gaugeData[LEFT_TEAM].gaugeValue = GAUGE_MAX_VALUE / 2.0f;
		gaugeData[RIGHT_TEAM].gaugeValue = GAUGE_MAX_VALUE / 2.0f;
	}
	oldMaxGauge = GAUGE_MAX_VALUE;

	//前フレームの振り切りフラグを保存。
	gaugeData[LEFT_TEAM].prevOverGaugeFlag = gaugeData[LEFT_TEAM].overGaugeFlag;
	gaugeData[RIGHT_TEAM].prevOverGaugeFlag = gaugeData[RIGHT_TEAM].overGaugeFlag;

	LimitGauge();

	//割合計算
	gaugeData[LEFT_TEAM].gaugeDivValue = gaugeData[LEFT_TEAM].gaugeValue / GAUGE_MAX_VALUE;
	gaugeData[RIGHT_TEAM].gaugeDivValue = gaugeData[RIGHT_TEAM].gaugeValue / GAUGE_MAX_VALUE;


	//拮抗しているかどうか
	if (gaugeData[LEFT_TEAM].gaugeDivValue == gaugeData[RIGHT_TEAM].gaugeDivValue)
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
		TexHandleMgr::GetTexBuffer(gaugeVarGraphEnemy), 1.0f, { 0.5f,0.5f }, { false,false }, Vec2<float>(gaugeData[LEFT_TEAM].gaugeDivValue, 0.0f));
}

void SuperiorityGauge::DebugValue(float *ADD_VALUE)
{
	if (DebugImGuiManager::Instance()->DrawFlag(imguiHandle))
	{
		ImGui::Begin("Gauge");
		ImGui::Text("Q...AddPlayerGaugeValue,W...AddEnemyGagueValue");
		ImGui::Text("PlayerGauge:%f EnemyGauge:%f", gaugeData[LEFT_TEAM].gaugeValue, gaugeData[RIGHT_TEAM].gaugeValue);
		ImGui::Text("PlayerDivGauge:%f EnemyDivGauge:%f", gaugeData[LEFT_TEAM].gaugeDivValue, gaugeData[RIGHT_TEAM].gaugeDivValue);
		ImGui::Text("PlayerOver:%d EnemyOver:%d", gaugeData[LEFT_TEAM].overGaugeFlag, gaugeData[RIGHT_TEAM].overGaugeFlag);
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
	gaugeData[LEFT_TEAM].overGaugeFlag = false;
	gaugeData[RIGHT_TEAM].overGaugeFlag = false;

	//プレイヤーの最小制限
	if (gaugeData[LEFT_TEAM].gaugeValue <= 0.0f)
	{
		gaugeData[LEFT_TEAM].gaugeValue = 0.0f;
	}
	//敵の最小制限
	if (gaugeData[RIGHT_TEAM].gaugeValue <= 0.0f)
	{
		gaugeData[RIGHT_TEAM].gaugeValue = 0.0f;
	}
	//プレイヤーが振り切った判定
	if (GAUGE_MAX_VALUE <= gaugeData[LEFT_TEAM].gaugeValue)
	{
		gaugeData[LEFT_TEAM].gaugeValue = GAUGE_MAX_VALUE;
		gaugeData[LEFT_TEAM].overGaugeFlag = true;
	}
	//敵が振り切った判定
	if (GAUGE_MAX_VALUE <= gaugeData[RIGHT_TEAM].gaugeValue)
	{
		gaugeData[RIGHT_TEAM].gaugeValue = GAUGE_MAX_VALUE;
		gaugeData[RIGHT_TEAM].overGaugeFlag = true;
	}
}
