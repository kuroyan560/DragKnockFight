#include "Player.h"
#include "PlayerHand.h"
#include "BulletMgr.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "SwingMgr.h"
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
#include "EventCollider.h"
#include"DebugParameter.h"
#include"SuperiorityGauge.h"
#include"AudioApp.h"
#include "SlowMgr.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

Player::Player()
{
	/*====== コンストラクタ =====*/

	lHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_time.png"));
	rHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_tele.png"));

	// 画像をロード
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav");
	AudioApp::Instance()->ChangeVolume(shotSE, 0.2f);

	this->Init(GetGeneratePos());

}

Player::~Player()
{
}

void Player::Init(const Vec2<float> &INIT_POS)
{

	/*===== 初期化処理 =====*/

	//プレイヤーの向き初期化
	//playerDir = DEFAULT;

	//アニメーション初期化
	anim.Init();

	// プレイヤーの位置をセット
	centerPos = INIT_POS;
	//prevFrameCenterPos = INIT_POS;

	// 移動量をリセット
	vel = {};

	// 重力をリセット
	gravity = 0;

	// 接地していないフラグをリセット
	onGround = false;
	firstShot = false;

	// 腕をセット
	static const float OFFSET = -8.0f;

	lHand->Init(-GetPlayerGraphSize().x + OFFSET);
	rHand->Init(GetPlayerGraphSize().x - OFFSET);

	// 連射タイマーを初期化
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// 重力無効化タイマーを初期化。
	gravityInvalidTimer = 0;

	isPrevFrameShotBeacon = false;
	firstShot = false;

	// 壁フラグを初期化
	isWallRight = false;
	isWallLeft = false;

	isDouji = false;

	isDead = false;

	// 壁ズリフラグを初期化。
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// 初期位置に戻るまでのタイマーを初期化
	handReturnTimer = DEF_HAND_RETURN_TIMER;

	// 手を初期位置に戻す。
	rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
	lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

	// 入力してから少しだけ数えるタイマーを初期化
	asSoonAsInputTimer = 0;

	inBubble = false;

	//ストレッチ初期化
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	//テレポート時の点滅の初期化
	flashTimer = TELE_FLASH_TIME;
	flashTotalTime = TELE_FLASH_TIME;

	firstRecoilParticleTimer = 0;
	drawCursorFlag = true;

	//同時ショット判定タイマーリセット
	isLeftFirstShotTimer = DOUJI_ALLOWANCE_FRAME;
	isRightFirstShotTimer = DOUJI_ALLOWANCE_FRAME;

	isZeroGravity = false;
	changeGravityTimer = 0;

	stuckWindowTimer = 0;

	//muffler.Init(INIT_POS);

	areaHitBox.center = &centerPos;
	areaHitBox.size = PLAYER_HIT_SIZE;

	swingCoolTime = 0;


	bulletHitBox = std::make_shared<SphereCollision>();
	bulletHitBox->center = &centerPos;
	bulletHitBox->radius = 10.0f;

	size = { 3.0f,3.0f };
}

void Player::Update(const vector<vector<int>> mapData, const Vec2<float> &bossPos)
{

	if (1.0f <= size.x && 1.0f <= size.y)
	{
		size.x -= 0.1f;
		size.y -= 0.1f;
		doorMoveUpDownFlag = true;
	}


	//デバック用の値変更
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	ADD_GRAVITY = data->ADD_GRAVITY;
	MAX_GRAVITY = data->MAX_GRAVITY;
	RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;
	RAPID_FIRE_TIMER = data->RAPID_FIRE_TIMER;


	/*===== 入力処理 =====*/

	if (!doorMoveLeftRightFlag && !doorMoveUpDownFlag && !isDead)
	{
		// 入力に関する更新処理を行う。
		Input(mapData, bossPos);
	}

	/*===== 更新処理 =====*/
	if (!doorMoveLeftRightFlag && !doorMoveUpDownFlag)
	{
		//移動に関する処理
		Move();
	}

	//ScrollMgr::Instance()->CalucurateScroll(prevFrameCenterPos - centerPos);
	prevFrameCenterPos = centerPos;

	if (!doorMoveLeftRightFlag)
	{
		// 重力に関する更新処理
		UpdateGravity();
	}
	doorMoveLeftRightFlag = false;
	doorMoveUpDownFlag = false;
	doorMoveDownFlag = false;




	// 連射タイマーを更新
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// 腕を更新
	lHand->Update(centerPos + anim.GetHandCenterOffset());
	rHand->Update(centerPos + anim.GetHandCenterOffset());

	// 弾を更新
	BulletMgr::Instance()->Update();

	// 入力を受け付けないフラグを初期化
	isWallRight = false;
	isWallLeft = false;

	// プレイヤーのすぐ一個上のマップチップ座標を検索する。
	int mapX = centerPos.x / MAP_CHIP_SIZE;
	int mapY = (centerPos.y - PLAYER_HIT_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= mapData.size()) mapY = mapData.size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[mapY].size()) mapX = mapData[mapY].size() - 1;

	// 一個上のマップチップがブロックで、X軸方向の移動量が一定以上だったらパーティクルを生成する。
	if (mapData[mapY][mapX] > 0 && mapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);

	// 壁ズリのパーティクルを生成。
	if (isSlippingWall[PLAYER_TOP]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);
	if (isSlippingWall[PLAYER_RIGHT]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x, centerPos.y), Vec2<float>(1, 0), HIT_MAP);
	if (isSlippingWall[PLAYER_BOTTOM]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y), Vec2<float>(0, 1), HIT_MAP);
	if (isSlippingWall[PLAYER_LEFT]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x, centerPos.y), Vec2<float>(-1, 0), HIT_MAP);


	// 壁ズリフラグを初期化。
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// 一定時間入力がなかったら腕を初期位置に戻すタイマーを更新。
	if (handReturnTimer > 0) {
		--handReturnTimer;
	}
	// 何かしらの移動量が存在したらNoInputを初期化する。
	if (vel.x != 0 || vel.y != 0 || gimmickVel.x != 0 || gimmickVel.y != 0 || gravity != 0) handReturnTimer = DEF_HAND_RETURN_TIMER;
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

	// ギミックから与えられる移動量を初期化。
	//gimmickVel = {};

	// 入力されてから数フレームを取得するタイマーを更新。
	if (asSoonAsInputTimer > 0) --asSoonAsInputTimer;

	//ストレッチ更新
	UpdateStretch();

	//アニメーション更新
	anim.Update();


	// 最初の一発のタイマーが起動していたらパーティクルを生成する。
	if (0 < firstRecoilParticleTimer) {

		// 移動している方向を求める。
		Vec2<float> invForwardVec = -vel;

		// 最高速度からのパーセンテージを求める。
		float per = (float)firstRecoilParticleTimer / (float)FIRST_SHOT_RECOIL_PARTICLE_TIMER;

		// 正規化する。
		invForwardVec.Normalize();

		if (!isDouji)
		{
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH);
		}
		else
		{
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH_DOUJI);
		}

		--firstRecoilParticleTimer;
	}


	++changeGravityTimer;
	if (CHANGE_GRAVITY_TIMER <= changeGravityTimer) {

		//isZeroGravity = isZeroGravity ? false : true;

		//changeGravityTimer = 0;

	}

	if (isZeroGravity) gravity = 0;

	//テレポート時のフラッシュのタイマー計測
	if (flashTimer < flashTotalTime)flashTimer++;


	// ウィンドウに挟まれたタイマーを更新
	if (0 < stuckWindowTimer) --stuckWindowTimer;

	// 振り回しのクールタイムを更新
	if (0 < swingCoolTime) --swingCoolTime;

	//muffler.Update(centerPos);
}

