#include "Vec.h"
#include <memory>
#include <vector>

// �X�^�~�i�N���X
class Stamina {

private:
	/*===== �����o�ϐ� =====*/

	float nowGauge;			// ���݂̃Q�[�W�� 0 ~ 100 �̒l
	bool isActivate;		// �Q�[�W�ʂ��ő�l�ɒB���Ă��āA�U��񂵂�_�b�V�����s�����Ԃ��̃t���O
	Vec2<float> expAmount;	// �񕜂����ۂɂ�����Ɗg�傷�鉉�o�Ɏg�p����ϐ��B


private:

	/*===== �萔 =====*/

	const float MAX_GAUGE = 100.0f;		// �Q�[�W�ʂ̍ő�l�B
	const float EXP_AMOUNT = 3.0f;		// �g��ʁB
	const float BIG_EXP_AMOUNT = 6.0f;	// �Q�[�W�����^���ɂȂ����Ƃ��̊g��ʁB


public:

	/*===== �����o�ϐ� =====*/

	Stamina();
	void Init();	// ������ �X�^�~�i�Q�[�W�𖄂߂�B
	void Empty();	// ������ �X�^�~�i�Q�[�W����ɂ���B
	void Update();
	void Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height);

	// �e��A�N�Z�b�^
	inline const bool& GetIsActivate() { return isActivate; }
	inline const float& GetNowGauge() { return nowGauge; }
	float AddNowGauge(const float& Add);
	void SetExp(const bool& isBig = false);

};

// �X�^�~�i�Ǘ��N���X
class StaminaMgr {

public:

	/*===== �����o�ϐ� =====*/

	std::vector<Stamina> stamina;


public:

	/*===== �����o�֐� =====*/

	StaminaMgr();
	void Init();
	void Update();
	void Draw(const Vec2<float>& CharaPos);

	// �X�^�~�i������
	void ConsumesStamina(const int& ConsumesStamina);

	// �X�^�~�i�����Z
	void AddStamina(const int& AddStamina);

	// �w�肵���A�N�V�������ł��邩�B
	bool CheckCanAction(const int& ConsumesStamina);

};