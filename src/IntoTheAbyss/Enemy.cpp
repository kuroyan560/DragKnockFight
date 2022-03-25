#include "Enemy.h"

#include"WinApp.h"
#include"DrawFunc.h"

Enemy::Enemy()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- コンストラクタ --*/

	// 各変数を初期化
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	forwardVec = { 1.0f,0.0f };

}

void Enemy::Init()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*-- 初期化処理 --*/

	// 各変数を初期化
	pos = { WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f - 100.0f };
	forwardVec = { 1.0f,0.0f };

}

void Enemy::Update()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	//const int WIN_HEIGHT = WinApp::Instance()->GetExpandWinSize().y;

	/*-- 更新処理 --*/

	// 移動処理を行う。
	pos += forwardVec * Vec2<float>(SPEED, SPEED);

	// 押し戻し処理を行う。
	if (pos.x + SIZE.x >= WIN_WIDTH) {

		// 押し戻す。
		pos.x = WIN_WIDTH - SIZE.x;

		// 移動方向を反転させる。
		forwardVec.x *= -1.0f;

	}
	if (pos.x - SIZE.x <= 0) {

		// 押し戻す。
		pos.x = SIZE.x;

		// 移動方向を反転させる。
		forwardVec.x *= -1.0f;

	}

}

#include"D3D12App.h"
void Enemy::Draw()
{

	/*-- 描画処理 --*/
	//DrawBox(pos.x - SIZE.x, pos.y - SIZE.y, pos.x + SIZE.x, pos.y + SIZE.y, GetColor(0xE5, 0x80, 0x80), TRUE);
	DrawFunc::DrawBox2D({ pos.x - SIZE.x, pos.y - SIZE.y }, { pos.x + SIZE.x, pos.y + SIZE.y }, Color(229, 128, 128, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

}
