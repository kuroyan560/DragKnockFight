#include "Vec.h"
#include <array>

// SwingingDestroyCounter�Ŏg�p���鐔��1��
class DestroyCounter {

public:

	/*===== �����o�ϐ� =====*/

	enum class STATUS {

		NONE,
		ENTRY,		// �J�n���
		STAY,		// �ҋ@���
		EXIT,		// �ޏo���

	};

	enum class DISIT_ID {

		DISIT_NONE,
		DISIT1,		// 1�̈�
		DISIT10,	// 10�̈�
		DISIT100,	// 100�̈�

	};

	Vec2<float> pos;	// �`����W
	int disit;			// ����
	int exitReservationTimer;
	int exitTimer;		// STAY��Ԃ��玩���I�ɏ�����܂ł̃^�C�}�[
	int alpha;
	float easingTimer;	// �����ɃC�[�W���O�������邽�߂̃^�C�}�[
	bool isActive;		// �L��������Ă��邩�̃t���O
	bool isExitReservation;	// Exit�\��
	int isInitReservation;
	STATUS status;		// ���݂̃X�e�[�^�X
	DISIT_ID disitID;	// ����

	const float ADD_EASING_TIMER = 0.1f;
	const float EASING_CHANGE = 100.0f;
	const float FONT_SIZE = 128.0f;
	const int EXIT_TIMER = 120;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	DestroyCounter();

	// ����������
	void Init();

	// ���ꂳ����B
	void SetEntry(const int& Disit, const DISIT_ID DisitID);

	// �ޏo������B
	void SetExit();

	// �X�V����
	void Update(const Vec2<float>& CharaPos);

	// �`�揈��
	void Draw(std::array<int, 12> NumberHandle);

};

// 1��̃t���񂵂ŉ��u���b�N�󂵂�����\������B
class SwingDestroyCounter {

private:

	/*===== �����o�ϐ� =====*/

	static const int DESTROY_COUNTER = 600;
	std::array<DestroyCounter, DESTROY_COUNTER> destroyCounter;		// UI�Ɏg�p���鐔��1�B
	std::array<int, 12> numberHandle;
	std::array<int, 12> numberHandle_yellow;

	// ���Z����Ă����莞�Ԍo�߂�����S��Exit��Ԃɂ��邽�߂̃t���O
	int exitTimer;
	const int EXIT_TIMER = 180;

	int nowCounter;		// ���݂̔j�󐔁B����̒l������UI�̃N���X�̐�����ݒ肷��B


public:

	/*===== �����o�ϐ� =====*/

	// �R���X�g���N�^
	SwingDestroyCounter();

	// ������
	void Init();

	// �������Ă��邷�ׂẴJ�E���^�[��Exit��Ԃɂ���B
	void AllExit();

	// �X�V����
	void Update(const Vec2<float>& CharaPos);

	// �`�揈��
	void Draw();

	// �l�����Z�B
	void Increment(int INCRE_NUM = 1);



	bool IsZero() { return !nowCounter; }

private:

	// �w��̌�����Index���擾�B
	void CheckDisitStayID(const DestroyCounter::DISIT_ID DisitID);

};