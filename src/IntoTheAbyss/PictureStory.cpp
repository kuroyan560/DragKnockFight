#include "PictureStory.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"
#include"../Engine/WinApp.h"

PictureStory::PictureStory()
{
}

void PictureStory::Init(const std::vector<int> &PICTURE_HANDLE, const std::vector<int> &STRING_HANDLE)
{
	pictureHandle = PICTURE_HANDLE;
	stringHandle = STRING_HANDLE;

	picturePos.resize(PICTURE_HANDLE.size());
	stringPos.resize(STRING_HANDLE.size());

	basePos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x / 2);
	basePos.y = -50.0f;
}

void PictureStory::InitScene()
{
	startFlag = false;
	goToNextSceneFlag = false;
	oneLoop = false;
}

#include"UsersInput.h"
void PictureStory::Update()
{
	if (startFlag)
	{
		if (!finishFlag)
		{
			if (0 < pictureArrayHandle && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_LEFT))
			{
				pictureArrayHandle--;
			}
			if (pictureArrayHandle < pictureHandle.size() - 1 && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_RIGHT))
			{
				pictureArrayHandle++;
			}
		}

		//ìríÜââèo
		if (nextFlag)
		{
			Rate(&stringNextRate, 5.0f);
			if (1.0f <= stringNextRate)
			{
				++stringArrayHandle;
				nextFlag = false;

				if (stringHandle.size() <= stringArrayHandle)
				{
					stringArrayHandle--;	//ç≈å„ÇÃâÊëúÇÃÇ‹Ç‹
					finishFlag = true;
				}
			}
		}
		else
		{
			MRate(&stringNextRate, 5.0f);
		}

		//ìoèÍââèo
		Rate(&stringAppearRate, 60.0f);
		stringPos[stringArrayHandle] = basePos;
		stringPos[stringArrayHandle].y += KuroMath::Ease(Out, Elastic, stringAppearRate, 0.0f, 1.0f) * 200.0f;

		//ìríÜââèoÇÃâ¡éZ
		stringPos[stringArrayHandle].y += -KuroMath::Ease(Out, Cubic, stringNextRate, 0.0f, 1.0f) * 10.0f;

		//èIóπèàóù
		if (finishFlag)
		{
			Rate(&stringFinishRate, 15.0f);
			if (1.0f <= stringFinishRate)
			{
				startCountDownTogoToNextSceneFlag = true;
			}
		}
		stringPos[stringArrayHandle].y += -KuroMath::Ease(In, Cubic, stringFinishRate, 0.0f, 1.0f) * 300.0f;




		//ç≈å„ÇÃâÊëúÇ‹Ç≈å©ÇΩ
		if (pictureHandle.size() - 1 <= pictureArrayHandle)
		{
			if (!oneLoop)oneLoop = true;
		}

		if (startCountDownTogoToNextSceneFlag)
		{
			++countDownTimer;
		}
		if (60 <= countDownTimer)
		{
			goToNextSceneFlag = true;
		}
	}
}

void PictureStory::Draw()
{
	static const float ARROR_X_OFFSET = 90;
	const auto winCenter = WinApp::Instance()->GetExpandWinCenter();
	static const int ARROW_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/arrow.png");
	static const float ARROW_FLOAT_Y = 10.0f;
	static float ARROW_FLOAT_RADIAN = 0.0f;

	ARROW_FLOAT_RADIAN += Angle::ConvertToRadian(3.0f);

	if (startFlag)
	{
		//adjPos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x / 2);
		//adjPos.y = static_cast<float>(WinApp::Instance()->GetWinSize().y / 2);
		DrawFunc::DrawRotaGraph2D(picturePos[pictureArrayHandle] + winCenter, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(pictureHandle[pictureArrayHandle]));
		DrawFunc::DrawRotaGraph2D(stringPos[stringArrayHandle], Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(stringHandle[stringArrayHandle]));

		//ñÓàÛÇÃï`âÊ
		const float offsetY = ARROW_FLOAT_Y * cos(ARROW_FLOAT_RADIAN);
		//ç∂ë§
		if (0 < pictureArrayHandle)
		{
			DrawFunc::DrawRotaGraph2D({ ARROR_X_OFFSET,winCenter.y + offsetY }, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH), { 0.5f,0.5f }, AlphaBlendMode_Trans, { true,false });
		}
		//âEë§
		if (pictureArrayHandle < pictureHandle.size() - 1)
		{
			DrawFunc::DrawRotaGraph2D({ winCenter.x * 2 - ARROR_X_OFFSET,winCenter.y + offsetY }, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH));
		}
	}
}

void PictureStory::Start()
{
	if (!startFlag)
	{
		pictureArrayHandle = 0;

		//ï∂éöï`âÊÇÃèâä˙âª
		stringArrayHandle = 0;
		stringAppearRate = 0.0f;
		stringNextRate = 0.0f;
		stringFinishRate = 0.0f;
		countDownTimer = 0;

		startCountDownTogoToNextSceneFlag = false;
		goToNextSceneFlag = false;
		nextFlag = false;
		finishFlag = false;
		startFlag = true;
	}
}

void PictureStory::GotoNextString()
{
	if (nextFlag)return;
	nextFlag = true;
}

bool PictureStory::GoToNextScene()
{
	return goToNextSceneFlag;
}
