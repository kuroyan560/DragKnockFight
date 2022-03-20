#include "Player.h"
#include "PlayerHand.h"
#include "BulletMgr.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "BulletParticleMgr.h"
#include <cmath>

#include"TexHandleMgr.h"
#include"UsersInput.h"
#include"DrawFunc.h"
#include"WinApp.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

Player::Player()
{
	/*====== コンストラクタ =====*/

	// プレイヤーの位置をセット
	centerPos = GetGeneratePos();

	// 移動量をリセット
	vel = {};

	// 重力をリセット
	gravity = 0;

	// 接地していないフラグをリセット
	onGround = false;

	// 腕をセット
	static const float OFFSET = -8.0f;
	lHand = make_unique<PlayerHand>();
	lHand->Init(-PLAYER_SIZE.x + OFFSET);
	rHand = make_unique<PlayerHand>();
	rHand->Init(PLAYER_SIZE.x - OFFSET);

	// 連射タイマーを初期化
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// 重力無効化タイマーを初期化。
	gravityInvalidTimer = 0;

	isPrevFrameShotBeacon = false;
	firstShot = false;

	// 画像をロード
	playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	// 壁フラグを初期化
	isWallRight = false;
	isWallLeft = false;

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

}

Player::~Player()
{
}

void Player::Init(const Vec2<float>& INIT_POS)
{

	/*===== 初期化処理 =====*/

	// プレイヤーの位置をセット
	centerPos = INIT_POS;
	prevFrameCenterPos = INIT_POS;

	// 移動量をリセット
	vel = {};

	// 重力をリセット
	gravity = 0;

	// 接地していないフラグをリセット
	onGround = false;
	firstShot = false;

	// 腕をセット
	lHand->Init(-PLAYER_SIZE.x);
	rHand->Init(PLAYER_SIZE.x);

	// 連射タイマーを初期化
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// 重力無効化タイマーを初期化。
	gravityInvalidTimer = 0;

	// 壁フラグを初期化
	isWallRight = false;
	isWallLeft = false;

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

}

void Player::Update(const vector<vector<int>> mapData)
{

	/*===== 入力処理 =====*/

	// 入力に関する更新処理を行う。
	Input(mapData);

	/*===== 更新処理 =====*/

	// 移動に関する処理
	Move();

	// 重力に関する更新処理
	UpdateGravity();

	// 連射タイマーを更新
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// 腕を更新
	lHand->Update(centerPos);
	rHand->Update(centerPos);

	// 弾を更新
	BulletMgr::Instance()->Update();

	// 入力を受け付けないフラグを初期化
	isWallRight = false;
	isWallLeft = false;

	// プレイヤーのすぐ一個上のマップチップ座標を検索する。
	int mapX = centerPos.x / MAP_CHIP_SIZE;
	int mapY = (centerPos.y - PLAYER_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= mapData.size()) mapY = mapData.size() - 1;

	// 一個上のマップチップがブロックで、X軸方向の移動量が一定以上だったらパーティクルを生成する。
	if (mapData[mapY][mapX] > 0 && mapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - PLAYER_SIZE.y), Vec2<float>(0, -1));

	// 壁ズリのパーティクルを生成。
	if (isSlippingWall[PLAYER_TOP]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - PLAYER_SIZE.y), Vec2<float>(0, -1));
	if (isSlippingWall[PLAYER_RIGHT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + PLAYER_SIZE.x, centerPos.y), Vec2<float>(1, 0));
	if (isSlippingWall[PLAYER_BOTTOM]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + PLAYER_SIZE.y), Vec2<float>(0, 1));
	if (isSlippingWall[PLAYER_LEFT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - PLAYER_SIZE.x, centerPos.y), Vec2<float>(-1, 0));

	// 壁ズリフラグを初期化。
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// 一定時間入力がなかったら腕を初期位置に戻すタイマーを更新。
	if (handReturnTimer > 0) {
		--handReturnTimer;
	}
	// 何かしらの移動量が存在したらNoInputを初期化する。
	if (vel.x != 0 || vel.y != 0 || gimmickVel.x != 0 || gimmickVel.y != 0) handReturnTimer = DEF_HAND_RETURN_TIMER;
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
	gimmickVel = {};

	// 入力されてから数フレームを取得するタイマーを更新。
	if (asSoonAsInputTimer > 0) --asSoonAsInputTimer;

}

