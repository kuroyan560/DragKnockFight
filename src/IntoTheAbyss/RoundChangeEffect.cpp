#include "RoundChangeEffect.h"
#include"../Common/KuroMath.h"
#include"../Engine/DrawFunc.h"
#include"../Engine/DrawFunc_FillTex.h"
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

	playerReticle = std::make_unique<Sprite>();
	enemyReticle = std::make_unique<Sprite>();

	roundData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");
	readyData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/ready.png");
	fightData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/fight.png");
	playerReticleData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/reticle_player.png");
	enemyReticleData->handle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/reticle_enemy.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, numberHandle.data());

	readyData->honraiPos = { 1280.0f / 2.0f,720.0f / 2.0f };
	readyData->pos = { 1280.0f / 2.0f,720.0f / 2.0f };

	numberData->handle = numberHandle[0];
}

void RoundChangeEffect::Init()
{
	startFlag = false;
}

void RoundChangeEffect::Start(const bool &LEFT_OR_RIGHT_FLAG)
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
		numberData->handle = numberHandle[1];

		readyData->honraiSize.y = 0.0f;
		readyData->size.y = 0.0f;

		fightData->honraiPos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->pos = { 1280.0f / 2.0f,720.0f / 2.0f };
		fightData->honraiSize = { 2.0f,2.0f };
		fightData->size = { 5.0f,5.0f };


		Vec2<float> restartPos = { 860.0f-420.0f,-150.0f };
		Vec2<float> restartPos2 = { 860.0f-120.0f,-150.0f };

		playerReticleData->honraiPos = restartPos;
		playerReticleData->pos = restartPos;
		playerReticleData->size = { 2.0f,2.0f };
		enemyReticleData->honraiPos = restartPos2;
		enemyReticleData->pos = restartPos2;
		enemyReticleData->size = { 2.0f,2.0f };


		initShakeFlag = false;
		maxShakeAmount = 25.0f;
		rate = 0.0f;
		oneFlameLateFlag = false;

		playerReticleAlpha = 255;
		enemyReticleAlpha = 255;
	}
	startFlag = true;

}

void RoundChangeEffect::Update()
{
	if (startFlag)
	{

#pragma region •¶š•`‰æ
		float roundDistance = 1.0f;
		if (roundData->honraiPos.x == 1280.0f / 2.0f)
		{
			//w’n‚ÉG‚ê‚½êŠ‚Ì‹t‚©‚ç’†‰›‚ÉŒü‚©‚Á‚ÄˆÚ“®‚·‚é
			roundDistance = Lerp(&roundData->honraiPos.x, &roundData->pos.x, 0.1f);
			numberData->honraiPos = roundData->pos + Vec2<float>(130.0f, 0.0f);
		}

		float size = 1.0f;
		//’†‰›‚É’B‚µ‚½‚ç•¶š‚ÌƒTƒCƒY‚ğ’†‰›‚ÉÁ‚¦‚é‚æ‚¤‚ÉƒTƒCƒY‚ğ•Ï‚¦‚é
		if (fabs(roundDistance) <= 0.1f)
		{
			roundData->honraiSize.y = 0.0f;
			numberData->honraiSize.y = 0.0f;
			Lerp(&roundData->honraiSize.y, &roundData->size.y, 0.1f);
			size = Lerp(&numberData->honraiSize.y, &numberData->size.y, 0.1f);
		}

		bool drawReadyFlag = false;
		float readySize = 1.0f;
		//Á‚¦‚½‚çReady‚ğ’†‰›‚©‚çã‰º‚ÉŠg‘å‚·‚é
		if (fabs(size) <= 0.001f)
		{
			readyData->honraiSize.y = 1.0f;
			drawReadyFlag = true;
			readySize = Lerp(&readyData->honraiSize.y, &readyData->size.y, 0.1f);
		}

		float downSize = 1.0f;
		//Fight‚ğ1ƒtƒŒ“oê‚³‚¹‚Äk¬‚ğ‚©‚¯‚é
		if (fabs(readySize) <= 0.001f)
		{
			drawFightFlag = true;
			fightData->honraiSize = { 1.0f ,1.0f };
			downSize = Lerp(&fightData->honraiSize.x, &fightData->size.x, 0.2f);
			downSize = Lerp(&fightData->honraiSize.y, &fightData->size.y, 0.2f);
		}


		//k¬‚µ‚«‚Á‚½‚ç¶‰E‚É‰æ‘œ‚ğ—h‚ç‚µ‚È‚ª‚ç—h‚ç‚·—Ê‚ğŒ¸‚ç‚·
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

		//k‚¦‚ª–³‚­‚È‚Á‚½‚ç‰æ‘œ‚ğ‰º‚É—‚Æ‚·
		if (maxShakeAmount <= 0.0f)
		{
			fightData->honraiPos.y = 720.0f / 2.0f + KuroMath::Ease(In, Cubic, rate, 0.0f, 1.0f) * 600.0f;
			rate += 0.01f;
			if (1.0f <= rate)
			{
				rate = 1.0f;
			}
			fightData->pos.y = fightData->honraiPos.y;
		}
#pragma endregion


#pragma region Æ€•`‰æ
		//Æ€‚ğ‰æ–Êã‚©‚çReady‚Ì‰¡‚É“oê‚·‚é‚æ‚¤‚É“®‚©‚·
		if (drawReadyFlag)
		{
			playerReticleData->honraiPos.y = 720.0f / 2.0f;
			enemyReticleData->honraiPos.y = 720.0f / 2.0f;
			Lerp(&playerReticleData->honraiPos.y, &playerReticleData->pos.y, 0.2f);
			Lerp(&enemyReticleData->honraiPos.y, &enemyReticleData->pos.y, 0.2f);
		}
		//ƒtƒ@ƒCƒg‚ªo‚½‚çƒ¿’l‚ÅÁ‚·
		if (drawFightFlag)
		{
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
		DrawFunc::DrawRotaGraph2D(numberData->pos, numberData->size, 0.0f, TexHandleMgr::GetTexBuffer(numberData->handle));

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
