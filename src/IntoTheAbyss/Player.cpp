#include "Player.h"
#include "PlayerHand.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
//#include "BulletParticleMgr.h"
#include "StunEffect.h"
#include "Collider.h"
#include <cmath>

#include"TexHandleMgr.h"
#include"UsersInput.h"
#include"DrawFunc.h"
#include"DrawFunc_Shadow.h"
#include"DrawFunc_FillTex.h"
#include"WinApp.h"
#include "ParticleMgr.h"
#include"DebugParameter.h"
#include"SuperiorityGauge.h"
#include"AudioApp.h"
#include "SlowMgr.h"
#include"CrashMgr.h"
#include "AfterImage.h"
#include "DrawFunc.h"
#include "Stamina.h"

#include"CharacterAIData.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

static const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
static const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
Player::Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const WHICH_TEAM& Team)
	:CharacterInterFace(PLAYER_HIT_SIZE), anim(CharacterName), controllerIdx((int)Team), tutorial(Team)
{
	/*====== コンストラクタ =====*/
	if (PLAYER_CHARACTER_NUM <= CharacterName)assert(0);
	static const std::string PLAYER_DIR = "resource/ChainCombat/player/";
	static const std::string NAME_DIR[PLAYER_CHARACTER_NUM] =
	{
		"luna",
		"lacy"
	};
	const int AIM_GRAPH = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/aim.png");

	// 画像をロード
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav");
	AudioApp::Instance()->ChangeVolume(shotSE, 0.2f);

	bulletGraph = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/bullet.png");

	playerPilotGraph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pilot.png");
	playerPilotGraph[1] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pilot_back.png");
	pilotGraph = playerPilotGraph[0];
}

Player::~Player()
{
}

void Player::OnInit()
{

	/*===== 初期化処理 =====*/

	//プレイヤーの向き初期化
	//playerDir = DEFAULT;

	//アニメーション初期化
	anim.Init();
	tiredTimer = TIRED_DRAW_TIME;

	// 腕をセット
	static const float OFFSET = -8.0f;

	// 連射タイマーを初期化
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	//ストレッチ初期化
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	drawCursorFlag = true;

	swingCoolTime = 0;

	sizeVel = 120.0f;

	playerDirX = GetWhichTeam() == LEFT_TEAM ? PLAYER_LEFT : PLAYER_RIGHT;
	playerDirY = PLAYER_FRONT;

	// 右のキャラだったら赤
	if (GetWhichTeam() == RIGHT_TEAM) {
		charaColor = { 239, 1, 144,255 };
	}
	// ひだりのキャラだったら緑
	else if (GetWhichTeam() == LEFT_TEAM) {
		charaColor = { 47, 255, 139,255 };
	}

	swingVec = { -1.0f,0.0f };

	inputInvalidTimerByCrash = 0;

	dashAftImgTimer = 0;
	isInputRightStick = false;
	isPrevInputRightStick = false;
	prevInputRightStick = {};

	autoPilotMove = { 0,0 };
	pilotGraph = playerPilotGraph[0];
}

