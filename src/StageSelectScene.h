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
	Vec2<float> multiTextureInterimPos;	// マルチプレイの画像のイージングの経由地点
	Vec2<float> multiTextureEndPos;		// マルチプレイの画像のイージングの到達地点
	Vec2<float> singleTexturePos;		// シングルプレイの画像の左上の座標
	Vec2<float> singleTextureInterimPos;// シングルプレイの画像のイージングの経由地点
	Vec2<float> singleTextureEndPos;	// シングルプレイの画像のイージングの到達地点

	bool isInvisibleArrow;			// 矢印を不可視にするかどうかフラグ
	int invisibleArrowTimer;		// 矢印を更かしにするタイマー



	// 画像ハンドル
	int multiHandle;	// マルチの画像ハンドル
	int singleHandle;	// シングルの画像ハンドル
	int hitoriHandle;	// 右下の一人プレイの画像ハンドル
	int futariHandle;	// 右下の二人プレイの画像ハンドル
	int arrowHandle;	// 矢印の画像ハンドル


public:


	/*-- マルチ、シングルを選択する画面で使用する定数 --*/

	const Vec2<float> SINGLE_MULTI_TEX_SIZE = { 1055.0f,640.0f };			// マルチプレイ、シングルプレイの説明画像のサイズ
	const Vec2<float> CURRENT_DEF_POS = { 0.0f,0.0f };						// 選択中の画像の左上の位置。
	const Vec2<float> CURRENT_INTERIM_POS = SINGLE_MULTI_TEX_SIZE / 2.0f;	// 選択した画像が正面に出る時に経由する座標。
	const Vec2<float> BEHIND_INTERIM_POS = -SINGLE_MULTI_TEX_SIZE / 2.0f;	// 選択されなかった画像が背面に戻る時に経由する座標。
	const Vec2<float> BEHIND_DEF_POS = SINGLE_MULTI_TEX_SIZE / 10.0f;		// 選択されなかった画像の左上の座標。
	const Vec2<float> PLAY_STR_POS = { 330,500 };							// 何人プレイかを描画する座標。
	const float ADD_EASING_TIMER = 0.03f;	// イージングん加算量
	const int INVISIBLE_ARROW_TIMER = 30;


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


private:

	// マルチ、シングル選択画面でのイラストを動かす処理
	void UpdateEasingMultiSingle(const float& EasingRate, Vec2<float>& SinglePos, Vec2<float>& MultiPos,
		const Vec2<float>& SingleDefPos, const Vec2<float>& SingleIntermPos, const Vec2<float>& SingleEndPos,
		const Vec2<float>& MultiDefPos, const Vec2<float>& MultiIntermPos, const Vec2<float>& MultiEndPos);

};

