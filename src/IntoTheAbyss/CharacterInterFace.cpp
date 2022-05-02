#include "CharacterInterFace.h"
#include"CrashMgr.h"
#include"WinApp.h"
#include"SuperiorityGauge.h"
#include"ScrollMgr.h"
#include"DebugParameter.h"
#include"StunEffect.h"
#include"FaceIcon.h"
#include "ResultTransfer.h"
#include "AfterImage.h"
#include "CrashEffectMgr.h"
#include "Stamina.h"

void CharacterInterFace::SwingUpdate()
{

	/*===== 振り回し中に呼ばれる処理 =====*/

	// 角度に加算する量を更新。
	addSwingAngle += ADD_SWING_ANGLE * addSwingRate;

	// 振り回しの経過時間を設定。
	++swingTimer;

	// 限界を超えていたら修正。
	if (MAX_SWING_ANGLE < addSwingAngle) {

		addSwingAngle = MAX_SWING_ANGLE;

	}

	// 現在の角度を求める。
	float nowAngle = atan2f(GetPartnerPos().y - pos.y, GetPartnerPos().x - pos.x);

	// 角度が-だったら矯正する。
	if (nowAngle < 0) {

		float angleBuff = 3.14f - fabs(nowAngle);

		nowAngle = 3.14f + angleBuff;

	}

	// 現在の角度に角度の加算量を足す。
	if (isSwingClockWise) {

		// 時計回りだったら
		nowAngle += addSwingAngle;

	}
	else {

		// 反時計回りだったら
		nowAngle -= addSwingAngle;

	}

	// この角度を現在のベクトルとして使用する。
	nowSwingVec = { cosf(nowAngle), sinf(nowAngle) };

	// 相方をここで振り回してしまう！
	float partnerDistance = (pos - partner.lock()->pos).Length();

	partner.lock()->pos = pos + nowSwingVec * partnerDistance;

	// 現在のベクトルと最終目標のベクトルで外積の左右判定を行い、通り越していたら振り回しを終える。 負の値が左(反時計回り)、正の値が右(時計回り)。
	float crossResult = nowSwingVec.Cross(swingTargetVec);

	// [最初が時計回り] 且つ [現在が反時計回り] だったら
	if (isSwingClockWise && crossResult < 0 && 15 < swingTimer) {

		// パートナーに慣性を与える。
		partner.lock()->vel = (partner.lock()->pos - partner.lock()->prevPos) * 0.5f;

		// 振り回し終わり！
		FinishSwing();
	}
	// [最初が反時計回り] 且つ [現在が時計回り] だったら
	if (!isSwingClockWise && 0 < crossResult && 15 < swingTimer) {

		// パートナーに慣性を与える。
		partner.lock()->vel = (partner.lock()->pos - partner.lock()->prevPos) * 0.5f;

		// 振り回し終わり！
		FinishSwing();
	}


	// 紐つかみ状態を削除。 この新仕様の振り回しで行くとしたらこの変数はいらないので削除する。
	//isHold = false;

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
	//SuperiorityGauge::Instance()->AddGauge(team, -10.0f);
	stagingDevice.StopSpin();

	OnCrash();

	if (stanTimer)return;	//スタン中ならダメージによる顔変更なし
	static const int DAMAGE_TOTAL_TIME = 90;

	damageTimer = DAMAGE_TOTAL_TIME;
	FaceIcon::Instance()->Change(team, FACE_STATUS::DAMAGE);
}

void CharacterInterFace::CrashUpdate()
{
	//振り回されている
	if (partner.lock()->nowSwing)
	{
		//一定量振り回した後
		static const float CRASH_SWING_RATE = 0.15f;
		if (ADD_SWING_ANGLE * 2.0f < partner.lock()->addSwingAngle)
		{
			Vec2<float>vec = { 0,0 };
			if (mapChipHit[LEFT])vec.x = -1.0f;
			else if (mapChipHit[RIGHT])vec.x = 1.0f;
			if (mapChipHit[TOP])vec.y = -1.0f;
			else if (mapChipHit[BOTTOM])vec.y = 1.0f;

			Crash(vec);
		}

		partner.lock()->FinishSwing();
	}
}