void Player::OnUpdate(const vector<vector<int>>& MapData)
{
	//デバック用の値変更
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;


	CharacterAIData::Instance()->playerData.dashStamina = DASH_STAMINA;
	CharacterAIData::Instance()->playerData.swingStamina = SWING_STAMINA;




	/*===== 入力処理 =====*/
	// 入力に関する更新処理を行う。
	if (GetCanMove())Input(MapData);

	/*===== 更新処理 =====*/

	// 先行入力タイマーを更新。
	if (isAdvancedEntrySwing) {

		--advancedEntrySwingTimer;
		if (advancedEntrySwingTimer < 0) isAdvancedEntrySwing = false;

	}

	// 相方が振り回しをしていたら。
	if (partner.lock()->GetNowSwing()) {

		// 残像を保存。
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate();
		AfterImageMgr::Instance()->Generate(pos, extRate, stagingDevice.GetSpinRadian(), anim.GetGraphHandle(), charaColor);

	}

	// 相方が振り回しをしていたら更新処理を行わない。
	if (!(!partner.lock()->GetNowSwing() && !nowSwing)) return;

	//移動に関する処理
	Move();

	// 連射タイマーを更新
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// プレイヤーのすぐ一個上のマップチップ座標を検索する。
	int mapX = pos.x / MAP_CHIP_SIZE;
	int mapY = (pos.y - PLAYER_HIT_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= MapData[0].size()) mapX = MapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= MapData.size()) mapY = MapData.size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapX <= 0) mapX = 1;
	if (mapX >= MapData[mapY].size()) mapX = MapData[mapY].size() - 1;

	// 一個上のマップチップがブロックで、X軸方向の移動量が一定以上だったらパーティクルを生成する。
	//if (MapData[mapY][mapX] > 0 && MapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)ParticleMgr::Instance()->Generate(Vec2<float>(pos.x, pos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);

	// 振り回しのクールタイムを更新
	if (0 < swingCoolTime) --swingCoolTime;

	//muffler.Update(pos);

	// 入力を無効化するタイマーを更新。
	if (0 < inputInvalidTimerByCrash)
	{
		--inputInvalidTimerByCrash;
		if (inputInvalidTimerByCrash <= 0 && !GetNowBreak())
		{
			anim.ChangeAnim(DEFAULT_FRONT);
		}
	}

	// グリップ力タイマーが0になったら、完全に回復するまで踏ん張れないようにする。
	/*if (gripPowerTimer <= 0) {

		isGripPowerEmpty = true;
		anim.ChangeAnim(TIRED);
	}*/

	//ダッシュの残像
	if (dashAftImgTimer)
	{
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate();
		AfterImageMgr::Instance()->Generate(pos, extRate, 0.0f, anim.GetGraphHandle(), GetTeamColor());
		dashAftImgTimer--;
	}

	// 振り回していなかったら
	if (!nowSwing) {

		if (anim.GetNowAnim() == HOLD)anim.ChangeAnim(DEFAULT_FRONT);

		// 紐つかみ状態(踏ん張り状態)を解除する。
		isHold = false;

		//tutorial.lock()->SetRstickInput(false);

	}

}

void Player::OnUpdateNoRelatedSwing()
{
	// 握力タイマーを規定値に近づける。
	//if (!nowSwing && gripPowerTimer < MAX_GRIP_POWER_TIMER) {
	//	gripPowerTimer += SlowMgr::Instance()->slowAmount;

	//	// 最大値になったら握力を使い切ってから回復している状態フラグを折る。
	//	if (MAX_GRIP_POWER_TIMER <= gripPowerTimer) {

	//		gripPowerTimer = MAX_GRIP_POWER_TIMER;
	//		isGripPowerEmpty = false;
	//		anim.ChangeAnim(DEFAULT_FRONT);
	//	}
	//}

	//ストレッチ更新
	UpdateStretch();

	//アニメーション更新
	anim.Update();

	if (tiredTimer < TIRED_DRAW_TIME)
	{
		tiredTimer++;
		if (TIRED_DRAW_TIME <= tiredTimer)anim.ChangeAnim(DEFAULT_FRONT);
	}

	if (staminaGauge->emptyTrigger)
	{
		tiredTimer = 0;
		anim.ChangeAnim(TIRED);
	}

}

