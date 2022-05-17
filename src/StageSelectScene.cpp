#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"

StageSelectScene::StageSelectScene()
{
	changeScene = std::make_shared<SceneCange>();
	stageNum = 0;
}

void StageSelectScene::OnInitialize()
{
	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	stageSelect.Init();
}

void StageSelectScene::OnUpdate()
{
	if (isSkip)
	{
		KuroEngine::Instance().ChangeScene(2, changeScene);
		SelectStage::Instance()->resetStageFlag = true;
	}

	if (charactersSelect)
	{
		//�L�����N�^�[�I���X�V
		CharacterManager::Instance()->CharactersSelectUpdate();

		//�Q�[���V�[���Ɉړ�����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			KuroEngine::Instance().ChangeScene(2, changeScene);
			SelectStage::Instance()->resetStageFlag = true;
		}
		//�X�e�[�W�I���֖߂�
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
		{
			charactersSelect = false;
		}
	}
	else
	{
		//�L�����N�^�[�I����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			charactersSelect = true;
		}
		//�^�C�g���V�[���Ɉړ�����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
		{
			KuroEngine::Instance().ChangeScene(0, changeScene);
		}

		//�X�e�[�W�ԍ��𑝂₷
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_UP))
		{
			++stageNum;
		}
		//�X�e�[�W�ԍ������炷
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_DOWN))
		{
			--stageNum;
		}


		if (StageMgr::Instance()->GetMaxStageNumber() - 1 <= stageNum)
		{
			stageNum = StageMgr::Instance()->GetMaxStageNumber() - 1;
		}
		if (stageNum <= 0)
		{
			stageNum = 0;
		}
		SelectStage::Instance()->SelectStageNum(stageNum);
	}

	stageSelect.Update();
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	stageSelect.Draw();
}

void StageSelectScene::OnImguiDebug()
{
	if (charactersSelect)
	{
		CharacterManager::Instance()->CharactersSelectDraw();
	}
	else
	{
		ImGui::Begin("StageSelect");
		ImGui::Text("StageNumber:%d", stageNum);
		ImGui::Text("MaxStageNumber:%d", StageMgr::Instance()->GetMaxStageNumber() - 1);
		ImGui::Text("Stick Up:Add Number");
		ImGui::Text("Stick Down:Subtract a number");
		ImGui::Text("Bbutton:TitleScene");
		ImGui::Text("Abutton:Done");
		ImGui::End();
	}

	stageSelect.ImGuiDraw();
}

void StageSelectScene::OnFinalize()
{
}
