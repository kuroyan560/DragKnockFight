#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"
#include<vector>

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
	int crossHandle;			// *�̉摜�n���h��
	std::vector<int> blueNumberHandle;// �̐����̉摜�n���h��
	std::vector<int> goldNumberHandle;// ���̐����̉摜�n���h��

	// �e�^�C�}�[
	int resultUITimer;			// ���U���g�̉摜�̃C�[�W���O�Ɏg�p����^�C�}�[
	int breakEnemyUITimer;		// BREAK�̉摜�n���h���G�Ɏg�p����^�C�}�[
	int breakPlayerUITimer;		// BREAK�̉摜�n���h���v���C���[�Ɏg�p����^�C�}�[
	int delayTimer;				// �e�C�[�W���O�̊Ԃ̒x���^�C�}�[

	// �e�N���b�V���̐���
	int breakEnemyAmount;		// �G�̃N���b�V���̐�
	int breakPlayerAmount;		// �v���C���[�̃N���b�V���̐�

	// �e�C�[�W���O��
	float resultEasingAmount;		// ���U���g�̉摜�̃C�[�W���O��
	float breakEnemyEasingAmount;	// BREAK�̉摜�̃C�[�W���O��
	float breakPlayerEasingAmount;	// BREAK�̉摜�̃C�[�W���O��


public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// �C�[�W���O���ʂ̍��W
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 80.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x + 10.0f, 150.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x + 90.0f, 250.0f };

	// �e�^�C�}�[�̃f�t�H���g�l
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int DELAY_TIMER = 30;


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;


private:
	// �������擾�B
	inline int CountDisits(const int& disits) {
		return to_string(disits).size();
	}
	// �w��̌����̐������擾�B
	inline int GetDisit(const int& disits, const int& disit) {
		return (disits % (int)pow(10, disit + 1)) / pow(10, disit);
	}

	// [BREAK]��`��
	void DrawBREAK(const Vec2<float>& windowSize, const Vec2<float>& targetPosm, const float& easingTimer, const bool& isBoss, const int& breakCount);

};