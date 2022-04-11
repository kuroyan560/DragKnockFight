#include "StunEffect.h"
#include "Camera.h"
#include "SlowMgr.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "KuroMath.h"
#include "WinApp.h"
#include "FaceIcon.h"

StunEffect::StunEffect()
{

	/*===== コンストラクタ =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

	enemyBreakHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	playerBreakHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_player.png");

}

void StunEffect::Init()
{

	/*===== 初期化処理 =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Activate(Vec2<float>& focusPos, const Vec2<float>& stunPos, const Vec2<float>& iconPos, const bool& isBoss)
{

	/*===== 生成処理 =====*/

	//focusTime = FOCUS_TIME;
	this->focusPos = &focusPos;
	this->iconPos = iconPos;
	isActive = true;
	status = StunEffect::STATUS_GENERATE;
	breakUITimer = 0;
	breakUIRadian = DEF_RADIAN + DEF_RADIAN / BREAK_UI_GENERATE_TIMER;


	// 引数のstunPosはワールド座標系なので、ローカル座標系に変換する。
	breakUIPos = stunPos - ScrollMgr::Instance()->scrollAmount;
	breakUIPosDef = breakUIPos;

	this->isBoss = isBoss;
	if (isBoss) {
		useHandle = enemyBreakHandle;
	}
	else {
		useHandle = playerBreakHandle;
	}

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
	//if (0 < focusTime)--focusTime;

	// フォーカスする。
	Camera::Instance()->Focus(*focusPos + Vec2<float>(0, -100), FOCUS_ZOOM);

	// スローをかける。
	SlowMgr::Instance()->slowAmount += (0.0f - SlowMgr::Instance()->slowAmount) / 10.0f;


	// switch内部で使用する変数。
	float easingAmount = 0;
	Vec2<float> windowCenterPos = { (float)WinApp::Instance()->GetWinCenter().x,(float)WinApp::Instance()->GetWinCenter().y };
	Vec2<float> changePos = {};

	// 現在のステータスに応じて処理を変える。
	switch (status)
	{
	case StunEffect::STATUS_GENERATE:

		/*-- 生成して画面中央に向かう処理 --*/

		// イージング量を求める。
		easingAmount = KuroMath::Ease(In, Quint, (float)breakUITimer / BREAK_UI_GENERATE_TIMER, 0, 1.0f);

		// 座標の変化量
		changePos = windowCenterPos - breakUIPosDef;

		// 座標とスケールと回転量を動かす。
		breakUIPos = changePos * easingAmount + breakUIPosDef;
		breakUIExtRate = EXT_RATE * easingAmount;
		breakUIRadian -= DEF_RADIAN / BREAK_UI_GENERATE_TIMER;

		++breakUITimer;

		// タイマーが既定値を超えていたら次の処理へ。
		if (BREAK_UI_GENERATE_TIMER < breakUITimer) {

			status = StunEffect::STATUS_STOP;
			breakUITimer = 0;

		}

		break;
	case StunEffect::STATUS_STOP:

		/*-- 画面中心にきてから一定時間停止する処理 --*/

		// タイマーが規定値の半分を超えていたら。
		if (BREAK_UI_STOP_TIMER / 2.0f <= breakUITimer) {

			easingAmount = KuroMath::Ease(Out, Sine, (breakUITimer - BREAK_UI_STOP_TIMER / 2.0f) / (BREAK_UI_STOP_TIMER / 2.0f), 0, 1.0f);

			// サイズにイージングをかけて大きくする。
			breakUIExtRate = ADD_EXT_RATE * easingAmount + EXT_RATE;

		}

		++breakUITimer;

		// タイマーが既定値を超えていたら次の処理へ。
		if (BREAK_UI_STOP_TIMER < breakUITimer) {

			status = StunEffect::STATUS_ICON;
			breakUITimer = 0;

		}

		break;
	case StunEffect::STATUS_ICON:

		/*-- 画面中央からアイコンに向かう処理 --*/

		// イージング量を求める。
		easingAmount = KuroMath::Ease(In, Cubic, (float)breakUITimer / BREAK_UI_ICON_TIMER, 0, 1.0f);

		// 座標の変化量
		changePos = iconPos - windowCenterPos;

		// 座標とスケールと回転量を動かす。
		breakUIPos = changePos * easingAmount + windowCenterPos;
		breakUIExtRate = -EXT_RATE - ADD_EXT_RATE + ((EXT_RATE + ADD_EXT_RATE) * easingAmount);
		breakUIRadian = 10.0f * easingAmount + 3.14f;

		++breakUITimer;

		// タイマーが既定値を超えていたら次の処理へ。
		if (BREAK_UI_ICON_TIMER < breakUITimer) {

			Camera::Instance()->Init();
			isActive = false;

			if(isBoss){
				FaceIcon::Instance()->Change(WHICH_FACE::RIGHT_FACE, FACE_STATUS::DAMAGE);
			}
			else{
				FaceIcon::Instance()->Change(WHICH_FACE::LEFT_FACE, FACE_STATUS::DAMAGE);
			}

		}

		break;
	default:
		break;
	}




	//// タイマーが0になったら。
	//if (focusTime <= 0) {

	//	// 有効化とフォーカスを切る。
	//	Camera::Instance()->Init();
	//	isActive = false;

	//}

}

void StunEffect::Draw()
{

	/*===== 描画処理 =====*/

	if (!isActive) return;

	DrawFunc::DrawRotaGraph2D(breakUIPos, Vec2<float>{breakUIExtRate, breakUIExtRate}, breakUIRadian, TexHandleMgr::GetTexBuffer(useHandle));

}
