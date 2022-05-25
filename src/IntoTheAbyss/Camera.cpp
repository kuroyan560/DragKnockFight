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
		//�`���̈ʒu�����߂�
		const auto targetOnDraw = ScrollMgr::Instance()->Affect(target);
		//��ʒ����Ƃ̍��������߂�
		const auto differ = targetOnDraw - WinApp::Instance()->GetExpandWinCenter() - scrollAffect;

		//�߂Â��Ă���
		scrollAffect = KuroMath::Lerp(scrollAffect, -differ, lerpAmount);
	}
	else
	{
		scrollAffect = KuroMath::Lerp(scrollAffect, { 0,0 }, initZoom);
	}

	ScrollMgr::Instance()->zoom = KuroMath::Lerp(ScrollMgr::Instance()->zoom, zoom, lerpAmount);
}

void Camera::Focus(const Vec2<float>& TargetPos, const float& Zoom, const float& LerpAmount)
{
	target = TargetPos;
	zoom = Zoom;
	active = 1;
	lerpAmount = LerpAmount;
}
