#pragma once
#include "Singleton.h"
#include "KuroEngine.h"

class RenderTarget;
class GaussianBlur;

// GameSceneの最後の画像をぼやかしてResultSceneで描画するためのクラス
class ResultSceneBackGround : public Singleton<ResultSceneBackGround> {

	friend class Singleton<ResultSceneBackGround>;

public:

	/*===== メンバ変数 =====*/

	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<RenderTarget>backGround;

	void Init();
	void Draw();

};