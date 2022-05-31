#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"
#include "StageMgr.h"
#include "ScoreManager.h"
#include "WinCounter.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "WinApp.h"
#include"SelectStage.h"
#include <IntoTheAbyss/Boss.h>
#include "RoundCountMgr.h"

RoundFinishEffect::RoundFinishEffect()
{

	/*===== コンストラクタ =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/perfect.png", 3, Vec2<int>(3, 1), perfectGraph.data());
	goodGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/good.png");
	greatGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/great.png");
	excellentGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/excellent.png");


	evaluationSoundHandle = EvaluationMgr::Instance()->soundData;
}

void RoundFinishEffect::Init()
{

	/*===== 初期化処理 =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();
	changeMap = false;
	perfectExp = {};
}

void RoundFinishEffect::Start(const bool& IsPerfect, const float& Rate, const float& CameraZoom)
{

	/*===== 初期化処理 =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = false;
	shakeAmount = Vec2<float>();
	isPerfect = IsPerfect;
	perfectAnimTimer = 0;
	perfectAnimIndex = 0;
	finishLap = false;
	cameraZoom = CameraZoom;
	isEndResultScene = false;

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();
	static const float FAIL_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).failRate;
	static const float GOOD_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).goodRate;
	static const float GREAT_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).greatRate;

	// 引数の割合からどの画像を使用するかをチェックする。
	if (Rate <= FAIL_PER)
	{

		soundType = SOUND_FAIL;
		useGraph = goodGraph;

	}
	else if (Rate <= GOOD_PER) {

		soundType = SOUND_GOOD;
		useGraph = goodGraph;

	}
	else if (Rate <= GREAT_PER) {

		soundType = SOUND_GREAT;
		useGraph = greatGraph;

	}
	else {

		soundType = SOUND_EXCELLENT;
		useGraph = excellentGraph;

	}
	addScoreFlag = false;
}

void RoundFinishEffect::Update(const Vec2<float>& LineCenterPos)
{

	static const int EXPLOSION_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/break.wav");
	static const int READY_EXPLOSION_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/readyExplosion.wav", 0.5f);

	/*===== 更新処理 =====*/

	if (isEnd) return;

	// パーフェクトの画像の動く量。
	const float PERFECT_MOVE_POS_Y = -15.0f;

	const float SHAKE_AMOUNT = 10.0f;
	float shakeRate = 0;
	float nowShakeAmount = 0;
	Vec2<float> playerDefPos = {};
	Vec2<float> enemyDefPos = {};
	float lerpRate = 0;
	float lerpRateBuff = 0;

	float mul = 0.0001f;

	switch (status)
	{
	case RoundFinishEffect::EFFECT_STATUS::NUM1_ZOOMIN:

		/*-- 第一段階 --*/

		// カメラを二人の真ん中にフォーカスさせる。
		Camera::Instance()->Focus(LineCenterPos, cameraZoom + 0.02f, 0.1f);

		// タイマーを更新して次へ。
		++timer;
		if (NUM1_ZOOMIN_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM2_ENEMY_SHAKE;
			AudioApp::Instance()->PlayWave(READY_EXPLOSION_SE);
		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM2_ENEMY_SHAKE:

		/*-- 第二段階 --*/

		// 座標からシェイク量を引く。
		CharacterManager::Instance()->Right()->pos -= shakeAmount;

		// 敵にフォーカスを合わせる。
		Camera::Instance()->Focus(CharacterManager::Instance()->Right()->pos, 2.0f, 0.1f);

		// 敵をシェイクさせる。
		shakeRate = static_cast<float>(timer) / static_cast<float>(NUM2_ENEMY_SHAKE_TIMER);
		nowShakeAmount = shakeRate * SHAKE_AMOUNT;
		shakeAmount = Vec2<float>(KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount, KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount);
		CharacterManager::Instance()->Right()->pos += shakeAmount;

		UsersInput::Instance()->ShakeController(0, 0.1f, 6);

		// タイマーを更新して次へ。
		++timer;
		if (NUM2_ENEMY_SHAKE_TIMER <= timer) {

			// 座標からシェイク量を引く。
			CharacterManager::Instance()->Right()->pos -= shakeAmount;
			std::dynamic_pointer_cast<Boss>(CharacterManager::Instance()->Right())->anim->ChangeAnim(Boss::EXPLOSION_OPEN);

			timer = 0;
			status = EFFECT_STATUS::NUM3_ENEMY_EXP;
			RoundFinishParticleMgr::Instance()->Init();
			RoundFinishParticleMgr::Instance()->Generate(CharacterManager::Instance()->Right()->pos);

			// パーフェクトの画像を動かす。
			perfectPos = WinApp::Instance()->GetWinDifferRate() * Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));
			perfectExp = { 0.0f,0.0f };
			perfectMoveAmount = PERFECT_MOVE_POS_Y;
			
			AudioApp::Instance()->StopWave(READY_EXPLOSION_SE);
			AudioApp::Instance()->PlayWave(EXPLOSION_SE);
			AudioApp::Instance()->PlayWave(evaluationSoundHandle[soundType]);
		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- 第三段階 --*/

		// カメラを一気に引く。
		Camera::Instance()->Focus(LineCenterPos, cameraZoom, 0.3f);

		// タイマーを更新して次へ。
		++timer;
		if (NUM3_ENEMY_EXP_TIMER / 2.0f == timer) {

			RoundFinishParticleMgr::Instance()->SetReturn();

			// カメラをプレイヤーに近づける。
			//Camera::Instance()->Focus(CharacterManager::Instance()->Left()->pos, 1.0f, 0.1f);

		}
		else if (timer == NUM3_ENEMY_EXP_TIMER * 0.75) {

			//ScoreManager::Instance()->AddDestroyPoint();
			addScoreFlag = true;

			UsersInput::Instance()->ShakeController(0, 0.5f, 12);

			// ラウンド数のUIを更新。
			RoundCountMgr::Instance()->RoundIncrement();

		}
		else if (timer < NUM3_ENEMY_EXP_TIMER / 2.0f) {

			if (SelectStage::Instance()->HaveNextLap()) {

				// カメラを一気に引く。
				Camera::Instance()->Focus(LineCenterPos, cameraZoom, 0.3f);
				UsersInput::Instance()->ShakeController(0, 1.0f, 5);

			}

		}

		if (NUM3_ENEMY_EXP_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM4_RETURN_DEFPOS;
			changeMap = true;
			std::dynamic_pointer_cast<Boss>(CharacterManager::Instance()->Right())->anim->ChangeAnim(Boss::EXPLOSION_CLOSE);
		}

		// パーフェクトの画像を動かす。
		perfectMoveAmount -= perfectMoveAmount / 10.0f;
		perfectPos.y += perfectMoveAmount;
		perfectExp.x += (1.0f - perfectExp.x) / 10.0f;
		perfectExp.y += (1.0f - perfectExp.y) / 10.0f;


		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM4_RETURN_DEFPOS:

		/*-- 第四段階 --*/

		// パーフェクトの画像を小さくする。
		perfectExp.x -= perfectExp.x / 5.0f;
		perfectExp.y -= perfectExp.y / 5.0f;
		if (perfectExp.x <= 0.05f) {

			perfectExp = { 0.0f,0.0f };

		}

		// 座標を規定値に戻す。
		if (SelectStage::Instance()->HaveNextLap())
		{
			playerDefPos = StageMgr::Instance()->GetPlayerResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
			enemyDefPos = StageMgr::Instance()->GetBossResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);


			Camera::Instance()->Init();

			CharacterManager::Instance()->Left()->pos += (playerDefPos - CharacterManager::Instance()->Left()->pos) / 30.0f;
			CharacterManager::Instance()->Right()->pos += (enemyDefPos - CharacterManager::Instance()->Right()->pos) / 30.0f;
		}
		//playerDefPos = Vec2<float>(100, 700);
		//enemyDefPos = Vec2<float>(5000, 700);

		++timer;
		if (NUM4_RETURN_DEFPOS_TIMER <= timer) {

			isEnd = true;
			//timer = 0;
			status = EFFECT_STATUS::NUM1_ZOOMIN;

			++WinCounter::Instance()->countRound;

			perfectExp = {};

			ScoreManager::Instance()->destroyPoint = 0;

			isEndResultScene = true;

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM5_RETURN_PLAYER_DEFPOS:

		/*-- 第五段階 --*/

		// プレイヤーの座標を規定値に戻す。

		++timer;

		// 座標を規定値に戻す。
		if (SelectStage::Instance()->HaveNextLap())
		{
			playerDefPos = StageMgr::Instance()->GetPlayerResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
			enemyDefPos = StageMgr::Instance()->GetBossResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
		}

		CharacterManager::Instance()->Left()->pos += (playerDefPos - CharacterManager::Instance()->Left()->pos) / 30.0f;


		if (NUM5_RETURN_PLAYER_DEF_POS <= timer) {

			isEnd = true;

			timer = 0;
			status = EFFECT_STATUS::NUM1_ZOOMIN;

			++WinCounter::Instance()->countRound;

		}


		break;

	default:

		break;

	}

	// ラウンド終了時のパーティクルを更新。
	RoundFinishParticleMgr::Instance()->Update(CharacterManager::Instance()->Left()->pos);

	// パーフェクト画像をアニメーションさせる。
	++perfectAnimTimer;
	if (PERFECT_ANIM_TIMER <= perfectAnimTimer) {

		perfectAnimTimer = 0;

		++perfectAnimIndex;

		if (EvaluationMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectAnimIndex) {

			perfectAnimIndex = 0;

		}

	}

}

#include <memory>
#include "GameSceneCamerMove.h"

void RoundFinishEffect::Draw()
{
	if (isEnd)return;
	/*===== 描画処理 =====*/

	std::shared_ptr<TextureBuffer> drawGraph = 0;

	if (isPerfect) {

		drawGraph = TexHandleMgr::GetTexBuffer(perfectGraph[perfectAnimIndex]);

	}
	else {

		drawGraph = TexHandleMgr::GetTexBuffer(useGraph);

	}

	DrawFunc::DrawRotaGraph2D(perfectPos + GameSceneCameraMove::Instance()->move, perfectExp, 0, drawGraph);

}