void Player::OnDraw()
{
	//if (vel.y < 0)playerDir = BACK;
	auto moveInput = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });

	if (!isHold && !anim.Compare({ SWINGED,TIRED,NON_PILOT }) && CompleteAppear())
	{
		if (moveInput.x)
		{
			if (playerDirY == PLAYER_BACK)anim.ChangeAnim(PULL_BACK);
			else anim.ChangeAnim(PULL_FRONT);
		}
		else
		{
			if (playerDirY == PLAYER_BACK)anim.ChangeAnim(DEFAULT_BACK);
			else anim.ChangeAnim(DEFAULT_FRONT);
		}
	}

	/*===== 描画処理 =====*/

	// プレイヤーの描画処理
	//Vec2<float>leftUp = { pos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//Vec2<float>rightBottom = { pos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//残像描画
	afImg.Draw();

	//muffler.Draw(LigManager);


	//rHand->Draw(EXT_RATE, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	//lHand->Draw(EXT_RATE, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//ストレッチ加算
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(pos + stagingDevice.GetShake());
	//胴体
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim.GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	bool mirorX = playerDirX == PLAYER_RIGHT || (isHold && (partner.lock()->pos - pos).x < 0);
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * staminaGauge->outOfStaminaEffect.GetSize() * appearExtRate,
		stagingDevice.GetSpinRadian(), bodyTex, CRASH_TEX, stagingDevice.GetFlashAlpha(), { 0.5f,0.5f }, { mirorX,false });
}

void Player::OnDrawUI()
{
	static const int LINE_ALPHA = 120;
	static std::shared_ptr<TextureBuffer> COLOR_TEX[TEAM_NUM] =
	{
		 D3D12App::Instance()->GenerateTextureBuffer(Color(47,255,139,LINE_ALPHA)),
		 D3D12App::Instance()->GenerateTextureBuffer(Color(239,1,144,LINE_ALPHA))
	};
	static const int RETICLE_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_enemy.png") };
	static const int ARROW_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/arrow_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/arrow_enemy.png") };
	static const Angle ARROW_ANGLE_OFFSET = Angle(1);
	static const float ARROW_DIST_OFFSET = 32.0f;

	const auto rightStickVec = UsersInput::Instance()->GetRightStickVec(controllerIdx, { 0.5f,0.5f });

	if (isHold && !GetNowSwing() && !StunEffect::Instance()->isActive)
	{
		const Vec2<float>drawScale = { ScrollMgr::Instance()->zoom ,ScrollMgr::Instance()->zoom };
		const auto team = GetWhichTeam();

		//振り回し先描画
		float dist = partner.lock()->pos.Distance(pos);
		Vec2<float>target = pos + rightStickVec * dist;
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(pos), ScrollMgr::Instance()->Affect(target), COLOR_TEX[GetWhichTeam()], 2, AlphaBlendMode_Trans);
		//DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(target), drawScale * 0.8f, 0.0f, TexHandleMgr::GetTexBuffer(RETICLE_GRAPH[team]));

		//振り回し方向描画
		bool clockWise = 0 < rightStickVec.Cross(partner.lock()->pos - pos);
		Angle arrowPosAngle = KuroFunc::GetAngle(pos, partner.lock()->pos);
		Angle rotateAngle = arrowPosAngle + Angle(90 * (clockWise ? -1 : 1));
		Vec2<float>vec = partner.lock()->pos - pos;
		vec.Normalize();
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(partner.lock()->pos + vec * ARROW_DIST_OFFSET), drawScale * 0.5f, rotateAngle, TexHandleMgr::GetTexBuffer(ARROW_GRAPH[team]), Color(), { 0.0f,0.5f });
	}

	const auto leftStickVec = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });
	//const auto leftTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LT);
	//const auto rightTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RT);
	const auto leftButton = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB);
	const auto rightButton = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RB);

	//tutorial.lock()->Draw(leftStickVec, rightStickVec, leftTrigger, rightTrigger);
	tutorial.Draw(leftStickVec, rightStickVec, leftButton, rightButton);
}

void Player::OnHitMapChip(const HIT_DIR& Dir)
{
}
void Player::OnPilotControl()
{
	const auto rightStickVec = UsersInput::Instance()->GetRightStickVec(controllerIdx, { 0.9f,0.9f });
	//static const float PILOT_SPEED = 18.0f;
	//auto move = rightStickVec * PILOT_SPEED;
	//pilotPos += move;
	const float PILOT_ACCEL_SPEED = 4.0f;
	//const auto accel = rightStickVec * PILOT_ACCEL_SPEED;
	//pilotVel += accel;
	pilotAccel = KuroMath::Lerp(pilotAccel, rightStickVec * PILOT_ACCEL_SPEED, 0.3f);
	pilotVel += pilotAccel;
	pilotPos += pilotVel;

	//空気抵抗で減速
	pilotVel = KuroMath::Lerp(pilotVel, { 0,0 }, 0.1f);

	if (rightStickVec.x < 0)pilotDrawMiror = false;
	if (0 < rightStickVec.x)pilotDrawMiror = true;

	if (rightStickVec.y < 0)pilotGraph = playerPilotGraph[1];	//後ろ向き
	if (0 < rightStickVec.y)pilotGraph = playerPilotGraph[0];	//前向き
}

