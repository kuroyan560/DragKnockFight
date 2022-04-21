#pragma once
#include "Singleton.h"
#include "KuroEngine.h"
#include"CharacterInterFace.h"

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

	//Crashの回数
	int leftCrashCount;
	int rightCrashCount;

	PLAYABLE_CHARACTER_NAME winner;

	void Init();
	void Draw();

};