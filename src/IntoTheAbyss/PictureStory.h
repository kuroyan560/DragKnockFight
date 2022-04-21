#pragma once
#include"Vec.h"
#include"KuroMath.h"
#include<vector>

class PictureStory
{
public:
	PictureStory();

	void Init(const std::vector<int> &PICTURE_HANDLE, const std::vector<int> &STRING_HANDLE);
	void InitScene();
	void Update();
	void Draw();

	void Start();
	void GotoNextString();

	bool GoToNextScene();
private:
	bool startFlag;

	std::vector<Vec2<float>>picturePos;
	std::vector<int>pictureHandle;
	int pictureArrayHandle;
	int changePictureTimer;
	

	Vec2<float>basePos;
	std::vector<Vec2<float>>stringPos;
	std::vector<int>stringHandle;
	int stringArrayHandle;
	float stringAppearRate;
	float stringNextRate;
	float stringFinishRate;
	bool nextFlag;
	bool finishFlag;
	bool goToNextSceneFlag;
	bool startCountDownTogoToNextSceneFlag;
	int countDownTimer;

	void Rate(float *RATE, const float &MAX_RATE)
	{
		*RATE += 1.0f / MAX_RATE;
		if (1.0f <= *RATE)
		{
			*RATE = 1.0f;
		}
	}
	void MRate(float *RATE, const float &MAX_RATE)
	{
		*RATE += -1.0f / MAX_RATE;
		if (*RATE <= 0.0f)
		{
			*RATE = 0.0f;
		}
	}
};

