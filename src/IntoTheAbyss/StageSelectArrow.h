#include"../KuroEngine.h"
#include"IStageSelectImage.h"

// �X�e�[�W�I����ʂ̖��
class StageSelectArrow {

private:

	/*===== �����o�ϐ� =====*/

	LerpData pos;		// ���W�Ȃ�
	LerpData expData;	// ���o�p
	Vec2<float> defPos;	// �f�t�H���g�̍��W
	float angle;		// ��]������p�x
	int arrowHandle;	// ��̉摜�n���h��
	bool isZoomOut;		// �Y�[�����Ă��邩�̃t���O

	float timer;		// ���T�[�W���Ȑ��Ɏg�p����^�C�}�[


public:

	/*===== �����o�ϐ� =====*/

	StageSelectArrow();
	void Init(const Vec2<float>& InitPos, const Vec2<float>& Pos, const float& Angle);
	void Update(const bool& isLeft);
	void Draw();

	void SetZoomOut(const bool& Zoom);
	void SetDefPos();
	void SetExitPos(const Vec2<float>& ExitPos, const Vec2<float>& ExitSize);
	void SetExpSize(const Vec2<float>& Size);

};