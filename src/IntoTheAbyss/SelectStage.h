#pragma once
#include"../Common/Singleton.h"

class SelectStage:public Singleton<SelectStage>
{
public:
	SelectStage();

	void Select(int STAGE_NUM)
	{
		stageNum = STAGE_NUM;
	};
	const int &GetStageNum();

	bool resetStageFlag;
private:
	int stageNum;	//ステージ番号
};

