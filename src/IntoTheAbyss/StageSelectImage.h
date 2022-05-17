#pragma once
#include"../KuroEngine.h"

/// <summary>
/// ƒXƒe[ƒW‘I‘ğ‰æ–Ê‚Ì”wŒi
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
	int backGroundHandle;//”wŒi
	Vec2<float>lerpPos;
	Vec2<float>pos;
	Vec2<float>lerpSize;
	Vec2<float>size;

	bool zoomOutlag;
};

