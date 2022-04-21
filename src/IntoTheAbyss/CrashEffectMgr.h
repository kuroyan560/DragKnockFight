#include "Vec.h"
#include "Singleton.h"
#include <array>

// �N���b�V�����̉��o�N���X
class ClassEffect {

private:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;	// ���W
	Vec2<float> size;	// �傫��
	int timer;			// �e�i�K�̕ω��A�C�[�W���O�Ɏg�p����^�C�}�[
	bool isExpY;		// Y���g���t���O ���̃N���X�̏����� �uY���g���i�K�v �uX���g���i�K�v ��2�ɕ�����Ă���B
	bool isActive;		// �L��������Ă��邩�t���O

	const int EXP_Y_TIMER = 5;	// Y���g���i�K�̃^�C�}�[
	const int EXP_X_TIMER = 10;	// X���g���i�K�̃^�C�}�[

	const float EXP_Y_AMOUNT = 20.0f;	// Y���g���i�K�ł�Y���̊g����
	const float EXP_X_AMOUNT = 400.0f;	// X���g���i�K�ł�X���̊g����


public:

	// �R���X�g���N�^
	ClassEffect() :pos({}), size({}), isExpY(false), isActive(false) {};

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	inline const bool& GetIsActive() { return isActive; }

};

// �N���b�V�����o�̊Ǘ��N���X
class CrashEffectMgr : public Singleton<CrashEffectMgr > {

private:

	/*===== �����o�ϐ� =====*/

	static const int CRASH_EFFECT_AMOUNT = 30;
	std::array<ClassEffect, CRASH_EFFECT_AMOUNT> crashEffects;


public:

	/*===== �����o�֐� =====*/

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& Pos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};