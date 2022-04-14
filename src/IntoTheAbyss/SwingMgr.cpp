#include "SwingMgr.h"
#include "KuroMath.h"

SwingMgr::SwingMgr()
{

	/*===== コンストラクタ =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};
	addEasingTimer = 0;

}

void SwingMgr::Init()
{

	/*===== 初期化処理 =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};
	addEasingTimer = 0;

}

void SwingMgr::Update(const Vec2<float>& playerPos, const Vec2<float>& bossPos, const float& lineLength)
{

	/*===== 更新処理 =====*/

	// イージング量を求める。
	float easingChange = ADD_EASINGTIMER_MAX - ADD_EASINGTIMER_MINIMUM;
	addEasingTimer = ADD_EASINGTIMER_MINIMUM + (fabs(fabs(easingEndVec.y) - fabs(easingStartVec.y)) * easingChange);

	// [補正]X軸のベクトルが0だったら強制的に0.01fにする。
	if (fabs(easingStartVec.x) <= 0.001f) easingStartVec.x = 0.001f;

	// 座標を保存。
	this->playerPos = playerPos;
	this->bossPos = bossPos;
	// 紐の長さを保存。
	this->lineLength = lineLength;

	// 振り回し中じゃなかったら処理を飛ばす。
	if (!(isSwingPlayer || isSwingBoss)) return;

	// イージングタイマーが限界を超えたらフラグを折る。
	if (1.0f <= easingTimer) {

		isSwingPlayer = false;
		isSwingBoss = false;

	}

	// イージングタイマーを更新する。
	if (easingTimer < 1.0f) {

		// イージングタイマーを更新する。
		easingTimer += addEasingTimer;

		// 限界値を超えないようにする。
		if (1.0f <= easingTimer) easingTimer = 1.0f;

		// イージングタイマーから求められるイージング量を求める。
		easeAmount = KuroMath::Ease(Out, Cubic, easingTimer, 0.0f, 1.0f);

		// 現在のベクトルを求める。
		easingNowVec.x = easingStartVec.x;
		easingNowVec.y = easingStartVec.y + (easeAmount * easeChangeAmountY);
		easingNowVec.Normalize();

	}

}

#include"AudioApp.h"
void SwingMgr::PlaySE()
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.55f);
	}
	AudioApp::Instance()->PlayWave(SE);
}
