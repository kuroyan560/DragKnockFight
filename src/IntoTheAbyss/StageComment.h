#pragma once
#include"../KuroEngine.h"

class StageComment
{
public:
	StageComment();

	void Init(int STAGE_NUM);
	void Update();
	void Draw();

private:
	std::shared_ptr<Sprite>commentSprite;

	int stageNum;
	float changeRate;

	int nonCommentHandle;
	std::vector<int>stageComment;
};

