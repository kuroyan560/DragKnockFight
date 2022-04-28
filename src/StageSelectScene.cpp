#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"
#include"Vec.h"
#include"KuroMath.h"
#include"DrawFunc_Color.h"
#include"DrawFunc.h"

StageSelectScene::StageSelectScene()
{
	changeScene = std::make_shared<SceneCange>();
	stageNum = 0;

	multiHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/multiPlay.png");
	singleHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/singlePlay.png");
}

void StageSelectScene::OnInitialize()
{
	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	isSelectMultiSingle = true;
	isMulti = false;
	isSingle = true;	// �}���`�A�V���O���I����ʂɂ����čŏ��ɃV���O���v���C�̉摜��\�����邽�߂�true�ɂ���B
	easingTimer = 0;
	singleTexturePos = CURRENT_DEF_POS;
	multiTexturePos = BEHIND_DEF_POS;
}

void StageSelectScene::OnUpdate()
{
	if (isSkip)
	{
		KuroEngine::Instance().ChangeScene(2, changeScene);
		SelectStage::Instance()->resetStageFlag = true;
	}


	// �}���`�A�V���O���I����ʂ�\������B
	if (isSelectMultiSingle) {

		// �}���`�A�V���O���̑I����ʂł̍X�V�������s���B
		SelectMultiSingleUpdate();

	}
	// �}���`�A�V���O���I������ʏI���B�t���O�ɂ���ď�����ς���B
	else {



	}



	//if (charactersSelect)
	//{
	//	//�L�����N�^�[�I���X�V
	//	CharacterManager::Instance()->CharactersSelectUpdate();

	//	//�Q�[���V�[���Ɉړ�����
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
	//	{
	//		KuroEngine::Instance().ChangeScene(2, changeScene);
	//		SelectStage::Instance()->resetStageFlag = true;
	//	}
	//	//�X�e�[�W�I���֖߂�
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
	//	{
	//		charactersSelect = false;
	//	}
	//}
	//else
	//{
	//	//�L�����N�^�[�I����
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
	//	{
	//		charactersSelect = true;
	//	}
	//	//�^�C�g���V�[���Ɉړ�����
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
	//	{
	//		KuroEngine::Instance().ChangeScene(0, changeScene);
	//	}

	//	//�X�e�[�W�ԍ��𑝂₷
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_UP))
	//	{
	//		++stageNum;
	//	}
	//	//�X�e�[�W�ԍ������炷
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_DOWN))
	//	{
	//		--stageNum;
	//	}


	//	if (StageMgr::Instance()->GetMaxStageNumber() - 1 <= stageNum)
	//	{
	//		stageNum = StageMgr::Instance()->GetMaxStageNumber() - 1;
	//	}
	//	if (stageNum <= 0)
	//	{
	//		stageNum = 0;
	//	}
	//	SelectStage::Instance()->SelectStageNum(stageNum);
	//}
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	// �}���`�V���O���I����ʂ�������
	if (isSelectMultiSingle) {

		const Vec2<float> PLAY_STR_POS = { 1700,700 };							// ���l�v���C����`�悷����W�B

			// �}���`�v���C��I�𒆂�������
		if (isMulti) {

			// �V���O���̉摜��\��
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
			// �}���`�̉摜��\��
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

		}
		else if (isSingle) {

			// �}���`�̉摜��\��
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
			// �V���O���̉摜��\��
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

		}



	}
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
}

void StageSelectScene::OnFinalize()
{
}