void CharacterInterFace::SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise)
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.13f);
	}

	//振り回し処理が既に走っている場合は、重ねて振り回せない
	if (partner.lock()->nowSwing || nowSwing)return;

	partner.lock()->OnSwinged();

	AudioApp::Instance()->PlayWave(SE);

	// 目標地点のベクトルを保存。
	swingTargetVec = SwingTargetVec;

	// 現在のフレームの相方とのベクトルを求める。
	nowSwingVec = GetPartnerPos() - pos;
	nowSwingVec.Normalize();

	float pi45 = Angle::ConvertToRadian(42);

	// 外積の左右判定によりどちら側に振り回すかを判断する。 負の値が左、正の値が右。
	float crossResult = nowSwingVec.Cross(swingTargetVec);
	if (!IsClockWise) {

		// マイナスなので左(反時計回り)。
		isSwingClockWise = false;

		// 時計回りの方に目標地点ベクトルを修正。
		float targetAngle = atan2f(SwingTargetVec.y, SwingTargetVec.x);

		targetAngle += pi45;

		//swingTargetVec = {cosf(targetAngle),sinf(targetAngle)};

	}
	else if (IsClockWise) {

		// プラスなので右(時計回り)。
		isSwingClockWise = true;

		// 時計回りの方に目標地点ベクトルを修正。
		float targetAngle = atan2f(SwingTargetVec.y, SwingTargetVec.x);

		targetAngle -= pi45;

		//swingTargetVec = {cosf(targetAngle),sinf(targetAngle)};

	}

	// 角度への加算量を初期化。
	addSwingAngle = 0;

	//振り回しフラグの有効化
	nowSwing = true;

	swingTimer = 0;

	partner.lock()->stagingDevice.StartSpin(isSwingClockWise);

	// 角度に加算する量の割合を決める。
	float partnerDistance = (pos - partner.lock()->pos).Length();

	const float MAX_LENGTH = 150.0f;

	// 距離が規定値以上だったら1.0fを代入する。
	if (MAX_LENGTH < partnerDistance) {

		addSwingRate = 1.0f;

	}
	else {

		// 割合を求める。
		addSwingRate = (partnerDistance / MAX_LENGTH) * 2.0f + 1.0f;

	}

}

void CharacterInterFace::SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine)
{
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_BOTTOM) isHitBottom = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_RIGHT) isHitRight = true;
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos)
{
	pos = GeneratePos;
	vel = { 0,0 };

	stackWindowTimer = 0;

	nowSwing = false;

	stackMapChip = false;
	for (auto& flg : mapChipHit)flg = false;

	stagingDevice.Init();

	addLineLength = 0.0f;

	//登場演出のため最初は動けない
	canMove = false;
	//登場演出のため最初は当たり判定とらない
	hitCheck = false;

	bulletMgr.Init();

	stanTimer = 0;

	stagingDevice.Init();

	OnInit();

	isHold = false;

	advancedEntrySwingTimer = 0;
	isAdvancedEntrySwing = false;

	damageTimer = 0;

	static const int RETICLE_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_enemy.png") };
	int team = GetWhichTeam();

	CWSwingSegmentMgr.Setting(true, rbHandle, arrowHandle, lineHandle, RETICLE_GRAPH[team]);
	CCWSwingSegmentMgr.Setting(false, lbHandle, arrowHandle, lineHandle, RETICLE_GRAPH[team]);
	isInputSwingRB = false;

	addSwingRate = 0;

	gaugeReturnTimer = 0;

	// 各キャラによってスタミナゲージのデフォルト量を決定。
	staminaGauge = std::make_shared<StaminaMgr>();
	staminaGauge->Init();

}

