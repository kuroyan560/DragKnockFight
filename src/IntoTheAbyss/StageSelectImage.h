#pragma once
#include"../KuroEngine.h"
#include"IStageSelectImage.h"
#include"StageSelectScreenShot.h"

/// <summary>
/// �X�e�[�W�I����ʂ̉摜
/// </summary>
class StageSelectImage
{
public:
	StageSelectImage();
	void Init();
	void Update();
	void Draw();

	void ImGuiDraw();
private:
	int backGroundHandle;//�w�i
	LerpData backGroundLerpData;

	bool zoomOutlag;

	//StageSelectScreenShot screenShot;
};

