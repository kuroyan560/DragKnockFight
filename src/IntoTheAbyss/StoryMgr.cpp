#include "StoryMgr.h"
#include"../Engine/UsersInput.h"
#include"TexHandleMgr.h"
#include"WinApp.h"
#include"Tutorial.h"

StoryMgr::StoryMgr()
{
	aButtonHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/star.png");
	drawButtonFlag = true;
	flashTimer = 0;
}

void StoryMgr::Init(const std::vector<PictureStoryHandleData> &HANDLES)
{
	for (int i = 0; i <= HANDLES.size() - 1; ++i)
	{
		story.push_back(std::make_shared<PictureStory>());
		story[i]->Init(HANDLES[i].pictureHandles, HANDLES[i].stringHandles);
	}
}

void StoryMgr::InitScene()
{
	goToTitleSceneFlag = false;
	storyHandle = 0;

	for (int i = 0; i < story.size(); ++i)
	{
		story[i]->InitScene();
	}
	story[0]->Start();
	Tutorial::SetStaticActiveAll(false);
}

#include"AudioApp.h"
void StoryMgr::Update()
{
	if (story[storyHandle]->GoToNextScene())
	{
		++storyHandle;
		if (story.size() <= storyHandle)
		{
			--storyHandle;
			goToTitleSceneFlag = true;
		}
		else
		{
			story[storyHandle]->Start();
		}

		//チュートリアルアイコン表示
		{
			if (storyHandle == 1)
			{
				Tutorial::SetStaticActive(Tutorial::MOVE, true);
				Tutorial::SetStaticActive(Tutorial::DASH, true);
				Tutorial::SetStaticActive(Tutorial::SWING, true);
			}
			if (storyHandle == 3)
			{
				Tutorial::SetStaticActive(Tutorial::PILOT, true);
			}
		}

		if (storyHandle == story.size() - 1)
		{
			Tutorial::SetStaticActiveAll(true);
		}
	}

	const bool startButtonTrigger = UsersInput::Instance()->ControllerOnTrigger(0, START);
	//最後のストーリーじゃないとき
	if (storyHandle < story.size() - 1)
	{
		//説明スキップ
		if (startButtonTrigger)
		{
			storyHandle = story.size() - 1;
			story[storyHandle]->Start();
			Tutorial::SetStaticActiveAll(true);
		}
	}
	//Aボタンでもう一度見る
	else
	{
		if (UsersInput::Instance()->ControllerOnTrigger(0, A))
		{
			InitScene();
		}
	}

	bool finishButtonTrigger = storyHandle == story.size() - 1 ? startButtonTrigger : UsersInput::Instance()->ControllerOnTrigger(0, A);
	story[storyHandle]->Update(finishButtonTrigger);

	++flashTimer;
	if (30 <= flashTimer)
	{
		drawButtonFlag = !drawButtonFlag;
		flashTimer = 0;
	}
}

#include"DrawFunc.h"
void StoryMgr::Draw()
{
	story[storyHandle]->Draw();

	static const Vec2<float>A_BUTTON_POS =
	{
		static_cast<float>(WinApp::Instance()->GetWinSize().x - 70 - 15),
		static_cast<float>(WinApp::Instance()->GetWinSize().y - 30)
	};
	if (story[storyHandle]->OneLooped() && drawButtonFlag && storyHandle != story.size() - 1)
	{
		DrawFunc::DrawRotaGraph2D(A_BUTTON_POS, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(aButtonHandle));
	}
}
