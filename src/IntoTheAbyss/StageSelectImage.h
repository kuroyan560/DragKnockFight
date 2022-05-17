#pragma once
#include"../KuroEngine.h"
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
	Vec2<float>lerpPos;
	Vec2<float>pos;
	Vec2<float>lerpSize;
	Vec2<float>size;

	bool zoomOutlag;

	StageSelectScreenShot screenShot;
};

