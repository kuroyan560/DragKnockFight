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

	// ���o�p�̊g�k���Z�b�g�B
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) { expData.Init(expPos, expRate); };

private:
	int selectNum;
	std::array<int, 3> screenShotHandle;
	LerpData screenShotLerpData;

	// �I�����ꂽ���Ɋg�k���邽�߂̕ϐ�
	LerpData expData;

	bool zoomOutFlag;
};

