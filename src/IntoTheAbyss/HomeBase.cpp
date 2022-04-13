#include "HomeBase.h"
#include"../Engine/ImguiApp.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"ShakeMgr.h"

int HomeBase::AREA_NUM = 0;

HomeBase::HomeBase()
{
	if (AREA_NUM == 0)
	{
		name = "PlayerHomeBase";
	}
	else
	{
		name = "EnemyHomeBase";
	}
	++AREA_NUM;
}

void HomeBase::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const bool& LeftPlayer)
{
	leftUpPos = LEFT_UP_POS;
	rightDownPos = RIGHT_DOWN_POS;

	hitBox.size = rightDownPos - leftUpPos;
	centerPos = leftUpPos + (hitBox.size / 2.0f);
	hitBox.center = &centerPos;

	leftPlayer = LeftPlayer;

	borderHeight.clear();
	for (int y = LEFT_UP_POS.y - BORDER_GRAPH_HEIGHT; y < RIGHT_DOWN_POS.y; y += BORDER_GRAPH_HEIGHT)
	{
		borderHeight.emplace_back(y);
	}

}

bool HomeBase::Collision(const Square &OBJ_A)
{
	return AreaCollider::Instance()->CheckHitArea(OBJ_A, hitBox);
}

#include"TexHandleMgr.h"
#include"DrawFunc_Mask.h"
#include"SlowMgr.h"
void HomeBase::Draw()
{
	Vec2<float>drawLeftUpPos = ScrollMgr::Instance()->Affect(leftUpPos);
	Vec2<float>drawRightDownPos = ScrollMgr::Instance()->Affect(rightDownPos);
	
	static const int AREA_ALPHA = 100;
	static Color PLAYER_COLOR = Color(47, 255, 139, AREA_ALPHA);
	static Color ENEMY_COLOR = Color(239, 1, 144, AREA_ALPHA);

	//—Ìˆæ•`‰æ
	DrawFunc::DrawBox2D(drawLeftUpPos, drawRightDownPos, leftPlayer ? PLAYER_COLOR : ENEMY_COLOR, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

	//‹«ŠE•`‰æ
	static const int PLAYER_BORDER = TexHandleMgr::LoadGraph("resource/ChainCombat/borderLine_player.png");
	static const int ENEMY_BORDER = TexHandleMgr::LoadGraph("resource/ChainCombat/borderLine_enemy.png");
	static const float BORDER_HEIGHT_MOVE = 1.0f;

	for (int i = 0; i < borderHeight.size(); ++i)
	{
		borderHeight[i] += BORDER_HEIGHT_MOVE * SlowMgr::Instance()->slowAmount;
		if (rightDownPos.y < borderHeight[i])borderHeight[i] = leftUpPos.y - BORDER_GRAPH_HEIGHT;

		Vec2<float>borderFromPos = leftUpPos;
		if (!leftPlayer)
		{
			borderFromPos.x += BORDER_GRAPH_WIDTH / 2.0f;
		}
		else
		{
			borderFromPos.x = rightDownPos.x;
			borderFromPos.x -= BORDER_GRAPH_WIDTH / 2.0f;
		}
		
		borderFromPos.y = borderHeight[i];
		Vec2<float>borderToPos = borderFromPos;
		borderToPos.y += BORDER_GRAPH_HEIGHT;
		DrawFunc_Mask::DrawLine2DGraph(ScrollMgr::Instance()->Affect(borderFromPos), 
			ScrollMgr::Instance()->Affect(borderToPos), TexHandleMgr::GetTexBuffer(leftPlayer ? PLAYER_BORDER : ENEMY_BORDER), BORDER_GRAPH_WIDTH, drawLeftUpPos, drawRightDownPos);
		//DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(borderFromPos), ScrollMgr::Instance()->Affect(borderToPos), TexHandleMgr::GetTexBuffer(leftPlayer ? PLAYER_BORDER : ENEMY_BORDER), BORDER_GRAPH_WIDTH);
	}

}

void HomeBase::Debug()
{
	ImGui::Begin(name.c_str());
	ImGui::InputFloat("leftUpPosX", &leftUpPos.x);
	ImGui::InputFloat("leftUpPosY", &leftUpPos.y);
	ImGui::InputFloat("rightDownPosX", &rightDownPos.x);
	ImGui::InputFloat("rightDownPosY", &rightDownPos.y);
	ImGui::End();
}
