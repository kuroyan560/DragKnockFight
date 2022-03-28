#pragma once
#include"Singleton.h"
class GUI : public Singleton<GUI>
{
	friend class Singleton<GUI>;
	GUI();

	int spearTeleGraph;
	int spearTimeGraph;
	float spearTeleRate;
	float spearTimeRate;

public:
	void SetSpearTeleRate(const float& Rate) { spearTeleRate = Rate; }
	void SetSpearTimeRate(const float& Rate) { spearTimeRate = Rate; }
	void Draw();
};

