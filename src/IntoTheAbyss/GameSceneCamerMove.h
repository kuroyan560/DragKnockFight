#pragma once
#include "Vec.h"
#include "Singleton.h"

class GameSceneCameraMove : public Singleton<GameSceneCameraMove> {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> move;		// �ړ��������

	GameSceneCameraMove() { move = {}; }

};