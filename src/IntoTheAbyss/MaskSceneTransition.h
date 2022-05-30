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
	int maskHandle;		// �V�[���`�F���W�̃}�X�N�̉摜
	float expRate;		// �摜�̊g�嗦
	float angle;		// ��]�p�x
	float t = 0.0f;
	bool startFlag;

	Vec2<float>pos, backGroundPos;
};