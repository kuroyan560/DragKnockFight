#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"
#include"IntoTheAbyss/StageSelectImage.h"
#include"IntoTheAbyss/StageSelectScreenShot.h"
#include"IntoTheAbyss/StageSelectArrow.h"
#include"IntoTheAbyss/StageSelectCharacterImage.h"
#include"../src/IntoTheAbyss/MaskSceneTransition.h"
#include"IntoTheAbyss/DrawMapChipForSceneChange.h"

class StageSelectScene : public BaseScene
{
	//�r�b�T�~��o�p
	bool isSkip = false;

	//�L�����N�^�[�I�𒆃t���O
	bool charactersSelect;

	StageSelectImage stageSelect;			// �X�e�[�W�I����ʂ̔w�i(�t���[��)
	StageSelectScreenShot screenShot;		// ���̃}�b�v�̃X�N�V��
	StageSelectArrow rightArrow;			// �E���
	StageSelectArrow leftArrow;				// �����
	StageSelectCharacterImage leftChara;	// �����̃L����
	StageSelectCharacterImage rightChara;	// �E���̃L����

	bool isPrevInputStickRight;
	bool isPrevInputSticlLeft;

public:
	StageSelectScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	// �V�[���`�F���W�Ɏg�p����B
	std::shared_ptr<MaskSceneTransition>maskSceneChange;
	std::shared_ptr< SceneTransition>sceneChange;

	enum
	{
		STAGE_SELECT,
		SCENE_CHANGE
	};
	std::array<std::array<DrawMapChipForSceneChange, 2>, 15>mapScreenShot;

	// �X�e�[�W�ԍ�
	int stageNum,prevStageNum;
};

