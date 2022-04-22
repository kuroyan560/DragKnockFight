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

void PictureStory::Update()
{
	static const int CHANGE_PIC_TIME = 60;
	if (startFlag)
	{
		if (!finishFlag)
		{
			//�摜�̐؂�ւ�-----------------------
			++changePictureTimer;
			if (CHANGE_PIC_TIME <= changePictureTimer)
			{
				++pictureArrayHandle;
				changePictureTimer = 0;
			}

			//�摜�̐؂�ւ�-----------------------
		}



		//�r�����o
		if (nextFlag)
		{
			Rate(&stringNextRate, 5.0f);
			if (1.0f <= stringNextRate)
			{
				++stringArrayHandle;
				nextFlag = false;

				if (stringHandle.size() <= stringArrayHandle)
				{
					stringArrayHandle--;	//�Ō�̉摜�̂܂�
					finishFlag = true;
				}
			}
		}
		else
		{
			MRate(&stringNextRate, 5.0f);
		}

		//�o�ꉉ�o
		Rate(&stringAppearRate, 60.0f);
		stringPos[stringArrayHandle] = basePos;
		stringPos[stringArrayHandle].y += KuroMath::Ease(Out, Elastic, stringAppearRate, 0.0f, 1.0f) * 200.0f;

		//�r�����o�̉��Z
		stringPos[stringArrayHandle].y += -KuroMath::Ease(Out, Cubic, stringNextRate, 0.0f, 1.0f) * 10.0f;

		//�I������
		if (finishFlag)
		{
			Rate(&stringFinishRate, 15.0f);
			if (1.0f <= stringFinishRate)
			{
				startCountDownTogoToNextSceneFlag = true;
			}
		}
		stringPos[stringArrayHandle].y += -KuroMath::Ease(In, Cubic, stringFinishRate, 0.0f, 1.0f) * 300.0f;




		//�摜�̃��[�v-----------------------
		if (pictureHandle.size() <= pictureArrayHandle)
		{
			pictureArrayHandle = 0;
			if (!oneLoop)oneLoop = true;
		}
		//�摜�̃��[�v-----------------------


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
		//�w�i�摜�̏�����
		changePictureTimer = 0;
		pictureArrayHandle = 0;

		//�����`��̏�����
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
