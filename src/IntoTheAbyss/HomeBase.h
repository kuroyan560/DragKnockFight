#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/AreaCollider.h"
#include<vector>

class HomeBase
{
public:
	HomeBase();

	void Init(const Vec2<float>& LEFT_UP_POS, const Vec2<float>& RIGHT_DOWN_POS, const bool& LeftPlayer);
	bool Collision(const Square &OBJ_A);
	void Draw();
	void Debug();

private:
	Vec2<float>leftUpPos, rightDownPos, centerPos;
	static int AREA_NUM;
	std::string name;

	Square hitBox;

	bool leftPlayer;
	
	static const int BORDER_GRAPH_HEIGHT = 720;
	static const int BORDER_GRAPH_WIDTH = 25;
	std::vector<float> borderHeight;
};
