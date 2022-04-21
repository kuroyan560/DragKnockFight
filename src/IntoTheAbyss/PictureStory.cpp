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
	stringPos.resize(PICTURE_HANDLE.size());

	basePos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x / 2);
	basePos.y = -50.0f;
}

void PictureStory::Update()
{
	if (startFlag)
	{
		//‰æ‘œ‚ÌØ‚è‘Ö‚¦-----------------------
		++changePictureTimer;
		if (120 <= changePictureTimer)
		{
			++pictureArrayHandle;
			changePictureTimer = 0;
		}

		++changeStringTimer;
		if (120 <= changeStringTimer && !nextFlag)
		{
			nextFlag = true;
			changeStringTimer = 0;
		}
		//‰æ‘œ‚ÌØ‚è‘Ö‚¦-----------------------




		//“r’†‰‰o
		if (nextFlag)
		{
			Rate(&stringNextRate, 30.0f);
			if (1.0f <= stringNextRate)
			{
				++stringArrayHandle;
				nextFlag = false;
			}
		}
		else
		{
			MRate(&stringNextRate, 30.0f);
		}

		//“oê‰‰o
		Rate(&stringAppearRate, 30.0f);
		stringPos[stringArrayHandle] = basePos;
		stringPos[stringArrayHandle].y += KuroMath::Ease(Out, Elastic, stringAppearRate, 0.0f, 1.0f) * 200.0f;

		//“r’†‰‰o‚Ì‰ÁZ
		stringPos[stringArrayHandle].y += -KuroMath::Ease(Out, Cubic, stringNextRate, 0.0f, 1.0f) * 10.0f;

		//I—¹ˆ—
		if (finishFlag)
		{
			Rate(&stringFinishRate, 30.0f);
			if (1.0f <= stringFinishRate)
			{
				goToNextSceneFlag = true;
			}
		}
		stringPos[stringArrayHandle].y += -KuroMath::Ease(In, Cubic, stringFinishRate, 0.0f, 1.0f) * 300.0f;




		//‰æ‘œ‚Ìƒ‹[ƒv-----------------------
		if (pictureHandle.size() <= pictureArrayHandle)
		{
			pictureArrayHandle = 0;
		}
		if (stringHandle.size() <= stringArrayHandle)
		{
			stringArrayHandle = 0;
		}
		//‰æ‘œ‚Ìƒ‹[ƒv-----------------------

	}
}

void PictureStory::Draw()
{
	if (startFlag)
	{
		Vec2<float>adjPos;
		adjPos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x / 2);
		adjPos.y = static_cast<float>(WinApp::Instance()->GetWinSize().y / 2);
		DrawFunc::DrawRotaGraph2D(picturePos[pictureArrayHandle] + adjPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(pictureHandle[pictureArrayHandle]));
		DrawFunc::DrawRotaGraph2D(stringPos[stringArrayHandle], Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(stringHandle[stringArrayHandle]));
	}
}

void PictureStory::Start()
{
	if (!startFlag)
	{
		//”wŒi‰æ‘œ‚Ì‰Šú‰»
		changePictureTimer = 0;
		pictureArrayHandle = 0;

		//•¶š•`‰æ‚Ì‰Šú‰»
		changeStringTimer = 0;
		stringArrayHandle = 0;
		stringAppearRate = 0.0f;
		stringNextRate = 0.0f;
		stringFinishRate = 0.0f;

		goToNextSceneFlag = false;
		nextFlag = false;
		finishFlag = false;
		startFlag = true;
	}
}

void PictureStory::Finish()
{
	finishFlag = true;
}
