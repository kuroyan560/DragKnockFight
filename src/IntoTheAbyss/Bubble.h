#pragma once
#include "Vec.h"

// BubbleClass
class Bubble {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;		// position
	Vec2<float> scale;		// Draw Scale Hikinobashi
	Vec2<float> easingScale;
	float radius;			// Draw Radius
	int breakCoolTime;		// Resporn cool time
	bool isBreak;			// states ga break ka
	int graphHandle;
	float addEasingTimer;	// イジングタイマーに加算する量。
	float easingTimer;		// easing timer
	bool easingFlag;		// 拡縮はXY軸で行うため、それぞれの軸で拡張と収縮のどちらを行うかを判断するためのフラグ
	bool isHitBullet;


public:

	/*===== 定数 =====*/

	const float RADIUS = 35.0f;				// Draw and CheckHit Radius
	const float EASINIG_RADIUS = 10.0f;		// Easing Radius
	const int BREAK_COOL_TIME = 120;		// Resporn cool time
	const int EASING_TIMER = 120;			// max easing timer


public:

	/*===== メンバ関数 =====*/

	// constructor
	Bubble();

	// generate
	void Generate(const Vec2<float>& generatePos);

	// Initialize
	void Init();

	// Update
	void Update();

	// Draw
	void Draw();

};