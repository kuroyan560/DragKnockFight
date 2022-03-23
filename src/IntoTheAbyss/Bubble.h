#pragma once
#include "Vec.h"

// BubbleClass
class Bubble {

public:

	/*===== ƒƒ“ƒo•Ï” =====*/

	Vec2<float> pos;		// position
	Vec2<float> scale;		// Draw Scale Hikinobashi
	Vec2<float> easingScale;
	float radius;			// Draw Radius
	int breakCoolTime;		// Resporn cool time
	bool isBreak;			// states ga break ka
	int graphHandle;
	int easingTimer;		// easing timer
	bool easingFlag;		// Šgk‚ÍXY²‚Ås‚¤‚½‚ßA‚»‚ê‚¼‚ê‚Ì²‚ÅŠg’£‚Æûk‚Ì‚Ç‚¿‚ç‚ğs‚¤‚©‚ğ”»’f‚·‚é‚½‚ß‚Ìƒtƒ‰ƒO


public:

	/*===== ’è” =====*/

	const float RADIUS = 35.0f;				// Draw and CheckHit Radius
	const float EASINIG_RADIUS = 10.0f;		// Easing Radius
	const int BREAK_COOL_TIME = 120;		// Resporn cool time
	const int EASING_TIMER = 60;			// max easing timer


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