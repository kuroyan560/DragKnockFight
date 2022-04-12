#pragma once
#include"../Common/Singleton.h"
#include"../Common/Vec.h"
#include"Bullet.h"
#include<vector>
#include<array>
#include<memory>

using namespace std;

class BossBulletManager :public Singleton<BossBulletManager>
{
	int bulletGraph;

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
	BossBulletManager();

	// ��������
	void Generate(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �e�̓����蔻��
	//void CheckHit(const vector<vector<int>> &mapData, vector<Bubble> &bubble);

	const shared_ptr<Bullet> &GetBullet(const int &index) { return bullets[index]; };

};

