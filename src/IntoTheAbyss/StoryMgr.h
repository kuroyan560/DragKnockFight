#pragma once
#include"PictureStory.h"
#include<array>
#include<vector>
#include<memory>

struct PictureStoryHandleData
{
	std::vector<int>pictureHandles;
	std::vector<int>stringHandles;

	PictureStoryHandleData(const std::vector<int> &PICTURE_HANDLE, const std::vector<int> &STRING_HANDLE) :pictureHandles(PICTURE_HANDLE), stringHandles(STRING_HANDLE)
	{
	}
};

class StoryMgr
{
public:
	StoryMgr();
	void Init(const std::vector<PictureStoryHandleData> &HANDLES);
	void InitScene();
	void Update();
	void Draw();

	bool goToTitleSceneFlag;
private:
	std::vector<std::shared_ptr<PictureStory>> story;
	int storyHandle;

};

