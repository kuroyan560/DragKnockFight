#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"

#include"IntoTheAbyss/Game.h"
#include"SceneCange.h"

class ResultScene : public BaseScene
{

	int backGroundHandle;		// �w�i(��)�̉摜�n���h��
	int winnerFrameHandle;		// ���҂̃t���[���̉摜�n���h��
	int resultHandle;			// ���U���g�̉摜�n���h��
	int breakEnemyHandle;		// BREAK�̉摜�n���h�� �G
	int breakPlayerHandle;		// BREAK�̉摜�n���h�� �v���C���[
	int scoreHandle;			// SCORE�̉摜�n���h��

	// �e�^�C�}�[
	int resultUITimer;			// ���U���g�̉摜�̃C�[�W���O�Ɏg�p����^�C�}�[
	int breakEnemyUITimer;		// BREAK�̉摜�n���h���G�Ɏg�p����^�C�}�[
	int breakPlayerUITimer;		// BREAK�̉摜�n���h���v���C���[�Ɏg�p����^�C�}�[
	int delayTimer;				// �e�C�[�W���O�̊Ԃ̒x���^�C�}�[

	// �e�C�[�W���O��
	float resultEasingAmount;		// ���U���g�̉摜�̃C�[�W���O��
	float breakEnemyEasingAmount;	// BREAK�̉摜�̃C�[�W���O��
	float breakPlayerEasingAmount;	// BREAK�̉摜�̃C�[�W���O��

	// �e�^�C�}�[�̃f�t�H���g�l
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int DELAY_TIMER = 30;


public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// �C�[�W���O���ʂ̍��W
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 80.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x + 10.0f, 150.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x + 90.0f, 250.0f };


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;
};