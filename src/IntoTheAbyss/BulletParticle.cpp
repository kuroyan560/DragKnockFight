#include "BulletParticle.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"

#include"KuroFunc.h"
#include"DrawFunc.h"

BulletParticle::BulletParticle()
{

	/*===== コンストラクタ =====*/

	pos = {};
	forwardVec = {};
	movedVel = {};
	alpha = 0;
	speed = 0;
	isAlive = false;

}

void BulletParticle::Init()
{

	/*===== 初期化処理 =====*/

	isAlive = false;

}

void BulletParticle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== 生成処理 =====*/

	// 座標を設定
	pos = generatePos;

	// 重ならないように切り上げする。
	pos.x = RoundUp(pos.x, SCALE);
	pos.y = RoundUp(pos.y, SCALE);
	scale = SCALE;

	// 進行方向を設定
	this->forwardVec = forwardVec;

	// 移動した量を初期化
	movedVel = {};

	// アルファ値を初期化
	alpha = 255.0f;

	// 移動量を初期化
	//speed = GetRand(DEF_SPEED * 100) / 100.0f;
	speed = KuroFunc::GetRand(DEF_SPEED * 100) / 100.0f;

	// 生存フラグを初期化
	isAlive = true;

}

void BulletParticle::Update()
{

	/*===== 更新処理 =====*/

	// 移動させる。
	movedVel += Vec2<float>(forwardVec.x * speed, forwardVec.y * speed);

	// 移動した量がサイズを上回ったらサイズを動かす。
	if (fabs(movedVel.x) >= scale) {

		// 符号を取得。
		int sign = signbit(movedVel.x) ? -1 : 1;

		// 座標を動かす。
		pos.x += scale * sign;

		// 移動した量を減らす。
		movedVel.x -= scale * sign;

	}
	// 移動した量がサイズを上回ったらサイズを動かす。Yバージョン
	if (fabs(movedVel.y) >= scale) {

		// 符号を取得。
		int sign = signbit(movedVel.y) ? -1 : 1;

		// 座標を動かす。
		pos.y += scale * sign;

		// 移動した量を減らす。
		movedVel.y -= SCALE * sign;

	}

	// スピードを下げる。
	if(speed > 0)speed -= SUB_SPEED;

	// スピードが規定値以下になったらアルファ値を下げ始める。
	if (speed <= 0.5f) {

		alpha -= alpha / 3.0f;

	}

	// アルファ値が一定以下になったら初期化する。
	if (alpha <= 10.0f) Init();

	if (alpha <= 123)scale = SCALE / 2.0f;

}

void BulletParticle::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	float drawScale = (scale / 2.0f) * ScrollMgr::Instance()->zoom;

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	//DrawBox(pos.x * ScrollMgr::Instance()->zoom - scale - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scale - scrollShakeZoom.y,
	//	pos.x * ScrollMgr::Instance()->zoom + scale - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + scale - scrollShakeZoom.y,
	//	GetColor(0xD9, 0x1A, 0x60), TRUE);

	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	Vec2<float>leftUp = { pos.x * ScrollMgr::Instance()->zoom - drawScale - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom - drawScale - scrollShakeZoom.y };
	Vec2<float>rightBottom = { pos.x * ScrollMgr::Instance()->zoom + drawScale - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom + drawScale - scrollShakeZoom.y };

	//DrawFunc::DrawBox2D(leftUp, rightBottom, Color(217, 26, 96, (int)alpha), true, AlphaBlendMode_Trans);
	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(255, 255, 255, 255), true, AlphaBlendMode_Trans);

}

