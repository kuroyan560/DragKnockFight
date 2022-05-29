#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectScene::StageSelectScene() : screenShot(&stageNum)
{
	changeScene = std::make_shared<MaskSceneTransition>();
	stageNum = 0;
}

void StageSelectScene::OnInitialize()
{
	zoomValue[0][STAGE_SELECT] = 0.2f;
	zoomValue[0][SCENE_CHANGE] = 0.2f;

	zoomValue[1][STAGE_SELECT] = 0.3f;
	zoomValue[1][SCENE_CHANGE] = 0.4f;

	zoomValue[2][STAGE_SELECT] = 0.3f;
	zoomValue[2][SCENE_CHANGE] = 0.4f;

	zoomValue[3][STAGE_SELECT] = 0.3f;
	zoomValue[3][SCENE_CHANGE] = 0.4f;

	zoomValue[4][STAGE_SELECT] = 0.3f;
	zoomValue[4][SCENE_CHANGE] = 0.4f;

	zoomValue[5][STAGE_SELECT] = 0.3f;
	zoomValue[5][SCENE_CHANGE] = 0.4f;

	zoomValue[6][STAGE_SELECT] = 0.3f;
	zoomValue[6][SCENE_CHANGE] = 0.4f;

	zoomValue[7][STAGE_SELECT] = 0.3f;
	zoomValue[7][SCENE_CHANGE] = 0.4f;

	zoomValue[8][STAGE_SELECT] = 0.3f;
	zoomValue[8][SCENE_CHANGE] = 0.4f;

	zoomValue[9][STAGE_SELECT] = 0.3f;
	zoomValue[9][SCENE_CHANGE] = 0.4f;

	zoomValue[10][STAGE_SELECT] = 0.3f;
	zoomValue[10][SCENE_CHANGE] = 0.4f;

	zoomValue[11][STAGE_SELECT] = 0.3f;
	zoomValue[11][SCENE_CHANGE] = 0.4f;

	zoomValue[12][STAGE_SELECT] = 0.3f;
	zoomValue[12][SCENE_CHANGE] = 0.4f;

	zoomValue[13][STAGE_SELECT] = 0.3f;
	zoomValue[13][SCENE_CHANGE] = 0.4f;

	zoomValue[14][STAGE_SELECT] = 0.3f;
	zoomValue[14][SCENE_CHANGE] = 0.4f;




	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	// �X�e�[�W�Z���N�g��ʂ̍X�V�����p�N���X���������B
	stageSelect.Init();
	// �}�b�v�̃X�N�V�����������B
	screenShot.Init();
	// �e����������
	rightArrow.Init(Vec2<float>(1180.0f, static_cast<float>(WinApp::Instance()->GetWinCenter().y) + 5.0f), 0);
	leftArrow.Init(Vec2<float>(100.0f, static_cast<float>(WinApp::Instance()->GetWinCenter().y) + 5.0f), DirectX::XM_PI);
	// �e�L�����̉摜���������B
	Vec2<float> leftCharaPos = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x * 0.25f + 55.0f), static_cast<float>(WinApp::Instance()->GetWinCenter().y - 7.0f));
	leftChara.Init(Vec2<float>(-550.0f, 881.0f), leftCharaPos, TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/character_card/luna.png"));
	Vec2<float> rightCharaPos = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x * 1.75f - 55.0f), static_cast<float>(WinApp::Instance()->GetWinCenter().y - 7.0f));
	rightChara.Init(Vec2<float>(1830.0f, 881.0f), rightCharaPos, TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/character_card/lacy.png"));

	isPrevInputStickRight = false;
	isPrevInputSticlLeft = false;

	prevStageNum = -1;

	for (int i = 0; i < mapScreenShot.size(); ++i)
	{
		//�X�e�[�W�I����ʗp
		mapScreenShot[i][STAGE_SELECT].Init(i, false, zoomValue[i][STAGE_SELECT]);
		//�V�[���J�ڗp
		mapScreenShot[i][SCENE_CHANGE].Init(i, true, zoomValue[i][SCENE_CHANGE]);
	}
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
		//���Z�f�o�b�O�p�@�L�����I���΂�
		{
			KuroEngine::Instance().ChangeScene(2, changeScene);
			SelectStage::Instance()->resetStageFlag = true;
		}

		//�L�����N�^�[�I���X�V
		CharacterManager::Instance()->CharactersSelectUpdate();

		//�Q�[���V�[���Ɉړ�����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A) && 1.0f <= stageSelect.GetLerpData().timer)
		{
			KuroEngine::Instance().ChangeScene(2, changeScene);
			SelectStage::Instance()->resetStageFlag = true;
		}
		//�X�e�[�W�I���֖߂�
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B) && 1.0f <= stageSelect.GetLerpData().timer)
		{
			charactersSelect = false;
			screenShot.SetZoomFlag(false);
			stageSelect.SetZoomFlag(false);

			// �����Y�[���C��������B
			rightArrow.SetZoomOut(false);
			leftArrow.SetZoomOut(false);

			// �����f�t�H���g�̏ꏊ�Ɉړ�������B
			rightArrow.SetDefPos();
			leftArrow.SetDefPos();

			// �L�����̃J�[�h���Y�[���C��������B
			leftChara.SetIsZoomOut(false);
			rightChara.SetIsZoomOut(false);

			// ��ʂ̃Y�[���A�E�g�̔�����X�N�V���̃Y�[���A�E�g�̔���ɂ��K��������B
			screenShot.SetZoomFlag(stageSelect.GetZoomOutFlag());
		}
	}
	else
	{
		//�L�����N�^�[�I����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A) && 1.0f <= stageSelect.GetLerpData().timer)
		{
			charactersSelect = true;
			//screenShot.SetZoomFlag(true);		// �Y�[���A�E�g������
			//stageSelect.SetZoomFlag(true);		// �Y�[���A�E�g������

			// �����Y�[���A�E�g������B
			//rightArrow.SetZoomOut(true);
			//leftArrow.SetZoomOut(true);

			// �����f�t�H���g�̏ꏊ�Ɉړ�������B
			//rightArrow.SetExitPos(Vec2<float>(2000, static_cast<float>(WinApp::Instance()->GetWinCenter().y)), Vec2<float>(0.0f, 0.0f));
			//leftArrow.SetExitPos(Vec2<float>(-1000, static_cast<float>(WinApp::Instance()->GetWinCenter().y)), Vec2<float>(0.0f, 0.0f));

			// �L�����̃J�[�h���Y�[���A�E�g������B
			//leftChara.SetIsZoomOut(true);
			//rightChara.SetIsZoomOut(true);

			// ��ʂ̃Y�[���A�E�g�̔�����X�N�V���̃Y�[���A�E�g�̔���ɂ��K��������B
			//screenShot.SetZoomFlag(stageSelect.GetZoomOutFlag());


			mapScreenShot[stageNum][SCENE_CHANGE].Init(stageNum, true, zoomValue[stageNum][SCENE_CHANGE]);

			mapScreenShot[stageNum][STAGE_SELECT].Finalize();
			mapScreenShot[stageNum][SCENE_CHANGE].Finalize();


		}
		//�^�C�g���V�[���Ɉړ�����
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
		{
			KuroEngine::Instance().ChangeScene(0, changeScene);
		}

		// ���͂̍X�V����
		Vec2<float> leftStickInput = UsersInput::Instance()->GetLeftStickVecFuna(0);
		leftStickInput /= {32768.0f, 32768.0f};
		bool isInputRight = 0.9 <= leftStickInput.x;
		bool isInputLeft = leftStickInput.x <= -0.9f;

		//�X�e�[�W�ԍ��𑝂₷
		if (!isPrevInputStickRight && isInputRight)
		{
			++stageNum;
			stageSelect.SetExp(Vec2<float>(0, 40), Vec2<float>(0.2f, 0.2f));
			screenShot.SetExp(Vec2<float>(0, 0), Vec2<float>(0.1f, 0.1f));
			rightArrow.SetExpSize(Vec2<float>(0.5f, 0.5f));
			leftArrow.SetExpSize(Vec2<float>(-0.1f, -0.1f));
		}
		//�X�e�[�W�ԍ������炷
		if (!isPrevInputSticlLeft && isInputLeft)
		{
			--stageNum;
			stageSelect.SetExp(Vec2<float>(0, 40), Vec2<float>(0.2f, 0.2f));
			screenShot.SetExp(Vec2<float>(0, 0), Vec2<float>(0.1f, 0.12f));
			leftArrow.SetExpSize(Vec2<float>(0.5f, 0.5f));
			rightArrow.SetExpSize(Vec2<float>(-0.1f, -0.1f));
		}

		isPrevInputStickRight = isInputRight;
		isPrevInputSticlLeft = isInputLeft;

		int maxSelectNum = min(screenShot.GetCanMaxSelectNum(), StageMgr::Instance()->GetMaxStageNumber());
		if (maxSelectNum <= stageNum)
		{
			stageNum = 0;
		}
		if (stageNum <= -1)
		{
			stageNum = maxSelectNum - 1;
		}
		SelectStage::Instance()->SelectStageNum(stageNum);
	}

	//if (stageNum != prevStageNum)
	//{
	//mapScreenShot[stageNum][0].Init(stageNum, false);
	//}
	prevStageNum = stageNum;

	screenShot.Update();
	stageSelect.Update();
	rightArrow.Update(false);
	leftArrow.Update(true);

	mapScreenShot[stageNum][STAGE_SELECT].Update();
	mapScreenShot[stageNum][SCENE_CHANGE].Update();

	// �w�i�̃L�����J�[�h�̍X�V����
	leftChara.Update();
	rightChara.Update();

	float moveOffset = 15.0f;
	// �L�[���͂ɉ����āA�`����W�̃I�t�Z�b�g��؂�ւ���B ���̓����K�v�ɂȂ�C������̂Ŏc���Ă����܂��B
	/*if (UsersInput::Instance()->KeyInput(DIK_UP)) {

		StageSelectOffsetPosDebug::Instance()->pos.y += moveOffset;

	}
	if (UsersInput::Instance()->KeyInput(DIK_DOWN)) {

		StageSelectOffsetPosDebug::Instance()->pos.y -= moveOffset;

	}
	if (UsersInput::Instance()->KeyInput(DIK_LEFT)) {

		StageSelectOffsetPosDebug::Instance()->pos.x += moveOffset;

	}
	if (UsersInput::Instance()->KeyInput(DIK_RIGHT)) {

		StageSelectOffsetPosDebug::Instance()->pos.x -= moveOffset;

	}*/

	changeScene->backGroundTex = mapScreenShot[stageNum][SCENE_CHANGE].mapBuffer;
	screenShot.screenShot = mapScreenShot[stageNum][STAGE_SELECT].mapBuffer;
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	stageSelect.Draw();
	screenShot.Draw();
	rightArrow.Draw();
	leftArrow.Draw();
	// �Ђ���̃L�����̕`�揈��
	leftChara.Draw();
	rightChara.Draw();

	mapScreenShot[stageNum][STAGE_SELECT].Draw();
	mapScreenShot[stageNum][SCENE_CHANGE].Draw();

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
	//screenShot.ImGuiDraw();
}

void StageSelectScene::OnFinalize()
{
}