void Player::Input(const vector<vector<int>>& MapData)
{
	/*===== 入力処理 =====*/

	// スタン演出中だったら入力を受け付けない。
	if (StunEffect::Instance()->isActive) return;

	// 入力を受け付けないタイマーが0より大きかったら処理を飛ばす。
	if (0 < inputInvalidTimerByCrash) return;

	// 壁に挟まって判定が無効化されている間は処理を受け付けない。
	if (0 < GetStackWinTimer()) return;

	// 振り回しているときは入力を受け付けない。
	if (nowSwing) return;

	// 振り回されているフラグ
	bool isSwingPartner = partner.lock()->GetNowSwing();

	// 振り回されている状態の時に踏ん張りの状態になっているかフラグ(実際には踏ん張っていない)。
	bool isSwingPartnerHold = false;

	const float INPUT_DEAD_LINE = 0.3f;

	Vec2<float> inputLeftVec;

	inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(controllerIdx);
	inputLeftVec /= {32768.0f, 32768.0f};

	Vec2<float> inputRightVec = UsersInput::Instance()->GetRightStickVecFuna(controllerIdx);
	inputRightVec /= {32768.0f, 32768.0f};

	// 入力情報を保存する。
	isPrevInputRightStick = isInputRightStick;
	isInputRightStick = 0.9f < inputRightVec.Length();

	// 入力のデッドラインを設ける。
	float inputRate = inputLeftVec.Length();
	if (inputRate >= 0.5f) {

		// 移動を受け付ける。
		if (vel.Length() < MOVE_SPEED_PLAYER) {
			vel.x = inputLeftVec.x * (MOVE_SPEED_PLAYER * inputRate);
			vel.y = inputLeftVec.y * (MOVE_SPEED_PLAYER * inputRate);
			autoPilotMove = vel;
		}

		if (inputLeftVec.x < 0)playerDirX = PLAYER_LEFT;
		else if (0 < inputLeftVec.x)playerDirX = PLAYER_RIGHT;
		if (inputLeftVec.y < 0)playerDirY = PLAYER_BACK;
		else if (0 < inputLeftVec.y)playerDirY = PLAYER_FRONT;
	}
	/* ※オートパイロットやるづらく感じたので、一旦コメントアウト
	//オートパイロット
	else if (IsPilotOutSide())
	{
		static const float AUTO_PILOT_SPEED = 10.0f;
		vel = autoPilotMove.GetNormal() * AUTO_PILOT_SPEED;

		if (autoPilotMove.x < 0)playerDirX = PLAYER_LEFT;
		else if (0 < autoPilotMove.x)playerDirX = PLAYER_RIGHT;
		if (autoPilotMove.y < 0)playerDirY = PLAYER_BACK;
		else if (0 < autoPilotMove.y)playerDirY = PLAYER_FRONT;
	}
	*/

	// 入力を受け付ける変数
	bool isInputLB = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB);
	bool isInputRB = UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::RB);

	//チュートリアルの表示 / 非表示
	if (UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::BACK))
	{
		tutorial.TurnActive();
	}


	// 相方との位置関係においての逆ベクトルに振り回す。
	Vec2<float> subPos = pos - partner.lock()->pos;
	swingVec = (subPos).GetNormal();

	// スタミナが残っているか？
	bool isSwingStamina = staminaGauge->CheckCanAction(SWING_STAMINA);

	// RTが押されたら
	bool canSwing = (!isInputRightStick && isPrevInputRightStick) && isSwingStamina && !IsPilotOutSide();
	if ((!isSwingPartner && canSwing || isAdvancedEntrySwing)) {

		// 振り回しの処理

		// 外積の左右判定により、時計回りか反時計回り家を取得する。負の値が左、正の値が右。
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(prevInputRightStick);

		SwingPartner({ swingVec }, isClockWise);

		isInputSwingRB = isInputRB;

		//キャラクターAI用のデータ集め
		CharacterAIData::Instance()->swingFlag = true;
	}
	//else if (isSwingPartner && canSwing && !isGripPowerEmpty && isInputRightStick) {
	else if (isSwingPartner && canSwing && isInputRightStick) {

		// 先行入力を保存。
		isAdvancedEntrySwing = true;
		advancedEntrySwingTimer = ADVANCED_ENTRY_SWING_TIMER;
	}
	else
	{
		//キャラクターAI用のデータ集め
		CharacterAIData::Instance()->swingFlag = false;
	}

	// スタミナが残っているか？
	bool isDashStamina = staminaGauge->CheckCanAction(DASH_STAMINA);

	// 入力のデッドラインを設ける。
	inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(controllerIdx);
	inputLeftVec /= {32768.0f, 32768.0f};
	inputRate = inputLeftVec.Length();
	if (isInputLB && !isPrevLeftBottom && 0.5f <= inputRate && isDashStamina) {

		// inputVec = ひだりスティックの入力方向
		const float DASH_SPEED = 30.0f;
		vel += inputLeftVec * DASH_SPEED;

		// 移動量が限界を超えないようにする。
		if (DASH_SPEED < vel.Length()) {

			vel.Normalize();
			vel *= DASH_SPEED;

		}

		//煙
		ParticleMgr::Instance()->Generate(pos, -inputLeftVec, BULLET);
		//残像
		dashAftImgTimer = 10;

		// スタミナを消費
		staminaGauge->ConsumesStamina(DASH_STAMINA);

		CharacterAIData::Instance()->dashFlag = true;
	}
	else
	{
		CharacterAIData::Instance()->dashFlag = false;
	}

	// スタミナが残っているか？
	isSwingStamina = staminaGauge->CheckCanAction(SWING_STAMINA);

	// 右スティックが入力されていたら、予測線を出す。
	if (isInputRightStick && isSwingStamina) {

		// 時計回りかどうか。負の値が左、正の値が右。
		inputRightVec.Normalize();
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(inputRightVec);

		if (isClockWise) {

			CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
			CWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
			CCWSwingSegmentMgr.Init();

		}
		else {

			CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
			CCWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
			CWSwingSegmentMgr.Init();

		}

	}
	else
	{
		if (!IsPilotOutSide())
		{
			//パイロット切り離し
			if (UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::RB))SetPilotDetachedFlg(true);
		}
	}
	if (UsersInput::Instance()->ControllerOffTrigger(controllerIdx, XBOX_BUTTON::RB))SetPilotDetachedFlg(false);

	// 入力されていなくて、スイング中じゃなかったら予測線を消す。
	if ((inputRightVec.Length() <= 0.5f && !nowSwing) || IsPilotOutSide()) {

		CCWSwingSegmentMgr.Init();
		CWSwingSegmentMgr.Init();
	}

	// 入力を保存。
	Vec2<float> buff = UsersInput::Instance()->GetRightStickVecFuna(controllerIdx);
	buff /= {32768.0f, 32768.0f};
	if (0.5f <= buff.Length()) {

		prevInputRightStick = buff;

	}

	// 左ショルダーの入力情報を保存。
	isPrevLeftBottom = isInputLB;

}

