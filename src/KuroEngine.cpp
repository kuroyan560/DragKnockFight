#include "KuroEngine.h"
#include<ctime>
#include"Fps.h"

void BaseScene::Initialize()
{
	OnInitialize();
}

void BaseScene::Update()
{
	OnUpdate();
}

void BaseScene::Draw()
{
	OnDraw();
}

void BaseScene::ImguiDebug()
{
	OnImguiDebug();
}

void BaseScene::Finalize()
{
	OnFinalize();
}

void KuroEngine::Render()
{
	d3d12App->SetViewPortsAndScissorRects(winApp->GetExpandWinSize());

	scenes[nowScene]->Draw();

	//�V�[���J�ڕ`��
	if (nowSceneTransition != nullptr)
	{
		nowSceneTransition->Draw();
	}

	//�O���t�B�b�N�X�}�l�[�W���̃R�}���h���X�g�S���s
	gManager.CommandsExcute(d3d12App->GetCmdList());

	//Imgui
	d3d12App->SetBackBufferRenderTarget();
	imguiApp->BeginImgui();
	scenes[nowScene]->ImguiDebug();

	ImGui::Begin("Fps");
	ImGui::Text("fps : %.5f", fps->GetNowFps());
	ImGui::End();

	imguiApp->EndImgui(d3d12App->GetCmdList());
}

KuroEngine::~KuroEngine()
{
	//XAudio2�̉��
	FreeLibrary(xaudioLib);

	//�V�[���̍폜
	/*for (int i = 0; i < scenes.size(); ++i)
	{
		delete scenes[i];
	}*/

	printf("KuroEngine�V���b�g�_�E��\n");
}

void KuroEngine::Initialize(const EngineOption& Option)
{
	if (!invalid)
	{
		printf("�G���[�FKuroEngine�͋N���ςł�\n");
		return;
	}
	printf("KuroEngine���N�����܂�\n");

	//XAudio�̓ǂݍ���
	xaudioLib = LoadLibrary(L"XAudio2_9.dll");

	//�����擾�p�V�[�h����
	srand(time(NULL));

	//�E�B���h�E�A�v������
	winApp = std::make_unique<WinApp>(Option.windowName, Option.windowSize, Option.iconPath);

	//D3D12�A�v������
	d3d12App = std::make_unique<D3D12App>(winApp->GetHwnd(), Option.windowSize, Option.useHDR, Option.backBuffClearColor);

	//�C���v�b�g�Ǘ��A�v������
	usersInput = std::make_unique<UsersInput>(winApp->GetWinClass(), winApp->GetHwnd());

	//�����֘A�A�v��
	audioApp = std::make_unique<AudioApp>();

	//Imgui�A�v��
	imguiApp = std::make_unique<ImguiApp>(d3d12App->GetDevice(), winApp->GetHwnd());

	//FPS�Œ�@�\
	fps = std::make_shared<Fps>(Option.frameRate);


	//���s���e�s��萔�o�b�t�@����
	parallelMatProjBuff = d3d12App->GenerateConstantBuffer(sizeof(XMMATRIX), 1,
		&XMMATRIX(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, winApp->GetExpandWinSize().x, winApp->GetExpandWinSize().y, 0.0f, 0.0f, 1.0f)));

	printf("KuroEngine�N������\n");
	invalid = false;

}

void KuroEngine::SetSceneList(const std::vector<std::shared_ptr<BaseScene>> &SceneList, const int& AwakeSceneNum)
{
	//�V�[�����X�g�ړ�
	scenes = SceneList;
	nowScene = AwakeSceneNum;
	scenes[nowScene]->Initialize();
}

void KuroEngine::Update()
{
	//FPS�X�V
	fps->Update();

	//�����֘A�A�v���X�V
	audioApp->Update();

	//�V�[���؂�ւ��t���O
	bool sceneChangeFlg = false;

	if (nowSceneTransition != nullptr) //�V�[���J�ڒ�
	{
		//�V�[���J�ڃN���X�̍X�V�֐��́A�V�[���؂�ւ��̃^�C�~���O�� true ���҂�
		sceneChangeFlg = nowSceneTransition->Update() && (nextScene != -1);

		//�V�[���J�ڏI��
		if (nowSceneTransition->Finish())
		{
			nowSceneTransition = nullptr;
		}
	}

	//�V�[���؂�ւ�
	if (sceneChangeFlg)
	{
		scenes[nowScene]->Finalize();	//�؂�ւ��O�̃V�[���I������
		nowScene = nextScene;		//�V�[���؂�ւ�
		scenes[nowScene]->Initialize();	//�؂�ւ���̃V�[������������
		nextScene = -1;
	}

	//���͍X�V
	usersInput->Update(winApp->GetHwnd(), winApp->GetExpandWinSize());

	scenes[nowScene]->Update();
}

void KuroEngine::Draw()
{
	d3d12App->Render(this);
}