void Player::Draw(LightManager &LigManager)
{
	//if (vel.y < 0)playerDir = BACK;
	if (vel.y < 0)anim.ChangeAnim(DEFAULT_BACK);
	//if (0 < vel.y)playerDir = FRONT;
	if (0 < vel.y)anim.ChangeAnim(DEFAULT_FRONT);

	/*===== 描画処理 =====*/

	// プレイヤーの描画処理
	//Vec2<float>leftUp = { centerPos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	centerPos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//Vec2<float>rightBottom = { centerPos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	centerPos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//残像描画
	afImg.Draw();

	//muffler.Draw(LigManager);

	static const int ARM_GRAPH_L = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_L.png");
	static const int ARM_GRAPH_R = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_R.png");
	rHand->Draw(LigManager, EXT_RATE * size.x, ARM_GRAPH_R, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	lHand->Draw(LigManager, EXT_RATE * size.y, ARM_GRAPH_L, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//ストレッチ加算
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(centerPos);
	//胴体
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim.GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	DrawFunc::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * size, 0.0f, bodyTex);

	//テレポート時のフラッシュ
	static auto white = D3D12App::Instance()->GenerateTextureBuffer(Color());
	const float flashAlpha = KuroMath::Ease(Out, Quint, flashTimer, flashTotalTime, 1.0f, 0.0f);
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom, 0.0f, bodyTex, white, flashAlpha);

	// 弾を描画
	BulletMgr::Instance()->Draw();

}

