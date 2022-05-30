#pragma once
#include "KuroEngine.h"
#include <array>

class CutInTransition : public SceneTransition {
public:
	CutInTransition();
	~CutInTransition();
	void OnStart()override;
	bool OnUpdate()override;
	void OnDraw()override;

	std::shared_ptr<RenderTarget>backGroundTex;
private:

	std::array<int, 3> lunaHandle;			// �m�b�N�A�E�g����Luna(�����̃L����)�̉摜�n���h�� �A�j���[�V����
	std::array<int, 3> lacyHandle;			// �m�b�N�A�E�g����Luna(�����̃L����)�̉摜�n���h�� �A�j���[�V����
	std::array<int, 5> maskHandle;			// �m�b�N�A�E�g���Ƀ}�X�N�Ƃ��Ďg�p����摜�̃n���h�� �A�j���[�V����
	std::array<int, 5> maskFrameHandle;		// �m�b�N�A�E�g���Ɏg�p����}�X�N�̃t���[�� �A�j���[�V����
	Vec2<float>maskPos;						// [KnockOut!!!!]�ɂ�����}�X�N�̍��W
	Vec2<float>charaPos;					// �}�X�N�̓����ɂ���L�����̍��W
	int maskAnimHandle;
	int lunaAnimHandle;
	int knockOutTimer;
	bool isEnd;
	bool isLeftChara;
	int backAlpha;		// ���ɕ`�悷�鍕�̃A���t�@�l

	// �m�b�N�A�E�g���̃X�e�[�^�X
	enum class KNOCK_OUT_PHASE {

		START_PHASE,		// �J�n�t�F�[�Y
		STOP_PHASE,			// �L�����J�b�g�C�����̈ꎞ��~�t�F�[�Y
		END_PHASE,			// �I���t�F�[�Y

	};
	KNOCK_OUT_PHASE knockOutPhase;

	Vec2<float>pos, backGroundPos;
};
