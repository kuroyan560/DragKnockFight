#include "Camera.h"

void Camera::Init()
{
	active = 0;
	scrollAffect = { 0,0 };
	//zoomAffect = 0.0f;
	zoom = initZoom;
}


#include"ShakeMgr.h"
#include"KuroMath.h"
#include"WinApp.h"
void Camera::Update()
{
	if (active)
	{
		//•`‰æã‚ÌˆÊ’u‚ð‹‚ß‚é
		const auto targetOnDraw = ScrollMgr::Instance()->Affect(target);
		//‰æ–Ê’†‰›‚Æ‚Ì·•ª‚ð‹‚ß‚é
		const auto differ =  targetOnDraw - WinApp::Instance()->GetExpandWinCenter() - scrollAffect;

		//‹ß‚Ã‚¢‚Ä‚¢‚­
		scrollAffect = KuroMath::Lerp(scrollAffect, -differ, 0.1f);
	}
	else
	{
		scrollAffect = KuroMath::Lerp(scrollAffect, { 0,0 }, initZoom);
	}

	ScrollMgr::Instance()->zoom = KuroMath::Lerp(ScrollMgr::Instance()->zoom, zoom, 0.1f);
}

void Camera::Focus(const Vec2<float>& TargetPos, const float& Zoom)
{
	target = TargetPos;
	zoom = Zoom;
	active = 1;
}
