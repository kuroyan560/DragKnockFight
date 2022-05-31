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
	//ビッサミ提出用
	bool isSkip = false;

	//キャラクター選択中フラグ
	bool charactersSelect;

	StageSelectImage stageSelect;			// ステージ選択画面の背景(フレーム)
	StageSelectScreenShot screenShot;		// そのマップのスクショ
	StageSelectArrow rightArrow;			// 右矢印
	StageSelectArrow leftArrow;				// 左矢印
	StageSelectCharacterImage leftChara;	// 左側のキャラ
	StageSelectCharacterImage rightChara;	// 右側のキャラ

	bool isPrevInputStickRight;
	bool isPrevInputSticlLeft;

	int backAlpha;		// TitleSceneから遷移した際にだんだん明るくするための変数

	int bgm;
	const float bgmVol = 0.2f;

public:
	StageSelectScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	// シーンチェンジに使用する。
	std::shared_ptr<MaskSceneTransition>maskSceneChange;
	std::shared_ptr< SceneTransition>sceneChange;

	enum
	{
		STAGE_SELECT,
		SCENE_CHANGE
	};
	std::vector<std::array<DrawMapChipForSceneChange, 2>>mapScreenShot;

	// ステージ番号
	int stageNum,prevStageNum;
};

