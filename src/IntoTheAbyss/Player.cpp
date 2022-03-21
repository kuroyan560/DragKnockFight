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

	lHand = make_unique<PlayerHand>();
	rHand = make_unique<PlayerHand>();

	// 画像をロード
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	this->Init(GetGeneratePos());
}

Player::~Player()
{
}

void Player::Init(const Vec2<float>& INIT_POS)
{

	/*===== 初期化処理 =====*/

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
	lHand->Update(centerPos + anim.GetHandOffsetLeft());
	rHand->Update(centerPos + anim.GetHandOffsetRight());

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

	// 一個上のマップチップがブロックで、X軸方向の移動量が一定以上だったらパーティクルを生成する。
	if (mapData[mapY][mapX] > 0 && mapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1));

	// 壁ズリのパーティクルを生成。
	if (isSlippingWall[PLAYER_TOP]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1));
	if (isSlippingWall[PLAYER_RIGHT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x, centerPos.y), Vec2<float>(1, 0));
	if (isSlippingWall[PLAYER_BOTTOM]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y), Vec2<float>(0, 1));
	if (isSlippingWall[PLAYER_LEFT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x, centerPos.y), Vec2<float>(-1, 0));

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

	//ストレッチ更新
	UpdateStretch();

	//アニメーション更新
	anim.Update();
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
	Vec2<float>leftUp = { centerPos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	Vec2<float>rightBottom = { centerPos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	const float expRate = EXT_RATE * ScrollMgr::Instance()->zoom;

	if (DIR == RIGHT)
	{
		rHand->Draw(expRate, HAND_GRAPH[DEFAULT ? RIGHT : LEFT], DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f });
	}
	else if (DIR == LEFT)
	{
		lHand->Draw(expRate, HAND_GRAPH[DEFAULT ? LEFT : RIGHT], DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f });
	}

	//ストレッチ加算
	leftUp += stretch_LU;
	rightBottom += stretch_RB;

	//胴体
	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom , TexHandleMgr::GetTexBuffer(anim.GetGraphHandle()), AlphaBlendMode_Trans, { DIR == LEFT,false });
	//DrawFunc::DrawRotaGraph2D(centerPos * ScrollMgr::Instance()->zoom - scrollShakeZoom, expRate, 0.0f, TexHandleMgr::GetTexBuffer(playerGraph),
		//{ 0.5f,0.5f }, AlphaBlendMode_Trans, { DIR == LEFT,false });

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
		INTERSECTED_LINE intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, vel, PLAYER_HIT_SIZE, onGround, mapData);

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
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();

		// 左右に当たった際に壁釣りさせるための処理。
		int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		int xChip = (centerPos.x - PLAYER_HIT_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		// プレイヤーの左側がマップチップだったら
		if (yChip > 0 && mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			HitMapChipLeft();
		}
		xChip = (centerPos.x + PLAYER_HIT_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
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
			bool isHitBubbleX = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.x + bubble[index].RADIUS;
			bool isHitBubbleY = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.y + bubble[index].RADIUS;

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
	stretch_LU.x = 0.0f;

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_LEFT] = true;

		//摩擦用アニメーション
		anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		anim.ChangeAnim(ON_WALL_WAIT);
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
	stretch_RB.x = 0.0f;

	// Y軸の移動量の合計が一定以上だったら摩擦を作る。
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// 摩擦をつける。
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// 壁ズリフラグを設定
		isSlippingWall[PLAYER_RIGHT] = true;

		//摩擦用アニメーション
		anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//壁貼り付きアニメーション
		anim.ChangeAnim(ON_WALL_WAIT);
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
	stretch_RB.y = 0.0f;

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

		//摩擦用アニメーション
		anim.ChangeAnim(ON_GROUND_DASH);
	}
	else {

		// X方向の移動量を無効化する。
		vel.x = 0;
		vel.y = 0;

		// 重力を無効化する。
		gravity = 0.5f;

		//摩擦無いときはストレッチを弱くする
		stretch_RB.x /= STRETCH_DIV_RATE;
		stretch_LU.x /= STRETCH_DIV_RATE;

		//待機アニメーションに戻す
		anim.ChangeAnim(ON_GROUND_WAIT);
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

		//ストレッチ
		CalculateStretch(vel);
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

		//ストレッチ
		CalculateStretch(vel);
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

			//ストレッチ
			CalculateStretch(rHand->teleportPike.pos - centerPos);

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

void Player::CalculateStretch(const Vec2<float>& Move)
{
	Vec2<float> stretchRate = { abs(Move.x / MAX_RECOIL_AMOUNT),abs(Move.y / MAX_RECOIL_AMOUNT) };

	//X移動の勢いの方が強い
	if (stretchRate.y < stretchRate.x)
	{
		anim.ChangeAnim(ON_AIR_DASH_X);
	}
	//Y移動の勢いの方が強い
	else if (stretchRate.x < stretchRate.y)
	{
		anim.ChangeAnim(ON_AIR_DASH_Y);
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

	//左右移動時
	if (Move.x != 0.0f)
	{
		//上下が縮む
		stretch_LU.y += MAX_STRETCH.y * stretchRate.x;
		if (MAX_STRETCH.y < stretch_LU.y)stretch_LU.y = MAX_STRETCH.y;
		stretch_RB.y -= MAX_STRETCH.y * stretchRate.x;
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
	if (Move.y != 0.0f)
	{
		//左右が縮む
		stretch_LU.x += MAX_STRETCH.x * stretchRate.y;
		if (MAX_STRETCH.x < stretch_LU.x)stretch_LU.x = MAX_STRETCH.x;
		stretch_RB.x -= MAX_STRETCH.x * stretchRate.y;
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
