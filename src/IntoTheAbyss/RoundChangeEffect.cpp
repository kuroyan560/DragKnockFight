#include "RoundChangeEffect.h"
#include "AudioApp.h"
#include"../Common/KuroMath.h"
#include"../Engine/DrawFunc.h"
#include"../Engine/DrawFunc_FillTex.h"
#include"../Engine/DrawFunc_Mask.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../IntoTheAbyss/ScrollMgr.h"
#include"../IntoTheAbyss/DebugParameter.h"

RoundChangeEffect::RoundChangeEffect()
{
	roundData = std::make_unique<DrawData>();
	readyData = std::make_unique<DrawData>();
	fightData = std::make_unique<DrawData>();
	numberData = std::make_unique<DrawData>();
	fightData = std::make_unique<DrawData>();
	playerReticleData = std::make_unique<DrawData>();
	enemyReticleData = std::make_unique<DrawData>();
	nextNumberData = std::make_unique<DrawData>();

	playerReticle = std::make_unique<Sprite>();
	enemyReticle = std::make_unique<Sprite>();

	roundData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");
	readyData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/ready.png");
	fightData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/fight.png");
	playerReticleData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/reticle_player.png");
	enemyReticleData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/reticle_enemy.png");

	fightSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_fight.wav");
	readySE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_ready.wav");
	round1SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_round1.wav");
	round2SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_round2.wav");
	round3SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_round3.wav");
	round4SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_round4.wav");
	round5SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_round5.wav");

	AudioApp::Instance()->ChangeVolume(round1SE, 0.13f);
	AudioApp::Instance()->ChangeVolume(round2SE, 0.13f);
	AudioApp::Instance()->ChangeVolume(round3SE, 0.13f);
	AudioApp::Instance()->ChangeVolume(round4SE, 0.13f);
	AudioApp::Instance()->ChangeVolume(round5SE, 0.13f);
	AudioApp::Instance()->ChangeVolume(readySE, 0.13f);
	AudioApp::Instance()->ChangeVolume(fightSE, 0.13f);

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, numberHandle.data());


	numberData->handle = numberHandle[0];
}

void RoundChangeEffect::Init()
{
	startFlag = false;
}

void RoundChangeEffect::Start(const int& ROUND_NUMBER, const bool& LEFT_OR_RIGHT_FLAG)
{
	if (!startFlag)
	{
		roundData->Init();
		roundData->Init();
		readyData->Init();
		fightData->Init();
		numberData->Init();
		nextNumberData->Init();

		playerReticleData->Init();
		enemyReticleData->Init();

		nowRoundCount = ROUND_NUMBER;

		playerOrEnemySideFlag = LEFT_OR_RIGHT_FLAG;
		if (playerOrEnemySideFlag)
		{
			roundData->basePos = { -100.0f, 720.0f / 2.0f };
		}
		else
		{
			roundData->basePos = { 1280.0f + 100.0f, 720.0f / 2.0f };
		}
		roundData->pos.y = roundData->basePos.y;
		roundData->size = { 1.0f,1.0f };

		//数字文字系の初期化
		numberData->handle = numberHandle[ROUND_NUMBER - 1];
		numberData->baseSize = { 1.5f,1.6f };
		numberData->size = numberData->baseSize;

		nextNumberData->handle = numberHandle[ROUND_NUMBER];
		nextNumberData->baseSize = numberData->size;
		nextNumberData->size = numberData->size;


		//Readyの初期化
		readyData->basePos = { 1280.0f / 2.0f + 25.0f,720.0f / 2.0f - 100.0f };
		readyData->pos = { 1280.0f / 2.0f + 25.0f,720.0f / 2.0f - 100.0f };
		readyData->baseSize.y = 0.0f;
		readyData->size.y = 0.0f;

		//Fight文字の初期化
		drawFightFlag = false;
		fightData->basePos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->pos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->baseSize = { 5.0f,5.0f };
		fightData->size = { 5.0f,5.0f };


		//照準の初期化
		Vec2<float> restartPos = { 860.0f - 420.0f,-150.0f };
		Vec2<float> restartPos2 = { 860.0f - 120.0f,-150.0f };
		playerReticleData->basePos = restartPos;
		playerReticleData->pos = restartPos;
		playerReticleData->size = { 2.0f,2.0f };
		enemyReticleData->basePos = restartPos2;
		enemyReticleData->pos = restartPos2;
		enemyReticleData->size = { 2.0f,2.0f };


		initShakeFlag = false;
		readyToInitFlag = false;
		initGameFlag = false;
		maxShakeAmount = 25.0f;
		rate = 0.0f;
		oneFlameLateFlag = false;

		playerReticleAlpha = 255;
		enemyReticleAlpha = 255;

		initMaskFlag = false;

		firstRoundFlag = ROUND_NUMBER == 1;
		if (firstRoundFlag)
		{
			numberData->handle = numberHandle[1];
		}
	}
	startFlag = true;

}

