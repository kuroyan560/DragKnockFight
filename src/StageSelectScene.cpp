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
	hitoriHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/singlePlay_str.png");
	futariHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/multiPlay_str.png");
	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/arrow.png");
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
	invisibleArrowTimer = 0;
	isInvisibleArrow = false;
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

		// �}���`�v���C��I�𒆂�������
		bool isFirstHalf = easingTimer < 0.5f && 0.0f < easingTimer;
		// �O��������������
		if (isFirstHalf) {

			// �}���`��������
			if (isMulti) {

				// �}���`�̉摜��\��
				DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
				// �V���O���̉摜��\��
				DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

			}
			else {

				// �V���O���̉摜��\��
				DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
				// �}���`�̉摜��\��
				DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

			}

		}
		else if (isMulti) {

			// �V���O���̉摜��\��
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
			// �}���`�̉摜��\��
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

		}
		// �V���O���v���C��I�𒆂�������
		else if (isSingle) {

			// �}���`�̉摜��\��
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
			// �V���O���̉摜��\��
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

		}

		// ���l�v���C����`��
		if (isMulti) {

			// ��l�v���C��`��
			DrawFunc::DrawGraph(PLAY_STR_POS, TexHandleMgr::GetTexBuffer(futariHandle));

		}
		else {

			// ��l�v���C��`��
			DrawFunc::DrawGraph(PLAY_STR_POS, TexHandleMgr::GetTexBuffer(hitoriHandle));

		}

		// ���`��B
		if (!isInvisibleArrow) {

			const Vec2<float> LEFT_ARROW_POS = { 220,620 };
			const Vec2<float> RIGHT_ARROW_POS = { 1200, 620 };

			DrawFunc::DrawRotaGraph2D(LEFT_ARROW_POS, { 1,1 }, 3.14f, TexHandleMgr::GetTexBuffer(arrowHandle));
			DrawFunc::DrawRotaGraph2D(RIGHT_ARROW_POS, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(arrowHandle));

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

			if (0 < inputLeftVec.x) {

				// �E���ɓ��͂�����B
				multiTextureInterimPos = BEHIND_INTERIM_POS;
				multiTextureEndPos = CURRENT_DEF_POS;
				singleTextureInterimPos = CURRENT_INTERIM_POS;
				singleTextureEndPos = BEHIND_DEF_POS;

			}
			else {

				// �����ɓ��͂�����B
				multiTextureInterimPos = CURRENT_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = BEHIND_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;


			}

		}
		// ���̃t���O���}���`�v���C�`�撆��������B
		else {

			// �V���O���v���C�ɂ���B
			isMulti = false;
			isSingle = true;

			if (0 < inputLeftVec.x) {

				// �E���ɓ��͂�����B
				multiTextureInterimPos = CURRENT_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = BEHIND_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;

			}
			else {

				// �����ɓ��͂�����B
				multiTextureInterimPos = BEHIND_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = CURRENT_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;


			}

		}

		// �C�[�W���O�^�C�}�[��i�߂�B�������邱�ƂŎ����I�ɃC�[�W���O���n�܂�B
		easingTimer += ADD_EASING_TIMER;

		// ������U�����B
		isInvisibleArrow = true;
		invisibleArrowTimer = 0;

	}

	// �C�[�W���O�^�C�}�[���N�����Ă�����C�[�W���O��i�߂�B
	if (easingTimer != 0) {

		// �摜���X�V�B
		UpdateEasingMultiSingle(easingTimer, singleTexturePos, multiTexturePos,
			CURRENT_DEF_POS, singleTextureInterimPos, singleTextureEndPos,
			BEHIND_DEF_POS, multiTextureInterimPos, multiTextureEndPos);

	}

	// �C�[�W���O�^�C�}�[���X�V�B
	easingTimer += ADD_EASING_TIMER;
	if (1.0f <= easingTimer) {

		easingTimer = 0;

	}

	// ��s���^�C�}�[���X�V�B
	++invisibleArrowTimer;
	if (INVISIBLE_ARROW_TIMER <= invisibleArrowTimer) {

		isInvisibleArrow = isInvisibleArrow ? false : true;
		invisibleArrowTimer = 0;

	}

}

void StageSelectScene::UpdateEasingMultiSingle(const float& EasingRate, Vec2<float>& SinglePos, Vec2<float>& MultiPos, const Vec2<float>& SingleDefPos, const Vec2<float>& SingleIntermPos, const Vec2<float>& SingleEndPos, const Vec2<float>& MultiDefPos, const Vec2<float>& MultiIntermPos, const Vec2<float>& MultiEndPos)
{

	// �C�[�W���O�̃^�C�}�[�̒l�ɂ���č����O�����㔼�����擾����B
	bool isFirstHalf = EasingRate < 0.5f;

	// �O����������B
	if (isFirstHalf) {

		float easingRate = easingTimer / 0.5f;

		float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

		// �V���O���v���C��w�ʂ��琳�ʂɎ����Ă���B
		SinglePos = SingleDefPos + (SingleIntermPos - SingleDefPos) * easingAmount;

		// �}���`�v���C��S�ʂ���w�ʂɎ����Ă���B
		MultiPos = MultiDefPos + (MultiIntermPos - MultiDefPos) * easingAmount;

	}
	// �㔼��������B
	else {

		float easingRate = (easingTimer - 0.5f) / 0.5f;

		float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

		// �V���O���v���C��w�ʂ��琳�ʂɎ����Ă���B
		SinglePos = SingleIntermPos + (SingleEndPos - SingleIntermPos) * easingAmount;

		// �}���`�v���C��S�ʂ���w�ʂɎ����Ă���B
		MultiPos = MultiIntermPos + (MultiEndPos - MultiIntermPos) * easingAmount;

	}

}
