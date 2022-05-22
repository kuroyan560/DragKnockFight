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
#include"CharacterManager.h"

const Color CharacterInterFace::TEAM_COLOR[TEAM_NUM] =
{
	Color(47,255,139,255),
	Color(239,1,144,255)
};

void CharacterInterFace::SwingUpdate()
{
	ADD_SWING_ANGLE = DebugParameter::Instance()->GetBossData().swingAngle;
	MAX_SWING_ANGLE = DebugParameter::Instance()->GetBossData().swingMax;

	/*===== 振り回し中に呼ばれる処理 =====*/

	// 角度に加算する量を更新。
	addSwingAngle += ADD_SWING_ANGLE * addSwingRate;

	// 振り回しの経過時間を設定。
	++swingTimer;

	// このタイミングでスタミナを消費する。
	if (swingTimer == 5) {
		//現状はこれで間に合わせる。
		if (CharacterManager::Instance()->Right()->GetCharacterName() == PLAYABLE_BOSS_0)
		{
			// スタミナを消費
			staminaGauge->ConsumesStamina(DebugParameter::Instance()->GetBossData().staminaSwing);
		}
		else
		{
			// スタミナを消費
			staminaGauge->ConsumesStamina(SWING_STAMINA);
		}
	}

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

void CharacterInterFace::Crash(const Vec2<float>& MyVec, const int& SmokeCol)
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

	CrashMgr::Instance()->Crash(pos, stagingDevice, ext, smokeVec, SmokeCol);
	//SuperiorityGauge::Instance()->AddGauge(team, -10.0f);
	stagingDevice.StopSpin();

	OnCrash();
	partner.lock()->OnPartnerCrash();

	if (stanTimer)return;	//スタン中ならダメージによる顔変更なし
	static const int DAMAGE_TOTAL_TIME = 90;

	damageTimer = DAMAGE_TOTAL_TIME;
	FaceIcon::Instance()->Change(team, FACE_STATUS::DAMAGE);
}

void CharacterInterFace::CrashUpdate()
{
	//振り回されている
	//if (partner.lock()->nowSwing)
	//{
	//	//一定量振り回した後
	//	static const float CRASH_SWING_RATE = 0.15f;
	//	if (ADD_SWING_ANGLE * 2.0f < partner.lock()->addSwingAngle)
	//	{
	//		Vec2<float>vec = { 0,0 };
	//		if (mapChipHit[LEFT])vec.x = -1.0f;
	//		else if (mapChipHit[RIGHT])vec.x = 1.0f;
	//		if (mapChipHit[TOP])vec.y = -1.0f;
	//		else if (mapChipHit[BOTTOM])vec.y = 1.0f;

	//		Crash(vec);
	//	}

	//	partner.lock()->FinishSwing();
	//}
}

void CharacterInterFace::SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise)
{

	// 振り回しの予測線を更新する際に使用する変数をセット。
	CWSwingSegmentMgr.SetCharaStartPos(pos);
	CCWSwingSegmentMgr.SetCharaStartPos(pos);

	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav", 0.13f);


	//振り回し処理が既に走っている場合は、重ねて振り回せない
	if (partner.lock()->nowSwing || nowSwing)return;

	//パイロット切り離し中なら使えない
	if (isPilotDetached)return;

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

void CharacterInterFace::SetPilotDetachedFlg(const bool& Flg)
{
	//パイロットでなくする
	return;

	if (isPilotDetached == Flg)return;
	//パイロット切り離しには最低でもスタミナバー１個分必要
	if (Flg && !staminaGauge->CheckCanAction(1))return;

	static const float PILOT_RETURN_DIST_BASE = 128.0f;
	static const int PILOT_RETURN_TOTAL_TIME_BASE = 5;	// PILOT_RETURN_DIST_BASE の距離をこのフレームで帰る速さ

	//切り離した瞬間
	if (Flg)
	{
		if (!IsPilotOutSide())
		{
			pilotPos = pos;	//ロボの座標からスタート
		}
		OnPilotLeave();
	}
	//ロボに戻る処理の初期化
	else
	{
		pilotReturnTimer = 0;
		pilotReturnStartPos = pilotPos;
		pilotReturnTotalTime = max(PILOT_RETURN_TOTAL_TIME_BASE * (pilotPos.Distance(pos) / PILOT_RETURN_DIST_BASE), 1);
	}
	isPilotDetached = Flg;
}

void CharacterInterFace::SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine, Vec2<int>& hitChipIndex, const Vec2<int>& hitChipIndexBuff)
{
	auto mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();
	(*mapChipDrawData)[hitChipIndexBuff.y][hitChipIndexBuff.x].shocked = 1.0f;

	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_TOP)isHitTop = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_BOTTOM) isHitBottom = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine != INTERSECTED_LINE::INTERSECTED_NONE) hitChipIndex = hitChipIndexBuff;
}

