#pragma once
#include"../Common/Singleton.h"
#include<vector>

struct StageEvaluationData
{
	float goodRate;
	float greatRate;
	float excellentRate;
	float perfectRate;

	StageEvaluationData() :goodRate(0.3f), greatRate(0.5f), excellentRate(0.8f), perfectRate(1.0f)
	{
	}
};

class EvaluationMgr:public Singleton<EvaluationMgr>
{
	std::vector<std::vector<StageEvaluationData>> data;
	std::vector<StageEvaluationData> resultData;

public:
	void Init(std::vector<std::vector<StageEvaluationData>> DATA, std::vector<StageEvaluationData> R_DATA)
	{
		data = DATA;
		resultData = R_DATA;
	};

	StageEvaluationData GetData(int STAGE_NUM, int ROOM_NUM)
	{
		return data[STAGE_NUM][ROOM_NUM];
	};

	const int PERFECT_ANIMATION_INTERVAL = 3;
};