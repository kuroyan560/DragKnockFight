#include "StoryMgr.h"
#include"../Engine/UsersInput.h"
#include"TexHandleMgr.h"

StoryMgr::StoryMgr()
{
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
	}

	if (UsersInput::Instance()->ControllerOnTrigger(0, A))
	{
		story[storyHandle]->GotoNextString();
	}

	story[storyHandle]->Update();
}

void StoryMgr::Draw()
{
	story[storyHandle]->Draw();
}
