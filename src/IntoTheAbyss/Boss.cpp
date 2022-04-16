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
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"
#include"BossPatternSwing.h"
#include"BossBulletManager.h"
#include"CrashMgr.h"
#include"CharacterInterFace.h"

#include"ParticleMgr.h"
#include"DebugKeyManager.h"

#include"DebugParameter.h"


static const Vec2<float> SCALE = { 80.0f,80.0f };
Boss::Boss() :CharacterInterFace(SCALE)
{
	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy_back.png");

	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();
	bossPattern[2] = std::make_unique<BossPatternSwing>();


	//パターンに渡すデータの初期化
	patternData.moveVel = &moveVel;


}

void Boss::OnInit()
{
	/*===== 生成処理 =====*/

	initScale = SCALE * 5.0f;
	size = initScale;
	moveVel = { 0,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	moveTimer = 0;
	initPaticleFlag = false;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;
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

	for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
	{
		patternData.nearLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.nearLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 150.0f;
		patternData.nearLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
	{
		patternData.farLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.farLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 350.0f;
		patternData.farLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	if (bossPatternNow != BOSS_PATTERN_NORMALMOVE)
	{
		moveVel = { 10.0f,0.0f };
	}



	for (int i = 0; i < patternData.bulltData.size(); ++i)
	{
		if (patternData.bulltData[i].initFlag)
		{
			patternData.bulltData[i].Reset();
		}
	}

	// [振り回し中か振り回され中だったら] 更新処理を行わない。　　臨の実装です。
	bool isSwingNow = this->GetNowSwing() || partner.lock()->GetNowSwing();

	// [硬直中] [スタン演出中] は動かさない
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// 何もしない。
	}
	else if (isSwingNow) {

	}
	else if (GetCanMove()) {

		//ボスのAI-----------------------
		++patternTimer;
		//次の状態選択
		if (120 <= patternTimer)
		{
			Vec2<float> dir = GetPartnerPos() - pos;
			dir.Normalize();

			while (1)
			{
				int random = KuroFunc::GetRand(BOSS_PATTERN_MAX - 1);
				//振り回しの条件がそろっていない時は振り回しをさせない
				if (fabs(dir.y) <= 0.3f && random == BOSS_PATTERN_SWING)
				{
					continue;
				}
				bossPatternNow = static_cast<E_BossPattern>(random);
				break;
			}
			patternTimer = 0;
		}
		//ボスのAI-----------------------
		//bossPatternNow = BOSS_PATTERN_NORMALMOVE;

		//ボスの挙動
		if (bossPatternNow != oldBossPattern)
		{
			bossPattern[bossPatternNow]->Init();
		}
		bossPattern[bossPatternNow]->Update(&patternData);
		oldBossPattern = bossPatternNow;

		//ボスの弾
		for (int i = 0; i < patternData.bulltData.size(); ++i)
		{
			if (patternData.bulltData[i].initFlag)
			{
				BossBulletManager::Instance()->Generate(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
			}
		}
	}
	DebugParameter::Instance()->bossDebugData.bossNowStatus = static_cast<E_BossPattern>(bossPatternNow);


	//振り回しの開始ベクトルを取得。
	//if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_O, "SwingBoss", TO_STRING(DIK_O)))
	if (patternData.swingFlag)
	{
		Vec2<float> dir = GetPartnerPos() - pos;
		dir.Normalize();

		// 振り回しのトリガー判定
		if (0.3f < fabs(dir.y))
		{
			SwingPartner();
		}
	}


	DebugParameter::Instance()->bossDebugData.moveVel = moveVel;

	// 移動量の総量を求める。
	auto swingAffect = GetSwingInertia();
	if (GetSwingRigor())
	{
		// 振り回し直後の硬直が残っている場合は、慣性のみを移動量とする。
		vel = swingAffect;
	}
	else
	{
		vel = moveVel + swingAffect;
	}
}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::OnDraw()
{
	/*===== 描画処理 =====*/
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = size * stagingDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));

	if (DebugParameter::Instance()->bossDebugData.drawBossFlag)
	{
		DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
			TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, stagingDevice.GetFlashAlpha());
	}

	//レイとの判定確認
	if (DebugParameter::Instance()->bossDebugData.drawNearRayFlag)
	{
		for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].endPos);
			if (patternData.nearLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 0, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 255, 255, 255));
			}
		}
	}

	if (DebugParameter::Instance()->bossDebugData.drawFarRayFlag)
	{
		for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].endPos);
			if (patternData.farLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 255, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 0, 255, 255));
			}
		}
	}
}

