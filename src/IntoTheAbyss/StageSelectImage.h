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
	void Init(bool MOVE_FROM_GAME_FLAG);
	void Update();
	void Draw();

	void ImGuiDraw();

	inline const bool& GetZoomOutFlag() { return zoomOutFlag; }
	void SetZoomFlag(const bool& Zoom);

	// 演出用の拡縮をセット。
	inline void SetExp(const Vec2<float>& expPos, const Vec2<float>& expRate) {
		expData.pos = expPos;
		expData.size = expRate;
		expData.timer = 0.0f;
	};

	inline LerpData& GetLerpData() { return backGroundLerpData; }

private:

	// 背景
	int backGroundHandle;

	// 背景画像を拡縮したり動かしたりするための変数
	LerpData backGroundLerpData;

	// 選択された時に拡縮するための変数
	LerpData expData;

	// ズームアウトするかどうかのフラグ trueだったらズームする。
	bool zoomOutFlag;

	//StageSelectScreenShot screenShot;[

	Vec2<float>startStageSelectSize;//ゲーム画面からステージ選択画面に入る際のズーム処理
};

