#pragma once
#include"../KuroEngine.h"
#include"CharacterManager.h"
#include"IStageSelectImage.h"

class StageSelectCharacterImage
{
public:
	StageSelectCharacterImage();
	void Init(const Vec2<float>& ZoomInPos, const Vec2<float>& ZoomOutPos, const int& GraphHandle);
	void Update();
	void Draw();

	inline void SetIsZoomOut(const bool& Zoom) {
		isZoomOut = Zoom;
		pos.timer = 0.0f;
	}

private:

	/*===== メンバ変数 =====*/

	LerpData zoomInPos;		// ズームインしているときの座標
	LerpData zoomOutPos;	// ズームアウトしているときの座標
	LerpData pos;			// 描画情報

	bool isZoomOut;			// ズームアウトしているのフラグ

	int graphHandle;		// 使用する画像のハンドル

};