void Player::Move()
{

	/*===== 移動に関する処理 =====*/

	// 移動量を0に近付ける。
	vel = KuroMath::Lerp(vel, { 0.0f,0.0f }, 0.05f);

	// 移動量が限界を超えないようにする。
	//if (fabs(vel.x) > MAX_RECOIL_AMOUNT) {

	//	// 符号を保存する。
	//	bool sign = std::signbit(vel.x);

	//	// 上限を与える。
	//	vel.x = MAX_RECOIL_AMOUNT;

	//	// 符号をかける。
	//	vel.x *= sign ? -1 : 1;

	//}
	//if (fabs(vel.y) > MAX_RECOIL_AMOUNT) {

	//	// 符号を保存する。
	//	bool sign = std::signbit(vel.y);

	//	// 上限を与える。
	//	vel.y = MAX_RECOIL_AMOUNT;

	//	// 符号をかける。
	//	vel.y *= sign ? -1 : 1;
	//}
}

void Player::Shot(const Vec2<float>& GeneratePos, const float& ForwardAngle)
{
	//弾速
	static const float BULLET_SPEED = 30.0f;
	bulletMgr.Generate(bulletGraph, GeneratePos, ForwardAngle, BULLET_SPEED);
}

void Player::PushBackWall()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*===== 壁との押し戻し処理 =====*/

	// X軸に関する押し戻し処理。

	// 右側
	const float PLAYER_RIGHT_X = pos.x + PLAYER_HIT_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// 差分押し戻す。
		pos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}

	// 左側
	const float PLAYER_LEFT_X = pos.x - PLAYER_HIT_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// 差分押し戻す。
		pos.x -= PLAYER_LEFT_X;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}


	// Y軸に関する押し戻し処理

	// 上側
	const float PLAYER_UP_Y = pos.y - PLAYER_HIT_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// 差分押し戻す。
		pos.y -= PLAYER_UP_Y;

		// Y方向の移動量を減らす。
		vel.y /= 2.0f;

	}

	// 下側
	const float PLAYER_DOWN_Y = pos.y + PLAYER_HIT_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// 差分押し戻す。
		pos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// 接地フラグを立てる。
		//onGround = true;

		// Y方向の移動量を無効化する。
		vel.y = 0;

		// X方向の移動量を無効化する。
		vel.x += (-vel.x) / 10.0f;

	}
	// 接地していなかったら
	else {

		// 接地フラグを折る。
		//onGround = false;

	}

}

