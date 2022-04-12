#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"
#include"IntoTheAbyss/Game.h"
#include"WinApp.h"
#include<memory>
#include"Vec.h"

class TitleScene : public BaseScene
{

	// �摜�n���h���n
	int frameHandle;		// �O�g�̃n���h��
	int starHandle;			// ���̃n���h��
	int lacyHandle;			// [lacy]�̃n���h��
	int lunaHandle;			// [luna]�̃n���h��
	int lacyRobotHandle;	// [lacy]���{�b�g�̃n���h��
	int lunaRobotHandle;	// [luna]���{�b�g�̃n���h��
	int titleHandle;		// �^�C�g�����S�̃n���h��
	int pressStartHandle;	// PRESSSTART�̃n���h��


private:

	// �������W
	const Vec2<float> WINDOW_SIZE = { 1280.0f,720.0f };
	const Vec2<float> LUNA_POS = { 0.0f,80.0f };
	const Vec2<float> LACY_POS = { WINDOW_SIZE.x - 410.0f, 80.0f };
	const Vec2<float> LUNA_ROBOT_POS = { 160.0f, 300.0f };
	const Vec2<float> LACY_ROBOT_POS = { WINDOW_SIZE.x / 2.0f, 360.0f };


public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;
};

