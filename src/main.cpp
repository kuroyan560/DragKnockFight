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
//Windows�A�v���ł̃G���g���[�|�C���g�imain�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	//�G���W���ݒ�=====================
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

	//�G���W���N��
	KuroEngine::Instance().Initialize(engineOption);

	//�V�[�����X�g=====================

	std::vector<BaseScene*>sceneList =
	{
		new ResultScene(),
		new TitleScene(),
		new StageSelectScene(),
		new GameScene(),
		new TutorialScene()
	};
	int awakeScene = 0;	//�J�n���̃X�e�[�W�ԍ�

	//================================

	//�G���W���ɃV�[�����X�g��n��
	KuroEngine::Instance().SetSceneList(sceneList, awakeScene);

	//���[�v
	while (1)
	{

		DebugKeyManager::Instance()->CountReset();

		KuroEngine::Instance().Update();
		KuroEngine::Instance().Draw();

		//�ÓI�N���X�������iDirty�t���O�n�j
		Transform::DirtyReset();
		Transform2D::DirtyReset();

		DrawFunc::CountReset();
		DrawFunc_Color::CountReset();
		DrawFunc_Shadow::CountReset();
		DrawFunc_FillTex::CountReset();
		DrawFunc_Mask::CountReset();

		//���b�Z�[�W������H
		MSG msg{};
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//�L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);	//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}

		//�I�����b�Z�[�W������ / �V�[���}�l�[�W���̏I���@�Ń��[�v�𔲂���
		if (msg.message == WM_QUIT || KuroEngine::Instance().End())
		{
			break;
		}
	}
}