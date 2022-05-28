#include "LocalCamera.h"

void LocalCamera::Init()
{
	active = 0;
	scrollAffect = { 0,0 };
	//zoomAffect = 0.0f;
	zoom = initZoom;
	lerpAmount = 0.1f;
}

#include"ShakeMgr.h"
#include"KuroMath.h"
#include"WinApp.h"
void LocalCamera::Update()
{

	ScrollMgr::Instance()->zoom = KuroMath::Lerp(ScrollMgr::Instance()->zoom, zoom, lerpAmount);

	if (active)
	{
		//描画上の位置を求める
		const auto targetondraw = ScrollMgr::Instance()->Affect(target);
		//画面中央との差分を求める
		const auto differ = targetondraw - WinApp::Instance()->GetExpandWinCenter() - scrollAffect;

		int a = 0;

		//const auto differ = (target - ScrollMgr::Instance()->scrollAmount) - scrollAffect;

		//近づいていく
		//scrollAffect.x = KuroMath::Lerp(scrollAffect.x, -differ.x, lerpAmount);
		//scrollAffect.y = KuroMath::Lerp(scrollAffect.y, -differ.y, lerpAmount);
		scrollAffect = -differ;
	}
	else
	{
		scrollAffect.x = KuroMath::Lerp(scrollAffect.x, 0, initZoom);
		scrollAffect.y = KuroMath::Lerp(scrollAffect.y, 0, initZoom);
	}
}

void LocalCamera::Focus(const Vec2<float> &TargetPos, const float &Zoom, const float &LerpAmount)
{
	target = TargetPos;
	zoom = Zoom;
	active = 1;
	lerpAmount = LerpAmount;
}

void LocalCamera::Zoom(const Vec2<float> &PLAYER_POS, const Vec2<float> &BOSS_POS, float MIN_ZOOM_SIZE)
{
	//互いの距離でカメラのズーム率を変える。
	float distance = PLAYER_POS.Distance(BOSS_POS);

	//最大距離
	const float MAX_ADD_ZOOM = 3500.0f;

	float zoomRate = 1.0f;
	float deadLine = 1200.0f;//この距離以下はズームしない

	// 限界より伸びていたら。
	if (MAX_ADD_ZOOM < distance)
	{
		zoomRate = 1.0f;
	}
	else if (deadLine <= distance)
	{
		zoomRate = (distance - deadLine) / MAX_ADD_ZOOM;
	}
	else
	{
		zoomRate = 0.0f;
	}
	static const float ZOOM_OFFSET = -0.01f;		// デフォルトで少しだけカメラを引き気味にする。
	Camera::Instance()->zoom = 0.5f - zoomRate + ZOOM_OFFSET;

	// カメラのズームが0.20f未満にならないようにする。
	float minZoomValue = MIN_ZOOM_SIZE;
	if (Camera::Instance()->zoom < minZoomValue)
	{
		Camera::Instance()->zoom = minZoomValue;
	}
}