//void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos, const Vec2<float>& lineCenterPos)
//{
//
//	/*===== 当たり判定 =====*/
//
//	// ボスがマップチップと当たっているかのフラグ
//	INTERSECTED_LINE intersectedBuff = {};
//
//	// マップチップ目線でどこに当たったか
//	bool isHitTop = false;
//	bool isHitRight = false;
//	bool isHitLeft = false;
//	bool isHitBottom = false;
//
//	bool onGround = false;
//	// 移動量を元にしたマップチップとの当たり判定を行う。
//	Vec2<float> moveDir = pos - prevPos;
//	moveDir.Normalize();
//	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * scale, scale, onGround, mapData, false);
//	isHitTop = intersectedLine == INTERSECTED_TOP;
//	isHitRight = intersectedLine == INTERSECTED_RIGHT;
//	isHitLeft = intersectedLine == INTERSECTED_LEFT;
//	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;
//
//	// 左上
//	Vec2<float> velPosBuff = pos - scale;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - scale, {}, {}, onGround, mapData, false);
//	pos = velPosBuff + scale;
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// 右上
//	velPosBuff = pos + Vec2<float>(scale.x, -scale.y);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(scale.x, -scale.y), {}, {}, onGround, mapData, false);
//	pos = velPosBuff - Vec2<float>(scale.x, -scale.y);
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// 右下
//	velPosBuff = pos + scale;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + scale, {}, {}, onGround, mapData, false);
//	pos = velPosBuff - scale;
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// 左下
//	velPosBuff = pos + Vec2<float>(-scale.x, scale.y);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-scale.x, scale.y), {}, {}, onGround, mapData, false);
//	pos = velPosBuff - Vec2<float>(-scale.x, scale.y);
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// スケールを元にしたマップチップとの当たり判定を行う。
//
//	// 上方向
//	float offset = 5.0f;
//	Vec2<float> posBuff = pos + Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
//	pos.y = posBuff.y;
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	posBuff = pos - Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
//	pos.y = posBuff.y;
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//
//	// 下方向
//	posBuff = pos + Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
//	pos.y = posBuff.y;
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//	posBuff = pos - Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
//	pos.y = posBuff.y;
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// 右方向
//	posBuff = pos + Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
//	pos.x = posBuff.x;
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	posBuff = pos - Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
//	pos.x = posBuff.x;
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//
//	// 下方向
//	posBuff = pos + Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
//	pos.x = posBuff.x;
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	posBuff = pos - Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
//	pos.x = posBuff.x;
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//
//
//	// マップチップと当たっていたら
//	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {
//
//		// マップチップと当たっている場所によって、引っかかっているかどうかを調べる。
//
//		// プレイヤーとの角度
//		float playerAngle = atan2(playerPos.y - pos.y, playerPos.x - pos.x);
//		playerAngle = fabs(playerAngle);
//
//		if (isHitTop) {
//
//			// マップチップの上にあたっていたということは、プレイヤーが下方向にいればOK！
//			// 下方向の具体的な値は
//			const float MIN_ANGLE = 0.785398f;
//			const float MAX_ANGLE = 2.35619f;
//
//			// 角度がこの値の間だったら
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// 引っかかっている。
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_BOTTOM;
//
//			}
//
//		}
//
//		if (isHitBottom) {
//
//			// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
//			// 上方向の具体的な値は
//			const float MIN_ANGLE = 3.92699f;
//			const float MAX_ANGLE = 5.49779f;
//
//			// 角度がこの値の間だったら
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// 引っかかっている。
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_TOP;
//
//			}
//
//		}
//
//		if (isHitRight) {
//
//			// マップチップの右にあたっていたということは、プレイヤーが左方向にいればOK！
//			// 左方向の具体的な値は
//			const float MIN_ANGLE = 2.35619f;
//			const float MAX_ANGLE = 3.92699f;
//
//			// 角度がこの値の間だったら
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// 引っかかっている。
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_RIGHT;
//
//			}
//
//		}
//
//		if (isHitLeft) {
//
//			// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
//			// 右方向の具体的な値は
//			const float MIN_ANGLE = 0.785398f;
//			const float MAX_ANGLE = 5.49779f;
//
//			// 角度がこの値の間だったら
//			if (MAX_ANGLE <= playerAngle || playerAngle <= MIN_ANGLE) {
//
//				// 引っかかっている。
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_LEFT;
//
//			}
//
//		}
//
//		// Swingのフレームが全く経過していなかったら処理を飛ばす。
//		if (SwingMgr::Instance()->easingTimer <= 0.05f) {
//
//		}
//		// 一定以下だったらダメージを与えない。
//		if (SwingMgr::Instance()->easingTimer <= 0.15f) {
//
//			// 振り回されている状態だったら、シェイクを発生させて振り回し状態を解除する。
//			Vec2<float>vec = { 0,0 };
//			if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {
//
//				if (isHitLeft)vec.x = -1.0f;
//				else if (isHitRight)vec.x = 1.0f;
//				if (isHitTop)vec.y = -1.0f;
//				else if (isHitBottom)vec.y = 1.0f;
//
//				/*Crash(vec);
//
//				SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);*/
//				SwingMgr::Instance()->isSwingPlayer = false;
//
//			}
//
//		}
//		else {
//
//			// 振り回されている状態だったら、シェイクを発生させて振り回し状態を解除する。
//			Vec2<float>vec = { 0,0 };
//			if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {
//
//				if (isHitLeft)vec.x = -1.0f;
//				else if (isHitRight)vec.x = 1.0f;
//				if (isHitTop)vec.y = -1.0f;
//				else if (isHitBottom)vec.y = 1.0f;
//
//				Crash(vec);
//
//				SuperiorityGauge::Instance()->AddPlayerGauge(10.0f);
//				SwingMgr::Instance()->isSwingPlayer = false;
//
//			}
//
//		}
//
//	}
//
//	// マップチップにあたっている状態で画面外に出たら
//	if (isHitMapChip && stuckWindowTimer <= 0) {
//
//		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
//		windowSize *= Vec2<int>(2, 2);
//
//		// ボスとプレイヤーの距離
//		float distanceX = fabs(lineCenterPos.x - pos.x);
//		float disntaceY = fabs(lineCenterPos.y - pos.y);
//
//		// ウィンドウ左右
//		bool winLeft = pos.x - scale.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
//		bool winRight = windowSize.x <= pos.x + scale.x - ScrollMgr::Instance()->scrollAmount.x;
//		if (winRight || winLeft) {
//
//			stuckWindowTimer = STRUCK_WINDOW_TIMER;
//			Crash({ winRight ? 1.0f : -1.0f , 0.0f });
//			SuperiorityGauge::Instance()->AddPlayerGauge(10);
//
//		}
//		// ウィンドウ上下
//		bool winTop = pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
//		bool winBottom = windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y;
//		if (winBottom || winTop) {
//
//			stuckWindowTimer = STRUCK_WINDOW_TIMER;
//			Crash({ 0.0f,winBottom ? 1.0f : -1.0f });
//			SuperiorityGauge::Instance()->AddPlayerGauge(10);
//
//		}
//
//	}
//
//	prevIntersectedLine = intersectedBuff;
//
//	if (0 < stuckWindowTimer) {
//
//		isHitMapChip = false;
//		--stuckWindowTimer;
//
//	}
//
//}

bool Boss::Appear()
{
	if (20 <= moveTimer)
	{
		return true;
	}

	if (SCALE.x < size.x && SCALE.y < size.y)
	{
		float time = 30.0f;
		size -= initScale / time;
	}
	else
	{
		if (!initPaticleFlag)
		{
			Vec2<float>radian(cosf(Angle::ConvertToRadian(0.0f)), sinf(Angle::ConvertToRadian(0.0f)));
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(90.0f)), sinf(Angle::ConvertToRadian(90.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(180.0f)), sinf(Angle::ConvertToRadian(180.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(270.0f)), sinf(Angle::ConvertToRadian(270.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);
			initPaticleFlag = true;
		}

		size = { 80.0f,80.0f };
		++moveTimer;
	}

	return false;
}
