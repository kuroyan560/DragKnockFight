#include "Camera.h"

void Camera::Init()
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
void Camera::Update()
{

	ScrollMgr::Instance()->zoom = KuroMath::Lerp(ScrollMgr::Instance()->zoom, zoom, lerpAmount);

	if (active)
	{
		//描画上の位置を求める
		const auto targetOnDraw = ScrollMgr::Instance()->Affect(target);
		//画面中央との差分を求める
		const auto differ = targetOnDraw - WinApp::Instance()->GetExpandWinCenter() - scrollAffect;

		int a = 0;

		//近づいていく
		scrollAffect = KuroMath::Lerp(scrollAffect, -differ, lerpAmount);
		//scrollAffect = -differ;
	}
	else
	{
		scrollAffect = KuroMath::Lerp(scrollAffect, { 0,0 }, initZoom);
	}
}

void Camera::Focus(const Vec2<float>& TargetPos, const float& Zoom, const float& LerpAmount)
{
	target = TargetPos;
	zoom = Zoom;
	active = 1;
	lerpAmount = LerpAmount;
}
