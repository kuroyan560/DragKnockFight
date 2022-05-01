#include "Vec.h"

// �X�^�~�i�N���X
class Stamina {

private:
	/*===== �����o�ϐ� =====*/

	float nowGauge;		// ���݂̃Q�[�W�� 0 ~ 100 �̒l
	bool isActivate;	// �Q�[�W�ʂ��ő�l�ɒB���Ă��āA�U��񂵂�_�b�V�����s�����Ԃ��̃t���O


private:

	/*===== �萔 =====*/

	const float MAX_GAUGE = 100.0f;		// �Q�[�W�ʂ̍ő�l�B


public:

	/*===== �����o�ϐ� =====*/

	Stamina();
	void Init();
	void Empty();
	void Update();
	void Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height);

	// �e��A�N�Z�b�^
	inline const bool& GetIsActivate() { return isActivate; }
	inline const float& GetNowGauge() { return nowGauge; }
	float AddNowGauge(const float& Add);

};