#pragma once
#include"../Common/Singleton.h"

class EavaluationDataMgr:public Singleton<EavaluationDataMgr>
{
public:
	const float GOOD_RATE = 0.5f;
	const float GREAT_RATE = 0.8f;
	const float EXCELLENT_RATE = 0.0f;
	const float PERFECT_RATE = 0.0f;
	const int PERFECT_ANIMATION_INTERVAL = 3;
};

