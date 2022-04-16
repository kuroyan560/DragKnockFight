#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <memory>
#include <vector>
#include <array>

using namespace std;

// �e�N���X
class Bullet;

// �e�Ǘ��N���X
class BulletMgr : public Singleton<BulletMgr> {

public:

	/*-- �萔 --*/

	static const int BULLET_COUNT = 150;	// �e�̍ő吔
	//const int GENERATE_COUNT = 5;			// �ꔭ�Ő������鐔
	const int GENERATE_COUNT = 1;			// �ꔭ�Ő������鐔
	//const float SLIP_ONE_BULLET = 0.1f;		// �ꔭ�̊p�x�̃Y��
	const float SLIP_ONE_BULLET = 0.0f;		// �ꔭ�̊p�x�̃Y��


public:

	/*-- �����o�ϐ� --*/

	array<shared_ptr<Bullet>, BULLET_COUNT> bullets;	// �e�N���X


public:

	/*-- �����o�֐� --*/

	// �Z�b�e�B���O���� ��ԍŏ��Ɉ�񂾂��Ăяo��
	void Setting();

	// ��������
	void Generate(const int& graphHandle, const Vec2<float>& generatePos, const float& forwardAngle, const bool& isShotRight = false);

	//����������
	void Init();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �e�̓����蔻��
	void CheckHit(const vector<vector<int>>& mapData);

	const shared_ptr<Bullet>& GetBullet(const int& index) { return bullets[index]; };

};