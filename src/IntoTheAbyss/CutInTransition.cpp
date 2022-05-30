#include "CutInTransition.h"
#include "TexHandleMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"
#include "DrawFunc_Mask.h"
#include "WinApp.h"

CutInTransition::CutInTransition()
{

	/*===== �R���X�g���N�^ =====*/

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/luna.png", 3, Vec2<int>(3, 1), lunaHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/lacy.png", 3, Vec2<int>(3, 1), lacyHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask.png", 5, Vec2<int>(5, 1), maskHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask_frame.png", 5, Vec2<int>(5, 1), maskFrameHandle.data());
	maskAnimHandle = 0;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;

}

CutInTransition::~CutInTransition()
{
}

void CutInTransition::OnStart()
{

	/*===== �J�n���� =====*/

	backAlpha = 0;
	maskAnimHandle = 0;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isLeftChara = KuroFunc::GetRand(0, 1);
	maskPos = { 10000,10000 };
	charaPos = { 10000,10000 };

}

bool CutInTransition::OnUpdate()
{

	/*===== �X�V���� =====*/

	if (isEnd) return false;

	//�m�b�N�A�E�g�̕����A�j���[�V����
	const int START_PHASE_TIME = 60;			// �J�n�t�F�[�Y�̎���
	const int STOP_PHASE_TIMER = 50;			// �ꎞ��~�t�F�[�Y�̎���
	const int END_PHASE_TIME = 50;				// �I���t�F�[�Y�̎���
	const int CHARA_ANIM_FRAME = 5;				// �L������1�A�j���[�V�����ɂ����鎞��
	const Vec2<float> KNOCK_OUT_APPEAR_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y + 200.0f) };
	const Vec2<float> MASK_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y) };

	const Vec2<float> WindowSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y));
	const Vec2<float> WindowHalfSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

	// ���҂ɂ���ă}�X�N��L�����A�C�R���̈ړ���ς��邽�߂̕ϐ��B
	Vec2<float> maskStartPos = { -WindowSize.x,-WindowSize.y };
	Vec2<float> maskEndPos = { WindowSize.x + WindowHalfSize.x, WindowSize.y + WindowHalfSize.y };
	Vec2<float> charaStartPos = maskEndPos;
	Vec2<float> charaEndPos = maskStartPos;

	// �E�̃L������������l�𔽓]������B
	if (!isLeftChara) {

		maskStartPos = { WindowSize.x + WindowSize.x, -WindowSize.y };
		maskEndPos = { -WindowHalfSize.x, WindowSize.y + WindowHalfSize.y };
		charaStartPos = maskEndPos;
		charaEndPos = maskStartPos;

	}

	// ���݂̃t�F�[�Y�ɂ���ăm�b�N�A�E�g�̉��o�̍X�V�������s���B
	switch (knockOutPhase)
	{
	case KNOCK_OUT_PHASE::START_PHASE:

		// �J�n�t�F�[�Y

		// [KnockOut!!!!]�ɂ�����}�X�N�̍��W�𓮂����B
		maskPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, maskStartPos, MASK_POS);

		// �}�X�N�̓����̃L�����N�^�[�𓮂����B
		charaPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, charaStartPos, MASK_POS);

		// �^�C�}�[��10�̔{���ɂȂ�����A�j���[�V�����n���h�������Z����B
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && maskAnimHandle < maskHandle.size() - 1)++maskAnimHandle;

		// �L�����N�^�[���A�j���[�V����������B
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// �^�C�}�[���K��l�ɒB������A���̃t�F�[�Y�ֈڍs����B
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::STOP_PHASE;

		}

		// ���ɕ`�悷�鍕�̃A���t�@�l��255�ɋ߂Â���B
		backAlpha += (255 - backAlpha) / 10.0f;

		break;
	case KNOCK_OUT_PHASE::STOP_PHASE:

		// �ꎞ��~�t�F�[�Y

		// �L�����N�^�[���A�j���[�V����������B
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// �^�C�}�[���K��l�ɒB������A���̃t�F�[�Y�ֈڍs����B
		if (STOP_PHASE_TIMER <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::END_PHASE;

		}

		break;
	case KNOCK_OUT_PHASE::END_PHASE:

		// �ŏI�t�F�[�Y

		// �L�����N�^�[���A�j���[�V����������B
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// �^�C�}�[��10�̔{���ɂȂ�����A�j���[�V�����n���h�������Z����B
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && 0 < maskAnimHandle)--maskAnimHandle;

		// [KnockOut!!!!]�ɂ�����}�X�N�̍��W�𓮂����B
		maskPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, maskEndPos);

		// �}�X�N�̓����̃L�����N�^�[�𓮂����B
		charaPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, charaEndPos);


		// �^�C�}�[���K��l�ɒB������A���̃t�F�[�Y�ֈڍs����B
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
			// �I���
			isEnd = true;

		}


		break;
	default:
		break;
	}

	knockOutTimer += 2;

	return isEnd;
}

void CutInTransition::OnDraw()
{

	/*===== �`�揈�� =====*/

	if (isEnd) return;

	static const float KNOCK_OUT_SCALE = 0.8f;

	static const int BLACK_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/black.png");

	// �w�i����������B
	DrawFunc::DrawExtendGraph2D(Vec2<float>(0, 0), WinApp::Instance()->GetExpandWinSize(), TexHandleMgr::GetTexBuffer(BLACK_HANDLE), Color(0, 0, 0, backAlpha));

	// �}�X�N�̓����̃L�����N�^�[��`��B
	if (isLeftChara) {
		// �}�X�N�̘g��`��B
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]));
		//DrawFunc_Mask::DrawGraphByMaskGraph(charaPos, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]), maskPos, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]), Vec2<float>(1.0f, 1.0f), Vec2<bool>(false, false));
	}
	else {
		// �}�X�N�̘g��`��B
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), Color(), Vec2<float>(0.5f, 0.5f), Vec2<bool>(true, false));
		//DrawFunc_Mask::DrawGraphByMaskGraph(charaPos, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), maskPos, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]), Vec2<float>(1.0f, 1.0f), Vec2<bool>(true, false));
	}


}
