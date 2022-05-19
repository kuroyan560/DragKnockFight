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

	inline const bool& GetZoomOutFlag() { return zoomOutFlag; }
	inline void SetZoomFlag(const bool& Zoom) { zoomOutFlag = Zoom; }

	// 演出用の拡縮をセット。
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) { expData.Init(expPos, expRate); };

private:

	// 背景
	int backGroundHandle;

	// 背景画像を拡縮したり動かしたりするための変数
	LerpData backGroundLerpData;

	// 選択された時に拡縮するための変数
	LerpData expData;

	// ズームアウトするかどうかのフラグ trueだったらズームする。
	bool zoomOutFlag;

	//StageSelectScreenShot screenShot;
};

