#pragma once
#include "Singleton.h"
#include "Vec.h"

class StageSelectOffsetPosDebug : public Singleton<StageSelectOffsetPosDebug> {

public:

	Vec2<float> pos;

	void Init() {
		pos.x = 0;
		pos.y = 0;
	}

};