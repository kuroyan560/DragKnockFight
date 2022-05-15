#include "Vec.h"
#include "Color.h"
#include <memory>
#include <vector>
#include"RunOutOfStaminaEffect.h"

// �X�^�~�i�N���X
class Stamina {

private:
	/*===== �����o�ϐ� =====*/

	float nowGauge;			// ���݂̃Q�[�W�� 0 ~ 100 �̒l
	bool isActivate;		// �Q�[�W�ʂ��ő�l�ɒB���Ă��āA�U��񂵂�_�b�V�����s�����Ԃ��̃t���O
	Vec2<float> expAmount;	// �񕜂����ۂɂ�����Ɗg�傷�鉉�o�Ɏg�p����ϐ��B
	Color innerColor;		// �Q�[�W�̓����̐F
	Color outerColor;		// �Q�[�W�̊O���̐F


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
	void SetColor(const Color& InnerColor, const Color& OuterColor);

};

// �X�^�~�i�Ǘ��N���X
class StaminaMgr {

public:
	static const int STAMINA_NUM = 5;
	static int STAMINA_HEAL_SE[STAMINA_NUM];

	/*===== �����o�ϐ� =====*/

	std::vector<Stamina> stamina;
	RunOutOfStaminaEffect outOfStaminaEffect;
	bool emptyTrigger;

public:

	/*===== �����o�֐� =====*/

	StaminaMgr();
	void Init();
	void Update(const bool& Heal, const Vec2<float>& CharacterPos);
	void Draw(const Vec2<float>& CharaPos);
	
	// �F��ݒ�B
	void SetColor(const Color& InnerColor, const Color& OuterColor);

	// �X�^�~�i������
	void ConsumesStamina(const int& ConsumesStamina);

	// �X�^�~�i�����Z
	void AddStamina(const int& AddStamina);

	// �w�肵���A�N�V�������ł��邩�B
	bool CheckCanAction(const int& ConsumesStamina);

	// �o�[�P�ʂł͂Ȃ��Q�[�W����(�p�C���b�g���������p)�A����Ȃ�������false��Ԃ�
	void ConsumesStaminaByGauge(const float& CounsumeStaminaGauge);

	// �X�^�~�i�Q�[�W�̃T�C�Y��ݒ肷��B
	void Resize(const int& Size);

};