#pragma once
#include "Vec.h"

// BubbleClass
class Bubble {

public:

	/*===== ƒƒ“ƒo•Ï” =====*/

	Vec2<float> pos;		// position
	float radius;			// Draw Radius
	int breakCoolTime;		// Resporn cool time
	bool isBreak;			// states ga break ka


public:

	/*===== ’è” =====*/

	const float CHECK_HIT_RADIUS = 25.0f;	// CheckHit Radius  and  Defoult Radius
	const int BREAK_COOL_TIME = 120;		// Resporn cool time


public:

	/*===== ƒƒ“ƒoŠÖ” =====*/

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