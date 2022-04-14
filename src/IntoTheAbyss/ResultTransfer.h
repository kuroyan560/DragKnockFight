#pragma once
#include "Singleton.h"
#include "KuroEngine.h"

class RenderTarget;
class GaussianBlur;

// GameSceneでの情報をリザルトシーンへ受け渡すためのクラス
class ResultTransfer : public Singleton<ResultTransfer> {

	friend class Singleton<ResultTransfer>;
	ResultTransfer();

public:

	/*===== メンバ変数 =====*/

	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<RenderTarget>backGround;

	//最終的なスコア
	int resultScore;

	//Breakの回数
	int leftBreakCount;
	int rightBreakCount;

	void Init();
	void Draw();

};