void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{

	//スタン状態更新
	if (stanTimer)
	{
		stanTimer--;
		//スタン終了
		if (stanTimer <= 0)
		{
			canMove = true;
			FaceIcon::Instance()->Change(team, FACE_STATUS::DEFAULT);
			SuperiorityGauge::Instance()->Init();
			OnBreakFinish();
		}
	}
	//ダメージ状態更新（顔制御）
	if (damageTimer)
	{
		damageTimer--;
		if (damageTimer <= 0)
		{
			FaceIcon::Instance()->Change(team, FACE_STATUS::DEFAULT);
		}
	}

	//自身が振り回し中
	if (nowSwing)
	{
		SwingUpdate();

	}

	prevPos = pos;


	if (SuperiorityGauge::Instance()->GetGaugeData(team).gaugeValue)
	{
		OnUpdate(MapData);
	}

	OnUpdateNoRelatedSwing();

	//ウィンドウの引っかかっている判定のタイマー更新
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
	}

	//弾の更新
	bulletMgr.Update();

	//引っかかっている
	if (stackMapChip)
	{
		CrashUpdate();
	}

	//マップチップとの当たり判定があったときの処理呼び出し
	for (int i = 0; i < HIT_DIR_NUM; ++i)
	{
		if (mapChipHit[i])
		{
			OnHitMapChip((HIT_DIR)i);
			mapChipHit[i] = false;
		}
	}

	//演出補助更新
	stagingDevice.Update();

	// 振り回し可視化用のクラスを更新。
	/*if (nowSwing) {
		CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), !isInputSwingRB, true, MapData);
		CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), isInputSwingRB, true, MapData);
	}
	else {
		CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), false, false, MapData);
		CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), false, false, MapData);
	}*/

	// 体幹ゲージをデフォルトに戻すタイマーが0だったら
	if (gaugeReturnTimer <= 0) {

		static const int DEF_GAUGE = 50;
		static const float RETURN_AMOUNT = 0.1f;

		// 右側のチームで、右側のゲージ量がデフォルト以下だったら右側のゲージを足す。
		float nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).gaugeValue;
		if (GetWhichTeam() == RIGHT_TEAM && nowGaugeValue < DEF_GAUGE) {

			SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, RETURN_AMOUNT);

			// 足す過程で限界を超えないようにする。
			nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).gaugeValue;
			if (DEF_GAUGE < nowGaugeValue) {

				SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, DEF_GAUGE - nowGaugeValue);

			}

		}

		// 左側のチームで、左側のゲージ量がデフォルト以下だったら左側のゲージを足す。
		nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).gaugeValue;
		if (GetWhichTeam() == LEFT_TEAM && nowGaugeValue < DEF_GAUGE) {

			SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, RETURN_AMOUNT);

			// 足す過程で限界を超えないようにする。
			nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).gaugeValue;
			if (DEF_GAUGE < nowGaugeValue) {

				SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, DEF_GAUGE - nowGaugeValue);

			}

		}

	}
	else {

		--gaugeReturnTimer;

	}

	staminaGauge->Update();

}

#include "DrawFunc.h"
#include"TexHandleMgr.h"
void CharacterInterFace::Draw()
{
	// 残像を描画
	CWSwingSegmentMgr.Draw(team);
	CCWSwingSegmentMgr.Draw(team);
	OnDraw();
	bulletMgr.Draw();
}

void CharacterInterFace::DrawUI()
{
	//握力ゲージ描画
	static Color GAUGE_COLOR[TEAM_NUM] = { Color(47,255,139,255),Color(239,1,144,255) };
	static Color GAUGE_SHADOW_COLOR[TEAM_NUM] = { Color(41,166,150,255),Color(162,27,108,255) };

	staminaGauge->Draw(pos);

	OnDrawUI();
}

void CharacterInterFace::Break()
{
	static const int STAN_TOTAL_TIME = 120;
	stanTimer = STAN_TOTAL_TIME + StunEffect::GetTotalTime();
	OnBreak();
}

void CharacterInterFace::Damage()
{
	static const int DAMAGE_TOTAL_TIME = 90;
	stagingDevice.Flash(DAMAGE_TOTAL_TIME, 0.7f);
	stagingDevice.Shake(DAMAGE_TOTAL_TIME / 2, 2, 3.0f);

	if (stanTimer)return;	//スタン中ならダメージによる顔変更なし

	damageTimer = DAMAGE_TOTAL_TIME;
	FaceIcon::Instance()->Change(team, FACE_STATUS::DAMAGE);
}

