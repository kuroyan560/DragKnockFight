#include "RunOutOfStaminaEffect.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/Angle.h"
#include"../Common/KuroFunc.h"
#include"ScrollMgr.h"

RunOutOfStaminaEffect::RunOutOfStaminaEffect()
{
	emptyHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
	maxHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
	lerpPlayerSize = { 1.0f,1.0f };
	playerSize = { 1.0f,1.0f };
	startFlag = false;
	drawFlag = false;
}

void RunOutOfStaminaEffect::Init()
{
	startFlag = false;
}

void RunOutOfStaminaEffect::Update()
{
	if (startFlag)
	{
		++timer;

		if(drawFlag)
		{
			//����ۂ̃v���C���[���o-----------------------
			//��ɓo�ꂳ����
			lerpEmptyStringPos =
			{
				baseEmptyStringPos.x,
				baseEmptyStringPos.y + -50.0f
			};
			emptyStringPos.x = baseEmptyStringPos.x;
			Vec2<float> pos = lerpEmptyStringPos - emptyStringPos;
			emptyStringPos.y += pos.y * 0.3f;

			if (0.5f < playerSize.x && !shrinkFlag)
			{
				lerpPlayerSize.x = 0.5f;
				shrinkFlag = true;
			}
			else if (playerSize.x <= 0.5f && !extendFlag)
			{
				extendFlag = true;
			}
			else if (extendFlag)
			{
				lerpPlayerSize.x = 1.0f;
			}
			//����ۂ̃v���C���[���o-----------------------
		}


		if(drawMaxFlag)
		{
			//���^���̃v���C���[���o-----------------------
			lerpMaxStringPos =
			{
				baseMaxStringPos.x,
				baseMaxStringPos.y + -50.0f
			};
			maxStringPos.x = baseMaxStringPos.x;
			Vec2<float> pos = lerpMaxStringPos - maxStringPos;
			maxStringPos.y += pos.y * 0.3f;


			if (0.5f < playerSize.x && !extendMaxFlag)
			{
				lerpPlayerSize.x = 1.5f;
				extendMaxFlag = true;
			}
			else if (1.5f <= playerSize.x && extendMaxFlag && !shrinkMaxFlag)
			{
				shrinkMaxFlag = true;
			}
			else if (shrinkMaxFlag && 1.0f < lerpPlayerSize.x)
			{
				lerpPlayerSize.x = 1.0f;
			}
			else if (playerSize.x <= 1.0f)
			{
				startFlag = false;
				playerSize = { 1.0f,1.0f };
				timer = 0;
			}
			//���^���̃v���C���[���o-----------------------
		}


		Vec2<float> size = lerpPlayerSize - playerSize;
		float rate = 0.5f;
		playerSize += size * rate;

		//���ʎ��Ԃ�菭���O�ɓ_�ł��J�n����
		if (finishTimer - 40 <= timer)
		{
			//�_�ŕ\��
			if (timer % 2 == 0)
			{
				drawFlag = !drawFlag;
			}
		}
		//���^�����o
		if (finishTimer < timer)
		{
			drawFlag = false;
			drawMaxFlag = true;
		}


	}
}

void RunOutOfStaminaEffect::Draw()
{
	//����ە\��
	if (startFlag && drawFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(emptyStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(emptyHande));
	}
	//���^���\��
	else if (startFlag && drawMaxFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(maxStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maxHande));
	}
}

void RunOutOfStaminaEffect::Start(const Vec2<float> &POS, int MAX_TIMER)
{
	if (!startFlag)
	{
		//��̕����ɔ�΂�
		float rad = Angle::ConvertToRadian(90);
		ParticleMgr::Instance()->Generate(POS, Vec2<float>(cosf(rad), sinf(rad)), BULLET);
		lerpPlayerSize = { 1.0f,1.0f };
		playerSize = { 1.0f,1.0f };

		lerpEmptyStringPos = POS;
		emptyStringPos = POS;

		lerpMaxStringPos = POS;
		maxStringPos = POS;


		timer = 0;
		finishTimer = MAX_TIMER;
		drawFlag = true;
		startFlag = true;
		shrinkFlag = false;
		extendFlag = false;

		drawMaxFlag = false;
		extendMaxFlag = false;
		shrinkMaxFlag = false;
	}

	baseEmptyStringPos = POS;
	baseMaxStringPos = POS;
}

bool RunOutOfStaminaEffect::isDraw()
{
	return startFlag;
}
