#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"

class StageSelectScene : public BaseScene
{
	//ビッサミ提出用
	bool isSkip = false;

	//キャラクター選択中フラグ
	bool charactersSelect;


	/*-- マルチ、シングルを選択する画面で使用する変数 --*/

	bool isSelectMultiSingle;		// マルチシングル選択画面かフラグ
	bool isMulti;					// マルチプレイを選択
	bool isSingle;					// シングルプレイを選択
	float easingTimer;				// イージングに使用するタイマー
	Vec2<float> multiTexturePos;	// マルチプレイの画像の左上の座標
	Vec2<float> singleTexturePos;	// シングルプレイの画像の左上の座標
	int multiHandle;
	int singleHandle;

	const Vec2<float> SINGLE_MULTI_TEX_SIZE = { 1055.0f,640.0f };			// マルチプレイ、シングルプレイの説明画像のサイズ
	const Vec2<float> CURRENT_DEF_POS = { 0.0f,0.0f };						// 選択中の画像の左上の位置。
	const Vec2<float> CURRENT_INTERIM_POS = SINGLE_MULTI_TEX_SIZE / 2.0f;	// 選択した画像が正面に出る時に経由する座標。
	const Vec2<float> BEHIND_INTERIM_POS = -SINGLE_MULTI_TEX_SIZE / 2.0f;	// 選択されなかった画像が背面に戻る時に経由する座標。
	const Vec2<float> BEHIND_DEF_POS = SINGLE_MULTI_TEX_SIZE / 10.0f;		// 選択されなかった画像の左上の座標。
	const Vec2<float> PLAY_STR_POS = { 1700,700 };
	const float ADD_EASING_TIMER = 0.03f;	// イージングん加算量


public:
	StageSelectScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	// マルチかシングルかを選択する更新処理。
	void SelectMultiSingleUpdate();

	std::shared_ptr<SceneCange>changeScene;

	int stageNum;
};

