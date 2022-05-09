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

	stringPos.resize(STRING_HANDLE.size());

	basePos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x / 2);
	basePos.y = 950;
}

void PictureStory::InitScene()
{
	startFlag = false;
	goToNextSceneFlag = false;
	oneLoop = false;
}

#include"UsersInput.h"
#include"AudioApp.h"
void PictureStory::Update(const bool& FinishButtonTrigger)
{
	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/select.wav");

	if (startFlag)
	{
		if (!finishFlag && !nextFlag)
		{
			////if (0 < pictureArrayHandle && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_LEFT))
			//if (0 < pictureArrayHandle && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::LT))
			//{
			//	pictureArrayHandle--;
			//	AudioApp::Instance()->PlayWave(SE);
			//}
			//if (pictureArrayHandle < pictureHandle.size() - 1 && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::RT))
			//{
			//	pictureArrayHandle++;
			//	AudioApp::Instance()->PlayWave(SE);
			//}
			
			//string
			if (stringArrayHandle < stringHandle.size() - 1 && UsersInput::Instance()->ControllerOnTrigger(0, RT))
			{
				stringArrayHandle++;
				AudioApp::Instance()->PlayWave(SE);
				nextFlag = true;
			}
			if (0 < stringArrayHandle && UsersInput::Instance()->ControllerOnTrigger(0, LT))
			{
				stringArrayHandle--;
				AudioApp::Instance()->PlayWave(SE);
				nextFlag = true;
			}
		}

		if (oneLoop && FinishButtonTrigger)
		{
			finishFlag = true;
		}

		//ìríÜââèo
		if (nextFlag)
		{
			Rate(&stringNextRate, 5.0f);
			if (1.0f <= stringNextRate)
			{
				//++stringArrayHandle;
				nextFlag = false;

				//if (stringHandle.size() <= stringArrayHandle)
				//{
				//	stringArrayHandle--;	//ç≈å„ÇÃâÊëúÇÃÇ‹Ç‹
				//	finishFlag = true;
				//}
			}
		}
		else
		{
			MRate(&stringNextRate, 5.0f);
		}

		//ìoèÍââèo
		Rate(&stringAppearRate, 60.0f);
		stringPos[stringArrayHandle] = basePos;
		stringPos[stringArrayHandle].y += KuroMath::Ease(Out, Elastic, stringAppearRate, 0.0f, 1.0f) * -330.0f;

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
		stringPos[stringArrayHandle].y += KuroMath::Ease(In, Cubic, stringFinishRate, 0.0f, 1.0f) * 300.0f;




		//ç≈å„ÇÃâÊëúÇ‹Ç≈å©ÇΩ
		//if (pictureHandle.size() - 1 <= pictureArrayHandle)
		//{
		//	if (!oneLoop)oneLoop = true;
		//}
		if (stringHandle.size() - 1 <= stringArrayHandle)
		{
			if (!oneLoop)oneLoop = true;
		}

		if (startCountDownTogoToNextSceneFlag)
		{
			++countDownTimer;
		}
		if (20 <= countDownTimer)
		{
			goToNextSceneFlag = true;
		}
	}
}

void PictureStory::Draw()
{
	static const float STR_EXT = 0.7f;
	const float ARROR_X_OFFSET = 130;
	const float ARROW_EXT = 0.8f;
	const auto winCenter = WinApp::Instance()->GetExpandWinCenter();
	static const int ARROW_GRAPH_LT = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/arrow_LT.png");
	static const int ARROW_GRAPH_RT = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/arrow_RT.png");
	static const float ARROW_FLOAT_Y = 10.0f;
	static float ARROW_FLOAT_RADIAN = 0.0f;

	ARROW_FLOAT_RADIAN += Angle::ConvertToRadian(3.0f);

	if (!pictureHandle.empty())
	{
		DrawFunc::DrawRotaGraph2D(WinApp::Instance()->GetExpandWinCenter(), { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(pictureHandle[0]));
	}

	if (startFlag)
	{
		DrawFunc::DrawRotaGraph2D(stringPos[stringArrayHandle], Vec2<float>(STR_EXT, STR_EXT), 0.0f, TexHandleMgr::GetTexBuffer(stringHandle[stringArrayHandle]));

		//ñÓàÛÇÃï`âÊ
		const float offsetY = ARROW_FLOAT_Y * cos(ARROW_FLOAT_RADIAN)/* + 250.0f*/;
		//ç∂ë§
		if (0 < stringArrayHandle)
		{
			//DrawFunc::DrawRotaGraph2D({ ARROR_X_OFFSET,winCenter.y + offsetY }, { ARROW_EXT,ARROW_EXT }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH_LT));
			DrawFunc::DrawRotaGraph2D({ ARROR_X_OFFSET,stringPos[stringArrayHandle].y + offsetY }, { ARROW_EXT,ARROW_EXT }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH_LT));
		}
		//âEë§
		if (stringArrayHandle < stringHandle.size() - 1)
		{
			//DrawFunc::DrawRotaGraph2D({ winCenter.x * 2 - ARROR_X_OFFSET,winCenter.y + offsetY }, { ARROW_EXT,ARROW_EXT }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH_RT));
			DrawFunc::DrawRotaGraph2D({ winCenter.x * 2 - ARROR_X_OFFSET,stringPos[stringArrayHandle].y + offsetY }, { ARROW_EXT,ARROW_EXT }, 0.0f, TexHandleMgr::GetTexBuffer(ARROW_GRAPH_RT));
		}
	}
}

void PictureStory::Start()
{
	if (!startFlag)
	{
		//pictureArrayHandle = 0;

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

bool PictureStory::GoToNextScene()
{
	return goToNextSceneFlag;
}
