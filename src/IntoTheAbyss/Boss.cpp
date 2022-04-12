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
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"
#include"BossPatternSwing.h"
#include"BossBulletManager.h"
#include"CrashMgr.h"

#include"ParticleMgr.h"

#include"DebugParameter.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;


	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();
	bossPattern[2] = std::make_unique<BossPatternSwing>();


	//パターンに渡すデータの初期化
	patternData.moveVel = &moveVel;
	patternData.stuckWindowTimer = &stuckWindowTimer;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;

	bulletHitBox = std::make_unique<SphereCollision>();
	bulletHitBox->center = &pos;
	bulletHitBox->radius = 40.0f;

	//Init();
}

void Boss::Init()
{
	pos = {};
	scale = {};
	vel = {};
	moveVel = {};
	crashDevice.Init();
}

void Boss::Generate(const Vec2<float> &generatePos)
{

	/*===== 生成処理 =====*/

	pos = generatePos;
	initScale = SCALE * 5.0f;
	scale = initScale;
	vel = { OFFSET_VEL,0 };
	moveVel = { OFFSET_VEL,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;
	areaHitBox.size = scale;
	allowToMoveFlag = false;
	moveTimer = 0;
	initPaticleFlag = false;
}

#include"Camera.h"
void Boss::Update()
{
	/*===== 更新処理 =====*/
	crashDevice.Update();

	// 前フレームの座標を保存
	prevPos = pos;

	if (80.0f < scale.x && 80.0f < scale.y)
	{
		float time = 30.0f;
		scale -= initScale / time;
	}

	if (scale.x <= 80.0f && scale.y <= 80.0f)
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

		scale = { 80.0f,80.0f };
		++moveTimer;
	}
	if (20 <= moveTimer)
	{
		allowToMoveFlag = true;
	}


	//通常サイズになるまで動けない
	if (scale.x <= SCALE.x && scale.y <= SCALE.y && !readyToStartRoundEffectFlag)
	{
		if (Camera::Instance()->Active())
		{
			moveVel = { 0,0 };
			return;
		}

		for (int i = 0; i < patternData.bulltData.size(); ++i)
		{
			if (patternData.bulltData[i].initFlag)
			{
				patternData.bulltData[i].Reset();
			}
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
		else if (isSwingNow && SwingMgr::Instance()->isSwingPlayer) {

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

			//ボスのAI-----------------------
			AiPattern();
			//ボスのAI-----------------------




			//ボスの挙動
			if (bossPatternNow != oldBossPattern)
			{
				bossPattern[bossPatternNow]->Init();
			}
			bossPattern[bossPatternNow]->Update(&patternData);
			oldBossPattern = bossPatternNow;
			if (UsersInput::Instance()->Input(DIK_0)) {
				vel.x = OFFSET_VEL * 5.0f;
			}

			//ボスの弾
			for (int i = 0; i < patternData.bulltData.size(); ++i)
			{
				if (patternData.bulltData[i].initFlag)
				{
					BossBulletManager::Instance()->Generate(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
				}
			}
		}
		// 移動量の総量を求める。

		if (0 < afterSwingDelay) {

			// 振り回し直後の硬直が残っている場合は、慣性のみを移動量とする。
			vel = swingInertiaVec * swingInertia;

		}
		else {

			vel = moveVel + swingInertiaVec * swingInertia;

		}
	}


	BossBulletManager::Instance()->Update();

}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::Draw()
{
	/*===== 描画処理 =====*/
	BossBulletManager::Instance()->Draw();
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + crashDevice.GetShake();
	auto drawScale = scale * crashDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
		TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, crashDevice.GetFlashAlpha());
}

void Boss::CheckHit(const vector<vector<int>> &mapData, bool &isHitMapChip, const Vec2<float> &playerPos, const Vec2<float> &lineCenterPos)
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

			Vec2<bool>ext = { true,true };
			if (!isHitLeft && !isHitRight)ext.y = false;
			if (!isHitTop && !isHitBottom)ext.x = false;

			CrashMgr::Instance()->Crash(pos, crashDevice, ext);
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->swingDamageValue);
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
			CrashMgr::Instance()->Crash(pos, crashDevice, { false,true });
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
		}
		// ウィンドウ上下
		if (windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y || pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			CrashMgr::Instance()->Crash(pos, crashDevice, { true,false });
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);

		}


	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}

void Boss::AiPattern()
{
	//120Flameごとにどの行動パターンを選択するか決める

	++patternTimer;
	if (120 <= patternTimer)
	{
		//プレイヤーとボスとの方向ベクトル確認
		Vec2<float>dir = SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos;
		float distance = dir.y - (dir.y * -1.0f);

		bool allowToSwingFlag = false;
		if (SWING_DISTANCE_DEADLINE <= fabs(distance))
		{
			allowToSwingFlag = true;
		}


		//次にどのパターンを選択するか
		//一部の条件でパターンを選択しないようにする
		while (1)
		{
			int randomPattern = KuroFunc::GetRand(BOSS_PATTERN_MAX - 1);

			//振り回し許可なしで振り回そうとしたらやり直し
			if (!allowToSwingFlag && randomPattern == BOSS_PATTERN_SWING)
			{
				continue;
			}
			bossPatternNow = static_cast<E_BossPattern>(randomPattern);
			break;
		}

		patternTimer = 0;
	}
}