void Player::CalculateStretch(const Vec2<float>& Move)
{
	Vec2<float> stretchRate = { abs(Move.x / MAX_RECOIL_AMOUNT),abs(Move.y / MAX_RECOIL_AMOUNT) };

	//X移動の勢いの方が強い
	if (stretchRate.y < stretchRate.x)
	{
		//anim.ChangeAnim(ON_AIR_DASH_X);
	}
	//Y移動の勢いの方が強い
	else if (stretchRate.x < stretchRate.y)
	{
		//anim.ChangeAnim(ON_AIR_DASH_Y);
	}

	if (1.0f < stretchRate.x)stretchRate.x = 1.0f;
	if (1.0f < stretchRate.y)stretchRate.y = 1.0f;

	//左
	if (Move.x < 0.0f)
	{
		//右に伸びる
		stretch_RB.x += MAX_STRETCH.x * stretchRate.x;
		if (MAX_STRETCH.x < stretch_RB.x)stretch_RB.x = MAX_STRETCH.x;
	}
	//右
	else if (0.0f < Move.x)
	{
		//左に伸びる
		stretch_LU.x -= MAX_STRETCH.x * stretchRate.x;
		if (stretch_LU.x < -MAX_STRETCH.x)stretch_LU.x = -MAX_STRETCH.x;
	}

	static const float SHRINK_RATE = 1.0f / 2.0f;
	static const float OTHER_STRETCH_BOOL_RATE = 0.8f;

	//左右移動時
	if (Move.x != 0.0f && stretchRate.y < OTHER_STRETCH_BOOL_RATE)
	{
		//上下が縮む
		stretch_LU.y += MAX_STRETCH.y * stretchRate.x * SHRINK_RATE;
		if (MAX_STRETCH.y < stretch_LU.y)stretch_LU.y = MAX_STRETCH.y;
		stretch_RB.y -= MAX_STRETCH.y * stretchRate.x * SHRINK_RATE;
		if (stretch_RB.y < -MAX_STRETCH.y)stretch_RB.y = -MAX_STRETCH.y;
	}

	//上
	if (Move.y < 0.0f)
	{
		//下に伸びる
		stretch_RB.y += MAX_STRETCH.y * stretchRate.y;
		if (MAX_STRETCH.y < stretch_RB.y)stretch_RB.y = MAX_STRETCH.y;
	}
	//下
	else if (0.0f < Move.y)
	{
		//上に伸びる
		stretch_LU.y -= MAX_STRETCH.y * stretchRate.y;
		if (stretch_LU.y < -MAX_STRETCH.y)stretch_LU.y = -MAX_STRETCH.y;
	}

	//上下移動時
	if (Move.y != 0.0f && stretchRate.x < OTHER_STRETCH_BOOL_RATE)
	{
		//左右が縮む
		stretch_LU.x += MAX_STRETCH.x * stretchRate.y * SHRINK_RATE;
		if (MAX_STRETCH.x < stretch_LU.x)stretch_LU.x = MAX_STRETCH.x;
		stretch_RB.x -= MAX_STRETCH.x * stretchRate.y * SHRINK_RATE;
		if (stretch_RB.x < -MAX_STRETCH.x)stretch_RB.x = -MAX_STRETCH.x;
	}

	stretchTimer = 0;
	fromStretch_LU = stretch_LU;
	fromStretch_RB = stretch_RB;
}