void CharacterInterFace::Appear()
{
	if (CompleteAppear())
	{
		return;
	}

	//サイズが1.0fになるまで動かない
	if (1.0f < appearExtRate.x && 1.0f < appearExtRate.y)
	{
		float time = 30.0f;
		appearExtRate.x -= INIT_SIZE / time;
		appearExtRate.y -= INIT_SIZE / time;
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

		appearExtRate = { 1.0f,1.0f };
		++moveTimer;
	}
}

void CharacterInterFace::InitSwingLineSegmetn()
{
	CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Init();
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos, const bool& Appear)
{
	if (pilotGraph != -1)
	{
		pilotSize = TexHandleMgr::GetTexBuffer(pilotGraph)->GetGraphSize().Float();
	}
	if (Appear)appearExtRate = { INIT_SIZE,INIT_SIZE };
	else appearExtRate = { 1.0f,1.0f };
	initPaticleFlag = false;
	moveTimer = 0;

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
	elecTimer = 0;

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

	isPilotDetached = false;
	pilotReturnTimer = 0;
	pilotReturnTotalTime = 0;
	gaugeReturnTimer = 0;

	// 各キャラによってスタミナゲージのデフォルト量を決定する予定。
	//staminaGauge = std::make_shared<StaminaMgr>();
	staminaGauge->Init();

	// キャラに寄ってスタミナゲージの色を設定する。
	bool isLeft = GetWhichTeam() == WHICH_TEAM::LEFT_TEAM;
	Color innerColor = Color();
	Color outerColor = Color();
	if (isLeft) {

		// 緑
		outerColor = Color(0x29, 0xC9, 0xB4, 0xFF);
		innerColor = Color(0x2F, 0xFF, 0x8B, 0xFF);

	}
	else {

		// 赤
		outerColor = Color(0xA2, 0x1B, 0x6C, 0xFF);
		innerColor = Color(0xEF, 0x01, 0x90, 0xFF);

	}
	staminaGauge->SetColor(innerColor, outerColor);

	prevSwingFlag = false;


	bulletMgr.Init();
	barrage->Init();
	barrageDelayTimer = 0;

	healAuraEaseRate = 1.0f;
}

