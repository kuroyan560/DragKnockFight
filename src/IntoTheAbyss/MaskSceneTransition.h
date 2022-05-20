#pragma once
#include "KuroEngine.h"

class MaskSceneTransition : public SceneTransition{

	public:
	MaskSceneTransition();
	~MaskSceneTransition();
	void OnStart()override;
	bool OnUpdate()override;
	void OnDraw()override;

	int maskHandle;		// �V�[���`�F���W�̃}�X�N�̉摜
	float expRate;		// �摜�̊g�嗦
	float angle;		// ��]�p�x

};