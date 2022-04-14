#include "CharacterInterFace.h"
#include"CrashMgr.h"
#include"WinApp.h"
#include"SuperiorityGauge.h"
#include"ScrollMgr.h"

void CharacterInterFace::SwingUpdate()
{
	static const float ADD_EASINGTIMER_MINIMUM = 0.025f;	// イージングタイマーに加算する量 最小値
	static const float ADD_EASINGTIMER_MAX = 0.05f;	// イージングタイマーに加算する量 最大値

	// イージング量を求める。
	float easingChange = ADD_EASINGTIMER_MAX - ADD_EASINGTIMER_MINIMUM;
	float addEasingTimer = ADD_EASINGTIMER_MINIMUM + (fabs(fabs(swingEndVec.y) - fabs(swingStartVec.y)) * easingChange);

	// [補正]X軸のベクトルが0だったら強制的に0.01fにする。
	if (fabs(swingStartVec.x) <= 0.001f) swingStartVec.x = 0.001f;

	Vec2<float>easeNowVec = { 0,0 };

	// イージングタイマーを更新する。
	if (swingEaseRate < 1.0f) {

		// イージングタイマーを更新する。
		swingEaseRate += addEasingTimer;

		// 限界値を超えないようにする。
		if (1.0f <= swingEaseRate) swingEaseRate = 1.0f;

		// イージングタイマーから求められるイージング量を求める。
		auto easeAmount = KuroMath::Ease(Out, Cubic, swingEaseRate, 0.0f, 1.0f);

		// 現在のベクトルを求める。
		easeNowVec.y = swingStartVec.y + (easeAmount * (swingEndVec.y - swingStartVec.y));
		if (easeNowVec.y < 0) {
			easeNowVec.x = 1.0f + easeNowVec.y;
		}
		else {
			easeNowVec.x = 1.0f - easeNowVec.y;
		}
		easeNowVec.Normalize();
	}

	const float lineLength = LINE_LENGTH * 2 + addLineLength + partner.lock()->addLineLength;
	partner.lock()->pos = this->pos + easeNowVec * lineLength;

	// イージングタイマーが限界を超えたらフラグを折る。
	if (1.0f <= swingEaseRate) {
		nowSwing = false;
		swingInertiaVec = (pos - prevPos).GetNormal();
		swingInertia = (prevPos - pos).Length();
		afterSwingDelay = AFTER_SWING_DELAY;
	}
}

void CharacterInterFace::Crash(const Vec2<float>& MyVec)
{
	Vec2<bool>ext = { true,true };
	if (MyVec.x == 0.0f)ext.y = false;
	if (MyVec.y == 0.0f)ext.x = false;

	Vec2<float>smokeVec;
	smokeVec = { 0,0 };

	if (0.0f < MyVec.x)smokeVec.x = -1.0f;
	else if (MyVec.x < 0.0f)smokeVec.x = 1.0f;
	if (0.0f < MyVec.y)smokeVec.y = -1.0f;
	else if (MyVec.y < 0.0f)smokeVec.y = 1.0f;

	CrashMgr::Instance()->Crash(pos, stagingDevice, ext, smokeVec);
	SuperiorityGauge::Instance()->AddPlayerGauge(10.0f);
}

void CharacterInterFace::CrashUpdate()
{
	//振り回されている
	if (partner.lock()->nowSwing)
	{
		//一定量振り回した後
		static const float CRASH_SWING_RATE = 0.15f;
		if (CRASH_SWING_RATE < partner.lock()->swingEaseRate)
		{
			Vec2<float>vec = { 0,0 };
			if (mapChipHit[LEFT])vec.x = -1.0f;
			else if (mapChipHit[RIGHT])vec.x = 1.0f;
			if (mapChipHit[TOP])vec.y = -1.0f;
			else if (mapChipHit[BOTTOM])vec.y = 1.0f;

			Crash(vec);
		}

		partner.lock()->nowSwing = false;
	}

	// マップチップにあたっている状態で画面外に出たら
	if (stuckWindowTimer <= 0)
	{
		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// ウィンドウ左右
		bool winLeft = pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
		bool winRight = windowSize.x <= pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x;
		if (winRight || winLeft) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ winRight ? 1.0f : -1.0f , 0.0f });
		}

		// ウィンドウ上下
		bool winTop = pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
		bool winBottom = windowSize.y <= pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y;
		if (winBottom || winTop) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ 0.0f,winBottom ? 1.0f : -1.0f });
		}
	}
}

void CharacterInterFace::SwingPartner()
{
	//振り回し処理が既に走っている場合は、重ねて振り回せない
	if (partner.lock()->nowSwing || nowSwing)return;

	//相手とのベクトル取得
	Vec2<float>dir = GetPartnerPos() - pos;
	dir.Normalize();

	//振り回しのデッドライン
	static const float SWING_DEAD_LINE = 0.3f;
	if (fabs(dir.y) <= SWING_DEAD_LINE)return;

	//振り回しの開始ベクトル
	swingStartVec = dir;
	// [補正]X軸のベクトルが0だったら強制的に0.01fにする。
	if (fabs(swingStartVec.x) <= 0.001f) swingStartVec.x = 0.001f;

	//振り回しの終了ベクトル
	swingEndVec = swingStartVec * Vec2<float>(1.0f, -1.0f);

	//振り回しイージング率リセット
	swingEaseRate = 0.0f;

	//振り回しフラグの有効化
	nowSwing = true;
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos, const Vec2<float>& Size)
{
	pos = GeneratePos;
	vel = { 0,0 };
	size = Size;

	stuckWindowTimer = 0;

	nowSwing = false;

	stackMapChip = false;
	for (auto& flg : mapChipHit)flg = false;

	stagingDevice.Init();

	addLineLength = 0.0f;

	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;

	OnInit();
}

