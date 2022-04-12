#include "RoundChangeEffect.h"
#include"../Common/KuroMath.h"
#include"../Engine/DrawFunc.h"
#include"../Engine/DrawFunc_FillTex.h"
#include"../Engine/DrawFunc_Mask.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../IntoTheAbyss/ScrollMgr.h"

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
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, numberHandle.data());

	readyData->honraiPos = { 1280.0f / 2.0f + 25.0f,720.0f / 2.0f - 100.0f };
	readyData->pos = { 1280.0f / 2.0f + 25.0f,720.0f / 2.0f - 100.0f };

	numberData->handle = numberHandle[0];
	numberData->size = { 1.5f,1.5f };
}

void RoundChangeEffect::Init()
{
	startFlag = false;
}

void RoundChangeEffect::Start(const int &ROUND_NUMBER, const bool &LEFT_OR_RIGHT_FLAG)
{
	if (!startFlag)
	{
		if (LEFT_OR_RIGHT_FLAG)
		{
			roundData->honraiPos = { -100.0f, 720.0f / 2.0f };
			roundData->pos = { -100.0f, 720.0f / 2.0f };
		}
		else
		{
			roundData->honraiPos = { 1280.0f + 100.0f, 720.0f / 2.0f };
			roundData->pos = { 1280.0f + 100.0f, 720.0f / 2.0f };
		}
		roundData->honraiSize = { 1.0f,1.0f };
		roundData->size = { 1.0f,1.0f };
		numberData->handle = numberHandle[ROUND_NUMBER - 1];
		numberData->honraiSize = { 1.5f,1.6f };
		numberData->size = { 1.5f,1.6f };
		nextNumberData->handle = numberHandle[ROUND_NUMBER];
		nextNumberData->honraiSize = numberData->size;
		nextNumberData->size = numberData->size;

		readyData->honraiSize.y = 0.0f;
		readyData->size.y = 0.0f;

		drawFightFlag = false;
		fightData->honraiPos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->pos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->honraiSize = { 2.0f,2.0f };
		fightData->size = { 5.0f,5.0f };


		Vec2<float> restartPos = { 860.0f - 420.0f,-150.0f };
		Vec2<float> restartPos2 = { 860.0f - 120.0f,-150.0f };

		playerReticleData->honraiPos = restartPos;
		playerReticleData->pos = restartPos;
		playerReticleData->size = { 2.0f,2.0f };
		enemyReticleData->honraiPos = restartPos2;
		enemyReticleData->pos = restartPos2;
		enemyReticleData->size = { 2.0f,2.0f };


		initShakeFlag = false;
		readyToInitFlag = false;
		initFlag = false;
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

#pragma region 文字描画
		float roundDistance = 1.0f;
		if (roundData->honraiPos.x == 1280.0f / 2.0f && !initMaskFlag)
		{
			//陣地に触れた場所の逆から中央に向かって移動する
			roundDistance = Lerp(&roundData->honraiPos.x, &roundData->pos.x, 0.1f);
			numberData->pos = roundData->pos + Vec2<float>(280.0f, 0.0f);
			nextNumberData->pos = numberData->pos;

			numberData->honraiNumberMaskPos = numberData->pos;
			numberData->numberMaskPos = numberData->pos;
			nextNumberData->honraiNumberMaskPos = numberData->pos + Vec2<float>(0.0f, -120.0f);
			nextNumberData->numberMaskPos = numberData->pos + Vec2<float>(0.0f, -120.0f);
		}


		//数字のマスク処理
		float numberDistacnce = 1.0f;
		if (!firstRoundFlag)
		{
			if (fabs(roundDistance) <= 0.1f || initMaskFlag)
			{
				if (!initMaskFlag)
				{
					numberData->honraiNumberMaskPos.y += 120.0f;
					nextNumberData->honraiNumberMaskPos.y += 120.0f;
					initMaskFlag = true;
				}
				Lerp(&numberData->honraiNumberMaskPos.y, &numberData->numberMaskPos.y, 0.1f);
				numberDistacnce = Lerp(&nextNumberData->honraiNumberMaskPos.y, &nextNumberData->numberMaskPos.y, 0.1f);
			}
		}

		float size = 1.0f;
		//中央に達したら文字のサイズを中央に消えるようにサイズを変える
		if (fabs(numberDistacnce) <= 0.1f && !firstRoundFlag)
		{
			roundData->honraiSize.y = 0.0f;
			nextNumberData->honraiSize.y = 0.0f;
			Lerp(&roundData->honraiSize.y, &roundData->size.y, 0.1f);
			size = Lerp(&nextNumberData->honraiSize.y, &nextNumberData->size.y, 0.1f);
		}
		else if(firstRoundFlag && fabs(roundDistance) <= 0.1f)
		{
			roundData->honraiSize.y = 0.0f;
			numberData->honraiSize.y = 0.0f;
			Lerp(&roundData->honraiSize.y, &roundData->size.y, 0.1f);
			size = Lerp(&numberData->honraiSize.y, &numberData->size.y, 0.1f);
		}



		bool drawReadyFlag = false;
		float readySize = 1.0f;
		//消えたらReadyを中央から上下に拡大する
		if (fabs(size) <= 0.001f)
		{
			readyData->honraiSize.y = 1.0f;
			drawReadyFlag = true;
			readySize = Lerp(&readyData->honraiSize.y, &readyData->size.y, 0.1f);
		}

		float downSize = 1.0f;
		//Fightを1フレ登場させて縮小をかける
		if (fabs(readySize) <= 0.001f)
		{
			drawFightFlag = true;
			fightData->honraiSize = { 1.0f ,1.0f };
			downSize = Lerp(&fightData->honraiSize.x, &fightData->size.x, 0.2f);
			downSize = Lerp(&fightData->honraiSize.y, &fightData->size.y, 0.2f);
		}


		//縮小しきったら左右に画像を揺らしながら揺らす量を減らす
		if (fabs(downSize) <= 0.01f)
		{
			if (!initShakeFlag)
			{
				baseX = fightData->honraiPos.x;
				initShakeFlag = true;
			}

			Shake();
			fightData->honraiPos.x = baseX + shakeAmount.x;
			fightData->pos = fightData->honraiPos;
		}

		//震えが無くなったら画像を下に落とす
		if (maxShakeAmount <= 0.0f)
		{
			fightData->honraiPos.y = 720.0f / 2.0f + KuroMath::Ease(In, Cubic, rate, 0.0f, 1.0f) * 600.0f;
			rate += 0.01f;
			if (1.0f <= rate)
			{
				rate = 1.0f;
				//終了処理
				Init();
			}
			fightData->pos.y = fightData->honraiPos.y;
		}

#pragma endregion


#pragma region 照準描画
		//照準を画面上からReadyの横に登場するように動かす
		if (drawReadyFlag)
		{
			playerReticleData->honraiPos.y = 720.0f / 2.0f;
			enemyReticleData->honraiPos.y = 720.0f / 2.0f;
			Lerp(&playerReticleData->honraiPos.y, &playerReticleData->pos.y, 0.2f);
			Lerp(&enemyReticleData->honraiPos.y, &enemyReticleData->pos.y, 0.2f);
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


	if (oneFlameLateFlag)
	{
		roundData->honraiPos.x = 1280.0f / 2.0f;
	}
	oneFlameLateFlag = true;

}

void RoundChangeEffect::Draw()
{
	if (startFlag)
	{
		//DrawFunc::DrawExtendGraph2D(roundData->GetLeftUpPos(), roundData->GetRightDownPos(), TexHandleMgr::GetTexBuffer(roundData->handle), AlphaBlendMode_None);
		DrawFunc::DrawRotaGraph2D(roundData->pos, roundData->size, 0.0f, TexHandleMgr::GetTexBuffer(roundData->handle));
		//DrawFunc::DrawRotaGraph2D(numberData->pos, numberData->size, 0.0f, TexHandleMgr::GetTexBuffer(numberData->handle));

		DrawFunc_Mask::DrawRotaGraph2D(numberData->pos, numberData->size, 0.0f, TexHandleMgr::GetTexBuffer(numberData->handle), numberData->numberMaskPos, Vec2<float>(120.0f, 120.0f));
		if (!firstRoundFlag)
		{
			DrawFunc_Mask::DrawRotaGraph2D(nextNumberData->pos, nextNumberData->size, 0.0f, TexHandleMgr::GetTexBuffer(nextNumberData->handle), nextNumberData->numberMaskPos, Vec2<float>(120.0f, 120.0f));
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
