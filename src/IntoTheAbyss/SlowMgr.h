#include "Vec.h"
#include "Singleton.h"

// �X���[���Ǘ�����N���X
class SlowMgr : public Singleton<SlowMgr> {

public:

	/*===== �����o�ϐ� =====*/

	float slowAmount;		// �X���[�̒l 0~1�͈̔�


public:

	/*===== �萔 =====*/


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	SlowMgr() {
		slowAmount = 1.0f;
	}

	// ����������
	inline void Init() {
		slowAmount = 1.0f;
	}

};