#include "SlowMgr.h"
void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos, const bool& isRoundStartEffect)
{

	if (team == WHICH_TEAM::RIGHT_TEAM && 0.8f <= SlowMgr::Instance()->slowAmount) {

		// 弾の更新処理
		bulletMgr.Update();

	}


	// 振り回し中だったら線分を更新する。
	if (nowSwing) {

		CWSwingSegmentMgr.UpdateSwing(pos);
		CCWSwingSegmentMgr.UpdateSwing(pos);

	}

	// 相手が振り回していたら、こちらの照準を消す。
	if (partner.lock()->GetNowSwing() || 0 < stanTimer || 0 < damageTimer) {
		CWSwingSegmentMgr.Init();
		CCWSwingSegmentMgr.Init();
	}

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

	// 棘にあたって一定時間動かないフラグが立っていたらリターン
	if (0 < elecTimer) {
		--elecTimer;
		return;
	}

	//自身が振り回し中
	if (nowSwing)
	{
		SwingUpdate();

	}

	//パイロット切り離し中
	if (isPilotDetached)
	{
		OnPilotControl();

		const auto vec = pilotPos - pos;
		if (PILOT_RANGE < vec.Length())
		{
			pilotPos = pos + vec.GetNormal() * PILOT_RANGE;
		}

		//スタミナ消費
		staminaGauge->ConsumesStaminaByGauge(1.0f);
		//if (!staminaGauge->ConsumesStaminaByGauge(0.5f))
		//スタミナが空っぽなら
		if (staminaGauge->emptyTrigger)
		{
			//強制的にパイロットを戻す
			SetPilotDetachedFlg(false);
		}
	}
	//パイロットが戻ってくる処理
	if (pilotReturnTimer < pilotReturnTotalTime)
	{
		pilotReturnTimer++;
		pilotPos = KuroMath::Ease(Out, Circ, pilotReturnTimer, pilotReturnTotalTime, pilotReturnStartPos, Vec2<float>(pos));

		//パイロットがロボにたどり着く
		if (pilotReturnTotalTime <= pilotReturnTimer)
		{
			OnPilotReturn();
		}
	}


	if (SuperiorityGauge::Instance()->GetGaugeData(team).gaugeValue)
	{
		MOVE_SPEED_PLAYER = DebugParameter::Instance()->playerData[0].playerSpeed;
		OnUpdate(MapData);
	}

	OnUpdateNoRelatedSwing();

	//ウィンドウの引っかかっている判定のタイマー更新
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
	}

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
	if (gaugeReturnTimer <= 0 && !GetNowBreak()) {

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

	if (team == WHICH_TEAM::LEFT_TEAM) {

		// ひだりのチーム(プレイヤー)だったら
		staminaAutoHealAmount = 1.5f;

	}
	else {

		// みぎのチーム(敵)だったら
		staminaAutoHealAmount = DebugParameter::Instance()->GetBossData().staminaHealAmount;

	}

	staminaGauge->Update(!isPilotDetached, pos, staminaAutoHealAmount);


	// 右のキャラだったら処理を行う。
	if (team == WHICH_TEAM::RIGHT_TEAM && !isRoundStartEffect) {

		// 弾幕の更新処理
		static const int BULLET_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/bullet_enemy.png");
		bool isEnd = barrage->Update(bulletMgr, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Left()->pos, BULLET_HANDLE);

		// 弾幕の更新が終わっていたら。
		if (isEnd) {

			// タイマーを更新。
			++barrageDelayTimer;

			// タイマーが規定値に達したら。
			if (BARRAGE_DELAY_TIMER <= barrageDelayTimer) {

				// 乱数を生成して、その乱数の弾幕をセットする。
				int random = KuroFunc::GetRand(6);

				switch (random)
				{
				case 0:

					barrage = std::make_unique<WhirlpoolBarrage>();

					break;

				case 1:

					barrage = std::make_unique<Whirlpool2WayBarrage>();

					break;

				case 2:

					barrage = std::make_unique<CircularBarrage>();

					break;

				case 3:

					barrage = std::make_unique<TargetShotBarrage>();

					break;

				case 4:

					barrage = std::make_unique<TargetShot3WayBarrage>();

					break;

				case 5:

					barrage = std::make_unique<ShotGunBarrage>();

					break;

				case 6:

					barrage = std::make_unique<WaveBarrage>();

					break;
				default:
					break;
				}

				barrage->Start();

			}

		}
		else {

			barrageDelayTimer = 0;

		}


	}


}

