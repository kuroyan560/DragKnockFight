#pragma once
#include "Vec.h"
#include <vector>
using namespace std;

// �e�𓖂Ă邱�Ƃœ����u���b�N
class MovingBlock {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;					// ���W
	Vec2<float> prevFramePos;			// �O�t���[���̍��W
	Vec2<float> vel;					// �ړ���
	float gravity;				// �d��
	bool onGround;				// �ڒn�t���O
	bool isCloseToThePlayer;	// �v���C���[�ɋ߂����ǂ����̃t���O �y�ʉ��̂��߂Ɏg�p


public:

	/*===== �萔 =====*/

	const float SCALE = 25.0f;			// �T�C�Y
	const float ADD_GRAVITY = 0.5f;		// �d�͂ɉ��Z����ʁB
	const float MAX_GRAVITY = 15.0f;	// �d�͂̍ő�ʁB
	const float ADD_VEL = 2.0f;			// �ړ��ʂɉ��Z����ʁB
	const float MAX_VEL = 15.0f;		// �ړ��ʂ̍ő�l�B
	const float DO_PROCESS_DISTANCE = 1500;	// �������s�������B �y�ʉ��̂��߂Ɏg�p


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	MovingBlock(const Vec2<float>& generatePos);

	// ����������
	void Init();

	// �X�V����
	void Update(const Vec2<float>& playerPos);

	// �`�揈��
	void Draw(const int& graphHandle);

	// �n�ʂƓ����������̏���
	void HitGround();

	// �e�Ɠ����������̏���
	void HitBullet(const Vec2<float>& bulletForwardVec);

	// �}�b�v�`�b�v�Ƃ̓����蔻��
	void CheckHitMapChip(const vector<vector<int>>& mapData);

};