void RoundChangeEffect::Update()
{
	if (startFlag)
	{
		roundData->maxTimer = DebugParameter::Instance()->roundData->roundData.maxTimer;
		roundData->sizeMaxTimer = DebugParameter::Instance()->roundData->roundData.sizeMaxTimer;

		readyData->maxTimer = DebugParameter::Instance()->roundData->readyData.maxTimer;
		readyData->sizeMaxTimer = DebugParameter::Instance()->roundData->readyData.sizeMaxTimer;

		fightData->maxTimer = DebugParameter::Instance()->roundData->fightData.maxTimer;
		fightData->sizeMaxTimer = DebugParameter::Instance()->roundData->fightData.sizeMaxTimer;

		numberData->maskMaxTimer = DebugParameter::Instance()->roundData->numberData.maskMaxTimer;
		numberData->sizeMaxTimer = DebugParameter::Instance()->roundData->roundData.sizeMaxTimer;

		nextNumberData->maskMaxTimer = DebugParameter::Instance()->roundData->nextNumberData.maskMaxTimer;
		nextNumberData->sizeMaxTimer = DebugParameter::Instance()->roundData->roundData.sizeMaxTimer;

		playerReticleData->maxTimer = DebugParameter::Instance()->roundData->readyData.sizeMaxTimer;
		enemyReticleData->maxTimer = DebugParameter::Instance()->roundData->readyData.sizeMaxTimer;

#pragma region 文字描画
		if (startFlag)
		{
			float code = 1.0f;
			if (!playerOrEnemySideFlag)
			{
				code *= -1;
			}

			// rateが0の時に"ラウンド◯"のSEを再生する。
			if (roundData->rate == 0) {

				switch (nowRoundCount)
				{
				case 1:
					AudioApp::Instance()->PlayWave(round1SE);
					break;

				case 2:
					AudioApp::Instance()->PlayWave(round2SE);
					break;

				case 3:
					AudioApp::Instance()->PlayWave(round3SE);
					break;

				case 4:
					AudioApp::Instance()->PlayWave(round4SE);
					break;

				case 5:
					AudioApp::Instance()->PlayWave(round5SE);
					break;

				default:
					break;
				}

			}

			//陣地に触れた場所の逆から中央に向かって移動する
			//数字とラウンドの移動
			Rate(&roundData->rate, roundData->maxTimer);
			roundData->pos.x = roundData->basePos.x + KuroMath::Ease(Out, Cubic, roundData->rate, 0.0f, 1.0f) * (1280.0f / 2.0f + 100) * code;
			numberData->pos = roundData->pos + Vec2<float>(280.0f, 0.0f);
			nextNumberData->pos = numberData->pos;

			//マスクの移動
			numberData->maskPos = numberData->pos + Vec2<float>(0.0f, 0.0f);
			nextNumberData->maskPos = numberData->pos + Vec2<float>(0.0f, -120.0f);
			numberData->basePos = numberData->pos;
			nextNumberData->basePos = numberData->pos;
		}


		//数字のマスク処理
		if (1.0f <= roundData->rate && !firstRoundFlag)
		{
			Rate(&numberData->maskRate, numberData->maskMaxTimer);
			Rate(&nextNumberData->maskRate, nextNumberData->maskMaxTimer);
			numberData->maskPos.y = numberData->basePos.y + KuroMath::Ease(Out, Cubic, numberData->maskRate, 0.0f, 1.0f) * 120.0f;
			nextNumberData->maskPos.y = nextNumberData->basePos.y + -120.0f + KuroMath::Ease(Out, Cubic, nextNumberData->maskRate, 0.0f, 1.0f) * 120.0f;
		}


		//中央に達したら文字のサイズを中央に消えるようにサイズを変える
		if (1.0f <= numberData->maskRate && !firstRoundFlag)
		{
			Rate(&roundData->sizeRate, roundData->sizeMaxTimer);
			Rate(&nextNumberData->sizeRate, nextNumberData->sizeMaxTimer);
			roundData->size.y = roundData->baseSize.y + -KuroMath::Ease(Out, Cubic, roundData->sizeRate, 0.0f, 1.0f) * roundData->baseSize.y;
			nextNumberData->size.y = nextNumberData->baseSize.y + -KuroMath::Ease(Out, Cubic, nextNumberData->sizeRate, 0.0f, 1.0f) * nextNumberData->baseSize.y;
		}
		else if (1.0f <= roundData->rate && firstRoundFlag)
		{

			Rate(&roundData->sizeRate, roundData->sizeMaxTimer);
			Rate(&numberData->sizeRate, numberData->sizeMaxTimer);
			roundData->size.y = roundData->baseSize.y + -KuroMath::Ease(Out, Cubic, numberData->sizeRate, 0.0f, 1.0f) * roundData->baseSize.y;
			numberData->size.y = numberData->baseSize.y + -KuroMath::Ease(Out, Cubic, numberData->sizeRate, 0.0f, 1.0f) * numberData->baseSize.y;
		}



		bool drawReadyFlag = false;
		//消えたらReadyを中央から上下に拡大する
		if (1.0f <= nextNumberData->sizeRate || 1.0f <= numberData->sizeRate)
		{
			// readyのSEを再生する。
			if (readyData->size.y == 0) {
				AudioApp::Instance()->PlayWave(readySE);
			}

			drawReadyFlag = true;
			Rate(&readyData->sizeRate, readyData->sizeMaxTimer);
			readyData->size.y = readyData->baseSize.y + KuroMath::Ease(Out, Cubic, readyData->sizeRate, 0.0f, 1.0f) * 1.0f;
		}


		//Fightを1フレ登場させて縮小をかける
		if (1.0f <= readyData->sizeRate)
		{
			// fightのSEを再生する。
			if (fightData->size.x == 5.0f) {
				AudioApp::Instance()->PlayWave(fightSE);
			}

			drawFightFlag = true;
			Rate(&fightData->sizeRate, fightData->sizeMaxTimer);
			fightData->size.x = fightData->baseSize.x + -KuroMath::Ease(Out, Cubic, fightData->sizeRate, 0.0f, 1.0f) * 4.0f;
			fightData->size.y = fightData->baseSize.y + -KuroMath::Ease(Out, Cubic, fightData->sizeRate, 0.0f, 1.0f) * 4.0f;
		}


		//縮小しきったら左右に画像を揺らしながら揺らす量を減らす
		if (1.0f <= fightData->sizeRate)
		{
			if (!initShakeFlag)
			{
				baseX = fightData->pos.x;
				initShakeFlag = true;
			}
			Shake();
			fightData->pos.x = baseX + shakeAmount.x;
		}

		//震えが無くなったら画像を下に落とす
		if (maxShakeAmount <= 0.0f)
		{
			Rate(&fightData->rate, fightData->maxTimer);
			fightData->pos.y = 720.0f / 2.0f + KuroMath::Ease(In, Cubic, fightData->rate, 0.0f, 1.0f) * 600.0f;
			if (1.0f <= fightData->rate)
			{
				//終了処理
				Init();
			}
		}

#pragma endregion


#pragma region 照準描画
		//照準を画面上からReadyの横に登場するように動かす
		if (drawReadyFlag)
		{
			Rate(&playerReticleData->rate, playerReticleData->maxTimer);
			Rate(&enemyReticleData->rate, enemyReticleData->maxTimer);
			playerReticleData->pos.y = playerReticleData->basePos.y + KuroMath::Ease(Out, Cubic, playerReticleData->rate, 0.0f, 1.0f) * (720.0f / 2.0f + 150.0f);
			enemyReticleData->pos.y = enemyReticleData->basePos.y + KuroMath::Ease(Out, Cubic, enemyReticleData->rate, 0.0f, 1.0f) * (720.0f / 2.0f + 150.0f);
		}
		//ファイトが出たらα値で消す
		if (drawFightFlag)
		{
			//5フレーム後に初期化準備
			if (playerReticleAlpha <= 200)
			{
				readyToInitFlag = true;
			}
			playerReticleAlpha -= 10;
			enemyReticleAlpha -= 10;

			if (playerReticleAlpha <= 0)
			{
				playerReticleAlpha = 0;
			}
			if (enemyReticleAlpha <= 0)
			{
				enemyReticleAlpha = 0;
			}
		}
		else
		{
			readyToInitFlag = false;
		}
#pragma endregion
	}
}

