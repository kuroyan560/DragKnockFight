#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include "UsersInput.h"
#include "SuperiorityGauge.h"
#include "SuperiorityGauge.h"
#include "SlowMgr.h"
#include "StunEffect.h"

#include"TexHandleMgr.h"
#include"CrashMgr.h"
#include"CharacterInterFace.h"

#include"ParticleMgr.h"
#include"DebugKeyManager.h"

#include"DebugParameter.h"
#include"AfterImage.h"

#include"CharacterAIData.h"
#include"CharacterManager.h"
#include"Stamina.h"

static const Vec2<float> SCALE = { 80.0f,80.0f };
Boss::Boss() :CharacterInterFace(SCALE)
{
	//graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy.png");
	//graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy_back.png");

	const std::string BossRelative = "resource/ChainCombat/boss/0/";

	std::vector<Anim>animations;
	animations.resize(ANIMAHANDLE_MAX);

	static const int DEFAULT_FRONT_NUM = 12;
	animations[FRONT].graph.resize(DEFAULT_FRONT_NUM);
	TexHandleMgr::LoadDivGraph(BossRelative + "default.png", DEFAULT_FRONT_NUM, { DEFAULT_FRONT_NUM,1 }, animations[FRONT].graph.data());
	animations[FRONT].interval = 5;
	animations[FRONT].loop = true;

	static const int DEFAULT_BACK_NUM = 12;
	animations[BACK].graph.resize(DEFAULT_BACK_NUM);
	TexHandleMgr::LoadDivGraph(BossRelative + "default_back.png", DEFAULT_FRONT_NUM, { DEFAULT_FRONT_NUM,1 }, animations[BACK].graph.data());
	animations[BACK].interval = 5;
	animations[BACK].loop = true;

	static const int DEFAULT_DAMAGE_NUM = 1;
	animations[DAMAGE].graph.resize(DEFAULT_DAMAGE_NUM);
	animations[DAMAGE].graph[0] = TexHandleMgr::LoadGraph(BossRelative + "damage.png");
	animations[DAMAGE].interval = 0;
	animations[DAMAGE].loop = false;


	anim = std::make_shared<PlayerAnimation>(animations);

	initNaviAiFlag = false;
}

