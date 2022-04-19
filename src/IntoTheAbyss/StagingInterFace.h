#pragma once
#include"Vec.h"
//ââèoÇ…ä÷Ç∑ÇÈã@î\
class StagingInterFace
{
	int shakeTimer;
	int shakeTotalTime;
	Vec2<float>shake;
	Vec2<int>sign;
	int shakeSpan;
	float shakePower;

	int extTimer;
	int extTotalTime;
	Vec2<float>extRate;
	Vec2<float>addExtRate;

	int flashTimer;
	int flashTotalTime;
	float flashAlpha;
	float flashRate;

	bool spin;
	int spinRadianSign;
	float spinRadian;
	float addSpinRadian;
	
public:
	void Init();
	void Update();

	const Vec2<float>GetShake() { return shake; }
	const float& GetFlashAlpha() { return flashAlpha; }
	const Vec2<float>& GetExtRate() { return extRate; }
	const float& GetSpinRadian() { return spinRadian; }

	void Shake(const int& Time, const int& Span, const int& Power);
	void Extend(const int& Time, const Vec2<bool>& Ext, const float& ExtRateAmount);
	void Flash(const int& Time,const float& FlashRate);
	void StartSpin(const bool& Add) { 
		spin = true; 
		if (Add)spinRadianSign = 1;
		else spinRadianSign = -1;
		addSpinRadian = 0.0f;
	}
	void StopSpin() { spin = false; spinRadian = 0.0f; }
};

