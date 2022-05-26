#include<windows.h>
#include"KuroEngine.h"
#include"GameScene.h"
#include"Transform.h"
#include"Transform2D.h"
#include"Color.h"
#include"DrawFunc.h"
#include"DrawFunc_Color.h"
#include"DrawFunc_Shadow.h"
#include"DrawFunc_FillTex.h"
#include"DrawFunc_Mask.h"
#include"IntoTheAbyss/TitleScene.h"
#include"../src/TutorialScene.h"
#include"ResultScene.h"
#include"StageSelectScene.h"
#include"IntoTheAbyss/DebugKeyManager.h"


#ifdef _DEBUG
int main()
#else
//Windowsアプリでのエントリーポイント（main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	//エンジン設定=====================
	EngineOption engineOption;
	engineOption.windowName = "DragKnockFight";
	engineOption.windowSize = { 1280,720 };
	engineOption.iconPath = nullptr;
	engineOption.backBuffClearColor = Color(56, 22, 74, 255);
	engineOption.useHDR = false;
#ifdef _DEBUG
	engineOption.frameRate = 43;
#else
	engineOption.frameRate = 200;
#endif

	engineOption.imguiActive = true;

	//===============================

	//エンジン起動
	KuroEngine::Instance().Initialize(engineOption);

	//シーンリスト=====================

	std::vector<BaseScene*>sceneList =
	{
		new ResultScene(),
		new TitleScene(),
		new StageSelectScene(),
		new GameScene(),
		new TutorialScene()
	};
	int awakeScene = 0;	//開始時のステージ番号

	//================================

	//エンジンにシーンリストを渡す
	KuroEngine::Instance().SetSceneList(sceneList, awakeScene);

	//ループ
	while (1)
	{

		DebugKeyManager::Instance()->CountReset();

		KuroEngine::Instance().Update();
		KuroEngine::Instance().Draw();

		//静的クラス初期化（Dirtyフラグ系）
		Transform::DirtyReset();
		Transform2D::DirtyReset();

		DrawFunc::CountReset();
		DrawFunc_Color::CountReset();
		DrawFunc_Shadow::CountReset();
		DrawFunc_FillTex::CountReset();
		DrawFunc_Mask::CountReset();

		//メッセージがある？
		MSG msg{};
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
		}

		//終了メッセージが来た / シーンマネージャの終了　でループを抜ける
		if (msg.message == WM_QUIT || KuroEngine::Instance().End())
		{
			break;
		}
	}
}