void Player::Draw()
{
	static enum { LEFT, RIGHT, DEFAULT = RIGHT, DIR_NUM }DIR = DEFAULT;
	if (vel.x < 0)DIR = LEFT;
	if (0 < vel.x)DIR = RIGHT;

	static const int HAND_GRAPH[DIR_NUM] =
	{
		TexHandleMgr::LoadGraph("resource/IntoTheAbyss/hand_L.png"),
		TexHandleMgr::LoadGraph("resource/IntoTheAbyss/hand_R.png"),
	};

	/*===== 描画処理 =====*/

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	// プレイヤーの描画処理
	Vec2<float>leftUp = { centerPos.x * ScrollMgr::Instance()->zoom - PLAYER_SIZE.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom - PLAYER_SIZE.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	Vec2<float>rightBottom = { centerPos.x * ScrollMgr::Instance()->zoom + PLAYER_SIZE.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom + PLAYER_SIZE.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	const float expRate = EXT_RATE * ScrollMgr::Instance()->zoom;

	if (DIR == RIGHT)
	{
		rHand->Draw(expRate, HAND_GRAPH[DEFAULT ? RIGHT : LEFT], DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f });
	}
	else if (DIR == LEFT)
	{
		lHand->Draw(expRate, HAND_GRAPH[DEFAULT ? LEFT : RIGHT], DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f });
	}

	//胴体
	//DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(playerGraph), AlphaBlendMode_Trans, { DIR == LEFT,false });
	DrawFunc::DrawRotaGraph2D(centerPos * ScrollMgr::Instance()->zoom - scrollShakeZoom, expRate, 0.0f, TexHandleMgr::GetTexBuffer(playerGraph),
		{ 0.5f,0.5f }, AlphaBlendMode_Trans, { DIR == LEFT,false });

	if (DIR == RIGHT)
	{
		lHand->Draw(expRate, HAND_GRAPH[DEFAULT ? LEFT : RIGHT], DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f });
	}
	else if (DIR == LEFT)
	{
		rHand->Draw(expRate, HAND_GRAPH[DEFAULT ? RIGHT : LEFT], DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f });
	}

	// 弾を描画
	BulletMgr::Instance()->Draw();

}

void Player::CheckHit(const vector<vector<int>> mapData, vector<Bubble>& bubble, TimeStopTestBlock& testBlock)
{

	/*===== マップチップとプレイヤーとの当たり判定全般 =====*/

	{

		// マップチップとプレイヤーの当たり判定 絶対に貫通させない為の処理
		INTERSECTED_LINE intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, vel, PLAYER_SIZE, onGround, mapData);

		// 当たった位置に応じて処理を分ける。
		if (intersectedLine == INTERSECTED_TOP) {

			// 下に当たった場合
			HitMapChipBottom();

		}
		else if (intersectedLine == INTERSECTED_RIGHT) {

			// 左に当たった場合
			HitMapChipLeft();

		}
		else if (intersectedLine == INTERSECTED_BOTTOM) {

			// 上に当たった場合
			HitMapChipTop();

		}
		else if (intersectedLine == INTERSECTED_LEFT) {

			// 右に当たった場合
			HitMapChipRight();

		}
		else if (intersectedLine == INTERSECTED_RISE) {

			// のし上がりの場所に当たった場合
			vel.y *= 0.5f;
			gravity = 0;

		}

		// マップチップとプレイヤーの当たり判定 絶対に被せないための処理
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_TOP);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_BOTTOM);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_LEFT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_RIGHT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();

		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_TOP);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_BOTTOM);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_LEFT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_RIGHT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();

		// 左右に当たった際に壁釣りさせるための処理。
		int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		int xChip = (centerPos.x - PLAYER_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		// プレイヤーの左側がマップチップだったら
		if (yChip > 0 && mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			HitMapChipLeft();
		}
		xChip = (centerPos.x + PLAYER_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		if (xChip >= mapData[yChip].size() - 1) xChip = mapData[yChip].size() - 1;
		// プレイヤーの右側がマップチップだったら
		if (yChip > 0 && mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			HitMapChipRight();
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
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_SIZE.x,MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_SIZE.y };

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

	{

		// 瞬間移動の短槍の当たり判定を行う。
		if (!rHand->teleportPike.isHitWall && rHand->teleportPike.isActive) {
			bool onGround = false;
			INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(rHand->teleportPike.pos, rHand->teleportPike.prevFramePos, rHand->teleportPike.forwardVec, Vec2<float>(rHand->teleportPike.SCALE, rHand->teleportPike.SCALE), onGround, mapData);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

			// 押し戻し
			Vec2<float> scale = Vec2<float>(rHand->teleportPike.SCALE * 2.0f, rHand->teleportPike.SCALE * 2.0f);
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_BOTTOM);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_TOP);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_LEFT);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_RIGHT);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		}

		// 時間停止の短槍の当たり判定を行う。
		if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
			bool onGround = false;
			INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(lHand->timeStopPike.pos, lHand->timeStopPike.prevFramePos, lHand->timeStopPike.forwardVec, Vec2<float>(lHand->timeStopPike.SCALE, lHand->timeStopPike.SCALE), onGround, mapData);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }

			// 押し戻し
			Vec2<float> scale = Vec2<float>(lHand->timeStopPike.SCALE * 2.0f, lHand->timeStopPike.SCALE * 2.0f);
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_BOTTOM);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_TOP);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_LEFT);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_RIGHT);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		}

		// 時間停止の短槍とテスト用ブロックの当たり判定を行う。
		if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
			// 時間停止の短槍の座標
			Vec2<float> pikePos = lHand->timeStopPike.pos;
			// ブロックの当たり判定 判定は円で適当です。
			if (testBlock.pos.Distance(lHand->timeStopPike.pos) <= testBlock.SCALE * 3.0f) {

				// 短槍を止める。
				lHand->timeStopPike.isHitWall = true;
				// ブロックを止める。
				testBlock.isTimeStop = true;

			}
			else {
				// 短槍が消えたら時間停止を解除するため。
				testBlock.isTimeStop = false;
			}
		}
		else {
			// 短槍が消えたら時間停止を開場するため。
			testBlock.isTimeStop = false;
		}

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
			bool isHitBubbleX = centerPos.Distance(bubble[index].pos) <= PLAYER_SIZE.x + bubble[index].RADIUS;
			bool isHitBubbleY = centerPos.Distance(bubble[index].pos) <= PLAYER_SIZE.y + bubble[index].RADIUS;

			// isHit
			if (!isHitBubbleX && !isHitBubbleY) continue;

			// Recovery of recoil
			rHand->isFirstShot = false;
			lHand->isFirstShot = false;

			// If there is an input
			if (inBubble && asSoonAsInputTimer >= 1) {

				// break a bubble
				bubble[index].isBreak = true;
				bubble[index].breakCoolTime = bubble[index].BREAK_COOL_TIME;
				inBubble = false;

			}
			else {

				vel = {};
				gravity = {};
				gimmickVel = {};

				// Get closer to the bubble
				centerPos.x += (bubble[index].pos.x - centerPos.x) / 5.0f;
				centerPos.y += (bubble[index].pos.y - centerPos.y) / 5.0f;

				inBubble = true;

			}

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

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_LEFT] = true;

	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

	}

	// 最初の一発は反動が強いフラグを初期化する。
	firstShot = false;

	// 入力を受け付けないフラグを設定
	isWallLeft = true;

	// 最初の一発フラグを初期化
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;

}

