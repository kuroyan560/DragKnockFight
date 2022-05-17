#pragma once
#include"IStageSelectImage.h"
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
	LerpData screenShotLerpData;
	bool zoomOutFlag;
};

