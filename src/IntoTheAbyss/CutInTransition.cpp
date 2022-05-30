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
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isNowTransition = false;

}

CutInTransition::~CutInTransition()
{
}

void CutInTransition::OnStart()
{

	/*===== 開始処理 =====*/

	backAlpha = 0;
	isNowTransition = true;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isLeftChara = KuroFunc::GetRand(0, 1);
	charaPos = { 10000,10000 };
	titlePos = { 10000,10000 };

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
	const Vec2<float> MASK_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y) };

	const Vec2<float> WindowSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y));
	const Vec2<float> WindowHalfSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

	// タイトルロゴのオフセットポス
	Vec2<float> titleOffset = Vec2<float>(0, 170);

	// 勝者によってマスクやキャラアイコンの移動を変えるための変数。
	Vec2<float> charaStartPos = { WindowSize.x + WindowHalfSize.x, WindowHalfSize.y };
	Vec2<float> charaEndPos = { -WindowSize.x,WindowHalfSize.y };
	Vec2<float> titleStartPos = charaEndPos + titleOffset;
	Vec2<float> titleEndPos = charaStartPos + titleOffset;

	// 右のキャラだったら値を反転させる。
	if (!isLeftChara) {

		Vec2<float> CharaStartPosBuff = charaStartPos;
		charaStartPos = charaEndPos;
		charaEndPos = CharaStartPosBuff;
		titleStartPos = charaEndPos + titleOffset;
		titleEndPos = charaStartPos + titleOffset;

	}

	// 現在のフェーズによってノックアウトの演出の更新処理を行う。
	switch (knockOutPhase)
	{
	case KNOCK_OUT_PHASE::START_PHASE:

		// 開始フェーズ

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, charaStartPos, MASK_POS);
		titlePos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, titleStartPos, MASK_POS + titleOffset);

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

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, charaEndPos);
		titlePos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS + titleOffset, titleEndPos);


		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
			// 終わり
			isEnd = true;
			isNowTransition = false;

		}


		break;
	default:
		break;
	}

	// 後ろに描画する黒のアルファ値を255に近づける。
	backAlpha += (255 - backAlpha) / 10.0f;
	if (240 < backAlpha) {
		backAlpha = 255;
	}

	knockOutTimer += 2;

	return isEnd;
}

void CutInTransition::OnDraw()
{

	/*===== 描画処理 =====*/

	if (isEnd) return;

	static const float KNOCK_OUT_SCALE = 0.8f;

	static const int TITLE_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/title.png");

	// 背景を黒くする。
	DrawFunc::DrawExtendGraph2D(Vec2<float>(0, 0), WinApp::Instance()->GetExpandWinSize(), D3D12App::Instance()->GenerateTextureBuffer(Color(56, 22, 74, backAlpha)));

	// マスクの内側のキャラクターを描画。
	if (isLeftChara) {
		// マスクの枠を描画。
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]));
		DrawFunc::DrawRotaGraph2D(titlePos, Vec2<float>(0.8f, 0.8f), 0.0f, TexHandleMgr::GetTexBuffer(TITLE_GRAPH));
	}
	else {
		// マスクの枠を描画。
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), Color(), Vec2<float>(0.5f, 0.5f), Vec2<bool>(true, false));
		DrawFunc::DrawRotaGraph2D(titlePos, Vec2<float>(0.8f, 0.8f), 0.0f, TexHandleMgr::GetTexBuffer(TITLE_GRAPH));
	}


}
