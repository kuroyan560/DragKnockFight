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

	inline void SetZoomFlag(const bool& Zoom) { zoomOutFlag = Zoom; }

	// 演出用の拡縮をセット。
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) { expData.Init(expPos, expRate); };

private:
	int selectNum;
	std::array<int, 3> screenShotHandle;
	LerpData screenShotLerpData;

	// 選択された時に拡縮するための変数
	LerpData expData;

	bool zoomOutFlag;
};

