#include "StoryMgr.h"
#include"../Engine/UsersInput.h"
#include"TexHandleMgr.h"
#include"WinApp.h"

StoryMgr::StoryMgr()
{
	aButtonHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/star.png");
	pos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x - 70 - 15);
	pos.y = static_cast<float>(WinApp::Instance()->GetWinSize().y - 70);
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
}

#include"AudioApp.h"
void StoryMgr::Update()
{
	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/select.wav");

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
	}

	if (story[storyHandle]->OneLooped() && UsersInput::Instance()->ControllerOnTrigger(0, A))
	{
		AudioApp::Instance()->PlayWave(SE);
		story[storyHandle]->GotoNextString();
	}

	story[storyHandle]->Update();

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

	if (story[storyHandle]->OneLooped() && drawButtonFlag)
	{
		DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(aButtonHandle));
	}
}
