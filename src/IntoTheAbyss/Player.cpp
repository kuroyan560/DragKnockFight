#include "Player.h"
#include "PlayerHand.h"
#include "BulletMgr.h"
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

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

static const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
static const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
Player::Player() :CharacterInterFace(PLAYER_HIT_SIZE)
{
	/*====== コンストラクタ =====*/

	lHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_time.png"));
	rHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_tele.png"));

	// 画像をロード
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav");
	AudioApp::Instance()->ChangeVolume(shotSE, 0.2f);


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

	// 腕をセット
	static const float OFFSET = -8.0f;

	lHand->Init(-GetPlayerGraphSize().x + OFFSET);
	rHand->Init(GetPlayerGraphSize().x - OFFSET);

	// 連射タイマーを初期化
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// 初期位置に戻るまでのタイマーを初期化
	handReturnTimer = DEF_HAND_RETURN_TIMER;

	// 手を初期位置に戻す。
	rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
	lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

	// 入力してから少しだけ数えるタイマーを初期化
	asSoonAsInputTimer = 0;

	//ストレッチ初期化
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	drawCursorFlag = true;

	swingCoolTime = 0;

	initSize = { 5.0f,5.0f };
	size = initSize;
	sizeVel = 120.0f;
	initPaticleFlag = false;
	moveTimer = 0;
}

void Player::OnUpdate(const vector<vector<int>>& MapData)
{
	//デバック用の値変更
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	ADD_GRAVITY = data->ADD_GRAVITY;
	MAX_GRAVITY = data->MAX_GRAVITY;
	RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;
	RAPID_FIRE_TIMER = data->RAPID_FIRE_TIMER;


	/*===== 入力処理 =====*/
	// 入力に関する更新処理を行う。
	if(GetCanMove())Input(MapData);

	/*===== 更新処理 =====*/
	//移動に関する処理
	Move();

	// 連射タイマーを更新
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// 腕を更新
	lHand->Update(pos + anim.GetHandCenterOffset());
	rHand->Update(pos + anim.GetHandCenterOffset());

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

	// 一定時間入力がなかったら腕を初期位置に戻すタイマーを更新。
	if (handReturnTimer > 0) {
		--handReturnTimer;
	}
	// 何かしらの移動量が存在したらNoInputを初期化する。
	if (vel.x != 0 || vel.y != 0) handReturnTimer = DEF_HAND_RETURN_TIMER;
	// 0以下になったら
	if (handReturnTimer <= 0) {

		// 手を初期位置に戻す。
		rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
		lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

		// Syokiiti Ni Modosu Timer Ga <=0 no Tokiha Honrai no Amount Wo Herasu
		rHand->SetIsNoInputTimer(true);
		lHand->SetIsNoInputTimer(true);

	}
	else {

		rHand->SetIsNoInputTimer(false);
		lHand->SetIsNoInputTimer(false);

	}

	// 入力されてから数フレームを取得するタイマーを更新。
	if (asSoonAsInputTimer > 0) --asSoonAsInputTimer;

	//ストレッチ更新
	UpdateStretch();

	//アニメーション更新
	anim.Update();

	// 振り回しのクールタイムを更新
	if (0 < swingCoolTime) --swingCoolTime;

	//muffler.Update(pos);
}