void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{
	prevPos = pos;

	// 慣性を更新。
	if (0 < swingInertia) {
		swingInertia -= swingInertia / 5.0f;
	}

	// 振り回し直後の硬直のタイマーを更新。
	if (0 < afterSwingDelay) --afterSwingDelay;

	//自身が振り回し中
	if (nowSwing)
	{
		SwingUpdate();
	}
	//相手が振り回していないとき
	else if (!partner.lock()->nowSwing)
	{
		OnUpdate(MapData);
		//ウィンドウの引っかかっている判定のタイマー更新
		if (0 < stuckWindowTimer) {

			--stuckWindowTimer;
		}
	}


	//当たり判定
	CheckHit(MapData, LineCenterPos);

	//引っかかっている
	if (stackMapChip)
	{
		CrashUpdate();
	}

	//マップチップと引っかかっているフラグを下ろす
	stackMapChip = false;

	//マップチップとの当たり判定があったときの処理呼び出し
	for (int i = 0; i < HIT_DIR_NUM; ++i)
	{
		if (mapChipHit[i])
		{
			OnHitMapChip((HIT_DIR)i);
			mapChipHit[i] = false;
		}
	}
}

void CharacterInterFace::Draw()
{
	OnDraw();
}

#include"Intersected.h"
#include"MapChipCollider.h"
void CharacterInterFace::CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{
	/*===== 当たり判定 =====*/

	//各キャラの特有の当たり判定
	OnCheckHit(MapData, LineCenterPos);

	// ボスがマップチップと当たっているかのフラグ
	INTERSECTED_LINE intersectedBuff = {};

	// マップチップ目線でどこに当たったか
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;

	// 移動量を元にしたマップチップとの当たり判定を行う。
	Vec2<float> moveDir = pos - prevPos;
	float velOffset = 3.0f;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * size, size, MapData, false);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;

	// 左上
	Vec2<float> velPosBuff = pos - size + Vec2<float>(velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - size + Vec2<float>(velOffset, velOffset), {}, {}, MapData, false);
	pos = velPosBuff + size - Vec2<float>(velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 右上
	velPosBuff = pos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset), {}, {}, MapData, false);
	pos = velPosBuff - Vec2<float>(size.x, -size.y) - Vec2<float>(-velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 右下
	velPosBuff = pos + size + Vec2<float>(-velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + size + Vec2<float>(-velOffset, -velOffset), {}, {}, MapData, false);
	pos = velPosBuff - size - Vec2<float>(-velOffset, -velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 左下
	velPosBuff = pos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset), {}, {}, MapData, false);
	pos = velPosBuff - Vec2<float>(-size.x, size.y) - Vec2<float>(velOffset, -velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// スケールを元にしたマップチップとの当たり判定を行う。

	// 上方向
	float offset = 5.0f;
	Vec2<float> posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	// 下方向
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 右方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// 下方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;

	// マップチップと当たっていたら
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

		// マップチップと当たっている場所によって、引っかかっているかどうかを調べる。

		// 相手との角度
		const auto partnerPos = partner.lock()->pos;
		float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);
		partnerAngle = fabs(partnerAngle);

		if (isHitTop) {

			mapChipHit[BOTTOM] = true;

			// マップチップの上にあたっていたということは、プレイヤーが下方向にいればOK！
			// 下方向の具体的な値は
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 2.35619f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				stackMapChip = true;
				intersectedBuff = INTERSECTED_BOTTOM;

			}

		}

		if (isHitBottom) {

			mapChipHit[TOP] = true;

			// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
			// 上方向の具体的な値は
			const float MIN_ANGLE = 3.92699f;
			const float MAX_ANGLE = 5.49779f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				stackMapChip = true;
				intersectedBuff = INTERSECTED_TOP;

			}

		}

		if (isHitRight) {

			mapChipHit[LEFT] = true;

			// マップチップの右にあたっていたということは、プレイヤーが左方向にいればOK！
			// 左方向の具体的な値は
			const float MIN_ANGLE = 2.35619f;
			const float MAX_ANGLE = 3.92699f;

			// 角度がこの値の間だったら
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// 引っかかっている。
				stackMapChip = true;
				intersectedBuff = INTERSECTED_RIGHT;
			}

		}

		if (isHitLeft) {

			mapChipHit[RIGHT] = true;

			// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
			// 右方向の具体的な値は
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 5.49779f;

			// 角度がこの値の間だったら
			if (MAX_ANGLE <= partnerAngle || partnerAngle <= MIN_ANGLE) {

				// 引っかかっている。
				stackMapChip = true;
				intersectedBuff = INTERSECTED_LEFT;

			}
		}
	}
}

