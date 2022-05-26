#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"
#include<vector>
#include<array>

#include"IntoTheAbyss/Game.h"
#include"InToTheAbyss/SceneCange.h"
#include"IntoTheAbyss/CharacterInterFace.h"

class ResultScene : public BaseScene
{

	int backGroundHandle;		// �w�i(��)�̉摜�n���h��
	int winnerFrameHandle;		// ���҂̃t���[���̉摜�n���h��
	int resultHandle;			// ���U���g�̉摜�n���h��
	int breakEnemyHandle;		// BREAK�̉摜�n���h�� �G
	int breakPlayerHandle;		// BREAK�̉摜�n���h�� �v���C���[
	int crashEnemyHandle;		// CRASH�̉摜�n���h�� �G
	int crashPlayerHandle;		// CRASH�̉摜�n���h�� �v���C���[
	int scoreHandle;			// SCORE�̉摜�n���h��
	int crossHandle;			// *�̉摜�n���h��
	std::array<int, 12> blueNumberHandle;// �̐����̉摜�n���h��
	std::array<int, 12> goldNumberHandle;// ���̐����̉摜�n���h��

	// �X�R�A
	int targetScore;				// �C�[�W���O�̖ڕW�l
	float scoreEffectTimer;			// �X�R�A���K���K���\�����邽�߂Ɏg�p����^�C�}�[
	std::array<int, 10> prevScore;	// �O�t���[���̃X�R�A
	std::array<float, 10> scoreSize;// �X�R�A�̃T�C�Y

	// �e�^�C�}�[
	int resultUITimer;			// ���U���g�̉摜�̃C�[�W���O�Ɏg�p����^�C�}�[
	int breakEnemyUITimer;		// BREAK�̉摜�n���h���G�Ɏg�p����^�C�}�[
	int breakPlayerUITimer;		// BREAK�̉摜�n���h���v���C���[�Ɏg�p����^�C�}�[
	int crashEnemyUITimer;		// CRASH�̉摜�n���h���G�Ɏg�p����^�C�}�[
	int crashPlayerUITimer;		// CRASH�̉摜�n���h���v���C���[�Ɏg�p����^�C�}�[
	int scoreUITimer;			// SCORE�̉摜�n���h���Ɏg�p����^�C�}�[
	int delayTimer;				// �e�C�[�W���O�̊Ԃ̒x���^�C�}�[

	// �e�N���b�V���̐���
	int breakEnemyAmount;		// �G�̃u���[�N�̐�
	int breakPlayerAmount;		// �v���C���[�̃u���[�N�̐�
	int crashEnemyAmount;		// �G�̃N���b�V���̐�
	int crashPlayerAmount;		// �v���C���[�̃N���b�V���̐�

	// �e�C�[�W���O��
	float resultEasingAmount;		// ���U���g�̉摜�̃C�[�W���O��
	float breakEnemyEasingAmount;	// BREAK�̉摜�̃C�[�W���O��
	float breakPlayerEasingAmount;	// BREAK�̉摜�̃C�[�W���O��
	float crashEnemyEasingAmount;	// CRASH�̉摜�̃C�[�W���O��
	float crashPlayerEasingAmount;	// CRASH�̉摜�̃C�[�W���O��
	float scoreEasingAmount;		// SCORE�̉摜�̃C�[�W���O��
	float scoreEffectEasingAmount;	// �X�R�A�̃K���K���\�����邽�߂Ɏg�p����^�C�}�[

	//�L�����̉摜
	int winnerGraph[PLAYABLE_CHARACTER_NUM];
	PLAYABLE_CHARACTER_NAME winnerName;

public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// �C�[�W���O���ʂ̍��W
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 200.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 150.0f };
	const Vec2<float> CRASH_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 250.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x - 110.0f, 350.0f };
	const Vec2<float> CRASH_PLAYER_POS = { (float)WINDOW_CENTER.x - 110.0f, 450.0f };
	const Vec2<float> SCORE_POS = { (float)WINDOW_CENTER.x - 100.0f, (float)WINDOW_CENTER.y + 180.0f };

	// �e�^�C�}�[�̃f�t�H���g�l
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int CRASH_ENEMY_UI_TIMER = 20;
	const int CRASH_PLAYER_UI_TIMER = 20;
	const int SCORE_UI_TIMER = 40;
	const int DELAY_TIMER = 30;
	const int SCORE_EFFECT_TIMER = 180;


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;


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
	void DrawBREAK(const Vec2<float>& targetPosm, const float& easingTimer, const int& graphHandle, const int& breakCount);

	// [SCORE][�X�R�A]��`��
	void DrawSCORE(const float& easingTimer, const double& scoreEffectEasingTimer);

};