#include "DrawFunc.h"
#include"TexHandleMgr.h"
void CharacterInterFace::Draw(const bool& isRoundStartEffect)
{
	// 残像を描画
	if (!GetNowBreak()) {
		CWSwingSegmentMgr.Draw(team);
		CCWSwingSegmentMgr.Draw(team);
	}
	OnDraw(isRoundStartEffect);

	static int HEAL_GRAPH[TEAM_NUM] = { 0 };
	if (!HEAL_GRAPH[0])
	{
		TexHandleMgr::LoadDivGraph("resource/ChainCombat/healAura.png", TEAM_NUM, { TEAM_NUM,1 }, HEAL_GRAPH);
	}
	if (healAuraEaseRate < 1.0f)
	{
		healAuraEaseRate += 0.02f;
		if (1.0f < healAuraEaseRate)healAuraEaseRate = 1.0f;
	}
	float auraAlpha = KuroMath::Ease(In, Cubic, healAuraEaseRate, 1.0f, 0.0f);
	float exp = ScrollMgr::Instance()->zoom;
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(HEAL_GRAPH[team]), Color(1.0f, 1.0f, 1.0f, auraAlpha),
		{ 0.5f,0.5f }, { false,false }, AlphaBlendMode_Add);

	static const int LINE_GRAPH[TEAM_NUM] =
	{
		TexHandleMgr::LoadGraph("resource/ChainCombat/chain_player.png"),
		TexHandleMgr::LoadGraph("resource/ChainCombat/chain_enemy.png")
	};
	static const int CHAIN_THICKNESS = 4;

	//パイロット描画
	if (IsPilotOutSide() && pilotGraph != -1)
	{
		const auto pilotDrawPos = ScrollMgr::Instance()->Affect(pilotPos);
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(pos), ScrollMgr::Instance()->Affect(pilotPos),
			TexHandleMgr::GetTexBuffer(LINE_GRAPH[team]), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		const auto sizeHalf = pilotSize * ScrollMgr::Instance()->zoom / 2.0f;
		DrawFunc::DrawExtendGraph2D(pilotDrawPos - sizeHalf, pilotDrawPos + sizeHalf, TexHandleMgr::GetTexBuffer(pilotGraph), Color(), { pilotDrawMiror,false });
	}

	bulletMgr.Draw();
}