void Player::CheckHit(const vector<vector<int>> mapData, vector<Bubble> &bubble, vector<DossunBlock> &dossun, const Vec2<float> &bossPos, bool &isHitMapChip, const Vec2<float> &lineCenterPos)
{

	/*===== マップチップとプレイヤーとの当たり判定全般 =====*/

	{

		// ウィンドウに挟まっていなかったら
		if (stuckWindowTimer <= 0) {

			// マップチップとプレイヤーの当たり判定 絶対に貫通させない為の処理
			CheckHitMapChipVel(centerPos, mapData, bossPos, isHitMapChip);

			// マップチップとプレイヤーの当たり判定 スケール
			CheckHitSize(centerPos, mapData, bossPos, isHitMapChip);

			// 左右に当たった際に壁釣りさせるための処理。
			//int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//int xChip = (centerPos.x - PLAYER_HIT_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//if (yChip <= 0) yChip = 1;
			//if (yChip >= mapData.size()) yChip = mapData.size() - 1;
			//if (xChip <= 0) xChip = 1;
			//if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			//// プレイヤーの左側がマップチップだったら
			//if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			//	HitMapChipLeft();
			//}
			//xChip = (centerPos.x + PLAYER_HIT_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//if (xChip < 0) xChip = 0;
			//if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			//// プレイヤーの右側がマップチップだったら
			//if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			//	HitMapChipRight();
			//}

			// 引き伸ばし判定の補助のための処理
			float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
			bossAngle = fabs(bossAngle);
			int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int xChip = (centerPos.x - PLAYER_HIT_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			if (yChip <= 0) yChip = 1;
			if (yChip >= mapData.size()) yChip = mapData.size() - 1;
			if (xChip <= 0) xChip = 1;
			if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			// プレイヤーの左側がマップチップだったら
			if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
				// 敵が指定の範囲の中にいたら
				const float MIN_ANGLE = 2.35619f;
				const float MAX_ANGLE = 3.92699f;
				// 角度がこの値の間だったら
				if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {
					// 引っかかっている。
					isHitMapChip = true;
				}
			}
			xChip = (centerPos.x + PLAYER_HIT_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			if (xChip < 0) xChip = 0;
			if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			// プレイヤーの右側がマップチップだったら
			if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
				// 敵が指定の範囲の中にいたら
				const float MIN_ANGLE = 0.785398f;
				const float MAX_ANGLE = 5.49779f;
				// 角度がこの値の間だったら
				if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {
					// 引っかかっている。
					isHitMapChip = true;
				}
			}

		}

	}


	/*===== 動的ブロックとプレイヤーの当たり判定 =====*/

	{

		const int MOVING_BLOCK_COUNT = MovingBlockMgr::Instance()->movingBlock.size();
		for (int movingIndex = 0; movingIndex < MOVING_BLOCK_COUNT; ++movingIndex) {

			// 動的ブロックのプレイヤーから近いフラグが立っていなかったら処理を飛ばす。
			if (!MovingBlockMgr::Instance()->movingBlock[movingIndex]->isCloseToThePlayer) continue;

			// X軸、Y軸での距離を求める。
			float distanceX = MovingBlockMgr::Instance()->movingBlock[movingIndex]->pos.x - centerPos.x;
			float distanceY = MovingBlockMgr::Instance()->movingBlock[movingIndex]->pos.y - centerPos.y;

			// 距離の絶対値
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// 移動ブロックとプレイヤーの大きさの合計。
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_HIT_SIZE.x,MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_HIT_SIZE.y };

			// XYの距離が動的ブロックとプレイヤーのサイズ以上だったら処理を飛ばす。
			if (distanceXABS >= playerMovingBlockSize.x ||
				distanceYABS >= playerMovingBlockSize.y) continue;

			// XYの遠い方を検索して遠い方のみ押し戻し処理を行う。
			bool isPushBackX = distanceXABS >= distanceYABS;

			// Xを押し戻す場合。
			if (isPushBackX) {

				// プレイヤーより右に居た場合
				if (distanceX <= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x + distanceX;

					// 押し戻す。
					centerPos.x += pushBackDistance;

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// 押し戻す。
					centerPos.x -= pushBackDistance;

				}

			}
			// Yを押し戻す場合。
			else {

				if (distanceY >= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// 押し戻す。
					centerPos.y -= pushBackDistance;

					// この場合は地面にあたった状態。
					HitMapChipBottom();

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// 押し戻す。
					centerPos.y += pushBackDistance;

				}

			}

		}

	}


	/*===== 短槍の当たり判定 =====*/

	// 時間停止の短槍とドッスンブロックが当たっているかを保存する変数
	bool isHitTeleportPikeDossun = false;
	int hitTeleportPikeIndex = 0;

	{

		//	// 瞬間移動の短槍の当たり判定を行う。
		//	if (!rHand->teleportPike.isHitWall && rHand->teleportPike.isActive) {
		//		bool onGround = false;
		//		INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(rHand->teleportPike.pos, rHand->teleportPike.prevFramePos, rHand->teleportPike.forwardVec, Vec2<float>(rHand->teleportPike.SCALE, rHand->teleportPike.SCALE), onGround, mapData);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		//		// 押し戻し
		//		Vec2<float> scale = Vec2<float>(rHand->teleportPike.SCALE * 1.0f, rHand->teleportPike.SCALE * 1.0f);
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_BOTTOM);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_TOP);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_LEFT);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_RIGHT);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		//	}

		//	// 時間停止の短槍とドッスンブロックの当たり判定を行う。
		//	const int DOSSUN_COUNT = dossun.size();
		//	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		//		//if (rHand->teleportPike.isHitWall) continue;

		//		// まずは当たっているかをチェックする。
		//		if (fabs(rHand->teleportPike.pos.x - dossun[index].pos.x) > rHand->teleportPike.SCALE + dossun[index].size.x) continue;
		//		if (fabs(rHand->teleportPike.pos.y - dossun[index].pos.y) > rHand->teleportPike.SCALE + dossun[index].size.y) continue;

		//		// 瞬間移動の短槍に壁にあたったフラグを付ける。
		//		rHand->teleportPike.isHitWall = true;

		//		rHand->teleportPike.gimmickVel = dossun[index].moveDir * Vec2<float>(dossun[index].speed, dossun[index].speed);

		//		isHitTeleportPikeDossun = true;
		//		hitTeleportPikeIndex = index;

		//	}

		//	// 時間停止の短槍の当たり判定を行う。
		//	if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
		//		bool onGround = false;
		//		INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(lHand->timeStopPike.pos, lHand->timeStopPike.prevFramePos, lHand->timeStopPike.forwardVec, Vec2<float>(lHand->timeStopPike.SCALE, lHand->timeStopPike.SCALE), onGround, mapData);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }

		//		// 押し戻し
		//		Vec2<float> scale = Vec2<float>(lHand->timeStopPike.SCALE * 1.0f, lHand->timeStopPike.SCALE * 1.0f);
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_BOTTOM);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_TOP);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_LEFT);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_RIGHT);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//	}

		//	// 時間停止の短槍とドッスンブロックの当たり判定を行う。
		//	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		//		if (lHand->timeStopPike.isHitWall) continue;

		//		// まずは当たっているかをチェックする。
		//		if (fabs(lHand->timeStopPike.pos.x - dossun[index].pos.x) > lHand->timeStopPike.SCALE + dossun[index].size.x) continue;
		//		if (fabs(lHand->timeStopPike.pos.y - dossun[index].pos.y) > lHand->timeStopPike.SCALE + dossun[index].size.y) continue;

		//		lHand->timeStopPike.isHitWall = true;
		//		lHand->timeStopPike.stopPos = dossun[index].speed;
		//		lHand->timeStopPike.stopTargetPos = &dossun[index].speed;

		//		// この時点でドッスンブロックは1F分移動しちゃっているので、押し戻す。
		//		dossun[index].pos -= dossun[index].moveDir * Vec2<float>(dossun[index].speed, dossun[index].speed);

		//		// ぷれいやーもおしもどす。
		//		centerPos -= gimmickVel;

		//		dossun[index].isTimeStopPikeAlive = &(lHand->timeStopPike.isHitWall);

		//	}

	}

	/*===== 腕の当たり判定 =====*/

	lHand->CheckHit(mapData);
	rHand->CheckHit(mapData);

	/*===== プレイヤーとシャボン玉の当たり判定 =====*/

	{
		const int BUBBLE_COUNT = bubble.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {

			if (bubble[index].isBreak) continue;

			// hitcheck
			bool isHitBubbleX = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.x + bubble[index].RADIUS;
			bool isHitBubbleY = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.y + bubble[index].RADIUS;

			// isHit
			if (!isHitBubbleX && !isHitBubbleY) continue;

			// If there is an input
			if (inBubble && asSoonAsInputTimer >= 1) {

				// break a bubble
				bubble[index].isBreak = true;
				bubble[index].breakCoolTime = bubble[index].BREAK_COOL_TIME;
				inBubble = false;

			}
			else {

				// 入った最初の1F目だったら
				if (fabs(vel.x) > 0 || fabs(vel.y) > 0 || fabs(gravity) > ADD_GRAVITY) bubble[index].addEasingTimer = 30.0f;

				vel = {};
				gravity = {};
				gimmickVel = {};

				// Get closer to the bubble
				centerPos.x += (bubble[index].pos.x - centerPos.x) / 5.0f;
				centerPos.y += (bubble[index].pos.y - centerPos.y) / 5.0f;

				inBubble = true;

				// Recovery of recoil
				rHand->isFirstShot = false;
				lHand->isFirstShot = false;
				isDouji = false;
			}

		}

	}


	/*===== プレイヤーとドッスンブロックの当たり判定 =====*/

	bool isHitDossun = false;

	const int DOSSUN_COUNT = dossun.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		bool isDossunVel = Collider::Instance()->CheckHitVel(centerPos, prevFrameCenterPos, vel + gimmickVel, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size) != INTERSECTED_NONE;
		bool isDossunTop = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_TOP) != INTERSECTED_NONE;
		bool isDossunRight = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_RIGHT) != INTERSECTED_NONE;
		bool isDossunLeft = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_LEFT) != INTERSECTED_NONE;
		bool isDossunBottom = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_BOTTOM) != INTERSECTED_NONE;

		// どこかしらにぶつかっていれば当たった判定にする。
		if (!isDossunTop && (isDossunVel || isDossunRight || isDossunLeft || isDossunBottom)) {

			// プレイヤーにドッスンブロックの移動量を渡す。
			gimmickVel = Vec2<float>(dossun[index].speed, dossun[index].speed) * dossun[index].moveDir;

			isHitDossun = true;

			// ドッスンの移動量タイマーを更新。
			dossun[index].isHitPlayer = true;

			// プレイヤーの移動量をかき消す。
			gravity *= 0.0f;
			vel *= {0.0f, 0.0f};

			// 最初の一発フラグを大きくする。
			lHand->isFirstShot = false;
			rHand->isFirstShot = false;
			isDouji = false;
		}
		else if (isHitTeleportPikeDossun && hitTeleportPikeIndex == index) {

			// ドッスンの移動量タイマーを更新。
			dossun[index].isHitPlayer = true;

		}
		else {

			// ドッスンの移動量タイマーを初期化。
			dossun[index].isHitPlayer = false;
			//isMoveTimer = 0;

		}

		if (isDossunTop) vel.y /= 2.0f;

		if (!isHitDossun) gimmickVel = {};

	}


	/*===== プレイヤーがドッスンに挟まれた判定 =====*/

	// プレイヤーの左右がドッスンもしくはマップチップと当たっていたら死亡判定を行う。
	{

		float offset = 1.0f;

		bool isHitLeft = false;
		bool isHitRight = false;

		// マップチップとの当たり判定。
		{

			// プレイヤーの左側のマップチップ番号を求める。
			int playerChipX = (centerPos.x - PLAYER_HIT_SIZE.x - offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// 求めた番号が範囲外じゃなかったら。
			if (0 < (centerPos.y - PLAYER_HIT_SIZE.y) && 0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// そのマップチップの番号が1~9の間だったら。
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// 当たっている
					isHitLeft = true;

				}

			}

		}

		// ドッスンとの当たり判定
		{

			Vec2<float> checkPos = { centerPos.x - PLAYER_HIT_SIZE.x - offset, centerPos.y };

			// 全てのドッスンブロックと判定を行う。
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// 求める座標がドッスンブロックの範囲内にあるかどうかをチェックする。
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// ここまでくれば当たっている判定。
				isHitLeft = true;

			}

		}

		// マップチップとの当たり判定。
		{
			// プレイヤーの右側のマップチップ番号を求める。
			int playerChipX = (centerPos.x + PLAYER_HIT_SIZE.x + offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// 求めた番号が範囲外じゃなかったら。
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// そのマップチップの番号が1~9の間だったら。
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// 当たっている
					isHitRight = true;

				}

			}

		}

		// ドッスンとの当たり判定
		{

			Vec2<float> checkPos = { centerPos.x + PLAYER_HIT_SIZE.x + offset, centerPos.y };

			// 全てのドッスンブロックと判定を行う。
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// 求める座標がドッスンブロックの範囲内にあるかどうかをチェックする。
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// ここまでくれば当たっている判定。
				isHitRight = true;

			}

		}

		// 挟まれていたら死亡
		if (isHitRight && isHitLeft) {

			isDead = true;

		}

	}

	// プレイヤーの上下がドッスンもしくはマップチップと当たっていたら死亡判定を行う。
	{

		float offset = 1.0f;

		bool isHitTop = false;
		bool isHitBottom = false;

		// マップチップとの当たり判定。
		{

			// プレイヤーの上側のマップチップ番号を求める。
			int playerChipX = (centerPos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y - PLAYER_HIT_SIZE.y - offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;



			// 求めた番号が範囲外じゃなかったら。
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// そのマップチップの番号が1~9の間だったら。
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// 当たっている
					isHitTop = true;

				}

			}

		}

		// ドッスンとの当たり判定
		{

			Vec2<float> checkPos = { centerPos.x, centerPos.y - PLAYER_HIT_SIZE.y - offset };

			// 全てのドッスンブロックと判定を行う。
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// 求める座標がドッスンブロックの範囲内にあるかどうかをチェックする。
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// ここまでくれば当たっている判定。
				isHitTop = true;

			}

		}

		// マップチップとの当たり判定。
		{

			// プレイヤーの下側のマップチップ番号を求める。
			int playerChipX = (centerPos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + PLAYER_HIT_SIZE.y + offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// 求めた番号が範囲外じゃなかったら。
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// そのマップチップの番号が1~9の間だったら。
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// 当たっている
					isHitBottom = true;

				}

			}

		}

		// ドッスンとの当たり判定
		{

			Vec2<float> checkPos = { centerPos.x, centerPos.y + PLAYER_HIT_SIZE.y + offset };

			// 全てのドッスンブロックと判定を行う。
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// 求める座標がドッスンブロックの範囲内にあるかどうかをチェックする。
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// ここまでくれば当たっている判定。
				isHitBottom = true;

			}

		}

		// 挟まれていたら死亡
		if (isHitTop && isHitBottom) {

			isDead = true;

		}

	}

	if (isDead) {

		int a = 0;

	}

	isDead = false;


	// マップチップにあたっている状態で画面外に出たら
	if (isHitMapChip) {

		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// ボスとプレイヤーの距離
		float distanceX = fabs(lineCenterPos.x - centerPos.x);
		float disntaceY = fabs(lineCenterPos.y - centerPos.y);

		// ウィンドウ左右
		if (windowSize.x <= centerPos.x + PLAYER_HIT_SIZE.x - ScrollMgr::Instance()->scrollAmount.x || centerPos.x - PLAYER_HIT_SIZE.x - ScrollMgr::Instance()->scrollAmount.x <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddEnemyGauge(10);

		}
		// ウィンドウ上下
		if (windowSize.y <= centerPos.y + PLAYER_HIT_SIZE.y - ScrollMgr::Instance()->scrollAmount.y || centerPos.y - PLAYER_HIT_SIZE.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddEnemyGauge(10);

		}

	}

}

