#pragma once
#include "Vec.h"
#include "Singleton.h"

class GameSceneCameraMove : public Singleton<GameSceneCameraMove> {

public:

	/*===== ƒƒ“ƒo•Ï” =====*/

	Vec2<float> move;		// ˆÚ“®‚³‚¹‚é—Ê

	GameSceneCameraMove() { move = {}; }

};