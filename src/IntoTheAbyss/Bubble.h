#pragma once
#include "Vec.h"

// BubbleClass
class Bubble {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;		// position
	float radius;			// Draw Radius
	int breakCoolTime;		// Resporn cool time
	bool isBreak;			// states ga break ka


public:

	/*===== �萔 =====*/

	const float RADIUS = 35.0f;				// Draw and CheckHit Radius
	const int BREAK_COOL_TIME = 120;		// Resporn cool time


public:

	/*===== �����o�֐� =====*/

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