void CharacterInterFace::DrawUI()
{
	//握力ゲージ描画
	static Color GAUGE_COLOR[TEAM_NUM] = { Color(47,255,139,255),Color(239,1,144,255) };
	static Color GAUGE_SHADOW_COLOR[TEAM_NUM] = { Color(41,166,150,255),Color(162,27,108,255) };

	if (GetCharacterName() != PLAYABLE_BOSS_0)
	{
		staminaGauge->Draw(pos);
	}


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
#include <IntoTheAbyss/StageMgr.h>
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
	Vec2<int> hitChipIndex;
	Vec2<int> hitChipIndexBuff;		// 一次保存用

	// 移動量を元にしたマップチップとの当たり判定を行う。
	Vec2<float> moveDir = pos - prevPos;
	float velOffset = 3.0f;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir, size, MapData, hitChipIndexBuff);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;
	if (intersectedLine != INTERSECTED_NONE) hitChipIndex = hitChipIndexBuff;

	// 左上
	Vec2<float> velPosBuff = pos - size + Vec2<float>(velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - size + Vec2<float>(velOffset, velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff + size - Vec2<float>(velOffset, velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 右上
	velPosBuff = pos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - Vec2<float>(size.x, -size.y) - Vec2<float>(-velOffset, velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 右下
	velPosBuff = pos + size + Vec2<float>(-velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + size + Vec2<float>(-velOffset, -velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - size - Vec2<float>(-velOffset, -velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 左下
	velPosBuff = pos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - Vec2<float>(-size.x, size.y) - Vec2<float>(velOffset, -velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// スケールを元にしたマップチップとの当たり判定を行う。

	// 上方向
	float offset = 5.0f;
	Vec2<float> posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 下方向
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 右方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// 下方向
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);


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

			// 触れただけでその色になるようにする。
			MapChipType hitChipData = StageMgr::Instance()->GetLocalMapChipType(hitChipIndex);
			if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_LEFT) {

				// 自分が右側のキャラだったら。
				if (team == WHICH_TEAM::RIGHT_TEAM) {

					// ブロックを右の色にする。
					StageMgr::Instance()->WriteMapChipData(hitChipIndex, MapChipData::MAPCHIP_TYPE_STATIC_COLOR_RIGHT);

				}

			}
			else if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_RIGHT) {

				// 自分が左側のキャラだったら。
				if (team == WHICH_TEAM::LEFT_TEAM) {

					// ブロックを右の色にする。
					StageMgr::Instance()->WriteMapChipData(hitChipIndex, MapChipData::MAPCHIP_TYPE_STATIC_COLOR_LEFT);

				}

			}


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

				int smokeCol = 0;

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

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
				CrashEffectMgr::Instance()->Generate(pos, GetTeamColor());

				// 今自分がどっちのチームかを取得。
				StaminaItem::CHARA_ID charaID;
				if (team != WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::LEFT;
				if (team == WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::RIGHT;

				// あたったチップが色ブロックだったら。
				MapChipType hitChipData = StageMgr::Instance()->GetLocalMapChipType(hitChipIndex);
				auto mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();


				if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_LEFT) {

					smokeCol = 1;
					(*mapChipDrawData)[hitChipIndex.y][hitChipIndex.x].expEaseRate = 0.0f;

					// 自分が右側のキャラだったら。
					if (team == WHICH_TEAM::RIGHT_TEAM) {

						// アイテムを生成する。
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &partner.lock()->pos, StaminaItem::CHARA_ID::LEFT, partner.lock()->pos);

					}
					// 自分が左側のキャラだったら。
					else {

						// アイテムを生成する。
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &pos, StaminaItem::CHARA_ID::LEFT, partner.lock()->pos);

					}

				}
				else if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_RIGHT) {

					smokeCol = 2;
					(*mapChipDrawData)[hitChipIndex.y][hitChipIndex.x].expEaseRate = 0.0f;

					// 自分が右側のキャラだったら。
					if (team == WHICH_TEAM::RIGHT_TEAM) {

						// アイテムを生成する。
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &pos, StaminaItem::CHARA_ID::RIGHT, partner.lock()->pos);

					}
					// 自分が左側のキャラだったら。
					else {

						// アイテムを生成する。
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &partner.lock()->pos, StaminaItem::CHARA_ID::RIGHT, partner.lock()->pos);

					}

				}

				// クラッシュさせる。
				Crash(vec, smokeCol);

				CWSwingSegmentMgr.Init();
				CCWSwingSegmentMgr.Init();

			}

			// ゲージがデフォルトに戻るまでのタイマーを更新。
			gaugeReturnTimer = GAUGE_RETURN_TIMER;

		}

	}

	// 引っかかり判定の更新
	CheckHitStuck(MapData);

}

