#include "Tutorial.h"
#include"TexHandleMgr.h"
static const float SCALE = 1.0f;

Tutorial::Tutorial(const WHICH_TEAM& Team) :team(Team)
{
	static bool LOAD = false;
	static IconGraphs ICON_GRAPHS[TEAM_NUM];
	if (!LOAD)
	{
		std::string additionalDir[TEAM_NUM] = { "left/","right/" };

		for (int i = 0; i < TEAM_NUM; ++i)
		{
			std::string dir = "resource/ChainCombat/tutorial/icon/" + additionalDir[i];
			ICON_GRAPHS[i].stickBase_L = TexHandleMgr::LoadGraph(dir + "L_stick_base.png");
			ICON_GRAPHS[i].stickBase_R = TexHandleMgr::LoadGraph(dir + "R_stick_base.png");
			ICON_GRAPHS[i].triggerOn_L = TexHandleMgr::LoadGraph(dir + "L_trigger_on.png");
			ICON_GRAPHS[i].triggerOn_R = TexHandleMgr::LoadGraph(dir + "R_trigger_on.png");
			ICON_GRAPHS[i].triggerOff_L = TexHandleMgr::LoadGraph(dir + "L_trigger_off.png");
			ICON_GRAPHS[i].triggerOff_R = TexHandleMgr::LoadGraph(dir + "R_trigger_off.png");
		}
		LOAD = true;
	}
	iconGraphs = ICON_GRAPHS[Team];
}

#include"DrawFunc.h"
void Tutorial::DrawIcon(const Vec2<float>& Pos, const int& Handle)
{
	DrawFunc::DrawRotaGraph2D(Pos, { 0.8 * SCALE, 0.8f * SCALE }, 0.0f, TexHandleMgr::GetTexBuffer(Handle));
	//DrawFunc::DrawGraph(Pos, TexHandleMgr::GetTexBuffer(Handle), AlphaBlendMode_Trans);
}

void Tutorial::Draw(const Vec2<float>& LStickVec, const Vec2<float>& RStickVec, const bool& LTrigger, const bool& RTrigger)
{
	if (!active)return;

	static const int STICK_HEAD_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/icon/stickHead.png");
	//右プレイヤーか左プレイヤーかでの位置オフセット
	static const Vec2<float> OFFSET[TEAM_NUM] = { {90,400},{1120,400} };
	//スティックのヘッドの半径オフセット
	static const float STICK_HEAD_RADIUS_OFFSET = 28.0f * SCALE;
	//アイコンごとのオフセット
	static const float ICON_OFFSET_Y = 80.0f;

	//左スティック
	static const Vec2<float>LstickPos = { 19,0 };
	DrawIcon(LstickPos + OFFSET[team], iconGraphs.stickBase_L);
	//左スティックヘッド
	static const Vec2<float>LstickHeadCenterOffset = Vec2<float>(-48.0f, 2.0f) * SCALE;
	const Vec2<float>LstickHeadPos = LstickPos + LstickHeadCenterOffset + LStickVec.GetNormal() * STICK_HEAD_RADIUS_OFFSET;
	DrawIcon(LstickHeadPos + OFFSET[team], STICK_HEAD_GRAPH);

	//左トリガー
	static const Vec2<float>LtriggerPos = { 22,ICON_OFFSET_Y + LstickPos.y };
	DrawIcon(LtriggerPos + OFFSET[team], LTrigger ? iconGraphs.triggerOn_L : iconGraphs.triggerOff_L);

	//右スティック＆トリガーアイコンのオフセットX
	static const float R_OFFSET_X = 20.0f;

	//右スティック
	static const Vec2<float>RstickPos = { 20 + R_OFFSET_X,20.0f + ICON_OFFSET_Y + LtriggerPos.y };
	DrawIcon(RstickPos + OFFSET[team], iconGraphs.stickBase_R);
	//右スティックヘッド
	static const Vec2<float>RstickHeadCenterOffset = Vec2<float>(33.0f, 3.0f) * SCALE;
	const Vec2<float>RstickHeadPos = RstickPos + RstickHeadCenterOffset + RStickVec.GetNormal() * STICK_HEAD_RADIUS_OFFSET;
	DrawIcon(RstickHeadPos + OFFSET[team], STICK_HEAD_GRAPH);

	//右トリガー
	static const Vec2<float>RtriggerPos = { 22 + R_OFFSET_X,ICON_OFFSET_Y + RstickPos.y };
	DrawIcon(RtriggerPos + OFFSET[team], RTrigger ? iconGraphs.triggerOn_R : iconGraphs.triggerOff_R);

}