void Player::HitMapChipRight()
{

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_RIGHT] = true;

	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

	}

	// 最初の一発は反動が強いフラグを初期化する。
	firstShot = false;

	gravity *= 0.9f;

	// 入力を受け付けないフラグを設定
	isWallRight = true;

	// 最初の一発フラグを初期化
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;

}

void Player::HitMapChipBottom()
{

	// 接地フラグを立てる。
	onGround = true;

	// X軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.x) >= STOP_DEADLINE_X) {

		// 摩擦をつける。
		vel.y *= 0.95f;
		vel.x *= 0.95f;

		// 重力を無効化する。
		gravity = 0.5f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_BOTTOM] = true;

	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;

		// 重力を無効化する。
		gravity = 0.5f;

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

}

void Player::Input(const vector<vector<int>> mapData)
{

	/*===== 入力処理 =====*/

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
		handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	// LBが押されたら反動をつける。
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

		// 反動をつける。
		float rHandAngle = lHand->GetAngle();

		// Getした値は手の向いている方向なので、-180度する。
		rHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// 移動量を加算
		if (!lHand->isFirstShot) {

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
			ShakeMgr::Instance()->maxShakeAmount = ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT;

			// プレイヤーの腕を動かす。
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), true);

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
		BulletMgr::Instance()->Generate(lHand->GetPos(), lHand->GetAngle(), isFirstShot, false);

		// 連射タイマーをセット
		rapidFireTimerLeft = RAPID_FIRE_TIMER;

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// 入力タイマーをセット。
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

	}

	// RBが押されたら反動をつける。
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

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
			ShakeMgr::Instance()->maxShakeAmount = ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT;

			// プレイヤーの腕を動かす。
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), true);

		}
		else {
			// onGroundがtrueだったら移動量を加算しない。
			if (!onGround || sinf(lHandAngle) < 0) {
				vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
			}

			vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

			// プレイヤーの腕を動かす。
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);

		}

		// 弾を生成する。
		BulletMgr::Instance()->Generate(rHand->GetPos(), rHand->GetAngle(), isFirstShot, true);

		// 連射タイマーをセット
		rapidFireTimerRight = RAPID_FIRE_TIMER;

		// 一定時間入力がなかったら初期位置に戻す
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// 入力タイマーをセット。
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

	}

	// 移動速度が限界値を超えないようにする。
	if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;

	// RTが押されたら
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RT)) {

		// 瞬間移動の短槍に関する処理を行う。

		// 右腕の弾が発射されていなかったら。
		if (!rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			// クールタイムが0以下だったら
			if (rHand->pikeCooltime <= 0) {

				// 弾を発射する処理を行う。
				rHand->teleportPike.Generate(rHand->GetPos(), Vec2<float>(cosf(rHand->GetAngle()), sinf(rHand->GetAngle())), PIKE_TELEPORT);

			}

		}
		// ビーコンが発射されていたら。
		else if (rHand->teleportPike.isActive && (rHand->teleportPike.isHitWall || rHand->teleportPike.isHitWindow) && !isPrevFrameShotBeacon) {

			// プレイヤーを瞬間移動させる。
			centerPos = rHand->teleportPike.pos;

			ScrollMgr::Instance()->WarpScroll(centerPos);

			// プレイヤーの重力を無効化させる。
			gravity = 0;

			// 移動量を無効化する。
			vel = {};

			// 移動量を無効化する。
			gimmickVel = {};

			// 重力無効化タイマーを設定。
			gravityInvalidTimer = GRAVITY_INVALID_TIMER;

			// ビーコンを初期化する。
			rHand->teleportPike.Init();

			// ビーコンのクールタイムを設定
			rHand->pikeCooltime = rHand->PIKE_COOL_TIME;

		}

		isPrevFrameShotBeacon = true;

	}
	else if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {

		// 時間停止の短槍に関する処理を行う。

		// 左腕の弾が発射されていなかったら。
		if (!lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			// クールタイムが0以下だったら
			if (lHand->pikeCooltime <= 0) {

				// 弾を発射する処理を行う。
				lHand->timeStopPike.Generate(lHand->GetPos(), Vec2<float>(cosf(lHand->GetAngle()), sinf(lHand->GetAngle())), PIKE_TIMESTOP);

			}

		}
		// ビーコンが発射されていたら。
		else if (lHand->timeStopPike.isActive && (lHand->timeStopPike.isHitWall || lHand->timeStopPike.isHitWindow) && !isPrevFrameShotBeacon) {

			// ビーコンを初期化する。
			lHand->timeStopPike.Init();

			// ビーコンのクールタイムを設定
			lHand->pikeCooltime = lHand->PIKE_COOL_TIME;

		}

		isPrevFrameShotBeacon = true;

	}
	else {

		isPrevFrameShotBeacon = false;

	}

}

