#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include "UsersInput.h"
#include "SuperiorityGauge.h"
#include "SwingMgr.h"
#include "SuperiorityGauge.h"
#include "SlowMgr.h"
#include "StunEffect.h"

#include"TexHandleMgr.h"
#include"CrashMgr.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;
	Init();
}

void Boss::Init()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};
	crashDevice.Init();

}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== 生成処理 =====*/

	pos = generatePos;
	scale = SCALE;
	vel = { OFFSET_VEL,0 };
	moveVel = { OFFSET_VEL,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;
	areaHitBox.size = scale;
}

#include"Camera.h"
void Boss::Update()
{
	/*===== 更新処理 =====*/
	crashDevice.Update();

	// 前フレームの座標を保存
	prevPos = pos;

	if (Camera::Instance()->Active())
	{
		moveVel = { 0,0 };
		return;
	}

	// 慣性を更新。
	if (0 < swingInertia) {
		swingInertia -= swingInertia / 5.0f;
	}

	// 振り回し直後の硬直のタイマーを更新。
	if (0 < afterSwingDelay) --afterSwingDelay;




	// [振り回し中か振り回され中だったら] 更新処理を行わない。　　臨時の実装です。
	bool isSwingNow = SwingMgr::Instance()->isSwingBoss || SwingMgr::Instance()->isSwingPlayer;

	// [硬直中] [スタン演出中] は動かさない
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// 何もしない。
	}
	else if (isSwingNow) {

		// 振り回し中だったら。

		pos = SwingMgr::Instance()->playerPos + SwingMgr::Instance()->easingNowVec * SwingMgr::Instance()->lineLength;

		// ちょうど終わった瞬間の場合、慣性関係の変数を更新する。
		if (1.0f <= SwingMgr::Instance()->easingTimer) {

			swingInertiaVec = (pos - prevPos).GetNormal();
			swingInertia = (prevPos - pos).Length();
			afterSwingDelay = AFTER_SWING_DELAY;

		}

	}
	else {

		static const int PULL_SPAN_MIN = 30;
		static const int PULL_SPAN_MAX = 70;
		static int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
		static int PULL_TIMER = 0;
		static Vec2<float>ACCEL = { 0.0f,0.0f };	//加速度
		static const float PULL_POWER_MIN = 15.0f;
		static const float PULL_POWER_MAX = 25.0f;

		if (PULL_TIMER < PULL_SPAN)
		{
			PULL_TIMER++;
			if (PULL_SPAN <= PULL_TIMER)
			{
				PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
				PULL_TIMER = 0;

				auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
				auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
				ACCEL.x = cos(rad) * power * 1.6f;
				ACCEL.y = sin(rad) * power;
			}
		}
		moveVel.x = KuroMath::Lerp(moveVel.x, OFFSET_VEL, 0.1f);
		moveVel.y = KuroMath::Lerp(moveVel.y, 0.0f, 0.1f);
		moveVel += ACCEL;

		ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

		if (UsersInput::Instance()->Input(DIK_0)) {
			vel.x = OFFSET_VEL * 5.0f;
		}

	}

	if (0 < stuckWindowTimer) --stuckWindowTimer;


	// 移動量の総量を求める。

	if (0 < afterSwingDelay) {

		// 振り回し直後の硬直が残っている場合は、慣性のみを移動量とする。
		vel = swingInertiaVec * swingInertia;

	}
	else {

		vel = moveVel + swingInertiaVec * swingInertia;

	}

}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::Draw()
{

	/*===== 描画処理 =====*/

	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + crashDevice.GetShake();
	static auto WHITE_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color());
	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - scale), ScrollMgr::Instance()->Affect(drawPos + scale),
		TexHandleMgr::GetTexBuffer(graphHandle[dir]), WHITE_TEX, crashDevice.GetFlashAlpha());
}

void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos, const Vec2<float>& lineCenterPos)
{

	/*===== 当たり判定 =====*/

	// ボスがマップチップと当たっているかのフラグ
	INTERSECTED_LINE intersectedBuff = {};

	// マップチップ目線でどこに当たったか
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;

	bool onGround = false;
	// 移動量を元にしたマップチップとの当たり判定を行う。
	Vec2<float> moveDir = pos - prevPos;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * scale, scale, onGround, mapData, false);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;

	// スケールを元にしたマップチップとの当たり判定を行う。
	float offset = 1.0f;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;


	// マップチップと当たっていたら
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

		// マップチップと当たっている場所によって、引っかかっているかどうかを調べる。

		// プレイヤーとの角度
		float playerAngle = atan2(playerPos.y - pos.y, playerPos.x - pos.x);
		playerAngle = fabs(playerAngle);

		if (isHitTop) {

			// マップチップの上にあたっていたということは、プレイヤーが下方向にいればOK！
			// 下方向の具体的な値は
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 2.35619f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_BOTTOM;

			}

		}

		if (isHitBottom) {

			// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
			// 上方向の具体的な値は
			const float MIN_ANGLE = 3.92699f;
			const float MAX_ANGLE = 5.49779f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_TOP;

			}

		}

		if (isHitRight) {

			// マップチップの右にあたっていたということは、プレイヤーが左方向にいればOK！
			// 左方向の具体的な値は
			const float MIN_ANGLE = 2.35619f;
			const float MAX_ANGLE = 3.92699f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_RIGHT;

			}

		}

		if (isHitLeft) {

			// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
			// 右方向の具体的な値は
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 5.49779f;

			// 角度がこの値の間だったら
			if (MAX_ANGLE <= playerAngle && playerAngle <= MIN_ANGLE) {

				// 引っかかっている。
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_LEFT;

			}

		}

		// 振り回されている状態だったら、シェイクを発生させて振り回し状態を解除する。
		if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {

			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);
			SwingMgr::Instance()->isSwingPlayer = false;

		}

	}

	// マップチップにあたっている状態で画面外に出たら
	if (isHitMapChip) {

		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// ボスとプレイヤーの距離
		float distanceX = fabs(lineCenterPos.x - pos.x);
		float disntaceY = fabs(lineCenterPos.y - pos.y);

		// ウィンドウ左右
		if (windowSize.x <= pos.x + scale.x - ScrollMgr::Instance()->scrollAmount.x || pos.x - scale.x - ScrollMgr::Instance()->scrollAmount.x <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}
		// ウィンドウ上下
		if (windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y || pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}

	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}