#include"KuroMath.h"
void Player::UpdateStretch()
{
	static const EASING_TYPE STRETCH_EASE_TYPE = Cubic;
	stretch_LU = KuroMath::Ease(Out, STRETCH_EASE_TYPE, stretchTimer, STRETCH_RETURN_TIME, fromStretch_LU, { 0.0f,0.0f });
	stretch_RB = KuroMath::Ease(Out, STRETCH_EASE_TYPE, stretchTimer, STRETCH_RETURN_TIME, fromStretch_RB, { 0.0f,0.0f });

	if (stretchTimer < STRETCH_RETURN_TIME)
	{
		stretchTimer++;
	}
}

Vec2<float> Player::GetPlayerGraphSize()
{
	//return { (56 * EXT_RATE) / 2.0f,(144 * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
	return { (anim.GetGraphSize().x * EXT_RATE) / 2.0f,(anim.GetGraphSize().y * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
}

void Player::CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& MapData)
{
	// マップチップとプレイヤーの当たり判定 絶対に貫通させない為の処理
	//Vec2<float> upperPlayerPos = pos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//Vec2<float> upperPlayerPosBuff = upperPlayerPos;
	INTERSECTED_LINE intersectedLine = {};

	// 通常時の当たり判定
	Vec2<float> moveDir = prevPos - pos;
	moveDir.Normalize();
	//intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevFrameCenterPos, vel + gimmickVel, Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), onGround, MapData);
	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), MapData);


	// 差分(押し戻された値を座標から引く)
	//pos += upperPlayerPos - upperPlayerPosBuff;

	// 相方とプレイヤーの角度
	auto partnerPos = GetPartnerPos();
	float bossAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);
	bossAngle = fabs(bossAngle);

	// 当たった位置に応じて処理を分ける。
	if (intersectedLine == INTERSECTED_TOP) {

		// 下に当たった場合
		mapChipHit[BOTTOM] = true;

		// マップチップの上にあたっていたということは、ボスが下方向にいればOK！
		// 下方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RIGHT) {

		// 左に当たった場合
		mapChipHit[LEFT] = true;

		// マップチップの右にあたっていたということは、ボスが左方向にいればOK！
		// 左方向の具体的な値は
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_BOTTOM) {

		// 上に当たった場合
		mapChipHit[TOP] = true;

		// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
		// 上方向の具体的な値は
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_LEFT) {

		// 右に当たった場合
		mapChipHit[RIGHT] = true;

		// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
		// 右方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// 引っかかっている。
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_RISE) {

		// のし上がりの場所に当たった場合
		vel.y *= 0.5f;
	}

}