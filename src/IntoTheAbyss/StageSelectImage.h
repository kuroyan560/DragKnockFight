#pragma once
#include"../KuroEngine.h"
#include"IStageSelectImage.h"
#include"StageSelectScreenShot.h"

/// <summary>
/// ステージ選択画面の画像
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
	int backGroundHandle;//背景
	LerpData backGroundLerpData;

	bool zoomOutlag;

	//StageSelectScreenShot screenShot;
};