void Boss::OnInit()
{
	/*===== 生成処理 =====*/

	moveVel = { 0,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;

	afterImgageTimer = 0;

	characterAi.Init();
	anim->Init(FRONT);

	//他の処理との都合上Initに一回のみ初期化
	if (!initNaviAiFlag)
	{
		//パターンに渡すデータの初期化
		navigationAi.Init(*StageMgr::Instance()->GetLocalMap());
		initNaviAiFlag = true;
	}

	handMgr.Init(false);

}

#include"Camera.h"
void Boss::OnUpdate(const std::vector<std::vector<int>> &MapData)
{
	/*===== 更新処理 =====*/

	//if (Camera::Instance()->Active())
	//{
	//	moveVel = { 0,0 };
	//	return;
	//}

	if (signbit(CharacterAIData::Instance()->prevPos.x - CharacterAIData::Instance()->nowPos.x))
	{
		anim->ChangeAnim(BACK);
	}
	else
	{
		anim->ChangeAnim(FRONT);
	}



	handMgr.Hold(-Vec2<float>(partner.lock()->pos - pos).GetNormal(), GetNowSwing());

	handMgr.Update(pos + Vec2<float>(0.0f, 20.0f));



	// パートナーが振り回していたら残像を出す。
	if (partner.lock()->GetNowSwing()) {
		AfterImageMgr::Instance()->Generate(pos, {}, 0, anim->GetGraphHandle(), Color(239, 1, 144, 255), true, size);
		anim->ChangeAnim(DAMAGE);
	}

	//プレイヤーを振り回し終えた後にダッシュする
	CharacterAIData::Instance()->releaseSwingFlag = !GetNowSwing() && CharacterAIData::Instance()->prevSwingFlag;
	CharacterAIData::Instance()->prevSwingFlag = GetNowSwing();

	// パートナーが振り回し状態だったら更新処理を行わない。
	if (!(!partner.lock()->GetNowSwing() && !nowSwing)) return;

	// [振り回し中か振り回され中だったら] 更新処理を行わない。　　臨の実装です。
	bool isSwingNow = this->GetNowSwing() || partner.lock()->GetNowSwing();


	if (CharacterManager::Instance()->Right()->GetNowBreak())
	{
		CWSwingSegmentMgr.Init();
		CCWSwingSegmentMgr.Init();
	}

	int staminaMax = DebugParameter::Instance()->GetBossData().staminaMax;
	if (staminaMax != prevStaminaMax)
	{
		staminaGauge->Resize(staminaMax);
	}
	prevStaminaMax = staminaMax;


	// [硬直中] [スタン演出中] は動かさない
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// 何もしない。
		anim->ChangeAnim(DAMAGE);
	}
	else if (isSwingNow) {
		anim->ChangeAnim(DAMAGE);
	}
	else if (GetCanMove()) {
		//ボスのAI-----------------------
		characterAi.shortestData = navigationAi.GetShortestRoute();

		navigationAi.startPoint = characterAi.startPoint;
		navigationAi.endPoint = characterAi.endPoint;
		navigationAi.startFlag = characterAi.startFlag;

		navigationAi.Update({});
		characterAi.Update();

		//ボスのAI-----------------------
	}

	//振り回し命令
	if (CharacterAIOrder::Instance()->swingClockWiseFlag)
	{
		// 振り回しのトリガー判定
		SwingPartner(-Vec2<float>(partner.lock()->pos - pos).GetNormal(), true);
		CharacterAIOrder::Instance()->swingClockWiseFlag = false;
	}
	else if (CharacterAIOrder::Instance()->swingCounterClockWiseFlag)
	{
		// 振り回しのトリガー判定
		SwingPartner(-Vec2<float>(partner.lock()->pos - pos).GetNormal(), false);
		CharacterAIOrder::Instance()->swingCounterClockWiseFlag = false;
	}

	CWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
	CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
	CharacterAIData::Instance()->swingClockwiseDistance = CWSwingSegmentMgr.CalSwingEndDistance(pos, swingTargetVec, (pos - partner.lock()->pos).Length());

	CCWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
	CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
	CharacterAIData::Instance()->swingCounterClockwiseDistance = CCWSwingSegmentMgr.CalSwingEndDistance(pos, swingTargetVec, (pos - partner.lock()->pos).Length());

	anim->Update();

	if (CharacterAIOrder::Instance()->dashFlag)
	{
		afterImgageTimer = 15.0f;
		CharacterAIOrder::Instance()->dashFlag = false;
	}
	//ダッシュの残像
	if (afterImgageTimer)
	{
		AfterImageMgr::Instance()->Generate(pos, Vec2<float>(1.0f, 1.0f) * ScrollMgr::Instance()->zoom, 0.0f, anim->GetGraphHandle(), GetTeamColor());
		afterImgageTimer--;
	}

	// 移動量に関する変数をここで全てvelに代入する。
	vel = CharacterAIOrder::Instance()->vel;
}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::OnDraw()
{
	/*===== 描画処理 =====*/
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = stagingDevice.GetExtRate() * SCALE * appearExtRate;
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));


	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
		TexHandleMgr::GetTexBuffer(anim->GetGraphHandle()), CRASH_TEX, stagingDevice.GetFlashAlpha());

	//CWSwingSegmentMgr.Draw(RIGHT_TEAM);
	//CCWSwingSegmentMgr.Draw(RIGHT_TEAM);

	//navigationAi.Draw();
	//characterAi.Draw();

	handMgr.Draw();

}

void Boss::Shot(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed)
{
	static const int BULLET_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/bullet_enemy.png");
	bulletMgr.Generate(BULLET_GRAPH, generatePos, forwardAngle, speed);
}