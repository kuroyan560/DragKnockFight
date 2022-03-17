#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <memory>
#include <array>

using namespace std;

// �e�p�[�e�B�N���N���X
class BulletParticle;

// �e�p�[�e�B�N���Ǘ��N���X
class BulletParticleMgr : public Singleton<BulletParticleMgr> {

public:

	/*===== �萔 =====*/

	static const int MAX_PARTICLE = 300;						// �e�p�[�e�B�N���̍ő吔
	const int GENERATE_PARTICLE = 5;							// ��������p�[�e�B�N��


public:

	/*===== �����o�ϐ� =====*/

	array<shared_ptr<BulletParticle>, MAX_PARTICLE> particle;	// �e�p�[�e�B�N��


public:

	/*===== �����o�֐� =====*/

	// �Z�b�e�B���O����
	void Setting();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};