void Player::HitMapChipTop()
{
	// Y方向の移動量を減らす。
	vel.y /= 2.0f;

	// 壁ズリフラグを設定
	isSlippingWall[PLAYER_TOP] = true;

	// 最初の一発は反動が強いフラグを初期化する。
	//rHand->Hit();
	//lHand->Hit();
}

void Player::HitMapChipLeft()
{
	stretch_LU.x = 0.0f;

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		//vel.y *= 0.95f;
		vel.y *= VEL_MUL_AMOUNT;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_LEFT] = true;

		//摩擦用アニメーション
		//anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		firstRecoilParticleTimer = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		//anim.ChangeAnim(ON_WALL_WAIT);
	}

	// 最初の一発は反動が強いフラグを初期化する。
	firstShot = false;

	// 入力を受け付けないフラグを設定
	isWallLeft = true;

	// 最初の一発フラグを初期化
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;
	isDouji = false;
}

void Player::HitMapChipRight()
{
	stretch_RB.x = 0.0f;

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		vel.y *= VEL_MUL_AMOUNT;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_RIGHT] = true;

		//摩擦用アニメーション
		//anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		firstRecoilParticleTimer = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		//anim.ChangeAnim(ON_WALL_WAIT);
	}

	// 最初の一発は反動が強いフラグを初期化する。
	firstShot = false;

	gravity *= 0.9f;

	// 入力を受け付けないフラグを設定
	isWallRight = true;

	// 最初の一発フラグを初期化
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;
	isDouji = false;
}

