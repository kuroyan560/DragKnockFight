#pragma once
#include "Vec.h"

#include"WinApp.h"

// �e���Ŏ��̃p�[�e�B�N��
class BulletParticle {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;			// ���W
	Vec2<float> forwardVec;	// �ړ�����
	Vec2<float> movedVel;		// �ړ�������
	float alpha;		// �A���t�@�l
	float speed;		// �ړ����x
	bool isAlive;		// �����t���O


public:

	/*===== �萔 =====*/

	const float DEF_SPEED = 3.0f;
	const float SCALE = 6.0f;
	const float SUB_SPEED = 0.3f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	BulletParticle();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

private:

	// �؂�グ
	int RoundUp(int size, int align) {
		return UINT(size + align - 1) & ~(align - 1);
	}


};

/*

��������
�E�p�[�e�B�N���͎����̃T�C�Y���̊i�q��ɂ��������Ȃ��B
�EmovedVel�������̃T�C�Y���ɒB������pos�Ɉړ������ɃT�C�Y���̒l�𑫂��B
�Espeed�͂��񂾂�x���Ȃ�B
�E�����ꏊ�͌��\�����_���B
�E�A���t�@�u�����h��������B

*/