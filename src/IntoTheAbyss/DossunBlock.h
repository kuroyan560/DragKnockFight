#pragma once
#include "Vec.h"
#include "Player.h"
#include <vector>

using namespace std;

// �h�b�X���N���X����ID
enum DossunID {

	DOSSUN_OFF_POWER,	// �d���I�t�̃h�b�X��
	DOSSUN_LOW_POWER,	// ��o�͂̃h�b�X��
	DOSSUN_HIGH_POWER,	// ���o�͂̃h�b�X��

};

// �h�b�X���N���X
class DossunBlock {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;			// ���W
	Vec2<float> size;			// �傫��
	Vec2<float> moveDir;		// �ړ�����
	float speed;		// �ړ����x
	int isMoveTimer;	// �ړ�����܂ł̃^�C�}�[(�v���C���[�����̃u���b�N�ɐG��Ă���w��t���[����ɓ����o���悤�ɂ��邽�߂ɕK�v�B)
	int changeDirTimer;	// �ǂƓ��������Ƃ��ɕ����]������^�C�}�[
	bool isHitPlayer;	// �v���C���[�Ɠ������Ă��邩�ǂ����B
	DossunID id;		// �ǂ̃h�b�X������ID


public:

	/*===== �萔 =====*/

	const int IS_MOVE_TIMER = 60;			// �v���C���[���u���b�N�ɐG��Ă��瓮���o���܂ł̃^�C�}�[
	const int CHANGE_DIR_TIMER = 180;		// �ǂɂ�������������]������^�C�}�[
	const float LOW_POWER_ADD_SPEED = 0.5f;	// ��o�̓h�b�X���̈ړ����x�̉��Z��
	const float LOW_POWER_MAX_SPEED = 5.0f;	// ��o�̓h�b�X���̈ړ����x�̍ő��


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	DossunBlock();

	// ����������
	void Init();

	// ��������
	void Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const DossunID& dossunID);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �����蔻��
	void CheckHit(Player& player, const vector<vector<int>>& mapData);

};