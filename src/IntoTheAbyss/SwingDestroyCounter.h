#include "Vec.h"
#include <array>

// SwingingDestroyCounter�Ŏg�p���鐔��1��
class DestroyCounter {

public:

	/*===== �����o�ϐ� =====*/

	enum class STATUS{
	
		ENTRY,		// �J�n���
		STAY,		// �ҋ@���
		EXIT,		// �ޏo���
	
	};

	Vec2<float> pos;	// �`����W
	int disit;			// ����
	float easingTimer;	// �����ɃC�[�W���O�������邽�߂̃^�C�}�[
	bool isActive;		// �L��������Ă��邩�̃t���O
	STATUS status;		// ���݂̃X�e�[�^�X

	const float ADD_EASING_TIMER = 0.05f;
	const float EASING_CHANGE = 100.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	DestroyCounter();

	// ����������
	void Init();

	// ���ꂳ����B
	void SetExtry(const int& Disit);

	// �ޏo������B
	void SetExit();

	// �X�V����
	void Update(const Vec2<float>& CharaPos);

	// �`�揈��
	void Draw();

};

// 1��̃t���񂵂ŉ��u���b�N�󂵂�����\������B
class SwingDestroyCounter {

private:

	/*===== �����o�ϐ� =====*/

	static const int DESTROY_COUNTER = 20;
	std::array<DestroyCounter, DESTROY_COUNTER> destroyCounter;		// UI�Ɏg�p���鐔��1�B

	int nowCounter;		// ���݂̔j�󐔁B����̒l������UI�̃N���X�̐�����ݒ肷��B


public:

	/*===== �����o�ϐ� =====*/

	// �R���X�g���N�^
	SwingDestroyCounter();

	// ������
	void Init();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �l�����Z�B
	void Increment();

};