void Player::HitMapChipBottom()
{
	stretch_RB.y = 0.0f;

	// 接地フラグを立てる。
	//onGround = true;

	// X軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.x) >= STOP_DEADLINE_X) {

		// 摩擦をつける。
		vel.y *= VEL_MUL_AMOUNT;
		vel.x *= VEL_MUL_AMOUNT;

		// 重力を無効化する。
		gravity = 0.5f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_BOTTOM] = true;

		//摩擦用アニメーション
		//anim.ChangeAnim(ON_GROUND_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;

		// 重力を無効化する。
		gravity = 0.5f;

		firstRecoilParticleTimer = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.x /= STRETCH_DIV_RATE;
		stretch_LU.x /= STRETCH_DIV_RATE;

		//待機アニメーションに戻す
		//anim.ChangeAnim(ON_GROUND_WAIT);
	}

	gravity = 0;
	vel.y = 0;

	// 移動量が一定以下になったら0にする。
	if (fabs(vel.x) <= 1.0f) vel.x = 0;

	// 最初の一発は反動が強いフラグを初期化する。
	firstShot = false;

	// 最初の一発フラグを初期化
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;
	isDouji = false;
}

void Player::StopDoorLeftRight()
{
	doorMoveLeftRightFlag = true;
}

void Player::StopDoorUpDown()
{
	doorMoveUpDownFlag = true;
}

