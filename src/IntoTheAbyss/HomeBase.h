#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/AreaCollider.h"
#include<vector>

class HomeBase
{
public:
	HomeBase();

	void Init(const Vec2<float>& LEFT_UP_POS, const Vec2<float>& RIGHT_DOWN_POS, const bool& LeftPlayer);
	bool Collision(const Square& OBJ_A);
	void Draw();
	void Debug();

	const Vec2<float>& GetLeftUpPos() { return leftUpPos; }
	Vec2<float> GetRightUpPos() { return Vec2<float>(rightDownPos.x, leftUpPos.y); }
	const Vec2<float>& GetRightDownPos() { return rightDownPos; }
	Vec2<float> GetLeftDownPos() { return Vec2<float>(leftUpPos.x, rightDownPos.y); }


	Square hitBox;
private:
	Vec2<float>leftUpPos, rightDownPos, centerPos;
	static int AREA_NUM;
	std::string name;


	bool leftPlayer;

	static const int BORDER_GRAPH_HEIGHT = 720;
	static const int BORDER_GRAPH_WIDTH = 25;
	std::vector<float> borderHeight;

};