void CharacterInterFace::CheckHitStuck(const std::vector<std::vector<int>>& MapData)
{

	/*===== 引っかかり判定の更新処理 =====*/

	// マップチップの番号のデータ
	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	// マップチップの要素数
	const int MAX_X = MapData[0].size() - 1;
	const int MAX_Y = MapData.size() - 1;

	// 検索する量。
	float searchOffset = MAP_CHIP_SIZE;

	// 上側の判定を行う。
	bool isHitTop = false;

	// 上側の座標を取得する。
	Vec2<float> searchPos = pos + Vec2<float>(0, -size.y - searchOffset);

	// 検索座標のマップチップ番号を求める。
	Vec2<float> searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// 検索座標のマップチップ番号が配列外だったら処理を飛ばす。
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// そのマップチップの番号がブロックかどうかをチェック。
		isHitTop = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// 下側の判定を行う。
	bool isHitBottom = false;

	// 下側の座標を取得する。
	searchPos = pos + Vec2<float>(0, size.y + searchOffset);

	// 検索座標のマップチップ番号を求める。
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// 検索座標のマップチップ番号が配列外だったら処理を飛ばす。
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// そのマップチップの番号がブロックかどうかをチェック。
		isHitBottom = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// 右側の判定を行う。
	bool isHitRight = false;

	// 右側の座標を取得する。
	searchPos = pos + Vec2<float>(size.x + searchOffset, 0);

	// 検索座標のマップチップ番号を求める。
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };
	Vec2<float> posIndex = { pos.x / MAP_CHIP_SIZE, pos.y / MAP_CHIP_SIZE };

	// 検索座標のマップチップ番号が配列外だったら処理を飛ばす。
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// そのマップチップの番号がブロックかどうかをチェック。
		isHitRight = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// 左側の判定を行う。
	bool isHitLeft = false;

	// 左側の座標を取得する。
	searchPos = pos + Vec2<float>(-size.x - searchOffset, 0);

	// 検索座標のマップチップ番号を求める。
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// 検索座標のマップチップ番号が配列外だったら処理を飛ばす。
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// そのマップチップの番号がブロックかどうかをチェック。
		isHitLeft = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// 相手との角度
	const auto partnerPos = partner.lock()->pos;
	float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);

	// 角度が-だったら180度以上
	if (partnerAngle < 0) {

		float angleBuff = 3.14f - fabs(partnerAngle);

		partnerAngle = 3.14f + angleBuff;

	}

	if (isHitBottom) {

		// マップチップの上にあたっていたということは、プレイヤーが下方向にいればOK！
		// 下方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;

		}

	}

	if (isHitTop) {

		// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
		// 上方向の具体的な値は
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;

		}

	}

	if (isHitLeft) {

		// マップチップの右にあたっていたということは、プレイヤーが左方向にいればOK！
		// 左方向の具体的な値は
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;
		}

	}

	if (isHitRight) {

		// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
		// 右方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MAX_ANGLE <= partnerAngle || partnerAngle <= MIN_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;

		}
	}

}

void CharacterInterFace::FinishSwing()
{
	nowSwing = false;
	partner.lock()->stagingDevice.StopSpin();
	partner.lock()->OnSwingedFinish();
	CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Init();
	addSwingAngle = 0;
	swingTimer = 0;

}

void CharacterInterFace::HealStamina(const int& HealAmount)
{
	if (HealAmount)healAuraEaseRate = 0.0f;
	staminaGauge->AddStamina(HealAmount);
	OnStaminaHeal(HealAmount);
}

void CharacterInterFace::OverWriteMapChipValueAround(const Vec2<int>& MapChipIndex, const MapChipType& DstType, const MapChipData& SrcData)
{

	// 指定されたインデックスの左側のチップも左側か右側のブロックかを調べる。
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(-1, 0)) == DstType) {
		// トゲブロックを棘無し状態にさせる。
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(-1, 0), SrcData);
	}

	// 指定されたインデックスの左側のチップも左側か右側のブロックかを調べる。
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(1, 0)) == DstType) {
		// トゲブロックを棘無し状態にさせる。
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(1, 0), SrcData);
	}

	// 指定されたインデックスの左側のチップも左側か右側のブロックかを調べる。
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(0, -1)) == DstType) {
		// トゲブロックを棘無し状態にさせる。
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(0, -1), SrcData);
	}

	// 指定されたインデックスの左側のチップも左側か右側のブロックかを調べる。
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(0, 1)) == DstType) {
		// トゲブロックを棘無し状態にさせる。
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(0, 1), SrcData);
	}

}

CharacterInterFace::CharacterInterFace(const Vec2<float>& HonraiSize) : size(HonraiSize)
{
	areaHitBox.center = &pos;
	areaHitBox.size = size;
	bulletHitSphere.center = &pos;
	bulletHitSphere.radius = size.x;
	rbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_RB.png");
	lbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_LB.png");
	lineHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_line.png");
	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_arrow.png");
	staminaGauge = std::make_shared<StaminaMgr>();
	bulletMgr.Init();
	barrage = std::make_unique<CircularBarrage>();
	barrage->Init();
}

