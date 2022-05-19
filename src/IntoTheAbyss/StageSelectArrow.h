#include"../KuroEngine.h"
#include"IStageSelectImage.h"

// �X�e�[�W�I����ʂ̖��
class StageSelectArrow {

private:

	/*===== �����o�ϐ� =====*/

	LerpData pos;		// ���W�Ȃ�
	LerpData expData;	// ���o�p
	float angle;		// ��]������p�x
	int arrowHandle;	// ��̉摜�n���h��
	bool isZoom;		// �Y�[�����Ă��邩�̃t���O


public:

	/*===== �����o�ϐ� =====*/

	StageSelectArrow();
	void Init(const Vec2<float>& Pos, const float& Angle);
	void Update();
	void Draw();

	inline void SetZoom(const bool& Zoom) { isZoom = Zoom; }

};