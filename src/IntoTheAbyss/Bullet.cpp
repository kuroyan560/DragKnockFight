#include "Bullet.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "SlowMgr.h"

#include"KuroFunc.h"
#include"DrawFunc.h"

#include"TexHandleMgr.h"

Bullet::Bullet()
{

	/*-- コンストラクタ --*/

	// 各種変数を初期化。
	pos = {};
	forwardVec = {};
	isActive = false;

	bulletHitBox = std::make_shared<SphereCollision>();
	bulletHitBox->center = &pos;
	bulletHitBox->radius = 10.0f;


}

void Bullet::Init()
{

	/*-- 初期化処理 --*/

	// 生存フラグを折る。
	isActive = false;
}

void Bullet::Generate(const int& graphHandle, const Vec2<float>& generatePos, const Vec2<float> forwardVec, const float& speed)
{

	/*-- 生成処理 --*/
	if (graphHandle != -1)
	{
		graph = graphHandle;
	}

	// 座標をセット
	pos = generatePos;
	prevPos = generatePos;

	// 進行方向ベクトルをセット
	this->forwardVec = forwardVec;

	// 一応正規化
	this->forwardVec.Normalize();

	// 生存フラグをたてる。
	isActive = true;

	alpha = 255;

	this->speed = speed;

	deadTimer = DEAD_TIMER;

	radius = MAX_RADIUS;

	// 最初の1Frameが経過したか
	isFirstFrame = false;
}

void Bullet::Update()
{

	/*-- 更新処理 --*/

	if (!isActive) return;

	// 移動させる。

	if (isFirstFrame) {
		prevPos = pos;
		pos += forwardVec * Vec2<float>(speed * SlowMgr::Instance()->slowAmount, speed * SlowMgr::Instance()->slowAmount);
	}

	isFirstFrame = true;

	//alpha -= 10;
	//speed += -speed / 10.0f;
	//if (speed < 0.3f) Init();

	--deadTimer;
	if (deadTimer <= 0) {

		radius -= 1.0f;

		if (radius <= 0) {
			Init();
		}

	}

}

#include"D3D12App.h"
#include"TexHandleMgr.h"
void Bullet::Draw()
{

	/*-- 描画処理 --*/

	if (!isActive) return;

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	//// 外側を描画
	//DrawBox(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x - MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y - MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x + MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y + MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	GetColor(0xD9, 0x1A, 0x60), TRUE);

	//// 内側を描画
	////DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	////	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	////	MAX_RADIUS * ScrollMgr::Instance()->zoom, GetColor(0xD9, 0x1A, 0x60), TRUE);

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	Vec2<float>leftUp = { pos.x - radius,pos.y - radius };
	leftUp = ScrollMgr::Instance()->Affect(leftUp);
	Vec2<float>rightBottom = { pos.x + radius,pos.y + radius };
	rightBottom = ScrollMgr::Instance()->Affect(rightBottom);

	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(graph));
	//DrawFunc::DrawBox2D(leftUp, rightBottom, Color(217, 26, 96, (int)alpha), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);
}