void RoundChangeEffect::Draw()
{
	if (startFlag)
	{
		//DrawFunc::DrawExtendGraph2D(roundData->GetLeftUpPos(), roundData->GetRightDownPos(), TexHandleMgr::GetTexBuffer(roundData->handle), AlphaBlendMode_None);
		DrawFunc::DrawRotaGraph2D(roundData->pos, roundData->size, 0.0f, TexHandleMgr::GetTexBuffer(roundData->handle));
		//DrawFunc::DrawRotaGraph2D(numberData->pos, numberData->size, 0.0f, TexHandleMgr::GetTexBuffer(numberData->handle));

		DrawFunc_Mask::DrawRotaGraph2D(numberData->maskPos, numberData->size, 0.0f, TexHandleMgr::GetTexBuffer(numberData->handle), numberData->pos, Vec2<float>(120.0f, 120.0f));
		if (!firstRoundFlag)
		{
			DrawFunc_Mask::DrawRotaGraph2D(nextNumberData->maskPos, nextNumberData->size, 0.0f, TexHandleMgr::GetTexBuffer(nextNumberData->handle), nextNumberData->pos, Vec2<float>(120.0f, 120.0f));
		}
		playerReticle->transform.SetPos(playerReticleData->pos);
		playerReticle->transform.SetScale(playerReticleData->size);
		playerReticle->SetTexture(TexHandleMgr::GetTexBuffer(playerReticleData->handle));
		playerReticle->SetColor(Color(255, 255, 255, playerReticleAlpha));
		playerReticle->Draw(AlphaBlendMode_Trans);

		enemyReticle->transform.SetPos(enemyReticleData->pos);
		enemyReticle->transform.SetScale(enemyReticleData->size);
		enemyReticle->SetTexture(TexHandleMgr::GetTexBuffer(enemyReticleData->handle));
		enemyReticle->SetColor(Color(255, 255, 255, enemyReticleAlpha));
		enemyReticle->Draw(AlphaBlendMode_Trans);

		if (drawFightFlag)
		{
			DrawFunc::DrawRotaGraph2D(fightData->pos, fightData->size, Angle::ConvertToRadian(-25.0f), TexHandleMgr::GetTexBuffer(fightData->handle));
		}
		else
		{
			DrawFunc::DrawRotaGraph2D(readyData->pos, readyData->size, 0.0f, TexHandleMgr::GetTexBuffer(readyData->handle));
		}
	}
}
