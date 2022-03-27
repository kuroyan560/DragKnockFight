#include "Beacon.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "SlowMgr.h"

#include"WinApp.h"
#include"DrawFunc.h"

Beacon::Beacon()
{

	/*===== コンストラクタ =====*/

	pos = {};
	forwardVec = {};
	speed = 0;
	isActive = false;
	isHitWall = false;
	prevFrameScroll = {};

}

void Beacon::Init()
{

	/*===== 初期化処理 =====*/

	pos = { -1000,-1000 };
	isActive = false;
	isHitWall = false;
	isHitWindow = false;
	stopTargetPos = nullptr;

}

void Beacon::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwadVec, const PIKE_ID& id)
{

	/*===== 生成処理 =====*/

	pos = generatePos;
	this->forwardVec = forwadVec;
	isActive = true;
	isHitWall = false;
	isHitWindow = false;
	stopTargetPos = nullptr;
	gimmickVel = {};

	// 識別IDを設定
	this->id = id;

}

void Beacon::Update()
{

	/*===== 更新処理 =====*/

	// 壁にあたっていなかったら移動させる。
	if (!isHitWall) {

		// 座標を保存
		prevFramePos = pos;

		pos += Vec2<float>(forwardVec.x * MAX_SPEED * SlowMgr::Instance()->playerDeadSlow, forwardVec.y * MAX_SPEED * SlowMgr::Instance()->playerDeadSlow);

		//// 座標が画面外にいったらその場所で止める。
		//const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
		//const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;
		//if (pos.x - ScrollMgr::Instance()->scrollAmount.x - SCALE * 2.0f <= 0 || pos.x - ScrollMgr::Instance()->scrollAmount.x + SCALE * 2.0f >= WIN_WIDTH ||
		//	pos.y - ScrollMgr::Instance()->scrollAmount.y - SCALE * 2.0f <= 0 || pos.y - ScrollMgr::Instance()->scrollAmount.y + SCALE * 2.0f >= WIN_HEIGHT) {

		//	isHitWall = true;
		//	isHitWindow = true;

		//	// スクロール量を保存。
		//	prevFrameScroll = ScrollMgr::Instance()->scrollAmount;

		//}

	}

	if (isHitWall && stopTargetPos != nullptr) {

		*stopTargetPos = 0;

	}

	// ウィンドウに刺さっていたら
	if (isHitWindow) {

		// スクロール量から移動量を求める。
		Vec2<float> moveAmount = ScrollMgr::Instance()->scrollAmount - prevFrameScroll;

		// 移動させる。
		pos += moveAmount;

		// スクロール量を保存。
		prevFrameScroll = ScrollMgr::Instance()->scrollAmount;

	}

	pos += gimmickVel;
	gimmickVel = {};

}

void Beacon::Draw()
{

	/*===== 描画処理 =====*/

	//unsigned int color = GetColor(0x18, 0xB5, 0x76);
	Color color = Color(24, 181, 118, 255);
	//unsigned int outerColor = GetColor(0x72, 0x74, 0xA1);s
	Color outerColor = Color(114, 116, 161, 255);

	// 時間停止の短槍だったら色を変える。
	//if(id == PIKE_TIMESTOP) color = GetColor(0xD9, 0x1A, 0x60);
	if (id == PIKE_TIMESTOP) color = Color(217, 26, 96, 255);

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	//DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	SCALE * ScrollMgr::Instance()->zoom, color, TRUE);
	//DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	SCALE * ScrollMgr::Instance()->zoom, outerColor, FALSE, 2);

	Vec2<float>center = { pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };

	DrawFunc::DrawCircle2D(center, SCALE * ScrollMgr::Instance()->zoom, color, TRUE);
	DrawFunc::DrawCircle2D(center, SCALE * ScrollMgr::Instance()->zoom, outerColor, FALSE, 2);

}
