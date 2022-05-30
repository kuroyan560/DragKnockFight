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
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isNowTransition = false;

}

CutInTransition::~CutInTransition()
{
}

void CutInTransition::OnStart()
{

	/*===== �J�n���� =====*/

	backAlpha = 0;
	isNowTransition = true;
	lunaAnimHandle = 0;
	knockOutTimer = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
	isEnd = false;
	isLeftChara = KuroFunc::GetRand(0, 1);
	charaPos = { 10000,10000 };
	titlePos = { 10000,10000 };

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
	const Vec2<float> MASK_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y) };

	const Vec2<float> WindowSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y));
	const Vec2<float> WindowHalfSize = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

	// �^�C�g�����S�̃I�t�Z�b�g�|�X
	Vec2<float> titleOffset = Vec2<float>(0, 170);

	// ���҂ɂ���ă}�X�N��L�����A�C�R���̈ړ���ς��邽�߂̕ϐ��B
	Vec2<float> charaStartPos = { WindowSize.x + WindowHalfSize.x, WindowHalfSize.y };
	Vec2<float> charaEndPos = { -WindowSize.x,WindowHalfSize.y };
	Vec2<float> titleStartPos = charaEndPos + titleOffset;
	Vec2<float> titleEndPos = charaStartPos + titleOffset;

	// �E�̃L������������l�𔽓]������B
	if (!isLeftChara) {

		Vec2<float> CharaStartPosBuff = charaStartPos;
		charaStartPos = charaEndPos;
		charaEndPos = CharaStartPosBuff;
		titleStartPos = charaEndPos + titleOffset;
		titleEndPos = charaStartPos + titleOffset;

	}

	// ���݂̃t�F�[�Y�ɂ���ăm�b�N�A�E�g�̉��o�̍X�V�������s���B
	switch (knockOutPhase)
	{
	case KNOCK_OUT_PHASE::START_PHASE:

		// �J�n�t�F�[�Y

		// �}�X�N�̓����̃L�����N�^�[�𓮂����B
		charaPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, charaStartPos, MASK_POS);
		titlePos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, titleStartPos, MASK_POS + titleOffset);

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

		// �}�X�N�̓����̃L�����N�^�[�𓮂����B
		charaPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, charaEndPos);
		titlePos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS + titleOffset, titleEndPos);


		// �^�C�}�[���K��l�ɒB������A���̃t�F�[�Y�ֈڍs����B
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
			// �I���
			isEnd = true;
			isNowTransition = false;

		}


		break;
	default:
		break;
	}

	// ���ɕ`�悷�鍕�̃A���t�@�l��255�ɋ߂Â���B
	backAlpha += (255 - backAlpha) / 10.0f;
	if (240 < backAlpha) {
		backAlpha = 255;
	}

	knockOutTimer += 2;

	return isEnd;
}

void CutInTransition::OnDraw()
{

	/*===== �`�揈�� =====*/

	if (isEnd) return;

	static const float KNOCK_OUT_SCALE = 0.8f;

	static const int TITLE_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/title.png");

	// �w�i����������B
	DrawFunc::DrawExtendGraph2D(Vec2<float>(0, 0), WinApp::Instance()->GetExpandWinSize(), D3D12App::Instance()->GenerateTextureBuffer(Color(56, 22, 74, backAlpha)));

	// �}�X�N�̓����̃L�����N�^�[��`��B
	if (isLeftChara) {
		// �}�X�N�̘g��`��B
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]));
		DrawFunc::DrawRotaGraph2D(titlePos, Vec2<float>(0.8f, 0.8f), 0.0f, TexHandleMgr::GetTexBuffer(TITLE_GRAPH));
	}
	else {
		// �}�X�N�̘g��`��B
		DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lacyHandle[lunaAnimHandle]), Color(), Vec2<float>(0.5f, 0.5f), Vec2<bool>(true, false));
		DrawFunc::DrawRotaGraph2D(titlePos, Vec2<float>(0.8f, 0.8f), 0.0f, TexHandleMgr::GetTexBuffer(TITLE_GRAPH));
	}


}
