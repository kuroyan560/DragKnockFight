#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"

class TitleScene : public BaseScene
{

	int frameHandle;		// �O�g�̃n���h��
	int starHandle;			// ���̃n���h��
	int charHandle;			// �L�����̃n���h��
	int robotHandle;		// ���{�b�g�̃n���h��
	int titleHandle;		// �^�C�g�����S�̃n���h��
	int pressStartHandle;	// PRESSSTART�̃n���h��


public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;
};