#include"Intersected.h"
#include"MapChipCollider.h"
#include "StaminaItemMgr.h"
void CharacterInterFace::CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{

	/*===== 当たり判定 =====*/

	//当たり判定
	if (!hitCheck)return;

	// 壁はさみからの当たり判定無効化する。
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
		stackMapChip = false;

		return;

	}


	//各キャラの特有の当たり判定
	//OnCheckHit(MapData, LineCenterPos);

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
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * size, size, MapData, true);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;

	// 左上
	Vec2<float> velPosBuff = pos - size + Vec2<float>(velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - size + Vec2<float>(velOffset, velOffset), {}, {}, MapData, true);
	pos = velPosBuff + size - Vec2<float>(velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 右上
	velPosBuff = pos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset), {}, {}, MapData, true);
	pos = velPosBuff - Vec2<float>(size.x, -size.y) - Vec2<float>(-velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 右下
	velPosBuff = pos + size + Vec2<float>(-velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + size + Vec2<float>(-velOffset, -velOffset), {}, {}, MapData, true);
	pos = velPosBuff - size - Vec2<float>(-velOffset, -velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// 左下
	velPosBuff = pos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset), {}, {}, MapData, true);
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
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// 下方向
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// 右方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// 下方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);


	//マップチップと引っかかっているフラグを下ろす
	stackMapChip = false;

	// マップチップと当たっていたら
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

		// マップチップと当たっている場所によって、引っかかっているかどうかを調べる。

		// 相手との角度
		const auto partnerPos = partner.lock()->pos;
		float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);

		// 角度が-だったら180度以上
		if (partnerAngle < 0) {

			float angleBuff = 3.14f - fabs(partnerAngle);

			partnerAngle = 3.14f + angleBuff;

		}

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




		// 一定以下だったらダメージを与えない。
		if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 2.0f) {

		}
		else if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 4.0f) {

			// 振り回されている状態だったら、シェイクを発生させて振り回し状態を解除する。
			Vec2<float>vec = { 0,0 };
			if (partner.lock()->GetNowSwing()) {

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

				/*Crash(vec);

				SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);*/
				partner.lock()->FinishSwing();

			}

			// ゲージがデフォルトに戻るまでのタイマーを更新。
			gaugeReturnTimer = GAUGE_RETURN_TIMER;

		}
		else {

			// 振り回されている状態だったら、シェイクを発生させて振り回し状態を解除する。
			Vec2<float>vec = { 0,0 };
			if (partner.lock()->GetNowSwing()) {

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

				Crash(vec);
				//CrashMgr::Instance()->Crash(pos, crashDevice, ext);
				//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->swingDamageValue);
				SuperiorityGauge::Instance()->AddGauge(team, -10);
				partner.lock()->FinishSwing();

				// チームに応じてクラッシュ数を加算する変数を変える。
				if (team == WHICH_TEAM::LEFT_TEAM) {
					++ResultTransfer::Instance()->leftCrashCount;
				}
				else {
					++ResultTransfer::Instance()->rightCrashCount;
				}

				// クラッシュ演出を追加。
				CrashEffectMgr::Instance()->Generate(pos);

				// アイテムを生成する。
				StaminaItem::CHARA_ID charaID;
				if (team == WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::LEFT;
				if (team != WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::RIGHT;
				StaminaItemMgr::Instance()->Generate(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &partner.lock()->pos, charaID);

			}

			// ゲージがデフォルトに戻るまでのタイマーを更新。
			gaugeReturnTimer = GAUGE_RETURN_TIMER;

		}

		// 壁はさみの判定
		//if (stackWindowTimer <= 0) {

		//	Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		//	windowSize *= Vec2<int>(2, 2);

		//	// ウィンドウ左右
		//	float windowOffset = 100.0f;
		//	bool winLeft = pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x <= -windowOffset;
		//	bool winRight = windowSize.x + windowOffset <= pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x;
		//	if (winRight || winLeft) {

		//		stackWindowTimer = STACK_WINDOW_TIMER;


		//		//CrashMgr::Instance()->Crash(pos, crashDevice, { false,true });
		//		//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
		//		SuperiorityGauge::Instance()->AddGauge(team, -20);
		//		Crash({ winRight ? 1.0f : -1.0f , 0.0f });

		//		// チームに応じてクラッシュ数を加算する変数を変える。
		//		if (team == WHICH_TEAM::LEFT_TEAM) {
		//			++ResultTransfer::Instance()->leftCrashCount;
		//		}
		//		else {
		//			++ResultTransfer::Instance()->rightCrashCount;
		//		}

		//		// クラッシュ演出を追加。
		//		CrashEffectMgr::Instance()->Generate(pos);
		//	}
		//	// ウィンドウ上下
		//	bool winTop = pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y <= -windowOffset;
		//	bool winBottom = windowSize.y + windowOffset <= pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y;
		//	if (winBottom || winTop) {

		//		stackWindowTimer = STACK_WINDOW_TIMER;

		//		//CrashMgr::Instance()->Crash(pos, crashDevice, { true,false });
		//		//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
		//		SuperiorityGauge::Instance()->AddGauge(team, -20);
		//		Crash({ 0.0f,winBottom ? 1.0f : -1.0f });

		//		// チームに応じてクラッシュ数を加算する変数を変える。
		//		if (team == WHICH_TEAM::LEFT_TEAM) {
		//			++ResultTransfer::Instance()->leftCrashCount;
		//		}
		//		else {
		//			++ResultTransfer::Instance()->rightCrashCount;
		//		}

		//		// クラッシュ演出を追加。
		//		CrashEffectMgr::Instance()->Generate(pos);
		//	}
		//}

	}

	//prevIntersectedLine = intersectedBuff;


}

void CharacterInterFace::FinishSwing()
{
	nowSwing = false;
	partner.lock()->stagingDevice.StopSpin();
	partner.lock()->OnSwingedFinish();

}

