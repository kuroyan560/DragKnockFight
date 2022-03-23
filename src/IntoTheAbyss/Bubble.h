#pragma once
#include "Vec.h"

// BubbleClass
class Bubble {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;		// position
	Vec2<float> scale;		// Draw Scale Hikinobashi
	Vec2<float> easingScale;
	float radius;			// Draw Radius
	int breakCoolTime;		// Resporn cool time
	bool isBreak;			// states ga break ka
	int graphHandle;
	int easingTimer;		// easing timer
	bool easingFlag;		// �g�k��XY���ōs�����߁A���ꂼ��̎��Ŋg���Ǝ��k�̂ǂ�����s�����𔻒f���邽�߂̃t���O


public:

	/*===== �萔 =====*/

	const float RADIUS = 35.0f;				// Draw and CheckHit Radius
	const float EASINIG_RADIUS = 10.0f;		// Easing Radius
	const int BREAK_COOL_TIME = 120;		// Resporn cool time
	const int EASING_TIMER = 60;			// max easing timer


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