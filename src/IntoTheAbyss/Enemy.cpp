#include "Enemy.h"
#include "Enemy.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "BulletMgr.h"
#include "Bullet.h"
#include "Collider.h"
#include "ParticleMgr.h"

#include"WinApp.h"
#include"DrawFunc.h"
#include"KuroFunc.h"

Enemy::Enemy()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- コンストラクタ --*/

	// 各変数を初期化
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	isActive = false;
	//forwardVec = { 1.0f,0.0f };

}

void Enemy::Init()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- 初期化処理 --*/

	// 各変数を初期化
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	isActive = false;
	//forwardVec = { 1.0f,0.0f };

}

void Enemy::Generate(const ENEMY_ID& id, const vector<vector<int>>& mapData)
{

	this->id = id;
	isActive = true;
	if (id == ENEMY_BOSS) {

		size = SIZE_BIG;

	}
	else {

		size = SIZE_SMALL;

	}

	// HPをセット
	if (id == ENEMY_SMALL) {
		hitPoint = HITPOINT_SMALL;
	}
	else {
		hitPoint = 1;
	}

	if (id != ENEMY_BOSS) {

		// 空白のブロックにランダムで生成する。
		const int HEIGHT = mapData.size() - 1;
		const int WIDTH = mapData[0].size() - 1;
		const float CHIP_SIZE = 50.0f;

		while (true) {

			int indexX = KuroFunc::GetRand(WIDTH);
			int indexY = KuroFunc::GetRand(HEIGHT);
			int mapIndex = mapData[indexY][indexX];
			if (mapIndex == 0) {


				pos = { indexX * CHIP_SIZE - (CHIP_SIZE / 2.0f), indexY * CHIP_SIZE - (CHIP_SIZE / 2.0f) };

				break;

			}

		}

	}

}

void Enemy::Update(const Vec2<float>& playerPos)
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	//const int WIN_HEIGHT = WinApp::Instance()->GetExpandWinSize().y;

	/*-- 更新処理 --*/

	// 移動処理を行う。
	//pos += forwardVec * Vec2<float>(SPEED, SPEED);

	//// 押し戻し処理を行う。
	//if (pos.x + SIZE.x >= WIN_WIDTH) {

	//	// 押し戻す。
	//	pos.x = WIN_WIDTH - SIZE.x;

	//	// 移動方向を反転させる。
	//	forwardVec.x *= -1.0f;

	//}
	//if (pos.x - SIZE.x <= 0) {

	//	// 押し戻す。
	//	pos.x = SIZE.x;

	//	// 移動方向を反転させる。
	//	forwardVec.x *= -1.0f;

	//}

	// プレイヤーに近づける。
	if (id == ENEMY_BOSS) {
		pos.x += (playerPos.x - pos.x) / 100.0f;
		pos.y += (playerPos.y - pos.y) / 100.0f;
	}
	else if (id == ENEMY_SMALL) {
		pos.x += (playerPos.x - pos.x) / 60.0f;
		pos.y += (playerPos.y - pos.y) / 60.0f;
	}

	isHit = false;

	if (hitPoint <= 0) Init();

}

#include"D3D12App.h"
void Enemy::Draw()
{

	/*-- 描画処理 --*/

	Color col = Color(229, 128, 128, 255);

	if (id == ENEMY_SMALL || id == ENEMY_NOMOVEMENT) {

		col = Color(150, 100, 100, 255);

	}





	if (id == ENEMY_SMALL) {

		// HP
		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			col, D3D12App::Instance()->GetBackBuffFormat(), true);

		// 下地
		float hitpointPar = (float)hitPoint / HITPOINT_SMALL;
		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x - size.x + size.x * 2.0f * hitpointPar - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			Color(229, 208, 208, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	}
	else if (id == ENEMY_NOMOVEMENT) {

		// HP
		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			col, D3D12App::Instance()->GetBackBuffFormat(), true);

		// 下地
		float hitpointPar = (float)hitPoint / HITPOINT_SMALL;
		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			Color(229, 168, 168, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	}
	else {

		//DrawBox(pos.x - SIZE.x, pos.y - SIZE.y, pos.x + SIZE.x, pos.y + SIZE.y, GetColor(0xE5, 0x80, 0x80), TRUE);
		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			col, D3D12App::Instance()->GetBackBuffFormat(), true);

	}


	if (isHit) {

		DrawFunc::DrawBox2D({ pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			{ pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x,
			pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y },
			Color(255, 0, 0, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	}


}

void Enemy::CheckHitBullet()
{

	/*===== 弾と敵の当たり判定 =====*/

	const int BULLET_COUNT = BulletMgr::Instance()->BULLET_COUNT;
	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 弾が生成されていなかったら処理を飛ばす。
		if (!BulletMgr::Instance()->bullets[index]->isActive)continue;

		// 当たり判定を行う。
		Vec2<float> bulletPos = BulletMgr::Instance()->bullets[index]->pos;
		Vec2<float> bulletSize = { BulletMgr::Instance()->bullets[index]->MAX_RADIUS,BulletMgr::Instance()->bullets[index]->MAX_RADIUS };
		Vec2<float> bulletSpeed = { BulletMgr::Instance()->bullets[index]->speed,BulletMgr::Instance()->bullets[index]->speed };
		Vec2<float> bulletDir = BulletMgr::Instance()->bullets[index]->forwardVec;
		bool isHitVel = Collider::Instance()->CheckHitVel(bulletPos, BulletMgr::Instance()->bullets[index]->prevPos, bulletSpeed, bulletSize, pos, size);
		bool isHitTop = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, size, INTERSECTED_TOP) != INTERSECTED_NONE;
		bool isHitBottom = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, size, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
		bool isHitRight = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, size, INTERSECTED_RIGHT) != INTERSECTED_NONE;
		bool isHitLeft = Collider::Instance()->CheckHitSize(bulletPos, bulletSize, pos, size, INTERSECTED_LEFT) != INTERSECTED_NONE;

		// どこかしらが当たっていたら
		if (isHitVel || isHitTop || isHitBottom || isHitRight || isHitLeft) {


			// ノックバックをつける。
			if (id == ENEMY_SMALL || id == ENEMY_NOMOVEMENT) {

				pos += bulletDir * Vec2<float>(KNOCK_BACK, KNOCK_BACK);
				--hitPoint;

			}
			else {

				pos += bulletDir * Vec2<float>(KNOCK_BACK_BOSS, KNOCK_BACK_BOSS);

			}

			// 弾を消す
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->bullets[index]->pos, BulletMgr::Instance()->bullets[index]->forwardVec, BULLET);
			BulletMgr::Instance()->bullets[index]->Init();

			isHit = true;

		}

	}

}
