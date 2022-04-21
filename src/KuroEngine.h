#pragma once
//�G���W���{��==========================================================
#include<string>
#include"Vec.h"
#include<memory>
#include"Color.h"

#include"D3D12App.h"
#include"WinApp.h"
#include"UsersInput.h"
#include"AudioApp.h"
#include"ImguiApp.h"
#include"GraphicsManager.h"

class Fps;

//�V�[���̊��N���X
class BaseScene
{
protected:
	BaseScene() {}
	virtual void OnInitialize() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnImguiDebug() = 0;
	virtual void OnFinalize() = 0;
public:
	virtual ~BaseScene() {};
	void Initialize();
	void Update();
	void Draw();
	void ImguiDebug();
	void Finalize();
};

//�V�[���J�ځi��{�͌p�����ė��p���邪�A�P�t���؂�ւ��J�ڂ͌p���Ȃ��ŗ��p�ł���j
class SceneTransition
{
protected:
	bool nowTrans = false;
	virtual void OnStart() { }
	virtual bool OnUpdate() { nowTrans = false; return true; }
	virtual void OnDraw() {}
public:
	virtual ~SceneTransition() {}
	void Start() { nowTrans = true; OnStart(); }	//�V�[���J�ڃX�^�[�g
	bool Update() { return OnUpdate(); }	//�V�[����؂�ւ���^�C�~���O��true��Ԃ�
	void Draw() { OnDraw(); }
	bool Finish() { return !nowTrans; }	//�V�[���J�ڂ����S�ɏI��������
};

//�A�v���̏����ݒ�
struct EngineOption
{
	//�E�B���h�E�֘A
	std::string windowName;	//�E�B���h�E��
	Vec2<int>windowSize;	//�E�B���h�E�T�C�Y
	const wchar_t *iconPath = nullptr;	//�E�B���h�E�A�C�R���t�@�C����

	//�o�b�N�o�b�t�@�֘A
	Color backBuffClearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	bool useHDR = false;	//HDR���g�����iHDR�Ή����Ă��Ȃ��ꍇ�g�p���Ȃ��j

	//�t���[�����[�g
	int frameRate = 60;

	//imgui�\���t���O
	bool imguiActive = true;
};

//�G���W���{��
class KuroEngine : public D3D12AppUser
{
public:
	//�V���O���g��
	static KuroEngine &Instance()
	{
		static KuroEngine engine;
		return engine;
	}

private:
	KuroEngine() {}

	//����������Ă��邩
	bool invalid = true;

	//XAudio
	HMODULE xaudioLib;
	//�E�B���h�E�A�v��
	std::unique_ptr<WinApp>winApp;
	//D3D12�A�v��
	std::unique_ptr<D3D12App>d3d12App;
	//���͊Ǘ�
	std::unique_ptr<UsersInput>usersInput;
	//�����֘A�A�v��
	std::unique_ptr<AudioApp>audioApp;
	//Imgui�A�v��
	std::unique_ptr<ImguiApp>imguiApp;

	//�O���t�B�b�N�X�}�l�[�W���i�����_�����O�����AZ�\�[�g�Ȃǂ��s���j
	GraphicsManager gManager;
	std::shared_ptr<ConstantBuffer>parallelMatProjBuff;

	//�V�[�����X�g
	std::vector<BaseScene *>scenes;
	int nowScene;	//���݂̃V�[���ԍ�
	int nextScene = -1;	//���̃V�[���ԍ�
	std::weak_ptr<SceneTransition>nowSceneTransition;	//���݃Z�b�g����Ă���V�[���J��

	//FPS�Œ�
	std::shared_ptr<Fps>fps;

	//imgui�\���t���O
	bool imguiActive;

	//�I���t���O
	bool end = false;

	void Render()override;
public:
	~KuroEngine();
	void Initialize(const EngineOption &Option);
	void SetSceneList(const std::vector<BaseScene *> &SceneList, const int &AwakeSceneNum);
	void Update();
	void Draw();
	bool End() { return end; }

	//�Q�[���I��
	void GameEnd() { end = true; }
	//�V�[���`�F���W
	void ChangeScene(const int& SceneNum, const std::shared_ptr< SceneTransition>& SceneTransition)
	{
		nextScene = SceneNum;
		nowSceneTransition = SceneTransition;
		SceneTransition->Start();
	}

	//�O���t�B�b�N�X�}�l�[�W���Q�b�^
	GraphicsManager &Graphics() { return gManager; }

	//���s���e�s��萔�o�b�t�@
	const std::shared_ptr<ConstantBuffer> &GetParallelMatProjBuff()
	{
		return parallelMatProjBuff;
	}

};

