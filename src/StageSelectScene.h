#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"

class StageSelectScene : public BaseScene
{
	//�r�b�T�~��o�p
	bool isSkip = false;

	//�L�����N�^�[�I�𒆃t���O
	bool charactersSelect;


	/*-- �}���`�A�V���O����I�������ʂŎg�p����ϐ� --*/

	bool isSelectMultiSingle;		// �}���`�V���O���I����ʂ��t���O
	bool isMulti;					// �}���`�v���C��I��
	bool isSingle;					// �V���O���v���C��I��
	float easingTimer;				// �C�[�W���O�Ɏg�p����^�C�}�[
	Vec2<float> multiTexturePos;	// �}���`�v���C�̉摜�̍���̍��W
	Vec2<float> multiTextureInterimPos;	// �}���`�v���C�̉摜�̃C�[�W���O�̌o�R�n�_
	Vec2<float> multiTextureEndPos;		// �}���`�v���C�̉摜�̃C�[�W���O�̓��B�n�_
	Vec2<float> singleTexturePos;		// �V���O���v���C�̉摜�̍���̍��W
	Vec2<float> singleTextureInterimPos;// �V���O���v���C�̉摜�̃C�[�W���O�̌o�R�n�_
	Vec2<float> singleTextureEndPos;	// �V���O���v���C�̉摜�̃C�[�W���O�̓��B�n�_

	bool isInvisibleArrow;			// ����s���ɂ��邩�ǂ����t���O
	int invisibleArrowTimer;		// �����X�����ɂ���^�C�}�[



	// �摜�n���h��
	int multiHandle;	// �}���`�̉摜�n���h��
	int singleHandle;	// �V���O���̉摜�n���h��
	int hitoriHandle;	// �E���̈�l�v���C�̉摜�n���h��
	int futariHandle;	// �E���̓�l�v���C�̉摜�n���h��
	int arrowHandle;	// ���̉摜�n���h��


public:


	/*-- �}���`�A�V���O����I�������ʂŎg�p����萔 --*/

	const Vec2<float> SINGLE_MULTI_TEX_SIZE = { 1055.0f,640.0f };			// �}���`�v���C�A�V���O���v���C�̐����摜�̃T�C�Y
	const Vec2<float> CURRENT_DEF_POS = { 0.0f,0.0f };						// �I�𒆂̉摜�̍���̈ʒu�B
	const Vec2<float> CURRENT_INTERIM_POS = SINGLE_MULTI_TEX_SIZE / 2.0f;	// �I�������摜�����ʂɏo�鎞�Ɍo�R������W�B
	const Vec2<float> BEHIND_INTERIM_POS = -SINGLE_MULTI_TEX_SIZE / 2.0f;	// �I������Ȃ������摜���w�ʂɖ߂鎞�Ɍo�R������W�B
	const Vec2<float> BEHIND_DEF_POS = SINGLE_MULTI_TEX_SIZE / 10.0f;		// �I������Ȃ������摜�̍���̍��W�B
	const Vec2<float> PLAY_STR_POS = { 330,500 };							// ���l�v���C����`�悷����W�B
	const float ADD_EASING_TIMER = 0.03f;	// �C�[�W���O����Z��
	const int INVISIBLE_ARROW_TIMER = 30;


public:
	StageSelectScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	// �}���`���V���O������I������X�V�����B
	void SelectMultiSingleUpdate();

	std::shared_ptr<SceneCange>changeScene;

	int stageNum;


private:

	// �}���`�A�V���O���I����ʂł̃C���X�g�𓮂�������
	void UpdateEasingMultiSingle(const float& EasingRate, Vec2<float>& SinglePos, Vec2<float>& MultiPos,
		const Vec2<float>& SingleDefPos, const Vec2<float>& SingleIntermPos, const Vec2<float>& SingleEndPos,
		const Vec2<float>& MultiDefPos, const Vec2<float>& MultiIntermPos, const Vec2<float>& MultiEndPos);

};