void Player::Input(const vector<vector<int>> mapData, const Vec2<float> &bossPos)
{
	/*===== 入力処理 =====*/

	//if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {

	//	isZeroGravity = isZeroGravity ? false : true;
	//	changeGravityTimer = 0;

	//}

	// スタン演出中だったら入力を受け付けない。
	if (StunEffect::Instance()->isActive) return;

	//同時ショット判定タイマー計測
	if (isLeftFirstShotTimer < DOUJI_ALLOWANCE_FRAME)isLeftFirstShotTimer++;
	if (isRightFirstShotTimer < DOUJI_ALLOWANCE_FRAME)isRightFirstShotTimer++;

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

	// 振り回し中及び振り回され中かを判断
	bool isSwingNow = SwingMgr::Instance()->isSwingBoss || SwingMgr::Instance()->isSwingPlayer;

	// LBが押されたら反動をつける。
	if (!isSwingNow && UsersInput::Instance()->Input(XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

		// 反動をつける。
		float rHandAngle = lHand->GetAngle();

		// Getした値は手の向いている方向なので、-180度する。
		rHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// 移動量を加算
		if (!lHand->isFirstShot) {

			// 撃った判定を保存。
			isLeftFirstShotTimer = 0;

			// onGroundがtrueだったら移動量を加算しない。
			if (!onGround || sinf(rHandAngle) < 0.6f) {
				vel.x += cosf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// プレイヤーのひとつ上のブロックを検索する為の処理。
			int mapX = centerPos.x / MAP_CHIP_SIZE;
			int mapY = centerPos.y / MAP_CHIP_SIZE;
			if (mapX <= 0) mapX = 1;
			if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
			if (mapY <= 0) mapY = 1;
			if (mapY >= mapData.size()) mapY = mapData.size() - 1;

			// 入力を受け付けないフラグが立っていなかったら
			if (!isWallRight && !isWallLeft) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			// プレイヤーの一個上のマップチップがブロックだったら最初に一発を大きくするフラグを大きくする。
			else if (mapData[mapY - 1][mapX] > 0 && mapData[mapY - 1][mapX] < 10 && cosf(rHandAngle) < 0.5f && cosf(rHandAngle) > -0.5f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallRight && cosf(rHandAngle) < 0.0f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallLeft && cosf(rHandAngle) > 0.1f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			isFirstShot = true;

			firstShot = true;

			lHand->isFirstShot = true;

			// シェイク量を設定
			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT);

			// プレイヤーの腕を動かす。
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), true);

			firstRecoilParticleTimer = FIRST_SHOT_RECOIL_PARTICLE_TIMER;

		}
		else {

			// onGroundがtrueだったら移動量を加算しない。
			//if (!onGround || sinf(rHandAngle) < 0.5f) {
			vel.x += cosf(rHandAngle) * RECOIL_AMOUNT;
			//}

			vel.y += sinf(rHandAngle) * RECOIL_AMOUNT;

			// プレイヤーの腕を動かす。
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), false);

		}

		// 弾を生成する。
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = 12.0f;

		float angle = lHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(lHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, isFirstShot, false);

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
	if (!isSwingNow && UsersInput::Instance()->Input(XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

		// 反動をつける。
		float lHandAngle = rHand->GetAngle();

		// Getした値は手の向いている方向なので、-180度する。
		lHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// プレイヤーのひとつ上のブロックを検索する為の処理。
		int mapX = centerPos.x / MAP_CHIP_SIZE;
		int mapY = centerPos.y / MAP_CHIP_SIZE;
		if (mapX <= 0) mapX = 1;
		if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
		if (mapY <= 0) mapY = 1;
		if (mapY >= mapData.size()) mapY = mapData.size() - 1;

		// 移動量を加算
		if (!rHand->isFirstShot) {

			// 右側に撃った判定を保存する。
			isRightFirstShotTimer = 0;

			// onGroundがtrueだったら移動量を加算しない。
			if (!onGround || sinf(lHandAngle) < 0.6f) {
				vel.x += cosf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// 入力を受け付けないフラグが立っていなかったら
			if (!isWallLeft && !isWallRight) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			// プレイヤーの一個上のマップチップがブロックだったら最初に一発を大きくするフラグを大きくする。
			else if (mapData[mapY - 1][mapX] > 0 && mapData[mapY - 1][mapX] < 10 && cosf(lHandAngle) < 0.5f && cosf(lHandAngle) > -0.5f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallRight && cosf(lHandAngle) < 0.3f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallLeft && cosf(lHandAngle) > 0.1f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// 最初の一発を撃ったフラグを立てる。
			firstShot = true;

			isFirstShot = true;

			rHand->isFirstShot = true;

			// シェイク量を設定
			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT);

			// プレイヤーの腕を動かす。
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), true);

			firstRecoilParticleTimer = FIRST_SHOT_RECOIL_PARTICLE_TIMER;

		}
		else {
			// onGroundがtrueだったら移動量を加算しない。
				//if (!onGround || sinf(lHandAngle) < 0) {
			vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
			//			//}

			{
				//=======
				//		if (!onGround || sinf(lHandAngle) < 0) {
				//			vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
				//		}
				//>>>>>>> 繧ｹ繧ｯ繝ｭ繝ｼ繝ｫ縺ｮ荳ｭ蠢・ｒ邱壼・縺ｮ荳ｭ蠢・↓縺吶ｋ縺溘ａ縺ｮ繝・せ繝医ヶ繝ｩ繝ｳ繝・
			}

			vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

			// プレイヤーの腕を動かす。
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);

		}

		// 弾を生成する。
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = -12.0f;

		float angle = rHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(rHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, isFirstShot, true);

		// 連射タイマーをセット
		rapidFireTimerRight = RAPID_FIRE_TIMER;

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// 入力タイマーをセット。
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

		//ストレッチ
		CalculateStretch(vel);
	}

	// 同時に撃ったフラグを取得する。
	if (!isDouji && isLeftFirstShotTimer < DOUJI_ALLOWANCE_FRAME && isRightFirstShotTimer < DOUJI_ALLOWANCE_FRAME) {

		isDouji = true;

		if (isLeftFirstShotTimer == 0 || isRightFirstShotTimer == 0)
		{
			//同時ショットエフェクト
			flashTimer = 0;
			flashTotalTime = DOUJI_FLASH_TIME;
			UsersInput::Instance()->ShakeController(1.0f, 10);
		}
	}

	// 移動速度が限界値を超えないようにする。
	if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;

	// RTが押されたら
	if (swingCoolTime <= 0 && UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RT)) {

		// 振り回しの処理

		// 振り回しのトリガー判定
		if (!SwingMgr::Instance()->isSwingPlayer) {

			// 振り回しの開始ベクトルを取得。
			SwingMgr::Instance()->easingStartVec = bossPos - centerPos;
			SwingMgr::Instance()->easingStartVec.Normalize();
			SwingMgr::Instance()->easingNowVec = SwingMgr::Instance()->easingStartVec;

			// 振り回しの終了ベクトルを取得。
			SwingMgr::Instance()->easingEndVec = (bossPos - centerPos) * Vec2<float>(1.0f, -1.0f);
			SwingMgr::Instance()->easingEndVec.Normalize();

			// 振り回しフラグを有効化する。
			SwingMgr::Instance()->isSwingPlayer = true;

			// 各タイマーを初期化。
			SwingMgr::Instance()->easingTimer = 0;
			SwingMgr::Instance()->easeAmount = 0;
			SwingMgr::Instance()->easeChangeAmountY = SwingMgr::Instance()->easingEndVec.y - SwingMgr::Instance()->easingStartVec.y;

			// クールタイムを設定。
			swingCoolTime = SWING_COOLTIME;

		}



		// 短槍の残骸
		{

			//// 瞬間移動の短槍に関する処理を行う。

			//// 右腕の弾が発射されていなかったら。
			//if (!rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			//	// クールタイムが0以下だったら
			//	if (rHand->pikeCooltime <= 0) {

			//		const float ARM_DISTANCE = 20.0f;
			//		const float OFFSET_Y = -14.0f;
			//		const float OFFSET_X = -12.0f;

			//		float angle = rHand->GetAngle();

			//		// 弾を発射する処理を行う。
			//		rHand->teleportPike.Generate(rHand->GetPos() + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), Vec2<float>(cosf(angle), sinf(angle)), PIKE_TELEPORT);

			//	}

			//}
			//// ビーコンが発射されていたら。
			//else if (rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			//	auto vec = rHand->teleportPike.pos - centerPos;

			//	//向き変え
			//	if (vec.x < 0)playerDir = LEFT;
			//	if (0 < vec.x)playerDir = RIGHT;

			//	//画像サイズの違いによって壁にくっつかないときがあるので少しめり込むようにする
			//	static const float OFFSET = 3.0f;
			//	rHand->teleportPike.pos.x += (playerDir == RIGHT) ? OFFSET : -OFFSET;
			//	vec = rHand->teleportPike.pos - centerPos;	//移動量更新

			//	//ストレッチ
			//	CalculateStretch(vec);

			//	//残像エミット
			//	afImg.EmitArray(centerPos, rHand->teleportPike.pos, anim.GetGraphHandle(), GetPlayerGraphSize(), { playerDir != DEFAULT,false });
			//	Vec2<int>graphSize = TexHandleMgr::GetTexBuffer(GetHandGraph(true))->GetGraphSize();
			//	graphSize *= EXT_RATE / 2.0f;
			//	lHand->EmitAfterImg(vec, GetHandGraph(true), graphSize.Float(), { false,false });
			//	graphSize = TexHandleMgr::GetTexBuffer(GetHandGraph(false))->GetGraphSize();
			//	graphSize *= EXT_RATE / 2.0f;
			//	rHand->EmitAfterImg(vec, GetHandGraph(false), graphSize.Float(), { false,false });

			//	//点滅
			//	flashTimer = 0;
			//	flashTotalTime = TELE_FLASH_TIME;

			//	// プレイヤーを瞬間移動させる。
			//	centerPos = rHand->teleportPike.pos;

			//	ScrollMgr::Instance()->WarpScroll(centerPos);

			//	// プレイヤーの重力を無効化させる。
			//	gravity = 0;

			//	// 移動量を無効化する。
			//	vel = {};

			//	// 移動量を無効化する。
			//	gimmickVel = {};

			//	// 重力無効化タイマーを設定。
			//	gravityInvalidTimer = GRAVITY_INVALID_TIMER;

			//	// ビーコンを初期化する。
			//	rHand->teleportPike.Init();

			//	// ビーコンのクールタイムを設定
			//	rHand->pikeCooltime = rHand->PIKE_COOL_TIME;
			//}

			//isPrevFrameShotBeacon = true;
		}

	}
	if (!UsersInput::Instance()->Input(XBOX_BUTTON::RT)) {

		// 振り回しフラグを折る。
		SwingMgr::Instance()->isSwingPlayer = false;

	}


	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {






		// 短槍の残骸
		{

			//// 時間停止の短槍に関する処理を行う。

			//// 左腕の弾が発射されていなかったら。
			//if (!lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			//	// クールタイムが0以下だったら
			//	if (lHand->pikeCooltime <= 0) {

			//		const float ARM_DISTANCE = 20.0f;
			//		const float OFFSET_Y = -14.0f;
			//		const float OFFSET_X = 12.0f;

			//		float angle = lHand->GetAngle();

			//		// 弾を発射する処理を行う。
			//		lHand->timeStopPike.Generate(lHand->GetPos() + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), Vec2<float>(cosf(angle), sinf(angle)), PIKE_TIMESTOP);

			//	}

			//}
			//// ビーコンが発射されていたら。
			//else if (lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			//	// 止められていたものを動かす。
			//	lHand->timeStopPike.MoveAgain();

			//	// ビーコンを初期化する。
			//	lHand->timeStopPike.Init();

			//	// ビーコンのクールタイムを設定
			//	lHand->pikeCooltime = lHand->PIKE_COOL_TIME;

			//}

			//isPrevFrameShotBeacon = true;
		}

	}
	else {

		isPrevFrameShotBeacon = false;

	}
}

