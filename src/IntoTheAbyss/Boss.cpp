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
	const std::string IronBallRelative = "resource/ChainCombat/ironBall/";

	std::vector<Anim>animations;
	animations.resize(ANIMAHANDLE_MAX);

	//DEFAULT
	static const int DEFAULT_NUM = 4;
	animations[DEFAULT].graph.resize(DEFAULT_NUM);
	TexHandleMgr::LoadDivGraph(IronBallRelative + "default.png", DEFAULT_NUM, Vec2<int>(DEFAULT_NUM, 1), animations[DEFAULT].graph.data());
	animations[DEFAULT].interval = 10;
	animations[DEFAULT].loop = true;

	//EXPLOSION_OPEN
	static const int EXPLOSION_OPEN_NUM = 3;
	animations[EXPLOSION_OPEN].graph.resize(EXPLOSION_OPEN_NUM);
	TexHandleMgr::LoadDivGraph(IronBallRelative + "explosion_open.png", EXPLOSION_OPEN_NUM, Vec2<int>(EXPLOSION_OPEN_NUM, 1), animations[EXPLOSION_OPEN].graph.data());
	animations[EXPLOSION_OPEN].interval = 5;
	animations[EXPLOSION_OPEN].loop = false;

	//EXPLOSION_CLOSE
	static const int EXPLOSION_CLOSE_NUM = 4;
	animations[EXPLOSION_CLOSE].graph.resize(EXPLOSION_CLOSE_NUM);
	TexHandleMgr::LoadDivGraph(IronBallRelative + "explosion_close.png", EXPLOSION_CLOSE_NUM, Vec2<int>(EXPLOSION_CLOSE_NUM, 1), animations[EXPLOSION_CLOSE].graph.data());
	animations[EXPLOSION_CLOSE].interval = 5;
	animations[EXPLOSION_CLOSE].loop = false;

	//CRASH
	static const int CRASH_NUM = 7;
	animations[CRASH].graph.resize(CRASH_NUM);
	TexHandleMgr::LoadDivGraph(IronBallRelative + "crash.png", CRASH_NUM, Vec2<int>(CRASH_NUM, 1), animations[CRASH].graph.data());
	animations[CRASH].interval = 5;
	animations[CRASH].loop = false;

	anim = std::make_shared<PlayerAnimation>(animations);

	initNaviAiFlag = false;

	appearBossGraphHandle = animations[DEFAULT].graph[0];
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
	bossCount = 0;
	//characterAi.Init();
	anim->Init(DEFAULT);

	//他の処理との都合上Initに一回のみ初期化
	if (!initNaviAiFlag)
	{
		//パターンに渡すデータの初期化
		//navigationAi.Init(*StageMgr::Instance()->GetLocalMap());
		initNaviAiFlag = true;
	}

	initShakeFalg = false;
	bossScale = { 0.7f,0.7f };

	flashTimer = 0;
	flashMaxTimer[FIRST_LEVEL] = 120;
	flashMaxTimer[SECOND_LEVEL] = 60;
	flashMaxTimer[THIRD_LEVEL] = 30;

	crashMaxNum[FIRST_LEVEL] = 100;
	crashMaxNum[SECOND_LEVEL] = 500;
	crashMaxNum[THIRD_LEVEL] = 1000;
}

