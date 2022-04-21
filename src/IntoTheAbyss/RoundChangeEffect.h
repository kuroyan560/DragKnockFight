#pragma once
#include"../Common/Vec.h"
#include<memory>
#include<array>
#include"../Common/KuroFunc.h"
#include"../Engine/Sprite.h"

class RoundChangeEffect
{
public:
	RoundChangeEffect();
	void Init();
	void Start(const int &ROUND_NUMBER, const bool &LEFT_OR_RIGHT_FLAG);
	void Update();
	void Draw();

private:
	struct DrawData
	{
		Vec2<float>basePos;
		Vec2<float>pos;

		Vec2<float>baseSize;
		Vec2<float>size;

		float rate;
		float maxTimer;

		float maskRate;
		float maskMaxTimer;

		float sizeRate;
		float sizeMaxTimer;

		Vec2<float>maskPos;
		int handle;

		DrawData()
		{
			pos = { 0.0f,0.0f };
			size = { 1.0f,1.0f };
			handle = -1;
		}

		void Init()
		{
			basePos = { 0.0f,0.0f };
			pos = { 0.0f,0.0f };
			baseSize = { 1.0f,1.0f };
			size = { 1.0f,1.0f };
			maskPos = { 0.0f,0.0f };

			rate = 0.0f;
			maxTimer = 120.0f;

			maskRate = 0.0f;
			maskMaxTimer = 120.0f;

			sizeRate = 0.0f;
			sizeMaxTimer = 120.0f;
		}
	};

	std::unique_ptr<DrawData> roundData, readyData, fightData, numberData, nextNumberData;
	float baseX;
	Vec2<float>shakeAmount;
	float maxShakeAmount;

	// サウンド
	int fightSE;
	int readySE;
	int round1SE;
	int round2SE;
	int round3SE;
	int round4SE;
	int round5SE;

	int nowRoundCount;

	std::array<int, 10>numberHandle;
	bool startFlag;
	bool oneFlameLateFlag;
	bool initShakeFlag;
	float rate;
	bool initMaskFlag;
	bool firstRoundFlag;
	bool playerOrEnemySideFlag;
	float Lerp(float *HONRAI, float *BASE, const float &DIV)
	{
		float distance = *HONRAI - *BASE;
		*BASE += distance * DIV;

		return distance;
	}

	void Rate(float *RATE, const float &MAX_RATE)
	{
		*RATE += 1.0f / MAX_RATE;
		if (1.0f <= *RATE)
		{
			*RATE = 1.0f;
		}
	}


	void Shake()
	{
		if (maxShakeAmount > 0) maxShakeAmount -= 0.5f;
		// シェイク量を更新する。
		//shakeAmount.x = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		//shakeAmount.y = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.x = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.y = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
	};

	std::unique_ptr<Sprite>playerReticle;
	std::unique_ptr<Sprite>enemyReticle;
	int playerReticleAlpha;
	int enemyReticleAlpha;

public:
	bool drawFightFlag;
	bool readyToInitFlag;
	bool initGameFlag;
	std::unique_ptr<DrawData> playerReticleData, enemyReticleData;
};

