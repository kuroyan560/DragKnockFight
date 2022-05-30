#pragma once
#include "KuroEngine.h"

class MaskSceneTransition : public SceneTransition{
public:
	MaskSceneTransition();
	~MaskSceneTransition();
	void OnStart()override;
	bool OnUpdate()override;
	void OnDraw()override;
	float GetChangeRate() { return t; }

	std::shared_ptr<RenderTarget>backGroundTex;
private:
	int maskHandle;		// シーンチェンジのマスクの画像
	float expRate;		// 画像の拡大率
	float angle;		// 回転角度
	float t = 0.0f;
	bool startFlag;

	Vec2<float>pos, backGroundPos;
};