#include"Camera.h"
void Boss::OnUpdate(const MapChipArray& MapData)
{
	/*===== 更新処理 =====*/

	//揺れ開始
	if (CharacterAIOrder::Instance()->prevSwingFlag && !initShakeFalg)
	{
		initShakeFalg = true;
	}
	//揺れ終了
	if (!CharacterAIOrder::Instance()->prevSwingFlag || GetNowSwing())
	{
		countDown = 0;
		bossCount = 0;
		angle = 0;
		initShakeFalg = false;
	}


	//揺れ中
	if (initShakeFalg)
	{
		float shakeValue = CharacterAIOrder::Instance()->prevRate;
		bossCount += (0.8f * shakeValue);
		const float PI2 = 3.14f;
		countDown = sinf(PI2 / 120.0f + bossCount);
	}

	pointPos.x = pos.x + cosf(Angle::ConvertToRadian(angle)) * 1.0f;
	pointPos.y = pos.y + sinf(Angle::ConvertToRadian(angle)) * 1.0f;
	angle += 10.0f;
	Vec2<float>nomal = pos - pointPos;
	nomal.Normalize();

	shakeValue = { (nomal.x * countDown) * 10.0f,(nomal.y * countDown) * 10.0f };


	characterAi.UpdateGauge();


	if (GetNowSwing())
	{
		Vec2<float>dir = -Vec2<float>(partner.lock()->pos - pos).GetNormal();
		bossGraphRadian = atan2f(dir.y, dir.x);
	}
	else
	{
		bossGraphRadian = 0.0f;
	}


	switch (bossCrashModel)
	{
	case Boss::NONE_LEVEL:
		starParticleMgr.ParticleLevel(0, 0);
		break;
	case Boss::FIRST_LEVEL:
		starParticleMgr.ParticleLevel(15, 50);
		break;
	case Boss::SECOND_LEVEL:
		starParticleMgr.ParticleLevel(20, 100);
		break;
	case Boss::THIRD_LEVEL:
		starParticleMgr.ParticleLevel(30, 150);
		break;
	default:
		break;
	}

	if(partner.lock()->isDestroyMode){
	
		starParticleMgr.ParticleLevel(20, 100);
	
	}


	starParticleMgr.Update();

	// パートナーが振り回していたら残像を出す。
	if (partner.lock()->GetNowSwing()) {
		AfterImageMgr::Instance()->Generate(pos, {}, 0, anim->GetGraphHandle(), Color(239, 1, 144, 255), true, size);
	}

	//バウンド中残像を出す
	if (0.1f < bounceVel.Length())
	{
		AfterImageMgr::Instance()->Generate(pos, {}, 0, anim->GetGraphHandle(), Color(239, 1, 144, 255), true, size);
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
	}
	else if (isSwingNow) {
	}
	else if (GetCanMove()) {
		//ボスのAI-----------------------
		characterAi.shortestData = navigationAi.GetShortestRoute();

		navigationAi.startPoint = characterAi.startPoint;
		navigationAi.endPoint = characterAi.endPoint;
		navigationAi.startFlag = characterAi.startFlag;

		if (DebugParameter::Instance()->aiFlag)
		{
			navigationAi.Update({});
			characterAi.Update();
		}
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
	CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData, true);
	CharacterAIData::Instance()->swingClockwiseDistance = CWSwingSegmentMgr.CalSwingEndDistance(pos, swingTargetVec, (pos - partner.lock()->pos).Length());

	CCWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
	CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData, true);
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


	//現在壊したブロックの数
	int crashNum = 0;

	//壊したブロックの数によって段階を分ける----------------------------------------------
	if (crashNum < crashMaxNum[FIRST_LEVEL])
	{
		bossCrashModel = NONE_LEVEL;
	}
	if (crashMaxNum[FIRST_LEVEL] <= crashNum)
	{
		bossCrashModel = FIRST_LEVEL;
	}
	if (crashMaxNum[SECOND_LEVEL] <= crashNum)
	{
		bossCrashModel = SECOND_LEVEL;
	}
	if (crashMaxNum[THIRD_LEVEL] <= crashNum)
	{
		bossCrashModel = THIRD_LEVEL;
	}
	//壊したブロックの数によって段階を分ける----------------------------------------------

	//フラッシュする----------------------------------------------
	if (partner.lock()->isDestroyMode && flashMaxTimer[SECOND_LEVEL] <= flashTimer)
	{
		stagingDevice.Flash(flashMaxTimer[SECOND_LEVEL], 0.7f);
		flashTimer = 0;
	}
	++flashTimer;
	//フラッシュする----------------------------------------------
}

#include"DrawFunc_FillTex.h"
#include"GameSceneCamerMove.h"
#include"D3D12App.h"
void Boss::OnDraw(const bool& isRoundStartEffect)
{
	if (anim->GetNowAnim() == CRASH && anim->FinishNowAnim())
	{
		anim->ChangeAnim(DEFAULT);
	}

	/*===== 描画処理 =====*/
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = stagingDevice.GetExtRate() * SCALE * appearExtRate;

	static std::shared_ptr<TextureBuffer> CRASH_TEX = {};

	//フラッシュする色を変える----------------------------------------------
	switch (bossCrashModel)
	{
	case Boss::FIRST_LEVEL:

		CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 255, 255, 255));
		break;
	case Boss::SECOND_LEVEL:
		CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(0, 0, 255, 255));
		break;
	case Boss::THIRD_LEVEL:
		CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(0, 255, 0, 255));
		break;
	default:
		break;
	}
	//フラッシュする色を変える----------------------------------------------


	if (!initPaticleFlag)
	{
		DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale) + GameSceneCameraMove::Instance()->move, ScrollMgr::Instance()->Affect(drawPos + drawScale),
			TexHandleMgr::GetTexBuffer(appearBossGraphHandle), CRASH_TEX, stagingDevice.GetFlashAlpha());
	}
	else
	{
		//float alphaRate = alpha / 255.0f;
		float alphaRate = 1.0f;
		DrawFunc_FillTex::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(drawPos + shakeValue) + GameSceneCameraMove::Instance()->move, bossScale * ScrollMgr::Instance()->zoom * alphaRate,
			bossGraphRadian, TexHandleMgr::GetTexBuffer(anim->GetGraphHandle()), CRASH_TEX, stagingDevice.GetFlashAlpha());
	}

	//CWSwingSegmentMgr.Draw(RIGHT_TEAM);
	//CCWSwingSegmentMgr.Draw(RIGHT_TEAM);

	//navigationAi.Draw();
	characterAi.Draw();
	starParticleMgr.Draw();
	/*bossGraph.SetTexture(TexHandleMgr::GetTexBuffer(anim->GetGraphHandle()));
	bossGraph.SetColor(Color(255, 0, 0, static_cast<int>(255 * stagingDevice.GetFlashAlpha())));
	bossGraph.transform.SetPos(ScrollMgr::Instance()->Affect(drawPos));
	bossGraph.transform.SetScale(Vec2<float>(2.5f, 2.5f));
	Vec3<Angle>lAngle = { 0.0f,0.0f,Angle::ConvertToDegree(bossGraphRadian) };
	bossGraph.transform.SetRotate(lAngle);
	bossGraph.Draw();*/

}

void Boss::OnCrash()
{
	anim->ChangeAnim(CRASH, true);
}

void Boss::Shot(const Vec2<float>& generatePos, const float& forwardAngle, const float& speed)
{
	static const int BULLET_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/bullet_enemy.png");
	//bulletMgr.Generate(BULLET_GRAPH, generatePos, forwardAngle, speed);
}