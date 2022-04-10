#include "StunEffect.h"
#include "Camera.h"
#include "SlowMgr.h"

StunEffect::StunEffect()
{

	/*===== コンストラクタ =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Init()
{

	/*===== 初期化処理 =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Activate(Vec2<float>& focusPos)
{

	/*===== 生成処理 =====*/

	focusTime = FOCUS_TIME;
	this->focusPos = &focusPos;
	isActive = true;

}

void StunEffect::Update()
{

	/*===== 更新処理 =====*/

	// 有効化されていなかったら処理を飛ばす。
	if (!isActive) {

		SlowMgr::Instance()->slowAmount += (1.0f - SlowMgr::Instance()->slowAmount) / 10.0f;

		return;

	}

	// タイマーを更新する。
	if (0 < focusTime)--focusTime;

	// フォーカスする。
	Camera::Instance()->Focus(*focusPos, FOCUS_ZOOM);

	// スローをかける。
	SlowMgr::Instance()->slowAmount += (0.0f - SlowMgr::Instance()->slowAmount) / 10.0f;

	// タイマーが0になったら。
	if (focusTime <= 0) {

		// 有効化とフォーカスを切る。
		Camera::Instance()->Init();
		isActive = false;

	}

}
