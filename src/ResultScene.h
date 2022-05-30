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
#include"IntoTheAbyss/CRT.h"

class ResultScene : public BaseScene
{
	int backGroundHandle;		// �w�i(��)�̉摜�n���h��
	int winnerFrameHandle;		// ���҂̃t���[���̉摜�n���h��
	int resultHandle;			// ���U���g�̉摜�n���h��
	int breakEnemyHandle;		// BREAK�̉摜�n���h�� �G
	int roundHandle;			// ROUND�̕`��
	int lunaHandle;
	int lacyHandle;
	int slashHandle;			//�X���b�V��
	int goodHandle;				//Good�̉摜�n���h��
	int greatHandle;			//Great�̉摜�n���h��
	int excellentHandle;		//Excellent�̉摜�n���h��
	int evaluationNowHandle;	
	bool evaluationFlag;		
	float lissajousTimer;
	std::array<int, 3> perfectHandle; //�X���b�V��
	std::array<int, 12> blueNumberHandle;// �̐����̉摜�n���h��

	//�X�R�A
	float baseBreakCount, breakCount, prevBreakCount;

	// �w�i�Ŕ�΂��L�����N�^�[�̃A�j���[�V�����^�C�}�[
	int backGroundCharaAnimTimer;
	int backGroundCharaAnimHandle;
	float backGroundCharaAngle;
	Vec2<float> backGroundCharaPos;
	Vec2<float> backGroundCharaVel;
	std::array<int, 3> useHandle;
	std::array<int, 3> redCharaHandle;
	std::array<int, 3> greenCharaHandle;
	std::array<int, 3> ironBallhandle;


	// �X�R�A
	int targetScore;				// �C�[�W���O�̖ڕW�l
	float scoreEffectTimer;			// �X�R�A���K���K���\�����邽�߂Ɏg�p����^�C�}�[
	std::array<int, 10> prevScore;	// �O�t���[���̃X�R�A
	std::array<float, 10> scoreSize;// �X�R�A�̃T�C�Y
	bool bigFontFlag;
	float defaultSize;

	//�]��
	float easeEvaluationPosY;
	int intervalTimer;
	bool timeUpFlag;
	float evaluationPosX;

	int perfectIndex,perfectInterval;

	// �e�^�C�}�[
	int resultUITimer;			// ���U���g�̉摜�̃C�[�W���O�Ɏg�p����^�C�}�[
	int breakEnemyUITimer;		// BREAK�̉摜�n���h���G�Ɏg�p����^�C�}�[
	int evaluationEasingTimer;
	int delayTimer;				// �e�C�[�W���O�̊Ԃ̒x���^�C�}�[


	// �e�C�[�W���O��
	float resultEasingAmount;		// ���U���g�̉摜�̃C�[�W���O��
	float breakCountEasingAmount;	// BREAK�̉摜�̃C�[�W���O��


	//�L�����̉摜
	int winnerGraph[PLAYABLE_CHARACTER_NUM];
	PLAYABLE_CHARACTER_NAME winnerName;

	Vec2<float> nowSize,breakSize, maxSize;

	CRT crt;
	bool endFlg = false;
	bool initSoundFlag;

	int ssIntervalTimer;

	std::array<int, 4>soundSe;
public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// �C�[�W���O���ʂ̍��W
	Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 200.0f, 30.0f };

	// �e�^�C�}�[�̃f�t�H���g�l
	const int RESULT_UI_TIMER = 20;
	const int BREAK_COUNTUI_TIMER = 20;
	const int SCORE_EFFECT_TIMER = 20;
	const int EVALUATION_EFFECT_TIMER = 20;
	const int DELAY_TIMER = 30;

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


	std::vector<int> CountNumber(int TIME)
	{
		float score = TIME;
		std::vector<int> Number(KuroFunc::GetDigit(TIME));
		for (int i = 0; i < Number.size(); ++i)
		{
			Number[i] = -1;
		}

		int tmp = score;
		//�X�R�A�v�Z
		for (int i = 0; tmp > 0; ++i)
		{
			float result = tmp % 10;
			//Number.push_back(result);
			Number[i] = result;
			tmp /= 10.0f;
		}
		//0����
		for (int i = 0; i < Number.size(); ++i)
		{
			if (Number[i] == -1)
			{
				Number[i] = 0;
			}
		}
		std::reverse(Number.begin(), Number.end());
		return Number;
	}
	void DrawBreakCount(float scoreEasingAmount,int BREAK_NOW_COUNT, int BREAK_MAX_COUNT, float OFFSET_X, Vec2<float> LISSAJOUS);
};