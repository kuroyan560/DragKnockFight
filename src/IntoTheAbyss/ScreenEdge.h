#pragma once
#include"../Common/Vec.h"

class ScreenEdge
{
public:
	ScreenEdge();

	void Init(const float &DISTANCE, const Vec2<float> INIT_POS, const float ANGLE);
	void StartWinEffect();
	void StartFinishEffect();
	void CaluPos(const float &RATE);

	void Update();
	void Draw();

private:
	float maxDistance;
	Vec2<float>basePos;
	Vec2<float>pos;

	Vec2<float>shakeValue;
	float shakeRate;
	bool reversFlag;

	float angle;
	Vec2<float> distance;

	enum VEC_TYPE{ X_VEC, Y_VEC };
	VEC_TYPE vecType;

	float rate;

	//画像ハンドル-----------------------
	int playerChancehandle;
	int playerFlamehandle;

	int enemyChancehandle;
	int enemyFlamehandle;

	int nowFlameHandle;
	int nowChanceHandle;
	//画像ハンドル-----------------------


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

