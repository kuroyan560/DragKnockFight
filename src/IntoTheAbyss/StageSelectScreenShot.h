#pragma once
#include"../KuroEngine.h"
#include<array>

class StageSelectScreenShot
{
public:
	StageSelectScreenShot();
	void Init();
	void Update();
	void Draw();

	void Next();
	void Prev();

	void ImGuiDraw();
private:
	int selectNum;
	std::array<int, 3> screenShotHandle;
	Vec2<float>pos, size;
	bool zoomOutFlag;
};

