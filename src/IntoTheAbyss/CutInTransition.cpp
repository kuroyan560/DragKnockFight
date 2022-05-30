#include "CutInTransition.h"
#include "TexHandleMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"
#include "DrawFunc_Mask.h"
#include "WinApp.h"

CutInTransition::CutInTransition()
{

	/*===== コンストラクタ =====*/

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/luna.png", 3, Vec2<int>(3, 1), lunaHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/lacy.png", 3, Vec2<int>(3, 1), lacyHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask.png", 5, Vec2<int>(5, 1), maskHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask_frame.png", 5, Vec2<int>(5, 1), maskFrameHandle.data());
	maskAnimHandle = 0;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;

}

CutInTransition::~CutInTransition()
{
}

void CutInTransition::OnStart()
{

	/*===== 開始処理 =====*/

	backAlpha = 0;
	maskAnimHandle = 0;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isLeftChara = KuroFunc::GetRand(0, 1);
	maskPos = { 10000,10000 };
	charaPos = { 10000,10000 };

}

bool CutInTransition::OnUpdate()
{

	/*===== 更新処理 =====*/

	if (isEnd) return false;

	//ノックアウトの文字アニメーション
	const int START_PHASE_TIME = 60;			// 開始フェーズの時間
	const int STOP_PHASE_TIMER = 50;			// 一時停止フェーズの時間
	const int END_PHASE_TIME = 50;				// 終了フェーズの時間
	const int CHARA_ANIM_FRAME = 5;				// キャラの1アニメーションにかかる時間
	const Vec2<float> KNOCK_OUT_APPEAR_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y + 200.0f) };
	const Vec2<float> MASK_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y) };

	const Vec2<float> WindowSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y));
	const Vec2<float> WindowHalfSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

	// 勝者によってマスクやキャラアイコンの移動を変えるための変数。
	Vec2<float> maskStartPos = { -WindowSize.x,-WindowSize.y };
	Vec2<float> maskEndPos = { WindowSize.x + WindowHalfSize.x, WindowSize.y + WindowHalfSize.y };
	Vec2<float> charaStartPos = maskEndPos;
	Vec2<float> charaEndPos = maskStartPos;

	// 右のキャラだったら値を反転させる。
	if (!isLeftChara) {

		maskStartPos = { WindowSize.x + WindowSize.x, -WindowSize.y };
		maskEndPos = { -WindowHalfSize.x, WindowSize.y + WindowHalfSize.y };
		charaStartPos = maskEndPos;
		charaEndPos = maskStartPos;

	}

	// 現在のフェーズによってノックアウトの演出の更新処理を行う。
	switch (knockOutPhase)
	{
	case KNOCK_OUT_PHASE::START_PHASE:

		// 開始フェーズ

		// [KnockOut!!!!]にかけるマスクの座標を動かす。
		maskPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, maskStartPos, MASK_POS);

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, charaStartPos, MASK_POS);

		// タイマーが10の倍数になったらアニメーションハンドルを加算する。
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && maskAnimHandle < maskHandle.size() - 1)++maskAnimHandle;

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::STOP_PHASE;

		}

		// 後ろに描画する黒のアルファ値を255に近づける。
		backAlpha += (255 - backAlpha) / 10.0f;

		break;
	case KNOCK_OUT_PHASE::STOP_PHASE:

		// 一時停止フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (STOP_PHASE_TIMER <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::END_PHASE;

		}

		break;
	case KNOCK_OUT_PHASE::END_PHASE:

		// 最終フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが10の倍数になったらアニメーションハンドルを減算する。
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && 0 < maskAnimHandle)--maskAnimHandle;

		// [KnockOut!!!!]にかけるマスクの座標を動かす。
		maskPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, maskEndPos);

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, charaEndPos);


		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
			// 終わり
			isEnd = true;

		}


		break;
	default:
		break;
	}

	knockOutTimer += 2;

	return isEnd;
}

void CutInTransition::OnDraw()
{

	/*===== 描画処理 =====*/

	if (isEnd) return;

	static const float KNOCK_OUT_SCALE = 0.8f;

	static const int BLACK_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/black.png");

	// 背景を黒くする。
	DrawFunc::DrawExtendGraph2D(Vec2<float>(0, 0), WinApp::Instance()->GetExpandWinSize(), TexHandleMgr::GetTexBuffer(BLACK_HANDLE), Color(0, 0, 0, backAlpha));

	// マスクの内側のキャラクターを描画。
	if (isLeftChara) {
		// マスクの枠を描画。
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]));
		//DrawFunc_Mask::DrawGraphByMaskGraph(charaPos, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]), maskPos, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]), Vec2<float>(1.0f, 1.0f), Vec2<bool>(false, false));
	}
	else {
		// マスクの枠を描画。
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), Color(), Vec2<float>(0.5f, 0.5f), Vec2<bool>(true, false));
		//DrawFunc_Mask::DrawGraphByMaskGraph(charaPos, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), maskPos, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]), Vec2<float>(1.0f, 1.0f), Vec2<bool>(true, false));
	}


}