void Player::OnDraw()
{
	//if (vel.y < 0)playerDir = BACK;
	if (vel.y < 0)anim.ChangeAnim(DEFAULT_BACK);
	//if (0 < vel.y)playerDir = FRONT;
	if (0 < vel.y)anim.ChangeAnim(DEFAULT_FRONT);

	/*===== 描画処理 =====*/

	// プレイヤーの描画処理
	//Vec2<float>leftUp = { pos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//Vec2<float>rightBottom = { pos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//残像描画
	afImg.Draw();

	//muffler.Draw(LigManager);

	static const int ARM_GRAPH_L = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_L.png");
	static const int ARM_GRAPH_R = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_R.png");
	rHand->Draw(EXT_RATE, ARM_GRAPH_R, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	lHand->Draw(EXT_RATE, ARM_GRAPH_L, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//ストレッチ加算
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(pos + stagingDevice.GetShake());
	//胴体
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim.GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * size,
		0.0f, bodyTex, CRASH_TEX, stagingDevice.GetFlashAlpha());


	// 弾を描画
	BulletMgr::Instance()->Draw();

}

void Player::OnCheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{

	/*===== マップチップとプレイヤーとの当たり判定全般 =====*/
	{
		// ウィンドウに挟まっていなかったら
		if (!GetNowStuckWin()) {
			// マップチップとプレイヤーの当たり判定 絶対に貫通させない為の処理
			CheckHitMapChipVel(pos, MapData);
		}

	}


	/*===== 腕の当たり判定 =====*/

	lHand->CheckHit(MapData);
	rHand->CheckHit(MapData);
}

void Player::OnHitMapChip(const HIT_DIR& Dir)
{
	if (Dir == TOP)
	{
		// Y方向の移動量を減らす。
		vel.y /= 2.0f;
	}
	else if (Dir == BOTTOM)
	{
		stretch_RB.y = 0.0f;

		// 接地フラグを立てる。
		//onGround = true;

		// X軸の移動量の合計が一定以上だったら摩擦を作る。
		if (fabs(vel.x) >= STOP_DEADLINE_X) {

			// 摩擦をつける。
			vel.y *= VEL_MUL_AMOUNT;
			vel.x *= VEL_MUL_AMOUNT;
		}
		else {

			// X方向の移動量を無効化する。
			vel.x = 0;
			vel.y = 0;

			//摩擦無いときはストレッチを弱くする
			stretch_RB.x /= STRETCH_DIV_RATE;
			stretch_LU.x /= STRETCH_DIV_RATE;

			//待機アニメーションに戻す
			//anim.ChangeAnim(ON_GROUND_WAIT);
		}

		vel.y = 0;

		// 移動量が一定以下になったら0にする。
		if (fabs(vel.x) <= 1.0f) vel.x = 0;
	}
	else if(Dir == LEFT)
	{
		stretch_LU.x = 0.0f;

		// X方向の移動量を無効化する。
		vel.x = 0;
		//vel.y = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		//anim.ChangeAnim(ON_WALL_WAIT);
	}
	else if (Dir == RIGHT)
	{
		stretch_RB.x = 0.0f;

		// X方向の移動量を無効化する。
		vel.x = 0;
		//vel.y = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		//anim.ChangeAnim(ON_WALL_WAIT);
	}
}

void Player::Input(const vector<vector<int>>& MapData)
{
	/*===== 入力処理 =====*/

	// スタン演出中だったら入力を受け付けない。
	if (StunEffect::Instance()->isActive) return;

	const float INPUT_DEAD_LINE = 0.3f;

	Vec2<float> inputVec;

	inputVec = UsersInput::Instance()->GetLeftStickVecFuna();
	inputVec /= {32768.0f, 32768.0f};
	// 入力のデッドラインを設ける。
	if (inputVec.Length() >= 0.9f) {

		// 右手の角度を更新
		lHand->SetAngle(KuroFunc::GetAngle(inputVec));

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	inputVec = UsersInput::Instance()->GetRightStickVecFuna();
	inputVec /= {32768.0f, 32768.0f};

	// 入力のデッドラインを設ける。
	if (inputVec.Length() >= 0.9f) {

		// 左手の角度を更新
		rHand->SetAngle(KuroFunc::GetAngle(inputVec));

		// 一定時間入力がなかったら初期位置に戻す
		//handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	// LBが押されたら反動をつける。
	if (UsersInput::Instance()->Input(XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

		// 反動をつける。
		float rHandAngle = lHand->GetAngle();

		// Getした値は手の向いている方向なので、-180度する。
		rHandAngle -= Angle::PI();

		// onGroundがtrueだったら移動量を加算しない。
		//if (!onGround || sinf(rHandAngle) < 0.5f) {
		vel.x += cosf(rHandAngle) * RECOIL_AMOUNT;
		//}

		vel.y += sinf(rHandAngle) * RECOIL_AMOUNT;

		// プレイヤーの腕を動かす。
		lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), false);


		// 弾を生成する。
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = 12.0f;

		float angle = lHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(lHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, false);

		// 連射タイマーをセット
		rapidFireTimerLeft = RAPID_FIRE_TIMER;

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// 入力タイマーをセット。
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

		//ストレッチ
		CalculateStretch(vel);
	}

	// RBが押されたら反動をつける。
	if (UsersInput::Instance()->Input(XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

		// 反動をつける。
		float lHandAngle = rHand->GetAngle();

		// Getした値は手の向いている方向なので、-180度する。
		lHandAngle -= Angle::PI();

		// プレイヤーのひとつ上のブロックを検索する為の処理。
		int mapX = pos.x / MAP_CHIP_SIZE;
		int mapY = pos.y / MAP_CHIP_SIZE;
		if (mapX <= 0) mapX = 1;
		if (mapX >= MapData[0].size()) mapX = MapData[0].size() - 1;
		if (mapY <= 0) mapY = 1;
		if (mapY >= MapData.size()) mapY = MapData.size() - 1;

		// onGroundがtrueだったら移動量を加算しない。
			//if (!onGround || sinf(lHandAngle) < 0) {
		vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;



		vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

		// プレイヤーの腕を動かす。
		rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);


		// 弾を生成する。
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = -12.0f;

		float angle = rHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(rHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, true);

		// 連射タイマーをセット
		rapidFireTimerRight = RAPID_FIRE_TIMER;

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// 入力タイマーをセット。
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

		//ストレッチ
		CalculateStretch(vel);
	}

	// 移動速度が限界値を超えないようにする。
	if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;

	// RTが押されたら
	if (swingCoolTime <= 0 && UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RT)) {

		// 振り回しの処理

		// 振り回しにデッドラインを設ける。
		Vec2<float> dir = GetPartnerPos() - pos;
		dir.Normalize();

		// 振り回しのトリガー判定
		if (0.3f < fabs(dir.y)) {

			SwingPartner();

			// クールタイムを設定。
			swingCoolTime = SWING_COOLTIME;
		}

	}
}

void Player::Move()
{

	/*===== 移動に関する処理 =====*/

	// 移動量を0に近付ける。
	vel = KuroMath::Lerp(vel, { 0.0f,0.0f }, 0.05f);

	// 移動量が限界を超えないようにする。
	if (fabs(vel.x) > MAX_RECOIL_AMOUNT) {

		// 符号を保存する。
		bool sign = std::signbit(vel.x);

		// 上限を与える。
		vel.x = MAX_RECOIL_AMOUNT;

		// 符号をかける。
		vel.x *= sign ? -1 : 1;

	}
	if (fabs(vel.y) > MAX_RECOIL_AMOUNT) {

		// 符号を保存する。
		bool sign = std::signbit(vel.y);

		// 上限を与える。
		vel.y = MAX_RECOIL_AMOUNT;

		// 符号をかける。
		vel.y *= sign ? -1 : 1;
	}
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

void Player::CheckHitMapChipVel(const Vec2<float> &checkPos, const vector<vector<int>> &MapData)
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

bool Player::Appear()
{
	if (20 <= moveTimer)
	{
		return true;
	}

	//サイズが1.0fになるまで動かない
	if (1.0f < size.x && 1.0f < size.y)
	{
		float time = 30.0f;
		size -= initSize / time;
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

		size = { 1.0f,1.0f };
		++moveTimer;
	}

	return false;
}
