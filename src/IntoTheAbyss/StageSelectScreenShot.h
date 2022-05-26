#pragma once
#include"IStageSelectImage.h"
#include<array>

class StageSelectScreenShot
{
public:
	StageSelectScreenShot(int* SelectNum);
	void Init();
	void Update();
	void Draw();

	void ImGuiDraw();

	void SetZoomFlag(const bool& Zoom);

	// ���o�p�̊g�k���Z�b�g�B
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) {
		expData.pos = expPos;
		expData.size = expRate;
	};

	int GetCanMaxSelectNum() { return min(screenShotHandle.size(), stageNumberHandle.size()); }

private:
	std::vector<int>screenShotHandle;
	std::vector<int> stageNumberHandle;
	LerpData screenShotLerpData;
	LerpData stageNumberData;

	// �I�����ꂽ���Ɋg�k���邽�߂̕ϐ�
	LerpData expData;
	LerpData stageNumberExpData;

	bool zoomOutFlag;

	float timer;		// ���T�[�W���Ȑ��Ɏg�p����^�C�}�[

	int* selectNumPtr;
};