void StageSelectScene::SelectMultiSingleUpdate()
{

	/*===== �}���`�A�V���O���I����ʂł̍X�V���� =====*/


	// ���X�e�B�b�N�̓��͂�ۑ�����B
	Vec2<float> inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(0);	// 1P����̓��͂��󂯕t����B
	inputLeftVec /= {32768.0f, 32768.0f};

	// �ǂ��炩�ɓ��͂��ꂽ��B
	bool isEndEasing = easingTimer == 0;	// �C�[�W���O���I����Ă��邩�B
	if (0.5f < fabs(inputLeftVec.x) && isEndEasing) {

		// ���̃t���O����l�v���C�`�撆��������B
		if (isSingle) {

			// �}���`�v���C�ɂ���B
			isMulti = true;
			isSingle = false;

		}
		// ���̃t���O���}���`�v���C�`�撆��������B
		else {

			// �V���O���v���C�ɂ���B
			isMulti = false;
			isSingle = true;

		}

		// �C�[�W���O�^�C�}�[��i�߂�B�������邱�ƂŎ����I�ɃC�[�W���O���n�܂�B
		easingTimer += ADD_EASING_TIMER;

	}

	// �C�[�W���O�^�C�}�[���N�����Ă�����C�[�W���O��i�߂�B
	if (easingTimer != 0) {

		// �I�𒆂������̂��V���O���v���C��������B
		if (isSingle) {

			// �C�[�W���O�̃^�C�}�[�̒l�ɂ���č����O�����㔼�����擾����B
			bool isFirstHalf = easingTimer < 0.5f;

			// �O����������B
			if (isFirstHalf) {

				float easingRate = easingTimer / 0.5f;

				float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

				// �V���O���v���C��w�ʂ��琳�ʂɎ����Ă���B
				singleTexturePos = BEHIND_DEF_POS + (CURRENT_INTERIM_POS - BEHIND_DEF_POS) * easingAmount;

				// �}���`�v���C��S�ʂ���w�ʂɎ����Ă���B
				multiTexturePos = CURRENT_DEF_POS + (BEHIND_INTERIM_POS - CURRENT_DEF_POS) * easingAmount;

			}
			// �㔼��������B
			else {

				float easingRate = (easingTimer - 0.5f) / 0.5f;

				float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

				// �V���O���v���C��w�ʂ��琳�ʂɎ����Ă���B
				singleTexturePos = CURRENT_INTERIM_POS + (CURRENT_DEF_POS - CURRENT_INTERIM_POS) * easingAmount;

				// �}���`�v���C��S�ʂ���w�ʂɎ����Ă���B
				multiTexturePos = BEHIND_INTERIM_POS + (BEHIND_DEF_POS - BEHIND_INTERIM_POS) * easingAmount;

			}

		}
		// �I�𒆂������̂��}���`�v���C��������B
		else {

			// �C�[�W���O�̃^�C�}�[�̒l�ɂ���č����O�����㔼�����擾����B
			bool isFirstHalf = easingTimer < 0.5f;

			// �O����������B
			if (isFirstHalf) {

				float easingRate = easingTimer / 0.5f;

				float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

				// �V���O���v���C��O�ʂ���w�ʂɎ����Ă����B
				singleTexturePos = CURRENT_DEF_POS + (BEHIND_INTERIM_POS - CURRENT_DEF_POS) * easingAmount;

				// �}���`�v���C��w�ʂ���O�ʂɎ����Ă����B
				multiTexturePos = BEHIND_DEF_POS + (CURRENT_INTERIM_POS - BEHIND_DEF_POS) * easingAmount;

			}
			// �㔼��������B
			else {

				float easingRate = (easingTimer - 0.5f) / 0.5f;

				float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

				// �V���O���v���C��O�ʂ���w�ʂɎ����Ă����B
				singleTexturePos = BEHIND_INTERIM_POS + (BEHIND_DEF_POS - BEHIND_INTERIM_POS) * easingAmount;

				// �}���`�v���C��S�ʂ���w�ʂɎ����Ă���B
				multiTexturePos = CURRENT_INTERIM_POS + (CURRENT_DEF_POS - CURRENT_INTERIM_POS) * easingAmount;

			}

		}

		// �C�[�W���O�^�C�}�[���X�V�B
		easingTimer += ADD_EASING_TIMER;
		if (1.0f <= easingTimer) {

			easingTimer = 0;

		}

	}

}