void Player::Move()
{

	/*===== 移動に関する処理 =====*/

	// 移動処理
	centerPos += vel;

	// ギミックから与えられる移動量を加算
	centerPos += gimmickVel;

	// スクロール量を更新
	//ScrollMgr::Instance()->honraiScrollAmount -= prevFrameCenterPos - centerPos;
	ScrollMgr::Instance()->CalucurateScroll(prevFrameCenterPos - centerPos);

	// 移動量を0に近付ける。
	vel.x -= vel.x / 25.0f;
	vel.y -= vel.y / 25.0f;

	// 中心座標を保存
	prevFrameCenterPos = centerPos;

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
	centerPos.y += gravity;

	// 接地フラグが立ってい且つ重力無効化タイマーが0以下だったらなかったら重力を加算。
	if (!onGround && gravityInvalidTimer <= 0) {

		// 重力を加算。
		gravity += ADD_GRAVITY;

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
	const float PLAYER_RIGHT_X = centerPos.x + PLAYER_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// 差分押し戻す。
		centerPos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}

	// 左側
	const float PLAYER_LEFT_X = centerPos.x - PLAYER_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// 差分押し戻す。
		centerPos.x -= PLAYER_LEFT_X;

		// X方向の移動量を無効化する。
		vel.x = 0;

	}


	// Y軸に関する押し戻し処理

	// 上側
	const float PLAYER_UP_Y = centerPos.y - PLAYER_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// 差分押し戻す。
		centerPos.y -= PLAYER_UP_Y;

		// Y方向の移動量を減らす。
		vel.y /= 2.0f;

	}

	// 下側
	const float PLAYER_DOWN_Y = centerPos.y + PLAYER_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// 差分押し戻す。
		centerPos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// 接地フラグを立てる。
		onGround = true;

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
