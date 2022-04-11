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
	void Start(const bool &LEFT_OR_RIGHT_FLAG = false);
	void Update();
	void Draw();

private:
	struct DrawData
	{
		Vec2<float>honraiPos;
		Vec2<float>pos;
		Vec2<float>honraiSize;
		Vec2<float>size;
		int handle;

		DrawData()
		{
			pos = {0.0f,0.0f};
			size = { 1.0f,1.0f };
			handle = -1;
		}
	};

	std::unique_ptr<DrawData> roundData, readyData, fightData, numberData;
	float baseX;
	Vec2<float>shakeAmount;
	float maxShakeAmount;

	std::array<int, 10>numberHandle;
	bool startFlag;
	bool oneFlameLateFlag;
	bool initShakeFlag;
	float rate;

	float Lerp(float *HONRAI, float *BASE, const float &DIV)
	{
		float distance = *HONRAI - *BASE;
		*BASE += distance * DIV;

		return distance;
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
	bool initFlag;
	std::unique_ptr<DrawData> playerReticleData, enemyReticleData;
};