void Player::Move()
{

	/*===== 移動に関する処理 =====*/

	// 移動処理
	//centerPos += vel;

	// ギミックから与えられる移動量を加算
	centerPos += gimmickVel;

	// スクロール量を更新
	//ScrollMgr::Instance()->honraiScrollAmount -= prevFrameCenterPos - centerPos;


	// 移動量を0に近付ける。
	//vel.x -= vel.x / 25.0f;
	//vel.y -= vel.y / 25.0f;
	vel = KuroMath::Lerp(vel, { 0.0f,0.0f }, 0.05f);

	// 中心座標を保存
	//prevFrameCenterPos = centerPos;

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

void Player::UpdateGravity()
{

	/*===== 重力に関する更新処理 =====*/

	// 重力を加算。
	//centerPos.y += gravity;

	// 接地フラグが立ってい且つ重力無効化タイマーが0以下だったらなかったら重力を加算。
	if (!onGround && gravityInvalidTimer <= 0) {

		// 重力を加算。
		//gravity += ADD_GRAVITY;

		// 重力が限界を超えないようにする。
		if (gravity > MAX_GRAVITY) {

			gravity = MAX_GRAVITY;

		}

	}

	// 重力無効化タイマーを更新。
	if (gravityInvalidTimer > 0) {

		--gravityInvalidTimer;

		gravity = 0;

	}

}

void Player::PushBackWall()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*===== 壁との押し戻し処理 =====*/

	// X軸に関する押し戻し処理。

	// 右側
	const float PLAYER_RIGHT_X = centerPos.x + PLAYER_HIT_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// 差分押し戻す。
		centerPos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}

	// 左側
	const float PLAYER_LEFT_X = centerPos.x - PLAYER_HIT_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// 差分押し戻す。
		centerPos.x -= PLAYER_LEFT_X;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}


	// Y軸に関する押し戻し処理

	// 上側
	const float PLAYER_UP_Y = centerPos.y - PLAYER_HIT_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// 差分押し戻す。
		centerPos.y -= PLAYER_UP_Y;

		// Y方向の移動量を減らす。
		vel.y /= 2.0f;

	}

	// 下側
	const float PLAYER_DOWN_Y = centerPos.y + PLAYER_HIT_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// 差分押し戻す。
		centerPos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// 接地フラグを立てる。
		//onGround = true;

		// 重力を無効化する。
		gravity = 0;

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

void Player::CalculateStretch(const Vec2<float> &Move)
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

