#pragma once
#include"../KuroEngine.h"
#include"PlayerAnimation.h"

struct BehaviorGraphData
{
	Anim handle;
	bool cautionFlag;
};

class BehaviorPredection
{
public:
	BehaviorPredection(const std::vector<BehaviorGraphData> &DATA);

	void Init();
	void Update(const Vec2<float> &POS, int NOW_HANDLE);
	void Draw();

private:
	std::unique_ptr<PlayerAnimation> handleData;
	std::vector<bool> cautionFlags;
	int nowHandle;
	int defaultFlameHandle;
	int cautionFlameHandle;

	int gaugeHandle;
	int varHandle;


	Vec2<float>pos, size;
	Vec2<float>gaugePos, gaugeSize;
	Vec2<float>varPos, varSize;
};

