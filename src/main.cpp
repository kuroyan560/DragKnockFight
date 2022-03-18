#include<windows.h>
#include"KuroEngine.h"
#include"GameScene.h"
#include"Transform.h"
#include"Transform2D.h"
#include"Color.h"
#include"DrawFunc.h"

#ifdef _DEBUG
int main()
#else
//Windows�A�v���ł̃G���g���[�|�C���g�imain�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	//�G���W���ݒ�=====================
	EngineOption engineOption;
	engineOption.windowName = "KuroEngine3.0";
	engineOption.windowSize = { 1280,720 };
	engineOption.iconPath = nullptr;
	engineOption.backBuffClearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	engineOption.useHDR = false;
	engineOption.frameRate = 43;

	//===============================

	//�G���W���N��
	KuroEngine::Instance().Initialize(engineOption);

	//�V�[�����X�g=====================

	std::vector<BaseScene*>sceneList =
	{
		new GameScene()
	};
	int awakeScene = 0;	//�J�n���̃X�e�[�W�ԍ�

	//================================

	//�G���W���ɃV�[�����X�g��n��
	KuroEngine::Instance().SetSceneList(sceneList, awakeScene);

	//���[�v
	while (1)
	{
		KuroEngine::Instance().Update();
		KuroEngine::Instance().Draw();

		//�ÓI�N���X�������iDirty�t���O�n�j
		Transform::DirtyReset();
		Transform2D::DirtyReset();

		DrawFunc::CountReset();

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