void Player::CheckHitMapChipVel(const Vec2<float> &checkPos, const vector<vector<int>> &mapData, const Vec2<float> &bossPos, bool &isHitMapChip)
{
	// マップチップとプレイヤーの当たり判定 絶対に貫通させない為の処理
	//Vec2<float> upperPlayerPos = centerPos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//Vec2<float> upperPlayerPosBuff = upperPlayerPos;
	INTERSECTED_LINE intersectedLine = {};

	// 通常時の当たり判定
	Vec2<float> moveDir = prevFrameCenterPos - centerPos;
	moveDir.Normalize();
	//intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, vel + gimmickVel, Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), onGround, mapData);
	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), onGround, mapData);


	// 差分(押し戻された値を座標から引く)
	//centerPos += upperPlayerPos - upperPlayerPosBuff;

	// ボスとプレイヤーの角度
	float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
	bossAngle = fabs(bossAngle);

	// 当たった位置に応じて処理を分ける。
	if (intersectedLine == INTERSECTED_TOP) {

		// 下に当たった場合
		HitMapChipBottom();

		// マップチップの上にあたっていたということは、ボスが下方向にいればOK！
		// 下方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RIGHT) {

		// 左に当たった場合
		HitMapChipLeft();

		// マップチップの右にあたっていたということは、ボスが左方向にいればOK！
		// 左方向の具体的な値は
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_BOTTOM) {

		// 上に当たった場合
		HitMapChipTop();

		// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
		// 上方向の具体的な値は
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_LEFT) {

		// 右に当たった場合
		HitMapChipRight();

		// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
		// 右方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RISE) {

		// のし上がりの場所に当たった場合
		vel.y *= 0.5f;
		gravity = 0;

	}

	//if (intersectedLine == INTERSECTED_NONE) {

	//	Vec2<float> downnerPlayerPos = centerPos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//	Vec2<float> downnerPlayerPosBuff = upperPlayerPos;
	//	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(downnerPlayerPos, prevFrameCenterPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), onGround, mapData);

	//	// 差分(押し戻された値を座標から引く)
	//	centerPos += downnerPlayerPos - downnerPlayerPosBuff;

	//	// 当たった位置に応じて処理を分ける。
	//	if (intersectedLine == INTERSECTED_TOP) {

	//		// 下に当たった場合
	//		HitMapChipBottom();

	//		// マップチップの上にあたっていたということは、ボスが下方向にいればOK！
	//		// 下方向の具体的な値は
	//		const float MIN_ANGLE = 0.785398f;
	//		const float MAX_ANGLE = 2.35619f;

	//		// 角度がこの値の間だったら
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// 引っかかっている。
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_RIGHT) {

	//		// 左に当たった場合
	//		HitMapChipLeft();

	//		// マップチップの右にあたっていたということは、ボスが左方向にいればOK！
	//		// 左方向の具体的な値は
	//		const float MIN_ANGLE = 2.35619f;
	//		const float MAX_ANGLE = 3.92699f;

	//		// 角度がこの値の間だったら
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// 引っかかっている。
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_BOTTOM) {

	//		// 上に当たった場合
	//		HitMapChipTop();

	//		// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
	//		// 上方向の具体的な値は
	//		const float MIN_ANGLE = 3.92699f;
	//		const float MAX_ANGLE = 5.49779f;

	//		// 角度がこの値の間だったら
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// 引っかかっている。
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_LEFT) {

	//		// 右に当たった場合
	//		HitMapChipRight();

	//		// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
	//		// 右方向の具体的な値は
	//		const float MIN_ANGLE = 0.785398f;
	//		const float MAX_ANGLE = 5.49779f;

	//		// 角度がこの値の間だったら
	//		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

	//			// 引っかかっている。
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_RISE) {

	//		// のし上がりの場所に当たった場合
	//		vel.y *= 0.5f;
	//		gravity = 0;

	//	}

	//}

}

void Player::CheckHitSize(const Vec2<float> &checkPos, const vector<vector<int>> &mapData, const Vec2<float> &bossPos, bool &isHitMapChip)
{

	// マップチップ目線でどこに当たったか
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;

	// マップチップとプレイヤーの当たり判定 絶対に被せないための処理 中央
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	const float OFFSET = 1.0f;

	// マップチップとプレイヤーの当たり判定 絶対に被せないための処理 上
	Vec2<float> centerPosBuff = centerPos + Vec2<float>(0, -PLAYER_HIT_SIZE.y + OFFSET);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (-PLAYER_HIT_SIZE.y + OFFSET);
	centerPosBuff = centerPos + Vec2<float>(0, -PLAYER_HIT_SIZE.y + OFFSET);
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (-PLAYER_HIT_SIZE.y + OFFSET);
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// マップチップとプレイヤーの当たり判定 絶対に被せないための処理 下
	centerPosBuff = centerPos + Vec2<float>(0, PLAYER_HIT_SIZE.y - OFFSET);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (PLAYER_HIT_SIZE.y - OFFSET);
	centerPosBuff = centerPos + Vec2<float>(0, PLAYER_HIT_SIZE.y - OFFSET);
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (PLAYER_HIT_SIZE.y - OFFSET);
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// マップチップとプレイヤーの当たり判定 絶対に被せないための処理 右
	centerPosBuff = centerPos + Vec2<float>(PLAYER_HIT_SIZE.x - OFFSET, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	centerPosBuff = centerPos + Vec2<float>(PLAYER_HIT_SIZE.x - OFFSET, 0);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// マップチップとプレイヤーの当たり判定 絶対に被せないための処理 左
	centerPosBuff = centerPos + Vec2<float>(-PLAYER_HIT_SIZE.x + OFFSET, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	centerPosBuff = centerPos + Vec2<float>(-PLAYER_HIT_SIZE.x + OFFSET, 0);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// ボスとプレイヤーの角度
	float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
	bossAngle = fabs(bossAngle);

	// 当たったフラグによって処理を分ける。
	if (isHitTop) {

		// マップチップの上にあたっていたということは、ボスが下方向にいればOK！
		// 下方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	if (isHitBottom) {

		// マップチップの下にあたっていたということは、プレイヤーが上方向にいればOK！
		// 上方向の具体的な値は
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	if (isHitLeft) {

		// マップチップの左にあたっていたということは、プレイヤーが右方向にいればOK！
		// 右方向の具体的な値は
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// 角度がこの値の間だったら
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}
	if (isHitRight) {

		// マップチップの右にあたっていたということは、ボスが左方向にいればOK！
		// 左方向の具体的な値は
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// 角度がこの値の間だったら
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// 引っかかっている。
			isHitMapChip = true;

		}

	}

}
